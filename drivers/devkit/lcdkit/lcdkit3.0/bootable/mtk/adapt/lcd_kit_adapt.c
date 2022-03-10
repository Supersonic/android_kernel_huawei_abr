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
#include "lcd_kit_disp.h"
#include "lcd_kit_power.h"
#include "lcd_kit_common.h"
#include "lcm_drv.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <lk_builtin_dtb.h>

#define MAX_BUF_SIZE 20000
#define POWER_BUF_SIZE 3
#define READ_MAX 100
#define BUF_MAX (4 * READ_MAX)

static uint32_t lcd_id_cur_status[LCD_TYPE_NAME_MAX] = {0};
char lcd_type_buf[LCD_TYPE_NAME_MAX];

static struct lcd_type_info lcm_info_list[] = {
	{ LCDKIT, "LCDKIT" },
	{ LCD_KIT, "LCD_KIT" },
};

extern LCM_UTIL_FUNCS lcm_util_mtk;

#define mipi_dsi_cmds_tx(cmdq, cmds) lcm_util_mtk.mipi_dsi_cmds_tx(cmdq, cmds)

#define mipi_dsi_cmds_rx(out, cmds, len) lcm_util_mtk.mipi_dsi_cmds_rx(out, cmds, len)

static int lcd_kit_get_data_by_property(const char *compatible,
	const char *propertyname, int **data, u32 *len)
{
	int offset;
	const struct fdt_property *property = NULL;
	void *pfdt = NULL;

	if ((!compatible) || (!propertyname) || (!data) || (!len)) {
		LCD_KIT_ERR("domain_name, item_name or value is NULL!\n");
		return LCD_KIT_FAIL;
	}

	pfdt = get_lk_overlayed_dtb();
	if (!pfdt) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}

	offset = fdt_node_offset_by_compatible(pfdt, -1, compatible);
	if (offset < 0) {
		LCD_KIT_INFO("can not find %s node by compatible\n",
			compatible);
		return LCD_KIT_FAIL;
	}

	property = fdt_get_property((const void *)pfdt, offset, propertyname, (int *)len);
	if (!property) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}

	if (!property->data)
		return LCD_KIT_FAIL;
	*data = (int *)property->data;
	return LCD_KIT_OK;
}

static int lcd_kit_get_dts_data_by_property(const void *pfdt, int offset,
		const char *propertyname, int **data, unsigned int *len)
{
	const struct fdt_property *property = NULL;

	if ((pfdt == NULL) || (propertyname == NULL) ||
		(data == NULL) || (len == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}

	if (offset < 0) {
		LCD_KIT_INFO("can not find %s node\n", propertyname);
		return LCD_KIT_FAIL;
	}

	property = fdt_get_property(pfdt, offset, propertyname, (int *)len);
	if (property == NULL) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}

	if (property->data == NULL)
		return LCD_KIT_FAIL;
	*data = (int *)property->data;
	return LCD_KIT_OK;
}

