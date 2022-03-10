/*
 * lcd_kit_adapt.c
 *
 * lcdkit adapt function for lcd driver
 *
 * Copyright (c) 2018-2019 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "lcd_kit_adapt.h"
#include "lcd_kit_common.h"
#include "lcd_kit_disp.h"
#include "lcd_kit_power.h"
#include <stdint.h>

#define MAX_DSI_CMD_LEN 256
#define MAX_BUF_LEN 20000
#define READ_MAX 100
#define BUF_MAX (4 * READ_MAX)

static void lcd_kit_dump_cmd(struct dsi_cmd_desc *cmd)
{
	unsigned int i;

	LCD_KIT_DEBUG("cmd->dtype = 0x%x\n", cmd->dtype);
	LCD_KIT_DEBUG("cmd->vc = 0x%x\n", cmd->vc);
	LCD_KIT_DEBUG("cmd->wait = 0x%x\n", cmd->wait);
	LCD_KIT_DEBUG("cmd->waittype = 0x%x\n", cmd->waittype);
	LCD_KIT_DEBUG("cmd->dlen = 0x%x\n", cmd->dlen);
	LCD_KIT_DEBUG("cmd->payload:\n");
	if (g_lcd_kit_msg_level >= MSG_LEVEL_DEBUG) {
		for (i = 0; i < cmd->dlen; i++)
			LCD_KIT_DEBUG("0x%x\n", cmd->payload[i]);
	}
}

static int lcd_cmds_to_dsi_cmds_ex(struct lcd_kit_dsi_cmd_desc *lcd_kit_cmds,
	struct dsi_cmd_desc *cmd, int link_state)
{
	if (lcd_kit_cmds == NULL) {
		LCD_KIT_ERR("lcd_kit_cmds is null point!\n");
		return LCD_KIT_FAIL;
	}
	if (cmd == NULL) {
		LCD_KIT_ERR("cmd is null point!\n");
		return LCD_KIT_FAIL;
	}
	cmd->dtype = lcd_kit_cmds->dtype;
	if (link_state == LCD_KIT_DSI_LP_MODE)
		cmd->dtype  |= GEN_VID_LP_CMD;
	cmd->vc = lcd_kit_cmds->vc;
	cmd->wait = lcd_kit_cmds->wait;
	cmd->waittype = lcd_kit_cmds->waittype;
	cmd->dlen = lcd_kit_cmds->dlen;
	cmd->payload = lcd_kit_cmds->payload;
	lcd_kit_dump_cmd(cmd);
	return LCD_KIT_OK;
}

static int lcd_kit_cmds_to_dsi_cmds(struct lcd_kit_dsi_cmd_desc *lcd_kit_cmds,
	struct dsi_cmd_desc *cmd, int link_state)
{
	if (lcd_kit_cmds == NULL) {
		LCD_KIT_ERR("lcd_kit_cmds is null point!\n");
		return LCD_KIT_FAIL;
	}
	if (cmd == NULL) {
		LCD_KIT_ERR("cmd is null point!\n");
		return LCD_KIT_FAIL;
	}
	cmd->dtype = lcd_kit_cmds->dtype;
	if (link_state == LCD_KIT_DSI_LP_MODE)
		cmd->dtype  |= GEN_VID_LP_CMD;

	cmd->vc = lcd_kit_cmds->vc;
	cmd->waittype = lcd_kit_cmds->waittype;
	cmd->dlen = lcd_kit_cmds->dlen;
	cmd->payload = lcd_kit_cmds->payload;
	lcd_kit_dump_cmd(cmd);
	return LCD_KIT_OK;
}

static bool lcd_kit_cmd_is_write(struct dsi_cmd_desc *cmd)
{
	bool is_write = true;

	switch (DSI_HDR_DTYPE(cmd->dtype)) {
	case DTYPE_GEN_WRITE:
	case DTYPE_GEN_WRITE1:
	case DTYPE_GEN_WRITE2:
	case DTYPE_GEN_LWRITE:
	case DTYPE_DCS_WRITE:
	case DTYPE_DCS_WRITE1:
	case DTYPE_DCS_LWRITE:
	case DTYPE_DSC_LWRITE:
		is_write = true;
		break;
	case DTYPE_GEN_READ:
	case DTYPE_GEN_READ1:
	case DTYPE_GEN_READ2:
	case DTYPE_DCS_READ:
		is_write = false;
		break;
	default:
		is_write = false;
		break;
	}
	return is_write;
}

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static int get_data_by_property_hilegacy(const char *np,
	const char *propertyname, int **data, u32 *len)
{
	void *fdt = NULL;
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	struct fdt_property *property = NULL;
	int offset;

	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("can't get fdt operators\n");
		return LCD_KIT_FAIL;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (dtimage_ops == NULL) {
		LCD_KIT_ERR("failed to get dtimage operators\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (fdt == NULL) {
		LCD_KIT_ERR("[%s]: fdt is null!!!\n", __func__);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	offset = fdt_ops->fdt_path_offset(fdt, np);
	if (offset < 0) {
		LCD_KIT_ERR("can not find %s node by np\n", np);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}

	property = (struct fdt_property *)fdt_ops->fdt_get_property(
		fdt, offset, propertyname, (int *)len);
	if (!property) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}

	*data = (int *)property->data;
	fdt_ops->fdt_operate_unlock();
	return LCD_KIT_OK;
}
#endif

extern void *lcd_fdt;
extern struct fdt_operators *lcd_fdt_ops;

static int get_data_by_property_hifastboot(const char *np,
	const char *propertyname, int **data, u32 *len)
{
	struct fdt_property *property = NULL;
	int offset;

	offset = lcd_fdt_ops->fdt_path_offset(lcd_fdt, np);
	if (offset < 0) {
		LCD_KIT_ERR("can not find %s node by np\n", np);
		return LCD_KIT_FAIL;
	}
	property = (struct fdt_property *)lcd_fdt_ops->fdt_get_property(
		lcd_fdt, offset, propertyname, (int *)len);
	if (!property) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}
	*data = (int *)property->data;
	return LCD_KIT_OK;
}

static int lcd_kit_get_data_by_property(const char *np,
	const char *propertyname, int **data, u32 *len)
{
	int ret;
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	ret = get_data_by_property_hilegacy(np, propertyname, data, len);
#else
	ret = get_data_by_property_hifastboot(np, propertyname, data, len);
#endif
	return ret;
}

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static int get_string_by_property_hilegacy(const char *np,
	const char *propertyname, char *out_str, unsigned int length)
{
	int offset;
	int ret;
	int len = 0;
	void *fdt = NULL;
	struct fdt_property *property = NULL;
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;

	if ((!np) || (!propertyname) || (!out_str)) {
		LCD_KIT_ERR("null pointer,compatible:%s, propertyname:%s, out_str:%s\n",
			np, propertyname, out_str);
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("can't get fdt operators\n");
		return LCD_KIT_FAIL;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (dtimage_ops == NULL) {
		LCD_KIT_ERR("failed to get dtimage operators\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (fdt == NULL) {
		LCD_KIT_ERR("[%s]: fdt is null!!!\n", __func__);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	offset = fdt_ops->fdt_path_offset(fdt, np);
	if (offset < 0) {
		LCD_KIT_ERR("can not find %s node by np\n", np);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	property = (struct fdt_property *)fdt_ops->fdt_get_property(
		fdt, offset, propertyname, &len);
	if (!property) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	if (!property->data) {
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	length = (length > (strlen((char *)property->data) + 1)) ?
		((strlen((char *)property->data)) + 1) : length;
	ret = memcpy_s(out_str, (unsigned int)length, (char *)property->data,
		(unsigned int)length);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_unlock();
	return LCD_KIT_OK;
}
#endif

static int get_string_by_property_hifastboot(const char *np,
	const char *propertyname, char *out_str, unsigned int length)
{
	int offset;
	int ret;
	int len = 0;
	struct fdt_property *property = NULL;

	if ((!np) || (!propertyname) || (!out_str)) {
		LCD_KIT_ERR("null pointer,compatible:%s, propertyname:%s, out_str:%s\n",
			np, propertyname, out_str);
		return LCD_KIT_FAIL;
	}
	offset = lcd_fdt_ops->fdt_path_offset(lcd_fdt, np);
	if (offset < 0) {
		LCD_KIT_ERR("can not find %s node by np\n", np);
		return LCD_KIT_FAIL;
	}
	property = (struct fdt_property *)lcd_fdt_ops->fdt_get_property(
		lcd_fdt, offset, propertyname, &len);
	if (!property) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}
	length = (length > (unsigned int)(strlen((char *)property->data) + 1)) ?
		(unsigned int)((strlen((char *)property->data)) + 1) : length;
	ret = memcpy_s(out_str, length, (char *)property->data, length);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

static int lcd_kit_get_string_by_property(const char *np,
	const char *propertyname, char *out_str, unsigned int length)
{
	int ret;
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	ret = get_string_by_property_hilegacy(np, propertyname, out_str, length);
#else
	ret = get_string_by_property_hifastboot(np, propertyname, out_str, length);
#endif
	return ret;
}
uint32_t lcd_kit_get_dsi_base_addr(struct hisi_fb_data_type *hisifd)
{
	struct hisi_panel_info *pinfo = hisifd->panel_info;

	if (pinfo == NULL)
		return hisifd->mipi_dsi0_base;
	if (pinfo->mipi.dsi_te_type == DSI1_TE1_TYPE)
		return hisifd->mipi_dsi1_base;

	return hisifd->mipi_dsi0_base;
}
int lcd_kit_dsi_diff_cmds_tx(void *hld,
	struct lcd_kit_dsi_panel_cmds *dsi0_cmds,
	struct lcd_kit_dsi_panel_cmds *dsi1_cmds)
{
	int ret = LCD_KIT_OK;
	int i;
	struct hisi_fb_data_type *hisifd = NULL;
	struct dsi_cmd_desc *dsi0_cmd = NULL;
	struct dsi_cmd_desc *dsi1_cmd = NULL;

	if ((dsi0_cmds == NULL) || (dsi1_cmds == NULL)) {
		LCD_KIT_ERR("cmd cnt is 0!\n");
		return LCD_KIT_FAIL;
	}
	if (((dsi0_cmds->cmds == NULL) || (dsi0_cmds->cmd_cnt <= 0))
		|| ((dsi1_cmds->cmds == NULL) || (dsi1_cmds->cmd_cnt <= 0))) {
		LCD_KIT_ERR("cmds is null, or cmds->cmd_cnt <= 0!\n");
		return LCD_KIT_FAIL;
	}
	if ((dsi0_cmds->cmd_cnt > MAX_DSI_CMD_LEN) ||
		(dsi1_cmds->cmd_cnt > MAX_DSI_CMD_LEN)) {
		LCD_KIT_ERR("cmds->cmd_cnt >= MAX_DSI_CMD_LEN!\n");
		return LCD_KIT_FAIL;
	}
	hisifd = (struct hisi_fb_data_type *)hld;
	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null!\n");
		return LCD_KIT_FAIL;
	}
	dsi0_cmd = alloc(sizeof(struct dsi_cmd_desc) * dsi0_cmds->cmd_cnt);
	if (!dsi0_cmd) {
		LCD_KIT_ERR("dsi0_cmd is null!\n");
		return LCD_KIT_FAIL;
	}

	for (i = 0; i < dsi0_cmds->cmd_cnt; i++)
		lcd_cmds_to_dsi_cmds_ex(&dsi0_cmds->cmds[i],
			(dsi0_cmd + i), dsi0_cmds->link_state);
	dsi1_cmd = alloc(sizeof(struct dsi_cmd_desc) * dsi1_cmds->cmd_cnt);
	if (!dsi1_cmd) {
		LCD_KIT_ERR("dsi1_cmd is null!\n");
		free(dsi0_cmd);
		return LCD_KIT_FAIL;
	}
	for (i = 0; i < dsi1_cmds->cmd_cnt; i++)
		lcd_cmds_to_dsi_cmds_ex(&dsi1_cmds->cmds[i],
			(dsi1_cmd + i), dsi1_cmds->link_state);
	(void)mipi_dual_dsi_cmds_tx(dsi0_cmd, dsi0_cmds->cmd_cnt,
			hisifd->mipi_dsi0_base, dsi1_cmd, dsi1_cmds->cmd_cnt,
			hisifd->mipi_dsi1_base, EN_DSI_TX_NORMAL_MODE);
	free(dsi0_cmd);
	free(dsi1_cmd);
	return ret;
}
/*
 *  dsi send cmds
 */
