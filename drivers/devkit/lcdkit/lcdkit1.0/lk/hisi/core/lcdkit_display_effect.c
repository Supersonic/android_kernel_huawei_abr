/*
 * lcdkit_display_effect.c
 *
 * lcdkit disp effect function for lcd driver
 *
 * Copyright (c) 2018-2020 Huawei Technologies Co., Ltd.
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

#include "lcdkit_display_effect.h"
#include "lcdkit_disp.h"

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN970)
static struct lcdbrightnesscoloroeminfo lcd_color_oeminfo = {0};
static struct panelid lcd_panelid = {0};
#define LCD_DDIC_INFO_LEN 64
extern uint32_t g_panel_vender_id;

int get_lcd_bright_rgbw_id_from_oeminfo(void)
{
	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops)
	{
		HISI_FB_ERR("get_operators error \n");
		return -1;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_LCD_BRIGHT_COLOR_CALIBRATION, sizeof(struct lcdbrightnesscoloroeminfo), (char*)&lcd_color_oeminfo))
	{
		HISI_FB_ERR("get bright color oeminfo  error \n");
		return -1;
	}

	return 0;
}

int set_lcd_bright_rgbw_id_to_oeminfo(void)
{
	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops)
	{
		HISI_FB_ERR("get_operators error \n");
		return -1;
	}

	if (oeminfo_ops->set_oeminfo(OEMINFO_LCD_BRIGHT_COLOR_CALIBRATION, sizeof(struct lcdbrightnesscoloroeminfo), (char*)&lcd_color_oeminfo))
	{
		HISI_FB_ERR("set bright color oeminfo error \n");
		return -1;
	}

	return 0;
}

static int lcdkit_panel_id_cmds_rx(uint8_t *out, struct lcdkit_dsi_panel_cmd *cmds, struct hisi_fb_data_type *hisifd, int max_out_size)
{
	int i;
	int ret;
	int dlen;
	int cnt = 0;
	struct dsi_cmd_desc *cm;
	uint32_t tmp_value[LCD_DDIC_INFO_LEN] = {0};
	int read_start_index;
	uint32_t mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;

	cm = cmds->cmds_set;
	HISI_FB_INFO("\n cmd_cnt=0x%x", cmds->cmd_cnt);

	for (i = 0; i < cmds->cmd_cnt; i++) {
		ret = mipi_dsi_lread(tmp_value, cm, cm->dlen,
			(char *)(uintptr_t)mipi_dsi0_base);
		if (ret) {
			HISI_FB_ERR("read reg error\n");
			return ret;
		}

		/* get invalid value start index */
		read_start_index = 0;
		if (cm->dlen > 1)
			read_start_index = (int)cm->payload[1];
		HISI_FB_INFO("\n cmds[%d]: cm->dlen=0x%x, read_start_index=0x%x", i, cm->dlen, read_start_index);
		for (dlen = 0; dlen < cm->dlen; dlen++) {
			if (dlen < read_start_index){
				continue;
			}
			switch (dlen % 4) {
			case 0:
				if ((tmp_value[dlen / 4] & 0xFF) == 0) {
					out[cnt] = (uint8_t)((tmp_value[dlen / 4] & 0xFF) + '0');
				} else {
					out[cnt] = (uint8_t)(tmp_value[dlen / 4] & 0xFF);
				}
				HISI_FB_INFO("\n out[%d]=0x%x ",cnt, out[cnt]);
				break;
			case 1:
				if (((tmp_value[dlen / 4] >> 8) & 0xFF) == 0) {
					out[cnt] = (uint8_t)(((tmp_value[dlen / 4] >> 8) & 0xFF) + '0');
				} else {
					out[cnt] = (uint8_t)((tmp_value[dlen / 4] >> 8) & 0xFF);
				}
				HISI_FB_INFO("\n out[%d]=0x%x ",cnt, out[cnt]);
				break;
			case 2:
				if (((tmp_value[dlen / 4] >> 16) & 0xFF) == 0) {
					out[cnt] = (uint8_t)(((tmp_value[dlen / 4] >> 16) & 0xFF) + '0');
				} else {
					out[cnt] = (uint8_t)((tmp_value[dlen / 4] >> 16) & 0xFF);
				}
				HISI_FB_INFO("\n out[%d]=0x%x ",cnt, out[cnt]);
				break;
			case 3:
				if (((tmp_value[dlen / 4] >> 24) & 0xFF) == 0) {
					out[cnt] = (uint8_t)(((tmp_value[dlen / 4] >> 24) & 0xFF) + '0');
				} else {
					out[cnt] = (uint8_t)((tmp_value[dlen / 4] >> 24) & 0xFF);
				}
				HISI_FB_INFO("\n out[%d]=0x%x ",cnt, out[cnt]);
				break;
			default:
				break;
			}
			cnt++;
			if (cnt >= max_out_size)
			{
				return 0;
			}
		}
		cm++;
	}
	return 0;
}