static int lcd_kit_get_dts_string_by_property(const void *pfdt, int offset,
	const char *propertyname, char *out_str, unsigned int length)
{
	const struct fdt_property *property = NULL;
	int len = 0;

	if ((pfdt == NULL) || (propertyname == NULL) ||
		(out_str == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}

	if (offset < 0) {
		LCD_KIT_INFO("can not find %s node\n", propertyname);
		return LCD_KIT_FAIL;
	}

	property = fdt_get_property(pfdt, offset, propertyname, &len);
	if (property == NULL) {
		LCD_KIT_INFO("can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}
	if (property->data == NULL)
		return LCD_KIT_FAIL;
	length = (length >= (strlen((char *)property->data) + 1)) ?
		((strlen((char *)property->data)) + 1) : (length - 1);
	memcpy(out_str, (char *)property->data, length);
	return LCD_KIT_OK;
}

static int get_dts_property(const char *compatible, const char *propertyname,
	const uint32_t **data, int *length)
{
	int offset;
	int len;
	const struct fdt_property *property = NULL;
	void *pfdt = NULL;

	if (!compatible || !propertyname || !data || !length) {
		LCD_KIT_ERR("domain_name, item_name or value is NULL!\n");
		return LCD_KIT_FAIL;
	}

	pfdt = get_lk_overlayed_dtb();
	if (!pfdt) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}

	offset = fdt_node_offset_by_compatible(pfdt, -1, compatible);
	if (offset < 0) {
		LCD_KIT_ERR("-----can not find %s node by compatible\n",
			compatible);
		return LCD_KIT_FAIL;
	}

	property = fdt_get_property((const void *)pfdt, offset, propertyname, &len);
	if (!property) {
		LCD_KIT_ERR("-----can not find %s\n", propertyname);
		return LCD_KIT_FAIL;
	}

	*data = (const uint32_t *)property->data;
	*length = len;

	return LCD_KIT_OK;
}

int lcd_kit_get_detect_type(void)
{
	u32 type = 0;
	int ret;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LCD_PANEL_TYPE,
		"detect_type", &type, 0);
	if (ret < 0)
		return LCD_KIT_FAIL;

	LCD_KIT_INFO("LCD panel detect type = %u\n", type);
	ret = (int)type;

	return ret;
}

void lcd_kit_get_lcdname(void)
{
	int offset;
	int len;
	const struct fdt_property *property = NULL;
	void *pfdt = NULL;

	pfdt = get_lk_overlayed_dtb();
	if (!pfdt) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return;
	}

	offset = fdt_node_offset_by_compatible(pfdt, -1,
		DTS_COMP_LCD_PANEL_TYPE);
	if (offset < 0) {
		LCD_KIT_ERR("-----can not find %s node by compatible\n",
			DTS_COMP_LCD_PANEL_TYPE);
		return;
	}

	property = fdt_get_property((const void *)pfdt, offset, "support_lcd_type", &len);
	if (!property) {
		LCD_KIT_ERR("-----can not find support_lcd_type\n");
		return;
	}

	memset(lcd_type_buf, 0, LCD_TYPE_NAME_MAX);
	memcpy(lcd_type_buf, (char *)property->data,
		(unsigned int)strlen((char *)property->data) + 1);
}

int lcd_kit_get_lcd_type(void)
{
	int i;
	int length = sizeof(lcm_info_list) / sizeof(struct lcd_type_info);

	for (i = 0; i < length; i++) {
		if (strncmp(lcd_type_buf, (char *)lcm_info_list[i].lcd_name,
			strlen((char *)(lcm_info_list[i].lcd_name))) == 0)
			return lcm_info_list[i].lcd_type;
	}
	return UNKNOWN_LCD;
}
void lcd_kit_set_lcd_name_to_no_lcd(void)
{
	int size = strlen("NO_LCD") + 1;

	memcpy(lcd_type_buf, "NO_LCD", size);
}

static int get_dts_u32_index(const char *compatible, const char *propertyname,
	uint32_t index, uint32_t *out_val)
{
	int ret;
	int len;
	const uint32_t *data = NULL;
	uint32_t ret_tmp;

	if ((!compatible) || (!propertyname) || (!out_val)) {
		LCD_KIT_ERR("domain_name, item_name or value is NULL!\n");
		return LCD_KIT_FAIL;
	}

	ret = get_dts_property(compatible, propertyname, &data, &len);
	if ((ret < 0) || (len < 0))
		return ret;

	if ((index * sizeof(uint32_t)) >= (uint32_t)len) {
		LCD_KIT_ERR("out of range!\n");
		return LCD_KIT_FAIL;
	}

	ret_tmp = *(data + index);
	ret_tmp = fdt32_to_cpu(ret_tmp);
	*out_val = ret_tmp;

	return LCD_KIT_OK;
}