int lcd_kit_dsi_cmds_tx(void *hld, struct lcd_kit_dsi_panel_cmds *cmds)
{
	int ret;
	int i;
	uint32_t dsi_base_addr;
	struct hisi_fb_data_type *hisifd = NULL;
	struct dsi_cmd_desc dsi_cmd;

	if (cmds == NULL) {
		LCD_KIT_ERR("cmd cnt is 0!\n");
		return LCD_KIT_FAIL;
	}
	ret = memset_s(&dsi_cmd, sizeof(dsi_cmd), 0, sizeof(dsi_cmd));
	if (ret != 0) {
		LCD_KIT_ERR("memset_s err\n");
		return LCD_KIT_FAIL;
	}
	if ((cmds->cmds == NULL) || (cmds->cmd_cnt <= 0)) {
		LCD_KIT_ERR("cmds is null, or cmds->cmd_cnt <= 0!\n");
		return LCD_KIT_FAIL;
	}
	hisifd = (struct hisi_fb_data_type *) hld;
	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null!\n");
		return LCD_KIT_FAIL;
	}
	/* switch to LP mode */
	if (cmds->link_state == LCD_KIT_DSI_LP_MODE)
		lcd_kit_set_mipi_link(hisifd, LCD_KIT_DSI_LP_MODE);

	dsi_base_addr = lcd_kit_get_dsi_base_addr(hisifd);
	for (i = 0; i < cmds->cmd_cnt; i++) {
		lcd_kit_cmds_to_dsi_cmds(&cmds->cmds[i], &dsi_cmd,
			cmds->link_state);
		if (!lcd_kit_dsi_fifo_is_full(dsi_base_addr))
			mipi_dsi_cmds_tx(&dsi_cmd, 1, dsi_base_addr);
		if (is_dual_mipi_panel(hisifd)) {
			if (!lcd_kit_dsi_fifo_is_full(hisifd->mipi_dsi1_base))
				mipi_dsi_cmds_tx(&dsi_cmd, 1,
					hisifd->mipi_dsi1_base);
		}
		lcd_kit_delay(cmds->cmds[i].wait, cmds->cmds[i].waittype);
	}
	/* switch to HS mode */
	if (cmds->link_state == LCD_KIT_DSI_LP_MODE)
		lcd_kit_set_mipi_link(hisifd, LCD_KIT_DSI_HS_MODE);
	return ret;
}

