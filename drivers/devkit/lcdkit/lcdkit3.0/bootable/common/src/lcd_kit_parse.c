/*
 * lcd_kit_parse.c
 *
 * lcdkit parse function for lcd driver
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

#include "lcd_kit_common.h"

#ifdef MTK_LCD_KIT_SUPPORT
#include <libfdt.h>
#define mem_alloc malloc
#else
#define mem_alloc alloc
#endif
#include "lcd_kit_utils.h"

#define LINK_STATE_LEN 20
#define MAX_BLEN_LEN 80000
#define MAX_DLEN_LEN 1000
#define MAX_CNT 600

int conver2le(u32 *src, u32 *desc, u32 len)
{
	unsigned int i = 0;
	u32 temp = 0;

	if ((src == NULL) || (desc == NULL)) {
		LCD_KIT_ERR("src or desc is null\n");
		return LCD_KIT_FAIL;
	}
	/* 24 means 24bits 16 means 16bits 8 means 8bits */
	for (i = 0; i < len; i++) {
		temp = ((src[i] & 0xff) << 24);
		temp |= (((src[i] >> 8) & 0xff) << 16);
		temp |= (((src[i] >> 16) & 0xff) << 8);
		temp |= ((src[i] >> 24) & 0xff);
		desc[i] = temp;
		temp = 0;
	}
	return LCD_KIT_OK;
}

static int lcd_scan_dcs_cmd(char *buf, int *len,
	struct lcd_kit_dsi_panel_cmds *pcmds)
{
	int i, j;
	int cnt = 0;
	char *bp = NULL;
	struct lcd_kit_dsi_cmd_desc_header *dchdr = NULL;

	bp = buf;
	while (*len >= (int)sizeof(struct lcd_kit_dsi_cmd_desc_header)) {
		dchdr = (struct lcd_kit_dsi_cmd_desc_header *)bp;
		bp += sizeof(struct lcd_kit_dsi_cmd_desc_header);
		*len -= (int)sizeof(struct lcd_kit_dsi_cmd_desc_header);
		if (dchdr->dlen > *len) {
			LCD_KIT_ERR("cmd = 0x%x parse error, len = %d\n",
				dchdr->dtype, dchdr->dlen);
			return LCD_KIT_FAIL;
		}

		bp += dchdr->dlen;
		*len -= dchdr->dlen;
		cnt++;
	}
	if (*len != 0) {
		LCD_KIT_ERR("dcs_cmd parse error! cmd len = %d\n", *len);
		return LCD_KIT_FAIL;
	}
	if ((cnt <= 0) || (cnt > MAX_CNT)) {
		LCD_KIT_ERR("cnt <= 0 or > MAX_CNT\n");
		return LCD_KIT_FAIL;
	}
	pcmds->cmds = (struct lcd_kit_dsi_cmd_desc *)mem_alloc(cnt * sizeof(struct lcd_kit_dsi_cmd_desc));
	if (!pcmds->cmds)
		return LCD_KIT_FAIL;
	pcmds->cmd_cnt = cnt;
	bp = buf;
	for (i = 0; i < cnt; i++) {
		dchdr = (struct lcd_kit_dsi_cmd_desc_header *)bp;
		*len -= (int)sizeof(struct lcd_kit_dsi_cmd_desc_header);
		bp += sizeof(struct lcd_kit_dsi_cmd_desc_header);
		pcmds->cmds[i].dtype = dchdr->dtype;
		pcmds->cmds[i].last = dchdr->last;
		pcmds->cmds[i].vc = dchdr->vc;
		pcmds->cmds[i].ack = dchdr->ack;
		pcmds->cmds[i].wait = dchdr->wait;
		pcmds->cmds[i].waittype = dchdr->waittype;
		pcmds->cmds[i].dlen = dchdr->dlen;
		if (dchdr->dlen <= 0) {
			LCD_KIT_ERR("cnt <= 0\n");
			return LCD_KIT_FAIL;
		}
		pcmds->cmds[i].payload = (char *)mem_alloc(dchdr->dlen * sizeof(char));
		for (j = 0; j < dchdr->dlen; j++)
			pcmds->cmds[i].payload[j] = bp[j];
		bp += dchdr->dlen;
		*len -= dchdr->dlen;
	}
	return LCD_KIT_OK;
}