static int parse_iovcc_dts(uint32_t *iovcc_ctrl_mode, uint32_t *ldo_gpio)
{
	int ret;
	u32 *buf = NULL;
	int i;

	/* get dts index 0 */
	ret = get_dts_u32_index(DTS_COMP_LCD_PANEL_TYPE, "iovcc_lcd",
		0, iovcc_ctrl_mode);
	if (ret < 0) {
		LCD_KIT_INFO("get iovcc_lcd mode failed!\n");
		return ret;
	}

	if (*iovcc_ctrl_mode == IOVCC_CTRL_GPIO) {
		/* get dts index 1 */
		ret = get_dts_u32_index(DTS_COMP_LCD_PANEL_TYPE,
			"iovcc_lcd", 1, ldo_gpio);
		if (ret < 0)
			LCD_KIT_INFO("get iovcc_lcd gpio failed!\n");
		return ret;
	}

	if (*iovcc_ctrl_mode == IOVCC_CTRL_REGULATOR) {
		buf = (u32 *)malloc(POWER_BUF_SIZE * sizeof(u32));
		if (!buf) {
			LCD_KIT_ERR("alloc buf fail\n");
			return LCD_KIT_FAIL;
		}
		for (i = 0; i < POWER_BUF_SIZE;i++) {
			/* get dts index 0~2 */
			ret = get_dts_u32_index(DTS_COMP_LCD_PANEL_TYPE,
				"iovcc_lcd", i, buf + i);
			if (ret < 0)
				LCD_KIT_ERR("get iovcc_lcd gpio failed!\n");
		}
		power_hdl->lcd_iovcc.buf = buf;
		LCD_KIT_INFO("Power Type = %d Power Num = %d,Power Vol = %d\n",
			power_hdl->lcd_iovcc.buf[POWER_MODE],
			power_hdl->lcd_iovcc.buf[POWER_NUMBER],
			power_hdl->lcd_iovcc.buf[POWER_VOLTAGE]);
	}

	return ret;
}

static void iovcc_power_ctrl(bool power_on,
	uint32_t iovcc_ctrl_mode, uint32_t ldo_gpio)
{
	if (power_on) {
		if (iovcc_ctrl_mode == IOVCC_CTRL_REGULATOR) {
			LCD_KIT_INFO("Power on iovcc ctrl by regulator, mode=%d!\n",
				iovcc_ctrl_mode);
			lcd_kit_pmu_ctrl(LCD_KIT_IOVCC, ENABLE);
		} else if (iovcc_ctrl_mode == IOVCC_CTRL_GPIO) {
			LCD_KIT_INFO("Power on iovcc ctrl by gpio, gpio=%d!\n", ldo_gpio);
			mt_set_gpio_mode(ldo_gpio, GPIO_MODE_00);
			mt_set_gpio_dir(ldo_gpio, GPIO_DIR_OUT);
			mt_set_gpio_out(ldo_gpio, GPIO_OUT_ONE);
		}
	} else {
		if (iovcc_ctrl_mode == IOVCC_CTRL_REGULATOR) {
			LCD_KIT_INFO("Power off iovcc ctrl by regulator, mode=%d!\n",
				iovcc_ctrl_mode);
			lcd_kit_pmu_ctrl(LCD_KIT_IOVCC, DISABLE);
		} else if (iovcc_ctrl_mode == IOVCC_CTRL_GPIO) {
			LCD_KIT_INFO("Power off iovcc ctrl by gpio, gpio=%d!\n", ldo_gpio);
			mt_set_gpio_out(ldo_gpio, GPIO_OUT_ZERO);
		}
	}
}

uint32_t lcdkit_get_lcd_id(void)
{
	uint32_t lcdid_count;
	int ret;
	uint32_t i;
	uint32_t gpio_id = 0;
	uint32_t lcd_id_status = 0;
	uint32_t lcd_id_up;
	uint32_t lcd_id_down;
	const int lcd_nc_value = 2;
	uint32_t *dts_data_p = NULL;
	int dts_data_len = 0;
	uint32_t iovcc_ctrl_mode = 0;
	uint32_t ldo_gpio = 0;

	ret = get_dts_property(DTS_COMP_LCD_PANEL_TYPE, "gpio_id",
		(const uint32_t **)&dts_data_p, &dts_data_len);
	if (ret < 0) {
		LCD_KIT_ERR("get id failed or excess max supported id pins!\n");
		return LCD_KIT_FAIL;
	}

	/* 4 means u32 has 4 bits */
	lcdid_count = dts_data_len / 4;

	ret = parse_iovcc_dts(&iovcc_ctrl_mode, &ldo_gpio);
	if (ret < 0) {
		iovcc_ctrl_mode = IOVCC_CTRL_NONE;
		LCD_KIT_INFO("initial iovcc_ctrl_mode to none\n");
	}

	iovcc_power_ctrl(true, iovcc_ctrl_mode, ldo_gpio);

	for (i = 0; i < lcdid_count; i++) {
		ret = get_dts_u32_index(DTS_COMP_LCD_PANEL_TYPE,
			"gpio_id", i, &gpio_id);
		mt_set_gpio_mode(gpio_id, GPIO_MODE_00);
		mt_set_gpio_dir(gpio_id, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(gpio_id, GPIO_PULL_ENABLE);
		mt_set_gpio_pull_select(gpio_id, GPIO_PULL_UP);
		mdelay(10);
		lcd_id_up = mt_get_gpio_in(gpio_id);

		mt_set_gpio_pull_select(gpio_id, GPIO_PULL_DOWN);
		mdelay(10);
		lcd_id_down = mt_get_gpio_in(gpio_id);
		mt_set_gpio_pull_select(gpio_id, GPIO_PULL_DISABLE);
		if (lcd_id_up == lcd_id_down)
			lcd_id_cur_status[i] = lcd_id_up;
		else
			lcd_id_cur_status[i] = lcd_nc_value;
		/* 2 means 2 mul i bits */
		lcd_id_status |= (lcd_id_cur_status[i] << (2 * i));
	}

	iovcc_power_ctrl(false, iovcc_ctrl_mode, ldo_gpio);

	LCD_KIT_INFO("[uboot]:%s ,lcd_id_status=%d.\n",
		__func__, lcd_id_status);
	return lcd_id_status;
}

int lcd_kit_get_product_id(void)
{
	int ret;
	u32 product_id = 0;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LCD_PANEL_TYPE,
		"product_id", &product_id, 0);
	if (ret < 0)
		return LCD_KIT_FAIL;

	ret = (int)product_id;

	return ret;
}