static int lcd_get_read_value(struct dsi_cmd_desc *dsi_cmd,
	uint8_t *dest, uint32_t *src, uint32_t len)
{
	unsigned int dlen;
	unsigned int cnt = 0;
	unsigned int start_index = 0;
	int div = sizeof(uint32_t) / sizeof(uint8_t);

	if (dsi_cmd->dlen > 1)
		start_index = (unsigned int)dsi_cmd->payload[1];
	for (dlen = 0; dlen < dsi_cmd->dlen; dlen++) {
		if ((dlen + 1) < start_index)
			continue;
		if (cnt >= len) {
			LCD_KIT_ERR("data len error\n");
			return LCD_KIT_FAIL;
		}
		/*
		 * 0 means remainder is 0 1 means remainder is 1
		 * 2 means remainder is 2 3 means remainder is 3
		 * 24 means 24bits 16 means 16bits 8 means 8bits
		 */
		switch (dlen % div) {
		case 0:
			dest[cnt] = (uint8_t)(src[dlen / div] & 0xFF);
			break;
		case 1:
			dest[cnt] = (uint8_t)((src[dlen / div] >> 8) & 0xFF);
			break;
		case 2:
			dest[cnt] = (uint8_t)((src[dlen / div] >> 16) & 0xFF);
			break;
		case 3:
			dest[cnt] = (uint8_t)((src[dlen / div] >> 24) & 0xFF);
			break;
		default:
			break;
		}
		cnt++;
	}
	return LCD_KIT_OK;
}