#define SN_LENGTH_HIMAX83112 9
static void read_serial_num_hx83112(struct hisi_fb_data_type* hisifd,uint8_t  out[] )
{
	uint32_t mipi_dsi0_base = 0;
	uint32_t read_value[2] = {0};
	int i;
	int ret;
	char project_id_reg[] = {0xbb};
	char project_addr[SN_LENGTH_HIMAX83112] ={ 0xB6, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD,0xBE, 0xBF};

	char playload1_enter1[] = {0xbb, 0x07, 0xB6, 0x00, 0x80, 0xff};
	char playload1_enter2[] = {0xbb, 0x07, 0xB6, 0x00, 0x00, 0xff};

	char otp_poweron_offset1[] = {0xe9, 0xcd};
	char otp_poweron_offset2[] = {0xe9, 0x00};
	char otp_poweron1[] = {0xbb, 0x22};
	char otp_poweron2[] = {0xbb, 0x23};
	char otp_poweroff[] = {0xbb, 0x20};

	if(NULL == hisifd || NULL == out){
		HISI_FB_ERR("NULL pointer! \n");
		return;
	}
	mipi_dsi0_base = hisifd->mipi_dsi0_base;

	HISI_FB_INFO(" Enter: read hx83112 ddic's recognizable nums for brightness-colorpoint calibration.\n");
	struct dsi_cmd_desc otp_poweron1_cmds[] =
	{
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron_offset1), otp_poweron_offset1},
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron1), otp_poweron1},
		{DTYPE_DCS_LWRITE, 0, 1, WAIT_TYPE_MS, sizeof(otp_poweron_offset2), otp_poweron_offset2},
	};
	struct dsi_cmd_desc otp_poweron2_cmds[] =
	{
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron_offset1), otp_poweron_offset1},
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron2), otp_poweron2},
		{DTYPE_DCS_LWRITE, 0, 1, WAIT_TYPE_MS, sizeof(otp_poweron_offset2), otp_poweron_offset2},
	};
	struct dsi_cmd_desc otp_poweroff_cmds[] =
	{
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron_offset1), otp_poweron_offset1},
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweroff), otp_poweroff},
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(otp_poweron_offset2), otp_poweron_offset2},
	};
	struct dsi_cmd_desc playload1_enter_cmds[] =
	{
		{DTYPE_DCS_LWRITE, 0, 1, WAIT_TYPE_US, sizeof(playload1_enter1), playload1_enter1},
		{DTYPE_DCS_LWRITE, 0, 0, WAIT_TYPE_US, sizeof(playload1_enter2), playload1_enter2},
	};
	struct dsi_cmd_desc project_id_cmd[] =
	{
		{DTYPE_GEN_READ, 0, 100, WAIT_TYPE_US, sizeof(project_id_reg), project_id_reg},
	};

	/* send otp power on cmds for hx83112 */
	mipi_dsi_cmds_tx(otp_poweron1_cmds, ARRAY_SIZE(otp_poweron1_cmds), mipi_dsi0_base);
	mipi_dsi_cmds_tx(otp_poweron2_cmds, ARRAY_SIZE(otp_poweron2_cmds), mipi_dsi0_base);
	for(i = 0; i< SN_LENGTH_HIMAX83112; i++)
	{
		playload1_enter1[2] =  project_addr[i];
		playload1_enter2[2] =  project_addr[i];

		mipi_dsi_cmds_tx(playload1_enter_cmds, ARRAY_SIZE(playload1_enter_cmds), mipi_dsi0_base);
		/* Here number "5" means to read five paramaters. */
		ret = mipi_dsi_lread(read_value, project_id_cmd, 5,
			(char *)(uintptr_t)mipi_dsi0_base);
		if (ret)
			HISI_FB_ERR("read reg error\n");

		if(read_value[1] == 0)
			read_value[1]+= '0';
		out[i] = read_value[1];
		HISI_FB_INFO("out[%d]=0x%x.\n",i,out[i]);
		memset(read_value,0,sizeof(read_value));
	}
	/* send otp power off cmds for hx83112 */
	mipi_dsi_cmds_tx(otp_poweroff_cmds, ARRAY_SIZE(otp_poweroff_cmds), mipi_dsi0_base);
	HISI_FB_INFO(" Exit: read hx83112 ddic's recognizable nums for brightness-colorpoint calibration.\n");
	return;
}
#define READ_REG_TD4336_NUM 75
void read_serial_num_td4336(struct hisi_fb_data_type* hisifd,
	uint8_t color_point_id[], uint8_t start_position, uint8_t length)
{
	int i;
	int ret;
	uint32_t mipi_dsi0_base = 0;
	uint32_t read_value_td4336[19] = {0};
	char td4336_reg[] = {0xbf};

	if ((NULL == color_point_id)||(NULL == hisifd) || (start_position + length > READ_REG_TD4336_NUM ))
	{
		HISI_FB_INFO("NULL Pointer!\n");
		return ;
	}

	struct dsi_cmd_desc td4336_reg_cmd[] =
	{
		{DTYPE_GEN_READ1, 0, 100, WAIT_TYPE_US, sizeof(td4336_reg), td4336_reg},
	};

	mipi_dsi0_base = hisifd ->mipi_dsi0_base;
	ret = mipi_dsi_lread(read_value_td4336, td4336_reg_cmd,
		READ_REG_TD4336_NUM, (char *)(uintptr_t)mipi_dsi0_base);
	if (ret)
		HISI_FB_ERR("read reg error\n");

	for(i = 0; i < length; i++,start_position++)
		{
			/* The following numbers such as "4","8","16","24", are have no specfic meanings, so needn't to define macro. */
			switch (start_position % 4)
				{
				case 0:
					if ((read_value_td4336[start_position / 4] & 0xFF) == 0)
						{
							color_point_id[i] = (uint8_t)((read_value_td4336[start_position / 4] & 0xFF) + '0');
						}
					else
						{
							color_point_id[i] = (uint8_t)(read_value_td4336[start_position / 4] & 0xFF);
						}
					break;
				case 1:
					if (((read_value_td4336[start_position / 4] >> 8) & 0xFF) == 0)
						{
							color_point_id[i] = (uint8_t)(((read_value_td4336[start_position / 4] >> 8) & 0xFF) + '0');
						}
					else
						{
							color_point_id[i] = (uint8_t)((read_value_td4336[start_position / 4] >> 8) & 0xFF);
						}
					break;
				case 2:
					if (((read_value_td4336[start_position / 4] >> 16) & 0xFF) == 0)
						{
							color_point_id[i] = (uint8_t)(((read_value_td4336[start_position / 4] >> 16) & 0xFF) + '0');
						}
					else
						{
							color_point_id[i] = (uint8_t)((read_value_td4336[start_position / 4] >> 16) & 0xFF);
						}
				break;
				case 3:
					if (((read_value_td4336[start_position / 4] >> 24) & 0xFF) == 0)
						{
							color_point_id[i] = (uint8_t)(((read_value_td4336[start_position / 4] >> 24) & 0xFF) + '0');
						}
					else
						{
							color_point_id[i] = (uint8_t)((read_value_td4336[start_position / 4] >> 24) & 0xFF);
						}
					break;
				default:
					break;
		}
		HISI_FB_INFO("color_point_id[%d]=0x%x \n ",i,color_point_id[i]);
	}
	return;
}
int get_lcd_panel_id_info(struct hisi_fb_data_type* hisifd)
{
	uint8_t read_panelid_value[LCDKIT_SERIAL_INFO_SIZE] = {0};
	uint32_t mipi_dsi0_base = hisifd->dss_base + DSS_MIPI_DSI0_OFFSET;
	HISI_FB_INFO("Enter get_lcd_panel_id_info eml_read_reg_flag=%d \n ",lcdkit_data.panel->eml_read_reg_flag );
	if(lcdkit_data.panel->eml_read_reg_flag == Hx83112C)
	{
		read_serial_num_hx83112(hisifd,read_panelid_value);
		/* modulesn:Serial number of inspection system(4Bytes) */
		lcd_panelid.modulesn = (((uint32_t)read_panelid_value[5]&0xFF) << 24)
								| (((uint32_t)read_panelid_value[6]&0xFF) << 16)
								| (((uint32_t)read_panelid_value[7]&0xFF) << 8)
								| ((uint32_t)read_panelid_value[8]&0xFF);
		HISI_FB_INFO("\n modulesn=0x%x \n",lcd_panelid.modulesn);

		/* equipid(1Bytes) */
		lcd_panelid.equipid = (uint32_t)read_panelid_value[1];
		HISI_FB_INFO("\n equipid=0x%x \n",lcd_panelid.equipid);

		/* modulemanufactdate:year(1Bytes)-month(1Bytes)-date(1Bytes) */
		lcd_panelid.modulemanufactdate = (((uint32_t)read_panelid_value[2]&0xFF) << 16)
											| (((uint32_t)read_panelid_value[3]&0xFF) << 8)
											| ((uint32_t)read_panelid_value[4]&0xFF);
		HISI_FB_INFO("\n modulemanufactdate=0x%x \n",lcd_panelid.modulemanufactdate);

		/* vendorid(1Bytes) */
		lcd_panelid.vendorid = (uint32_t)read_panelid_value[0];
		HISI_FB_INFO("\n vendorid=0x%x \n",lcd_panelid.vendorid);
	}
	else if((lcdkit_data.panel->eml_read_reg_flag == TD4336))
	{
		read_serial_num_td4336(hisifd,read_panelid_value,COLOR_POINT_START_POSITION_TD4336,COLOR_POINT_LENGTH);
		/* modulesn:Serial number of inspection system(4Bytes) */
		lcd_panelid.modulesn = (((uint32_t)read_panelid_value[6]&0xFF) << 24)
								| (((uint32_t)read_panelid_value[7]&0xFF) << 16)
								| (((uint32_t)read_panelid_value[8]&0xFF) << 8)
								| ((uint32_t)read_panelid_value[9]&0xFF);
		HISI_FB_INFO("\n modulesn=0x%x \n",lcd_panelid.modulesn);

		/* equipid(1Bytes) */
		lcd_panelid.equipid = (uint32_t)read_panelid_value[2];
		HISI_FB_INFO("\n equipid=0x%x \n",lcd_panelid.equipid);

		/* modulemanufactdate:year(1Bytes)-month(1Bytes)-date(1Bytes) */
		lcd_panelid.modulemanufactdate = (((uint32_t)read_panelid_value[3]&0xFF) << 16)
											| (((uint32_t)read_panelid_value[4]&0xFF) << 8)
											| ((uint32_t)read_panelid_value[5]&0xFF);
		HISI_FB_INFO("\n modulemanufactdate=0x%x \n",lcd_panelid.modulemanufactdate);

		/* vendorid(1Bytes) */
		lcd_panelid.vendorid = (uint32_t)read_panelid_value[0];
		HISI_FB_INFO("\n vendorid=0x%x \n",lcd_panelid.vendorid);
	}
	else if((lcdkit_data.panel->eml_read_reg_flag == NT36772A))
	{
		/* enter CMD*-PAGE* */
		mipi_dsi_cmds_tx(lcdkit_infos.panel_info_consistency_enter_cmds.cmds_set, \
						 lcdkit_infos.panel_info_consistency_enter_cmds.cmd_cnt, mipi_dsi0_base);
		/* read panelid info reg */
		lcdkit_panel_id_cmds_rx(read_panelid_value, &lcdkit_infos.panel_info_consistency_cmds, hisifd, LCDKIT_SERIAL_INFO_SIZE);

		/* exit CMD*-PAGE* */
		mipi_dsi_cmds_tx(lcdkit_infos.panel_info_consistency_exit_cmds.cmds_set, \
						 lcdkit_infos.panel_info_consistency_exit_cmds.cmd_cnt, mipi_dsi0_base);

		/* modulesn:Serial number of inspection system(4Bytes) */
		lcd_panelid.modulesn = (((uint32_t)read_panelid_value[14]&0xFF) << 24)
								| (((uint32_t)read_panelid_value[15]&0xFF) << 16)
								| (((uint32_t)read_panelid_value[16]&0xFF) << 8)
								| ((uint32_t)read_panelid_value[17]&0xFF);
		HISI_FB_INFO("\n boltzmann's modulesn=0x%x \n",lcd_panelid.modulesn);

		/* equipid(1Bytes) */
		lcd_panelid.equipid = (uint32_t)read_panelid_value[10];
		HISI_FB_INFO("\n boltzmann's equipid=0x%x \n",lcd_panelid.equipid);

		/* modulemanufactdate:year(1Bytes)-month(1Bytes)-date(1Bytes) */
		lcd_panelid.modulemanufactdate = (((uint32_t)read_panelid_value[11]&0xFF) << 16)
										| (((uint32_t)read_panelid_value[12]&0xFF) << 8)
										| ((uint32_t)read_panelid_value[13]&0xFF);
		HISI_FB_INFO("\n boltzmann's modulemanufactdate=0x%x \n",lcd_panelid.modulemanufactdate);

		/* vendorid(1Bytes) */
		lcd_panelid.vendorid = (uint32_t)read_panelid_value[8];
		HISI_FB_INFO("\n boltzmann's vendorid=0x%x \n",lcd_panelid.vendorid);
	}

	else
	{
		/* enter CMD*-PAGE* */
		mipi_dsi_cmds_tx(lcdkit_infos.panel_info_consistency_enter_cmds.cmds_set, \
						 lcdkit_infos.panel_info_consistency_enter_cmds.cmd_cnt, mipi_dsi0_base);

		/* read panelid info reg */
		lcdkit_panel_id_cmds_rx(read_panelid_value, &lcdkit_infos.panel_info_consistency_cmds, hisifd, LCDKIT_SERIAL_INFO_SIZE);

		/* exit CMD*-PAGE* */
		mipi_dsi_cmds_tx(lcdkit_infos.panel_info_consistency_exit_cmds.cmds_set, \
						 lcdkit_infos.panel_info_consistency_exit_cmds.cmd_cnt, mipi_dsi0_base);

		/* modulesn:Serial number of inspection system(4Bytes) */
		lcd_panelid.modulesn = (((uint32_t)read_panelid_value[3]&0xFF) << 24)
								| (((uint32_t)read_panelid_value[4]&0xFF) << 16)
								| (((uint32_t)read_panelid_value[5]&0xFF) << 8)
								| ((uint32_t)read_panelid_value[6]&0xFF);
		HISI_FB_INFO("\n modulesn=0x%x \n",lcd_panelid.modulesn);

		/* equipid(1Bytes) */
		lcd_panelid.equipid = (uint32_t)read_panelid_value[7];
		HISI_FB_INFO("\n equipid=0x%x \n",lcd_panelid.equipid);

		/* modulemanufactdate:year(1Bytes)-month(1Bytes)-date(1Bytes) */
		lcd_panelid.modulemanufactdate = (((uint32_t)read_panelid_value[0]&0xFF) << 16)
											| (((uint32_t)read_panelid_value[1]&0xFF) << 8)
											| ((uint32_t)read_panelid_value[2]&0xFF);
		HISI_FB_INFO("\n modulemanufactdate=0x%x \n",lcd_panelid.modulemanufactdate);

		/* vendorid(1Bytes) */
		lcd_panelid.vendorid = g_panel_vender_id;
		HISI_FB_INFO("\n vendorid=0x%x \n",lcd_panelid.vendorid);
	}

	return 0;
}