void lcdkit_set_lcd_panel_type(char *type)
{
	int ret;
	int offset;
	void *kernel_fdt = NULL;

	if (!type) {
		LCD_KIT_ERR("type is NULL!\n");
		return;
	}

#ifdef DEVICE_TREE_SUPPORT
	kernel_fdt = get_kernel_fdt();
#endif
	if (!kernel_fdt) {
		LCD_KIT_ERR("kernel_fdt is NULL!\n");
		return;
	}

	offset = fdt_path_offset(kernel_fdt, DTS_LCD_PANEL_TYPE);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find panel node, change dts failed\n");
		return;
	}

	ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"lcd_panel_type",
		(const void *)type);
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
}

void lcdkit_set_lcd_ddic_max_brightness(unsigned int bl_val)
{
	int ret;
	int offset;
	void *kernel_fdt = NULL;

#ifdef DEVICE_TREE_SUPPORT
	kernel_fdt = get_kernel_fdt();
#endif
	if (!kernel_fdt) {
		LCD_KIT_ERR("kernel_fdt is NULL!\n");
		return;
	}

	offset = fdt_path_offset(kernel_fdt, DTS_LCD_PANEL_TYPE);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find huawei,lcd_panel node\n");
		return;
	}

	ret = fdt_setprop_cell(kernel_fdt, offset,
		(const char *)"panel_ddic_max_brightness", bl_val);
	if (ret)
		LCD_KIT_ERR("Cannot update lcd max brightness(errno=%d)!\n",
			ret);
}

void lcdkit_set_lcd_panel_status(char *lcd_name)
{
	int ret;
	int offset;
	void *kernel_fdt = NULL;

	if (!lcd_name) {
		LCD_KIT_ERR("type is NULL!\n");
		return;
	}

#ifdef DEVICE_TREE_SUPPORT
	kernel_fdt = get_kernel_fdt();
#endif
	if (!kernel_fdt) {
		LCD_KIT_ERR("kernel_fdt is NULL!\n");
		return;
	}

	offset = fdt_path_offset(kernel_fdt, lcd_name);
	if (offset < 0) {
		LCD_KIT_ERR("can not find panel node, change fb dts failed\n");
		return;
	}

	ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
		(const void *)"ok");
	if (ret) {
		LCD_KIT_ERR("Cannot update lcd panel type(errno=%d)!\n", ret);
		return;
	}

	LCD_KIT_INFO("lcdkit_set_lcd_panel_status OK!\n");
}

static int lcd_kit_cmds_to_mtk_dsi_cmds(
	struct lcd_kit_dsi_cmd_desc *lcd_kit_cmds,
	struct dsi_cmd_desc *cmd)
{
	if (lcd_kit_cmds == NULL) {
		LCD_KIT_ERR("lcd_kit_cmds is null point!\n");
		return LCD_KIT_FAIL;
	}
	if (cmd == NULL) {
		LCD_KIT_ERR("cmd is null point!\n");
		return LCD_KIT_FAIL;
	}