static int lcd_kit_cmd_rx(struct hisi_fb_data_type *hisifd, uint8_t *out,
	int out_len, struct dsi_cmd_desc *cmd, uint32_t dsi_base)
{
	int ret = LCD_KIT_OK;
	uint32_t tmp[READ_MAX] = {0};

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		ret = LCD_KIT_FAIL;
		return ret;
	}
	if (lcd_kit_dsi_fifo_is_full(dsi_base)) {
		LCD_KIT_ERR("mipi read error\n");
		ret = LCD_KIT_FAIL;
		return ret;
	}
	if (lcd_kit_cmd_is_write(cmd)) {
		(void)mipi_dsi_cmds_tx(cmd, 1, dsi_base);
	} else {
		ret = mipi_dsi_lread(tmp, cmd, cmd->dlen,
			(char *)(uintptr_t)dsi_base);
		if (ret) {
			LCD_KIT_ERR("mipi read error\n");
			return ret;
		}
		ret = lcd_get_read_value(cmd, out, tmp, out_len);
		if (ret) {
			LCD_KIT_ERR("get read value error\n");
			return ret;
		}
	}

	return ret;
}

int lcd_kit_dsi_cmds_rx(void *hld, uint8_t *out, int out_len,
	struct lcd_kit_dsi_panel_cmds *cmds)
{
	int i, link_state, ret;
	unsigned int j;
	int cnt = 0;
	uint32_t dsi_base_addr;
	struct hisi_fb_data_type *hisifd = NULL;
	struct dsi_cmd_desc dsi_cmd;
	uint8_t tmp[BUF_MAX] = {0};