int lcd_kit_parse_dcs_cmds(const char *np, const char *propertyname,
	const char *link_key, struct lcd_kit_dsi_panel_cmds *pcmds)
{
	int len = 0;
	int ret;
	u32 blen = 0;
	char *buf = NULL;
	int *data = NULL;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	char link_key_str[LINK_STATE_LEN + 1] = {0};

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property)
		adpat_ops->get_data_by_property(np, propertyname,
			&data, &blen);
	/* scan dcs commands */
	if (adpat_ops->buf_trans)
		adpat_ops->buf_trans((char *)data, blen, &buf, &len);
	ret = lcd_scan_dcs_cmd(buf, &len, pcmds);
	if (ret < 0) {
		free(buf);
		return LCD_KIT_OK;
	}
	/* Set default link state to HS Mode */
	pcmds->link_state = LCD_KIT_DSI_HS_MODE;
	if (link_key) {
		if (adpat_ops->get_string_by_property)
			adpat_ops->get_string_by_property(np, link_key,
				link_key_str, sizeof(link_key_str));
		if (!strncmp(link_key_str, "dsi_lp_mode",
			strlen("dsi_lp_mode")))
			pcmds->link_state = LCD_KIT_DSI_LP_MODE;
		else
			pcmds->link_state = LCD_KIT_DSI_HS_MODE;
	}
	/* free buf */
	free(buf);
	return LCD_KIT_OK;
}

int lcd_kit_parse_get_u64_default(const char *np,
	const char *propertyname, uint64_t *value, uint64_t def_val)
{
	u32 blen = 0;
	int *data = NULL;
	uint64_t *tmp = NULL;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;

	if (!np || !propertyname || !value) {
		LCD_KIT_ERR("pointer is null\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property)
		adpat_ops->get_data_by_property(np,
			propertyname, &data, &blen);
	if (!data) {
		LCD_KIT_INFO("data is null\n");
		*value = def_val;
		return LCD_KIT_FAIL;
	}
	tmp = (uint64_t *)data;
	*value = (uint64_t)fdt64_to_cpu(*tmp);
	return LCD_KIT_OK;
}

int lcd_kit_parse_get_u32_default(const char *np,
	const char *propertyname, u32 *value, u32 def_val)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	int *data = NULL;
	u32 blen = 0;
	int ret_tmp;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property)
		adpat_ops->get_data_by_property(np,
			propertyname, &data, &blen);
	if (!data) {
		LCD_KIT_INFO("data is null\n");
		*value = def_val;
		return LCD_KIT_FAIL;
	}
	ret_tmp = *data;
	ret_tmp = (int)fdt32_to_cpu((uint32_t)ret_tmp);
	*value = ret_tmp;
	return LCD_KIT_OK;
}

int lcd_kit_parse_get_u8_default(const char *np,
	const char *propertyname, u8 *value, u32 def_val)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	int *data = NULL;
	u32 blen = 0;
	int ret_tmp;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property)
		adpat_ops->get_data_by_property(np, propertyname,
			&data, &blen);
	if (!data) {
		LCD_KIT_ERR("data is null\n");
		*value = def_val;
		return LCD_KIT_FAIL;
	}
	ret_tmp = *data;
	ret_tmp = (int)fdt32_to_cpu((uint32_t)ret_tmp);
	*value = ret_tmp;
	return LCD_KIT_OK;
}