	cmd->dtype = lcd_kit_cmds->payload[0];
	cmd->vc = lcd_kit_cmds->vc;
	cmd->dlen = (lcd_kit_cmds->dlen - 1);
	cmd->payload = &lcd_kit_cmds->payload[1];
	cmd->link_state = MIPI_MODE_LP;

	return LCD_KIT_OK;
}

static int lcd_kit_cmds_to_mtk_dsi_read_cmds(
	struct lcd_kit_dsi_cmd_desc *lcd_kit_cmds,
	struct dsi_cmd_desc *cmd)
{
	if (lcd_kit_cmds == NULL) {
		LCD_KIT_ERR("lcd_kit_cmds is null point!\n");
		return LCD_KIT_FAIL;
	}
	if (cmd == NULL) {
		LCD_KIT_ERR("cmd is null point!\n");
		return LCD_KIT_FAIL;
	}

	cmd->dtype = lcd_kit_cmds->payload[0];
	cmd->vc = lcd_kit_cmds->vc;
	cmd->dlen = lcd_kit_cmds->dlen;
	cmd->link_state = MIPI_MODE_LP;

	return LCD_KIT_OK;
}

static int mtk_mipi_dsi_cmds_tx(struct lcd_kit_dsi_cmd_desc *cmds, int cnt)
{
	struct lcd_kit_dsi_cmd_desc *cm = cmds;
	struct dsi_cmd_desc dsi_cmd;
	int i;

	if (cmds == NULL) {
		LCD_KIT_ERR("cmds is NULL");
		return LCD_KIT_FAIL;
	}

	for (i = 0; i < cnt; i++) {
		lcd_kit_cmds_to_mtk_dsi_cmds(cm, &dsi_cmd);
		(void)mipi_dsi_cmds_tx(NULL, &dsi_cmd);

		if (cm->wait) {
			if (cm->waittype == WAIT_TYPE_US)
				udelay(cm->wait);
			else if (cm->waittype == WAIT_TYPE_MS)
				mdelay(cm->wait);
			else
				/* 1000 means second */
				mdelay(cm->wait * 1000);
		}
		cm++;
	}
	return cnt;
}
static int lcd_kit_cmd_is_write(struct lcd_kit_dsi_cmd_desc *cmd)
{
	int ret = LCD_KIT_FAIL;

	if (!cmd) {
		LCD_KIT_ERR("cmd is NULL!\n");
		return LCD_KIT_FAIL;
	}

	switch (cmd->dtype) {
	case DTYPE_GEN_WRITE:
	case DTYPE_GEN_WRITE1:
	case DTYPE_GEN_WRITE2:
	case DTYPE_GEN_LWRITE:
	case DTYPE_DCS_WRITE:
	case DTYPE_DCS_WRITE1:
	case DTYPE_DCS_LWRITE:
	case DTYPE_DSC_LWRITE:
		ret = LCD_KIT_OK;
		break;
	case DTYPE_GEN_READ:
	case DTYPE_GEN_READ1:
	case DTYPE_GEN_READ2:
	case DTYPE_DCS_READ:
		ret = LCD_KIT_FAIL;
		break;
	default:
		ret = LCD_KIT_FAIL;
		break;
	}
	return ret;
}

/*
 *  dsi send cmds
 */
int lcd_kit_dsi_cmds_tx(void *hld, struct lcd_kit_dsi_panel_cmds *cmds)
{
	int i;

	if (!cmds) {
		LCD_KIT_ERR("cmd is NULL!\n");
		return LCD_KIT_FAIL;
	}

	for (i = 0; i < cmds->cmd_cnt; i++)
		mtk_mipi_dsi_cmds_tx(&cmds->cmds[i], 1);

	return LCD_KIT_OK;
}

/*
 *  dsi receive cmds
 */
int lcd_kit_dsi_cmds_rx(void *hld, u8 *out, int out_len,
	struct lcd_kit_dsi_panel_cmds *cmds)
{
	int i;
	int ret;
	struct dsi_cmd_desc dsi_cmd;