	hisifd = (struct hisi_fb_data_type *)hld;
	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null!\n");
		return LCD_KIT_FAIL;
	}
	if (!cmds || !cmds->cmds || cmds->cmd_cnt <= 0 || !out) {
		LCD_KIT_ERR("cmd is null, or cmd_cnt <= 0, or out is null!\n");
		return LCD_KIT_FAIL;
	}
	if (out_len > BUF_MAX) {
		LCD_KIT_ERR("out_len > BUF_MAX\n");
		return LCD_KIT_FAIL;
	}
	ret = memset_s(&dsi_cmd, sizeof(dsi_cmd), 0, sizeof(dsi_cmd));
	if (ret != 0) {
		LCD_KIT_ERR("memset_s err\n");
		return LCD_KIT_FAIL;
	}
	link_state = cmds->link_state;
	if (link_state == LCD_KIT_DSI_LP_MODE)
		lcd_kit_set_mipi_link(hisifd, LCD_KIT_DSI_LP_MODE);
	dsi_base_addr = lcd_kit_get_dsi_base_addr(hisifd);
	for (i = 0; i < cmds->cmd_cnt; i++) {
		ret = memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
		if (ret != 0) {
			LCD_KIT_ERR("memset_s err\n");
			return LCD_KIT_FAIL;
		}
		lcd_kit_cmds_to_dsi_cmds(&cmds->cmds[i], &dsi_cmd, link_state);
		ret = lcd_kit_cmd_rx(hisifd, tmp, BUF_MAX, &dsi_cmd,
			dsi_base_addr);
		if (ret)
			LCD_KIT_ERR("mipi rx error\n");
		lcd_kit_delay(cmds->cmds[i].wait, cmds->cmds[i].waittype);
		if (lcd_kit_cmd_is_write(&dsi_cmd)) {
			continue;
		}
		for (j = 0; j < dsi_cmd.dlen; j++) {
			if (cnt < out_len)
				out[cnt++] = tmp[j];
		}
	}
	if (link_state == LCD_KIT_DSI_LP_MODE)
		lcd_kit_set_mipi_link(hisifd, LCD_KIT_DSI_HS_MODE);
	return ret;
}