unsigned long lcd_get_color_mem_addr(void)
{
	return HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_BASE;
}

void lcd_write_oeminfo_to_mem(void)
{
	unsigned long mem_addr;
	int ret;

	mem_addr = lcd_get_color_mem_addr();
	ret = memcpy_s((void *)(uintptr_t)mem_addr, sizeof(lcd_color_oeminfo),
		&lcd_color_oeminfo, sizeof(lcd_color_oeminfo));
	if (ret != EOK)
		HISI_FB_ERR("memcpy_s failed,error\n");

	return;
}

void lcd_bright_rgbw_id_from_oeminfo_process(struct hisi_fb_data_type* phisifd)
{
	int ret;

	if(NULL == phisifd)
	{
		HISI_FB_ERR("pointer is NULL \n");
		return;
	}
	ret = get_lcd_panel_id_info(phisifd);
	if(ret != 0)
		HISI_FB_ERR("read panel id from lcd failed!\n");
	ret = get_lcd_bright_rgbw_id_from_oeminfo();
	if(ret < 0)
		HISI_FB_ERR("read panel id wight and color info from oeminfo failed!\n");

	if(lcd_color_oeminfo.id_flag == 0)
	{
		HISI_FB_INFO("panel id_flag is 0\n");
		lcd_color_oeminfo.panel_id = lcd_panelid;
		lcd_color_oeminfo.id_flag = 1;
		ret = set_lcd_bright_rgbw_id_to_oeminfo();
		if(ret < 0)
			HISI_FB_ERR("write panel id wight and color info to oeminfo failed!\n");
		lcd_write_oeminfo_to_mem();
		return;
	}
	else
	{
		if(!memcmp(&lcd_color_oeminfo.panel_id, &lcd_panelid, sizeof(lcd_color_oeminfo.panel_id)))
		{
			HISI_FB_INFO("panel id is same\n");
			lcd_write_oeminfo_to_mem();
			return;
		}
		else
		{
			lcd_color_oeminfo.panel_id = lcd_panelid;
			if((lcd_color_oeminfo.panel_id.modulesn == lcd_panelid.modulesn)&&(lcd_color_oeminfo.panel_id.equipid == lcd_panelid.equipid)&&(lcd_color_oeminfo.panel_id.modulemanufactdate == lcd_panelid.modulemanufactdate))
			{
				HISI_FB_INFO("panle id and oeminfo panel id is same\n");
				if(lcd_color_oeminfo.tc_flag == 0)
				{
					HISI_FB_INFO("panle id and oeminfo panel id is same, set tc_flag to true\n");
					lcd_color_oeminfo.tc_flag = 1;
				}
			}
			else
			{
				HISI_FB_INFO("panle id and oeminfo panel id is not same, set tc_flag to false\n");
				lcd_color_oeminfo.tc_flag = 0;
			}
			ret = set_lcd_bright_rgbw_id_to_oeminfo();
			if(ret < 0)
				HISI_FB_ERR("write panel id wight and color info to oeminfo failed!\n");
			lcd_write_oeminfo_to_mem();
			return;
		}
	}
}
#endif