	if (!cmds || !out) {
		LCD_KIT_ERR("cmds or out is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if (out_len > BUF_MAX) {
		LCD_KIT_ERR("out_len > BUF_MAX\n");
		return LCD_KIT_FAIL;
	}

	for (i = 0; i < cmds->cmd_cnt; i++) {
		if (!lcd_kit_cmd_is_write(&cmds->cmds[i])) {
			lcd_kit_cmds_to_mtk_dsi_cmds(&cmds->cmds[i], &dsi_cmd);
			ret = mtk_mipi_dsi_cmds_tx(&cmds->cmds[i], 1);
			if (ret < 0) {
				LCD_KIT_ERR("mipi cmd tx failed!\n");
				return LCD_KIT_FAIL;
			}
		} else {
			lcd_kit_cmds_to_mtk_dsi_read_cmds(&cmds->cmds[i],
				&dsi_cmd);
			ret = mipi_dsi_cmds_rx((char *)out, &dsi_cmd, dsi_cmd.dlen);
			if (ret == 0) {
				LCD_KIT_ERR("mipi cmd rx failed\n");
				return LCD_KIT_FAIL;
			}
		}
	}
	return LCD_KIT_OK;
}

static int lcd_kit_buf_trans(const char *inbuf, int inlen, char **outbuf,
	int *outlen)
{
	char *buf = NULL;
	int i;
	int bufsize = inlen;

	if (!inbuf || !outbuf || !outlen) {
		LCD_KIT_ERR("inbuf is null!\n");
		return LCD_KIT_FAIL;
	}
	if ((bufsize <= 0) || (bufsize > MAX_BUF_SIZE)) {
		LCD_KIT_ERR("bufsize <= 0 or > MAX_BUF_SIZE!\n");
		return LCD_KIT_FAIL;
	}
	/* The property is 4bytes long per element in cells: <> */
	bufsize = bufsize / 4;
	/* If use bype property: [], this division should be removed */
	buf = (char *)malloc(sizeof(char) * bufsize);
	if (!buf) {
		LCD_KIT_ERR("buf is null\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means 4bytes 3 means the third element */
	for (i = 0; i < bufsize; i++)
		buf[i] = inbuf[i * 4 + 3];

	*outbuf = buf;
	*outlen = bufsize;
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_disable(u32 type)
{
	lcd_kit_gpio_tx(type, GPIO_LOW);
	lcd_kit_gpio_tx(type, GPIO_RELEASE);
	return LCD_KIT_OK;
}

static int lcd_kit_gpio_enable(u32 type)
{
	lcd_kit_gpio_tx(type, GPIO_REQ);
	lcd_kit_gpio_tx(type, GPIO_HIGH);
	return LCD_KIT_OK;
}

static int lcd_kit_regulator_disable(u32 type)
{
	int ret;

	switch (type) {
	case LCD_KIT_VCI:
	case LCD_KIT_IOVCC:
		ret = lcd_kit_pmu_ctrl(type, DISABLE);
		break;
	case LCD_KIT_VSP:
	case LCD_KIT_VSN:
	case LCD_KIT_BL:
		ret = lcd_kit_charger_ctrl(type, DISABLE);
		break;
	default:
		ret = LCD_KIT_FAIL;
		LCD_KIT_ERR("regulator type:%d not support!\n", type);
		break;
	}
	return ret;
}

static int lcd_kit_regulator_enable(u32 type)
{
	int ret;

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
		LCD_KIT_ERR("regulator type:%d not support!\n", type);
		break;
	}
	return ret;
}

struct lcd_kit_adapt_ops adapt_ops = {
	.mipi_tx = lcd_kit_dsi_cmds_tx,
	.mipi_rx = lcd_kit_dsi_cmds_rx,
	.gpio_enable = lcd_kit_gpio_enable,
	.gpio_disable = lcd_kit_gpio_disable,
	.regulator_enable = lcd_kit_regulator_enable,
	.regulator_disable = lcd_kit_regulator_disable,
	.buf_trans = lcd_kit_buf_trans,
	.get_data_by_property = lcd_kit_get_data_by_property,
	.get_dts_data_by_property = lcd_kit_get_dts_data_by_property,
	.get_dts_string_by_property = lcd_kit_get_dts_string_by_property,
};

int lcd_kit_adapt_init(void)
{
	int ret;

	ret = lcd_kit_adapt_register(&adapt_ops);
	return ret;
}