static int lcd_kit_buf_trans(const char *inbuf, int inlen, char **outbuf,
	int *outlen)
{
	char *buf = NULL;
	int i;
	int bufsize = inlen;

	if (!inbuf) {
		LCD_KIT_INFO("inbuf is null point!\n");
		return LCD_KIT_FAIL;
	}
	if ((bufsize <= 0) || (bufsize > MAX_BUF_LEN)) {
		LCD_KIT_ERR("bufsize <= 0 or > MAX_BUF_LEN!\n");
		return LCD_KIT_FAIL;
	}
	/* The property is 4 bytes long per element in cells: <> */
	bufsize = bufsize / 4;
	/* If use bype property: [], this division should be removed */
	buf = alloc(sizeof(char) * bufsize);
	if (!buf) {
		LCD_KIT_ERR("buf is null point!\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means 4 bytes 3 means the third element */
	for (i = 0; i < bufsize; i++)
		buf[i] = inbuf[i * 4 + 3];
	*outbuf = buf;
	*outlen = bufsize;
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_enable(u32 type)
{

	lcd_kit_gpio_tx(type, GPIO_REQ);
	lcd_kit_gpio_tx(type, GPIO_HIGH);
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_disable(u32 type)
{
	lcd_kit_gpio_tx(type, GPIO_LOW);
	lcd_kit_gpio_tx(type, GPIO_RELEASE);
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_enable_nolock(u32 type)
{
	lcd_kit_gpio_tx(type, GPIO_REQ);
	lcd_kit_gpio_tx(type, GPIO_HIGH);
	lcd_kit_gpio_tx(type, GPIO_RELEASE);
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_disable_nolock(u32 type)
{
	lcd_kit_gpio_tx(type, GPIO_REQ);
	lcd_kit_gpio_tx(type, GPIO_LOW);
	lcd_kit_gpio_tx(type, GPIO_RELEASE);
	return LCD_KIT_OK;
}

static int lcd_kit_regulator_enable(u32 type)
{
	int ret = LCD_KIT_OK;

	switch (type) {
	case LCD_KIT_VCI:
	case LCD_KIT_IOVCC:
	case LCD_KIT_VDD:
		ret = lcd_kit_pmu_ctrl(type, ENABLE);
		break;
	case LCD_KIT_VSP:
	case LCD_KIT_VSN:
	case LCD_KIT_BL:
		ret = lcd_kit_charger_ctrl(type, ENABLE);
		break;
	default:
		ret = LCD_KIT_FAIL;
		LCD_KIT_ERR("regulator type:%d not support\n", type);
		break;
	}
	return ret;
}

static int lcd_kit_regulator_disable(u32 type)
{
	int ret = LCD_KIT_OK;

	switch (type) {
	case LCD_KIT_VCI:
	case LCD_KIT_IOVCC:
	case LCD_KIT_VDD:
		ret = lcd_kit_pmu_ctrl(type, DISABLE);
		break;
	case LCD_KIT_VSP:
	case LCD_KIT_VSN:
	case LCD_KIT_BL:
		ret = lcd_kit_charger_ctrl(type, DISABLE);
		break;
	default:
		LCD_KIT_ERR("regulator type:%d not support\n", type);
		break;
	}
	return ret;
}

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static int change_dts_status_by_compatible_hilegacy(const char *name)
{
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	void *fdt = NULL;
	int ret = 0;
	int offset;

	if (!name) {
		LCD_KIT_ERR("name is null!\n");
		return ret;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return ret;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (!dtimage_ops) {
		LCD_KIT_ERR("failed to get dtimage operators!\n");
		return ret;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		fdt_ops->fdt_operate_unlock();
		return ret;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		fdt_ops->fdt_operate_unlock();
		return ret;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, name);
	if (ret < 0) {
		LCD_KIT_ERR("can not find panel node, change fb dts failed\n");
		fdt_ops->fdt_operate_unlock();
		return ret;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char *)"status",
		(const void *)"ok");
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
	fdt_ops->fdt_operate_unlock();
	return ret;
}
#endif

static int change_dts_status_by_compatible_hifastboot(const char *name)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return LCD_KIT_OK;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret = 0;
	int offset;

	if (!name) {
		LCD_KIT_ERR("name is null!\n");
		return ret;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return ret;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return ret;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return ret;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, name);
	if (ret < 0) {
		LCD_KIT_ERR("can not find panel node, change fb dts failed\n");
		return ret;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char *)"status",
		(const void *)"ok");
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
	return ret;
#endif
}

int lcd_kit_change_dts_status_by_compatible(const char *name)
{
	int ret;
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	ret = change_dts_status_by_compatible_hilegacy(name);
#else
	ret = change_dts_status_by_compatible_hifastboot(name);
#endif
	return ret;
}

static int lcd_kit_fwdtb_change_bl_dts_status(void)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return LCD_KIT_OK;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret = 0;
	int offset;

	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return ret;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return ret;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return ret;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, disp_info->bl_name);
	if (ret < 0) {
		LCD_KIT_ERR("can not find panel node, change fb dts failed\n");
		return ret;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char *)"status",
		(const void *)"ok");
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
	return ret;
#endif
}

static int lcd_kit_fwdtb_change_bl_dts(const char *name)
{
	struct fdt_operators *fdt_ops = NULL;
	unsigned int length = strlen(name) + 1;
	int ret;

	disp_info->bl_name = (char *)alloc_uncache(length);
	ret = memcpy_s(disp_info->bl_name, length, name, length);
	if (ret != LCD_KIT_OK) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("fdt_ops is null\n");
		return LCD_KIT_FAIL;
	}
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	fdt_ops->register_update_dts_func(lcd_kit_fwdtb_change_bl_dts_status);
#endif
	return LCD_KIT_OK;
}