#if defined (FASTBOOT_DISPLAY_LOGO_HI3660) || defined (FASTBOOT_DISPLAY_LOGO_KIRIN970)
#define OEMINFO_GAMMA_DATA 114 /* OEMINFO ID used for gamma data */
#define OEMINFO_GAMMA_LEN  115 /* OEMINFO ID used fir gamma data len */

/*
 * 1542 equals gamma_r + gamma_g + gamma_b
 * equals (257 + 257 + 257) multiply sizeof(u16)
 * 1542 equals degamma_r + degamma_g + degamma_b
 * equals (257 + 257 +257) multiply  sizeof(u16)
 */
#define GM_IGM_LEN (1542 + 1542)
#define GM_LUT_LEN 257

static int gamma_len = 0;
static uint16_t gamma_lut[GM_LUT_LEN * 6] = {0};

static int lcdkit_read_gamma_from_oeminfo()
{
	struct oeminfo_operators *oeminfo_ops = NULL;

	oeminfo_ops = get_operators(OEMINFO_MODULE_NAME_STR);
	if (!oeminfo_ops) {
		HISI_FB_ERR("get_operators error \n");
		return -1;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_GAMMA_LEN, 4, (char*)&gamma_len)) {
		HISI_FB_ERR("get gamma oeminfo len error, and the gamma len = %d! \n",gamma_len);
		return -1;
	}
	if (gamma_len != GM_IGM_LEN) {
		HISI_FB_ERR("gamma oeminfo len is not correct \n");
		return -1;
	}

	if (oeminfo_ops->get_oeminfo(OEMINFO_GAMMA_DATA, GM_IGM_LEN, (char*)gamma_lut)) {
		HISI_FB_ERR("get gamma oeminfo data error \n");
		return -1;
	}

	return 0;
}

int lcdkit_write_gm_to_reserved_mem()
{
	int ret;
	unsigned long gm_addr;
	unsigned long gm_size;

	gm_addr = HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_BASE;
	gm_size = HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_SIZE;

	if (gm_size < GM_IGM_LEN + 4) {
		HISI_FB_ERR("gamma mem size is not enough !\n");
		return -1;
	}

	ret = lcdkit_read_gamma_from_oeminfo();
	if (ret) {
		writel(0, gm_addr);
		HISI_FB_ERR("can not get oeminfo gamma data!\n");
		return -1;
	}

	writel((unsigned int)GM_IGM_LEN, gm_addr);
	gm_addr += 4;
	ret = memcpy_s((void *)(uintptr_t)gm_addr, GM_IGM_LEN,
		gamma_lut, GM_IGM_LEN);
	if (ret != EOK)
		HISI_FB_ERR("memcpy_s failed,error\n");

	return 0;
}

#endif