int lcd_kit_parse_array(const char *np, const char *propertyname,
	struct lcd_kit_array_data *out)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	u32 blen = 0;
	int *data = NULL;
	u32 *buf = NULL;
	int ret = 0;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property) {
		ret = adpat_ops->get_data_by_property(np, propertyname,
			&data, &blen);
		if (ret < 0)
			return LCD_KIT_FAIL;
	}
	if ((blen <= 0) || (blen > MAX_BLEN_LEN)) {
		LCD_KIT_ERR("blen <= 0 or > MAX_BLEN_LEN!\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means u32 has 4 bytes */
	blen = blen / 4;
	buf = (u32 *)mem_alloc(blen * sizeof(u32));
	if (!buf) {
		LCD_KIT_ERR("alloc buf fail\n");
		return LCD_KIT_FAIL;
	}
	memcpy((void *)buf, (const void *)data, blen * sizeof(u32));
	conver2le(buf, buf, blen);
	out->buf = buf;
	out->cnt = blen;
	return LCD_KIT_OK;

}

int lcd_kit_parse_u32_array(const char *np, const char *propertyname,
	u32 out[], unsigned int len)
{
	int ret;
	u32 blen = 0;
	int *data = NULL;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;

	if (!np || !propertyname || !out) {
		LCD_KIT_ERR("pointer is null\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property) {
		ret = adpat_ops->get_data_by_property(np, propertyname,
			&data, &blen);
		if (ret < 0)
			return LCD_KIT_FAIL;
	}
	if ((blen <= 0) || (blen > MAX_BLEN_LEN)) {
		LCD_KIT_ERR("blen <= 0 or > MAX_BLEN_LEN!\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means u32 has 4 bytes */
	blen = blen / 4;
	ret = memcpy_s((void *)out, len * sizeof(u32),
		(const void *)data, blen * sizeof(u32));
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return LCD_KIT_FAIL;
	}
	(void)conver2le(out, out, len);
	return LCD_KIT_OK;
}

int lcd_kit_parse_arrays(const char *np, const char *propertyname,
	struct lcd_kit_arrays_data *out, u32 num)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	int i;
	int cnt = 0;
	u32 blen = 0;
	u32 length;
	int *data = NULL;
	int *bp = NULL;
	int *buf = NULL;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_data_by_property)
		adpat_ops->get_data_by_property(np, propertyname,
			&data, &blen);
	if ((blen <= 0) || (blen > MAX_BLEN_LEN)) {
		LCD_KIT_ERR("blen <= 0 or > MAX_BLEN_LEN!\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means u32 has 4 bytes */
	blen = blen / 4;
	length = blen;
	buf = (int *)mem_alloc(blen * sizeof(u32));
	if (!buf) {
		LCD_KIT_ERR("alloc buf fail\n");
		return LCD_KIT_FAIL;
	}
	if (!data) {
		LCD_KIT_ERR("data is null\n");
		return LCD_KIT_FAIL;
	}
	memcpy((void *)buf, (const void *)data, blen * sizeof(u32));
	conver2le((unsigned int *)buf, (unsigned int *)buf, blen);
	while (length >= num) {
		if (num > length) {
			LCD_KIT_ERR("data length = %x error\n", length);
			return LCD_KIT_FAIL;
		}
		length -= num;
		cnt++;
	}
	if (length != 0) {
		LCD_KIT_ERR("dts data parse error! data len = %d\n", length);
		return LCD_KIT_FAIL;
	}
	out->cnt = cnt;
	if ((cnt <= 0) || (cnt > MAX_CNT)) {
		LCD_KIT_ERR("cnt <= 0 or > MAX_CNT\n");
		return LCD_KIT_FAIL;
	}
	out->arry_data = (struct lcd_kit_array_data *)mem_alloc(cnt *
		sizeof(struct lcd_kit_array_data));
	if (!out->arry_data) {
		LCD_KIT_ERR("out->arry_data is null\n");
		return LCD_KIT_FAIL;
	}
	bp = buf;
	for (i = 0; i < cnt; i++) {
		out->arry_data[i].buf = (uint32_t *)bp;
		out->arry_data[i].cnt = num;
		bp += num;
	}
	return LCD_KIT_OK;
}

int lcd_kit_get_dts_u32_default(const void *fdt, int nodeoffset,
	const char *propertyname, u32 *value, u32 def_val)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	int *data = NULL;
	u32 blen = 0;
	int ret_tmp;

	if ((fdt == NULL) || (propertyname == NULL) ||
		(value == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		*value = def_val;
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_dts_data_by_property)
		adpat_ops->get_dts_data_by_property(fdt, nodeoffset,
			propertyname, &data, &blen);
	if (data == NULL) {
		LCD_KIT_INFO("data is null\n");
		*value = def_val;
		return LCD_KIT_FAIL;
	}
	ret_tmp = *data;
	ret_tmp = (int)fdt32_to_cpu((uint32_t)ret_tmp);
	*value = ret_tmp;
	return LCD_KIT_OK;
}

int lcd_kit_parse_dts_array(const void *fdt, int nodeoffset,
	const char *propertyname, struct lcd_kit_array_data *out)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	u32 blen = 0;
	int *data = NULL;
	u32 *buf = NULL;
	int ret;

	if ((fdt == NULL) || (propertyname == NULL) ||
		(out == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if (out == NULL) {
		LCD_KIT_ERR("out parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_dts_data_by_property) {
		ret = adpat_ops->get_dts_data_by_property(fdt, nodeoffset,
			propertyname, &data, &blen);
		if (ret < 0)
			return LCD_KIT_FAIL;
	}
	if ((blen <= 0) || (blen > MAX_BLEN_LEN)) {
		LCD_KIT_ERR("blen <= 0 or > MAX_BLEN_LEN!\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means u32 has 4 bytes */
	blen = blen / 4;
	buf = (u32 *)mem_alloc(blen * sizeof(u32));
	if (buf == NULL) {
		LCD_KIT_ERR("alloc buf fail\n");
		return LCD_KIT_FAIL;
	}
	memcpy((void *)buf, (const void *)data, blen * sizeof(u32));
	conver2le(buf, buf, blen);
	out->buf = buf;
	out->cnt = blen;
	return LCD_KIT_OK;
}

int lcd_kit_parse_dts_arrays(const void *fdt, int nodeoffset,
	const char *propertyname, struct lcd_kit_arrays_data *out, u32 num)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	int i;
	int cnt = 0;
	u32 blen = 0;
	u32 length;
	int *data = NULL;
	int *bp = NULL;
	int *buf = NULL;

	if ((fdt == NULL) || (propertyname == NULL) ||
		(out == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (adpat_ops == NULL) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_dts_data_by_property)
		adpat_ops->get_dts_data_by_property(fdt, nodeoffset,
			propertyname, &data, &blen);
	if ((blen <= 0) || (blen > MAX_BLEN_LEN)) {
		LCD_KIT_ERR("blen <= 0 or > MAX_BLEN_LEN!\n");
		return LCD_KIT_FAIL;
	}
	if (data == NULL) {
		LCD_KIT_ERR("data is null\n");
		return LCD_KIT_FAIL;
	}
	/* 4 means u32 has 4 bytes */
	blen = blen / 4;
	length = blen;
	buf = (int *)mem_alloc(blen * sizeof(u32));
	if (buf == NULL) {
		LCD_KIT_ERR("alloc buf fail\n");
		return LCD_KIT_FAIL;
	}
	memcpy((void *)buf, (const void *)data, blen * sizeof(u32));
	conver2le((unsigned int *)buf, (unsigned int *)buf, blen);
	while (length >= num) {
		if (num > length) {
			LCD_KIT_ERR("data length = %x error\n", length);
			free(buf);
			return LCD_KIT_FAIL;
		}
		length -= num;
		cnt++;
	}
	if (length != 0) {
		LCD_KIT_ERR("dts data parse error! data len = %d\n", length);
		free(buf);
		return LCD_KIT_FAIL;
	}
	out->cnt = cnt;
	if ((cnt <= 0) || (cnt > MAX_CNT)) {
		LCD_KIT_ERR("cnt <= 0 or > MAX_CNT\n");
		free(buf);
		return LCD_KIT_FAIL;
	}
	out->arry_data = (struct lcd_kit_array_data *)mem_alloc(cnt *
		sizeof(struct lcd_kit_array_data));
	if (out->arry_data == NULL) {
		LCD_KIT_ERR("out->arry_data is null\n");
		free(buf);
		return LCD_KIT_FAIL;
	}
	bp = buf;
	for (i = 0; i < cnt; i++) {
		out->arry_data[i].buf = (uint32_t *)bp;
		out->arry_data[i].cnt = num;
		bp += num;
	}
	return LCD_KIT_OK;
}

int lcd_kit_parse_dts_dcs_cmds(const void *fdt, int nodeoffset,
	const char *propertyname, const char *link_key,
	struct lcd_kit_dsi_panel_cmds *pcmds)
{
	int len = 0;
	int ret;
	u32 blen = 0;
	char *buf = NULL;
	int *data = NULL;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;
	char link_key_str[LINK_STATE_LEN + 1] = {0};

	if ((fdt == NULL) || (propertyname == NULL) ||
		(link_key == NULL) || (pcmds == NULL)) {
		LCD_KIT_ERR("input parameter is NULL!\n");
		return LCD_KIT_FAIL;
	}
	adpat_ops = lcd_kit_get_adapt_ops();
	if (adpat_ops == NULL) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->get_dts_data_by_property)
		adpat_ops->get_dts_data_by_property(fdt, nodeoffset,
			propertyname, &data, &blen);
	/* scan dcs commands */
	if (adpat_ops->buf_trans)
		adpat_ops->buf_trans((char *)data, blen, &buf, &len);
	ret = lcd_scan_dcs_cmd(buf, &len, pcmds);
	if (ret < 0) {
		free(buf);
		return LCD_KIT_OK;
	}
	/* Set default link state to HS Mode */
	pcmds->link_state = LCD_KIT_DSI_HS_MODE;
	if (link_key) {
		if (adpat_ops->get_dts_string_by_property)
			adpat_ops->get_dts_string_by_property(fdt, nodeoffset,
				link_key, link_key_str, sizeof(link_key_str));
		if (!strncmp(link_key_str, "dsi_lp_mode",
			strlen("dsi_lp_mode")))
			pcmds->link_state = LCD_KIT_DSI_LP_MODE;
		else
			pcmds->link_state = LCD_KIT_DSI_HS_MODE;
	}
	/* free buf */
	free(buf);
	return LCD_KIT_OK;
}
int lcd_kit_set_dts_data(const char *np, const char *property_name,
	u32 value)
{
	int ret;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->set_data_by_property) {
		ret = adpat_ops->set_data_by_property(np, property_name, value);
		if (ret < 0)
			return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}
int lcd_kit_set_dts_array_data(const char *np, const char *property_name,
	const char *value, u32 len)
{
	int ret;
	struct lcd_kit_adapt_ops *adpat_ops = NULL;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return LCD_KIT_FAIL;
	}
	if (adpat_ops->set_array_data_by_property) {
		ret = adpat_ops->set_array_data_by_property(np, property_name,
			value, len);
		if (ret < 0)
			return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}