static int lcd_kit_fwdtb_change_bias_dts_status(void)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return LCD_KIT_OK;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret = 0;
	int offset;

	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return ret;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return ret;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return ret;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0,
		disp_info->bias_name);
	if (ret < 0) {
		LCD_KIT_ERR("can not find panel node, change fb dts failed\n");
		return ret;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, (const char *)"status",
		(const void *)"ok");
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
	return ret;
#endif
}

static int lcd_kit_fwdtb_change_bias_dts(const char *name)
{
	struct fdt_operators *fdt_ops = NULL;
	int ret;

	int length = strlen(name) + 1;
	disp_info->bias_name = (char *)alloc_uncache(length);
	ret = memcpy_s(disp_info->bias_name, length, name, length);
	if (ret != LCD_KIT_OK) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("fdt_ops is null\n");
		return LCD_KIT_FAIL;
	}
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	fdt_ops->register_update_dts_func(lcd_kit_fwdtb_change_bias_dts_status);
#endif
	return LCD_KIT_OK;
}
static int lcd_kit_set_data_by_property(const char *np,
	const char *property_name, u32 value)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if (!np || !property_name) {
		LCD_KIT_ERR("pointer is null\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return LCD_KIT_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(UPDATE_DTS_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_path_offset(fdt, np);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find panel node, change fb dts failed\n");
		return LCD_KIT_FAIL;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_u32(fdt, offset, property_name, value);
	if (ret) {
		LCD_KIT_ERR("add dts fail %s\n", property_name);
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
#endif
}
static int lcd_kit_set_array_data_by_property(const char *np,
	const char *property_name, const char *value, u32 len)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if (!np || !property_name) {
		LCD_KIT_ERR("pointer is null\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return LCD_KIT_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(UPDATE_DTS_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_path_offset(fdt, np);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find panel node, change fb dts failed\n");
		return LCD_KIT_FAIL;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_operators(fdt, offset, property_name,
		(const char *)value, len);
	if (ret) {
		LCD_KIT_ERR("add dts fail %s\n", property_name);
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
#endif
}
struct lcd_kit_adapt_ops adapt_ops = {
	.mipi_tx = lcd_kit_dsi_cmds_tx,
	.mipi_rx = lcd_kit_dsi_cmds_rx,
	.gpio_enable = lcd_kit_gpio_enable,
	.gpio_disable = lcd_kit_gpio_disable,
	.gpio_enable_nolock = lcd_kit_gpio_enable_nolock,
	.gpio_disable_nolock = lcd_kit_gpio_disable_nolock,
	.regulator_enable = lcd_kit_regulator_enable,
	.regulator_disable = lcd_kit_regulator_disable,
	.buf_trans = lcd_kit_buf_trans,
	.get_data_by_property = lcd_kit_get_data_by_property,
	.get_string_by_property = lcd_kit_get_string_by_property,
	.change_dts_status_by_compatible =
		lcd_kit_change_dts_status_by_compatible,
	.fwdtb_change_bl_dts = lcd_kit_fwdtb_change_bl_dts,
	.fwdtb_change_bias_dts = lcd_kit_fwdtb_change_bias_dts,
	.set_data_by_property = lcd_kit_set_data_by_property,
	.set_array_data_by_property = lcd_kit_set_array_data_by_property,
};
int lcd_kit_adapt_init(void)
{
	int ret;

	ret = lcd_kit_adapt_register(&adapt_ops);
	return ret;
}
