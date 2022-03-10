

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_SCHARGER_INTERFACE_H__
#define __SOC_SCHARGER_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/4) REG_PD
 ****************************************************************************/
/* �Ĵ���˵����Vendor ID Low
   λ����UNION�ṹ:  SOC_SCHARGER_VENDIDL_UNION */
#define SOC_SCHARGER_VENDIDL_ADDR(base)               ((base) + (0x00))

/* �Ĵ���˵����Vendor ID High
   λ����UNION�ṹ:  SOC_SCHARGER_VENDIDH_UNION */
#define SOC_SCHARGER_VENDIDH_ADDR(base)               ((base) + (0x01))

/* �Ĵ���˵����Product ID Low
   λ����UNION�ṹ:  SOC_SCHARGER_PRODIDL_UNION */
#define SOC_SCHARGER_PRODIDL_ADDR(base)               ((base) + (0x02))

/* �Ĵ���˵����Product ID High
   λ����UNION�ṹ:  SOC_SCHARGER_PRODIDH_UNION */
#define SOC_SCHARGER_PRODIDH_ADDR(base)               ((base) + (0x03))

/* �Ĵ���˵����Device ID Low
   λ����UNION�ṹ:  SOC_SCHARGER_DEVIDL_UNION */
#define SOC_SCHARGER_DEVIDL_ADDR(base)                ((base) + (0x04))

/* �Ĵ���˵����Device ID High
   λ����UNION�ṹ:  SOC_SCHARGER_DEVIDH_UNION */
#define SOC_SCHARGER_DEVIDH_ADDR(base)                ((base) + (0x05))

/* �Ĵ���˵����USB Type-C Revision Low
   λ����UNION�ṹ:  SOC_SCHARGER_TYPECREVL_UNION */
#define SOC_SCHARGER_TYPECREVL_ADDR(base)             ((base) + (0x06))

/* �Ĵ���˵����USB Type-C Revision High
   λ����UNION�ṹ:  SOC_SCHARGER_TYPECREVH_UNION */
#define SOC_SCHARGER_TYPECREVH_ADDR(base)             ((base) + (0x07))

/* �Ĵ���˵����USB PD Version
   λ����UNION�ṹ:  SOC_SCHARGER_USBPDVER_UNION */
#define SOC_SCHARGER_USBPDVER_ADDR(base)              ((base) + (0x08))

/* �Ĵ���˵����USB PD Revision
   λ����UNION�ṹ:  SOC_SCHARGER_USBPDREV_UNION */
#define SOC_SCHARGER_USBPDREV_ADDR(base)              ((base) + (0x09))

/* �Ĵ���˵����USB PD Interface Revision Low
   λ����UNION�ṹ:  SOC_SCHARGER_PDIFREVL_UNION */
#define SOC_SCHARGER_PDIFREVL_ADDR(base)              ((base) + (0x0A))

/* �Ĵ���˵����USB PD Interface Revision High
   λ����UNION�ṹ:  SOC_SCHARGER_PDIFREVH_UNION */
#define SOC_SCHARGER_PDIFREVH_ADDR(base)              ((base) + (0x0B))

/* �Ĵ���˵����PD Alert Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_ALERT_L_UNION */
#define SOC_SCHARGER_PD_ALERT_L_ADDR(base)            ((base) + (0x10))

/* �Ĵ���˵����PD Alert High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_ALERT_H_UNION */
#define SOC_SCHARGER_PD_ALERT_H_ADDR(base)            ((base) + (0x11))

/* �Ĵ���˵����PD Alert Mask Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_ALERT_MSK_L_UNION */
#define SOC_SCHARGER_PD_ALERT_MSK_L_ADDR(base)        ((base) + (0x12))

/* �Ĵ���˵����PD Alert Mask High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_ALERT_MSK_H_UNION */
#define SOC_SCHARGER_PD_ALERT_MSK_H_ADDR(base)        ((base) + (0x13))

/* �Ĵ���˵����PD Power State Mask
   λ����UNION�ṹ:  SOC_SCHARGER_PD_PWRSTAT_MSK_UNION */
#define SOC_SCHARGER_PD_PWRSTAT_MSK_ADDR(base)        ((base) + (0x14))

/* �Ĵ���˵����PD Fault State Mask
   λ����UNION�ṹ:  SOC_SCHARGER_PD_FAULTSTAT_MSK_UNION */
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_ADDR(base)      ((base) + (0x15))

/* �Ĵ���˵����PD TCPC Control
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TCPC_CTRL_UNION */
#define SOC_SCHARGER_PD_TCPC_CTRL_ADDR(base)          ((base) + (0x19))

/* �Ĵ���˵����PD Role Control
   λ����UNION�ṹ:  SOC_SCHARGER_PD_ROLE_CTRL_UNION */
#define SOC_SCHARGER_PD_ROLE_CTRL_ADDR(base)          ((base) + (0x1A))

/* �Ĵ���˵����PD Fault Control
   λ����UNION�ṹ:  SOC_SCHARGER_PD_FAULT_CTRL_UNION */
#define SOC_SCHARGER_PD_FAULT_CTRL_ADDR(base)         ((base) + (0x1B))

/* �Ĵ���˵����PD Power Control
   λ����UNION�ṹ:  SOC_SCHARGER_PD_PWR_CTRL_UNION */
#define SOC_SCHARGER_PD_PWR_CTRL_ADDR(base)           ((base) + (0x1C))

/* �Ĵ���˵����PD CC Status
   λ����UNION�ṹ:  SOC_SCHARGER_PD_CC_STATUS_UNION */
#define SOC_SCHARGER_PD_CC_STATUS_ADDR(base)          ((base) + (0x1D))

/* �Ĵ���˵����PD Power Status
   λ����UNION�ṹ:  SOC_SCHARGER_PD_PWR_STATUS_UNION */
#define SOC_SCHARGER_PD_PWR_STATUS_ADDR(base)         ((base) + (0x1E))

/* �Ĵ���˵����PD Fault Status
   λ����UNION�ṹ:  SOC_SCHARGER_PD_FAULT_STATUS_UNION */
#define SOC_SCHARGER_PD_FAULT_STATUS_ADDR(base)       ((base) + (0x1F))

/* �Ĵ���˵����PD Command
   λ����UNION�ṹ:  SOC_SCHARGER_PD_COMMAND_UNION */
#define SOC_SCHARGER_PD_COMMAND_ADDR(base)            ((base) + (0x23))

/* �Ĵ���˵����PD Device Cap1 Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DEVCAP1L_UNION */
#define SOC_SCHARGER_PD_DEVCAP1L_ADDR(base)           ((base) + (0x24))

/* �Ĵ���˵����PD Device Cap1 High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DEVCAP1H_UNION */
#define SOC_SCHARGER_PD_DEVCAP1H_ADDR(base)           ((base) + (0x25))

/* �Ĵ���˵����PD Device Cap2 Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DEVCAP2L_UNION */
#define SOC_SCHARGER_PD_DEVCAP2L_ADDR(base)           ((base) + (0x26))

/* �Ĵ���˵����PD Device Cap2 High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DEVCAP2H_UNION */
#define SOC_SCHARGER_PD_DEVCAP2H_ADDR(base)           ((base) + (0x27))

/* �Ĵ���˵����PD Standard Input Capabilities
   λ����UNION�ṹ:  SOC_SCHARGER_PD_STDIN_CAP_UNION */
#define SOC_SCHARGER_PD_STDIN_CAP_ADDR(base)          ((base) + (0x28))

/* �Ĵ���˵����PD Standard Output Capabilities
   λ����UNION�ṹ:  SOC_SCHARGER_PD_STDOUT_CAP_UNION */
#define SOC_SCHARGER_PD_STDOUT_CAP_ADDR(base)         ((base) + (0x29))

/* �Ĵ���˵����PD Message Header
   λ����UNION�ṹ:  SOC_SCHARGER_PD_MSG_HEADR_UNION */
#define SOC_SCHARGER_PD_MSG_HEADR_ADDR(base)          ((base) + (0x2E))

/* �Ĵ���˵����PD RX Detect
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXDETECT_UNION */
#define SOC_SCHARGER_PD_RXDETECT_ADDR(base)           ((base) + (0x2F))

/* �Ĵ���˵����PD RX ByteCount
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXBYTECNT_UNION */
#define SOC_SCHARGER_PD_RXBYTECNT_ADDR(base)          ((base) + (0x30))

/* �Ĵ���˵����PD RX Type
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXTYPE_UNION */
#define SOC_SCHARGER_PD_RXTYPE_ADDR(base)             ((base) + (0x31))

/* �Ĵ���˵����PD RX Header Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXHEADL_UNION */
#define SOC_SCHARGER_PD_RXHEADL_ADDR(base)            ((base) + (0x32))

/* �Ĵ���˵����PD RX Header High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXHEADH_UNION */
#define SOC_SCHARGER_PD_RXHEADH_ADDR(base)            ((base) + (0x33))

/* �Ĵ���˵����PD RX Data Payload
   λ����UNION�ṹ:  SOC_SCHARGER_PD_RXDATA_0_UNION */
#define SOC_SCHARGER_PD_RXDATA_0_ADDR(base)           ((base) + (k*1+0x34))

/* �Ĵ���˵����PD Transmit
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TRANSMIT_UNION */
#define SOC_SCHARGER_PD_TRANSMIT_ADDR(base)           ((base) + (0x50))

/* �Ĵ���˵����PD TX Byte Count
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TXBYTECNT_UNION */
#define SOC_SCHARGER_PD_TXBYTECNT_ADDR(base)          ((base) + (0x51))

/* �Ĵ���˵����PD TX Header Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TXHEADL_UNION */
#define SOC_SCHARGER_PD_TXHEADL_ADDR(base)            ((base) + (0x52))

/* �Ĵ���˵����PD TX Header High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TXHEADH_UNION */
#define SOC_SCHARGER_PD_TXHEADH_ADDR(base)            ((base) + (0x53))

/* �Ĵ���˵����PD TX Payload
   λ����UNION�ṹ:  SOC_SCHARGER_PD_TXDATA_UNION */
#define SOC_SCHARGER_PD_TXDATA_ADDR(base)             ((base) + (k*1+0x54))

/* �Ĵ���˵����PD VBUS Voltage Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_VOL_L_UNION */
#define SOC_SCHARGER_PD_VBUS_VOL_L_ADDR(base)         ((base) + (0x70))

/* �Ĵ���˵����PD VBUS Voltage High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_VOL_H_UNION */
#define SOC_SCHARGER_PD_VBUS_VOL_H_ADDR(base)         ((base) + (0x71))

/* �Ĵ���˵����VBUS Sink Disconnect Threshold Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_SNK_DISCL_UNION */
#define SOC_SCHARGER_PD_VBUS_SNK_DISCL_ADDR(base)     ((base) + (0x72))

/* �Ĵ���˵����VBUS Sink Disconnect Threshold High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_SNK_DISCH_UNION */
#define SOC_SCHARGER_PD_VBUS_SNK_DISCH_ADDR(base)     ((base) + (0x73))

/* �Ĵ���˵����VBUS Discharge Stop Threshold Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_STOP_DISCL_UNION */
#define SOC_SCHARGER_PD_VBUS_STOP_DISCL_ADDR(base)    ((base) + (0x74))

/* �Ĵ���˵����VBUS Discharge Stop Threshold High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VBUS_STOP_DISCH_UNION */
#define SOC_SCHARGER_PD_VBUS_STOP_DISCH_ADDR(base)    ((base) + (0x75))

/* �Ĵ���˵����Voltage High Trip Point Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VALARMH_CFGL_UNION */
#define SOC_SCHARGER_PD_VALARMH_CFGL_ADDR(base)       ((base) + (0x76))

/* �Ĵ���˵����Voltage High Trip Point High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VALARMH_CFGH_UNION */
#define SOC_SCHARGER_PD_VALARMH_CFGH_ADDR(base)       ((base) + (0x77))

/* �Ĵ���˵����Voltage Low Trip Point Low
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VALARML_CFGL_UNION */
#define SOC_SCHARGER_PD_VALARML_CFGL_ADDR(base)       ((base) + (0x78))

/* �Ĵ���˵����Voltage Low Trip Point High
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VALARML_CFGH_UNION */
#define SOC_SCHARGER_PD_VALARML_CFGH_ADDR(base)       ((base) + (0x79))

/* �Ĵ���˵����PD�Զ������üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VDM_CFG_0_UNION */
#define SOC_SCHARGER_PD_VDM_CFG_0_ADDR(base)          ((base) + (0x7A))

/* �Ĵ���˵����PD�Զ���ʹ�ܼĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VDM_ENABLE_UNION */
#define SOC_SCHARGER_PD_VDM_ENABLE_ADDR(base)         ((base) + (0x7B))

/* �Ĵ���˵����PD�Զ������üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_VDM_CFG_1_UNION */
#define SOC_SCHARGER_PD_VDM_CFG_1_ADDR(base)          ((base) + (0x7C))

/* �Ĵ���˵����оƬ�����ã����Բ�Ʒ���š������������üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RDATA_CFG_UNION */
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_ADDR(base)      ((base) + (0x7D))

/* �Ĵ���˵����оƬ�����ã����Բ�Ʒ���š����Իض����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RDATA_UNION */
#define SOC_SCHARGER_PD_DBG_RDATA_ADDR(base)          ((base) + (0x7E))

/* �Ĵ���˵����Vendor Define Register, Page Select
   λ����UNION�ṹ:  SOC_SCHARGER_VDM_PAGE_SELECT_UNION */
#define SOC_SCHARGER_VDM_PAGE_SELECT_ADDR(base)       ((base) + (0x7F))



/****************************************************************************
                     (2/4) REG_PAGE0
 ****************************************************************************/
 #define PAGE0_BASE    (0x0080)
/* �Ĵ���˵����fcp slave ����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_STATUIS_UNION */
#define SOC_SCHARGER_STATUIS_ADDR(base)               ((base) + (0x00) + PAGE0_BASE)

/* �Ĵ���˵����FCP CNTL���üĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_CNTL_UNION */
#define SOC_SCHARGER_CNTL_ADDR(base)                  ((base) + (0x01) + PAGE0_BASE)

/* �Ĵ���˵����fcp ��д�������üĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_CMD_UNION */
#define SOC_SCHARGER_CMD_ADDR(base)                   ((base) + (0x04) + PAGE0_BASE)

/* �Ĵ���˵����fcp burst��д�������üĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_LENGTH_UNION */
#define SOC_SCHARGER_FCP_LENGTH_ADDR(base)            ((base) + (0x05) + PAGE0_BASE)

/* �Ĵ���˵����fcp ��д��ַ���üĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_ADDR_UNION */
#define SOC_SCHARGER_ADDR_ADDR(base)                  ((base) + (0x07) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA0_UNION */
#define SOC_SCHARGER_DATA0_ADDR(base)                 ((base) + (0x08) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA1_UNION */
#define SOC_SCHARGER_DATA1_ADDR(base)                 ((base) + (0x09) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA2_UNION */
#define SOC_SCHARGER_DATA2_ADDR(base)                 ((base) + (0x0A) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA3_UNION */
#define SOC_SCHARGER_DATA3_ADDR(base)                 ((base) + (0x0B) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA4_UNION */
#define SOC_SCHARGER_DATA4_ADDR(base)                 ((base) + (0x0C) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA5_UNION */
#define SOC_SCHARGER_DATA5_ADDR(base)                 ((base) + (0x0D) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA6_UNION */
#define SOC_SCHARGER_DATA6_ADDR(base)                 ((base) + (0x0E) + PAGE0_BASE)

/* �Ĵ���˵����fcp д���ݼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA7_UNION */
#define SOC_SCHARGER_DATA7_ADDR(base)                 ((base) + (0x0F) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA0_UNION */
#define SOC_SCHARGER_FCP_RDATA0_ADDR(base)            ((base) + (0x10) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA1_UNION */
#define SOC_SCHARGER_FCP_RDATA1_ADDR(base)            ((base) + (0x11) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA2_UNION */
#define SOC_SCHARGER_FCP_RDATA2_ADDR(base)            ((base) + (0x12) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA3_UNION */
#define SOC_SCHARGER_FCP_RDATA3_ADDR(base)            ((base) + (0x13) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA4_UNION */
#define SOC_SCHARGER_FCP_RDATA4_ADDR(base)            ((base) + (0x14) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA5_UNION */
#define SOC_SCHARGER_FCP_RDATA5_ADDR(base)            ((base) + (0x15) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA6_UNION */
#define SOC_SCHARGER_FCP_RDATA6_ADDR(base)            ((base) + (0x16) + PAGE0_BASE)

/* �Ĵ���˵����slave���ص����ݡ�
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA7_UNION */
#define SOC_SCHARGER_FCP_RDATA7_ADDR(base)            ((base) + (0x17) + PAGE0_BASE)

/* �Ĵ���˵����FCP �ж�1�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_ISR1_UNION */
#define SOC_SCHARGER_ISR1_ADDR(base)                  ((base) + (0x19) + PAGE0_BASE)

/* �Ĵ���˵����FCP �ж�2�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_ISR2_UNION */
#define SOC_SCHARGER_ISR2_ADDR(base)                  ((base) + (0x1A) + PAGE0_BASE)

/* �Ĵ���˵����FCP �ж�����1�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_IMR1_UNION */
#define SOC_SCHARGER_IMR1_ADDR(base)                  ((base) + (0x1B) + PAGE0_BASE)

/* �Ĵ���˵����FCP �ж�����2�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_IMR2_UNION */
#define SOC_SCHARGER_IMR2_ADDR(base)                  ((base) + (0x1C) + PAGE0_BASE)

/* �Ĵ���˵����FCP�ж�5�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ5_UNION */
#define SOC_SCHARGER_FCP_IRQ5_ADDR(base)              ((base) + (0x1D) + PAGE0_BASE)

/* �Ĵ���˵����FCP�ж�����5�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ5_MASK_UNION */
#define SOC_SCHARGER_FCP_IRQ5_MASK_ADDR(base)         ((base) + (0x1E) + PAGE0_BASE)

/* �Ĵ���˵������ѹ�����ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_CTRL_UNION */
#define SOC_SCHARGER_FCP_CTRL_ADDR(base)              ((base) + (0x1F) + PAGE0_BASE)

/* �Ĵ���˵������ѹ���Э��ģʽ2��λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_MODE2_SET_UNION */
#define SOC_SCHARGER_FCP_MODE2_SET_ADDR(base)         ((base) + (0x20) + PAGE0_BASE)

/* �Ĵ���˵������ѹ���Adapter���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_ADAP_CTRL_UNION */
#define SOC_SCHARGER_FCP_ADAP_CTRL_ADDR(base)         ((base) + (0x21) + PAGE0_BASE)

/* �Ĵ���˵����slave�������ݲɼ���ָʾ�źš�
   λ����UNION�ṹ:  SOC_SCHARGER_RDATA_READY_UNION */
#define SOC_SCHARGER_RDATA_READY_ADDR(base)           ((base) + (0x22) + PAGE0_BASE)

/* �Ĵ���˵����FCP��λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_SOFT_RST_CTRL_UNION */
#define SOC_SCHARGER_FCP_SOFT_RST_CTRL_ADDR(base)     ((base) + (0x23) + PAGE0_BASE)

/* �Ĵ���˵����FCP������Parity����ض��Ĵ���
            ˵�������Բ�Ʒ����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RDATA_PARITY_ERR_UNION */
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_ADDR(base)  ((base) + (0x25) + PAGE0_BASE)

/* �Ĵ���˵����FCP��ʼ��Retry���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_INIT_RETRY_CFG_UNION */
#define SOC_SCHARGER_FCP_INIT_RETRY_CFG_ADDR(base)    ((base) + (0x26) + PAGE0_BASE)

/* �Ĵ���˵����HKADC��λ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_SOFT_RST_CTRL_UNION */
#define SOC_SCHARGER_HKADC_SOFT_RST_CTRL_ADDR(base)   ((base) + (0x28) + PAGE0_BASE)

/* �Ĵ���˵����HKADC���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CTRL0_UNION */
#define SOC_SCHARGER_HKADC_CTRL0_ADDR(base)           ((base) + (0x29) + PAGE0_BASE)

/* �Ĵ���˵����HKADC START�ź�����
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_START_UNION */
#define SOC_SCHARGER_HKADC_START_ADDR(base)           ((base) + (0x2A) + PAGE0_BASE)

/* �Ĵ���˵����HKADC���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CTRL1_UNION */
#define SOC_SCHARGER_HKADC_CTRL1_ADDR(base)           ((base) + (0x2B) + PAGE0_BASE)

/* �Ĵ���˵����HKADC��ѯ��־λ��8bit���߾��ȣ�
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_SEQ_CH_H_UNION */
#define SOC_SCHARGER_HKADC_SEQ_CH_H_ADDR(base)        ((base) + (0x2C) + PAGE0_BASE)

/* �Ĵ���˵����HKADC��ѯ��־λ��8bit���߾��ȣ�
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_SEQ_CH_L_UNION */
#define SOC_SCHARGER_HKADC_SEQ_CH_L_ADDR(base)        ((base) + (0x2D) + PAGE0_BASE)

/* �Ĵ���˵����HKADC���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CTRL2_UNION */
#define SOC_SCHARGER_HKADC_CTRL2_ADDR(base)           ((base) + (0x2E) + PAGE0_BASE)

/* �Ĵ���˵�����ŵ���ƼĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_DISCHG_EN_UNION */
#define SOC_SCHARGER_SOH_DISCHG_EN_ADDR(base)         ((base) + (0x2F) + PAGE0_BASE)

/* �Ĵ���˵����ACR ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_CTRL_UNION */
#define SOC_SCHARGER_ACR_CTRL_ADDR(base)              ((base) + (0x30) + PAGE0_BASE)

/* �Ĵ���˵����HKADCѭ����ѯģʽ���ݶ������ź�
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_RD_SEQ_UNION */
#define SOC_SCHARGER_HKADC_RD_SEQ_ADDR(base)          ((base) + (0x31) + PAGE0_BASE)

/* �Ĵ���˵������ǰADC�����Ǵ��ڳ��״̬���Ƿǳ��״̬
   λ����UNION�ṹ:  SOC_SCHARGER_PULSE_NON_CHG_FLAG_UNION */
#define SOC_SCHARGER_PULSE_NON_CHG_FLAG_ADDR(base)    ((base) + (0x32) + PAGE0_BASE)

/* �Ĵ���˵����VUSB���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_VUSB_ADC_L_UNION */
#define SOC_SCHARGER_VUSB_ADC_L_ADDR(base)            ((base) + (0x33) + PAGE0_BASE)

/* �Ĵ���˵����VUSB���������6bit
   λ����UNION�ṹ:  SOC_SCHARGER_VUSB_ADC_H_UNION */
#define SOC_SCHARGER_VUSB_ADC_H_ADDR(base)            ((base) + (0x34) + PAGE0_BASE)

/* �Ĵ���˵����IBUS���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_IBUS_ADC_L_UNION */
#define SOC_SCHARGER_IBUS_ADC_L_ADDR(base)            ((base) + (0x35) + PAGE0_BASE)

/* �Ĵ���˵����IBUS���������5bit
   λ����UNION�ṹ:  SOC_SCHARGER_IBUS_ADC_H_UNION */
#define SOC_SCHARGER_IBUS_ADC_H_ADDR(base)            ((base) + (0x36) + PAGE0_BASE)

/* �Ĵ���˵����VBUS���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_VBUS_ADC_L_UNION */
#define SOC_SCHARGER_VBUS_ADC_L_ADDR(base)            ((base) + (0x37) + PAGE0_BASE)

/* �Ĵ���˵����VBUS���������6bit
   λ����UNION�ṹ:  SOC_SCHARGER_VBUS_ADC_H_UNION */
#define SOC_SCHARGER_VBUS_ADC_H_ADDR(base)            ((base) + (0x38) + PAGE0_BASE)

/* �Ĵ���˵����VOUT���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_VOUT_ADC_L_UNION */
#define SOC_SCHARGER_VOUT_ADC_L_ADDR(base)            ((base) + (0x39) + PAGE0_BASE)

/* �Ĵ���˵����VOUT���������5bit
   λ����UNION�ṹ:  SOC_SCHARGER_VOUT_ADC_H_UNION */
#define SOC_SCHARGER_VOUT_ADC_H_ADDR(base)            ((base) + (0x3A) + PAGE0_BASE)

/* �Ĵ���˵����VBAT���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_VBAT_ADC_L_UNION */
#define SOC_SCHARGER_VBAT_ADC_L_ADDR(base)            ((base) + (0x3B) + PAGE0_BASE)

/* �Ĵ���˵����VBAT���������5bit
   λ����UNION�ṹ:  SOC_SCHARGER_VBAT_ADC_H_UNION */
#define SOC_SCHARGER_VBAT_ADC_H_ADDR(base)            ((base) + (0x3C) + PAGE0_BASE)

/* �Ĵ���˵����IBAT���������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_IBAT_ADC_L_UNION */
#define SOC_SCHARGER_IBAT_ADC_L_ADDR(base)            ((base) + (0x3D) + PAGE0_BASE)

/* �Ĵ���˵����IBAT���������6bit
   λ����UNION�ṹ:  SOC_SCHARGER_IBAT_ADC_H_UNION */
#define SOC_SCHARGER_IBAT_ADC_H_ADDR(base)            ((base) + (0x3E) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��6 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_6_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_6_L_ADDR(base)        ((base) + (0x3F) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��6 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_6_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_6_H_ADDR(base)        ((base) + (0x40) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��7 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_7_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_7_L_ADDR(base)        ((base) + (0x41) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��7 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_7_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_7_H_ADDR(base)        ((base) + (0x42) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��8 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_8_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_8_L_ADDR(base)        ((base) + (0x43) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��8 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_8_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_8_H_ADDR(base)        ((base) + (0x44) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��9 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_9_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_9_L_ADDR(base)        ((base) + (0x45) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��9 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_9_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_9_H_ADDR(base)        ((base) + (0x46) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��10 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_10_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_10_L_ADDR(base)       ((base) + (0x47) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��10 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_10_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_10_H_ADDR(base)       ((base) + (0x48) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��11 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_11_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_11_L_ADDR(base)       ((base) + (0x49) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��11 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_11_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_11_H_ADDR(base)       ((base) + (0x4A) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��12 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_12_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_12_L_ADDR(base)       ((base) + (0x4B) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��12 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_12_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_12_H_ADDR(base)       ((base) + (0x4C) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��13 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_13_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_13_L_ADDR(base)       ((base) + (0x4D) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��13 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_13_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_13_H_ADDR(base)       ((base) + (0x4E) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��14 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_14_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_14_L_ADDR(base)       ((base) + (0x4F) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��14 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_14_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_14_H_ADDR(base)       ((base) + (0x50) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��15 ADCת�������8bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_15_L_UNION */
#define SOC_SCHARGER_HKADC_DATA_15_L_ADDR(base)       ((base) + (0x51) + PAGE0_BASE)

/* �Ĵ���˵����ͨ��15 ADCת�������4bit
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_DATA_15_H_UNION */
#define SOC_SCHARGER_HKADC_DATA_15_H_ADDR(base)       ((base) + (0x52) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��BMCʱ�ӻָ���·���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_CDR_CFG_0_UNION */
#define SOC_SCHARGER_PD_CDR_CFG_0_ADDR(base)          ((base) + (0x58) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��BMCʱ�ӻָ���·���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_CDR_CFG_1_UNION */
#define SOC_SCHARGER_PD_CDR_CFG_1_ADDR(base)          ((base) + (0x59) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_CFG_0_UNION */
#define SOC_SCHARGER_PD_DBG_CFG_0_ADDR(base)          ((base) + (0x5A) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_CFG_1_UNION */
#define SOC_SCHARGER_PD_DBG_CFG_1_ADDR(base)          ((base) + (0x5B) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�ûض��Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RO_0_UNION */
#define SOC_SCHARGER_PD_DBG_RO_0_ADDR(base)           ((base) + (0x5C) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�ûض��Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RO_1_UNION */
#define SOC_SCHARGER_PD_DBG_RO_1_ADDR(base)           ((base) + (0x5D) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�ûض��Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RO_2_UNION */
#define SOC_SCHARGER_PD_DBG_RO_2_ADDR(base)           ((base) + (0x5E) + PAGE0_BASE)

/* �Ĵ���˵����PDģ��Debug�ûض��Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_PD_DBG_RO_3_UNION */
#define SOC_SCHARGER_PD_DBG_RO_3_ADDR(base)           ((base) + (0x5F) + PAGE0_BASE)

/* �Ĵ���˵����ʵ���жϺ�α�ж�ѡ���ź�
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FAKE_SEL_UNION */
#define SOC_SCHARGER_IRQ_FAKE_SEL_ADDR(base)          ((base) + (0x60) + PAGE0_BASE)

/* �Ĵ���˵����α�ж�Դ
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FAKE_UNION */
#define SOC_SCHARGER_IRQ_FAKE_ADDR(base)              ((base) + (0x61) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_UNION */
#define SOC_SCHARGER_IRQ_FLAG_ADDR(base)              ((base) + (0x62) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_0_UNION */
#define SOC_SCHARGER_IRQ_FLAG_0_ADDR(base)            ((base) + (0x63) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_1_UNION */
#define SOC_SCHARGER_IRQ_FLAG_1_ADDR(base)            ((base) + (0x64) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_2_UNION */
#define SOC_SCHARGER_IRQ_FLAG_2_ADDR(base)            ((base) + (0x65) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_3_UNION */
#define SOC_SCHARGER_IRQ_FLAG_3_ADDR(base)            ((base) + (0x66) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_4_UNION */
#define SOC_SCHARGER_IRQ_FLAG_4_ADDR(base)            ((base) + (0x67) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_5_UNION */
#define SOC_SCHARGER_IRQ_FLAG_5_ADDR(base)            ((base) + (0x68) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_6_UNION */
#define SOC_SCHARGER_IRQ_FLAG_6_ADDR(base)            ((base) + (0x69) + PAGE0_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_FLAG_7_UNION */
#define SOC_SCHARGER_IRQ_FLAG_7_ADDR(base)            ((base) + (0x6A) + PAGE0_BASE)

/* �Ĵ���˵�������Ź���λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_WDT_SOFT_RST_UNION */
#define SOC_SCHARGER_WDT_SOFT_RST_ADDR(base)          ((base) + (0x6B) + PAGE0_BASE)

/* �Ĵ���˵����ι��ʱ����ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_WDT_CTRL_UNION */
#define SOC_SCHARGER_WDT_CTRL_ADDR(base)              ((base) + (0x6C) + PAGE0_BASE)

/* �Ĵ���˵����ֱ��IBATУ׼��λ���ڼĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_DC_IBAT_REGULATOR_UNION */
#define SOC_SCHARGER_DC_IBAT_REGULATOR_ADDR(base)     ((base) + (0x6D) + PAGE0_BASE)

/* �Ĵ���˵����ֱ��VBATУ׼��λ���ڼĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_DC_VBAT_REGULATOR_UNION */
#define SOC_SCHARGER_DC_VBAT_REGULATOR_ADDR(base)     ((base) + (0x6E) + PAGE0_BASE)

/* �Ĵ���˵����ֱ��VOUTУ׼��λ���ڼĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_DC_VOUT_REGULATOR_UNION */
#define SOC_SCHARGER_DC_VOUT_REGULATOR_ADDR(base)     ((base) + (0x6F) + PAGE0_BASE)

/* �Ĵ���˵����OTGģ�����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_UNION */
#define SOC_SCHARGER_OTG_CFG_ADDR(base)               ((base) + (0x70) + PAGE0_BASE)

/* �Ĵ���˵�������������üĴ���0
   λ����UNION�ṹ:  SOC_SCHARGER_PULSE_CHG_CFG0_UNION */
#define SOC_SCHARGER_PULSE_CHG_CFG0_ADDR(base)        ((base) + (0x71) + PAGE0_BASE)

/* �Ĵ���˵�������������üĴ���1
   λ����UNION�ṹ:  SOC_SCHARGER_PULSE_CHG_CFG1_UNION */
#define SOC_SCHARGER_PULSE_CHG_CFG1_ADDR(base)        ((base) + (0x72) + PAGE0_BASE)

/* �Ĵ���˵��������ģʽ�ŵ�ʱ�����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_DISCHG_TIME_UNION */
#define SOC_SCHARGER_DISCHG_TIME_ADDR(base)           ((base) + (0x73) + PAGE0_BASE)

/* �Ĵ���˵���������ڲ��ź�״̬�Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_DIG_STATUS0_UNION */
#define SOC_SCHARGER_DIG_STATUS0_ADDR(base)           ((base) + (0x74) + PAGE0_BASE)

/* �Ĵ���˵���������������üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_SC_TESTBUS_CFG_UNION */
#define SOC_SCHARGER_SC_TESTBUS_CFG_ADDR(base)        ((base) + (0x76) + PAGE0_BASE)

/* �Ĵ���˵�����������߻ض��Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_SC_TESTBUS_RDATA_UNION */
#define SOC_SCHARGER_SC_TESTBUS_RDATA_ADDR(base)      ((base) + (0x77) + PAGE0_BASE)

/* �Ĵ���˵����ȫ����λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_GLB_SOFT_RST_CTRL_UNION */
#define SOC_SCHARGER_GLB_SOFT_RST_CTRL_ADDR(base)     ((base) + (0x78) + PAGE0_BASE)

/* �Ĵ���˵����EFUSE��λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_UNION */
#define SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_ADDR(base)   ((base) + (0x79) + PAGE0_BASE)

/* �Ĵ���˵����CORE CRGʱ��ʹ�����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_UNION */
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_ADDR(base)    ((base) + (0x7A) + PAGE0_BASE)

/* �Ĵ���˵����BUCKģʽ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_MODE_CFG_UNION */
#define SOC_SCHARGER_BUCK_MODE_CFG_ADDR(base)         ((base) + (0x7B) + PAGE0_BASE)

/* �Ĵ���˵����SCC���ģʽ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_SC_MODE_CFG_UNION */
#define SOC_SCHARGER_SC_MODE_CFG_ADDR(base)           ((base) + (0x7C) + PAGE0_BASE)

/* �Ĵ���˵����LVC���ģʽ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_LVC_MODE_CFG_UNION */
#define SOC_SCHARGER_LVC_MODE_CFG_ADDR(base)          ((base) + (0x7D) + PAGE0_BASE)

/* �Ĵ���˵����BUCKʹ�������ź�
   λ����UNION�ṹ:  SOC_SCHARGER_SC_BUCK_ENB_UNION */
#define SOC_SCHARGER_SC_BUCK_ENB_ADDR(base)           ((base) + (0x7E) + PAGE0_BASE)

/* �Ĵ���˵����OVPģ��ʹ�ܿ���
   λ����UNION�ṹ:  SOC_SCHARGER_SC_OVP_MOS_EN_UNION */
#define SOC_SCHARGER_SC_OVP_MOS_EN_ADDR(base)         ((base) + (0x7F) + PAGE0_BASE)



/****************************************************************************
                     (3/4) REG_PAGE1
 ****************************************************************************/
 #define PAGE1_BASE     (0x0180)
/* �Ĵ���˵����FCP�ж�3�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ3_UNION */
#define SOC_SCHARGER_FCP_IRQ3_ADDR(base)              ((base) + (0x00) + PAGE1_BASE)

/* �Ĵ���˵����FCP�ж�4�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ4_UNION */
#define SOC_SCHARGER_FCP_IRQ4_ADDR(base)              ((base) + (0x01) + PAGE1_BASE)

/* �Ĵ���˵����FCP�ж�����3�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ3_MASK_UNION */
#define SOC_SCHARGER_FCP_IRQ3_MASK_ADDR(base)         ((base) + (0x02) + PAGE1_BASE)

/* �Ĵ���˵����FCP�ж�����4�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_IRQ4_MASK_UNION */
#define SOC_SCHARGER_FCP_IRQ4_MASK_ADDR(base)         ((base) + (0x03) + PAGE1_BASE)

/* �Ĵ���˵����״̬��״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_MSTATE_UNION */
#define SOC_SCHARGER_MSTATE_ADDR(base)                ((base) + (0x04) + PAGE1_BASE)

/* �Ĵ���˵����crc ʹ�ܿ��ƼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_CRC_ENABLE_UNION */
#define SOC_SCHARGER_CRC_ENABLE_ADDR(base)            ((base) + (0x05) + PAGE1_BASE)

/* �Ĵ���˵����crc ��ʼֵ��
   λ����UNION�ṹ:  SOC_SCHARGER_CRC_START_VALUE_UNION */
#define SOC_SCHARGER_CRC_START_VALUE_ADDR(base)       ((base) + (0x06) + PAGE1_BASE)

/* �Ĵ���˵��������������Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_SAMPLE_CNT_ADJ_UNION */
#define SOC_SCHARGER_SAMPLE_CNT_ADJ_ADDR(base)        ((base) + (0x07) + PAGE1_BASE)

/* �Ĵ���˵����RX ping ��С���ȸ�λ��
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PING_WIDTH_MIN_H_UNION */
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_H_ADDR(base)   ((base) + (0x08) + PAGE1_BASE)

/* �Ĵ���˵����RX ping ��С���ȵ�8λ
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PING_WIDTH_MIN_L_UNION */
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_L_ADDR(base)   ((base) + (0x09) + PAGE1_BASE)

/* �Ĵ���˵����RX ping ��󳤶ȸ�λ
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PING_WIDTH_MAX_H_UNION */
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_H_ADDR(base)   ((base) + (0x0A) + PAGE1_BASE)

/* �Ĵ���˵����RX ping ��󳤶ȵ�8λ��
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PING_WIDTH_MAX_L_UNION */
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_L_ADDR(base)   ((base) + (0x0B) + PAGE1_BASE)

/* �Ĵ���˵�������ݵȴ�ʱ��Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_DATA_WAIT_TIME_UNION */
#define SOC_SCHARGER_DATA_WAIT_TIME_ADDR(base)        ((base) + (0x0C) + PAGE1_BASE)

/* �Ĵ���˵�����������·��ʹ�����
   λ����UNION�ṹ:  SOC_SCHARGER_RETRY_CNT_UNION */
#define SOC_SCHARGER_RETRY_CNT_ADDR(base)             ((base) + (0x0D) + PAGE1_BASE)

/* �Ĵ���˵����fcpֻ��Ԥ���Ĵ�����
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_RO_RESERVE_UNION */
#define SOC_SCHARGER_FCP_RO_RESERVE_ADDR(base)        ((base) + (0x0E) + PAGE1_BASE)

/* �Ĵ���˵����FCP debug�Ĵ���0��
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_DEBUG_REG0_UNION */
#define SOC_SCHARGER_FCP_DEBUG_REG0_ADDR(base)        ((base) + (0x0F) + PAGE1_BASE)

/* �Ĵ���˵����FCP debug�Ĵ���1��
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_DEBUG_REG1_UNION */
#define SOC_SCHARGER_FCP_DEBUG_REG1_ADDR(base)        ((base) + (0x10) + PAGE1_BASE)

/* �Ĵ���˵����FCP debug�Ĵ���2��
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_DEBUG_REG2_UNION */
#define SOC_SCHARGER_FCP_DEBUG_REG2_ADDR(base)        ((base) + (0x11) + PAGE1_BASE)

/* �Ĵ���˵����FCP debug�Ĵ���3��
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_DEBUG_REG3_UNION */
#define SOC_SCHARGER_FCP_DEBUG_REG3_ADDR(base)        ((base) + (0x12) + PAGE1_BASE)

/* �Ĵ���˵����FCP debug�Ĵ���4��
   λ����UNION�ṹ:  SOC_SCHARGER_FCP_DEBUG_REG4_UNION */
#define SOC_SCHARGER_FCP_DEBUG_REG4_ADDR(base)        ((base) + (0x13) + PAGE1_BASE)

/* �Ĵ���˵����ACKǰͬ��ͷ�ȴ�΢���ڼĴ�����
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_UNION */
#define SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_ADDR(base) ((base) + (0x14) + PAGE1_BASE)

/* �Ĵ���˵����FCP����΢���Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_SAMPLE_CNT_TINYJUST_UNION */
#define SOC_SCHARGER_SAMPLE_CNT_TINYJUST_ADDR(base)   ((base) + (0x15) + PAGE1_BASE)

/* �Ĵ���˵����FCP���RX PING CNT΢���Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_RX_PING_CNT_TINYJUST_UNION */
#define SOC_SCHARGER_RX_PING_CNT_TINYJUST_ADDR(base)  ((base) + (0x16) + PAGE1_BASE)

/* �Ĵ���˵����SHIFT_CNT���ֵ���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_SHIFT_CNT_CFG_MAX_UNION */
#define SOC_SCHARGER_SHIFT_CNT_CFG_MAX_ADDR(base)     ((base) + (0x17) + PAGE1_BASE)

/* �Ĵ���˵����HKADC_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CFG_REG_0_UNION */
#define SOC_SCHARGER_HKADC_CFG_REG_0_ADDR(base)       ((base) + (0x18) + PAGE1_BASE)

/* �Ĵ���˵����HKADC_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CFG_REG_1_UNION */
#define SOC_SCHARGER_HKADC_CFG_REG_1_ADDR(base)       ((base) + (0x19) + PAGE1_BASE)

/* �Ĵ���˵����HKADC_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_CFG_REG_2_UNION */
#define SOC_SCHARGER_HKADC_CFG_REG_2_ADDR(base)       ((base) + (0x1A) + PAGE1_BASE)

/* �Ĵ���˵����HKADC ����0p1У��ֵ
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_OFFSET_0P1_UNION */
#define SOC_SCHARGER_HKADC_OFFSET_0P1_ADDR(base)      ((base) + (0x1B) + PAGE1_BASE)

/* �Ĵ���˵����HKADC ����2p45У��ֵ
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_OFFSET_2P45_UNION */
#define SOC_SCHARGER_HKADC_OFFSET_2P45_ADDR(base)     ((base) + (0x1C) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ0�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH0_L_UNION */
#define SOC_SCHARGER_SOH_OVH_TH0_L_ADDR(base)         ((base) + (0x1D) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ0�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH0_H_UNION */
#define SOC_SCHARGER_SOH_OVH_TH0_H_ADDR(base)         ((base) + (0x1E) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ0�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH0_L_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH0_L_ADDR(base)       ((base) + (0x1F) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ0�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH0_H_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH0_H_ADDR(base)       ((base) + (0x20) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ1�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH1_L_UNION */
#define SOC_SCHARGER_SOH_OVH_TH1_L_ADDR(base)         ((base) + (0x21) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ1�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH1_H_UNION */
#define SOC_SCHARGER_SOH_OVH_TH1_H_ADDR(base)         ((base) + (0x22) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ1�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH1_L_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH1_L_ADDR(base)       ((base) + (0x23) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ1�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH1_H_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH1_H_ADDR(base)       ((base) + (0x24) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ2�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH2_L_UNION */
#define SOC_SCHARGER_SOH_OVH_TH2_L_ADDR(base)         ((base) + (0x25) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ2�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVH_TH2_H_UNION */
#define SOC_SCHARGER_SOH_OVH_TH2_H_ADDR(base)         ((base) + (0x26) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ2�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH2_L_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH2_L_ADDR(base)       ((base) + (0x27) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ�����¶ȼ�����ֵ2�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_TSBAT_OVH_TH2_H_UNION */
#define SOC_SCHARGER_TSBAT_OVH_TH2_H_ADDR(base)       ((base) + (0x28) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ�ĵ�8bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVL_TH_L_UNION */
#define SOC_SCHARGER_SOH_OVL_TH_L_ADDR(base)          ((base) + (0x29) + PAGE1_BASE)

/* �Ĵ���˵������ع�ѹ������ѹ������ֵ�ĸ�4bit
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVL_TH_H_UNION */
#define SOC_SCHARGER_SOH_OVL_TH_H_ADDR(base)          ((base) + (0x2A) + PAGE1_BASE)

/* �Ĵ���˵����SOH OVP ��/����ֵ���ʵʱ��¼
   λ����UNION�ṹ:  SOC_SCHARGER_SOH_OVP_REAL_UNION */
#define SOC_SCHARGER_SOH_OVP_REAL_ADDR(base)          ((base) + (0x2B) + PAGE1_BASE)

/* �Ĵ���˵����HKADC Testbusʹ�ܼ�ѡ���ź�
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_TB_EN_SEL_UNION */
#define SOC_SCHARGER_HKADC_TB_EN_SEL_ADDR(base)       ((base) + (0x2C) + PAGE1_BASE)

/* �Ĵ���˵����HKADC testbus�ض�����
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_TB_DATA0_UNION */
#define SOC_SCHARGER_HKADC_TB_DATA0_ADDR(base)        ((base) + (0x2D) + PAGE1_BASE)

/* �Ĵ���˵����HKADC testbus�ض�����
   λ����UNION�ṹ:  SOC_SCHARGER_HKADC_TB_DATA1_UNION */
#define SOC_SCHARGER_HKADC_TB_DATA1_ADDR(base)        ((base) + (0x2E) + PAGE1_BASE)

/* �Ĵ���˵����ACR_TOP_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_TOP_CFG_REG_0_UNION */
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_ADDR(base)     ((base) + (0x30) + PAGE1_BASE)

/* �Ĵ���˵����ACR_TOP_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_TOP_CFG_REG_1_UNION */
#define SOC_SCHARGER_ACR_TOP_CFG_REG_1_ADDR(base)     ((base) + (0x31) + PAGE1_BASE)

/* �Ĵ���˵����ACR_TOP_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_TOP_CFG_REG_2_UNION */
#define SOC_SCHARGER_ACR_TOP_CFG_REG_2_ADDR(base)     ((base) + (0x32) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڲ���ʱ�䵵λ
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_SAMPLE_TIME_H_UNION */
#define SOC_SCHARGER_ACR_SAMPLE_TIME_H_ADDR(base)     ((base) + (0x33) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�͵�ƽ�ڲ���ʱ�䵵λ
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_SAMPLE_TIME_L_UNION */
#define SOC_SCHARGER_ACR_SAMPLE_TIME_L_ADDR(base)     ((base) + (0x34) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA0_L_UNION */
#define SOC_SCHARGER_ACR_DATA0_L_ADDR(base)           ((base) + (0x35) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA0_H_UNION */
#define SOC_SCHARGER_ACR_DATA0_H_ADDR(base)           ((base) + (0x36) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA1_L_UNION */
#define SOC_SCHARGER_ACR_DATA1_L_ADDR(base)           ((base) + (0x37) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA1_H_UNION */
#define SOC_SCHARGER_ACR_DATA1_H_ADDR(base)           ((base) + (0x38) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA2_L_UNION */
#define SOC_SCHARGER_ACR_DATA2_L_ADDR(base)           ((base) + (0x39) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA2_H_UNION */
#define SOC_SCHARGER_ACR_DATA2_H_ADDR(base)           ((base) + (0x3A) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA3_L_UNION */
#define SOC_SCHARGER_ACR_DATA3_L_ADDR(base)           ((base) + (0x3B) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA3_H_UNION */
#define SOC_SCHARGER_ACR_DATA3_H_ADDR(base)           ((base) + (0x3C) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ������������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA4_L_UNION */
#define SOC_SCHARGER_ACR_DATA4_L_ADDR(base)           ((base) + (0x3D) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ������������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA4_H_UNION */
#define SOC_SCHARGER_ACR_DATA4_H_ADDR(base)           ((base) + (0x3E) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA5_L_UNION */
#define SOC_SCHARGER_ACR_DATA5_L_ADDR(base)           ((base) + (0x3F) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA5_H_UNION */
#define SOC_SCHARGER_ACR_DATA5_H_ADDR(base)           ((base) + (0x40) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA6_L_UNION */
#define SOC_SCHARGER_ACR_DATA6_L_ADDR(base)           ((base) + (0x41) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA6_H_UNION */
#define SOC_SCHARGER_ACR_DATA6_H_ADDR(base)           ((base) + (0x42) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݵ�8bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA7_L_UNION */
#define SOC_SCHARGER_ACR_DATA7_L_ADDR(base)           ((base) + (0x43) + PAGE1_BASE)

/* �Ĵ���˵������n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݸ�4bit��
   λ����UNION�ṹ:  SOC_SCHARGER_ACR_DATA7_H_UNION */
#define SOC_SCHARGER_ACR_DATA7_H_ADDR(base)           ((base) + (0x44) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_UNION */
#define SOC_SCHARGER_IRQ_MASK_ADDR(base)              ((base) + (0x48) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_0_UNION */
#define SOC_SCHARGER_IRQ_MASK_0_ADDR(base)            ((base) + (0x49) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_1_UNION */
#define SOC_SCHARGER_IRQ_MASK_1_ADDR(base)            ((base) + (0x4A) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_2_UNION */
#define SOC_SCHARGER_IRQ_MASK_2_ADDR(base)            ((base) + (0x4B) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_3_UNION */
#define SOC_SCHARGER_IRQ_MASK_3_ADDR(base)            ((base) + (0x4C) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_4_UNION */
#define SOC_SCHARGER_IRQ_MASK_4_ADDR(base)            ((base) + (0x4D) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_5_UNION */
#define SOC_SCHARGER_IRQ_MASK_5_ADDR(base)            ((base) + (0x4E) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_6_UNION */
#define SOC_SCHARGER_IRQ_MASK_6_ADDR(base)            ((base) + (0x4F) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_MASK_7_UNION */
#define SOC_SCHARGER_IRQ_MASK_7_ADDR(base)            ((base) + (0x50) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_0_UNION */
#define SOC_SCHARGER_IRQ_STATUS_0_ADDR(base)          ((base) + (0x51) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_1_UNION */
#define SOC_SCHARGER_IRQ_STATUS_1_ADDR(base)          ((base) + (0x52) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_2_UNION */
#define SOC_SCHARGER_IRQ_STATUS_2_ADDR(base)          ((base) + (0x53) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_3_UNION */
#define SOC_SCHARGER_IRQ_STATUS_3_ADDR(base)          ((base) + (0x54) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_4_UNION */
#define SOC_SCHARGER_IRQ_STATUS_4_ADDR(base)          ((base) + (0x55) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_5_UNION */
#define SOC_SCHARGER_IRQ_STATUS_5_ADDR(base)          ((base) + (0x56) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_6_UNION */
#define SOC_SCHARGER_IRQ_STATUS_6_ADDR(base)          ((base) + (0x57) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_7_UNION */
#define SOC_SCHARGER_IRQ_STATUS_7_ADDR(base)          ((base) + (0x58) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_IRQ_STATUS_8_UNION */
#define SOC_SCHARGER_IRQ_STATUS_8_ADDR(base)          ((base) + (0x59) + PAGE1_BASE)

/* �Ĵ���˵����EFUSEѡ���ź�
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_SEL_UNION */
#define SOC_SCHARGER_EFUSE_SEL_ADDR(base)             ((base) + (0x60) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_CFG_0_UNION */
#define SOC_SCHARGER_EFUSE_CFG_0_ADDR(base)           ((base) + (0x61) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_0_UNION */
#define SOC_SCHARGER_EFUSE_WE_0_ADDR(base)            ((base) + (0x62) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_1_UNION */
#define SOC_SCHARGER_EFUSE_WE_1_ADDR(base)            ((base) + (0x63) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_2_UNION */
#define SOC_SCHARGER_EFUSE_WE_2_ADDR(base)            ((base) + (0x64) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_3_UNION */
#define SOC_SCHARGER_EFUSE_WE_3_ADDR(base)            ((base) + (0x65) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_4_UNION */
#define SOC_SCHARGER_EFUSE_WE_4_ADDR(base)            ((base) + (0x66) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_5_UNION */
#define SOC_SCHARGER_EFUSE_WE_5_ADDR(base)            ((base) + (0x67) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_6_UNION */
#define SOC_SCHARGER_EFUSE_WE_6_ADDR(base)            ((base) + (0x68) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_WE_7_UNION */
#define SOC_SCHARGER_EFUSE_WE_7_ADDR(base)            ((base) + (0x69) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_UNION */
#define SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_ADDR(base) ((base) + (0x6A) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_UNION */
#define SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_ADDR(base)  ((base) + (0x6B) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE1_TESTBUS_0_UNION */
#define SOC_SCHARGER_EFUSE1_TESTBUS_0_ADDR(base)      ((base) + (0x6C) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE2_TESTBUS_0_UNION */
#define SOC_SCHARGER_EFUSE2_TESTBUS_0_ADDR(base)      ((base) + (0x6D) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE3_TESTBUS_0_UNION */
#define SOC_SCHARGER_EFUSE3_TESTBUS_0_ADDR(base)      ((base) + (0x6E) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_TESTBUS_SEL_UNION */
#define SOC_SCHARGER_EFUSE_TESTBUS_SEL_ADDR(base)     ((base) + (0x6F) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE_TESTBUS_CFG_UNION */
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_ADDR(base)     ((base) + (0x70) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_UNION */
#define SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_ADDR(base)  ((base) + (0x71) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_UNION */
#define SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_ADDR(base)  ((base) + (0x72) + PAGE1_BASE)

/* �Ĵ���˵����
   λ����UNION�ṹ:  SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_UNION */
#define SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_ADDR(base)  ((base) + (0x73) + PAGE1_BASE)

/* �Ĵ���˵��������ģ������ź����üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_GLB_TESTBUS_CFG_UNION */
#define SOC_SCHARGER_GLB_TESTBUS_CFG_ADDR(base)       ((base) + (0x74) + PAGE1_BASE)

/* �Ĵ���˵��������ģ������ź�
   λ����UNION�ṹ:  SOC_SCHARGER_GLB_TEST_DATA_UNION */
#define SOC_SCHARGER_GLB_TEST_DATA_ADDR(base)         ((base) + (0x75) + PAGE1_BASE)



/****************************************************************************
                     (4/4) REG_PAGE2
 ****************************************************************************/
 #define PAGE2_BASE  (0x0280)
/* �Ĵ���˵����CHARGER_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_0_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_0_ADDR(base)     ((base) + (0x00) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_1_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_1_ADDR(base)     ((base) + (0x01) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_2_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_2_ADDR(base)     ((base) + (0x02) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_3_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_3_ADDR(base)     ((base) + (0x03) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_4_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_4_ADDR(base)     ((base) + (0x04) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_5_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_5_ADDR(base)     ((base) + (0x05) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_6
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_6_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_6_ADDR(base)     ((base) + (0x06) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_7
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_7_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_7_ADDR(base)     ((base) + (0x07) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_8
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_8_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_8_ADDR(base)     ((base) + (0x08) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_���üĴ���_9
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_CFG_REG_9_UNION */
#define SOC_SCHARGER_CHARGER_CFG_REG_9_ADDR(base)     ((base) + (0x09) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_ֻ���Ĵ���_10
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_RO_REG_10_UNION */
#define SOC_SCHARGER_CHARGER_RO_REG_10_ADDR(base)     ((base) + (0x0A) + PAGE2_BASE)

/* �Ĵ���˵����CHARGER_ֻ���Ĵ���_11
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_RO_REG_11_UNION */
#define SOC_SCHARGER_CHARGER_RO_REG_11_ADDR(base)     ((base) + (0x0B) + PAGE2_BASE)

/* �Ĵ���˵����USB_OVP_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_USB_OVP_CFG_REG_0_UNION */
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_ADDR(base)     ((base) + (0x0C) + PAGE2_BASE)

/* �Ĵ���˵����USB_OVP_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_USB_OVP_CFG_REG_1_UNION */
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_ADDR(base)     ((base) + (0x0D) + PAGE2_BASE)

/* �Ĵ���˵����TCPC_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_TCPC_CFG_REG_1_UNION */
#define SOC_SCHARGER_TCPC_CFG_REG_1_ADDR(base)        ((base) + (0x0E) + PAGE2_BASE)

/* �Ĵ���˵����TCPC_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_TCPC_CFG_REG_2_UNION */
#define SOC_SCHARGER_TCPC_CFG_REG_2_ADDR(base)        ((base) + (0x0F) + PAGE2_BASE)

/* �Ĵ���˵����TCPC_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_TCPC_CFG_REG_3_UNION */
#define SOC_SCHARGER_TCPC_CFG_REG_3_ADDR(base)        ((base) + (0x10) + PAGE2_BASE)

/* �Ĵ���˵����TCPC_ֻ���Ĵ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_TCPC_RO_REG_5_UNION */
#define SOC_SCHARGER_TCPC_RO_REG_5_ADDR(base)         ((base) + (0x11) + PAGE2_BASE)

/* �Ĵ���˵����SCHG_LOGIC_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_UNION */
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_ADDR(base)  ((base) + (0x12) + PAGE2_BASE)

/* �Ĵ���˵����SCHG_LOGIC_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_UNION */
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_ADDR(base)  ((base) + (0x13) + PAGE2_BASE)

/* �Ĵ���˵����SCHG_LOGIC_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_UNION */
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_ADDR(base)  ((base) + (0x14) + PAGE2_BASE)

/* �Ĵ���˵����SCHG_LOGIC_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_UNION */
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_ADDR(base)  ((base) + (0x15) + PAGE2_BASE)

/* �Ĵ���˵����SCHG_LOGIC_ֻ���Ĵ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_UNION */
#define SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_ADDR(base)   ((base) + (0x16) + PAGE2_BASE)

/* �Ĵ���˵����BATFET���üĴ���
   λ����UNION�ṹ:  SOC_SCHARGER_CHARGER_BATFET_CTRL_UNION */
#define SOC_SCHARGER_CHARGER_BATFET_CTRL_ADDR(base)   ((base) + (0x17) + PAGE2_BASE)

/* �Ĵ���˵����VBAT LV�Ĵ���
   λ����UNION�ṹ:  SOC_SCHARGER_VBAT_LV_REG_UNION */
#define SOC_SCHARGER_VBAT_LV_REG_ADDR(base)           ((base) + (0x18) + PAGE2_BASE)

/* �Ĵ���˵����VDM����ģʽ�����üĴ���0
   λ����UNION�ṹ:  SOC_SCHARGER_VDM_CFG_REG_0_UNION */
#define SOC_SCHARGER_VDM_CFG_REG_0_ADDR(base)         ((base) + (0x1A) + PAGE2_BASE)

/* �Ĵ���˵����VDM����ģʽ�����üĴ���1
   λ����UNION�ṹ:  SOC_SCHARGER_VDM_CFG_REG_1_UNION */
#define SOC_SCHARGER_VDM_CFG_REG_1_ADDR(base)         ((base) + (0x1B) + PAGE2_BASE)

/* �Ĵ���˵����VDM����ģʽ�����üĴ���2
   λ����UNION�ṹ:  SOC_SCHARGER_VDM_CFG_REG_2_UNION */
#define SOC_SCHARGER_VDM_CFG_REG_2_ADDR(base)         ((base) + (0x1C) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_0_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_ADDR(base)     ((base) + (0x20) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_1_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_ADDR(base)     ((base) + (0x21) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_2_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_2_ADDR(base)     ((base) + (0x22) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_3_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_3_ADDR(base)     ((base) + (0x23) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_4_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_ADDR(base)     ((base) + (0x24) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_5_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_ADDR(base)     ((base) + (0x25) + PAGE2_BASE)

/* �Ĵ���˵����DET_TOP_���üĴ���_6
   λ����UNION�ṹ:  SOC_SCHARGER_DET_TOP_CFG_REG_6_UNION */
#define SOC_SCHARGER_DET_TOP_CFG_REG_6_ADDR(base)     ((base) + (0x26) + PAGE2_BASE)

/* �Ĵ���˵����PSEL_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_PSEL_CFG_REG_0_UNION */
#define SOC_SCHARGER_PSEL_CFG_REG_0_ADDR(base)        ((base) + (0x27) + PAGE2_BASE)

/* �Ĵ���˵����PSEL_ֻ���Ĵ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_PSEL_RO_REG_1_UNION */
#define SOC_SCHARGER_PSEL_RO_REG_1_ADDR(base)         ((base) + (0x28) + PAGE2_BASE)

/* �Ĵ���˵����REF_TOP_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_REF_TOP_CFG_REG_0_UNION */
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_ADDR(base)     ((base) + (0x29) + PAGE2_BASE)

/* �Ĵ���˵����REF_TOP_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_REF_TOP_CFG_REG_1_UNION */
#define SOC_SCHARGER_REF_TOP_CFG_REG_1_ADDR(base)     ((base) + (0x2A) + PAGE2_BASE)

/* �Ĵ���˵����REF_TOP_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_REF_TOP_CFG_REG_2_UNION */
#define SOC_SCHARGER_REF_TOP_CFG_REG_2_ADDR(base)     ((base) + (0x2B) + PAGE2_BASE)

/* �Ĵ���˵����REF_TOP_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_REF_TOP_CFG_REG_3_UNION */
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_ADDR(base)     ((base) + (0x2C) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_0_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_ADDR(base)      ((base) + (0x30) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_1_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_ADDR(base)      ((base) + (0x31) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_2_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_2_ADDR(base)      ((base) + (0x32) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_3_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_3_ADDR(base)      ((base) + (0x33) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_4_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_4_ADDR(base)      ((base) + (0x34) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_5_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_5_ADDR(base)      ((base) + (0x35) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_6
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_6_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_6_ADDR(base)      ((base) + (0x36) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_7
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_7_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_ADDR(base)      ((base) + (0x37) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_8
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_8_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_ADDR(base)      ((base) + (0x38) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_9
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_9_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_9_ADDR(base)      ((base) + (0x39) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_10
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_10_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_ADDR(base)     ((base) + (0x3A) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_11
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_11_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_ADDR(base)     ((base) + (0x3B) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_12
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_12_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_ADDR(base)     ((base) + (0x3C) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_13
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_13_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_13_ADDR(base)     ((base) + (0x3D) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_14
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_14_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_14_ADDR(base)     ((base) + (0x3E) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_15
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_15_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_15_ADDR(base)     ((base) + (0x3F) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_16
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_16_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_16_ADDR(base)     ((base) + (0x40) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_17
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_17_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_ADDR(base)     ((base) + (0x41) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_18
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_18_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_ADDR(base)     ((base) + (0x42) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_19
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_19_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_ADDR(base)     ((base) + (0x43) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_20
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_20_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_ADDR(base)     ((base) + (0x44) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_21
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_21_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_ADDR(base)     ((base) + (0x45) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_���üĴ���_22
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_CFG_REG_22_UNION */
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_ADDR(base)     ((base) + (0x46) + PAGE2_BASE)

/* �Ĵ���˵����DC_TOP_ֻ���Ĵ���_23
   λ����UNION�ṹ:  SOC_SCHARGER_DC_TOP_RO_REG_23_UNION */
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ADDR(base)      ((base) + (0x47) + PAGE2_BASE)

/* �Ĵ���˵����VERSION0_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION0_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION0_RO_REG_0_ADDR(base)     ((base) + (0x49) + PAGE2_BASE)

/* �Ĵ���˵����VERSION1_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION1_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION1_RO_REG_0_ADDR(base)     ((base) + (0x4A) + PAGE2_BASE)

/* �Ĵ���˵����VERSION2_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION2_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION2_RO_REG_0_ADDR(base)     ((base) + (0x4B) + PAGE2_BASE)

/* �Ĵ���˵����VERSION3_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION3_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION3_RO_REG_0_ADDR(base)     ((base) + (0x4C) + PAGE2_BASE)

/* �Ĵ���˵����VERSION4_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION4_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION4_RO_REG_0_ADDR(base)     ((base) + (0x4D) + PAGE2_BASE)

/* �Ĵ���˵����VERSION5_ֻ���Ĵ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_VERSION5_RO_REG_0_UNION */
#define SOC_SCHARGER_VERSION5_RO_REG_0_ADDR(base)     ((base) + (0x4E) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_0_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_0_ADDR(base)        ((base) + (0x50) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_1_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_1_ADDR(base)        ((base) + (0x51) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_2_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_2_ADDR(base)        ((base) + (0x52) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_3_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_3_ADDR(base)        ((base) + (0x53) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_4_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_4_ADDR(base)        ((base) + (0x54) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_5_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_5_ADDR(base)        ((base) + (0x55) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_6
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_6_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_6_ADDR(base)        ((base) + (0x56) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_7
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_7_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_7_ADDR(base)        ((base) + (0x57) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_8
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_8_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_8_ADDR(base)        ((base) + (0x58) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_9
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_9_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_9_ADDR(base)        ((base) + (0x59) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_10
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_10_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_10_ADDR(base)       ((base) + (0x5A) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_11
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_11_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_11_ADDR(base)       ((base) + (0x5B) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_12
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_12_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_12_ADDR(base)       ((base) + (0x5C) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_13
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_13_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_13_ADDR(base)       ((base) + (0x5D) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_14
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_14_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_14_ADDR(base)       ((base) + (0x5E) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_15
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_15_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_15_ADDR(base)       ((base) + (0x5F) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_16
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_16_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_16_ADDR(base)       ((base) + (0x60) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_17
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_17_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_17_ADDR(base)       ((base) + (0x61) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_18
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_18_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_18_ADDR(base)       ((base) + (0x62) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_19
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_19_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_19_ADDR(base)       ((base) + (0x63) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_20
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_20_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_20_ADDR(base)       ((base) + (0x64) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_21
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_21_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_21_ADDR(base)       ((base) + (0x65) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_22
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_22_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_22_ADDR(base)       ((base) + (0x66) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_23
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_23_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_23_ADDR(base)       ((base) + (0x67) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_24
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_24_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_24_ADDR(base)       ((base) + (0x68) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_���üĴ���_25
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_CFG_REG_25_UNION */
#define SOC_SCHARGER_BUCK_CFG_REG_25_ADDR(base)       ((base) + (0x69) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_ֻ���Ĵ���_26
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_RO_REG_26_UNION */
#define SOC_SCHARGER_BUCK_RO_REG_26_ADDR(base)        ((base) + (0x6A) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_ֻ���Ĵ���_27
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_RO_REG_27_UNION */
#define SOC_SCHARGER_BUCK_RO_REG_27_ADDR(base)        ((base) + (0x6B) + PAGE2_BASE)

/* �Ĵ���˵����BUCK_ֻ���Ĵ���_28
   λ����UNION�ṹ:  SOC_SCHARGER_BUCK_RO_REG_28_UNION */
#define SOC_SCHARGER_BUCK_RO_REG_28_ADDR(base)        ((base) + (0x6C) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_0
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_0_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_0_ADDR(base)         ((base) + (0x6D) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_1
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_1_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_1_ADDR(base)         ((base) + (0x6E) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_2
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_2_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_2_ADDR(base)         ((base) + (0x6F) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_3
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_3_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_3_ADDR(base)         ((base) + (0x70) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_4
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_4_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_4_ADDR(base)         ((base) + (0x71) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_5
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_5_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_5_ADDR(base)         ((base) + (0x72) + PAGE2_BASE)

/* �Ĵ���˵����OTG_���üĴ���_6
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_CFG_REG_6_UNION */
#define SOC_SCHARGER_OTG_CFG_REG_6_ADDR(base)         ((base) + (0x73) + PAGE2_BASE)

/* �Ĵ���˵����OTG_ֻ���Ĵ���_7
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_RO_REG_7_UNION */
#define SOC_SCHARGER_OTG_RO_REG_7_ADDR(base)          ((base) + (0x74) + PAGE2_BASE)

/* �Ĵ���˵����OTG_ֻ���Ĵ���_8
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_RO_REG_8_UNION */
#define SOC_SCHARGER_OTG_RO_REG_8_ADDR(base)          ((base) + (0x75) + PAGE2_BASE)

/* �Ĵ���˵����OTG_ֻ���Ĵ���_9
   λ����UNION�ṹ:  SOC_SCHARGER_OTG_RO_REG_9_UNION */
#define SOC_SCHARGER_OTG_RO_REG_9_ADDR(base)          ((base) + (0x76) + PAGE2_BASE)





/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/



/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/



/*****************************************************************************
  6 STRUCT����
*****************************************************************************/



/*****************************************************************************
  7 UNION����
*****************************************************************************/

/****************************************************************************
                     (1/4) REG_PD
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VENDIDL_UNION
 �ṹ˵��  : VENDIDL �Ĵ����ṹ���塣��ַƫ����:0x00����ֵ:0xD1�����:8
 �Ĵ���˵��: Vendor ID Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vend_id_l : 8;  /* bit[0-7]: Vendor ID��8���� */
    } reg;
} SOC_SCHARGER_VENDIDL_UNION;
#endif
#define SOC_SCHARGER_VENDIDL_pd_vend_id_l_START  (0)
#define SOC_SCHARGER_VENDIDL_pd_vend_id_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VENDIDH_UNION
 �ṹ˵��  : VENDIDH �Ĵ����ṹ���塣��ַƫ����:0x01����ֵ:0x12�����:8
 �Ĵ���˵��: Vendor ID High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vend_id_h : 8;  /* bit[0-7]: Vendor ID��8���� */
    } reg;
} SOC_SCHARGER_VENDIDH_UNION;
#endif
#define SOC_SCHARGER_VENDIDH_pd_vend_id_h_START  (0)
#define SOC_SCHARGER_VENDIDH_pd_vend_id_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PRODIDL_UNION
 �ṹ˵��  : PRODIDL �Ĵ����ṹ���塣��ַƫ����:0x02����ֵ:0x26�����:8
 �Ĵ���˵��: Product ID Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_prod_id_l : 8;  /* bit[0-7]: Product ID��8���� */
    } reg;
} SOC_SCHARGER_PRODIDL_UNION;
#endif
#define SOC_SCHARGER_PRODIDL_pd_prod_id_l_START  (0)
#define SOC_SCHARGER_PRODIDL_pd_prod_id_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PRODIDH_UNION
 �ṹ˵��  : PRODIDH �Ĵ����ṹ���塣��ַƫ����:0x03����ֵ:0x65�����:8
 �Ĵ���˵��: Product ID High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_prod_id_h : 8;  /* bit[0-7]: Product ID��8���� */
    } reg;
} SOC_SCHARGER_PRODIDH_UNION;
#endif
#define SOC_SCHARGER_PRODIDH_pd_prod_id_h_START  (0)
#define SOC_SCHARGER_PRODIDH_pd_prod_id_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DEVIDL_UNION
 �ṹ˵��  : DEVIDL �Ĵ����ṹ���塣��ַƫ����:0x04����ֵ:0x00�����:8
 �Ĵ���˵��: Device ID Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_id_l : 8;  /* bit[0-7]: Device ID��8���� */
    } reg;
} SOC_SCHARGER_DEVIDL_UNION;
#endif
#define SOC_SCHARGER_DEVIDL_pd_dev_id_l_START  (0)
#define SOC_SCHARGER_DEVIDL_pd_dev_id_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DEVIDH_UNION
 �ṹ˵��  : DEVIDH �Ĵ����ṹ���塣��ַƫ����:0x05����ֵ:0x01�����:8
 �Ĵ���˵��: Device ID High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_id_h : 8;  /* bit[0-7]: Device ID��8���� */
    } reg;
} SOC_SCHARGER_DEVIDH_UNION;
#endif
#define SOC_SCHARGER_DEVIDH_pd_dev_id_h_START  (0)
#define SOC_SCHARGER_DEVIDH_pd_dev_id_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TYPECREVL_UNION
 �ṹ˵��  : TYPECREVL �Ĵ����ṹ���塣��ַƫ����:0x06����ֵ:0x12�����:8
 �Ĵ���˵��: USB Type-C Revision Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_typc_revision_l : 8;  /* bit[0-7]: Type-C Version��8���أ�Reference to SuperSwitch�� */
    } reg;
} SOC_SCHARGER_TYPECREVL_UNION;
#endif
#define SOC_SCHARGER_TYPECREVL_pd_typc_revision_l_START  (0)
#define SOC_SCHARGER_TYPECREVL_pd_typc_revision_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TYPECREVH_UNION
 �ṹ˵��  : TYPECREVH �Ĵ����ṹ���塣��ַƫ����:0x07����ֵ:0x00�����:8
 �Ĵ���˵��: USB Type-C Revision High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_typc_revision_h : 8;  /* bit[0-7]: Type-C Version��8���� */
    } reg;
} SOC_SCHARGER_TYPECREVH_UNION;
#endif
#define SOC_SCHARGER_TYPECREVH_pd_typc_revision_h_START  (0)
#define SOC_SCHARGER_TYPECREVH_pd_typc_revision_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_USBPDVER_UNION
 �ṹ˵��  : USBPDVER �Ĵ����ṹ���塣��ַƫ����:0x08����ֵ:0x10�����:8
 �Ĵ���˵��: USB PD Version
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_version : 8;  /* bit[0-7]: USB PD Version */
    } reg;
} SOC_SCHARGER_USBPDVER_UNION;
#endif
#define SOC_SCHARGER_USBPDVER_pd_version_START  (0)
#define SOC_SCHARGER_USBPDVER_pd_version_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_USBPDREV_UNION
 �ṹ˵��  : USBPDREV �Ĵ����ṹ���塣��ַƫ����:0x09����ֵ:0x30�����:8
 �Ĵ���˵��: USB PD Revision
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_revision : 8;  /* bit[0-7]: USB PD Revision */
    } reg;
} SOC_SCHARGER_USBPDREV_UNION;
#endif
#define SOC_SCHARGER_USBPDREV_pd_revision_START  (0)
#define SOC_SCHARGER_USBPDREV_pd_revision_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PDIFREVL_UNION
 �ṹ˵��  : PDIFREVL �Ĵ����ṹ���塣��ַƫ����:0x0A����ֵ:0x10�����:8
 �Ĵ���˵��: USB PD Interface Revision Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_itf_revision_l : 8;  /* bit[0-7]: USB PD Interface Revision��8���� */
    } reg;
} SOC_SCHARGER_PDIFREVL_UNION;
#endif
#define SOC_SCHARGER_PDIFREVL_pd_itf_revision_l_START  (0)
#define SOC_SCHARGER_PDIFREVL_pd_itf_revision_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PDIFREVH_UNION
 �ṹ˵��  : PDIFREVH �Ĵ����ṹ���塣��ַƫ����:0x0B����ֵ:0x10�����:8
 �Ĵ���˵��: USB PD Interface Revision High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_itf_revision_h : 8;  /* bit[0-7]: USB PD Interface Revision��8���� */
    } reg;
} SOC_SCHARGER_PDIFREVH_UNION;
#endif
#define SOC_SCHARGER_PDIFREVH_pd_itf_revision_h_START  (0)
#define SOC_SCHARGER_PDIFREVH_pd_itf_revision_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_ALERT_L_UNION
 �ṹ˵��  : PD_ALERT_L �Ĵ����ṹ���塣��ַƫ����:0x10����ֵ:0x00�����:8
 �Ĵ���˵��: PD Alert Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_int_ccstatus    : 1;  /* bit[0-0]: CC1/CC2״̬�����仯
                                                             1b:CC Status�����仯�ж�
                                                             0b:û���жϷ���
                                                             ע�⣺�����CC1/CC2״̬�����仯ָ����PD_CC_STATUS��pd_cc1_stat/pd_cc2_stat����pd_look4con��pd_con_result�ı仯���ᴥ���ж�
                                                             Ĭ��0 */
        unsigned char  pd_int_port_pwr    : 1;  /* bit[1-1]: Power Status��PD_PWR_STATUS�Ĵ����������仯�ж�
                                                             1b:Power Status�����仯�ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_rxstat      : 1;  /* bit[2-2]: RXͨ�����յ�SOP* Message�жϡ�
                                                             1b:RX���յ�SOP* Msg�ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_rxhardrst   : 1;  /* bit[3-3]: RXͨ�����յ�Hard Reset�жϡ�
                                                             1b:RXͨ�����յ�Hard Reset�жϷ���
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_txfail      : 1;  /* bit[4-4]: TXͨ������ʧ�ܣ���CRCReceiveʱ����û���յ���Ч��GoodCRC��
                                                             1b:TXͨ������ʧ���ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_txdisc      : 1;  /* bit[5-5]: TXͨ�����ͱ�����������������ڷ������ݣ������ڽ������ݣ���TX�ɹ���ʧ�ܣ������ж�û�����㣻��RX�ɹ���RX Hard Reset�ж�û�����㣻����ʱ��PD_TXBYTECNTΪ0��ʱ�򴥷�TRANSMIT���������ݻᱻ������

                                                             1b:TXͨ�����ͱ������ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_txsucc      : 1;  /* bit[6-6]: TXͨ�����ͳɹ�
                                                             1b:TXͨ�����ͳɹ��ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
        unsigned char  pd_int_vbus_alrm_h : 1;  /* bit[7-7]: Vbus�ߵ�ѹ�ж�
                                                             1b:����Vbus�ߵ�ѹ�ж�
                                                             0b:û���жϷ���
                                                             Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_ALERT_L_UNION;
#endif
#define SOC_SCHARGER_PD_ALERT_L_pd_int_ccstatus_START     (0)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_ccstatus_END       (0)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_port_pwr_START     (1)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_port_pwr_END       (1)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_rxstat_START       (2)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_rxstat_END         (2)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_rxhardrst_START    (3)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_rxhardrst_END      (3)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txfail_START       (4)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txfail_END         (4)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txdisc_START       (5)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txdisc_END         (5)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txsucc_START       (6)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_txsucc_END         (6)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_vbus_alrm_h_START  (7)
#define SOC_SCHARGER_PD_ALERT_L_pd_int_vbus_alrm_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_ALERT_H_UNION
 �ṹ˵��  : PD_ALERT_H �Ĵ����ṹ���塣��ַƫ����:0x11����ֵ:0x00�����:8
 �Ĵ���˵��: PD Alert High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_int_vbus_alrm_l   : 1;  /* bit[0-0]: Vbus�͵�ѹ�ж�
                                                               1b:����Vbus�͵�ѹ�ж�
                                                               0b:û���жϷ���
                                                               Ĭ��0 */
        unsigned char  pd_int_fault         : 1;  /* bit[1-1]: �д�������Fault��Ҫ�ض�Fault Status�Ĵ���ȥ�жϣ�
                                                               1b:�������жϣ���Ҫ�ض�PD_FAULT_STATUS�Ĵ����жϴ������͡�
                                                               0b:û���жϷ���
                                                               Ĭ��0 */
        unsigned char  pd_int_rx_full       : 1;  /* bit[2-2]: RXͨ������жϡ�
                                                               1b:RX buffer����д�����������󲻻��GoodCRC���Զˡ�
                                                               0b:û���жϷ���
                                                               Ĭ��0
                                                               ע�⣺�Ըñ��ؽ���д1��������ñ��ء���ҪдPD_ALERT_H�Ĵ�����pd_int_rxstatΪ1��������ñ���Ϊ0��
                                                               Ĭ��0 */
        unsigned char  pd_int_vbus_snk_disc : 1;  /* bit[3-3]: Vbus Sink�γ��жϡ�
                                                               1b:Vbus Sink�γ��жϷ���
                                                               0b:û���жϷ���
                                                               Ĭ��0 */
        unsigned char  reserved             : 3;  /* bit[4-6]: reserved */
        unsigned char  pd_int_fr_swap       : 1;  /* bit[7-7]: Fast Role Swap�ж�
                                                               1b:����Fast Role Swap�жϣ�
                                                               0b:û���жϷ���
                                                               Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_ALERT_H_UNION;
#endif
#define SOC_SCHARGER_PD_ALERT_H_pd_int_vbus_alrm_l_START    (0)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_vbus_alrm_l_END      (0)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_fault_START          (1)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_fault_END            (1)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_rx_full_START        (2)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_rx_full_END          (2)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_vbus_snk_disc_START  (3)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_vbus_snk_disc_END    (3)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_fr_swap_START        (7)
#define SOC_SCHARGER_PD_ALERT_H_pd_int_fr_swap_END          (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_ALERT_MSK_L_UNION
 �ṹ˵��  : PD_ALERT_MSK_L �Ĵ����ṹ���塣��ַƫ����:0x12����ֵ:0xFF�����:8
 �Ĵ���˵��: PD Alert Mask Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_msk_ccstatus    : 1;  /* bit[0-0]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_port_pwr    : 1;  /* bit[1-1]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_rxstat      : 1;  /* bit[2-2]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_rxhardrst   : 1;  /* bit[3-3]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_txfail      : 1;  /* bit[4-4]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_txdisc      : 1;  /* bit[5-5]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_txsucc      : 1;  /* bit[6-6]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_vbus_alrm_h : 1;  /* bit[7-7]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
    } reg;
} SOC_SCHARGER_PD_ALERT_MSK_L_UNION;
#endif
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_ccstatus_START     (0)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_ccstatus_END       (0)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_port_pwr_START     (1)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_port_pwr_END       (1)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_rxstat_START       (2)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_rxstat_END         (2)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_rxhardrst_START    (3)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_rxhardrst_END      (3)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txfail_START       (4)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txfail_END         (4)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txdisc_START       (5)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txdisc_END         (5)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txsucc_START       (6)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_txsucc_END         (6)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_vbus_alrm_h_START  (7)
#define SOC_SCHARGER_PD_ALERT_MSK_L_pd_msk_vbus_alrm_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_ALERT_MSK_H_UNION
 �ṹ˵��  : PD_ALERT_MSK_H �Ĵ����ṹ���塣��ַƫ����:0x13����ֵ:0x0F�����:8
 �Ĵ���˵��: PD Alert Mask High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_msk_vbus_alrm_l   : 1;  /* bit[0-0]: 1b: Interrupt unmasked
                                                               0b: Interrupt masked
                                                               Default: 1b */
        unsigned char  pd_msk_fault         : 1;  /* bit[1-1]: 1b: Interrupt unmasked
                                                               0b: Interrupt masked
                                                               Default: 1b */
        unsigned char  pd_msk_rx_full       : 1;  /* bit[2-2]: 1b: Interrupt unmasked
                                                               0b: Interrupt masked
                                                               Default: 1b */
        unsigned char  pd_msk_vbus_snk_disc : 1;  /* bit[3-3]: 1b: Interrupt unmasked
                                                               0b: Interrupt masked
                                                               Default: 1b */
        unsigned char  reserved             : 3;  /* bit[4-6]: reserved */
        unsigned char  pd_msk_fr_swap       : 1;  /* bit[7-7]: 1b: Interrupt unmasked
                                                               0b: Interrupt masked
                                                               Default: 0 */
    } reg;
} SOC_SCHARGER_PD_ALERT_MSK_H_UNION;
#endif
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_vbus_alrm_l_START    (0)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_vbus_alrm_l_END      (0)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_fault_START          (1)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_fault_END            (1)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_rx_full_START        (2)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_rx_full_END          (2)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_vbus_snk_disc_START  (3)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_vbus_snk_disc_END    (3)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_fr_swap_START        (7)
#define SOC_SCHARGER_PD_ALERT_MSK_H_pd_msk_fr_swap_END          (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_PWRSTAT_MSK_UNION
 �ṹ˵��  : PD_PWRSTAT_MSK �Ĵ����ṹ���塣��ַƫ����:0x14����ֵ:0xDF�����:8
 �Ĵ���˵��: PD Power State Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_msk_snkvbus     : 1;  /* bit[0-0]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_vconn_vld   : 1;  /* bit[1-1]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_vbus_vld    : 1;  /* bit[2-2]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_vbus_vld_en : 1;  /* bit[3-3]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_src_vbus    : 1;  /* bit[4-4]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  reserved           : 1;  /* bit[5-5]: reserved */
        unsigned char  pd_msk_init_stat   : 1;  /* bit[6-6]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
        unsigned char  pd_msk_debug_acc   : 1;  /* bit[7-7]: 1b: Interrupt unmasked
                                                             0b: Interrupt masked
                                                             Default: 1b */
    } reg;
} SOC_SCHARGER_PD_PWRSTAT_MSK_UNION;
#endif
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_snkvbus_START      (0)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_snkvbus_END        (0)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vconn_vld_START    (1)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vconn_vld_END      (1)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vbus_vld_START     (2)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vbus_vld_END       (2)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vbus_vld_en_START  (3)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_vbus_vld_en_END    (3)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_src_vbus_START     (4)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_src_vbus_END       (4)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_init_stat_START    (6)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_init_stat_END      (6)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_debug_acc_START    (7)
#define SOC_SCHARGER_PD_PWRSTAT_MSK_pd_msk_debug_acc_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_FAULTSTAT_MSK_UNION
 �ṹ˵��  : PD_FAULTSTAT_MSK �Ĵ����ṹ���塣��ַƫ����:0x15����ֵ:0xF7�����:8
 �Ĵ���˵��: PD Fault State Mask
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_msk_i2c_err          : 1;  /* bit[0-0]: 1b: Interrupt unmasked
                                                                  0b: Interrupt masked
                                                                  Default: 1b */
        unsigned char  pd_msk_vconn_ocp        : 1;  /* bit[1-1]: no use */
        unsigned char  pd_msk_vbus_ovp         : 1;  /* bit[2-2]: no use */
        unsigned char  reserved_0              : 1;  /* bit[3-3]: no use */
        unsigned char  pd_msk_force_disch_fail : 1;  /* bit[4-4]: 1b: Interrupt unmasked
                                                                  0b: Interrupt masked
                                                                  Default: 1b */
        unsigned char  pd_msk_auto_disch_fail  : 1;  /* bit[5-5]: 1b: Interrupt unmasked
                                                                  0b: Interrupt masked
                                                                  Default: 1b */
        unsigned char  reserved_1              : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_FAULTSTAT_MSK_UNION;
#endif
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_i2c_err_START           (0)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_i2c_err_END             (0)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_vconn_ocp_START         (1)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_vconn_ocp_END           (1)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_vbus_ovp_START          (2)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_vbus_ovp_END            (2)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_force_disch_fail_START  (4)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_force_disch_fail_END    (4)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_auto_disch_fail_START   (5)
#define SOC_SCHARGER_PD_FAULTSTAT_MSK_pd_msk_auto_disch_fail_END     (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TCPC_CTRL_UNION
 �ṹ˵��  : PD_TCPC_CTRL �Ĵ����ṹ���塣��ַƫ����:0x19����ֵ:0x00�����:8
 �Ĵ���˵��: PD TCPC Control
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_orient         : 1;  /* bit[0-0]: CC1/CC2ͨ·ѡ�����üĴ�����ÿ�����ӽ���ʱ��TCPC�ϱ�CC1/CC2�����жϣ�TCPM����Ҫ�˶�tCCDebounceʱ����жϷ�����������pd_orient�����㷽����ԭ����һ�������������ã�
                                                            0b��ѡ��CC1Ϊͨ��ͨ·�����CC1�Ƿ���ͨ�ŷ�������Vconn��Ч������CC2ͨ·��
                                                            1b��ѡ��CC2Ϊͨ��ͨ·�����CC2�Ƿ���ͨ�ŷ�������Vconn��Ч������CC1ͨ·��
                                                            Ĭ��0 */
        unsigned char  pd_bist_mode      : 1;  /* bit[1-1]: BISTģʽ���üĴ���������Ϊ1��Ҫ����USB�����Բ����ǲ���PD����㡣�����ӶϿ���TCPM��Ҫд�ñ���Ϊ0��
                                                            0b:����ģʽ��PD_RXDETECT�Ĵ�����ʹ�ܼ���SOP* msg��ͨ���жϷ�ʽ��֪TCPM��
                                                            1b��BIST����ģʽ��PD_RXDETECT�Ĵ�����ʹ�ܼ���SOP* msg����ͨ���жϷ�ʽ��֪TCPM��TCPC������Զ��洢�յ���msg��У��CRC���ظ�GoodCRC�����У�������λPD_ALERT_L��pd_int_rxstat�����п�����λPD_ALERT_H��pd_int_rx_full
                                                            Ĭ��0 */
        unsigned char  reserved_0        : 2;  /* bit[2-3]: I2C_CLK_STRECTH���ܲ�֧�֡����reserved */
        unsigned char  pd_debug_acc_ctrl : 1;  /* bit[4-4]: Debug Accessory���ƼĴ���
                                                            0b����TCPC�Զ����ƽ������˳���
                                                            1b����TCPM���ƣ�������Ϊ1����TCPC����Debugģʽ�����˳�Debugģʽ����TCPC������Debugģʽ���򲻿ɽ���Debugģʽ�����ղ���VDM���
                                                            Ĭ��0 */
        unsigned char  reserved_1        : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_TCPC_CTRL_UNION;
#endif
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_orient_START          (0)
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_orient_END            (0)
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_bist_mode_START       (1)
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_bist_mode_END         (1)
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_debug_acc_ctrl_START  (4)
#define SOC_SCHARGER_PD_TCPC_CTRL_pd_debug_acc_ctrl_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_ROLE_CTRL_UNION
 �ṹ˵��  : PD_ROLE_CTRL �Ĵ����ṹ���塣��ַƫ����:0x1A����ֵ:0x0A�����:8
 �Ĵ���˵��: PD Role Control
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_cc1_cfg : 2;  /* bit[0-1]: CC1ͨ·�������������üĴ�����TCPC�ϵ縴λ��Ĭ��˫Rp������
                                                     00b:Ra
                                                     01b:Rp
                                                     10b:Rd
                                                     11b:Open
                                                     Ĭ��:10b */
        unsigned char  pd_cc2_cfg : 2;  /* bit[2-3]: CC2ͨ·�������������üĴ�����TCPC�ϵ縴λ��Ĭ��˫Rp������
                                                     00b:Ra
                                                     01b:Rp
                                                     10b:Rd
                                                     11b:Open
                                                     Ĭ��:10b */
        unsigned char  pd_rp_cfg  : 2;  /* bit[4-5]: Rp������ֵ���üĴ���
                                                     00b��RpĬ��
                                                     01b��Rp 1.5A
                                                     10b��Rp 3.0A
                                                     11b��Ԥ������������
                                                     Ĭ��00b */
        unsigned char  pd_drp     : 1;  /* bit[6-6]: DRP����ʹ�����üĴ���
                                                     0b����ʹ��DRP toggle���ܣ�pd_cc1/cc2_cfg�Ĵ���ֱ�Ӿ������������裻
                                                     1b��ʹ��DRP���ܡ�ʹ��DRP��TCPM��������pd_cc1/cc2_cfgΪ˫Rp��������˫Rd�������ó�ʼ��λ����������ֵ��Ч����Ȼ��дPD_COMMAND�Ĵ���Look4Connection����TCPC��ʼ�Գ�ʼ��λ��toggle��Toggle����⵽��Ч�����TCPC�ϱ��жϣ�TCPM��ȡ��Ϣ�������Ӻ���Ҫ����pd_drpΪ0��
                                                     Ĭ��0b */
        unsigned char  reserved   : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_ROLE_CTRL_UNION;
#endif
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_cc1_cfg_START  (0)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_cc1_cfg_END    (1)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_cc2_cfg_START  (2)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_cc2_cfg_END    (3)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_rp_cfg_START   (4)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_rp_cfg_END     (5)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_drp_START      (6)
#define SOC_SCHARGER_PD_ROLE_CTRL_pd_drp_END        (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_FAULT_CTRL_UNION
 �ṹ˵��  : PD_FAULT_CTRL �Ĵ����ṹ���塣��ַƫ����:0x1B����ֵ:0x00�����:8
 �Ĵ���˵��: PD Fault Control
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vconn_ocp_en    : 1;  /* bit[0-0]: No Use��Vconn��������·ʹ�ܡ����ڸò�����ģ��Ĵ����������ã��������û��ʹ�ã�Ԥ���Ĵ����� */
        unsigned char  reserved_0         : 2;  /* bit[1-2]: No Use��Vbus OCP/OVP����·ʹ�ܡ����ڸò�����ģ��Ĵ����������ã��������û��ʹ�ã�Ԥ���Ĵ����� */
        unsigned char  pd_disch_timer_dis : 1;  /* bit[3-3]: Vbus�ŵ�Fail����·���ƼĴ���
                                                             0b��ʹ��Vbus�ŵ�Fail����·��������
                                                             1b����ʹ��Vbus�ŵ�Fail����·����������PD_FAULT_STATUS�Ĵ�����pd_auto_disch_fail/pd_force_disch_fail�����ϱ���
                                                             Ĭ��0 */
        unsigned char  reserved_1         : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_FAULT_CTRL_UNION;
#endif
#define SOC_SCHARGER_PD_FAULT_CTRL_pd_vconn_ocp_en_START     (0)
#define SOC_SCHARGER_PD_FAULT_CTRL_pd_vconn_ocp_en_END       (0)
#define SOC_SCHARGER_PD_FAULT_CTRL_pd_disch_timer_dis_START  (3)
#define SOC_SCHARGER_PD_FAULT_CTRL_pd_disch_timer_dis_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_PWR_CTRL_UNION
 �ṹ˵��  : PD_PWR_CTRL �Ĵ����ṹ���塣��ַƫ����:0x1C����ֵ:0x60�����:8
 �Ĵ���˵��: PD Power Control
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vconn_en       : 1;  /* bit[0-0]: Vconnʹ��
                                                            0b����ʹ��Vconn Source
                                                            1b��ʹ��Vconn Source��������PD_TCPC_CTR�Ĵ�����pd_orient�������õ���Ӧ��CC1/CC2ͨ·��
                                                            Ĭ��0 */
        unsigned char  reserved_0        : 1;  /* bit[1-1]: No use��Vconn Power�������á�����оƬ�ֻ�ṩ1W��Vconn Source�����û��ʹ�á� */
        unsigned char  pd_force_disch_en : 1;  /* bit[2-2]: VBUSй��ͨ·ʹ���źš�����Ϊ1��ֱ�Ӷ�ģ���·ʹ��к�Ź��ܡ�
                                                            0b: ��ʹ��VBUSй�ŵ���
                                                            1b: ʹ��VBUSй�ŵ���
                                                            Ĭ��0 */
        unsigned char  pd_bleed_disch_en : 1;  /* bit[3-3]: VBUS Bleedй��ͨ·ʹ���źš�����Ϊ1��ֱ�Ӷ�ģ���·ʹ��к�Ź��ܡ�
                                                            0b: ��ʹ��Bleed VBUSй�ŵ���
                                                            1b: ʹ��Bleed VBUSй�ŵ���
                                                            Ĭ��0 */
        unsigned char  pd_auto_disch     : 1;  /* bit[4-4]: VBUS�ڰγ�ʱ�Զ��ŵ�ʹ��
                                                            0b: �γ�ʱ��ʹ���Զ��ŵ�
                                                            1b: �γ�ʱʹ���Զ��ŵ�
                                                            Ĭ��0 */
        unsigned char  pd_valarm_dis     : 1;  /* bit[5-5]: VBUS��ѹ�ߵ���ֵ�澯ʹ�����üĴ���
                                                            0b��VBUS��ѹ�澯ʹ�ܣ���VBUS��ѹ����PD_VALARMH�����ϱ�pd_int_vbus_alrm_h�жϣ���VBUS��ѹ���͹�PD_VALARML�����ϱ�pd_int_vbus_alrm_l�жϣ�����ʹ��ǰ��Ҫ����HKADC����VBUS��ѹ��ֵ��
                                                            1b����ʹ��VBUS��ѹ�澯���ܡ������ϱ�pd_int_vbus_alrm_h/pd_int_vbus_alrm_l�жϣ�
                                                            Ĭ��1�� */
        unsigned char  pd_vbus_mon_dis   : 1;  /* bit[6-6]: VBUS��ѹ��ֵ�������ź�
                                                            0b:ʹ��VBUS��ѹ��ֵ��⣬����ͨ���ض�VBUS_VOLTAGE��ȡVBUS��ѹ��ֵ��
                                                            1b����ʹ��VBUS��ѹ��ֵ��⣬VBUS_VOLTAGE�ض�Ϊȫ0.
                                                            Ĭ��1 */
        unsigned char  reserved_1        : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_PWR_CTRL_UNION;
#endif
#define SOC_SCHARGER_PD_PWR_CTRL_pd_vconn_en_START        (0)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_vconn_en_END          (0)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_force_disch_en_START  (2)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_force_disch_en_END    (2)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_bleed_disch_en_START  (3)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_bleed_disch_en_END    (3)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_auto_disch_START      (4)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_auto_disch_END        (4)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_valarm_dis_START      (5)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_valarm_dis_END        (5)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_vbus_mon_dis_START    (6)
#define SOC_SCHARGER_PD_PWR_CTRL_pd_vbus_mon_dis_END      (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_CC_STATUS_UNION
 �ṹ˵��  : PD_CC_STATUS �Ĵ����ṹ���塣��ַƫ����:0x1D����ֵ:0x10�����:8
 �Ĵ���˵��: PD CC Status
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_cc1_stat   : 2;  /* bit[0-1]: CC1ͨ·״̬��
                                                        ��CC1ΪRp��������ֵΪ��
                                                        00b��SRC.Open��Open, Rp��
                                                        01��SRC.Ra����vRa���ֵ���£�
                                                        10b��SRC.Rd����vRd��Χ֮�ڣ�
                                                        11b��Ԥ��

                                                        ��CC1ΪRd��������ֵΪ��
                                                        00b��SNK.Open����vRa���ֵ���£�
                                                        01b��SNK.Default����vRd-Connect��Сֵ���ϣ�
                                                        10b��SNK.Power1.5����vRd-Connect��Сֵ���ϼ�⵽Rp 1.5A��
                                                        11b��SNK.Power3.0����vRd-Connect��Сֵ���ϼ�⵽Rp 3.0A��

                                                        ���⣬����pd_cc1_cfgΪRa����Open����Ϊ0��
                                                        ��pd_look4conΪ1ʱ�򣬸ñ���״̬Ϊ0��
                                                        Ĭ��00b */
        unsigned char  pd_cc2_stat   : 2;  /* bit[2-3]: CC2ͨ·״̬��
                                                        ��CC2ΪRp��������ֵΪ��
                                                        00b��SRC.Open��Open, Rp��
                                                        01��SRC.Ra����vRa���ֵ���£�
                                                        10b��SRC.Rd����vRd��Χ֮�ڣ�
                                                        11b��Ԥ��

                                                        ��CC2ΪRd��������ֵΪ��
                                                        00b��SNK.Open����vRa���ֵ���£�
                                                        01b��SNK.Default����vRd-Connect��Сֵ���ϣ�
                                                        10b��SNK.Power1.5����vRd-Connect��Сֵ���ϼ�⵽Rp 1.5A��
                                                        11b��SNK.Power3.0����vRd-Connect��Сֵ���ϼ�⵽Rp 3.0A��

                                                        ���⣬����pd_cc2_cfgΪRa����Open����Ϊ0��
                                                        ��pd_look4conΪ1ʱ�򣬸ñ���״̬Ϊ0��
                                                        Ĭ��00b */
        unsigned char  pd_con_result : 1;  /* bit[4-4]: 0b��TCPCĿǰ״̬Ϊ˫Rp������
                                                        1b��TCPCĿǰ״̬Ϊ˫Rd������
                                                        Ĭ��1 */
        unsigned char  pd_look4con   : 1;  /* bit[5-5]: 0b��TCPC���ڼ����Ч����
                                                        1b��TCPC���ڼ����Ч���ӣ�
                                                        ��pd_look4con����1��0�ı仯����ζ�ż�⵽Ǳ�ڵ����ӡ�
                                                        Ĭ��0 */
        unsigned char  reserved      : 2;  /* bit[6-7]:  */
    } reg;
} SOC_SCHARGER_PD_CC_STATUS_UNION;
#endif
#define SOC_SCHARGER_PD_CC_STATUS_pd_cc1_stat_START    (0)
#define SOC_SCHARGER_PD_CC_STATUS_pd_cc1_stat_END      (1)
#define SOC_SCHARGER_PD_CC_STATUS_pd_cc2_stat_START    (2)
#define SOC_SCHARGER_PD_CC_STATUS_pd_cc2_stat_END      (3)
#define SOC_SCHARGER_PD_CC_STATUS_pd_con_result_START  (4)
#define SOC_SCHARGER_PD_CC_STATUS_pd_con_result_END    (4)
#define SOC_SCHARGER_PD_CC_STATUS_pd_look4con_START    (5)
#define SOC_SCHARGER_PD_CC_STATUS_pd_look4con_END      (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_PWR_STATUS_UNION
 �ṹ˵��  : PD_PWR_STATUS �Ĵ����ṹ���塣��ַƫ����:0x1E����ֵ:0x48�����:8
 �Ĵ���˵��: PD Power Status
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_sinking_vbus        : 1;  /* bit[0-0]: 0b��Sink is Disconned
                                                                 1b��TCPC����Sinking Vbus�����أ�
                                                                 Ĭ��0 */
        unsigned char  pd_vconn_present       : 1;  /* bit[1-1]: 0b��Vconnû����λ��
                                                                 1b��Vconn apply��CC1��CC2�ϣ�
                                                                 ��PD_PWR_CTRL�Ĵ�����pd_vconn_enΪ0����ñ���Ϊ0��
                                                                 Ĭ��0 */
        unsigned char  pd_vbus_present        : 1;  /* bit[2-2]: 0b��Vbus����λ��
                                                                 1b��Vbus��λ��
                                                                 ��Vbus��ѹС��3.4V����ΪVbus����λ
                                                                 Ĭ��0 */
        unsigned char  pd_vbus_pres_detect_en : 1;  /* bit[3-3]: 0b��Vbus��λ���û��ʹ�ܣ�
                                                                 1b��Vbus��λ����Ѿ�ʹ�ܣ�
                                                                 ���TCPC�ڲ�Vbus��λ����·�Ƿ�ʹ��
                                                                 Ĭ��1��ʹ�� */
        unsigned char  pd_source_vbus         : 1;  /* bit[4-4]: 0b��TCPCû����Source Vbus��
                                                                 1b��TCPC��Source Vbus
                                                                 �ñ��ؽ���ΪTCPC�Ƿ���Source Vbus�ļ�⣬�������ơ�
                                                                 Ĭ��0 */
        unsigned char  reserved               : 1;  /* bit[5-5]: Sourcing High Voltageû��ʹ�ã�Ԥ�� */
        unsigned char  pd_tcpc_init_stat      : 1;  /* bit[6-6]: 0b��TCPC�Ѿ���ɳ�ʼ�������мĴ�����������Ч�ģ�
                                                                 1b��TCPC���ڻ�����δ��ɳ�ʼ�����Ĵ�������֤00h~0Fh�Ļض�ֵ��ȷ��
                                                                 Ĭ��1 */
        unsigned char  pd_debug_acc_connect   : 1;  /* bit[7-7]: 0b��û��Debug Accessory���ӣ�
                                                                 1b����Debug Accessory���ӣ���оƬ��Debugģʽ�½���ΪDebug.Snk������ΪDebug.Src��
                                                                 Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_PWR_STATUS_UNION;
#endif
#define SOC_SCHARGER_PD_PWR_STATUS_pd_sinking_vbus_START         (0)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_sinking_vbus_END           (0)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vconn_present_START        (1)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vconn_present_END          (1)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vbus_present_START         (2)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vbus_present_END           (2)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vbus_pres_detect_en_START  (3)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_vbus_pres_detect_en_END    (3)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_source_vbus_START          (4)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_source_vbus_END            (4)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_tcpc_init_stat_START       (6)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_tcpc_init_stat_END         (6)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_debug_acc_connect_START    (7)
#define SOC_SCHARGER_PD_PWR_STATUS_pd_debug_acc_connect_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_FAULT_STATUS_UNION
 �ṹ˵��  : PD_FAULT_STATUS �Ĵ����ṹ���塣��ַƫ����:0x1F����ֵ:0x80�����:8
 �Ĵ���˵��: PD Fault Status
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_i2c_err           : 1;  /* bit[0-0]: I2C����Error
                                                               0b��û�д�������
                                                               1b��I2C���ô�������
                                                               ������Transmit��������SOP*����msg��ʱ����TX buffer cntС��2�����ϱ�I2C Error
                                                               Ĭ��0 */
        unsigned char  pd_vconn_ocp         : 1;  /* bit[1-1]: VCONN OCP��ϵͳ�ж��ϱ������ﲻ�ϱ��� */
        unsigned char  pd_vusb_ovp          : 1;  /* bit[2-2]: VBUS OVP��ϵͳ�ж��ϱ������ﲻ�ϱ��� */
        unsigned char  reserved_0           : 1;  /* bit[3-3]: VBUS OCP no use */
        unsigned char  pd_force_disch_fail  : 1;  /* bit[4-4]: 0b��force�ŵ�û��ʧ�ܣ�
                                                               1b��force�ŵ�ʧ�ܡ���PD_PWR_CTRL�е�pd_force_disch_en����Ϊ1��pd_disch_timer_dis����Ϊ0������tSave0v��250ms����VBUS��ѹû���½���vSafe0v�����ϱ��Զ��ŵ�ʧ�ܡ� */
        unsigned char  pd_auto_disch_fail   : 1;  /* bit[5-5]: 0b���Զ��ŵ�û��ʧ�ܣ�
                                                               1b���Զ��ŵ�ʧ�ܡ���PD_PWR_CTRL�е�pd_auto_disch����Ϊ1����pd_disch_timer_dis����Ϊ0�����ڰγ�ʱ�������tSave0v��250ms����VBUS��ѹû���½���vSafe0v�����ϱ��Զ��ŵ�ʧ�ܡ�
                                                               Ĭ��0 */
        unsigned char  reserved_1           : 1;  /* bit[6-6]: ԭЭ��ΪForce Off Vbus����оƬ��֧�֡� */
        unsigned char  pd_reg_reset_default : 1;  /* bit[7-7]: ��TCPCоƬ������PDЭ����صļĴ�������λ��Ĭ��ֵ���ñ�����1������Ϊ0���������Ρ������power up��ʼ�������쳣�ϵ縴λ��ʱ������ */
    } reg;
} SOC_SCHARGER_PD_FAULT_STATUS_UNION;
#endif
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_i2c_err_START            (0)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_i2c_err_END              (0)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_vconn_ocp_START          (1)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_vconn_ocp_END            (1)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_vusb_ovp_START           (2)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_vusb_ovp_END             (2)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_force_disch_fail_START   (4)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_force_disch_fail_END     (4)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_auto_disch_fail_START    (5)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_auto_disch_fail_END      (5)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_reg_reset_default_START  (7)
#define SOC_SCHARGER_PD_FAULT_STATUS_pd_reg_reset_default_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_COMMAND_UNION
 �ṹ˵��  : PD_COMMAND �Ĵ����ṹ���塣��ַƫ����:0x23����ֵ:0x00�����:8
 �Ĵ���˵��: PD Command
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_command : 8;  /* bit[0-7]: ����Ĵ�����TCPMͨ��I2C�ӿڶԸüĴ�������Ϊ��ͬ��ֵ�Ӷ�������ͬ����������б�������ʾ��
                                                     8'h11��WakeI2C����֧�֣�
                                                     8'h22��DisableVbusDetect��֧�֣��ر�Vbus��ѹ��⹦�ܡ���TCPC��sourcing����sinking power��ʱ���鲻�رա�
                                                     8'h33��EnableVbusDetect��֧�֣�ʹ��Vbus��ѹ��⹦��
                                                     8'h44��DisableSinkVbus����֧�֣����ͨ������sc_ovp_mos_en�Ĵ���Ϊ0��ֹͣSink Vbus��
                                                     8'h55��SinkVbus����֧�֣����ͨ������sc_ovp_mos_en�Ĵ���Ϊ1��ʹ��Sink Vbus��
                                                     8'h66��DisableSourceVbus����֧�֣����ͨ�����ùر��ڲ�OTGʹ�ܣ����忴otg���ָ�ϲ��֣���ֹͣSource Vbus
                                                     6'h77��SourceVbusDefaultVol����֧�֣����ͨ�����ÿ����ڲ�OTGʹ�ܣ����忴otg���ָ�ϲ��֣���Source Vbus��
                                                     6'h88��SourceVbusHighVol����֧�֣�
                                                     6'h99��Look4Connection��֧�֣���PD_ROLE_CTRL��pd_cc2_cfg/pd_cc1_cfgΪ˫Rp����˫Rd����pd_drpΪ1����TCPC��ʼDRP Toggle������������������û�ж�����
                                                     6'hAA��RxOneMore��֧�֣�ʹ��PD���ն��ڷ�������һ��GoodCRC���Զ���PD_RXDETECT�Ĵ������������ĳһ�ض�����Թر�msg�Ľ��գ��ְ���RX FIFO����Ȳ����Ƹù��ܵ�ʹ�á�
                                                     ��������֧�� */
    } reg;
} SOC_SCHARGER_PD_COMMAND_UNION;
#endif
#define SOC_SCHARGER_PD_COMMAND_pd_command_START  (0)
#define SOC_SCHARGER_PD_COMMAND_pd_command_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DEVCAP1L_UNION
 �ṹ˵��  : PD_DEVCAP1L �Ĵ����ṹ���塣��ַƫ����:0x24����ֵ:0x00�����:8
 �Ĵ���˵��: PD Device Cap1 Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_cap1_l : 8;  /* bit[0-7]: Device Capabilities1��8���� */
    } reg;
} SOC_SCHARGER_PD_DEVCAP1L_UNION;
#endif
#define SOC_SCHARGER_PD_DEVCAP1L_pd_dev_cap1_l_START  (0)
#define SOC_SCHARGER_PD_DEVCAP1L_pd_dev_cap1_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DEVCAP1H_UNION
 �ṹ˵��  : PD_DEVCAP1H �Ĵ����ṹ���塣��ַƫ����:0x25����ֵ:0x00�����:8
 �Ĵ���˵��: PD Device Cap1 High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_cap1_h : 8;  /* bit[0-7]: Device Capabilities1��8���� */
    } reg;
} SOC_SCHARGER_PD_DEVCAP1H_UNION;
#endif
#define SOC_SCHARGER_PD_DEVCAP1H_pd_dev_cap1_h_START  (0)
#define SOC_SCHARGER_PD_DEVCAP1H_pd_dev_cap1_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DEVCAP2L_UNION
 �ṹ˵��  : PD_DEVCAP2L �Ĵ����ṹ���塣��ַƫ����:0x26����ֵ:0x00�����:8
 �Ĵ���˵��: PD Device Cap2 Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_cap2_l : 8;  /* bit[0-7]: Device Capabilities2��8���� */
    } reg;
} SOC_SCHARGER_PD_DEVCAP2L_UNION;
#endif
#define SOC_SCHARGER_PD_DEVCAP2L_pd_dev_cap2_l_START  (0)
#define SOC_SCHARGER_PD_DEVCAP2L_pd_dev_cap2_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DEVCAP2H_UNION
 �ṹ˵��  : PD_DEVCAP2H �Ĵ����ṹ���塣��ַƫ����:0x27����ֵ:0x00�����:8
 �Ĵ���˵��: PD Device Cap2 High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dev_cap2_h : 8;  /* bit[0-7]: Device Capabilities2��8���� */
    } reg;
} SOC_SCHARGER_PD_DEVCAP2H_UNION;
#endif
#define SOC_SCHARGER_PD_DEVCAP2H_pd_dev_cap2_h_START  (0)
#define SOC_SCHARGER_PD_DEVCAP2H_pd_dev_cap2_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_STDIN_CAP_UNION
 �ṹ˵��  : PD_STDIN_CAP �Ĵ����ṹ���塣��ַƫ����:0x28����ֵ:0x00�����:8
 �Ĵ���˵��: PD Standard Input Capabilities
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_stdin_cap : 8;  /* bit[0-7]: Standard Input Capabilities */
    } reg;
} SOC_SCHARGER_PD_STDIN_CAP_UNION;
#endif
#define SOC_SCHARGER_PD_STDIN_CAP_pd_stdin_cap_START  (0)
#define SOC_SCHARGER_PD_STDIN_CAP_pd_stdin_cap_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_STDOUT_CAP_UNION
 �ṹ˵��  : PD_STDOUT_CAP �Ĵ����ṹ���塣��ַƫ����:0x29����ֵ:0x00�����:8
 �Ĵ���˵��: PD Standard Output Capabilities
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_stdout_cap : 8;  /* bit[0-7]: Standard Output Capabilities */
    } reg;
} SOC_SCHARGER_PD_STDOUT_CAP_UNION;
#endif
#define SOC_SCHARGER_PD_STDOUT_CAP_pd_stdout_cap_START  (0)
#define SOC_SCHARGER_PD_STDOUT_CAP_pd_stdout_cap_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_MSG_HEADR_UNION
 �ṹ˵��  : PD_MSG_HEADR �Ĵ����ṹ���塣��ַƫ����:0x2E����ֵ:0x02�����:8
 �Ĵ���˵��: PD Message Header
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_msg_header : 8;  /* bit[0-7]: TCPM��Ҫ��TCPC��ɳ�ʼ��������ҵ���������øüĴ�����
                                                        TCPM�ڼ�⵽��Ч���Ӻ�������PD_RXDETECT�Ĵ���ʹ�ܽ���ǰ����Ҫˢ�¸üĴ�����
                                                        TCPC�ڻ�GoodCRCʱ��ӸüĴ���ȡ��Ӧ��Ϣ��
                                                        Bit[7:5]��reserved
                                                        bit[4]��Cable Plug
                                                        0b:Msg oriented from Source, Sink, or DRP
                                                        1b:Msg oriented from a Cable Plug
                                                        bit[3]:Data Role
                                                        0b:UFP
                                                        1b:DFP
                                                        bit[2:1]:USB PD spec. Revision
                                                        00b:Revision 1.0
                                                        01b:Revision 2.0
                                                        Other:reserved
                                                        bit[0] Power Role
                                                        0b: Sink
                                                        1b: Source */
    } reg;
} SOC_SCHARGER_PD_MSG_HEADR_UNION;
#endif
#define SOC_SCHARGER_PD_MSG_HEADR_pd_msg_header_START  (0)
#define SOC_SCHARGER_PD_MSG_HEADR_pd_msg_header_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXDETECT_UNION
 �ṹ˵��  : PD_RXDETECT �Ĵ����ṹ���塣��ַƫ����:0x2F����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX Detect
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_en_sop        : 1;  /* bit[0-0]: 0b:TCPC�����SOP msg
                                                           1b��TCPC���SOP msg
                                                           Ĭ��0 */
        unsigned char  pd_en_sop1       : 1;  /* bit[1-1]: 0b:TCPC�����SOP' msg
                                                           1b��TCPC���SOP' msg
                                                           Ĭ��0 */
        unsigned char  pd_en_sop2       : 1;  /* bit[2-2]: 0b:TCPC�����SOP'' msg
                                                           1b��TCPC���SOP'' msg
                                                           Ĭ��0 */
        unsigned char  pd_en_sop1_debug : 1;  /* bit[3-3]: 0b:TCPC�����SOP_DBG' msg
                                                           1b��TCPC���SOP_DBG' msg
                                                           Ĭ��0 */
        unsigned char  pd_en_sop2_debug : 1;  /* bit[4-4]: 0b:TCPC�����SOPDBG'' msg
                                                           1b��TCPC���SOPDBG'' msg
                                                           Ĭ��0 */
        unsigned char  pd_hard_rst      : 1;  /* bit[5-5]: 0b:TCPC�����Hard Reset
                                                           1b��TCPC���Hard Reset
                                                           Ĭ��0 */
        unsigned char  pd_cable_rst     : 1;  /* bit[6-6]: 0b:TCPC�����Cable Reset
                                                           1b��TCPC���Cable Reset
                                                           Ĭ��0 */
        unsigned char  reserved         : 1;  /* bit[7-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXDETECT_UNION;
#endif
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop_START         (0)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop_END           (0)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop1_START        (1)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop1_END          (1)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop2_START        (2)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop2_END          (2)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop1_debug_START  (3)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop1_debug_END    (3)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop2_debug_START  (4)
#define SOC_SCHARGER_PD_RXDETECT_pd_en_sop2_debug_END    (4)
#define SOC_SCHARGER_PD_RXDETECT_pd_hard_rst_START       (5)
#define SOC_SCHARGER_PD_RXDETECT_pd_hard_rst_END         (5)
#define SOC_SCHARGER_PD_RXDETECT_pd_cable_rst_START      (6)
#define SOC_SCHARGER_PD_RXDETECT_pd_cable_rst_END        (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXBYTECNT_UNION
 �ṹ˵��  : PD_RXBYTECNT �Ĵ����ṹ���塣��ַƫ����:0x30����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX ByteCount
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_rx_bytecnt : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXBYTECNT_UNION;
#endif
#define SOC_SCHARGER_PD_RXBYTECNT_pd_rx_bytecnt_START  (0)
#define SOC_SCHARGER_PD_RXBYTECNT_pd_rx_bytecnt_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXTYPE_UNION
 �ṹ˵��  : PD_RXTYPE �Ĵ����ṹ���塣��ַƫ����:0x31����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX Type
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_rx_type : 3;  /* bit[0-2]:  */
        unsigned char  reserved   : 5;  /* bit[3-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXTYPE_UNION;
#endif
#define SOC_SCHARGER_PD_RXTYPE_pd_rx_type_START  (0)
#define SOC_SCHARGER_PD_RXTYPE_pd_rx_type_END    (2)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXHEADL_UNION
 �ṹ˵��  : PD_RXHEADL �Ĵ����ṹ���塣��ַƫ����:0x32����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX Header Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_rx_head_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXHEADL_UNION;
#endif
#define SOC_SCHARGER_PD_RXHEADL_pd_rx_head_l_START  (0)
#define SOC_SCHARGER_PD_RXHEADL_pd_rx_head_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXHEADH_UNION
 �ṹ˵��  : PD_RXHEADH �Ĵ����ṹ���塣��ַƫ����:0x33����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX Header High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_rx_head_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXHEADH_UNION;
#endif
#define SOC_SCHARGER_PD_RXHEADH_pd_rx_head_h_START  (0)
#define SOC_SCHARGER_PD_RXHEADH_pd_rx_head_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_RXDATA_0_UNION
 �ṹ˵��  : PD_RXDATA_0 �Ĵ����ṹ���塣��ַƫ����:k*1+0x34����ֵ:0x00�����:8
 �Ĵ���˵��: PD RX Data Payload
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_rx_data : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_RXDATA_0_UNION;
#endif
#define SOC_SCHARGER_PD_RXDATA_0_pd_rx_data_START  (0)
#define SOC_SCHARGER_PD_RXDATA_0_pd_rx_data_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TRANSMIT_UNION
 �ṹ˵��  : PD_TRANSMIT �Ĵ����ṹ���塣��ַƫ����:0x50����ֵ:0x00�����:8
 �Ĵ���˵��: PD Transmit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_transmit  : 3;  /* bit[0-2]:  */
        unsigned char  reserved_0   : 1;  /* bit[3-3]:  */
        unsigned char  pd_retry_cnt : 2;  /* bit[4-5]:  */
        unsigned char  reserved_1   : 2;  /* bit[6-7]:  */
    } reg;
} SOC_SCHARGER_PD_TRANSMIT_UNION;
#endif
#define SOC_SCHARGER_PD_TRANSMIT_pd_transmit_START   (0)
#define SOC_SCHARGER_PD_TRANSMIT_pd_transmit_END     (2)
#define SOC_SCHARGER_PD_TRANSMIT_pd_retry_cnt_START  (4)
#define SOC_SCHARGER_PD_TRANSMIT_pd_retry_cnt_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TXBYTECNT_UNION
 �ṹ˵��  : PD_TXBYTECNT �Ĵ����ṹ���塣��ַƫ����:0x51����ֵ:0x00�����:8
 �Ĵ���˵��: PD TX Byte Count
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_tx_bytecnt : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_TXBYTECNT_UNION;
#endif
#define SOC_SCHARGER_PD_TXBYTECNT_pd_tx_bytecnt_START  (0)
#define SOC_SCHARGER_PD_TXBYTECNT_pd_tx_bytecnt_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TXHEADL_UNION
 �ṹ˵��  : PD_TXHEADL �Ĵ����ṹ���塣��ַƫ����:0x52����ֵ:0x00�����:8
 �Ĵ���˵��: PD TX Header Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_tx_head_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_TXHEADL_UNION;
#endif
#define SOC_SCHARGER_PD_TXHEADL_pd_tx_head_l_START  (0)
#define SOC_SCHARGER_PD_TXHEADL_pd_tx_head_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TXHEADH_UNION
 �ṹ˵��  : PD_TXHEADH �Ĵ����ṹ���塣��ַƫ����:0x53����ֵ:0x00�����:8
 �Ĵ���˵��: PD TX Header High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_tx_head_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_TXHEADH_UNION;
#endif
#define SOC_SCHARGER_PD_TXHEADH_pd_tx_head_h_START  (0)
#define SOC_SCHARGER_PD_TXHEADH_pd_tx_head_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_TXDATA_UNION
 �ṹ˵��  : PD_TXDATA �Ĵ����ṹ���塣��ַƫ����:k*1+0x54����ֵ:0x00�����:8
 �Ĵ���˵��: PD TX Payload
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_tx_data : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_TXDATA_UNION;
#endif
#define SOC_SCHARGER_PD_TXDATA_pd_tx_data_START  (0)
#define SOC_SCHARGER_PD_TXDATA_pd_tx_data_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_VOL_L_UNION
 �ṹ˵��  : PD_VBUS_VOL_L �Ĵ����ṹ���塣��ַƫ����:0x70����ֵ:0x00�����:8
 �Ĵ���˵��: PD VBUS Voltage Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_vol_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_VOL_L_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_VOL_L_pd_vbus_vol_l_START  (0)
#define SOC_SCHARGER_PD_VBUS_VOL_L_pd_vbus_vol_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_VOL_H_UNION
 �ṹ˵��  : PD_VBUS_VOL_H �Ĵ����ṹ���塣��ַƫ����:0x71����ֵ:0x00�����:8
 �Ĵ���˵��: PD VBUS Voltage High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_vol_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_VOL_H_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_VOL_H_pd_vbus_vol_h_START  (0)
#define SOC_SCHARGER_PD_VBUS_VOL_H_pd_vbus_vol_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_SNK_DISCL_UNION
 �ṹ˵��  : PD_VBUS_SNK_DISCL �Ĵ����ṹ���塣��ַƫ����:0x72����ֵ:0x90�����:8
 �Ĵ���˵��: VBUS Sink Disconnect Threshold Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_snk_disc_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_SNK_DISCL_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_SNK_DISCL_pd_vbus_snk_disc_l_START  (0)
#define SOC_SCHARGER_PD_VBUS_SNK_DISCL_pd_vbus_snk_disc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_SNK_DISCH_UNION
 �ṹ˵��  : PD_VBUS_SNK_DISCH �Ĵ����ṹ���塣��ַƫ����:0x73����ֵ:0x00�����:8
 �Ĵ���˵��: VBUS Sink Disconnect Threshold High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_snk_disc_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_SNK_DISCH_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_SNK_DISCH_pd_vbus_snk_disc_h_START  (0)
#define SOC_SCHARGER_PD_VBUS_SNK_DISCH_pd_vbus_snk_disc_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_STOP_DISCL_UNION
 �ṹ˵��  : PD_VBUS_STOP_DISCL �Ĵ����ṹ���塣��ַƫ����:0x74����ֵ:0x1C�����:8
 �Ĵ���˵��: VBUS Discharge Stop Threshold Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_stop_disc_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_STOP_DISCL_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_STOP_DISCL_pd_vbus_stop_disc_l_START  (0)
#define SOC_SCHARGER_PD_VBUS_STOP_DISCL_pd_vbus_stop_disc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VBUS_STOP_DISCH_UNION
 �ṹ˵��  : PD_VBUS_STOP_DISCH �Ĵ����ṹ���塣��ַƫ����:0x75����ֵ:0x00�����:8
 �Ĵ���˵��: VBUS Discharge Stop Threshold High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_vbus_stop_disc_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VBUS_STOP_DISCH_UNION;
#endif
#define SOC_SCHARGER_PD_VBUS_STOP_DISCH_pd_vbus_stop_disc_h_START  (0)
#define SOC_SCHARGER_PD_VBUS_STOP_DISCH_pd_vbus_stop_disc_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VALARMH_CFGL_UNION
 �ṹ˵��  : PD_VALARMH_CFGL �Ĵ����ṹ���塣��ַƫ����:0x76����ֵ:0x00�����:8
 �Ĵ���˵��: Voltage High Trip Point Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_valarm_high_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VALARMH_CFGL_UNION;
#endif
#define SOC_SCHARGER_PD_VALARMH_CFGL_pd_valarm_high_l_START  (0)
#define SOC_SCHARGER_PD_VALARMH_CFGL_pd_valarm_high_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VALARMH_CFGH_UNION
 �ṹ˵��  : PD_VALARMH_CFGH �Ĵ����ṹ���塣��ַƫ����:0x77����ֵ:0x00�����:8
 �Ĵ���˵��: Voltage High Trip Point High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_valarm_high_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VALARMH_CFGH_UNION;
#endif
#define SOC_SCHARGER_PD_VALARMH_CFGH_pd_valarm_high_h_START  (0)
#define SOC_SCHARGER_PD_VALARMH_CFGH_pd_valarm_high_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VALARML_CFGL_UNION
 �ṹ˵��  : PD_VALARML_CFGL �Ĵ����ṹ���塣��ַƫ����:0x78����ֵ:0x00�����:8
 �Ĵ���˵��: Voltage Low Trip Point Low
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_valarm_low_l : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VALARML_CFGL_UNION;
#endif
#define SOC_SCHARGER_PD_VALARML_CFGL_pd_valarm_low_l_START  (0)
#define SOC_SCHARGER_PD_VALARML_CFGL_pd_valarm_low_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VALARML_CFGH_UNION
 �ṹ˵��  : PD_VALARML_CFGH �Ĵ����ṹ���塣��ַƫ����:0x79����ֵ:0x00�����:8
 �Ĵ���˵��: Voltage Low Trip Point High
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_valarm_low_h : 8;  /* bit[0-7]:  */
    } reg;
} SOC_SCHARGER_PD_VALARML_CFGH_UNION;
#endif
#define SOC_SCHARGER_PD_VALARML_CFGH_pd_valarm_low_h_START  (0)
#define SOC_SCHARGER_PD_VALARML_CFGH_pd_valarm_low_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VDM_CFG_0_UNION
 �ṹ˵��  : PD_VDM_CFG_0 �Ĵ����ṹ���塣��ַƫ����:0x7A����ֵ:0x60�����:8
 �Ĵ���˵��: PD�Զ������üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_frs_en          : 1;  /* bit[0-0]: �ⲿSwitchоƬ����5V��ѹ��ʹ���źţ�
                                                             0b����ʹ��
                                                             1b��ʹ��
                                                             Ĭ��0 */
        unsigned char  da_vconn_dis_en    : 1;  /* bit[1-1]: VCONNй��ͨ·ʹ���źţ�
                                                             0b����ʹ��VCONNй�ŵ���
                                                             1b��ʹ��VCONNй�ŵ���
                                                             Ĭ��0 */
        unsigned char  pd_drp_thres       : 2;  /* bit[2-3]: TCPC DRP Togglingʱ�����á�DRP Toggling����ʱ��Ϊ75ms������ͨ������pd_drp_thres��������ΪSrc��ʱ�䣻
                                                             00:dcSRC.DRPΪ22.5ms��
                                                             01:dcSRC.DRPΪ30ms��
                                                             10:dcSRC.DRPΪ37.5ms��
                                                             11:dcSRC.DRPΪ52.5ms��
                                                             Ĭ��Ϊ00 */
        unsigned char  pd_try_snk_en      : 1;  /* bit[4-4]: TCPC״̬��TrySnkʹ�ܡ���ʹ��TrySnk���ƣ�����Unattach.SRC״̬ʱ��⵽���������������״̬������ת��Try.Snk�Գ�����ΪSnk�������ӣ�����ʹ��TrySnk���ƣ���Unattach.SRC״̬ʱ��⵽���������������״̬������ת��Attached.SRC��������TrySnk���ӡ�
                                                             0b: ��ʹ��TrySnk���ƣ�
                                                             1b��ʹ��TrySnk���ƣ�
                                                             Ĭ��0 */
        unsigned char  da_force_unplug_en : 1;  /* bit[5-5]: Rd��ϵͳ��λʱ�Զ��Ͽ�ʹ���źţ�
                                                             0b����ʹ���Զ��Ͽ�����
                                                             1b��ʹ���Զ��Ͽ�����
                                                             da_force_unplug_enֻ��pwr_rst_n��λ�����ܹܽŸ�λreset_n����λӰ�죻
                                                             Ĭ��1 */
        unsigned char  pd_bmc_cdr_sel     : 1;  /* bit[6-6]: PD�����RXͨ·BMC����CDR��ʱ�ӻָ���·������ѡ��
                                                             0b:ѡ��counter�Ĵ�ƽ��Ԥ������������
                                                             1b:ѡ�����໷����������
                                                             Ĭ��1 */
        unsigned char  pd_cc_orient_sel   : 1;  /* bit[7-7]: Type-CЭ��CC1/CC2ͨ��orient����ѡ��
                                                             0b����Debugģʽ��Ĭ����Ӳ���Զ�������ȷ�����Զ�����orient��
                                                             1b����Debugģʽ��������ȷ�������������orient��
                                                             Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_VDM_CFG_0_UNION;
#endif
#define SOC_SCHARGER_PD_VDM_CFG_0_da_frs_en_START           (0)
#define SOC_SCHARGER_PD_VDM_CFG_0_da_frs_en_END             (0)
#define SOC_SCHARGER_PD_VDM_CFG_0_da_vconn_dis_en_START     (1)
#define SOC_SCHARGER_PD_VDM_CFG_0_da_vconn_dis_en_END       (1)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_drp_thres_START        (2)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_drp_thres_END          (3)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_try_snk_en_START       (4)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_try_snk_en_END         (4)
#define SOC_SCHARGER_PD_VDM_CFG_0_da_force_unplug_en_START  (5)
#define SOC_SCHARGER_PD_VDM_CFG_0_da_force_unplug_en_END    (5)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_bmc_cdr_sel_START      (6)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_bmc_cdr_sel_END        (6)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_cc_orient_sel_START    (7)
#define SOC_SCHARGER_PD_VDM_CFG_0_pd_cc_orient_sel_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VDM_ENABLE_UNION
 �ṹ˵��  : PD_VDM_ENABLE �Ĵ����ṹ���塣��ַƫ����:0x7B����ֵ:0x01�����:8
 �Ĵ���˵��: PD�Զ���ʹ�ܼĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  enable_pd : 1;  /* bit[0-0]: PDЭ��ʹ�ܼĴ�����ʹ�ܺ���ܽ����뷢��PD��Ϣ����Ӱ��Type-C�˿ڼ���·���Ӧ���á�
                                                    0b����ʹ��PDЭ��
                                                    1b: ʹ��PDЭ�顣Ĭ�ϵ�Type-C�˿ڼ���·��⵽��Ч���Ӻ�PD����ʱ�ӻῪ����PDЭ�鴦��Ready״̬��������TCPM���ý����뷢��PD��Ϣ��
                                                    Ĭ��1 */
        unsigned char  reserved  : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_VDM_ENABLE_UNION;
#endif
#define SOC_SCHARGER_PD_VDM_ENABLE_enable_pd_START  (0)
#define SOC_SCHARGER_PD_VDM_ENABLE_enable_pd_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_VDM_CFG_1_UNION
 �ṹ˵��  : PD_VDM_CFG_1 �Ĵ����ṹ���塣��ַƫ����:0x7C����ֵ:0x00�����:8
 �Ĵ���˵��: PD�Զ������üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tc_fsm_reset         : 1;  /* bit[0-0]: Type-C״̬����λ�źţ��ߵ�ƽ��Ч
                                                               0b������λType-C״̬����
                                                               1b����λType-C״̬����
                                                               Ĭ��0 */
        unsigned char  pd_fsm_reset         : 1;  /* bit[1-1]: PD״̬����λ�źţ��ߵ�ƽ��Ч
                                                               0b������λPD״̬����
                                                               1b����λPD״̬����
                                                               Ĭ��0 */
        unsigned char  pd_tx_phy_soft_reset : 1;  /* bit[2-2]: PD TX�������λ���ߵ�ƽ��Ч��
                                                               0b������λPD TX PHY��
                                                               1b����λPD TX PHY��
                                                               Ĭ��0 */
        unsigned char  pd_rx_phy_soft_reset : 1;  /* bit[3-3]: PD RX�������λ���ߵ�ƽ��Ч��
                                                               0b������λPD RX PHY��
                                                               1b����λPD RX PHY��
                                                               Ĭ��0 */
        unsigned char  pd_snk_disc_by_cc    : 1;  /* bit[4-4]: 0b:��ΪSinkʱ����֧������CC״̬�ж�Source���Ƴ���
                                                               1b:��ΪSinkʱ��֧������CC״̬�ж�Source���Ƴ���
                                                               Ĭ��0 */
        unsigned char  reserved             : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_PD_VDM_CFG_1_UNION;
#endif
#define SOC_SCHARGER_PD_VDM_CFG_1_tc_fsm_reset_START          (0)
#define SOC_SCHARGER_PD_VDM_CFG_1_tc_fsm_reset_END            (0)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_fsm_reset_START          (1)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_fsm_reset_END            (1)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_tx_phy_soft_reset_START  (2)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_tx_phy_soft_reset_END    (2)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_rx_phy_soft_reset_START  (3)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_rx_phy_soft_reset_END    (3)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_snk_disc_by_cc_START     (4)
#define SOC_SCHARGER_PD_VDM_CFG_1_pd_snk_disc_by_cc_END       (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RDATA_CFG_UNION
 �ṹ˵��  : PD_DBG_RDATA_CFG �Ĵ����ṹ���塣��ַƫ����:0x7D����ֵ:0x00�����:8
 �Ĵ���˵��: оƬ�����ã����Բ�Ʒ���š������������üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_rdata_sel  : 4;  /* bit[0-3]: оƬ�����ã����Բ�Ʒ���š�
                                                            ���Իض�����ѡ��
                                                            4'b0000:ѡ���0·����
                                                            4'b0001:ѡ���1·����
                                                            ��
                                                            4'b1111:ѡ���15·����
                                                            Ĭ��0 */
        unsigned char  pd_dbg_module_sel : 3;  /* bit[4-6]: оƬ�����ã����Բ�Ʒ���š�
                                                            ����ģ��ѡ��
                                                            3'b000:TCPC FSM
                                                            3'b001:PD TX/RX FSM
                                                            3'b010:PD TX PHY
                                                            3'b011:PD RX PHY
                                                            3'b100:VDM
                                                            3'b101:PD Timer/Counter
                                                            3'b110:PD Interrupt
                                                            3'b111:reserved
                                                            Ĭ�ϣ�3'b000 */
        unsigned char  pd_dbg_rdata_en   : 1;  /* bit[7-7]: оƬ�����ã����Բ�Ʒ���š�
                                                            ���Իض�����ʹ�ܣ��ߵ�ƽ��Ч��
                                                            1'b1:ʹ�ܲ������ݻض������ݴ�PD_DBG_RDATA�лض�������
                                                            1'b0:��ʹ�ܲ������ݻض���
                                                            Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_DBG_RDATA_CFG_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_rdata_sel_START   (0)
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_rdata_sel_END     (3)
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_module_sel_START  (4)
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_module_sel_END    (6)
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_rdata_en_START    (7)
#define SOC_SCHARGER_PD_DBG_RDATA_CFG_pd_dbg_rdata_en_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RDATA_UNION
 �ṹ˵��  : PD_DBG_RDATA �Ĵ����ṹ���塣��ַƫ����:0x7E����ֵ:0x00�����:8
 �Ĵ���˵��: оƬ�����ã����Բ�Ʒ���š����Իض����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_rdata : 8;  /* bit[0-7]: оƬ�����ã����Բ�Ʒ���š�
                                                       ���Իض����ݡ� */
    } reg;
} SOC_SCHARGER_PD_DBG_RDATA_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RDATA_pd_dbg_rdata_START  (0)
#define SOC_SCHARGER_PD_DBG_RDATA_pd_dbg_rdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VDM_PAGE_SELECT_UNION
 �ṹ˵��  : VDM_PAGE_SELECT �Ĵ����ṹ���塣��ַƫ����:0x7F����ֵ:0x00�����:8
 �Ĵ���˵��: Vendor Define Register, Page Select
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  page_select : 2;  /* bit[0-1]: оƬ�ڲ��Ĵ�����ҳ����
                                                      2'b00: REG_PAGE0;
                                                      2'b01: REG_PAGE1;
                                                      2'b10: REG_PAGE2;
                                                      2'b11: No Use, RFU
                                                      Default: 2'b00 */
        unsigned char  reserved    : 6;  /* bit[2-7]:  */
    } reg;
} SOC_SCHARGER_VDM_PAGE_SELECT_UNION;
#endif
#define SOC_SCHARGER_VDM_PAGE_SELECT_page_select_START  (0)
#define SOC_SCHARGER_VDM_PAGE_SELECT_page_select_END    (1)




/****************************************************************************
                     (2/4) REG_PAGE0
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_STATUIS_UNION
 �ṹ˵��  : STATUIS �Ĵ����ṹ���塣��ַƫ����:0x00����ֵ:0x00�����:8
 �Ĵ���˵��: fcp slave ����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  attach   : 1;  /* bit[0]  : 0����⵽װ��δ���룻
                                                   1����⵽װ�ò��� */
        unsigned char  reserved : 5;  /* bit[1-5]: ������ */
        unsigned char  dvc      : 2;  /* bit[6-7]: 00�����û�п�ʼ�������ڼ��״̬��
                                                   01����⵽��ЧFCP slave��
                                                   10��������
                                                   11����⵽FCP slave�� */
    } reg;
} SOC_SCHARGER_STATUIS_UNION;
#endif
#define SOC_SCHARGER_STATUIS_attach_START    (0)
#define SOC_SCHARGER_STATUIS_attach_END      (0)
#define SOC_SCHARGER_STATUIS_dvc_START       (6)
#define SOC_SCHARGER_STATUIS_dvc_END         (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CNTL_UNION
 �ṹ˵��  : CNTL �Ĵ����ṹ���塣��ַƫ����:0x01����ֵ:0x00�����:8
 �Ĵ���˵��: FCP CNTL���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sndcmd   : 1;  /* bit[0]  : fcp master transaction ��ʼ���ƼĴ����� */
        unsigned char  reserved_0: 1;  /* bit[1]  : ������ */
        unsigned char  mstr_rst : 1;  /* bit[2]  : fcp master ����slave��λ�Ĵ����� */
        unsigned char  enable   : 1;  /* bit[3]  : fcp ʹ�ܿ��ƼĴ����� */
        unsigned char  reserved_1: 4;  /* bit[4-7]: ������ */
    } reg;
} SOC_SCHARGER_CNTL_UNION;
#endif
#define SOC_SCHARGER_CNTL_sndcmd_START    (0)
#define SOC_SCHARGER_CNTL_sndcmd_END      (0)
#define SOC_SCHARGER_CNTL_mstr_rst_START  (2)
#define SOC_SCHARGER_CNTL_mstr_rst_END    (2)
#define SOC_SCHARGER_CNTL_enable_START    (3)
#define SOC_SCHARGER_CNTL_enable_END      (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CMD_UNION
 �ṹ˵��  : CMD �Ĵ����ṹ���塣��ַƫ����:0x04����ֵ:0x00�����:8
 �Ĵ���˵��: fcp ��д�������üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cmd : 8;  /* bit[0-7]: fcp ��д�������üĴ����� */
    } reg;
} SOC_SCHARGER_CMD_UNION;
#endif
#define SOC_SCHARGER_CMD_cmd_START  (0)
#define SOC_SCHARGER_CMD_cmd_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_LENGTH_UNION
 �ṹ˵��  : FCP_LENGTH �Ĵ����ṹ���塣��ַƫ����:0x05����ֵ:0x01�����:8
 �Ĵ���˵��: fcp burst��д�������üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_length : 4;  /* bit[0-3]: burst��д�������üĴ���
                                                     4'd1����������д1byte��
                                                     4'd2����������д2byte��
                                                     4'd3����������д3byte��
                                                     4'd4����������д4byte��
                                                     4'd5����������д5byte��
                                                     4'd6����������д6byte��
                                                     4'd7����������д7byte��
                                                     4'd8����������д8byte��
                                                     0�����8��ֵΪ�Ƿ�ֵ�������ڲ����жϡ�
                                                     Ĭ��1  */
        unsigned char  reserved   : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_FCP_LENGTH_UNION;
#endif
#define SOC_SCHARGER_FCP_LENGTH_fcp_length_START  (0)
#define SOC_SCHARGER_FCP_LENGTH_fcp_length_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ADDR_UNION
 �ṹ˵��  : ADDR �Ĵ����ṹ���塣��ַƫ����:0x07����ֵ:0x00�����:8
 �Ĵ���˵��: fcp ��д��ַ���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  addr : 8;  /* bit[0-7]: fcp ��д��ַ���üĴ����� */
    } reg;
} SOC_SCHARGER_ADDR_UNION;
#endif
#define SOC_SCHARGER_ADDR_addr_START  (0)
#define SOC_SCHARGER_ADDR_addr_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA0_UNION
 �ṹ˵��  : DATA0 �Ĵ����ṹ���塣��ַƫ����:0x08����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data0 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA0_UNION;
#endif
#define SOC_SCHARGER_DATA0_data0_START  (0)
#define SOC_SCHARGER_DATA0_data0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA1_UNION
 �ṹ˵��  : DATA1 �Ĵ����ṹ���塣��ַƫ����:0x09����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data1 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA1_UNION;
#endif
#define SOC_SCHARGER_DATA1_data1_START  (0)
#define SOC_SCHARGER_DATA1_data1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA2_UNION
 �ṹ˵��  : DATA2 �Ĵ����ṹ���塣��ַƫ����:0x0A����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data2 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA2_UNION;
#endif
#define SOC_SCHARGER_DATA2_data2_START  (0)
#define SOC_SCHARGER_DATA2_data2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA3_UNION
 �ṹ˵��  : DATA3 �Ĵ����ṹ���塣��ַƫ����:0x0B����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data3 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA3_UNION;
#endif
#define SOC_SCHARGER_DATA3_data3_START  (0)
#define SOC_SCHARGER_DATA3_data3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA4_UNION
 �ṹ˵��  : DATA4 �Ĵ����ṹ���塣��ַƫ����:0x0C����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data4 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA4_UNION;
#endif
#define SOC_SCHARGER_DATA4_data4_START  (0)
#define SOC_SCHARGER_DATA4_data4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA5_UNION
 �ṹ˵��  : DATA5 �Ĵ����ṹ���塣��ַƫ����:0x0D����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data5 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA5_UNION;
#endif
#define SOC_SCHARGER_DATA5_data5_START  (0)
#define SOC_SCHARGER_DATA5_data5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA6_UNION
 �ṹ˵��  : DATA6 �Ĵ����ṹ���塣��ַƫ����:0x0E����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data6 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA6_UNION;
#endif
#define SOC_SCHARGER_DATA6_data6_START  (0)
#define SOC_SCHARGER_DATA6_data6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA7_UNION
 �ṹ˵��  : DATA7 �Ĵ����ṹ���塣��ַƫ����:0x0F����ֵ:0x00�����:8
 �Ĵ���˵��: fcp д���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data7 : 8;  /* bit[0-7]: fcp д���ݼĴ����� */
    } reg;
} SOC_SCHARGER_DATA7_UNION;
#endif
#define SOC_SCHARGER_DATA7_data7_START  (0)
#define SOC_SCHARGER_DATA7_data7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA0_UNION
 �ṹ˵��  : FCP_RDATA0 �Ĵ����ṹ���塣��ַƫ����:0x10����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata0 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA0_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA0_fcp_rdata0_START  (0)
#define SOC_SCHARGER_FCP_RDATA0_fcp_rdata0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA1_UNION
 �ṹ˵��  : FCP_RDATA1 �Ĵ����ṹ���塣��ַƫ����:0x11����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata1 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA1_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA1_fcp_rdata1_START  (0)
#define SOC_SCHARGER_FCP_RDATA1_fcp_rdata1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA2_UNION
 �ṹ˵��  : FCP_RDATA2 �Ĵ����ṹ���塣��ַƫ����:0x12����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata2 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA2_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA2_fcp_rdata2_START  (0)
#define SOC_SCHARGER_FCP_RDATA2_fcp_rdata2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA3_UNION
 �ṹ˵��  : FCP_RDATA3 �Ĵ����ṹ���塣��ַƫ����:0x13����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata3 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA3_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA3_fcp_rdata3_START  (0)
#define SOC_SCHARGER_FCP_RDATA3_fcp_rdata3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA4_UNION
 �ṹ˵��  : FCP_RDATA4 �Ĵ����ṹ���塣��ַƫ����:0x14����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata4 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA4_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA4_fcp_rdata4_START  (0)
#define SOC_SCHARGER_FCP_RDATA4_fcp_rdata4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA5_UNION
 �ṹ˵��  : FCP_RDATA5 �Ĵ����ṹ���塣��ַƫ����:0x15����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata5 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA5_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA5_fcp_rdata5_START  (0)
#define SOC_SCHARGER_FCP_RDATA5_fcp_rdata5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA6_UNION
 �ṹ˵��  : FCP_RDATA6 �Ĵ����ṹ���塣��ַƫ����:0x16����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata6 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA6_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA6_fcp_rdata6_START  (0)
#define SOC_SCHARGER_FCP_RDATA6_fcp_rdata6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA7_UNION
 �ṹ˵��  : FCP_RDATA7 �Ĵ����ṹ���塣��ַƫ����:0x17����ֵ:0x00�����:8
 �Ĵ���˵��: slave���ص����ݡ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata7 : 8;  /* bit[0-7]: I2Cͨ��master���ص�slaver�ļĴ���ֵ�� */
    } reg;
} SOC_SCHARGER_FCP_RDATA7_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA7_fcp_rdata7_START  (0)
#define SOC_SCHARGER_FCP_RDATA7_fcp_rdata7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ISR1_UNION
 �ṹ˵��  : ISR1 �Ĵ����ṹ���塣��ַƫ����:0x19����ֵ:0x00�����:8
 �Ĵ���˵��: FCP �ж�1�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0 : 1;  /* bit[0-0]: ������ */
        unsigned char  nack_alarm : 1;  /* bit[1]  : Slaver ����NACK_ALARM�жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
        unsigned char  ack_alarm  : 1;  /* bit[2]  : Slaver ����ACK_ALARM�жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
        unsigned char  crcpar     : 1;  /* bit[3]  : Slaverû�����ݰ������жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
        unsigned char  nack       : 1;  /* bit[4]  : Slaver ����NACK�жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
        unsigned char  reserved_1 : 1;  /* bit[5]  : ������ */
        unsigned char  ack        : 1;  /* bit[6]  : Slaver ����ACK�жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
        unsigned char  cmdcpl     : 1;  /* bit[7]  : FCP����ɹ�����жϣ�
                                                     0���޴��жϣ�
                                                     1�����жϡ� */
    } reg;
} SOC_SCHARGER_ISR1_UNION;
#endif
#define SOC_SCHARGER_ISR1_nack_alarm_START  (1)
#define SOC_SCHARGER_ISR1_nack_alarm_END    (1)
#define SOC_SCHARGER_ISR1_ack_alarm_START   (2)
#define SOC_SCHARGER_ISR1_ack_alarm_END     (2)
#define SOC_SCHARGER_ISR1_crcpar_START      (3)
#define SOC_SCHARGER_ISR1_crcpar_END        (3)
#define SOC_SCHARGER_ISR1_nack_START        (4)
#define SOC_SCHARGER_ISR1_nack_END          (4)
#define SOC_SCHARGER_ISR1_ack_START         (6)
#define SOC_SCHARGER_ISR1_ack_END           (6)
#define SOC_SCHARGER_ISR1_cmdcpl_START      (7)
#define SOC_SCHARGER_ISR1_cmdcpl_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ISR2_UNION
 �ṹ˵��  : ISR2 �Ĵ����ṹ���塣��ַƫ����:0x1A����ֵ:0x00�����:8
 �Ĵ���˵��: FCP �ж�2�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0: 1;  /* bit[0]  : ������ */
        unsigned char  protstat : 1;  /* bit[1]  : Slaver���״̬�仯�жϣ�
                                                   0���޴��жϣ�
                                                   1�����жϡ� */
        unsigned char  reserved_1: 1;  /* bit[2]  : ������ */
        unsigned char  parrx    : 1;  /* bit[3]  : Slaver��������parityУ������жϣ�
                                                   0���޴��жϣ�
                                                   1�����жϡ� */
        unsigned char  crcrx    : 1;  /* bit[4]  : Slaver��������crcУ������жϣ�
                                                   0���޴��жϣ�
                                                   1�����жϡ� */
        unsigned char  reserved_2: 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_ISR2_UNION;
#endif
#define SOC_SCHARGER_ISR2_protstat_START  (1)
#define SOC_SCHARGER_ISR2_protstat_END    (1)
#define SOC_SCHARGER_ISR2_parrx_START     (3)
#define SOC_SCHARGER_ISR2_parrx_END       (3)
#define SOC_SCHARGER_ISR2_crcrx_START     (4)
#define SOC_SCHARGER_ISR2_crcrx_END       (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IMR1_UNION
 �ṹ˵��  : IMR1 �Ĵ����ṹ���塣��ַƫ����:0x1B����ֵ:0x00�����:8
 �Ĵ���˵��: FCP �ж�����1�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0    : 1;  /* bit[0]: ������ */
        unsigned char  nack_alarm_mk : 1;  /* bit[1]: nack_alarm�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  ack_alarm_mk  : 1;  /* bit[2]: ack_alarm�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  crcpar_mk     : 1;  /* bit[3]: crcpar�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  nack_mk       : 1;  /* bit[4]: nack�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  reserved_1    : 1;  /* bit[5]: ������ */
        unsigned char  ack_mk        : 1;  /* bit[6]: ack�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  cmdcpl_mk     : 1;  /* bit[7]: cmdcpl�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
    } reg;
} SOC_SCHARGER_IMR1_UNION;
#endif
#define SOC_SCHARGER_IMR1_nack_alarm_mk_START  (1)
#define SOC_SCHARGER_IMR1_nack_alarm_mk_END    (1)
#define SOC_SCHARGER_IMR1_ack_alarm_mk_START   (2)
#define SOC_SCHARGER_IMR1_ack_alarm_mk_END     (2)
#define SOC_SCHARGER_IMR1_crcpar_mk_START      (3)
#define SOC_SCHARGER_IMR1_crcpar_mk_END        (3)
#define SOC_SCHARGER_IMR1_nack_mk_START        (4)
#define SOC_SCHARGER_IMR1_nack_mk_END          (4)
#define SOC_SCHARGER_IMR1_ack_mk_START         (6)
#define SOC_SCHARGER_IMR1_ack_mk_END           (6)
#define SOC_SCHARGER_IMR1_cmdcpl_mk_START      (7)
#define SOC_SCHARGER_IMR1_cmdcpl_mk_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IMR2_UNION
 �ṹ˵��  : IMR2 �Ĵ����ṹ���塣��ַƫ����:0x1C����ֵ:0x00�����:8
 �Ĵ���˵��: FCP �ж�����2�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved_0  : 1;  /* bit[0]  : ������ */
        unsigned char  protstat_mk : 1;  /* bit[1]  : protstat�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  reserved_1  : 1;  /* bit[2]  : ������ */
        unsigned char  parrx_mk    : 1;  /* bit[3]  : parrx�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  crcrx_mk    : 1;  /* bit[4]  : crcrx�ж����μĴ�����
                                                      0�������Σ�
                                                      1�����Ρ� */
        unsigned char  reserved_2  : 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_IMR2_UNION;
#endif
#define SOC_SCHARGER_IMR2_protstat_mk_START  (1)
#define SOC_SCHARGER_IMR2_protstat_mk_END    (1)
#define SOC_SCHARGER_IMR2_parrx_mk_START     (3)
#define SOC_SCHARGER_IMR2_parrx_mk_END       (3)
#define SOC_SCHARGER_IMR2_crcrx_mk_START     (4)
#define SOC_SCHARGER_IMR2_crcrx_mk_END       (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ5_UNION
 �ṹ˵��  : FCP_IRQ5 �Ĵ����ṹ���塣��ַƫ����:0x1D����ֵ:0x00�����:8
 �Ĵ���˵��: FCP�ж�5�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_set_d60m_int : 1;  /* bit[0]  : ֧�ָ�ѹ����Adapter�жϣ�fcp_set�˲�60ms����������أ��ϱ��жϣ���
                                                           0���޴��жϣ�
                                                           1�����жϡ�
                                                           ע����fcp_cmp_en=0ʱ��ģ���ͳ���fcp_set�ź���0�ġ� */
        unsigned char  fcp_en_det_int   : 1;  /* bit[1]  : fcp_det_en&amp;fcp_cmp_en�ź�Ϊ�ߵ�ƽ��2s�ڣ�û���յ�fcp_set�������жϣ�
                                                           0���޴��жϣ�
                                                           1�����жϡ� */
        unsigned char  reserved         : 6;  /* bit[2-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_IRQ5_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ5_fcp_set_d60m_int_START  (0)
#define SOC_SCHARGER_FCP_IRQ5_fcp_set_d60m_int_END    (0)
#define SOC_SCHARGER_FCP_IRQ5_fcp_en_det_int_START    (1)
#define SOC_SCHARGER_FCP_IRQ5_fcp_en_det_int_END      (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ5_MASK_UNION
 �ṹ˵��  : FCP_IRQ5_MASK �Ĵ����ṹ���塣��ַƫ����:0x1E����ֵ:0x00�����:8
 �Ĵ���˵��: FCP�ж�����5�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_set_d60m_r_mk : 1;  /* bit[0]  : 0��ʹ�ܸ��жϣ�
                                                            1�����θ��жϡ� */
        unsigned char  fcp_en_det_int_mk : 1;  /* bit[1]  : 0��ʹ�ܸ��жϣ�
                                                            1�����θ��жϡ� */
        unsigned char  reserved          : 6;  /* bit[2-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_IRQ5_MASK_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ5_MASK_fcp_set_d60m_r_mk_START  (0)
#define SOC_SCHARGER_FCP_IRQ5_MASK_fcp_set_d60m_r_mk_END    (0)
#define SOC_SCHARGER_FCP_IRQ5_MASK_fcp_en_det_int_mk_START  (1)
#define SOC_SCHARGER_FCP_IRQ5_MASK_fcp_en_det_int_mk_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_CTRL_UNION
 �ṹ˵��  : FCP_CTRL �Ĵ����ṹ���塣��ַƫ����:0x1F����ֵ:0x00�����:8
 �Ĵ���˵��: ��ѹ�����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_clk_test : 1;  /* bit[0]  : ��ѹ���Ĳ���ģʽѡ��
                                                       0����������ģʽ��
                                                       1������ģʽ�� */
        unsigned char  fcp_mode     : 1;  /* bit[1]  : ��ѹ���ģʽѡ��
                                                       0��ģʽ1��
                                                       1��ģʽ2�� */
        unsigned char  fcp_cmp_en   : 1;  /* bit[2]  : ��ѹ���Э����Ƚ���ʹ�ܣ�
                                                       0���رռ��Ƚ�����
                                                       1�����Ƚ�����
                                                       ��ע�����Ź������쳣��üĴ����ᱻ��0���� */
        unsigned char  fcp_det_en   : 1;  /* bit[3]  : ��ѹ���Э����ʹ�ܣ�
                                                       0���رտ��Э���⹦�ܣ�
                                                       1���������Э���⹦�ܡ�
                                                       ��ע�����Ź������쳣��üĴ����ᱻ��0���� */
        unsigned char  reserved     : 4;  /* bit[4-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_CTRL_UNION;
#endif
#define SOC_SCHARGER_FCP_CTRL_fcp_clk_test_START  (0)
#define SOC_SCHARGER_FCP_CTRL_fcp_clk_test_END    (0)
#define SOC_SCHARGER_FCP_CTRL_fcp_mode_START      (1)
#define SOC_SCHARGER_FCP_CTRL_fcp_mode_END        (1)
#define SOC_SCHARGER_FCP_CTRL_fcp_cmp_en_START    (2)
#define SOC_SCHARGER_FCP_CTRL_fcp_cmp_en_END      (2)
#define SOC_SCHARGER_FCP_CTRL_fcp_det_en_START    (3)
#define SOC_SCHARGER_FCP_CTRL_fcp_det_en_END      (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_MODE2_SET_UNION
 �ṹ˵��  : FCP_MODE2_SET �Ĵ����ṹ���塣��ַƫ����:0x20����ֵ:0x00�����:8
 �Ĵ���˵��: ��ѹ���Э��ģʽ2��λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_mod2_set : 2;  /* bit[0-1]: ��ѹ���Э��ģʽ2��λ���ƣ�
                                                       00����0.6V,0V)
                                                       01: (3.3V,0.6V)
                                                       10: (0.6V,0.6V)
                                                       11: (3.3V,3.3V) */
        unsigned char  reserved     : 6;  /* bit[2-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_MODE2_SET_UNION;
#endif
#define SOC_SCHARGER_FCP_MODE2_SET_fcp_mod2_set_START  (0)
#define SOC_SCHARGER_FCP_MODE2_SET_fcp_mod2_set_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_ADAP_CTRL_UNION
 �ṹ˵��  : FCP_ADAP_CTRL �Ĵ����ṹ���塣��ַƫ����:0x21����ֵ:0x00�����:8
 �Ĵ���˵��: ��ѹ���Adapter���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_set_d60m_r : 1;  /* bit[0]  : ��ѹ����Adapter�жϣ�fcp_set_d60m_r�ж�״̬�Ĵ�������
                                                         0����֧�ָ�ѹ����Adapter��
                                                         1��֧�ָ�ѹ����Adapter�� */
        unsigned char  reserved       : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_ADAP_CTRL_UNION;
#endif
#define SOC_SCHARGER_FCP_ADAP_CTRL_fcp_set_d60m_r_START  (0)
#define SOC_SCHARGER_FCP_ADAP_CTRL_fcp_set_d60m_r_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RDATA_READY_UNION
 �ṹ˵��  : RDATA_READY �Ĵ����ṹ���塣��ַƫ����:0x22����ֵ:0x00�����:8
 �Ĵ���˵��: slave�������ݲɼ���ָʾ�źš�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_rdata_ready : 1;  /* bit[0]  : I2Cͨ��master���ص�slaver�ļĴ���ֵ׼����ָʾ�źš� */
        unsigned char  reserved        : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_RDATA_READY_UNION;
#endif
#define SOC_SCHARGER_RDATA_READY_fcp_rdata_ready_START  (0)
#define SOC_SCHARGER_RDATA_READY_fcp_rdata_ready_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_SOFT_RST_CTRL_UNION
 �ṹ˵��  : FCP_SOFT_RST_CTRL �Ĵ����ṹ���塣��ַƫ����:0x23����ֵ:0x5A�����:8
 �Ĵ���˵��: FCP��λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_soft_rst_cfg : 8;  /* bit[0-7]: FCP��λ���üĴ�������λFCP�����߼����͵�ƽ��Ч��
                                                           ��д��0x5Aʱ����λΪ�ߵ�ƽ������λ��
                                                           ��д��0xACʱ����λΪ�͵�ƽ����λFCP�����߼���
                                                           ��д������ֵ����λ���ֵ�����䣬���֣� */
    } reg;
} SOC_SCHARGER_FCP_SOFT_RST_CTRL_UNION;
#endif
#define SOC_SCHARGER_FCP_SOFT_RST_CTRL_fcp_soft_rst_cfg_START  (0)
#define SOC_SCHARGER_FCP_SOFT_RST_CTRL_fcp_soft_rst_cfg_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RDATA_PARITY_ERR_UNION
 �ṹ˵��  : FCP_RDATA_PARITY_ERR �Ĵ����ṹ���塣��ַƫ����:0x25����ֵ:0xff�����:8
 �Ĵ���˵��: FCP������Parity����ض��Ĵ���
            ˵�������Բ�Ʒ����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rdata_parity_err0 : 1;  /* bit[0]: burst����ģʽ��Slaver���ض����ݵĵ�0byte parityУ�顣 */
        unsigned char  rdata_parity_err1 : 1;  /* bit[1]: burst����ģʽ��Slaver���ض����ݵĵ�1byte parityУ�顣 */
        unsigned char  rdata_parity_err2 : 1;  /* bit[2]: burst����ģʽ��Slaver���ض����ݵĵ�2byte parityУ�顣 */
        unsigned char  rdata_parity_err3 : 1;  /* bit[3]: burst����ģʽ��Slaver���ض����ݵĵ�3byte parityУ�顣 */
        unsigned char  rdata_parity_err4 : 1;  /* bit[4]: burst����ģʽ��Slaver���ض����ݵĵ�4byte parityУ�顣 */
        unsigned char  rdata_parity_err5 : 1;  /* bit[5]: burst����ģʽ��Slaver���ض����ݵĵ�5byte parityУ�顣 */
        unsigned char  rdata_parity_err6 : 1;  /* bit[6]: burst����ģʽ��Slaver���ض����ݵĵ�6byte parityУ�顣 */
        unsigned char  rdata_parity_err7 : 1;  /* bit[7]: burst����ģʽ��Slaver���ض����ݵĵ�7byte parityУ�顣 */
    } reg;
} SOC_SCHARGER_FCP_RDATA_PARITY_ERR_UNION;
#endif
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err0_START  (0)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err0_END    (0)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err1_START  (1)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err1_END    (1)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err2_START  (2)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err2_END    (2)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err3_START  (3)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err3_END    (3)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err4_START  (4)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err4_END    (4)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err5_START  (5)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err5_END    (5)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err6_START  (6)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err6_END    (6)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err7_START  (7)
#define SOC_SCHARGER_FCP_RDATA_PARITY_ERR_rdata_parity_err7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_INIT_RETRY_CFG_UNION
 �ṹ˵��  : FCP_INIT_RETRY_CFG �Ĵ����ṹ���塣��ַƫ����:0x26����ֵ:0x0F�����:8
 �Ĵ���˵��: FCP��ʼ��Retry���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_init_retry_cfg : 5;  /* bit[0-4]: FCP��ʼ��Retry���üĴ��� */
        unsigned char  reserved           : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_FCP_INIT_RETRY_CFG_UNION;
#endif
#define SOC_SCHARGER_FCP_INIT_RETRY_CFG_fcp_init_retry_cfg_START  (0)
#define SOC_SCHARGER_FCP_INIT_RETRY_CFG_fcp_init_retry_cfg_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_SOFT_RST_CTRL_UNION
 �ṹ˵��  : HKADC_SOFT_RST_CTRL �Ĵ����ṹ���塣��ַƫ����:0x28����ֵ:0x5A�����:8
 �Ĵ���˵��: HKADC��λ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_soft_rst_n : 8;  /* bit[0-7]: HKADC��λ���üĴ�������λHKADC�����߼����͵�ƽ��Ч��
                                                              ��д��0x5Aʱ����λΪ�ߵ�ƽ������λ��
                                                              ��д��0xACʱ����λΪ�͵�ƽ����λHKADC�����߼���
                                                              ��д������ֵ����λ���ֵ�����䣬���֣� */
    } reg;
} SOC_SCHARGER_HKADC_SOFT_RST_CTRL_UNION;
#endif
#define SOC_SCHARGER_HKADC_SOFT_RST_CTRL_sc_hkadc_soft_rst_n_START  (0)
#define SOC_SCHARGER_HKADC_SOFT_RST_CTRL_sc_hkadc_soft_rst_n_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CTRL0_UNION
 �ṹ˵��  : HKADC_CTRL0 �Ĵ����ṹ���塣��ַƫ����:0x29����ֵ:0x10�����:8
 �Ĵ���˵��: HKADC���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_chanel_sel : 4;  /* bit[0-3]: �;���HKADCͨ��ѡ���źţ�
                                                              4'b0000��VUSB�����ѹ��Ĭ�ϣ���
                                                              4'b0001��VBUS���������
                                                              4'b0010��VBUS�����ѹ��
                                                              4'b0011:��������ѹ(VOUT)��
                                                              4'b0100����������ѹ��VBAT)��
                                                              4'b0101����ص�����
                                                              4'b0110��ACR��ѹ��
                                                              4'b0111��ACR������
                                                              4'b1000��VBUS����ο�������
                                                              4'b1001�����ã�
                                                              4'b1010��DPLUS��DMINUS��ѹ��⣻
                                                              4'b1011���ⲿ�����¶ȼ�⣨TSBAT)��
                                                              4'b1100���ⲿ�����¶ȼ�⣨TSBUS)��
                                                              4'b1101:�ⲿ�����¶ȼ�⣨TSCHIP)��
                                                              4'b1110:REF��ѹ0.1V����gain error��offsetУ����
                                                              4'b1111��REF��ѹ2.45V����gain error��offsetУ���� */
        unsigned char  sc_hkadc_seq_conv   : 1;  /* bit[4-4]: HKADCת��ģʽѡ��
                                                              1'b0���;��ȵ���ת����
                                                              1'b1���߾��ȵ��ζ�ͨ��ת��(Ĭ��) */
        unsigned char  sc_hkadc_seq_loop   : 1;  /* bit[5-5]: ��ͨ��ѭ����ѯʹ���źţ�
                                                              1'b0����ʹ�ܶ�ͨ��ѭ����ѯ��Ĭ�ϣ�
                                                              1'b1��ʹ�ܶ�ͨ��ѭ����ѯ */
        unsigned char  sc_hkadc_cali_ref   : 1;  /* bit[6-6]: �Ƿ����ʵʱУ��ѡ���źţ�
                                                              1'b0����/�;���HKADCת����Ĭ�ϣ�
                                                              1'b1��ʵʱУ�� */
        unsigned char  sc_hkadc_en         : 1;  /* bit[7-7]: HKADCʹ���źţ�
                                                              1'b0����ʹ�ܣ�Ĭ�ϣ���
                                                              1'b1��ʹ�� */
    } reg;
} SOC_SCHARGER_HKADC_CTRL0_UNION;
#endif
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_chanel_sel_START  (0)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_chanel_sel_END    (3)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_seq_conv_START    (4)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_seq_conv_END      (4)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_seq_loop_START    (5)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_seq_loop_END      (5)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_cali_ref_START    (6)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_cali_ref_END      (6)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_en_START          (7)
#define SOC_SCHARGER_HKADC_CTRL0_sc_hkadc_en_END            (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_START_UNION
 �ṹ˵��  : HKADC_START �Ĵ����ṹ���塣��ַƫ����:0x2A����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC START�ź�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_start_cfg : 1;  /* bit[0-0]: HKADC�������üĴ�����Ĭ��0��дһ�ε�ַ0xB1������0��1�����üĴ�������һ�η�ת������һ������ת��
                                                             ��ע����ֹ��HKADC����hkadc_start��ת��������(hkadc_valid=0)�ظ�����hkadc_start�źţ�����hkadc�ڲ��߼��ᷢ�����ң���Ҫ���¿���hkadc�����hkadc_reset�á�1��ǿ���߼���λ��
                                                             ע����Ҫ���ٳ���3��hkadc����ʱ��clk���ڣ�����ƵΪ3��125kʱ�ӣ���ƵΪ3��62.5kʱ�ӣ��������� */
        unsigned char  reserved           : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_START_UNION;
#endif
#define SOC_SCHARGER_HKADC_START_sc_hkadc_start_cfg_START  (0)
#define SOC_SCHARGER_HKADC_START_sc_hkadc_start_cfg_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CTRL1_UNION
 �ṹ˵��  : HKADC_CTRL1 �Ĵ����ṹ���塣��ַƫ����:0x2B����ֵ:0x27�����:8
 �Ĵ���˵��: HKADC���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_avg_times     : 2;  /* bit[0-1]: HKADC������ƽ������ѡ��
                                                                 2'b00��2�Σ�
                                                                 2'b01��4�Σ�
                                                                 2'b10��8�Σ�
                                                                 2'b11��16�Σ�Ĭ�ϣ���
                                                                 ˵����ACRģʽ��ֻ��������Ϊ2'b00��2'b01 */
        unsigned char  sc_hkadc_cali_set      : 1;  /* bit[2-2]: HKADCת�������������Ƿ�Ҫ����У�����㣺
                                                                 1'b0������Ҫ���м��㣻
                                                                 1'b1����Ҫ���м��㣨Ĭ�ϣ� */
        unsigned char  sc_hkadc_cali_realtime : 1;  /* bit[3-3]: HKADC�������У��ϵ������Դѡ��
                                                                 1'b0��ѡ��efuse���ݣ�Ĭ�ϣ���
                                                                 1'b1��ѡ��Ĵ������� */
        unsigned char  sc_hkadc_chopper       : 1;  /* bit[4-4]: HKADC chopperָʾ�źţ�
                                                                 1'b0����������ģʽ��Ĭ�ϣ�
                                                                 1'b1����������ģʽ */
        unsigned char  sc_hkadc_chopper_time  : 1;  /* bit[5-5]: Chopperģʽ�ø�da_hkadc_chopper����һ��ת���ĵȴ�ʱ�䣺
                                                                 1'b0��6us��
                                                                 1'b1��12.5us��Ĭ�ϣ� */
        unsigned char  sc_hkadc_chopper_en    : 1;  /* bit[6-6]: hkadc chopper����ʹ���źţ�
                                                                 1'b0����ʹ��chopper���ܣ�Ĭ�ϣ�
                                                                 1'b1��ʹ��chopper���� */
        unsigned char  sc_hkadc_cul_time      : 1;  /* bit[7-7]: HKADC��������ʱ�����ã�
                                                                 1'b0����������ʱ��Ϊ25us��Ĭ�ϣ�
                                                                 1'b1����������ʱ��Ϊ28us */
    } reg;
} SOC_SCHARGER_HKADC_CTRL1_UNION;
#endif
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_avg_times_START      (0)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_avg_times_END        (1)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cali_set_START       (2)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cali_set_END         (2)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cali_realtime_START  (3)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cali_realtime_END    (3)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_START        (4)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_END          (4)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_time_START   (5)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_time_END     (5)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_en_START     (6)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_chopper_en_END       (6)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cul_time_START       (7)
#define SOC_SCHARGER_HKADC_CTRL1_sc_hkadc_cul_time_END         (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_SEQ_CH_H_UNION
 �ṹ˵��  : HKADC_SEQ_CH_H �Ĵ����ṹ���塣��ַƫ����:0x2C����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC��ѯ��־λ��8bit���߾��ȣ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_seq_chanel_h : 8;  /* bit[0-7]: �߾�����ѯ��־λ����ͨ����Ӧ������Ϊ1��ͨ���Ų�����ѯ
                                                                bit[0]��VBUS����ο�������
                                                                bit[1]��reserved��
                                                                bit[2]��DPLUS��DMINUS��ѹ��⣻
                                                                bit[3]���ⲿ�����¶ȼ�⣨TSBAT)��
                                                                bit[4]���ⲿ�����¶ȼ�⣨TSBUS)��
                                                                bit[5]:�ⲿ�����¶ȼ�⣨TSCHIP)��
                                                                bit[6]:REF��ѹ0.1V����gain error��offsetУ����
                                                                bit[7]��REF��ѹ2.45V����gain error��offsetУ���� */
    } reg;
} SOC_SCHARGER_HKADC_SEQ_CH_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_SEQ_CH_H_sc_hkadc_seq_chanel_h_START  (0)
#define SOC_SCHARGER_HKADC_SEQ_CH_H_sc_hkadc_seq_chanel_h_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_SEQ_CH_L_UNION
 �ṹ˵��  : HKADC_SEQ_CH_L �Ĵ����ṹ���塣��ַƫ����:0x2D����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC��ѯ��־λ��8bit���߾��ȣ�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_seq_chanel_l : 8;  /* bit[0-7]: ��ѯ��־λ����ͨ����Ӧ������Ϊ1��ͨ���Ų�����ѯ
                                                                bit[0]��VUSB�����ѹ��Ĭ�ϣ���
                                                                bit[1]��VBUS���������
                                                                bit[2]��VBUS�����ѹ��
                                                                bit[3]:��������ѹ(VOUT)��
                                                                bit[4]����������ѹ��VBAT)��
                                                                bit[5]����ص�����
                                                                bit[6]��ACR��ѹ��
                                                                bit[7]��reserved */
    } reg;
} SOC_SCHARGER_HKADC_SEQ_CH_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_SEQ_CH_L_sc_hkadc_seq_chanel_l_START  (0)
#define SOC_SCHARGER_HKADC_SEQ_CH_L_sc_hkadc_seq_chanel_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CTRL2_UNION
 �ṹ˵��  : HKADC_CTRL2 �Ĵ����ṹ���塣��ַƫ����:0x2E����ֵ:0x10�����:8
 �Ĵ���˵��: HKADC���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_test_mode : 1;  /* bit[0-0]: ��ع�ѹ����ʱ�䵵λ����ģʽ������ʹ�ã����Բ�Ʒ���ţ���
                                                           1'b0������ģʽ��ʱ�䵵λΪ10min��20min��30min��40min�� ��Ĭ�ϣ�
                                                           1'b1������ģʽ��ʱ�䵵λΪ30s */
        unsigned char  reserved_0       : 3;  /* bit[1-3]: reserved */
        unsigned char  sc_sohov_timer   : 2;  /* bit[4-5]: ��ع�ѹ������ʱ����λ�����źţ�
                                                           2'b00��10min��
                                                           2'b01��20min��Ĭ�ϣ���
                                                           2'b10��30min��
                                                           2'bb11��40min�� */
        unsigned char  reserved_1       : 1;  /* bit[6-6]: reserved */
        unsigned char  sc_sohov_en      : 1;  /* bit[7-7]: ��ع�ѹ����ʹ���źţ�
                                                           1'b0����ʹ�ܣ�Ĭ�ϣ�
                                                           1'b1��ʹ�� */
    } reg;
} SOC_SCHARGER_HKADC_CTRL2_UNION;
#endif
#define SOC_SCHARGER_HKADC_CTRL2_sc_soh_test_mode_START  (0)
#define SOC_SCHARGER_HKADC_CTRL2_sc_soh_test_mode_END    (0)
#define SOC_SCHARGER_HKADC_CTRL2_sc_sohov_timer_START    (4)
#define SOC_SCHARGER_HKADC_CTRL2_sc_sohov_timer_END      (5)
#define SOC_SCHARGER_HKADC_CTRL2_sc_sohov_en_START       (7)
#define SOC_SCHARGER_HKADC_CTRL2_sc_sohov_en_END         (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_DISCHG_EN_UNION
 �ṹ˵��  : SOH_DISCHG_EN �Ĵ����ṹ���塣��ַƫ����:0x2F����ֵ:0xAC�����:8
 �Ĵ���˵��: �ŵ���ƼĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_dischg_en : 8;  /* bit[0-7]: �ŵ�����ź����üĴ��������Ʒŵ磬�ߵ�ƽ��ʾ�ŵ硣
                                                           ��д��0x5Aʱ�򣬷ŵ�����ź�Ϊ�ߵ�ƽ�����зŵ磻
                                                           ��д��0xACʱ�򣬷ŵ�����ź�Ϊ�͵�ƽ�������зŵ硣
                                                           ��д������ֵ���ŵ�����ź����ֵ�����䣬���֣� */
    } reg;
} SOC_SCHARGER_SOH_DISCHG_EN_UNION;
#endif
#define SOC_SCHARGER_SOH_DISCHG_EN_sc_soh_dischg_en_START  (0)
#define SOC_SCHARGER_SOH_DISCHG_EN_sc_soh_dischg_en_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_CTRL_UNION
 �ṹ˵��  : ACR_CTRL �Ĵ����ṹ���塣��ַƫ����:0x30����ֵ:0x01�����:8
 �Ĵ���˵��: ACR ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_acr_pulse_num : 2;  /* bit[0-1]: acr_pulse��һ��en_pulse�ߵ�ƽ�ڼ��ڵ�������n_acr��
                                                           2��b00��8��
                                                           2��b01��16��Ĭ�ϣ���
                                                           2��b10��24��
                                                           2��b11��32�� */
        unsigned char  reserved_0       : 2;  /* bit[2-3]: reserved */
        unsigned char  sc_acr_en        : 1;  /* bit[4-4]: ACRʹ���źţ�
                                                           1'b0����ʹ�ܣ�Ĭ�ϣ���
                                                           1'b1��ʹ�ܡ� */
        unsigned char  reserved_1       : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_CTRL_UNION;
#endif
#define SOC_SCHARGER_ACR_CTRL_sc_acr_pulse_num_START  (0)
#define SOC_SCHARGER_ACR_CTRL_sc_acr_pulse_num_END    (1)
#define SOC_SCHARGER_ACR_CTRL_sc_acr_en_START         (4)
#define SOC_SCHARGER_ACR_CTRL_sc_acr_en_END           (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_RD_SEQ_UNION
 �ṹ˵��  : HKADC_RD_SEQ �Ĵ����ṹ���塣��ַƫ����:0x31����ֵ:0x00�����:8
 �Ĵ���˵��: HKADCѭ����ѯģʽ���ݶ������ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_rd_req : 1;  /* bit[0-0]: ��ͨ��ѭ���������źţ�SOC�Ը��źŶ�Ӧ��ַ����һ��д�����������ڲ��ź�hkadc_rd_req����һ�η�ת��ʹ��hkadc_rd_req�źŵ�˫�ض����ݽ������մ��� */
        unsigned char  reserved        : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_RD_SEQ_UNION;
#endif
#define SOC_SCHARGER_HKADC_RD_SEQ_sc_hkadc_rd_req_START  (0)
#define SOC_SCHARGER_HKADC_RD_SEQ_sc_hkadc_rd_req_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PULSE_NON_CHG_FLAG_UNION
 �ṹ˵��  : PULSE_NON_CHG_FLAG �Ĵ����ṹ���塣��ַƫ����:0x32����ֵ:0x00�����:8
 �Ĵ���˵��: ��ǰADC�����Ǵ��ڳ��״̬���Ƿǳ��״̬
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pulse_non_chg_flag : 1;  /* bit[0-0]: ��ǰADC�����Ǵ��ڳ��״̬���Ƿǳ��״̬��
                                                             1'b0����ǰ������������ģʽ�ǳ��״̬
                                                             1'b1����ǰ����������ģʽ�ǳ��״̬�� */
        unsigned char  reserved_0         : 3;  /* bit[1-3]: reserved */
        unsigned char  hkadc_data_valid   : 1;  /* bit[4-4]: HKADC���ݲ������ָʾ�źţ�
                                                             1��b0��δ��ʼת�������ڽ�������ת����
                                                             1��b1������ת����� */
        unsigned char  reserved_1         : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_PULSE_NON_CHG_FLAG_UNION;
#endif
#define SOC_SCHARGER_PULSE_NON_CHG_FLAG_pulse_non_chg_flag_START  (0)
#define SOC_SCHARGER_PULSE_NON_CHG_FLAG_pulse_non_chg_flag_END    (0)
#define SOC_SCHARGER_PULSE_NON_CHG_FLAG_hkadc_data_valid_START    (4)
#define SOC_SCHARGER_PULSE_NON_CHG_FLAG_hkadc_data_valid_END      (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VUSB_ADC_L_UNION
 �ṹ˵��  : VUSB_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x33����ֵ:0x00�����:8
 �Ĵ���˵��: VUSB���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vusb_adc_l : 8;  /* bit[0-7]: VUSB���������8bit */
    } reg;
} SOC_SCHARGER_VUSB_ADC_L_UNION;
#endif
#define SOC_SCHARGER_VUSB_ADC_L_vusb_adc_l_START  (0)
#define SOC_SCHARGER_VUSB_ADC_L_vusb_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VUSB_ADC_H_UNION
 �ṹ˵��  : VUSB_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x34����ֵ:0x00�����:8
 �Ĵ���˵��: VUSB���������6bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vusb_adc_h : 6;  /* bit[0-5]: VUSB���������6bit */
        unsigned char  reserved   : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_VUSB_ADC_H_UNION;
#endif
#define SOC_SCHARGER_VUSB_ADC_H_vusb_adc_h_START  (0)
#define SOC_SCHARGER_VUSB_ADC_H_vusb_adc_h_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IBUS_ADC_L_UNION
 �ṹ˵��  : IBUS_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x35����ֵ:0x00�����:8
 �Ĵ���˵��: IBUS���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibus_adc_l : 8;  /* bit[0-7]: IBUS���������8bit */
    } reg;
} SOC_SCHARGER_IBUS_ADC_L_UNION;
#endif
#define SOC_SCHARGER_IBUS_ADC_L_ibus_adc_l_START  (0)
#define SOC_SCHARGER_IBUS_ADC_L_ibus_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IBUS_ADC_H_UNION
 �ṹ˵��  : IBUS_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x36����ֵ:0x00�����:8
 �Ĵ���˵��: IBUS���������5bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibus_adc_h : 5;  /* bit[0-4]: IBUS���������5bit */
        unsigned char  reserved   : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_IBUS_ADC_H_UNION;
#endif
#define SOC_SCHARGER_IBUS_ADC_H_ibus_adc_h_START  (0)
#define SOC_SCHARGER_IBUS_ADC_H_ibus_adc_h_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VBUS_ADC_L_UNION
 �ṹ˵��  : VBUS_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x37����ֵ:0x00�����:8
 �Ĵ���˵��: VBUS���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_adc_l : 8;  /* bit[0-7]: VBUS���������8bit */
    } reg;
} SOC_SCHARGER_VBUS_ADC_L_UNION;
#endif
#define SOC_SCHARGER_VBUS_ADC_L_vbus_adc_l_START  (0)
#define SOC_SCHARGER_VBUS_ADC_L_vbus_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VBUS_ADC_H_UNION
 �ṹ˵��  : VBUS_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x38����ֵ:0x00�����:8
 �Ĵ���˵��: VBUS���������6bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbus_adc_h : 6;  /* bit[0-5]: VBUS���������6bit */
        unsigned char  reserved   : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_VBUS_ADC_H_UNION;
#endif
#define SOC_SCHARGER_VBUS_ADC_H_vbus_adc_h_START  (0)
#define SOC_SCHARGER_VBUS_ADC_H_vbus_adc_h_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VOUT_ADC_L_UNION
 �ṹ˵��  : VOUT_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x39����ֵ:0x00�����:8
 �Ĵ���˵��: VOUT���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vout_adc_l : 8;  /* bit[0-7]: VOUT���������8bit */
    } reg;
} SOC_SCHARGER_VOUT_ADC_L_UNION;
#endif
#define SOC_SCHARGER_VOUT_ADC_L_vout_adc_l_START  (0)
#define SOC_SCHARGER_VOUT_ADC_L_vout_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VOUT_ADC_H_UNION
 �ṹ˵��  : VOUT_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x3A����ֵ:0x00�����:8
 �Ĵ���˵��: VOUT���������5bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vout_adc_h : 5;  /* bit[0-4]: VOUT���������5bit */
        unsigned char  reserved   : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_VOUT_ADC_H_UNION;
#endif
#define SOC_SCHARGER_VOUT_ADC_H_vout_adc_h_START  (0)
#define SOC_SCHARGER_VOUT_ADC_H_vout_adc_h_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VBAT_ADC_L_UNION
 �ṹ˵��  : VBAT_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x3B����ֵ:0x00�����:8
 �Ĵ���˵��: VBAT���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbat_adc_l : 8;  /* bit[0-7]: VBAT���������8bit */
    } reg;
} SOC_SCHARGER_VBAT_ADC_L_UNION;
#endif
#define SOC_SCHARGER_VBAT_ADC_L_vbat_adc_l_START  (0)
#define SOC_SCHARGER_VBAT_ADC_L_vbat_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VBAT_ADC_H_UNION
 �ṹ˵��  : VBAT_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x3C����ֵ:0x00�����:8
 �Ĵ���˵��: VBAT���������5bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbat_adc_h : 5;  /* bit[0-4]: VBAT���������5bit */
        unsigned char  reserved   : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_VBAT_ADC_H_UNION;
#endif
#define SOC_SCHARGER_VBAT_ADC_H_vbat_adc_h_START  (0)
#define SOC_SCHARGER_VBAT_ADC_H_vbat_adc_h_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IBAT_ADC_L_UNION
 �ṹ˵��  : IBAT_ADC_L �Ĵ����ṹ���塣��ַƫ����:0x3D����ֵ:0x00�����:8
 �Ĵ���˵��: IBAT���������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibat_adc_l : 8;  /* bit[0-7]: IBAT���������8bit */
    } reg;
} SOC_SCHARGER_IBAT_ADC_L_UNION;
#endif
#define SOC_SCHARGER_IBAT_ADC_L_ibat_adc_l_START  (0)
#define SOC_SCHARGER_IBAT_ADC_L_ibat_adc_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IBAT_ADC_H_UNION
 �ṹ˵��  : IBAT_ADC_H �Ĵ����ṹ���塣��ַƫ����:0x3E����ֵ:0x00�����:8
 �Ĵ���˵��: IBAT���������6bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ibat_adc_h : 6;  /* bit[0-5]: IBAT���������6bit */
        unsigned char  reserved   : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_IBAT_ADC_H_UNION;
#endif
#define SOC_SCHARGER_IBAT_ADC_H_ibat_adc_h_START  (0)
#define SOC_SCHARGER_IBAT_ADC_H_ibat_adc_h_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_6_L_UNION
 �ṹ˵��  : HKADC_DATA_6_L �Ĵ����ṹ���塣��ַƫ����:0x3F����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��6 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_6_l : 8;  /* bit[0-7]: ͨ��6 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_6_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_6_L_hkadc_data_6_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_6_L_hkadc_data_6_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_6_H_UNION
 �ṹ˵��  : HKADC_DATA_6_H �Ĵ����ṹ���塣��ַƫ����:0x40����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��6 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_6_h : 4;  /* bit[0-3]: ͨ��6 ADCת�������4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_6_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_6_H_hkadc_data_6_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_6_H_hkadc_data_6_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_7_L_UNION
 �ṹ˵��  : HKADC_DATA_7_L �Ĵ����ṹ���塣��ַƫ����:0x41����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��7 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_7_l : 8;  /* bit[0-7]: ͨ��7 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_7_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_7_L_hkadc_data_7_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_7_L_hkadc_data_7_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_7_H_UNION
 �ṹ˵��  : HKADC_DATA_7_H �Ĵ����ṹ���塣��ַƫ����:0x42����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��7 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_7_h : 4;  /* bit[0-3]: ͨ��7 ADCת�������4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_7_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_7_H_hkadc_data_7_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_7_H_hkadc_data_7_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_8_L_UNION
 �ṹ˵��  : HKADC_DATA_8_L �Ĵ����ṹ���塣��ַƫ����:0x43����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��8 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_8_l : 8;  /* bit[0-7]: ͨ��8 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_8_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_8_L_hkadc_data_8_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_8_L_hkadc_data_8_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_8_H_UNION
 �ṹ˵��  : HKADC_DATA_8_H �Ĵ����ṹ���塣��ַƫ����:0x44����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��8 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_8_h : 4;  /* bit[0-3]: ͨ��8 ADCת�������4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_8_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_8_H_hkadc_data_8_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_8_H_hkadc_data_8_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_9_L_UNION
 �ṹ˵��  : HKADC_DATA_9_L �Ĵ����ṹ���塣��ַƫ����:0x45����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��9 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_9_l : 8;  /* bit[0-7]: ͨ��9 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_9_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_9_L_hkadc_data_9_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_9_L_hkadc_data_9_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_9_H_UNION
 �ṹ˵��  : HKADC_DATA_9_H �Ĵ����ṹ���塣��ַƫ����:0x46����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��9 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_9_h : 4;  /* bit[0-3]: ͨ��9 ADCת�������4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_9_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_9_H_hkadc_data_9_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_9_H_hkadc_data_9_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_10_L_UNION
 �ṹ˵��  : HKADC_DATA_10_L �Ĵ����ṹ���塣��ַƫ����:0x47����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��10 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_10_l : 8;  /* bit[0-7]: ͨ��10 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_10_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_10_L_hkadc_data_10_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_10_L_hkadc_data_10_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_10_H_UNION
 �ṹ˵��  : HKADC_DATA_10_H �Ĵ����ṹ���塣��ַƫ����:0x48����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��10 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_10_h : 4;  /* bit[0-3]: ͨ��10 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_10_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_10_H_hkadc_data_10_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_10_H_hkadc_data_10_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_11_L_UNION
 �ṹ˵��  : HKADC_DATA_11_L �Ĵ����ṹ���塣��ַƫ����:0x49����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��11 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_11_l : 8;  /* bit[0-7]: ͨ��11 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_11_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_11_L_hkadc_data_11_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_11_L_hkadc_data_11_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_11_H_UNION
 �ṹ˵��  : HKADC_DATA_11_H �Ĵ����ṹ���塣��ַƫ����:0x4A����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��11 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_11_h : 4;  /* bit[0-3]: ͨ��11 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_11_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_11_H_hkadc_data_11_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_11_H_hkadc_data_11_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_12_L_UNION
 �ṹ˵��  : HKADC_DATA_12_L �Ĵ����ṹ���塣��ַƫ����:0x4B����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��12 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_12_l : 8;  /* bit[0-7]: ͨ��12 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_12_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_12_L_hkadc_data_12_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_12_L_hkadc_data_12_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_12_H_UNION
 �ṹ˵��  : HKADC_DATA_12_H �Ĵ����ṹ���塣��ַƫ����:0x4C����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��12 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_12_h : 4;  /* bit[0-3]: ͨ��12 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_12_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_12_H_hkadc_data_12_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_12_H_hkadc_data_12_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_13_L_UNION
 �ṹ˵��  : HKADC_DATA_13_L �Ĵ����ṹ���塣��ַƫ����:0x4D����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��13 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_13_l : 8;  /* bit[0-7]: ͨ��13 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_13_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_13_L_hkadc_data_13_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_13_L_hkadc_data_13_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_13_H_UNION
 �ṹ˵��  : HKADC_DATA_13_H �Ĵ����ṹ���塣��ַƫ����:0x4E����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��13 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_13_h : 4;  /* bit[0-3]: ͨ��13 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_13_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_13_H_hkadc_data_13_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_13_H_hkadc_data_13_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_14_L_UNION
 �ṹ˵��  : HKADC_DATA_14_L �Ĵ����ṹ���塣��ַƫ����:0x4F����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��14 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_14_l : 8;  /* bit[0-7]: ͨ��14 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_14_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_14_L_hkadc_data_14_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_14_L_hkadc_data_14_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_14_H_UNION
 �ṹ˵��  : HKADC_DATA_14_H �Ĵ����ṹ���塣��ַƫ����:0x50����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��14 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_14_h : 4;  /* bit[0-3]: ͨ��14 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_14_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_14_H_hkadc_data_14_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_14_H_hkadc_data_14_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_15_L_UNION
 �ṹ˵��  : HKADC_DATA_15_L �Ĵ����ṹ���塣��ַƫ����:0x51����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��15 ADCת�������8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_15_l : 8;  /* bit[0-7]: ͨ��15 ADCת�������8bit */
    } reg;
} SOC_SCHARGER_HKADC_DATA_15_L_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_15_L_hkadc_data_15_l_START  (0)
#define SOC_SCHARGER_HKADC_DATA_15_L_hkadc_data_15_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_DATA_15_H_UNION
 �ṹ˵��  : HKADC_DATA_15_H �Ĵ����ṹ���塣��ַƫ����:0x52����ֵ:0x00�����:8
 �Ĵ���˵��: ͨ��15 ADCת�������4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_data_15_h : 4;  /* bit[0-3]: ͨ��15 ADCת�������4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_DATA_15_H_UNION;
#endif
#define SOC_SCHARGER_HKADC_DATA_15_H_hkadc_data_15_h_START  (0)
#define SOC_SCHARGER_HKADC_DATA_15_H_hkadc_data_15_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_CDR_CFG_0_UNION
 �ṹ˵��  : PD_CDR_CFG_0 �Ĵ����ṹ���塣��ַƫ����:0x58����ֵ:0x16�����:8
 �Ĵ���˵��: PDģ��BMCʱ�ӻָ���·���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_cdr_cfg_0 : 8;  /* bit[0-7]: PDģ��BMCʱ�ӻָ���·���üĴ���
                                                       bit[0]: pd_bmc_cdr_edge_sel
                                                       bit[2:1]:pd_rx_cdr_edge_cfg
                                                       0X:˫�ؼ��
                                                       10:�����ؼ��
                                                       11:�½��ؼ��
                                                       Ĭ�ϣ�11
                                                       bit[4:3]:pd_rx_preamble_cfg
                                                       00������32���ط����ж�ΪPreamble
                                                       01������24���ط����ж�ΪPreamble
                                                       10: ����16���ط����ж�ΪPreamble
                                                       11: ����8���ط����ж�ΪPreamble
                                                       Ĭ�ϣ�10
                                                       bit[5]:pd_bmc_dds_init_sel
                                                       1��
                                                       0��
                                                       Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_PD_CDR_CFG_0_UNION;
#endif
#define SOC_SCHARGER_PD_CDR_CFG_0_pd_cdr_cfg_0_START  (0)
#define SOC_SCHARGER_PD_CDR_CFG_0_pd_cdr_cfg_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_CDR_CFG_1_UNION
 �ṹ˵��  : PD_CDR_CFG_1 �Ĵ����ṹ���塣��ַƫ����:0x59����ֵ:0x21�����:8
 �Ĵ���˵��: PDģ��BMCʱ�ӻָ���·���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_cdr_cfg_1 : 8;  /* bit[0-7]: PDģ��BMCʱ�ӻָ���·���üĴ���
                                                       bit[2:0]��CDR Kp��λ����
                                                       bit[7:4]��CDR Ki��λ���� */
    } reg;
} SOC_SCHARGER_PD_CDR_CFG_1_UNION;
#endif
#define SOC_SCHARGER_PD_CDR_CFG_1_pd_cdr_cfg_1_START  (0)
#define SOC_SCHARGER_PD_CDR_CFG_1_pd_cdr_cfg_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_CFG_0_UNION
 �ṹ˵��  : PD_DBG_CFG_0 �Ĵ����ṹ���塣��ַƫ����:0x5A����ֵ:0x9A�����:8
 �Ĵ���˵��: PDģ��Debug�����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_cfg_0 : 8;  /* bit[0-7]: PDģ��Debug�����üĴ���
                                                       CDR FCWԤ������ֵ��8���� */
    } reg;
} SOC_SCHARGER_PD_DBG_CFG_0_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_CFG_0_pd_dbg_cfg_0_START  (0)
#define SOC_SCHARGER_PD_DBG_CFG_0_pd_dbg_cfg_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_CFG_1_UNION
 �ṹ˵��  : PD_DBG_CFG_1 �Ĵ����ṹ���塣��ַƫ����:0x5B����ֵ:0x19�����:8
 �Ĵ���˵��: PDģ��Debug�����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_cfg_1 : 8;  /* bit[0-7]: PDģ��Debug�����üĴ���
                                                       bit[4:0]��CDR FCWԤ������ֵ��5����
                                                       bit[5]��pd_disch_thres_sel���Զ��ŵ���vSafe0V��ʱ����ֵtSave0V����λ
                                                       1b:250ms
                                                       0b:400ms
                                                       Ĭ��0
                                                       bit[6]��wd_chg_reset_disable
                                                       1b�����Ź����в���λTypeCЭ����CC1/CC2���������裻
                                                       0b�����Ź����лḴλTypeCЭ�飬���ҶϿ�CC1/CC2ΪOpen������260ms��������Ϊ˫Rd������
                                                       Ĭ��0
                                                       bit[7]:pd_rx_sop_window_disable
                                                       0b:ʹ��RX SOP��ⴰ��
                                                       1b:��ʹ��RX SOP��ⴰ��
                                                       Ĭ��0 */
    } reg;
} SOC_SCHARGER_PD_DBG_CFG_1_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_CFG_1_pd_dbg_cfg_1_START  (0)
#define SOC_SCHARGER_PD_DBG_CFG_1_pd_dbg_cfg_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RO_0_UNION
 �ṹ˵��  : PD_DBG_RO_0 �Ĵ����ṹ���塣��ַƫ����:0x5C����ֵ:0x00�����:8
 �Ĵ���˵��: PDģ��Debug�ûض��Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_status_0 : 8;  /* bit[0-7]: PDģ��Debug�ûض��Ĵ��� */
    } reg;
} SOC_SCHARGER_PD_DBG_RO_0_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RO_0_pd_dbg_status_0_START  (0)
#define SOC_SCHARGER_PD_DBG_RO_0_pd_dbg_status_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RO_1_UNION
 �ṹ˵��  : PD_DBG_RO_1 �Ĵ����ṹ���塣��ַƫ����:0x5D����ֵ:0x00�����:8
 �Ĵ���˵��: PDģ��Debug�ûض��Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_status_1 : 8;  /* bit[0-7]: PDģ��Debug�ûض��Ĵ��� */
    } reg;
} SOC_SCHARGER_PD_DBG_RO_1_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RO_1_pd_dbg_status_1_START  (0)
#define SOC_SCHARGER_PD_DBG_RO_1_pd_dbg_status_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RO_2_UNION
 �ṹ˵��  : PD_DBG_RO_2 �Ĵ����ṹ���塣��ַƫ����:0x5E����ֵ:0x00�����:8
 �Ĵ���˵��: PDģ��Debug�ûض��Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_status_2 : 8;  /* bit[0-7]: PDģ��Debug�ûض��Ĵ��� */
    } reg;
} SOC_SCHARGER_PD_DBG_RO_2_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RO_2_pd_dbg_status_2_START  (0)
#define SOC_SCHARGER_PD_DBG_RO_2_pd_dbg_status_2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PD_DBG_RO_3_UNION
 �ṹ˵��  : PD_DBG_RO_3 �Ĵ����ṹ���塣��ַƫ����:0x5F����ֵ:0xFF�����:8
 �Ĵ���˵��: PDģ��Debug�ûض��Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  pd_dbg_status_3 : 8;  /* bit[0-7]: PDģ��Debug�ûض��Ĵ��� */
    } reg;
} SOC_SCHARGER_PD_DBG_RO_3_UNION;
#endif
#define SOC_SCHARGER_PD_DBG_RO_3_pd_dbg_status_3_START  (0)
#define SOC_SCHARGER_PD_DBG_RO_3_pd_dbg_status_3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FAKE_SEL_UNION
 �ṹ˵��  : IRQ_FAKE_SEL �Ĵ����ṹ���塣��ַƫ����:0x60����ֵ:0xAC�����:8
 �Ĵ���˵��: ʵ���жϺ�α�ж�ѡ���ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_sel : 8;  /* bit[0-7]: ʵ���ж���α�ж�ѡ���źţ�
                                                     0xAC���ж�ԴΪʵ���жϣ�
                                                     0x5A���ж�ԴΪα�жϣ�
                                                     Ĭ��Ϊ0xAC */
    } reg;
} SOC_SCHARGER_IRQ_FAKE_SEL_UNION;
#endif
#define SOC_SCHARGER_IRQ_FAKE_SEL_sc_irq_sel_START  (0)
#define SOC_SCHARGER_IRQ_FAKE_SEL_sc_irq_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FAKE_UNION
 �ṹ˵��  : IRQ_FAKE �Ĵ����ṹ���塣��ַƫ����:0x61����ֵ:0xFF�����:8
 �Ĵ���˵��: α�ж�Դ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_fake_irq : 8;  /* bit[0-7]: α�ж�Դ��
                                                      8��d0 irq_vbus_ovp
                                                      8��d1 irq_vbus_uvp
                                                      8��d2 irq_vbat_ovp
                                                      8��d3 irq_otg_ovp
                                                      8��d4 irq_otg_uvp
                                                      8��d5 irq_regn_ocp
                                                      8��d6 irq_buck_scp
                                                      8��d7 irq_buck_ocp
                                                      8��d8 irq_otg_scp
                                                      8��d9 irq_otg_ocp
                                                      8��d10 irq_chg_batfet_ocp
                                                      8��d11 irq_trickle_chg_fault
                                                      8��d12 irq_pre_chg_fault
                                                      8��d13 irq_fast_chg_fault
                                                      8��d14 irq_otmp_jreg
                                                      8��d15 irq_otmp_140
                                                      8��d16 irq_chg_chgstate[1:0]
                                                      8��d17 irq_chg_rechg_state
                                                      8��d18 irq_sleep_mod
                                                      8��d19 irq_reversbst
                                                      8��d20 irq_vusb_buck_ovp
                                                      8��d21 irq_vusb_lvc_ovp
                                                      8��d22 irq_vusb_otg_ovp
                                                      8��d23 irq_vusb_scauto_ovp
                                                      8��d24 irq_vusb_ovp_alm
                                                      8��d25 irq_vusb_uv
                                                      8��d26 irq_bat_ov_clamp
                                                      8��d27 irq_vout_ov_clamp
                                                      8��d28 irq_ibus_clamp
                                                      8��d29 irq_ibat_clamp
                                                      8��d30 irq_vbus_sc_ov
                                                      8��d31 irq_vbus_lvc_ov
                                                      8��d32 irq_vbus_sc_uv
                                                      8��d33 irq_vbus_lvc_uv
                                                      8��d34 irq_ibus_dc_ocp
                                                      8��d35 irq_ibus_dc_ucp
                                                      8��d36 irq_ibus_dc_rcp
                                                      8��d37 irq_vin2vout_sc
                                                      8��d38 irq_vdrop_lvc_ov
                                                      8��d39 irq_vbat_dc_ovp
                                                      8��d40 irq_vbat_sc_uvp
                                                      8��d41 irq_ibat_dc_ocp
                                                      8��d42 irq_ibat_dcucp_alm
                                                      8��d43 irq_ilim_sc_ocp
                                                      8��d44 irq_ilim_bus_sc_ocp
                                                      8��d45 irq_tdie_ot_alm
                                                      8��d46 irq_vbat_lv
                                                      8��d47 irq_cc_ov
                                                      8��d48 irq_cc_ocp
                                                      8��d49 irq_cc_uv
                                                      8��d50 irq_fcp_set
                                                      8��d51 soh_ovh
                                                      8��d52 soh_ovl
                                                      8��d53 acr_flag
                                                      8��d54 irq_wl_otg_usbok
                                                      8��d55 irq_acr_scp
                                                      8'd56~8'd255�����α�ж�Ϊ0 */
    } reg;
} SOC_SCHARGER_IRQ_FAKE_UNION;
#endif
#define SOC_SCHARGER_IRQ_FAKE_sc_fake_irq_START  (0)
#define SOC_SCHARGER_IRQ_FAKE_sc_fake_irq_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_UNION
 �ṹ˵��  : IRQ_FLAG �Ĵ����ṹ���塣��ַƫ����:0x62����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                      1���жϷ���
                                                      0���жϲ�����
                                                      bit[7:5]��reserved
                                                      bit[4]��BUCK���ж�
                                                      bit[3]��LVC_SC���ж�
                                                      bit[2]��PD���ж�
                                                      bit[1]��Others���ж�
                                                      bit[0]��FCP���ж� */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_sc_irq_flag_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_sc_irq_flag_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_0_UNION
 �ṹ˵��  : IRQ_FLAG_0 �Ĵ����ṹ���塣��ַƫ����:0x63����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_0 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_chg_chgstate
                                                        bit[6]:irq_vbus_ovp
                                                        bit[5]:irq_vbus_uvp
                                                        bit[4]:irq_vbat_ovp
                                                        bit[3]:irq_otg_ovp
                                                        bit[2]:irq_otg_uvp
                                                        bit[1]:irq_buck_ocp
                                                        bit[0]:irq_otg_scp */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_0_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_0_sc_irq_flag_0_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_0_sc_irq_flag_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_1_UNION
 �ṹ˵��  : IRQ_FLAG_1 �Ĵ����ṹ���塣��ַƫ����:0x64����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_1 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_otg_ocp
                                                        bit[6]:irq_buck_scp
                                                        bit[5]:irq_trickle_chg_fault
                                                        bit[4]:irq_pre_chg_fault
                                                        bit[3]:irq_fast_chg_fault
                                                        bit[2]:irq_otmp_jreg
                                                        bit[1]:irq_chg_rechg_state
                                                        bit[0]:irq_sleep_mod */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_1_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_1_sc_irq_flag_1_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_1_sc_irq_flag_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_2_UNION
 �ṹ˵��  : IRQ_FLAG_2 �Ĵ����ṹ���塣��ַƫ����:0x65����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_2 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_reversbst
                                                        bit[6]:irq_vusb_buck_ovp
                                                        bit[5]:irq_vusb_otg_ovp

                                                        bit[4]:irq_vusb_lvc_ovp
                                                        bit[3]:irq_vbus_lvc_ov
                                                        bit[2]:irq_vbus_lvc_uv
                                                        bit[1]:irq_ibus_dc_ocp
                                                        bit[0]:irq_ibus_dc_ucp */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_2_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_2_sc_irq_flag_2_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_2_sc_irq_flag_2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_3_UNION
 �ṹ˵��  : IRQ_FLAG_3 �Ĵ����ṹ���塣��ַƫ����:0x66����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_3 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_ibus_dc_rcp
                                                        bit[6]:irq_vdrop_lvc_ov
                                                        bit[5]:irq_vbat_dc_ovp
                                                        bit[4]:irq_vusb_scauto_ovp
                                                        bit[3]:irq_bat_ov_clamp
                                                        bit[2]:irq_vout_ov_clamp
                                                        bit[1]:irq_ibus_clamp
                                                        bit[0]:irq_ibat_clamp */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_3_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_3_sc_irq_flag_3_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_3_sc_irq_flag_3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_4_UNION
 �ṹ˵��  : IRQ_FLAG_4 �Ĵ����ṹ���塣��ַƫ����:0x67����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_4 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_vbus_sc_ov
                                                        bit[6]:irq_vbus_sc_uv
                                                        bit[5]:irq_vin2vout_sc
                                                        bit[4]:irq_vbat_sc_uvp
                                                        bit[3]:irq_ibat_dc_ocp
                                                        bit[2]:irq_ibat_dcucp_alm
                                                        bit[1]:irq_ilim_sc_ocp
                                                        bit[0]:irq_ilim_bus_sc_ocp */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_4_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_4_sc_irq_flag_4_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_4_sc_irq_flag_4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_5_UNION
 �ṹ˵��  : IRQ_FLAG_5 �Ĵ����ṹ���塣��ַƫ����:0x68����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_5 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_cc_ov
                                                        bit[6]:irq_cc_ocp
                                                        bit[5]:irq_cc_uv

                                                        bit[4]:irq_acr_scp
                                                        bit[3]:soh_ovh
                                                        bit[2]:soh_ovl
                                                        bit[1]:acr_flag
                                                        bit[0]:irq_wdt */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_5_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_5_sc_irq_flag_5_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_5_sc_irq_flag_5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_6_UNION
 �ṹ˵��  : IRQ_FLAG_6 �Ĵ����ṹ���塣��ַƫ����:0x69����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_6 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_regn_ocp
                                                        bit[6]:irq_chg_batfet_ocp
                                                        bit[5]:irq_otmp_140
                                                        bit[4]:irq_wl_otg_usbok
                                                        bit[3]:irq_vusb_ovp_alm
                                                        bit[2]:irq_vusb_uv
                                                        bit[1]:irq_tdie_ot_alm
                                                        bit[0]:irq_vbat_lv */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_6_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_6_sc_irq_flag_6_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_6_sc_irq_flag_6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_FLAG_7_UNION
 �ṹ˵��  : IRQ_FLAG_7 �Ĵ����ṹ���塣��ַƫ����:0x6A����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_flag_7 : 8;  /* bit[0-7]: �жϱ�ǼĴ�����д1����
                                                        1���жϷ���
                                                        0���жϲ�����
                                                        bit[7]:irq_reserved[7]
                                                        bit[6]:irq_reserved[6]
                                                        bit[5]:irq_reserved[5]
                                                        bit[4]:irq_reserved[4]
                                                        bit[3]:irq_reserved[3]��irq_vout_dc_ovp vout��ѹ������dcģʽ��ʹ��
                                                        bit[2]:irq_reserved[2]��Ԥ���жϼĴ���
                                                        bit[1]:irq_reserved[1]��irq_vdrop_usb_ovp ovp�� drop�����жϣ�dcģʽ��ʹ�ܣ�
                                                        bit[0]:irq_reserved[0]��irq_cfly_scp,fly���ݶ�·�жϣ�dcģʽ��ʹ�� */
    } reg;
} SOC_SCHARGER_IRQ_FLAG_7_UNION;
#endif
#define SOC_SCHARGER_IRQ_FLAG_7_sc_irq_flag_7_START  (0)
#define SOC_SCHARGER_IRQ_FLAG_7_sc_irq_flag_7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_WDT_SOFT_RST_UNION
 �ṹ˵��  : WDT_SOFT_RST �Ĵ����ṹ���塣��ַƫ����:0x6B����ֵ:0x00�����:8
 �Ĵ���˵��: ���Ź���λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  wd_rst_n : 1;  /* bit[0-0]: д1�üĴ�����ϵͳwatchdog timer�������¼�ʱ(д1���Զ�����)
                                                   ���SoC������ʱ���ڲ��ԸüĴ���д��1����������λchg_en��watchdog_timer�Ĵ����� */
        unsigned char  reserved : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_WDT_SOFT_RST_UNION;
#endif
#define SOC_SCHARGER_WDT_SOFT_RST_wd_rst_n_START  (0)
#define SOC_SCHARGER_WDT_SOFT_RST_wd_rst_n_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_WDT_CTRL_UNION
 �ṹ˵��  : WDT_CTRL �Ĵ����ṹ���塣��ַƫ����:0x6C����ֵ:0x00�����:8
 �Ĵ���˵��: ι��ʱ����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_watchdog_timer : 3;  /* bit[0-2]: SOCÿ��һ��ʱ���wd_rst_n����һ�μĴ���д���������û��д�����¼������������Defaultģʽ����λsc_chg_en/sc_sc_en/sc_lvc_en/fcp_det_en/fcp_cmp_en��sc_watchdog_timer�Ĵ�����������������Hostģʽ��
                                                            ϵͳwatchdog_timerʱ�����ã�
                                                            3'b000��ϵͳwatchdog_timer�������Σ�
                                                            3'b001��0.5s��
                                                            3'b010��1s��
                                                            3'b011��2s��
                                                            3'b100��10s��
                                                            3'b101��20s��
                                                            3'b110��40s��
                                                            3'b111��80s�� */
        unsigned char  reserved_0        : 1;  /* bit[3-3]: ���� */
        unsigned char  sc_wdt_test_sel   : 1;  /* bit[4-4]: WDTʱ�䵵λ�ɸ��ݲ���ģʽ������ģʽѡ���źţ�
                                                            1��b0�����Ź�ʱ�䵵λΪ������λ��
                                                            1��b1�����Ź�ʱ�䵵λΪ���Ե�λ��sc_watchdog_timer=3'b000��ϵͳwatchdog_timer�������Σ�
                                                            sc_watchdog_timer=3'b001~3'b111��1ms�� */
        unsigned char  reserved_1        : 3;  /* bit[5-7]: ���� */
    } reg;
} SOC_SCHARGER_WDT_CTRL_UNION;
#endif
#define SOC_SCHARGER_WDT_CTRL_sc_watchdog_timer_START  (0)
#define SOC_SCHARGER_WDT_CTRL_sc_watchdog_timer_END    (2)
#define SOC_SCHARGER_WDT_CTRL_sc_wdt_test_sel_START    (4)
#define SOC_SCHARGER_WDT_CTRL_sc_wdt_test_sel_END      (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_IBAT_REGULATOR_UNION
 �ṹ˵��  : DC_IBAT_REGULATOR �Ĵ����ṹ���塣��ַƫ����:0x6D����ֵ:0x14�����:8
 �Ĵ���˵��: ֱ��IBATУ׼��λ���ڼĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_dc_ibat_regulator : 8;  /* bit[0-7]: ֱ��IBAT У׼��λ���ڼĴ�����
                                                               ������Χ3A~13A��step=50mA
                                                               8'h00:3A;
                                                               8'h01:3.05A;
                                                               ...
                                                               8'h14:4A;
                                                               ...
                                                               8'hC7:12.95A;
                                                               8'hC8:13A;
                                                               8'hC9~8'hFF:13A���Ĵ�������13A���������ֲ������⴦����EFUSE��ֵ���֮�������ʹ���)

                                                               Ĭ��0x14 */
    } reg;
} SOC_SCHARGER_DC_IBAT_REGULATOR_UNION;
#endif
#define SOC_SCHARGER_DC_IBAT_REGULATOR_sc_dc_ibat_regulator_START  (0)
#define SOC_SCHARGER_DC_IBAT_REGULATOR_sc_dc_ibat_regulator_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_VBAT_REGULATOR_UNION
 �ṹ˵��  : DC_VBAT_REGULATOR �Ĵ����ṹ���塣��ַƫ����:0x6E����ֵ:0x0A�����:8
 �Ĵ���˵��: ֱ��VBATУ׼��λ���ڼĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_dc_vbat_regulator : 7;  /* bit[0-6]: ֱ��VBATУ׼��λ���ڼĴ���:
                                                               ��ѹ��Χ4.2V~5V, step=10mV��
                                                               7'h00:4.2V;
                                                               7'h01:4.21V;
                                                               ...
                                                               7'h0A:4.3V;
                                                               ...
                                                               7'h50:5V;
                                                               7'h51~7'h7F:5V��
                                                               ���Ĵ�������5V���������ֲ������⴦����EFUSE��ֵ���֮�������ʹ���)step=10mV��Ĭ��4.3V */
        unsigned char  reserved             : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_VBAT_REGULATOR_UNION;
#endif
#define SOC_SCHARGER_DC_VBAT_REGULATOR_sc_dc_vbat_regulator_START  (0)
#define SOC_SCHARGER_DC_VBAT_REGULATOR_sc_dc_vbat_regulator_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_VOUT_REGULATOR_UNION
 �ṹ˵��  : DC_VOUT_REGULATOR �Ĵ����ṹ���塣��ַƫ����:0x6F����ֵ:0x0A�����:8
 �Ĵ���˵��: ֱ��VOUTУ׼��λ���ڼĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_dc_vout_regulator : 7;  /* bit[0-6]: ֱ��VOUTУ׼��λ���ڼĴ���:
                                                               ��ѹ��Χ4.2V~5V, step=10mV��
                                                               7'h00:4.2V;
                                                               7'h01:4.21V;
                                                               ...
                                                               7'h0A:4.3V;
                                                               ...
                                                               7'h50:5V;
                                                               7'h51~7'h7F:5V��
                                                               ���Ĵ�������5V���������ֲ������⴦����EFUSE��ֵ���֮�������ʹ���)step=10mV��Ĭ��4.3V */
        unsigned char  reserved             : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_VOUT_REGULATOR_UNION;
#endif
#define SOC_SCHARGER_DC_VOUT_REGULATOR_sc_dc_vout_regulator_START  (0)
#define SOC_SCHARGER_DC_VOUT_REGULATOR_sc_dc_vout_regulator_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_UNION
 �ṹ˵��  : OTG_CFG �Ĵ����ṹ���塣��ַƫ����:0x70����ֵ:0x09�����:8
 �Ĵ���˵��: OTGģ�����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_otg_dmd_ofs  : 4;  /* bit[0-3]: boost DMDʧ����ѹ����
                                                          0000: 20mV (300mA)
                                                          0001: 16mV (240mA)
                                                          0010: 14mV (210mA)
                                                          0011: 12mV (180mA)
                                                          0100: 10mV (150mA)
                                                          0101: 8mV (120mA)
                                                          0110: 6mV (90mA)
                                                          0111: 4mV (60mA)
                                                          1000: 2mV (30mA)
                                                          1001:0
                                                          1010: -16mV (-180mA).
                                                          1011: -32mV (-360mA).
                                                          1100: -48mV (-540mA).
                                                          1101: -64mV (-720mA)..
                                                          1110: -128mV (-900mA).
                                                          1111: -256mV (-1080mA) */
        unsigned char  sc_otg_dmd_ramp : 3;  /* bit[4-6]: ��������DMD�㷨���Ż��Ĵ����������Ժ�̶���ʵ�ʻ�Ӱ��DMD��λ�� */
        unsigned char  sc_otg_en       : 1;  /* bit[7-7]: OTGʹ���źţ�
                                                          1'b0����ʹ�ܣ�
                                                          1'b1��ʹ�� */
    } reg;
} SOC_SCHARGER_OTG_CFG_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_sc_otg_dmd_ofs_START   (0)
#define SOC_SCHARGER_OTG_CFG_sc_otg_dmd_ofs_END     (3)
#define SOC_SCHARGER_OTG_CFG_sc_otg_dmd_ramp_START  (4)
#define SOC_SCHARGER_OTG_CFG_sc_otg_dmd_ramp_END    (6)
#define SOC_SCHARGER_OTG_CFG_sc_otg_en_START        (7)
#define SOC_SCHARGER_OTG_CFG_sc_otg_en_END          (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PULSE_CHG_CFG0_UNION
 �ṹ˵��  : PULSE_CHG_CFG0 �Ĵ����ṹ���塣��ַƫ����:0x71����ֵ:0x00�����:8
 �Ĵ���˵��: ���������üĴ���0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_lvc_en        : 1;  /* bit[0-0]: ��ѹֱ��ȫ�ֳ��ʹ�ܣ�
                                                           1��b0: �رգ�Ĭ�ϣ���
                                                           1��b1: ʹ�ܡ�
                                                           ��ע�����Ź������쳣��üĴ����ᱻ��ΪĬ��ֵ�� */
        unsigned char  sc_pulse_mode_en : 1;  /* bit[1-1]: �������ֱ�����ģʽѡ��
                                                           1��b0����ʹ���������ģʽ��Ĭ�ϣ���
                                                           1��b1��ʹ��������ģʽ��硣 */
        unsigned char  reserved_0       : 2;  /* bit[2-3]: reserved */
        unsigned char  sc_chg_en        : 1;  /* bit[4-4]: ��ѹ���ȫ�ֳ��ʹ�ܣ�
                                                           1'b0����ʹ�ܳ�磨Ĭ�ϣ�
                                                           1'b1��ʹ�ܳ��
                                                           ��ע�����Ź������쳣��üĴ����ᱻ��ΪĬ��ֵ�� */
        unsigned char  reserved_1       : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_PULSE_CHG_CFG0_UNION;
#endif
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_lvc_en_START         (0)
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_lvc_en_END           (0)
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_pulse_mode_en_START  (1)
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_pulse_mode_en_END    (1)
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_chg_en_START         (4)
#define SOC_SCHARGER_PULSE_CHG_CFG0_sc_chg_en_END           (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PULSE_CHG_CFG1_UNION
 �ṹ˵��  : PULSE_CHG_CFG1 �Ĵ����ṹ���塣��ַƫ����:0x72����ֵ:0x13�����:8
 �Ĵ���˵��: ���������üĴ���1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_chg_time_set : 5;  /* bit[0-4]: ����ģʽ���ʱ�����ã�
                                                          5��b00000:100ms
                                                          5��b 00001:200ms
                                                          5��b 00010:300ms
                                                          5��b 11110:3100ms
                                                          5��b 11111:3200ms
                                                          Ĭ��2000ms */
        unsigned char  sc_sc_en        : 1;  /* bit[5-5]: ��ѹֱ��ʹ���źţ�
                                                          1��b0: �رգ�Ĭ�ϣ���
                                                          1��b1: ʹ�ܡ�
                                                          ��ע�����Ź������쳣��üĴ����ᱻ��ΪĬ��ֵ�� */
        unsigned char  reserved        : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_PULSE_CHG_CFG1_UNION;
#endif
#define SOC_SCHARGER_PULSE_CHG_CFG1_sc_chg_time_set_START  (0)
#define SOC_SCHARGER_PULSE_CHG_CFG1_sc_chg_time_set_END    (4)
#define SOC_SCHARGER_PULSE_CHG_CFG1_sc_sc_en_START         (5)
#define SOC_SCHARGER_PULSE_CHG_CFG1_sc_sc_en_END           (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DISCHG_TIME_UNION
 �ṹ˵��  : DISCHG_TIME �Ĵ����ṹ���塣��ַƫ����:0x73����ֵ:0x63�����:8
 �Ĵ���˵��: ����ģʽ�ŵ�ʱ�����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_dischg_time_set : 8;  
    } reg;
} SOC_SCHARGER_DISCHG_TIME_UNION;
#endif
#define SOC_SCHARGER_DISCHG_TIME_sc_dischg_time_set_START  (0)
#define SOC_SCHARGER_DISCHG_TIME_sc_dischg_time_set_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DIG_STATUS0_UNION
 �ṹ˵��  : DIG_STATUS0 �Ĵ����ṹ���塣��ַƫ����:0x74����ֵ:0x01�����:8
 �Ĵ���˵��: �����ڲ��ź�״̬�Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  wdt_time_out_n : 1;  /* bit[0-0]: ���Ź�״̬�źţ�
                                                         0: watchdog timer �����SOC�ڹ涨ʱ����û����watchdog timer�Ĵ��������ڹ涨ʱ����û�ж�wd_rst_n��0xE8���Ĵ���д1��
                                                         1: watchdog timer ����û�д��� ��watchdog timer������
                                                         ˵�������ڿ��Ź�����timeout֮��Ὣ���Ź�ʱ�䵵λsc_watchdog_timer�Ĵ�����λΪ0����disable���Ź����ܣ�����wdt_time_out_nΪ0��ʱ��ԼΪ24us�����¸üĴ����ض���״̬��0�ĸ��ʷǳ�С����ѯ���Ź������ʹ�ÿ��Ź�timeout�жϼĴ���sc_irq_flag_5[0] */
        unsigned char  reserved       : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_DIG_STATUS0_UNION;
#endif
#define SOC_SCHARGER_DIG_STATUS0_wdt_time_out_n_START  (0)
#define SOC_SCHARGER_DIG_STATUS0_wdt_time_out_n_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_TESTBUS_CFG_UNION
 �ṹ˵��  : SC_TESTBUS_CFG �Ĵ����ṹ���塣��ַƫ����:0x76����ֵ:0x00�����:8
 �Ĵ���˵��: �����������üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_cg_testbus_sel : 4;  /* bit[0-3]: ��������CRG�ڲ��۲��źŵ�ѡ�����
                                                            ��Programming Guide
                                                            Ĭ��4'b0000 */
        unsigned char  sc_cg_testsig_sel : 3;  /* bit[4-6]: �����͸�ģ�ⵥ�����ź�ѡ��
                                                            000:ѡ��clk_fcpʱ�������(125KHz)
                                                            001:ѡ��clk_2mʱ�������(2MHz)
                                                            010��ѡ��clk_pdʱ�������6MHz��
                                                            Others: reserved
                                                            Ĭ�ϣ�3'b000 */
        unsigned char  sc_cg_testbus_en  : 1;  /* bit[7-7]: �����͸�ģ������ź�ʹ��
                                                            0:��ʹ�ܲ����ź�
                                                            1��ʹ�ܲ����ź����
                                                            Ĭ��0 */
    } reg;
} SOC_SCHARGER_SC_TESTBUS_CFG_UNION;
#endif
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testbus_sel_START  (0)
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testbus_sel_END    (3)
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testsig_sel_START  (4)
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testsig_sel_END    (6)
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testbus_en_START   (7)
#define SOC_SCHARGER_SC_TESTBUS_CFG_sc_cg_testbus_en_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_TESTBUS_RDATA_UNION
 �ṹ˵��  : SC_TESTBUS_RDATA �Ĵ����ṹ���塣��ַƫ����:0x77����ֵ:0x00�����:8
 �Ĵ���˵��: �������߻ض��Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cg_testbus_rdata : 8;  /* bit[0-7]: CRG�ڲ��۲��źŵĻض����� */
    } reg;
} SOC_SCHARGER_SC_TESTBUS_RDATA_UNION;
#endif
#define SOC_SCHARGER_SC_TESTBUS_RDATA_cg_testbus_rdata_START  (0)
#define SOC_SCHARGER_SC_TESTBUS_RDATA_cg_testbus_rdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_GLB_SOFT_RST_CTRL_UNION
 �ṹ˵��  : GLB_SOFT_RST_CTRL �Ĵ����ṹ���塣��ַƫ����:0x78����ֵ:0x5A�����:8
 �Ĵ���˵��: ȫ����λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_cg_glb_soft_rst_n : 8;  /* bit[0-7]: оƬȫ����λ���üĴ�����д������
                                                               ��д��0x5Aʱ����λΪ�ߵ�ƽ������λ��
                                                               ��д��0xACʱ����λΪ�͵�ƽ����λ��batfet_ctrl��vbat_lv�Լ�EFUSE������������߼���
                                                               ��д������ֵ����λ�����䣬���֣� */
    } reg;
} SOC_SCHARGER_GLB_SOFT_RST_CTRL_UNION;
#endif
#define SOC_SCHARGER_GLB_SOFT_RST_CTRL_sc_cg_glb_soft_rst_n_START  (0)
#define SOC_SCHARGER_GLB_SOFT_RST_CTRL_sc_cg_glb_soft_rst_n_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_UNION
 �ṹ˵��  : EFUSE_SOFT_RST_CTRL �Ĵ����ṹ���塣��ַƫ����:0x79����ֵ:0x5A�����:8
 �Ĵ���˵��: EFUSE��λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_cg_efuse_soft_rst_n : 8;  /* bit[0-7]: EFUSE��λ���üĴ�����д����
                                                                 ��д��0x5Aʱ����λΪ�ߵ�ƽ������λEFUSE�����߼���
                                                                 ��д��0xACʱ����λΪ�͵�ƽ����λEFUSE�����߼���EFUSE�����߼����EFUSE���»ض�һ�����ݡ�
                                                                 ��д������ֵ����λ�����䣬���֣� */
    } reg;
} SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_UNION;
#endif
#define SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_sc_cg_efuse_soft_rst_n_START  (0)
#define SOC_SCHARGER_EFUSE_SOFT_RST_CTRL_sc_cg_efuse_soft_rst_n_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_UNION
 �ṹ˵��  : SC_CRG_CLK_EN_CTRL �Ĵ����ṹ���塣��ַƫ����:0x7A����ֵ:0x0F�����:8
 �Ĵ���˵��: CORE CRGʱ��ʹ�����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_cg_com_clk_en    : 1;  /* bit[0-0]: 1MHzʱ����ʱ��ʹ�����üĴ�����
                                                              0: ��ʹ��1MHzʱ����
                                                              1��ʹ��1MHzʱ����
                                                              Ĭ��1 */
        unsigned char  sc_cg_red_clk_en    : 1;  /* bit[1-1]: ���ֲ����á�Redundancyʱ����ʱ��ʹ�����üĴ�����
                                                              0: ��ʹ��
                                                              1��ʹ��
                                                              Ĭ��1 */
        unsigned char  sc_cg_pd_clk_en     : 1;  /* bit[2-2]: ���ֲ����á�clk_pdʱ����ʱ��ʹ�����üĴ�����
                                                              0: ��ʹ��
                                                              1��ʹ��
                                                              Ĭ��1 */
        unsigned char  sc_cg_2m_clk_en     : 1;  /* bit[3-3]: ���ֲ����á�clk_2mʱ����ʱ��ʹ�����üĴ�����
                                                              0: ��ʹ��
                                                              1��ʹ��
                                                              Ĭ��1 */
        unsigned char  sc_cg_pd_clk_en_sel : 1;  /* bit[4-4]: ���ֲ����á�clk_pdʱ����ʱ��ʹ��Դͷѡ��Ĵ�����
                                                              0: ʹ��ģ�����ad_clk2m_en��Ϊ�ſ��ź�
                                                              1��ʹ���������sc_cg_pd_clk_en��Ϊ�ſ��ź�
                                                              Ĭ��0 */
        unsigned char  sc_cg_2m_clk_en_sel : 1;  /* bit[5-5]: ���ֲ����á�clk_2mʱ����ʱ��ʹ��Դͷѡ��Ĵ�����
                                                              0: ʹ��ģ�����ad_clk2m_en��Ϊ�ſ��ź�
                                                              1��ʹ���������sc_cg_2m_clk_en��Ϊ�ſ��ź�
                                                              Ĭ��0 */
        unsigned char  reserved            : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_UNION;
#endif
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_com_clk_en_START     (0)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_com_clk_en_END       (0)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_red_clk_en_START     (1)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_red_clk_en_END       (1)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_pd_clk_en_START      (2)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_pd_clk_en_END        (2)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_2m_clk_en_START      (3)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_2m_clk_en_END        (3)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_pd_clk_en_sel_START  (4)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_pd_clk_en_sel_END    (4)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_2m_clk_en_sel_START  (5)
#define SOC_SCHARGER_SC_CRG_CLK_EN_CTRL_sc_cg_2m_clk_en_sel_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_MODE_CFG_UNION
 �ṹ˵��  : BUCK_MODE_CFG �Ĵ����ṹ���塣��ַƫ����:0x7B����ֵ:0x01�����:8
 �Ĵ���˵��: BUCKģʽ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  buck_mode_cfg : 1;  /* bit[0-0]: buckģʽָʾ
                                                        0:����buckģʽ��
                                                        1��buckģʽ��
                                                        ����irq_vusb_uvΪ�ߵ�ʱ��ḴλΪ��ʼֵ��
                                                        Ĭ��1 */
        unsigned char  reserved      : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_MODE_CFG_UNION;
#endif
#define SOC_SCHARGER_BUCK_MODE_CFG_buck_mode_cfg_START  (0)
#define SOC_SCHARGER_BUCK_MODE_CFG_buck_mode_cfg_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_MODE_CFG_UNION
 �ṹ˵��  : SC_MODE_CFG �Ĵ����ṹ���塣��ַƫ����:0x7C����ֵ:0x00�����:8
 �Ĵ���˵��: SCC���ģʽ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_mode_cfg : 1;  /* bit[0-0]: ���ģʽѡ��
                                                      0����SCCģʽ��
                                                      1��SCCģʽ��
                                                      ����irq_vusb_uvΪ�ߵ�ʱ��ḴλΪ��ʼֵ��
                                                      Ĭ��0 */
        unsigned char  reserved    : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_SC_MODE_CFG_UNION;
#endif
#define SOC_SCHARGER_SC_MODE_CFG_sc_mode_cfg_START  (0)
#define SOC_SCHARGER_SC_MODE_CFG_sc_mode_cfg_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_LVC_MODE_CFG_UNION
 �ṹ˵��  : LVC_MODE_CFG �Ĵ����ṹ���塣��ַƫ����:0x7D����ֵ:0x00�����:8
 �Ĵ���˵��: LVC���ģʽ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  lvc_mode_cfg : 1;  /* bit[0-0]: ���ģʽѡ��
                                                       0����LVCģʽ��
                                                       1��LVCģʽ��
                                                       ����irq_vusb_uvΪ�ߵ�ʱ��ḴλΪ��ʼֵ��
                                                       Ĭ��0 */
        unsigned char  reserved     : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_LVC_MODE_CFG_UNION;
#endif
#define SOC_SCHARGER_LVC_MODE_CFG_lvc_mode_cfg_START  (0)
#define SOC_SCHARGER_LVC_MODE_CFG_lvc_mode_cfg_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_BUCK_ENB_UNION
 �ṹ˵��  : SC_BUCK_ENB �Ĵ����ṹ���塣��ַƫ����:0x7E����ֵ:0x00�����:8
 �Ĵ���˵��: BUCKʹ�������ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_buck_enb : 1;  /* bit[0-0]: buckǿ�ƹرտ��ƼĴ���
                                                      0b��da_buck_enb�ź�Ϊ0��buckʹ����ģ���Զ����ƣ�
                                                      1b��da_buck_enb�ź�Ϊ1��buckǿ�ƹرգ�
                                                      Ĭ��0 */
        unsigned char  reserved    : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_SC_BUCK_ENB_UNION;
#endif
#define SOC_SCHARGER_SC_BUCK_ENB_sc_buck_enb_START  (0)
#define SOC_SCHARGER_SC_BUCK_ENB_sc_buck_enb_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SC_OVP_MOS_EN_UNION
 �ṹ˵��  : SC_OVP_MOS_EN �Ĵ����ṹ���塣��ַƫ����:0x7F����ֵ:0x01�����:8
 �Ĵ���˵��: OVPģ��ʹ�ܿ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_ovp_mos_en : 1;  /* bit[0-0]: OVPģ��ʹ�ܿ��ƼĴ���
                                                        0b��da_ovp_mos_en�ź�Ϊ0��OVPģ�鲻ʹ�ܣ�
                                                        1b��da_ovp_mos_en�ź�Ϊ1��OVPģ��ʹ��
                                                        Ĭ��1 */
        unsigned char  reserved      : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_SC_OVP_MOS_EN_UNION;
#endif
#define SOC_SCHARGER_SC_OVP_MOS_EN_sc_ovp_mos_en_START  (0)
#define SOC_SCHARGER_SC_OVP_MOS_EN_sc_ovp_mos_en_END    (0)




/****************************************************************************
                     (3/4) REG_PAGE1
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ3_UNION
 �ṹ˵��  : FCP_IRQ3 �Ĵ����ṹ���塣��ַƫ����:0x00����ֵ:0x00�����:8
 �Ĵ���˵��: FCP�ж�3�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  last_hand_fail_irq  : 1;  /* bit[0]: ���ݰ������������ʧ���жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  tail_hand_fail_irq  : 1;  /* bit[1]: Slaver �������ݺ��һ�η���ping����ʧ���жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  trans_hand_fail_irq : 1;  /* bit[2]: Master�������ݺ�����ʧ���жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  init_hand_fail_irq  : 1;  /* bit[3]: ��ʼ������ʧ���жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  rx_data_det_irq     : 1;  /* bit[4]: �Ȳ���slaver���ص������жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  rx_head_det_irq     : 1;  /* bit[5]: �Ȳ���slaver���ص����ݰ�ͷ�жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  cmd_err_irq         : 1;  /* bit[6]: SNDCMD��Ϊ������д�жϣ�
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
        unsigned char  length_err_irq      : 1;  /* bit[7]: LENGTH����
                                                            0���޴��жϣ�
                                                            1�����жϡ� */
    } reg;
} SOC_SCHARGER_FCP_IRQ3_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ3_last_hand_fail_irq_START   (0)
#define SOC_SCHARGER_FCP_IRQ3_last_hand_fail_irq_END     (0)
#define SOC_SCHARGER_FCP_IRQ3_tail_hand_fail_irq_START   (1)
#define SOC_SCHARGER_FCP_IRQ3_tail_hand_fail_irq_END     (1)
#define SOC_SCHARGER_FCP_IRQ3_trans_hand_fail_irq_START  (2)
#define SOC_SCHARGER_FCP_IRQ3_trans_hand_fail_irq_END    (2)
#define SOC_SCHARGER_FCP_IRQ3_init_hand_fail_irq_START   (3)
#define SOC_SCHARGER_FCP_IRQ3_init_hand_fail_irq_END     (3)
#define SOC_SCHARGER_FCP_IRQ3_rx_data_det_irq_START      (4)
#define SOC_SCHARGER_FCP_IRQ3_rx_data_det_irq_END        (4)
#define SOC_SCHARGER_FCP_IRQ3_rx_head_det_irq_START      (5)
#define SOC_SCHARGER_FCP_IRQ3_rx_head_det_irq_END        (5)
#define SOC_SCHARGER_FCP_IRQ3_cmd_err_irq_START          (6)
#define SOC_SCHARGER_FCP_IRQ3_cmd_err_irq_END            (6)
#define SOC_SCHARGER_FCP_IRQ3_length_err_irq_START       (7)
#define SOC_SCHARGER_FCP_IRQ3_length_err_irq_END         (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ4_UNION
 �ṹ˵��  : FCP_IRQ4 �Ĵ����ṹ���塣��ַƫ����:0x01����ֵ:0x00�����:8
 �Ĵ���˵��: FCP�ж�4�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  last_hand_no_respond_irq   : 1;  /* bit[0]  : ���ݰ��������Ȳ���slaver ping�жϣ�
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  tail_hand_no_respond_irq   : 1;  /* bit[1]  : Slaver �������ݺ��һ�η���ping���ֵȲ���slaver ping�жϣ�
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  trans_hand_no_respond_irq  : 1;  /* bit[2]  : Master�������ݺ����ֵȲ���slaver ping�жϣ�
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  init_hand_no_respond_irq   : 1;  /* bit[3]  : ��ʼ��slaverû��ping�����жϣ�
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  enable_hand_fail_irq       : 1;  /* bit[4]  : ͨ��enable����master ping������ʧ�ܣ�����ʧ�ܻ��Զ��ظ�����15�Σ���
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  enable_hand_no_respond_irq : 1;  /* bit[5]  : ͨ��enable����master ping��slaverû����Ӧ�жϣ�����ʧ�ܻ��Զ��ظ�����15�Σ���
                                                                     0���޴��жϣ�
                                                                     1�����жϡ� */
        unsigned char  reserved                   : 2;  /* bit[6-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_IRQ4_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ4_last_hand_no_respond_irq_START    (0)
#define SOC_SCHARGER_FCP_IRQ4_last_hand_no_respond_irq_END      (0)
#define SOC_SCHARGER_FCP_IRQ4_tail_hand_no_respond_irq_START    (1)
#define SOC_SCHARGER_FCP_IRQ4_tail_hand_no_respond_irq_END      (1)
#define SOC_SCHARGER_FCP_IRQ4_trans_hand_no_respond_irq_START   (2)
#define SOC_SCHARGER_FCP_IRQ4_trans_hand_no_respond_irq_END     (2)
#define SOC_SCHARGER_FCP_IRQ4_init_hand_no_respond_irq_START    (3)
#define SOC_SCHARGER_FCP_IRQ4_init_hand_no_respond_irq_END      (3)
#define SOC_SCHARGER_FCP_IRQ4_enable_hand_fail_irq_START        (4)
#define SOC_SCHARGER_FCP_IRQ4_enable_hand_fail_irq_END          (4)
#define SOC_SCHARGER_FCP_IRQ4_enable_hand_no_respond_irq_START  (5)
#define SOC_SCHARGER_FCP_IRQ4_enable_hand_no_respond_irq_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ3_MASK_UNION
 �ṹ˵��  : FCP_IRQ3_MASK �Ĵ����ṹ���塣��ַƫ����:0x02����ֵ:0xFF�����:8
 �Ĵ���˵��: FCP�ж�����3�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  last_hand_fail_irq_mk  : 1;  /* bit[0]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  tail_hand_fail_irq_mk  : 1;  /* bit[1]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  trans_hand_fail_irq_mk : 1;  /* bit[2]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  init_hand_fail_irq_mk  : 1;  /* bit[3]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  rx_data_det_irq_mk     : 1;  /* bit[4]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  rx_head_det_irq_mk     : 1;  /* bit[5]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  cmd_err_irq_mk         : 1;  /* bit[6]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
        unsigned char  length_err_irq_mk      : 1;  /* bit[7]: 0��ʹ�ܸ��жϣ�
                                                               1�����θ��жϡ� */
    } reg;
} SOC_SCHARGER_FCP_IRQ3_MASK_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ3_MASK_last_hand_fail_irq_mk_START   (0)
#define SOC_SCHARGER_FCP_IRQ3_MASK_last_hand_fail_irq_mk_END     (0)
#define SOC_SCHARGER_FCP_IRQ3_MASK_tail_hand_fail_irq_mk_START   (1)
#define SOC_SCHARGER_FCP_IRQ3_MASK_tail_hand_fail_irq_mk_END     (1)
#define SOC_SCHARGER_FCP_IRQ3_MASK_trans_hand_fail_irq_mk_START  (2)
#define SOC_SCHARGER_FCP_IRQ3_MASK_trans_hand_fail_irq_mk_END    (2)
#define SOC_SCHARGER_FCP_IRQ3_MASK_init_hand_fail_irq_mk_START   (3)
#define SOC_SCHARGER_FCP_IRQ3_MASK_init_hand_fail_irq_mk_END     (3)
#define SOC_SCHARGER_FCP_IRQ3_MASK_rx_data_det_irq_mk_START      (4)
#define SOC_SCHARGER_FCP_IRQ3_MASK_rx_data_det_irq_mk_END        (4)
#define SOC_SCHARGER_FCP_IRQ3_MASK_rx_head_det_irq_mk_START      (5)
#define SOC_SCHARGER_FCP_IRQ3_MASK_rx_head_det_irq_mk_END        (5)
#define SOC_SCHARGER_FCP_IRQ3_MASK_cmd_err_irq_mk_START          (6)
#define SOC_SCHARGER_FCP_IRQ3_MASK_cmd_err_irq_mk_END            (6)
#define SOC_SCHARGER_FCP_IRQ3_MASK_length_err_irq_mk_START       (7)
#define SOC_SCHARGER_FCP_IRQ3_MASK_length_err_irq_mk_END         (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_IRQ4_MASK_UNION
 �ṹ˵��  : FCP_IRQ4_MASK �Ĵ����ṹ���塣��ַƫ����:0x03����ֵ:0x3F�����:8
 �Ĵ���˵��: FCP�ж�����4�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  last_hand_no_respond_irq_mk   : 1;  /* bit[0]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  tail_hand_no_respond_irq_mk   : 1;  /* bit[1]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  trans_hand_no_respond_irq_mk  : 1;  /* bit[2]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  init_hand_no_respond_irq_mk   : 1;  /* bit[3]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  enable_hand_fail_irq_mk       : 1;  /* bit[4]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  enable_hand_no_respond_irq_mk : 1;  /* bit[5]  : 0��ʹ�ܸ��жϣ�
                                                                        1�����θ��жϡ� */
        unsigned char  reserved                      : 2;  /* bit[6-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_IRQ4_MASK_UNION;
#endif
#define SOC_SCHARGER_FCP_IRQ4_MASK_last_hand_no_respond_irq_mk_START    (0)
#define SOC_SCHARGER_FCP_IRQ4_MASK_last_hand_no_respond_irq_mk_END      (0)
#define SOC_SCHARGER_FCP_IRQ4_MASK_tail_hand_no_respond_irq_mk_START    (1)
#define SOC_SCHARGER_FCP_IRQ4_MASK_tail_hand_no_respond_irq_mk_END      (1)
#define SOC_SCHARGER_FCP_IRQ4_MASK_trans_hand_no_respond_irq_mk_START   (2)
#define SOC_SCHARGER_FCP_IRQ4_MASK_trans_hand_no_respond_irq_mk_END     (2)
#define SOC_SCHARGER_FCP_IRQ4_MASK_init_hand_no_respond_irq_mk_START    (3)
#define SOC_SCHARGER_FCP_IRQ4_MASK_init_hand_no_respond_irq_mk_END      (3)
#define SOC_SCHARGER_FCP_IRQ4_MASK_enable_hand_fail_irq_mk_START        (4)
#define SOC_SCHARGER_FCP_IRQ4_MASK_enable_hand_fail_irq_mk_END          (4)
#define SOC_SCHARGER_FCP_IRQ4_MASK_enable_hand_no_respond_irq_mk_START  (5)
#define SOC_SCHARGER_FCP_IRQ4_MASK_enable_hand_no_respond_irq_mk_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_MSTATE_UNION
 �ṹ˵��  : MSTATE �Ĵ����ṹ���塣��ַƫ����:0x04����ֵ:0x00�����:8
 �Ĵ���˵��: ״̬��״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  mstate   : 4;  /* bit[0-3]: FCP״̬��״̬�Ĵ����� */
        unsigned char  reserved : 4;  /* bit[4-7]: ������ */
    } reg;
} SOC_SCHARGER_MSTATE_UNION;
#endif
#define SOC_SCHARGER_MSTATE_mstate_START    (0)
#define SOC_SCHARGER_MSTATE_mstate_END      (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CRC_ENABLE_UNION
 �ṹ˵��  : CRC_ENABLE �Ĵ����ṹ���塣��ַƫ����:0x05����ֵ:0x01�����:8
 �Ĵ���˵��: crc ʹ�ܿ��ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  crc_en   : 1;  /* bit[0]  : crc ʹ�ܿ��ƼĴ�����
                                                   0����ʹ�ܣ�
                                                   1��ʹ�ܡ� */
        unsigned char  reserved : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_CRC_ENABLE_UNION;
#endif
#define SOC_SCHARGER_CRC_ENABLE_crc_en_START    (0)
#define SOC_SCHARGER_CRC_ENABLE_crc_en_END      (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CRC_START_VALUE_UNION
 �ṹ˵��  : CRC_START_VALUE �Ĵ����ṹ���塣��ַƫ����:0x06����ֵ:0x00�����:8
 �Ĵ���˵��: crc ��ʼֵ��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  crc_start_val : 8;  /* bit[0-7]: crc ��ʼֵ�� */
    } reg;
} SOC_SCHARGER_CRC_START_VALUE_UNION;
#endif
#define SOC_SCHARGER_CRC_START_VALUE_crc_start_val_START  (0)
#define SOC_SCHARGER_CRC_START_VALUE_crc_start_val_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SAMPLE_CNT_ADJ_UNION
 �ṹ˵��  : SAMPLE_CNT_ADJ �Ĵ����ṹ���塣��ַƫ����:0x07����ֵ:0x00�����:8
 �Ĵ���˵��: ����������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sample_cnt_adjust : 5;  /* bit[0-4]: ����slaver���������ֶ����ڼĴ�����Ϊ0ʱ��������Ϊ����slaver ping���ȵó�������������ֵʱ���������ھ�Ϊ����ֵ��
                                                            ע�������õ����ֵΪ28 */
        unsigned char  reserved          : 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_SAMPLE_CNT_ADJ_UNION;
#endif
#define SOC_SCHARGER_SAMPLE_CNT_ADJ_sample_cnt_adjust_START  (0)
#define SOC_SCHARGER_SAMPLE_CNT_ADJ_sample_cnt_adjust_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PING_WIDTH_MIN_H_UNION
 �ṹ˵��  : RX_PING_WIDTH_MIN_H �Ĵ����ṹ���塣��ַƫ����:0x08����ֵ:0x01�����:8
 �Ĵ���˵��: RX ping ��С���ȸ�λ��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_width_min_h : 1;  /* bit[0]  : Slaver ping�����Ч���ȸ�λ�� */
        unsigned char  reserved            : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_RX_PING_WIDTH_MIN_H_UNION;
#endif
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_H_rx_ping_width_min_h_START  (0)
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_H_rx_ping_width_min_h_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PING_WIDTH_MIN_L_UNION
 �ṹ˵��  : RX_PING_WIDTH_MIN_L �Ĵ����ṹ���塣��ַƫ����:0x09����ֵ:0x00�����:8
 �Ĵ���˵��: RX ping ��С���ȵ�8λ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_width_min_l : 8;  /* bit[0-7]: Slaver ping�����Ч���ȵ�8λ�� */
    } reg;
} SOC_SCHARGER_RX_PING_WIDTH_MIN_L_UNION;
#endif
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_L_rx_ping_width_min_l_START  (0)
#define SOC_SCHARGER_RX_PING_WIDTH_MIN_L_rx_ping_width_min_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PING_WIDTH_MAX_H_UNION
 �ṹ˵��  : RX_PING_WIDTH_MAX_H �Ĵ����ṹ���塣��ַƫ����:0x0A����ֵ:0x01�����:8
 �Ĵ���˵��: RX ping ��󳤶ȸ�λ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_width_max_h : 1;  /* bit[0]  : Slaver ping���Ч���ȸ�λ�� */
        unsigned char  reserved            : 7;  /* bit[1-7]: ������ */
    } reg;
} SOC_SCHARGER_RX_PING_WIDTH_MAX_H_UNION;
#endif
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_H_rx_ping_width_max_h_START  (0)
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_H_rx_ping_width_max_h_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PING_WIDTH_MAX_L_UNION
 �ṹ˵��  : RX_PING_WIDTH_MAX_L �Ĵ����ṹ���塣��ַƫ����:0x0B����ֵ:0x8B�����:8
 �Ĵ���˵��: RX ping ��󳤶ȵ�8λ��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_width_max_l : 8;  /* bit[0-7]: Slaver ping���Ч���ȵ�8λ�� */
    } reg;
} SOC_SCHARGER_RX_PING_WIDTH_MAX_L_UNION;
#endif
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_L_rx_ping_width_max_l_START  (0)
#define SOC_SCHARGER_RX_PING_WIDTH_MAX_L_rx_ping_width_max_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DATA_WAIT_TIME_UNION
 �ṹ˵��  : DATA_WAIT_TIME �Ĵ����ṹ���塣��ַƫ����:0x0C����ֵ:0x64�����:8
 �Ĵ���˵��: ���ݵȴ�ʱ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  data_wait_time : 7;  /* bit[0-6]: ���ݰ��ȴ�����ʱ��Ĵ�����ʵ������Ϊ����ֵ*4 cycle�� */
        unsigned char  reserved       : 1;  /* bit[7]  : ������ */
    } reg;
} SOC_SCHARGER_DATA_WAIT_TIME_UNION;
#endif
#define SOC_SCHARGER_DATA_WAIT_TIME_data_wait_time_START  (0)
#define SOC_SCHARGER_DATA_WAIT_TIME_data_wait_time_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RETRY_CNT_UNION
 �ṹ˵��  : RETRY_CNT �Ĵ����ṹ���塣��ַƫ����:0x0D����ֵ:0x03�����:8
 �Ĵ���˵��: �������·��ʹ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  cmd_retry_config : 4;  /* bit[0-3]: ���ݰ�����master retry������ */
        unsigned char  reserved         : 4;  /* bit[4-7]: ������ */
    } reg;
} SOC_SCHARGER_RETRY_CNT_UNION;
#endif
#define SOC_SCHARGER_RETRY_CNT_cmd_retry_config_START  (0)
#define SOC_SCHARGER_RETRY_CNT_cmd_retry_config_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_RO_RESERVE_UNION
 �ṹ˵��  : FCP_RO_RESERVE �Ĵ����ṹ���塣��ַƫ����:0x0E����ֵ:0x00�����:8
 �Ĵ���˵��: fcpֻ��Ԥ���Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  fcp_ro_reserve : 8;  /* bit[0-7]: fcpֻ��Ԥ���Ĵ����� */
    } reg;
} SOC_SCHARGER_FCP_RO_RESERVE_UNION;
#endif
#define SOC_SCHARGER_FCP_RO_RESERVE_fcp_ro_reserve_START  (0)
#define SOC_SCHARGER_FCP_RO_RESERVE_fcp_ro_reserve_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_DEBUG_REG0_UNION
 �ṹ˵��  : FCP_DEBUG_REG0 �Ĵ����ṹ���塣��ַƫ����:0x0F����ֵ:0x0a�����:8
 �Ĵ���˵��: FCP debug�Ĵ���0��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  slv_crc_err         : 1;  /* bit[0]  : Slaver����crcУ��״̬�� */
        unsigned char  resp_ack_parity_err : 1;  /* bit[1]  : Slaver���ص�ACK/NACK���ݵ�partityУ�顣 */
        unsigned char  fcp_head_early_err  : 1;  /* bit[2]  : Slaver���ص�ͬ��ͷ�����ϱ����ж� */
        unsigned char  slv_crc_parity_err  : 1;  /* bit[3]  : Slaver���ص�CRC���ݵ�parityУ�顣 */
        unsigned char  rdata_range_err     : 1;  /* bit[4]  : Slave��������Ƶƫ����master�ɽ�����Χ���� */
        unsigned char  reserved            : 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_FCP_DEBUG_REG0_UNION;
#endif
#define SOC_SCHARGER_FCP_DEBUG_REG0_slv_crc_err_START          (0)
#define SOC_SCHARGER_FCP_DEBUG_REG0_slv_crc_err_END            (0)
#define SOC_SCHARGER_FCP_DEBUG_REG0_resp_ack_parity_err_START  (1)
#define SOC_SCHARGER_FCP_DEBUG_REG0_resp_ack_parity_err_END    (1)
#define SOC_SCHARGER_FCP_DEBUG_REG0_fcp_head_early_err_START   (2)
#define SOC_SCHARGER_FCP_DEBUG_REG0_fcp_head_early_err_END     (2)
#define SOC_SCHARGER_FCP_DEBUG_REG0_slv_crc_parity_err_START   (3)
#define SOC_SCHARGER_FCP_DEBUG_REG0_slv_crc_parity_err_END     (3)
#define SOC_SCHARGER_FCP_DEBUG_REG0_rdata_range_err_START      (4)
#define SOC_SCHARGER_FCP_DEBUG_REG0_rdata_range_err_END        (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_DEBUG_REG1_UNION
 �ṹ˵��  : FCP_DEBUG_REG1 �Ĵ����ṹ���塣��ַƫ����:0x10����ֵ:0x00�����:8
 �Ĵ���˵��: FCP debug�Ĵ���1��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  resp_ack : 8;  /* bit[0-7]: FCP debug�Ĵ���1��Slaver���ص�ACK/NAC���ݡ� */
    } reg;
} SOC_SCHARGER_FCP_DEBUG_REG1_UNION;
#endif
#define SOC_SCHARGER_FCP_DEBUG_REG1_resp_ack_START  (0)
#define SOC_SCHARGER_FCP_DEBUG_REG1_resp_ack_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_DEBUG_REG2_UNION
 �ṹ˵��  : FCP_DEBUG_REG2 �Ĵ����ṹ���塣��ַƫ����:0x11����ֵ:0x00�����:8
 �Ĵ���˵��: FCP debug�Ĵ���2��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  slv_crc : 8;  /* bit[0-7]: FCP debug�Ĵ���2��Slaver���ص�CRC���ݡ� */
    } reg;
} SOC_SCHARGER_FCP_DEBUG_REG2_UNION;
#endif
#define SOC_SCHARGER_FCP_DEBUG_REG2_slv_crc_START  (0)
#define SOC_SCHARGER_FCP_DEBUG_REG2_slv_crc_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_DEBUG_REG3_UNION
 �ṹ˵��  : FCP_DEBUG_REG3 �Ĵ����ṹ���塣��ַƫ����:0x12����ֵ:0x07�����:8
 �Ĵ���˵��: FCP debug�Ĵ���3��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_cnt_l : 8;  /* bit[0-7]: FCP debug�Ĵ���3����⵽slave ping���ȣ���8bit�� */
    } reg;
} SOC_SCHARGER_FCP_DEBUG_REG3_UNION;
#endif
#define SOC_SCHARGER_FCP_DEBUG_REG3_rx_ping_cnt_l_START  (0)
#define SOC_SCHARGER_FCP_DEBUG_REG3_rx_ping_cnt_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_FCP_DEBUG_REG4_UNION
 �ṹ˵��  : FCP_DEBUG_REG4 �Ĵ����ṹ���塣��ַƫ����:0x13����ֵ:0x00�����:8
 �Ĵ���˵��: FCP debug�Ĵ���4��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved        : 2;  /* bit[0-1]: ������ */
        unsigned char  rx_ping_low_cnt : 5;  /* bit[2-6]: FCP debug�Ĵ���5����⵽slave ping��UI���ȡ� */
        unsigned char  rx_ping_cnt_h   : 1;  /* bit[7]  : FCP debug�Ĵ���4����⵽slave ping���ȣ����bit�� */
    } reg;
} SOC_SCHARGER_FCP_DEBUG_REG4_UNION;
#endif
#define SOC_SCHARGER_FCP_DEBUG_REG4_rx_ping_low_cnt_START  (2)
#define SOC_SCHARGER_FCP_DEBUG_REG4_rx_ping_low_cnt_END    (6)
#define SOC_SCHARGER_FCP_DEBUG_REG4_rx_ping_cnt_h_START    (7)
#define SOC_SCHARGER_FCP_DEBUG_REG4_rx_ping_cnt_h_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_UNION
 �ṹ˵��  : RX_PACKET_WAIT_ADJUST �Ĵ����ṹ���塣��ַƫ����:0x14����ֵ:0x14�����:8
 �Ĵ���˵��: ACKǰͬ��ͷ�ȴ�΢���ڼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_packet_wait_adjust : 7;  /* bit[0-6]: SLAVE PING��ACK֮��Ĭ�ϵȴ�5UI���üĴ���Ϊ��ԭʱ������ϵ�΢���Ĵ������ȴ�ʱ��Ϊ5UI+����ֵ�� */
        unsigned char  reserved              : 1;  /* bit[7]  : ������ */
    } reg;
} SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_UNION;
#endif
#define SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_rx_packet_wait_adjust_START  (0)
#define SOC_SCHARGER_RX_PACKET_WAIT_ADJUST_rx_packet_wait_adjust_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SAMPLE_CNT_TINYJUST_UNION
 �ṹ˵��  : SAMPLE_CNT_TINYJUST �Ĵ����ṹ���塣��ַƫ����:0x15����ֵ:0x00�����:8
 �Ĵ���˵��: FCP����΢���Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sample_cnt_tinyjust : 5;  /* bit[0-4]: Ĭ�ϲ�����Ϊslave ping/32�����üĴ�����΢�������㡣 sample_cnt_tinyjust[4]Ϊ1�� ԭ������ + sample_cnt_tinyjust[3:0]. ��ע��sample_cnt_tinyjust[3:0]��������0��1�� sample_cnt_tinyjust[4]Ϊ0�� ԭ������ - sample_cnt_tinyjust[3:0]�� ��ע��sample_cnt_tinyjust[3:0]��������С��8��  */
        unsigned char  reserved            : 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_SAMPLE_CNT_TINYJUST_UNION;
#endif
#define SOC_SCHARGER_SAMPLE_CNT_TINYJUST_sample_cnt_tinyjust_START  (0)
#define SOC_SCHARGER_SAMPLE_CNT_TINYJUST_sample_cnt_tinyjust_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_RX_PING_CNT_TINYJUST_UNION
 �ṹ˵��  : RX_PING_CNT_TINYJUST �Ĵ����ṹ���塣��ַƫ����:0x16����ֵ:0x00�����:8
 �Ĵ���˵��: FCP���RX PING CNT΢���Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  rx_ping_cnt_tinyjust : 5;  /* bit[0-4]: ͨ��slave ping���ȵõ�rx_ping_cnt��ͬʱ��cnt��Ϊ�ڲ��ɼ����ݻ�׼�� rx_ping_cnt_tinyjust[4]Ϊ1�� rx_ping_cnt + rx_ping_cnt_tinyjust[3:0]. rx_ping_cnt_tinyjust[4]Ϊ0�� rx_ping_cnt - rx_ping_cnt_tinyjust[3:0]�� ��ע��rx_ping_cnt_tinyjust��������ΪС��7��  */
        unsigned char  reserved             : 3;  /* bit[5-7]: ������ */
    } reg;
} SOC_SCHARGER_RX_PING_CNT_TINYJUST_UNION;
#endif
#define SOC_SCHARGER_RX_PING_CNT_TINYJUST_rx_ping_cnt_tinyjust_START  (0)
#define SOC_SCHARGER_RX_PING_CNT_TINYJUST_rx_ping_cnt_tinyjust_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SHIFT_CNT_CFG_MAX_UNION
 �ṹ˵��  : SHIFT_CNT_CFG_MAX �Ĵ����ṹ���塣��ַƫ����:0x17����ֵ:0x0C�����:8
 �Ĵ���˵��: SHIFT_CNT���ֵ���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  shift_cnt_cfg_max : 4;  /* bit[0-3]: �������������ֵ���üĴ�����������ֵmaster״̬����ת��һ״̬�� */
        unsigned char  reserved          : 4;  /* bit[4-7]: ������ */
    } reg;
} SOC_SCHARGER_SHIFT_CNT_CFG_MAX_UNION;
#endif
#define SOC_SCHARGER_SHIFT_CNT_CFG_MAX_shift_cnt_cfg_max_START  (0)
#define SOC_SCHARGER_SHIFT_CNT_CFG_MAX_shift_cnt_cfg_max_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CFG_REG_0_UNION
 �ṹ˵��  : HKADC_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x18����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_hkadc_fdiv : 1;  /* bit[0-0]: HKADCƵ��ѡ��
                                                        0��Ĭ��Ƶ�ʣ�
                                                        1��Ƶ�ʼ��룻 */
        unsigned char  reserved      : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_HKADC_CFG_REG_0_da_hkadc_fdiv_START  (0)
#define SOC_SCHARGER_HKADC_CFG_REG_0_da_hkadc_fdiv_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CFG_REG_1_UNION
 �ṹ˵��  : HKADC_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x19����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_hkadc_ibias_sel : 8;  /* bit[0-7]: HKADC�����������ã�Ĭ��0 */
    } reg;
} SOC_SCHARGER_HKADC_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_HKADC_CFG_REG_1_da_hkadc_ibias_sel_START  (0)
#define SOC_SCHARGER_HKADC_CFG_REG_1_da_hkadc_ibias_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_CFG_REG_2_UNION
 �ṹ˵��  : HKADC_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x1A����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_hkadc_reset    : 1;  /* bit[0-0]: HKADC�ڲ��߼���λ���ƣ�0����λ��1ǿ�Ƹ�λ */
        unsigned char  da_hkadc_reserved : 2;  /* bit[1-2]: HKADC���üĴ��� */
        unsigned char  da_hkadc_ibsel    : 3;  /* bit[3-5]: HKADC�����������ã�Ĭ��0 */
        unsigned char  reserved          : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_reset_START     (0)
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_reset_END       (0)
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_reserved_START  (1)
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_reserved_END    (2)
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_ibsel_START     (3)
#define SOC_SCHARGER_HKADC_CFG_REG_2_da_hkadc_ibsel_END       (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_OFFSET_0P1_UNION
 �ṹ˵��  : HKADC_OFFSET_0P1 �Ĵ����ṹ���塣��ַƫ����:0x1B����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC ����0p1У��ֵ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_offset_0p1_r : 7;  /* bit[0-6]: HKADC�������У��adc_code_0p1�Ĵ�������ֵ */
        unsigned char  reserved        : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_OFFSET_0P1_UNION;
#endif
#define SOC_SCHARGER_HKADC_OFFSET_0P1_sc_offset_0p1_r_START  (0)
#define SOC_SCHARGER_HKADC_OFFSET_0P1_sc_offset_0p1_r_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_OFFSET_2P45_UNION
 �ṹ˵��  : HKADC_OFFSET_2P45 �Ĵ����ṹ���塣��ַƫ����:0x1C����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC ����2p45У��ֵ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_offset_2p45_r : 7;  /* bit[0-6]: HKADC�������У��adc_code_2p45�Ĵ�������ֵ */
        unsigned char  reserved         : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_HKADC_OFFSET_2P45_UNION;
#endif
#define SOC_SCHARGER_HKADC_OFFSET_2P45_sc_offset_2p45_r_START  (0)
#define SOC_SCHARGER_HKADC_OFFSET_2P45_sc_offset_2p45_r_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH0_L_UNION
 �ṹ˵��  : SOH_OVH_TH0_L �Ĵ����ṹ���塣��ַƫ����:0x1D����ֵ:0xFF�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ0�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th0_l : 8;  /* bit[0-7]: ��ع�ѹ������ѹ������ֵ0�ĵ�8bit */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH0_L_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH0_L_sc_soh_ovh_th0_l_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH0_L_sc_soh_ovh_th0_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH0_H_UNION
 �ṹ˵��  : SOH_OVH_TH0_H �Ĵ����ṹ���塣��ַƫ����:0x1E����ֵ:0x0F�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ0�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th0_h : 4;  /* bit[0-3]: ��ع�ѹ������ѹ������ֵ0�ĸ�4bit */
        unsigned char  reserved         : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH0_H_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH0_H_sc_soh_ovh_th0_h_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH0_H_sc_soh_ovh_th0_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH0_L_UNION
 �ṹ˵��  : TSBAT_OVH_TH0_L �Ĵ����ṹ���塣��ַƫ����:0x1F����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ0�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot0_l : 8;  /* bit[0-7]: ��ع�ѹ�����¶ȼ�����ֵ0�ĵ�8bit */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH0_L_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH0_L_sc_tsbat_ot0_l_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH0_L_sc_tsbat_ot0_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH0_H_UNION
 �ṹ˵��  : TSBAT_OVH_TH0_H �Ĵ����ṹ���塣��ַƫ����:0x20����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ0�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot0_h : 4;  /* bit[0-3]: ��ع�ѹ�����¶ȼ�����ֵ0�ĸ�4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH0_H_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH0_H_sc_tsbat_ot0_h_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH0_H_sc_tsbat_ot0_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH1_L_UNION
 �ṹ˵��  : SOH_OVH_TH1_L �Ĵ����ṹ���塣��ַƫ����:0x21����ֵ:0xFF�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ1�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th1_l : 8;  /* bit[0-7]: ��ع�ѹ������ѹ������ֵ1�ĵ�8bit */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH1_L_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH1_L_sc_soh_ovh_th1_l_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH1_L_sc_soh_ovh_th1_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH1_H_UNION
 �ṹ˵��  : SOH_OVH_TH1_H �Ĵ����ṹ���塣��ַƫ����:0x22����ֵ:0x0F�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ1�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th1_h : 4;  /* bit[0-3]: ��ع�ѹ������ѹ������ֵ1�ĸ�4bit */
        unsigned char  reserved         : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH1_H_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH1_H_sc_soh_ovh_th1_h_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH1_H_sc_soh_ovh_th1_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH1_L_UNION
 �ṹ˵��  : TSBAT_OVH_TH1_L �Ĵ����ṹ���塣��ַƫ����:0x23����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ1�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot1_l : 8;  /* bit[0-7]: ��ع�ѹ�����¶ȼ�����ֵ1�ĵ�8bit */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH1_L_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH1_L_sc_tsbat_ot1_l_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH1_L_sc_tsbat_ot1_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH1_H_UNION
 �ṹ˵��  : TSBAT_OVH_TH1_H �Ĵ����ṹ���塣��ַƫ����:0x24����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ1�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot1_h : 4;  /* bit[0-3]: ��ع�ѹ�����¶ȼ�����ֵ1�ĸ�4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH1_H_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH1_H_sc_tsbat_ot1_h_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH1_H_sc_tsbat_ot1_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH2_L_UNION
 �ṹ˵��  : SOH_OVH_TH2_L �Ĵ����ṹ���塣��ַƫ����:0x25����ֵ:0xFF�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ2�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th2_l : 8;  /* bit[0-7]: ��ع�ѹ������ѹ������ֵ2�ĵ�8bit */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH2_L_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH2_L_sc_soh_ovh_th2_l_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH2_L_sc_soh_ovh_th2_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVH_TH2_H_UNION
 �ṹ˵��  : SOH_OVH_TH2_H �Ĵ����ṹ���塣��ַƫ����:0x26����ֵ:0x0F�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ2�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovh_th2_h : 4;  /* bit[0-3]: ��ع�ѹ������ѹ������ֵ2�ĸ�4bit */
        unsigned char  reserved         : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_SOH_OVH_TH2_H_UNION;
#endif
#define SOC_SCHARGER_SOH_OVH_TH2_H_sc_soh_ovh_th2_h_START  (0)
#define SOC_SCHARGER_SOH_OVH_TH2_H_sc_soh_ovh_th2_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH2_L_UNION
 �ṹ˵��  : TSBAT_OVH_TH2_L �Ĵ����ṹ���塣��ַƫ����:0x27����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ2�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot2_l : 8;  /* bit[0-7]: ��ع�ѹ�����¶ȼ�����ֵ2�ĵ�8bit */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH2_L_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH2_L_sc_tsbat_ot2_l_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH2_L_sc_tsbat_ot2_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TSBAT_OVH_TH2_H_UNION
 �ṹ˵��  : TSBAT_OVH_TH2_H �Ĵ����ṹ���塣��ַƫ����:0x28����ֵ:0x00�����:8
 �Ĵ���˵��: ��ع�ѹ�����¶ȼ�����ֵ2�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_tsbat_ot2_h : 4;  /* bit[0-3]: ��ع�ѹ�����¶ȼ�����ֵ2�ĸ�4bit */
        unsigned char  reserved       : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_TSBAT_OVH_TH2_H_UNION;
#endif
#define SOC_SCHARGER_TSBAT_OVH_TH2_H_sc_tsbat_ot2_h_START  (0)
#define SOC_SCHARGER_TSBAT_OVH_TH2_H_sc_tsbat_ot2_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVL_TH_L_UNION
 �ṹ˵��  : SOH_OVL_TH_L �Ĵ����ṹ���塣��ַƫ����:0x29����ֵ:0x52�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ�ĵ�8bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovl_th_l : 8;  /* bit[0-7]: ��ع�ѹ������ѹ������ֵ�ĵ�8bit */
    } reg;
} SOC_SCHARGER_SOH_OVL_TH_L_UNION;
#endif
#define SOC_SCHARGER_SOH_OVL_TH_L_sc_soh_ovl_th_l_START  (0)
#define SOC_SCHARGER_SOH_OVL_TH_L_sc_soh_ovl_th_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVL_TH_H_UNION
 �ṹ˵��  : SOH_OVL_TH_H �Ĵ����ṹ���塣��ַƫ����:0x2A����ֵ:0x08�����:8
 �Ĵ���˵��: ��ع�ѹ������ѹ������ֵ�ĸ�4bit
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_soh_ovl_th_h : 4;  /* bit[0-3]: ��ع�ѹ������ѹ������ֵ�ĸ�4bit */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_SOH_OVL_TH_H_UNION;
#endif
#define SOC_SCHARGER_SOH_OVL_TH_H_sc_soh_ovl_th_h_START  (0)
#define SOC_SCHARGER_SOH_OVL_TH_H_sc_soh_ovl_th_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SOH_OVP_REAL_UNION
 �ṹ˵��  : SOH_OVP_REAL �Ĵ����ṹ���塣��ַƫ����:0x2B����ֵ:0x00�����:8
 �Ĵ���˵��: SOH OVP ��/����ֵ���ʵʱ��¼
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_soh_ovh   : 1;  /* bit[0-0]: ����ֵ�����������ʱ��Ƚ϶̣��鿴��Ӧ���жϼĴ����� */
        unsigned char  tb_soh_ovl   : 1;  /* bit[1-1]: ��ع�ѹ��������ֵ����� */
        unsigned char  tb_tmp_ovh_2 : 1;  /* bit[2-2]: TBAT�¶ȱ�������ֵ2����� */
        unsigned char  tb_soh_ovh_2 : 1;  /* bit[3-3]: ��ع�ѹ������ѹ��ֵ2����� */
        unsigned char  tb_tmp_ovh_1 : 1;  /* bit[4-4]: TBAT�¶ȱ�������ֵ1����� */
        unsigned char  tb_soh_ovh_1 : 1;  /* bit[5-5]: ��ع�ѹ������ѹ��ֵ1����� */
        unsigned char  tb_tmp_ovh_0 : 1;  /* bit[6-6]: TBAT�¶ȱ�������ֵ0����� */
        unsigned char  tb_soh_ovh_0 : 1;  /* bit[7-7]: ��ع�ѹ������ѹ��ֵ0����� */
    } reg;
} SOC_SCHARGER_SOH_OVP_REAL_UNION;
#endif
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_START    (0)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_END      (0)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovl_START    (1)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovl_END      (1)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_2_START  (2)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_2_END    (2)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_2_START  (3)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_2_END    (3)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_1_START  (4)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_1_END    (4)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_1_START  (5)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_1_END    (5)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_0_START  (6)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_tmp_ovh_0_END    (6)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_0_START  (7)
#define SOC_SCHARGER_SOH_OVP_REAL_tb_soh_ovh_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_TB_EN_SEL_UNION
 �ṹ˵��  : HKADC_TB_EN_SEL �Ĵ����ṹ���塣��ַƫ����:0x2C����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC Testbusʹ�ܼ�ѡ���ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_hkadc_tb_sel : 7;  /* bit[0-6]: Testbusѡ���źţ�debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  sc_hkadc_tb_en  : 1;  /* bit[7-7]: Testbusʹ���źţ�����Ч��debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_HKADC_TB_EN_SEL_UNION;
#endif
#define SOC_SCHARGER_HKADC_TB_EN_SEL_sc_hkadc_tb_sel_START  (0)
#define SOC_SCHARGER_HKADC_TB_EN_SEL_sc_hkadc_tb_sel_END    (6)
#define SOC_SCHARGER_HKADC_TB_EN_SEL_sc_hkadc_tb_en_START   (7)
#define SOC_SCHARGER_HKADC_TB_EN_SEL_sc_hkadc_tb_en_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_TB_DATA0_UNION
 �ṹ˵��  : HKADC_TB_DATA0 �Ĵ����ṹ���塣��ַƫ����:0x2D����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC testbus�ض�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_tb_bus_0 : 8;  /* bit[0-7]: HKADC testbus���Իض����ݣ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_HKADC_TB_DATA0_UNION;
#endif
#define SOC_SCHARGER_HKADC_TB_DATA0_hkadc_tb_bus_0_START  (0)
#define SOC_SCHARGER_HKADC_TB_DATA0_hkadc_tb_bus_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_HKADC_TB_DATA1_UNION
 �ṹ˵��  : HKADC_TB_DATA1 �Ĵ����ṹ���塣��ַƫ����:0x2E����ֵ:0x00�����:8
 �Ĵ���˵��: HKADC testbus�ض�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  hkadc_tb_bus_1 : 8;  /* bit[0-7]: HKADC testbus���Իض����ݣ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_HKADC_TB_DATA1_UNION;
#endif
#define SOC_SCHARGER_HKADC_TB_DATA1_hkadc_tb_bus_1_START  (0)
#define SOC_SCHARGER_HKADC_TB_DATA1_hkadc_tb_bus_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_TOP_CFG_REG_0_UNION
 �ṹ˵��  : ACR_TOP_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x30����ֵ:0x09�����:8
 �Ĵ���˵��: ACR_TOP_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_acr_mul_sel    : 2;  /* bit[0-1]: acr��ѹ���Ŵ�����
                                                            00��50��
                                                            01��100��
                                                            10��200��
                                                            11��300�� */
        unsigned char  da_acr_iref_sel   : 1;  /* bit[2-2]: ���ڽ����ڲ��ŵ�������üĴ�����Ч */
        unsigned char  da_acr_cap_sel    : 2;  /* bit[3-4]: acr��ѹ��ⲹ������ѡ��
                                                            00��8C
                                                            01��4C
                                                            10��2C
                                                            11��1C */
        unsigned char  da_acr_vdetop_cap : 1;  /* bit[5-5]: vdet_op�ڲ��������ݣ�
                                                            0��������vdet_op�ڲ��������ݣ�
                                                            1������vdet_op�ڲ��������ݣ� */
        unsigned char  da_acr_testmode   : 1;  /* bit[6-6]: acr����ģʽ��
                                                            0����ʹ�ܲ���ģʽ
                                                            1��ʹ�ܲ���ģʽ */
        unsigned char  reserved          : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_TOP_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_mul_sel_START     (0)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_mul_sel_END       (1)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_iref_sel_START    (2)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_iref_sel_END      (2)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_cap_sel_START     (3)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_cap_sel_END       (4)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_vdetop_cap_START  (5)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_vdetop_cap_END    (5)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_testmode_START    (6)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_0_da_acr_testmode_END      (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_TOP_CFG_REG_1_UNION
 �ṹ˵��  : ACR_TOP_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x31����ֵ:0x00�����:8
 �Ĵ���˵��: ACR_TOP_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_acr_reserve : 8;  /* bit[0-7]: Ԥ���Ĵ���:
                                                         <2:0>:ֱ�䱣���ڲ��ź����ѡ��
                                                         000-DGND
                                                         001-IBUS_UCP_INIT
                                                         010-VDROP_OVP_INIT
                                                         011-VDROP_NEG_OVP_INIT
                                                         100-VDROP_OVP_NFLT_INIT
                                                         101-VDROP_NEG_OVP_NFLT_INIT
                                                         110-IBAT_DCUCP_ALM_INIT
                                                         111-S_CFLYP_SCP
                                                         <3>:ֱ�䱣���ڲ��ź����ʹ�ܿ��ƣ�1-�����0-�����
                                                         <7:4>:Ԥ�� */
    } reg;
} SOC_SCHARGER_ACR_TOP_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_ACR_TOP_CFG_REG_1_da_acr_reserve_START  (0)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_1_da_acr_reserve_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_TOP_CFG_REG_2_UNION
 �ṹ˵��  : ACR_TOP_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x32����ֵ:0xFF�����:8
 �Ĵ���˵��: ACR_TOP_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ovp_idis_sel : 4;  /* bit[0-3]: ovp�ŵ����ѡ��:
                                                          0000��50mA
                                                          0001��60mA
                                                          0010��70mA
                                                          0011��80mA
                                                          0100��90mA
                                                          0101��100mA
                                                          0110��110mA
                                                          0111��120mA
                                                          1000��130mA
                                                          1001��140mA
                                                          1010��150mA
                                                          1011��160mA
                                                          1100��170mA
                                                          1101��180mA
                                                          1110��190mA
                                                          1111��200mA */
        unsigned char  da_pc_idis_sel  : 4;  /* bit[4-7]: ������ģʽ�ŵ����ѡ��
                                                          0000��50mA
                                                          0001��60mA
                                                          0010��70mA
                                                          0011��80mA
                                                          0100��90mA
                                                          0101��100mA
                                                          0110��110mA
                                                          0111��120mA
                                                          1000��130mA
                                                          1001��140mA
                                                          1010��150mA
                                                          1011��160mA
                                                          1100��170mA
                                                          1101��180mA
                                                          1110��190mA
                                                          1111��200mA */
    } reg;
} SOC_SCHARGER_ACR_TOP_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_ACR_TOP_CFG_REG_2_da_ovp_idis_sel_START  (0)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_2_da_ovp_idis_sel_END    (3)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_2_da_pc_idis_sel_START   (4)
#define SOC_SCHARGER_ACR_TOP_CFG_REG_2_da_pc_idis_sel_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_SAMPLE_TIME_H_UNION
 �ṹ˵��  : ACR_SAMPLE_TIME_H �Ĵ����ṹ���塣��ַƫ����:0x33����ֵ:0x33�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڲ���ʱ�䵵λ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_acr_sample_t2 : 3;  /* bit[0-2]: ACRģ�鱻�����źŸߵ�ƽ�ڼ�������T2��λ��
                                                           3��b000��113us��
                                                           3��b001��109us��
                                                           3��b010��105us��
                                                           3��b011��101us��Ĭ�ϣ���
                                                           3��b100��97us��
                                                           3��b101��93us��
                                                           3��b110��89us��
                                                           3��b111��85us��
                                                           �����֤��1�����õ�T1+4*T2������acr_pulse�İ�����ڣ�500us)��
                                                           2��T3+4*T4���ܳ��������acr_pulse���ڣ�500us�� */
        unsigned char  reserved_0       : 1;  /* bit[3-3]: reserved */
        unsigned char  sc_acr_sample_t1 : 3;  /* bit[4-6]: ACRģ�鱻�����ź��������ȶ�ʱ��T1��λ��
                                                           3��b000��48us��
                                                           3��b001��64us��
                                                           3��b010��80us��
                                                           3��b011��96us��Ĭ�ϣ���
                                                           3��b100��112us��
                                                           3��b101��128us��
                                                           3��b110��144us��
                                                           3��b111��160us��
                                                           �����֤��1�����õ�T1+4*T2������acr_pulse�İ�����ڣ�500us)��
                                                           2��T3+4*T4���ܳ��������acr_pulse���ڣ�500us�� */
        unsigned char  reserved_1       : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_SAMPLE_TIME_H_UNION;
#endif
#define SOC_SCHARGER_ACR_SAMPLE_TIME_H_sc_acr_sample_t2_START  (0)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_H_sc_acr_sample_t2_END    (2)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_H_sc_acr_sample_t1_START  (4)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_H_sc_acr_sample_t1_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_SAMPLE_TIME_L_UNION
 �ṹ˵��  : ACR_SAMPLE_TIME_L �Ĵ����ṹ���塣��ַƫ����:0x34����ֵ:0x33�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�͵�ƽ�ڲ���ʱ�䵵λ
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_acr_sample_t4 : 3;  /* bit[0-2]: ACRģ�鱻�����źŵ͵�ƽ�ڼ�������T4��λ��
                                                           3��b000��113us��
                                                           3��b001��109us��
                                                           3��b010��105us��
                                                           3��b011��101us��Ĭ�ϣ���
                                                           3��b100��97us��
                                                           3��b101��93us��
                                                           3��b110��89us��
                                                           3��b111��85us��
                                                           �����֤��1�����õ�T1+4*T2������acr_pulse�İ�����ڣ�500us)��
                                                           2��T3+4*T4���ܳ��������acr_pulse���ڣ�500us�� */
        unsigned char  reserved_0       : 1;  /* bit[3-3]: reserved */
        unsigned char  sc_acr_sample_t3 : 3;  /* bit[4-6]: ACRģ�鱻�����ź��½����ȶ�ʱ��T3��λ��
                                                           3��b000��48us��
                                                           3��b001��64us��
                                                           3��b010��80us��
                                                           3��b011��96us��Ĭ�ϣ���
                                                           3��b100��112us��
                                                           3��b101��128us��
                                                           3��b110��144us��
                                                           3��b111��160us��
                                                           �����֤��1�����õ�T1+4*T2������acr_pulse�İ�����ڣ�500us)��
                                                           2��T3+4*T4���ܳ��������acr_pulse���ڣ�500us�� */
        unsigned char  reserved_1       : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_SAMPLE_TIME_L_UNION;
#endif
#define SOC_SCHARGER_ACR_SAMPLE_TIME_L_sc_acr_sample_t4_START  (0)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_L_sc_acr_sample_t4_END    (2)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_L_sc_acr_sample_t3_START  (4)
#define SOC_SCHARGER_ACR_SAMPLE_TIME_L_sc_acr_sample_t3_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA0_L_UNION
 �ṹ˵��  : ACR_DATA0_L �Ĵ����ṹ���塣��ַƫ����:0x35����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data0_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA0_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA0_L_acr_data0_l_START  (0)
#define SOC_SCHARGER_ACR_DATA0_L_acr_data0_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA0_H_UNION
 �ṹ˵��  : ACR_DATA0_H �Ĵ����ṹ���塣��ַƫ����:0x36����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data0_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ��һ���������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA0_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA0_H_acr_data0_h_START  (0)
#define SOC_SCHARGER_ACR_DATA0_H_acr_data0_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA1_L_UNION
 �ṹ˵��  : ACR_DATA1_L �Ĵ����ṹ���塣��ַƫ����:0x37����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data1_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA1_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA1_L_acr_data1_l_START  (0)
#define SOC_SCHARGER_ACR_DATA1_L_acr_data1_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA1_H_UNION
 �ṹ˵��  : ACR_DATA1_H �Ĵ����ṹ���塣��ַƫ����:0x38����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data1_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڶ����������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA1_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA1_H_acr_data1_h_START  (0)
#define SOC_SCHARGER_ACR_DATA1_H_acr_data1_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA2_L_UNION
 �ṹ˵��  : ACR_DATA2_L �Ĵ����ṹ���塣��ַƫ����:0x39����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data2_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA2_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA2_L_acr_data2_l_START  (0)
#define SOC_SCHARGER_ACR_DATA2_L_acr_data2_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA2_H_UNION
 �ṹ˵��  : ACR_DATA2_H �Ĵ����ṹ���塣��ַƫ����:0x3A����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data2_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA2_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA2_H_acr_data2_h_START  (0)
#define SOC_SCHARGER_ACR_DATA2_H_acr_data2_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA3_L_UNION
 �ṹ˵��  : ACR_DATA3_L �Ĵ����ṹ���塣��ַƫ����:0x3B����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data3_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA3_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA3_L_acr_data3_l_START  (0)
#define SOC_SCHARGER_ACR_DATA3_L_acr_data3_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA3_H_UNION
 �ṹ˵��  : ACR_DATA3_H �Ĵ����ṹ���塣��ַƫ����:0x3C����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data3_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ���ĸ��������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA3_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA3_H_acr_data3_h_START  (0)
#define SOC_SCHARGER_ACR_DATA3_H_acr_data3_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA4_L_UNION
 �ṹ˵��  : ACR_DATA4_L �Ĵ����ṹ���塣��ַƫ����:0x3D����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ������������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data4_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ������������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA4_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA4_L_acr_data4_l_START  (0)
#define SOC_SCHARGER_ACR_DATA4_L_acr_data4_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA4_H_UNION
 �ṹ˵��  : ACR_DATA4_H �Ĵ����ṹ���塣��ַƫ����:0x3E����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ������������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data4_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ������������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA4_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA4_H_acr_data4_h_START  (0)
#define SOC_SCHARGER_ACR_DATA4_H_acr_data4_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA5_L_UNION
 �ṹ˵��  : ACR_DATA5_L �Ĵ����ṹ���塣��ַƫ����:0x3F����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data5_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA5_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA5_L_acr_data5_l_START  (0)
#define SOC_SCHARGER_ACR_DATA5_L_acr_data5_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA5_H_UNION
 �ṹ˵��  : ACR_DATA5_H �Ĵ����ṹ���塣��ַƫ����:0x40����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data5_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�������������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA5_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA5_H_acr_data5_h_START  (0)
#define SOC_SCHARGER_ACR_DATA5_H_acr_data5_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA6_L_UNION
 �ṹ˵��  : ACR_DATA6_L �Ĵ����ṹ���塣��ַƫ����:0x41����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data6_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA6_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA6_L_acr_data6_l_START  (0)
#define SOC_SCHARGER_ACR_DATA6_L_acr_data6_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA6_H_UNION
 �ṹ˵��  : ACR_DATA6_H �Ĵ����ṹ���塣��ַƫ����:0x42����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data6_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ���߸��������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA6_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA6_H_acr_data6_h_START  (0)
#define SOC_SCHARGER_ACR_DATA6_H_acr_data6_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA7_L_UNION
 �ṹ˵��  : ACR_DATA7_L �Ĵ����ṹ���塣��ַƫ����:0x43����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݵ�8bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data7_l : 8;  /* bit[0-7]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݵ�8bit�� */
    } reg;
} SOC_SCHARGER_ACR_DATA7_L_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA7_L_acr_data7_l_START  (0)
#define SOC_SCHARGER_ACR_DATA7_L_acr_data7_l_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_ACR_DATA7_H_UNION
 �ṹ˵��  : ACR_DATA7_H �Ĵ����ṹ���塣��ַƫ����:0x44����ֵ:0x00�����:8
 �Ĵ���˵��: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݸ�4bit��
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  acr_data7_h : 4;  /* bit[0-3]: ��n+1��acr_pulse�ߵ�ƽ�ڼ�ڰ˸��������ݸ�4bit�� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_ACR_DATA7_H_UNION;
#endif
#define SOC_SCHARGER_ACR_DATA7_H_acr_data7_h_START  (0)
#define SOC_SCHARGER_ACR_DATA7_H_acr_data7_h_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_UNION
 �ṹ˵��  : IRQ_MASK �Ĵ����ṹ���塣��ַƫ����:0x48����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask : 8;  /* bit[0-7]: �ж������ź�
                                                      bit[7]��ȫ�����μĴ���
                                                      1: ���������ж��ϱ�
                                                      0�������������ж��ϱ�
                                                      bit[6]���ж�����Դͷѡ��
                                                      1���ж�����λ�����ж�Դͷ
                                                      0���ж�����λ�������ж�Դͷ�����������
                                                      bit[5]��reserved
                                                      bit[4]��BUCK���ж����Σ�
                                                      1�������ϱ��жϣ�
                                                      0�������ϱ��жϣ�
                                                      bit[3]��LVC_SC���ж����Σ�
                                                      1�������ϱ��жϣ�
                                                      0�������ϱ��жϣ�
                                                      bit[2]��PD���ж�����
                                                      1: �����ж��ϱ�
                                                      0���������ж��ϱ�
                                                      bit[1]��Others���ж������ź�
                                                      1: �����ϱ��ж�
                                                      0���������ϱ��ж�
                                                      bit[0]��FCP���ж������ź�
                                                      1: �����ϱ��ж�
                                                      0���������ϱ��ж� */
    } reg;
} SOC_SCHARGER_IRQ_MASK_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_sc_irq_mask_START  (0)
#define SOC_SCHARGER_IRQ_MASK_sc_irq_mask_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_0_UNION
 �ṹ˵��  : IRQ_MASK_0 �Ĵ����ṹ���塣��ַƫ����:0x49����ֵ:0x02�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_0 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_chg_chgstate����Ӧ��ģ�ӿ���bit�źţ�
                                                        bit[6]:irq_vbus_ovp
                                                        bit[5]:irq_vbus_uvp
                                                        bit[4]:irq_vbat_ovp
                                                        bit[3]:irq_otg_ovp
                                                        bit[2]:irq_otg_uvp
                                                        bit[1]:irq_buck_ocp��Ĭ�������ϱ��жϣ�
                                                        bit[0]:irq_otg_scp */
    } reg;
} SOC_SCHARGER_IRQ_MASK_0_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_0_sc_irq_mask_0_START  (0)
#define SOC_SCHARGER_IRQ_MASK_0_sc_irq_mask_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_1_UNION
 �ṹ˵��  : IRQ_MASK_1 �Ĵ����ṹ���塣��ַƫ����:0x4A����ֵ:0x80�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_1 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_otg_ocp��Ĭ�������ϱ��жϣ�
                                                        bit[6]:irq_buck_scp
                                                        bit[5]:irq_trickle_chg_fault
                                                        bit[4]:irq_pre_chg_fault
                                                        bit[3]:irq_fast_chg_fault
                                                        bit[2]:irq_otmp_jreg
                                                        bit[1]:irq_chg_rechg_state
                                                        bit[0]:irq_sleep_mod */
    } reg;
} SOC_SCHARGER_IRQ_MASK_1_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_1_sc_irq_mask_1_START  (0)
#define SOC_SCHARGER_IRQ_MASK_1_sc_irq_mask_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_2_UNION
 �ṹ˵��  : IRQ_MASK_2 �Ĵ����ṹ���塣��ַƫ����:0x4B����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_2 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_reversbst
                                                        bit[6]:irq_vusb_buck_ovp
                                                        bit[5]:irq_vusb_otg_ovp

                                                        bit[4]:irq_vusb_lvc_ovp
                                                        bit[3]:irq_vbus_lvc_ov
                                                        bit[2]:irq_vbus_lvc_uv
                                                        bit[1]:irq_ibus_dc_ocp
                                                        bit[0]:irq_ibus_dc_ucp */
    } reg;
} SOC_SCHARGER_IRQ_MASK_2_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_2_sc_irq_mask_2_START  (0)
#define SOC_SCHARGER_IRQ_MASK_2_sc_irq_mask_2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_3_UNION
 �ṹ˵��  : IRQ_MASK_3 �Ĵ����ṹ���塣��ַƫ����:0x4C����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_3 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_ibus_dc_rcp
                                                        bit[6]:irq_vdrop_lvc_ov
                                                        bit[5]:irq_vbat_dc_ovp
                                                        bit[4]:irq_vusb_scauto_ovp
                                                        bit[3]:irq_bat_ov_clamp
                                                        bit[2]:irq_vout_ov_clamp
                                                        bit[1]:irq_ibus_clamp
                                                        bit[0]:irq_ibat_clamp */
    } reg;
} SOC_SCHARGER_IRQ_MASK_3_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_3_sc_irq_mask_3_START  (0)
#define SOC_SCHARGER_IRQ_MASK_3_sc_irq_mask_3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_4_UNION
 �ṹ˵��  : IRQ_MASK_4 �Ĵ����ṹ���塣��ַƫ����:0x4D����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_4 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_vbus_sc_ov
                                                        bit[6]:irq_vbus_sc_uv
                                                        bit[5]:irq_vin2vout_sc
                                                        bit[4]:irq_vbat_sc_uvp
                                                        bit[3]:irq_ibat_dc_ocp
                                                        bit[2]:irq_ibat_dcucp_alm
                                                        bit[1]:irq_ilim_sc_ocp
                                                        bit[0]:irq_ilim_bus_sc_ocp */
    } reg;
} SOC_SCHARGER_IRQ_MASK_4_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_4_sc_irq_mask_4_START  (0)
#define SOC_SCHARGER_IRQ_MASK_4_sc_irq_mask_4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_5_UNION
 �ṹ˵��  : IRQ_MASK_5 �Ĵ����ṹ���塣��ַƫ����:0x4E����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_5 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_cc_ov
                                                        bit[6]:irq_cc_ocp
                                                        bit[5]:irq_cc_uv
                                                        bit[4]:irq_acr_scp�����������÷ŵ�ͨ·��������жϣ�ģ���ڲ���Ϊ�أ����ж�ʵ����Ч��
                                                        bit[3]:soh_ovh
                                                        bit[2]:soh_ovl
                                                        bit[1]:acr_flag
                                                        bit[0]:irq_wdt */
    } reg;
} SOC_SCHARGER_IRQ_MASK_5_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_5_sc_irq_mask_5_START  (0)
#define SOC_SCHARGER_IRQ_MASK_5_sc_irq_mask_5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_6_UNION
 �ṹ˵��  : IRQ_MASK_6 �Ĵ����ṹ���塣��ַƫ����:0x4F����ֵ:0x04�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_6 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_regn_ocp
                                                        bit[6]:irq_chg_batfet_ocp
                                                        bit[5]:irq_otmp_140
                                                        bit[4]:irq_wl_otg_usbok
                                                        bit[3]:irq_vusb_ovp_alm
                                                        bit[2]:irq_vusb_uv��Ĭ�������ϱ��жϣ�
                                                        bit[1]:irq_tdie_ot_alm
                                                        bit[0]:irq_vbat_lv */
    } reg;
} SOC_SCHARGER_IRQ_MASK_6_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_6_sc_irq_mask_6_START  (0)
#define SOC_SCHARGER_IRQ_MASK_6_sc_irq_mask_6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_MASK_7_UNION
 �ṹ˵��  : IRQ_MASK_7 �Ĵ����ṹ���塣��ַƫ����:0x50����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_irq_mask_7 : 8;  /* bit[0-7]: �ж����μĴ�����
                                                        1�����ζ�Ӧ�жϣ������жϹܽ��ϱ����������жϱ�־�Ĵ���
                                                        0�������ζ�Ӧ�жϣ��������жϹܽź��жϱ�־�Ĵ���
                                                        bit[7]:irq_reserved[7]
                                                        bit[6]:irq_reserved[6]
                                                        bit[5]:irq_reserved[5]
                                                        bit[4]:irq_reserved[4]
                                                        bit[3]:irq_reserved[3]��irq_vout_dc_ovp vout��ѹ������dcģʽ��ʹ��
                                                        bit[2]:irq_reserved[2]
                                                        bit[1]:irq_reserved[1]��irq_vdrop_usb_ovp ovp�� drop�����жϣ�dcģʽ��ʹ�ܣ�
                                                        bit[0]:irq_reserved[0]��irq_cfly_scp,fly���ݶ�·�жϣ�dcģʽ��ʹ�� */
    } reg;
} SOC_SCHARGER_IRQ_MASK_7_UNION;
#endif
#define SOC_SCHARGER_IRQ_MASK_7_sc_irq_mask_7_START  (0)
#define SOC_SCHARGER_IRQ_MASK_7_sc_irq_mask_7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_0_UNION
 �ṹ˵��  : IRQ_STATUS_0 �Ĵ����ṹ���塣��ַƫ����:0x51����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_0 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:reserved
                                                          bit[6]:irq_vbus_ovp
                                                          bit[5]:irq_vbus_uvp
                                                          bit[4]:irq_vbat_ovp
                                                          bit[3]:irq_otg_ovp
                                                          bit[2]:irq_otg_uvp
                                                          bit[1]:irq_buck_ocp
                                                          bit[0]:irq_otg_scp */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_0_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_0_tb_irq_status_0_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_0_tb_irq_status_0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_1_UNION
 �ṹ˵��  : IRQ_STATUS_1 �Ĵ����ṹ���塣��ַƫ����:0x52����ֵ:0x01�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_1 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_otg_ocp
                                                          bit[6]:irq_buck_scp
                                                          bit[5]:irq_trickle_chg_fault
                                                          bit[4]:irq_pre_chg_fault
                                                          bit[3]:irq_fast_chg_fault
                                                          bit[2]:irq_otmp_jreg
                                                          bit[1]:irq_chg_rechg_state
                                                          bit[0]:irq_sleep_mod */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_1_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_1_tb_irq_status_1_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_1_tb_irq_status_1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_2_UNION
 �ṹ˵��  : IRQ_STATUS_2 �Ĵ����ṹ���塣��ַƫ����:0x53����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_2 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_reversbst
                                                          bit[6]:irq_vusb_buck_ovp
                                                          bit[5]:irq_vusb_otg_ovp

                                                          bit[4]:irq_vusb_lvc_ovp
                                                          bit[3]:irq_vbus_lvc_ov
                                                          bit[2]:irq_vbus_lvc_uv
                                                          bit[1]:irq_ibus_dc_ocp
                                                          bit[0]:irq_ibus_dc_ucp */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_2_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_2_tb_irq_status_2_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_2_tb_irq_status_2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_3_UNION
 �ṹ˵��  : IRQ_STATUS_3 �Ĵ����ṹ���塣��ַƫ����:0x54����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_3 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_ibus_dc_rcp
                                                          bit[6]:irq_vdrop_lvc_ov
                                                          bit[5]:irq_vbat_dc_ovp
                                                          bit[4]:irq_vusb_scauto_ovp
                                                          bit[3]:irq_bat_ov_clamp
                                                          bit[2]:irq_vout_ov_clamp
                                                          bit[1]:irq_ibus_clamp
                                                          bit[0]:irq_ibat_clamp */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_3_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_3_tb_irq_status_3_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_3_tb_irq_status_3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_4_UNION
 �ṹ˵��  : IRQ_STATUS_4 �Ĵ����ṹ���塣��ַƫ����:0x55����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_4 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_vbus_sc_ov
                                                          bit[6]:irq_vbus_sc_uv
                                                          bit[5]:irq_vin2vout_sc
                                                          bit[4]:irq_vbat_sc_uvp
                                                          bit[3]:irq_ibat_dc_ocp
                                                          bit[2]:irq_ibat_dcucp_alm
                                                          bit[1]:irq_ilim_sc_ocp
                                                          bit[0]:irq_ilim_bus_sc_ocp */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_4_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_4_tb_irq_status_4_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_4_tb_irq_status_4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_5_UNION
 �ṹ˵��  : IRQ_STATUS_5 �Ĵ����ṹ���塣��ַƫ����:0x56����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_5 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_cc_ov
                                                          bit[6]:irq_cc_ocp
                                                          bit[5]:irq_cc_uv
                                                          bit[4]:irq_acr_scp�����������÷ŵ�ͨ·��������жϣ�ģ���ڲ���Ϊ�أ����ж�ʵ����Ч��
                                                          bit[3]:soh_ovh
                                                          bit[2]:soh_ovl
                                                          bit[1]:acr_flag
                                                          bit[0]:irq_wdt
                                                          ˵�������ڿ��Ź�����timeout֮��Ὣ���Ź�ʱ�䵵λsc_watchdog_timer�Ĵ�����λΪ0����disable���Ź����ܣ����¿��Ź��ϱ��ж��ź�Ϊ1��ʱ��ԼΪ24us������sc_irq_flag_5[0]�ض���״̬1�ĸ��ʷǳ�С����ѯ���Ź������ʹ�ÿ��Ź�timeout�жϼĴ���sc_irq_flag_5[0] */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_5_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_5_tb_irq_status_5_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_5_tb_irq_status_5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_6_UNION
 �ṹ˵��  : IRQ_STATUS_6 �Ĵ����ṹ���塣��ַƫ����:0x57����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_6 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_regn_ocp
                                                          bit[6]:irq_chg_batfet_ocp
                                                          bit[5]:irq_otmp_140
                                                          bit[4]:irq_wl_otg_usbok
                                                          bit[3]:irq_vusb_ovp_alm
                                                          bit[2]:irq_vusb_uv
                                                          bit[1]:irq_tdie_ot_alm
                                                          bit[0]:irq_vbat_lv */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_6_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_6_tb_irq_status_6_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_6_tb_irq_status_6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_7_UNION
 �ṹ˵��  : IRQ_STATUS_7 �Ĵ����ṹ���塣��ַƫ����:0x58����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_7 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ���
                                                          bit[7]:irq_reserved[7]
                                                          bit[6]:irq_reserved[6]
                                                          bit[5]:irq_reserved[5]
                                                          bit[4]:irq_reserved[4]
                                                          bit[3]:irq_reserved[3]��irq_vout_dc_ovp vout��ѹ������dcģʽ��ʹ��
                                                          bit[2]:irq_reserved[2]
                                                          bit[1]:irq_reserved[1]��irq_vdrop_usb_ovp ovp�� drop�����жϣ�dcģʽ��ʹ�ܣ�
                                                          bit[0]:irq_reserved[0]��irq_cfly_scp,fly���ݶ�·�жϣ�dcģʽ��ʹ�� */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_7_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_7_tb_irq_status_7_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_7_tb_irq_status_7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_IRQ_STATUS_8_UNION
 �ṹ˵��  : IRQ_STATUS_8 �Ĵ����ṹ���塣��ַƫ����:0x59����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_irq_status_8 : 8;  /* bit[0-7]: �ж�Դʵʱ״̬�Ĵ�����
                                                          bit[7:2]��reserved
                                                          bit[1:0]��irq_chg_chgstate[1:0] */
    } reg;
} SOC_SCHARGER_IRQ_STATUS_8_UNION;
#endif
#define SOC_SCHARGER_IRQ_STATUS_8_tb_irq_status_8_START  (0)
#define SOC_SCHARGER_IRQ_STATUS_8_tb_irq_status_8_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_SEL_UNION
 �ṹ˵��  : EFUSE_SEL �Ĵ����ṹ���塣��ַƫ����:0x60����ֵ:0x00�����:8
 �Ĵ���˵��: EFUSEѡ���ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_sel      : 2;  /* bit[0-1]: efuseѡ���źţ�
                                                            2'b00����EFUSE1���п��ƣ�Ĭ�ϣ���
                                                            2'b01����EFUSE2���п��ƣ�
                                                            2'b10����EFUSE3���п��ƣ�
                                                            2'b11����ѡ���κ�EFUSE */
        unsigned char  reserved_0        : 2;  /* bit[2-3]: reserved */
        unsigned char  sc_efuse_pdob_sel : 1;  /* bit[4-4]: EFUSE���ݼĴ�������Ӧ��Ԥ�޵��Ĵ���ѡ���źţ�
                                                            1��b0������޵��źţ�Ĭ�ϣ���
                                                            1��b1������Ĵ���Ԥ�޵��ź� */
        unsigned char  reserved_1        : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_EFUSE_SEL_UNION;
#endif
#define SOC_SCHARGER_EFUSE_SEL_sc_efuse_sel_START       (0)
#define SOC_SCHARGER_EFUSE_SEL_sc_efuse_sel_END         (1)
#define SOC_SCHARGER_EFUSE_SEL_sc_efuse_pdob_sel_START  (4)
#define SOC_SCHARGER_EFUSE_SEL_sc_efuse_pdob_sel_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_CFG_0_UNION
 �ṹ˵��  : EFUSE_CFG_0 �Ĵ����ṹ���塣��ַƫ����:0x61����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_nr_cfg      : 1;  /* bit[0-0]: EFUSEģ���������NR�źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_pgenb_cfg   : 1;  /* bit[1-1]: EFUSEģ���������PGENB�źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_strobe_cfg  : 1;  /* bit[2-2]: EFUSEģ���������STROBE�źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_rd_ctrl     : 1;  /* bit[3-3]: EFUSEģ������������źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_prog_int    : 1;  /* bit[4-4]: EFUSEģ�������̺Ͷ�ģʽѡ���źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_prog_sel    : 1;  /* bit[5-5]: EFUSEģ����ѡ���źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_inctrl_sel  : 1;  /* bit[6-6]: EFUSEģ��EFUSE���������Զ�ģʽ�������ֶ�ģʽѡ���źţ���ӦEFUSE��sc_efuse_selѡ�� */
        unsigned char  sc_efuse_rd_mode_sel : 1;  /* bit[7-7]: EFUSEģ���ģʽѡ���źţ���ӦEFUSE��sc_efuse_selѡ�񣩣�
                                                               1��ͳһ64����ˢ�£�
                                                               0��ÿһ����ˢ�£�
                                                               Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_EFUSE_CFG_0_UNION;
#endif
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_nr_cfg_START       (0)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_nr_cfg_END         (0)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_pgenb_cfg_START    (1)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_pgenb_cfg_END      (1)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_strobe_cfg_START   (2)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_strobe_cfg_END     (2)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_rd_ctrl_START      (3)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_rd_ctrl_END        (3)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_prog_int_START     (4)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_prog_int_END       (4)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_prog_sel_START     (5)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_prog_sel_END       (5)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_inctrl_sel_START   (6)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_inctrl_sel_END     (6)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_rd_mode_sel_START  (7)
#define SOC_SCHARGER_EFUSE_CFG_0_sc_efuse_rd_mode_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_0_UNION
 �ṹ˵��  : EFUSE_WE_0 �Ĵ����ṹ���塣��ַƫ����:0x62����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we0 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE0����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_0_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_0_sc_efuse_we0_START  (0)
#define SOC_SCHARGER_EFUSE_WE_0_sc_efuse_we0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_1_UNION
 �ṹ˵��  : EFUSE_WE_1 �Ĵ����ṹ���塣��ַƫ����:0x63����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we1 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE1����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_1_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_1_sc_efuse_we1_START  (0)
#define SOC_SCHARGER_EFUSE_WE_1_sc_efuse_we1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_2_UNION
 �ṹ˵��  : EFUSE_WE_2 �Ĵ����ṹ���塣��ַƫ����:0x64����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we2 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE2����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_2_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_2_sc_efuse_we2_START  (0)
#define SOC_SCHARGER_EFUSE_WE_2_sc_efuse_we2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_3_UNION
 �ṹ˵��  : EFUSE_WE_3 �Ĵ����ṹ���塣��ַƫ����:0x65����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we3 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE3����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_3_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_3_sc_efuse_we3_START  (0)
#define SOC_SCHARGER_EFUSE_WE_3_sc_efuse_we3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_4_UNION
 �ṹ˵��  : EFUSE_WE_4 �Ĵ����ṹ���塣��ַƫ����:0x66����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we4 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE4����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_4_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_4_sc_efuse_we4_START  (0)
#define SOC_SCHARGER_EFUSE_WE_4_sc_efuse_we4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_5_UNION
 �ṹ˵��  : EFUSE_WE_5 �Ĵ����ṹ���塣��ַƫ����:0x67����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we5 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE5����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_5_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_5_sc_efuse_we5_START  (0)
#define SOC_SCHARGER_EFUSE_WE_5_sc_efuse_we5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_6_UNION
 �ṹ˵��  : EFUSE_WE_6 �Ĵ����ṹ���塣��ַƫ����:0x68����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we6 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE6����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_6_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_6_sc_efuse_we6_START  (0)
#define SOC_SCHARGER_EFUSE_WE_6_sc_efuse_we6_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_WE_7_UNION
 �ṹ˵��  : EFUSE_WE_7 �Ĵ����ṹ���塣��ַƫ����:0x69����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_we7 : 8;  /* bit[0-7]: EFUSEģ�������ź�WE7����ӦEFUSE��sc_efuse_selѡ�� */
    } reg;
} SOC_SCHARGER_EFUSE_WE_7_UNION;
#endif
#define SOC_SCHARGER_EFUSE_WE_7_sc_efuse_we7_START  (0)
#define SOC_SCHARGER_EFUSE_WE_7_sc_efuse_we7_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_UNION
 �ṹ˵��  : EFUSE_PDOB_PRE_ADDR_WE �Ĵ����ṹ���塣��ַƫ����:0x6A����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_pdob_pre_addr : 3;  /* bit[0-2]: EFUSEģ��Ԥ���Ĵ�����ַѡ��:
                                                                 3'b000��ѡ��Ԥ���Ĵ���0
                                                                 3'b001��ѡ��Ԥ���Ĵ���1
                                                                 ��
                                                                 3'b111��ѡ��Ԥ���Ĵ���7
                                                                 Ĭ�ϣ�0 */
        unsigned char  reserved_0             : 1;  /* bit[3-3]: reserved */
        unsigned char  sc_efuse_pdob_pre_we   : 1;  /* bit[4-4]: EFUSEģ��Ԥ���Ĵ���дʹ�ܣ��ߵ�ƽ��Ч���ٴ�д����������Ϊ0
                                                                 Ĭ�ϣ�0 */
        unsigned char  reserved_1             : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_UNION;
#endif
#define SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_sc_efuse_pdob_pre_addr_START  (0)
#define SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_sc_efuse_pdob_pre_addr_END    (2)
#define SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_sc_efuse_pdob_pre_we_START    (4)
#define SOC_SCHARGER_EFUSE_PDOB_PRE_ADDR_WE_sc_efuse_pdob_pre_we_END      (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_UNION
 �ṹ˵��  : EFUSE_PDOB_PRE_WDATA �Ĵ����ṹ���塣��ַƫ����:0x6B����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse_pdob_pre_wdata : 8;  /* bit[0-7]: EFUSEģ��Ԥ���Ĵ���д��ֵ
                                                                  Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_UNION;
#endif
#define SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_sc_efuse_pdob_pre_wdata_START  (0)
#define SOC_SCHARGER_EFUSE_PDOB_PRE_WDATA_sc_efuse_pdob_pre_wdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE1_TESTBUS_0_UNION
 �ṹ˵��  : EFUSE1_TESTBUS_0 �Ĵ����ṹ���塣��ַƫ����:0x6C����ֵ:0x10�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_efuse1_state      : 4;  /* bit[0-3]: EFUSE1ģ���ڲ�״̬���ض��źţ�debugʹ�ã����Բ�Ʒ���ţ���
                                                               4'h0:STANDBY
                                                               4'h1:RD_READY
                                                               4'h2:RD_START
                                                               4'h3:RD_BIT
                                                               4'h4:RD_BIT_DONE
                                                               4'h5:RD_INT
                                                               4'h6:WR_READY
                                                               4'h7:WR_START
                                                               4'h8:WR_BIT
                                                               4'h9:WR_BIT_DONE
                                                               4'ha:WR_INT
                                                               Ĭ�ϣ�4'h0 */
        unsigned char  tb_efuse1_por_int_ro : 1;  /* bit[4-4]: ��ģ�ӿ��ź�efuse_por_int�ض���debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  reserved             : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_EFUSE1_TESTBUS_0_UNION;
#endif
#define SOC_SCHARGER_EFUSE1_TESTBUS_0_tb_efuse1_state_START       (0)
#define SOC_SCHARGER_EFUSE1_TESTBUS_0_tb_efuse1_state_END         (3)
#define SOC_SCHARGER_EFUSE1_TESTBUS_0_tb_efuse1_por_int_ro_START  (4)
#define SOC_SCHARGER_EFUSE1_TESTBUS_0_tb_efuse1_por_int_ro_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE2_TESTBUS_0_UNION
 �ṹ˵��  : EFUSE2_TESTBUS_0 �Ĵ����ṹ���塣��ַƫ����:0x6D����ֵ:0x10�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_efuse2_state      : 4;  /* bit[0-3]: EFUSE2ģ���ڲ�״̬���ض��źţ�debugʹ�ã����Բ�Ʒ���ţ���
                                                               4'h0:STANDBY
                                                               4'h1:RD_READY
                                                               4'h2:RD_START
                                                               4'h3:RD_BIT
                                                               4'h4:RD_BIT_DONE
                                                               4'h5:RD_INT
                                                               4'h6:WR_READY
                                                               4'h7:WR_START
                                                               4'h8:WR_BIT
                                                               4'h9:WR_BIT_DONE
                                                               4'ha:WR_INT
                                                               Ĭ�ϣ�4'h0 */
        unsigned char  tb_efuse2_por_int_ro : 1;  /* bit[4-4]: ��ģ�ӿ��ź�efuse_por_int�ض���debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  reserved             : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_EFUSE2_TESTBUS_0_UNION;
#endif
#define SOC_SCHARGER_EFUSE2_TESTBUS_0_tb_efuse2_state_START       (0)
#define SOC_SCHARGER_EFUSE2_TESTBUS_0_tb_efuse2_state_END         (3)
#define SOC_SCHARGER_EFUSE2_TESTBUS_0_tb_efuse2_por_int_ro_START  (4)
#define SOC_SCHARGER_EFUSE2_TESTBUS_0_tb_efuse2_por_int_ro_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE3_TESTBUS_0_UNION
 �ṹ˵��  : EFUSE3_TESTBUS_0 �Ĵ����ṹ���塣��ַƫ����:0x6E����ֵ:0x10�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_efuse3_state      : 4;  /* bit[0-3]: EFUSE3ģ���ڲ�״̬���ض��źţ�debugʹ�ã����Բ�Ʒ���ţ���
                                                               4'h0:STANDBY
                                                               4'h1:RD_READY
                                                               4'h2:RD_START
                                                               4'h3:RD_BIT
                                                               4'h4:RD_BIT_DONE
                                                               4'h5:RD_INT
                                                               4'h6:WR_READY
                                                               4'h7:WR_START
                                                               4'h8:WR_BIT
                                                               4'h9:WR_BIT_DONE
                                                               4'ha:WR_INT
                                                               Ĭ�ϣ�4'h0 */
        unsigned char  tb_efuse3_por_int_ro : 1;  /* bit[4-4]: ��ģ�ӿ��ź�efuse_por_int�ض���debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  reserved             : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_EFUSE3_TESTBUS_0_UNION;
#endif
#define SOC_SCHARGER_EFUSE3_TESTBUS_0_tb_efuse3_state_START       (0)
#define SOC_SCHARGER_EFUSE3_TESTBUS_0_tb_efuse3_state_END         (3)
#define SOC_SCHARGER_EFUSE3_TESTBUS_0_tb_efuse3_por_int_ro_START  (4)
#define SOC_SCHARGER_EFUSE3_TESTBUS_0_tb_efuse3_por_int_ro_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_TESTBUS_SEL_UNION
 �ṹ˵��  : EFUSE_TESTBUS_SEL �Ĵ����ṹ���塣��ַƫ����:0x6F����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse2_testbus_sel : 4;  /* bit[0-3]: EFUSE2ģ������ź�ѡ��debugʹ�ã����Բ�Ʒ���ţ�
                                                                Ĭ�ϣ�0 */
        unsigned char  sc_efuse3_testbus_sel : 4;  /* bit[4-7]: EFUSE3ģ������ź�ѡ��debugʹ�ã����Բ�Ʒ���ţ�
                                                                Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_EFUSE_TESTBUS_SEL_UNION;
#endif
#define SOC_SCHARGER_EFUSE_TESTBUS_SEL_sc_efuse2_testbus_sel_START  (0)
#define SOC_SCHARGER_EFUSE_TESTBUS_SEL_sc_efuse2_testbus_sel_END    (3)
#define SOC_SCHARGER_EFUSE_TESTBUS_SEL_sc_efuse3_testbus_sel_START  (4)
#define SOC_SCHARGER_EFUSE_TESTBUS_SEL_sc_efuse3_testbus_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE_TESTBUS_CFG_UNION
 �ṹ˵��  : EFUSE_TESTBUS_CFG �Ĵ����ṹ���塣��ַƫ����:0x70����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_efuse1_state_reset : 1;  /* bit[0-0]: EFUSE1ģ��״̬����λ�źţ���ƽ��Ч��debugʹ�ã����Բ�Ʒ���ţ�
                                                                1����λ״̬����
                                                                0������λ״̬����
                                                                Ĭ�ϣ�0 */
        unsigned char  sc_efuse2_state_reset : 1;  /* bit[1-1]: EFUSE2ģ��״̬����λ�źţ���ƽ��Ч��debugʹ�ã����Բ�Ʒ���ţ�
                                                                1����λ״̬����
                                                                0������λ״̬����
                                                                Ĭ�ϣ�0 */
        unsigned char  sc_efuse3_state_reset : 1;  /* bit[2-2]: EFUSE3ģ��״̬����λ�źţ���ƽ��Ч��debugʹ�ã����Բ�Ʒ���ţ�
                                                                1����λ״̬����
                                                                0������λ״̬����
                                                                Ĭ�ϣ�0 */
        unsigned char  sc_efuse_testbus_en   : 1;  /* bit[3-3]: EFUSE1/2/3ģ������ź����ʹ�ܣ�debugʹ�ã����Բ�Ʒ���ţ���
                                                                1����������źţ�
                                                                0������������źţ�
                                                                Ĭ�ϣ�0 */
        unsigned char  sc_efuse1_testbus_sel : 4;  /* bit[4-7]: EFUSE1ģ������ź�ѡ��debugʹ�ã����Բ�Ʒ���ţ�
                                                                Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_EFUSE_TESTBUS_CFG_UNION;
#endif
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse1_state_reset_START  (0)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse1_state_reset_END    (0)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse2_state_reset_START  (1)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse2_state_reset_END    (1)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse3_state_reset_START  (2)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse3_state_reset_END    (2)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse_testbus_en_START    (3)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse_testbus_en_END      (3)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse1_testbus_sel_START  (4)
#define SOC_SCHARGER_EFUSE_TESTBUS_CFG_sc_efuse1_testbus_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_UNION
 �ṹ˵��  : EFUSE1_TESTBUS_RDATA �Ĵ����ṹ���塣��ַƫ����:0x71����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  efuse1_testbus_rdata : 8;  /* bit[0-7]: EFUSE1ģ����Իض����ݣ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_UNION;
#endif
#define SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_efuse1_testbus_rdata_START  (0)
#define SOC_SCHARGER_EFUSE1_TESTBUS_RDATA_efuse1_testbus_rdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_UNION
 �ṹ˵��  : EFUSE2_TESTBUS_RDATA �Ĵ����ṹ���塣��ַƫ����:0x72����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  efuse2_testbus_rdata : 8;  /* bit[0-7]: EFUSE2ģ����Իض����ݣ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_UNION;
#endif
#define SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_efuse2_testbus_rdata_START  (0)
#define SOC_SCHARGER_EFUSE2_TESTBUS_RDATA_efuse2_testbus_rdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_UNION
 �ṹ˵��  : EFUSE3_TESTBUS_RDATA �Ĵ����ṹ���塣��ַƫ����:0x73����ֵ:0x00�����:8
 �Ĵ���˵��:
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  efuse3_testbus_rdata : 8;  /* bit[0-7]: EFUSE3ģ����Իض����ݣ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_UNION;
#endif
#define SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_efuse3_testbus_rdata_START  (0)
#define SOC_SCHARGER_EFUSE3_TESTBUS_RDATA_efuse3_testbus_rdata_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_GLB_TESTBUS_CFG_UNION
 �ṹ˵��  : GLB_TESTBUS_CFG �Ĵ����ṹ���塣��ַƫ����:0x74����ֵ:0x00�����:8
 �Ĵ���˵��: ����ģ������ź����üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_glb_tb_sel : 4;  /* bit[0-3]: ����ģ������ź�ѡ���źţ�debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  reserved_0    : 1;  /* bit[4-4]: reserved */
        unsigned char  sc_glb_tb_en  : 1;  /* bit[5-5]: ���ģ������ź�ʹ���źţ�debugʹ�ã����Բ�Ʒ���ţ� */
        unsigned char  reserved_1    : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_GLB_TESTBUS_CFG_UNION;
#endif
#define SOC_SCHARGER_GLB_TESTBUS_CFG_sc_glb_tb_sel_START  (0)
#define SOC_SCHARGER_GLB_TESTBUS_CFG_sc_glb_tb_sel_END    (3)
#define SOC_SCHARGER_GLB_TESTBUS_CFG_sc_glb_tb_en_START   (5)
#define SOC_SCHARGER_GLB_TESTBUS_CFG_sc_glb_tb_en_END     (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_GLB_TEST_DATA_UNION
 �ṹ˵��  : GLB_TEST_DATA �Ĵ����ṹ���塣��ַƫ����:0x75����ֵ:0x00�����:8
 �Ĵ���˵��: ����ģ������ź�
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  tb_glb_data : 8;  /* bit[0-7]: ����ģ������źţ�debugʹ�ã����Բ�Ʒ���ţ� */
    } reg;
} SOC_SCHARGER_GLB_TEST_DATA_UNION;
#endif
#define SOC_SCHARGER_GLB_TEST_DATA_tb_glb_data_START  (0)
#define SOC_SCHARGER_GLB_TEST_DATA_tb_glb_data_END    (7)




/****************************************************************************
                     (4/4) REG_PAGE2
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_0_UNION
 �ṹ˵��  : CHARGER_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x00����ֵ:0x09�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_cap2_sel    : 2;  /* bit[0-1]: charger��·�������ݵ���
                                                             00:2.2pF
                                                             01:4.4pF
                                                             10:6.6pF
                                                             11:8.8pF */
        unsigned char  da_chg_cap1_sel    : 2;  /* bit[2-3]: charger��·�������ݵ���
                                                             00:1.1pF
                                                             01:2.2pF
                                                             10:3.3pF
                                                             11:4.4pF */
        unsigned char  da_chg_bat_open    : 1;  /* bit[4-4]: ��ز���λָʾ
                                                             0�������λ
                                                             1����ز���λ */
        unsigned char  da_chg_ate_mode    : 1;  /* bit[5-5]: ATE����ģʽ����λ��
                                                             0������ATE����ģʽ
                                                             1������ΪATE����ģʽ */
        unsigned char  da_chg_acl_rpt_en  : 1;  /* bit[6-6]: ACL״̬�ϱ�ʹ��λ��
                                                             0����ʹ��ACL�ϱ�
                                                             1��ʹ��ACL�ϱ� */
        unsigned char  da_chg_2x_ibias_en : 1;  /* bit[7-7]: rev_ok�Ƚ���ƫ�õ�������λ��
                                                             0��defaultƫ�õ���
                                                             1��ƫ�õ���x2 */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_cap2_sel_START     (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_cap2_sel_END       (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_cap1_sel_START     (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_cap1_sel_END       (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_bat_open_START     (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_bat_open_END       (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_ate_mode_START     (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_ate_mode_END       (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_acl_rpt_en_START   (6)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_acl_rpt_en_END     (6)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_2x_ibias_en_START  (7)
#define SOC_SCHARGER_CHARGER_CFG_REG_0_da_chg_2x_ibias_en_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_1_UNION
 �ṹ˵��  : CHARGER_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x01����ֵ:0x96�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_eoc_del2m_en : 1;  /* bit[0-0]: EOC 2msȥ����λʹ��λ��
                                                              0����ʹ��2msȥ��
                                                              1��ʹ��2msȥ�� */
        unsigned char  da_chg_en_term      : 1;  /* bit[1-1]: ��ֹ������λ
                                                              0: Disabled
                                                              1: Enabled */
        unsigned char  da_chg_cp_src_sel   : 1;  /* bit[2-2]: cpʱ�ӵ�Դѡ��λ��
                                                              0��cpʱ�ӵ�Դǿ��ѡ��VBAT
                                                              1��cpʱ�ӵ�Դ��ǿ��ѡ��VBAT */
        unsigned char  da_chg_clk_div2_shd : 1;  /* bit[3-3]: ��䰲ȫ��ʱ�����ȵ���������������DPM�����зŻ�2��������������λ
                                                              0: 2X���ܲ�����
                                                              1: 2X�������� */
        unsigned char  da_chg_cap4_sel     : 2;  /* bit[4-5]: charger��·�������ݵ���
                                                              00:3.6pF
                                                              01:7.2pF
                                                              10:10.8pF
                                                              11:14.4pF */
        unsigned char  da_chg_cap3_sel     : 2;  /* bit[6-7]: charger��·�������ݵ���
                                                              00:2.3pF
                                                              01:4.6pF
                                                              10:6.9pF
                                                              11:9.2pF */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_eoc_del2m_en_START  (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_eoc_del2m_en_END    (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_en_term_START       (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_en_term_END         (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cp_src_sel_START    (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cp_src_sel_END      (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_clk_div2_shd_START  (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_clk_div2_shd_END    (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cap4_sel_START      (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cap4_sel_END        (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cap3_sel_START      (6)
#define SOC_SCHARGER_CHARGER_CFG_REG_1_da_chg_cap3_sel_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_2_UNION
 �ṹ˵��  : CHARGER_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x02����ֵ:0x13�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_fast_ichg : 5;  /* bit[0-4]: ���������С����λ
                                                           00000: 100mA
                                                           00001: 200mA
                                                           00010: 300mA
                                                           00011: 400mA
                                                           00100: 500mA
                                                           00101: 600mA
                                                           00110: 700mA
                                                           00111: 800mA
                                                           01000: 900mA
                                                           01001: 1000mA
                                                           01010: 1100mA
                                                           01011: 1200mA
                                                           01100: 1300mA
                                                           01101: 1400mA
                                                           01110: 1500mA
                                                           01111: 1600mA
                                                           10000: 1700mA
                                                           10001: 1800mA
                                                           10010: 1900mA
                                                           10011: 2000mA
                                                           10100: 2100mA
                                                           10101: 2200mA
                                                           10110: 2300mA
                                                           10111: 2400mA
                                                           11000: 2500mA
                                                           11001: 2600mA
                                                           11010: 2700mA
                                                           11011: 2800mA
                                                           11100: 2900mA
                                                           11101: 3000mA
                                                           11110: 3100mA
                                                           11111: 3200mA */
        unsigned char  reserved         : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_2_da_chg_fast_ichg_START  (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_2_da_chg_fast_ichg_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_3_UNION
 �ṹ˵��  : CHARGER_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x03����ֵ:0x60�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_fastchg_timer : 2;  /* bit[0-1]: ��䰲ȫ��ʱ����Сʱ��
                                                               00: 5 h
                                                               01: 8 h
                                                               10: 12 h
                                                               11: 20 h */
        unsigned char  da_chg_fast_vchg     : 5;  /* bit[2-6]: ������ɵ�ѹ��ֵ��С����λ
                                                               00000: 4V
                                                                ��
                                                                ��
                                                                ��
                                                               11000: 4.4V
                                                                ��
                                                                ��
                                                                ��
                                                               11111: 4.516V */
        unsigned char  reserved             : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_3_da_chg_fastchg_timer_START  (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_3_da_chg_fastchg_timer_END    (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_3_da_chg_fast_vchg_START      (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_3_da_chg_fast_vchg_END        (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_4_UNION
 �ṹ˵��  : CHARGER_CFG_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x04����ֵ:0x0C�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_ocp_adj    : 2;  /* bit[0-1]: batfet ocp��λ���ã�
                                                            00��1x
                                                            01��-20%
                                                            10��+20%
                                                            11��+30% */
        unsigned char  da_chg_md_sel     : 1;  /* bit[2-2]: VBAT>3.65V����絵λС��400mA��IR/GAPģʽ����λ��
                                                            0������ΪGAPģʽ
                                                            1������ΪIRģʽ */
        unsigned char  da_chg_iref_clamp : 2;  /* bit[3-4]: ������ǯλ
                                                            00��100mV
                                                            01: 150mV
                                                            10��200mV
                                                            11��250mV */
        unsigned char  da_chg_ir_set     : 3;  /* bit[5-7]: ���ͨ�����貹��
                                                            000: 0mohm
                                                            001: 15mohm
                                                            010: 30mohm
                                                            011: 45mohm
                                                            100: 60mohm
                                                            101: 75mohm
                                                            110: 90mohm
                                                            111: 105mohm */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_4_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_ocp_adj_START     (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_ocp_adj_END       (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_md_sel_START      (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_md_sel_END        (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_iref_clamp_START  (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_iref_clamp_END    (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_ir_set_START      (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_4_da_chg_ir_set_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_5_UNION
 �ṹ˵��  : CHARGER_CFG_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x05����ֵ:0x04�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_pre_vchg      : 2;  /* bit[0-1]: Ԥ����ѹ��ֵ��С����λ
                                                               00: 2.8V
                                                               01: 3.0V
                                                               10: 3.1V
                                                               11: 3.2V */
        unsigned char  da_chg_pre_ichg      : 2;  /* bit[2-3]: Ԥ��������С����λ
                                                               00: 100mA
                                                               01: 200mA
                                                               10: 300mA
                                                               11: 400mA */
        unsigned char  da_chg_ocp_test      : 1;  /* bit[4-4]: batfet ocp test mode����λ��
                                                               0��batfet ocp��ʹ��test mode
                                                               1:batfet ocpʹ��test mode */
        unsigned char  da_chg_ocp_shd       : 1;  /* bit[5-5]: ������ocp��������λ
                                                               0��������
                                                               1������ */
        unsigned char  da_chg_ocp_delay_shd : 1;  /* bit[6-6]: chg_ocp 128usȥ��ʱ������λ��
                                                               0��������128usȥ��
                                                               1������128usȥ�� */
        unsigned char  reserved             : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_5_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_pre_vchg_START       (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_pre_vchg_END         (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_pre_ichg_START       (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_pre_ichg_END         (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_test_START       (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_test_END         (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_shd_START        (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_shd_END          (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_delay_shd_START  (6)
#define SOC_SCHARGER_CHARGER_CFG_REG_5_da_chg_ocp_delay_shd_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_6_UNION
 �ṹ˵��  : CHARGER_CFG_REG_6 �Ĵ����ṹ���塣��ַƫ����:0x06����ֵ:0x02�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_rechg_time   : 2;  /* bit[0-1]: ���³��ģʽȥ��ʱ������λ
                                                              00: 0.1s
                                                              01: 1s
                                                              10: 2s
                                                              11: 4s */
        unsigned char  da_chg_q4_3x_shd    : 1;  /* bit[2-2]: batfet �����������λ
                                                              0������batfet�������󵽵���ֵ4��
                                                              1��������batfet�������󵽵���ֵ4�� */
        unsigned char  da_chg_prechg_timer : 2;  /* bit[3-4]: Ԥ����ʱ�������ӣ�
                                                              00: 30min
                                                              01: 45min
                                                              10: 60min
                                                              11: 75min */
        unsigned char  reserved            : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_6_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_rechg_time_START    (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_rechg_time_END      (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_q4_3x_shd_START     (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_q4_3x_shd_END       (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_prechg_timer_START  (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_6_da_chg_prechg_timer_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_7_UNION
 �ṹ˵��  : CHARGER_CFG_REG_7 �Ĵ����ṹ���塣��ַƫ����:0x07����ֵ:0x01�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_scp_en   : 1;  /* bit[0-0]: batfet_scp����ʹ��λ
                                                          0����ʹ��batfet_scp
                                                          1��ʹ��batfet_scp */
        unsigned char  da_chg_rpt_en   : 1;  /* bit[1-1]: charger�ڲ��ź��ϱ�ʹ��λ��
                                                          0����ʹ��charger�ڲ��ź��ϱ�
                                                          1��ʹ��charger�ڲ��ź��ϱ� */
        unsigned char  da_chg_rev_mode : 2;  /* bit[2-3]: supplement��·����ģʽѡ��
                                                          00��������ģʽ
                                                          01��������ģʽ
                                                          10�����ԣ�С����+����ģʽ
                                                          11�����ԣ���������+����ģʽ */
        unsigned char  da_chg_resvo    : 4;  /* bit[4-7]: bit<0>:gapģʽsenseդ����ѹ��������λ
                                                          0��ʹ��
                                                          1����ʹ��
                                                          bit<1>:IRģʽդ����ѹǿ�ƿ���������ѹʹ��λ
                                                          0����ʹ��
                                                          1��ʹ��
                                                          bit<3:2>:rev_ok�Ƚ�����ת��ֵ����λ
                                                          00:43.75mV
                                                          01:31.25mV
                                                          10:56.25mV
                                                          11:68.75mV */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_7_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_scp_en_START    (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_scp_en_END      (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_rpt_en_START    (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_rpt_en_END      (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_rev_mode_START  (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_rev_mode_END    (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_resvo_START     (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_7_da_chg_resvo_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_8_UNION
 �ṹ˵��  : CHARGER_CFG_REG_8 �Ĵ����ṹ���塣��ַƫ����:0x08����ֵ:0x10�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_8
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_vclamp_set   : 3;  /* bit[0-2]: ���ͨ�����貹��ǯλ��ѹ
                                                              000�� 0mV
                                                              001: 50mV
                                                              010: 100mV
                                                              011: 150mV
                                                              000: 200mV
                                                              101: 250mV
                                                              110: 300mV
                                                              111: 350mV */
        unsigned char  da_chg_vbat_plus6mv : 1;  /* bit[3-3]: rev_ok�Ƚ����Ϸ�ת��ֵѡ��λ��
                                                              0���Ƚ����Ϸ�ת��ֵ����Ϊvbat-6mV
                                                              1���Ƚ����Ϸ�ת��ֵ����Ϊvbat+6mV */
        unsigned char  da_chg_timer_en     : 1;  /* bit[4-4]: ��簲ȫ��ʱ��ʹ��λ��
                                                              0����ʹ��timer
                                                              1��ʹ��timer */
        unsigned char  da_chg_test_md      : 1;  /* bit[5-5]: ����ģʽ����λ��
                                                              0���ǲ���ģʽ
                                                              1������Ϊ����ģʽ */
        unsigned char  da_chg_term_ichg    : 2;  /* bit[6-7]: ��ֹ��������ֵ����λ

                                                              00: 150mA
                                                              01: 200mA
                                                              10: 300mA
                                                              11: 400mA */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_8_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_vclamp_set_START    (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_vclamp_set_END      (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_vbat_plus6mv_START  (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_vbat_plus6mv_END    (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_timer_en_START      (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_timer_en_END        (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_test_md_START       (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_test_md_END         (5)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_term_ichg_START     (6)
#define SOC_SCHARGER_CHARGER_CFG_REG_8_da_chg_term_ichg_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_CFG_REG_9_UNION
 �ṹ˵��  : CHARGER_CFG_REG_9 �Ĵ����ṹ���塣��ַƫ����:0x09����ֵ:0x15�����:8
 �Ĵ���˵��: CHARGER_���üĴ���_9
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_chg_vres2_sel  : 2;  /* bit[0-1]: charger��·����mos�������
                                                            00:1X RES
                                                            01:2X RES
                                                            10:3X RES
                                                            11:4X RES */
        unsigned char  da_chg_vres1_sel  : 2;  /* bit[2-3]: charger��·����mos�������
                                                            00:1X RES
                                                            01:2X RES
                                                            10:3X RES
                                                            11:4X RES */
        unsigned char  da_chg_vrechg_hys : 2;  /* bit[4-5]: ���³����͵�ѹ����λ
                                                            00: 100mV
                                                            01: 200mV
                                                            10: 300mV
                                                            11: 400mV */
        unsigned char  reserved          : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_CFG_REG_9_UNION;
#endif
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vres2_sel_START   (0)
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vres2_sel_END     (1)
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vres1_sel_START   (2)
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vres1_sel_END     (3)
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vrechg_hys_START  (4)
#define SOC_SCHARGER_CHARGER_CFG_REG_9_da_chg_vrechg_hys_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_RO_REG_10_UNION
 �ṹ˵��  : CHARGER_RO_REG_10 �Ĵ����ṹ���塣��ַƫ����:0x0A����ֵ:0x00�����:8
 �Ĵ���˵��: CHARGER_ֻ���Ĵ���_10
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_chg_sft_pd    : 1;  /* bit[0-0]: batfet ���ر��ź��ϱ���
                                                           0��batfetû�����ر�
                                                           1��batfet���ر� */
        unsigned char  ad_chg_rev_ok    : 1;  /* bit[1-1]: rev_ok�Ƚ������״̬�ϱ�
                                                           0��rev_ok=0
                                                           1��rev_ok=1 */
        unsigned char  ad_chg_rev_en    : 1;  /* bit[2-2]: supplementģʽʹ���ź��ϱ���
                                                           0��supplementģʽ��ʹ��
                                                           1��supplementģʽʹ�� */
        unsigned char  ad_chg_pu_btft   : 1;  /* bit[3-3]: batfet gate pull up�ź��ϱ���
                                                           0��batfetû��pull up
                                                           1��batfet pull up */
        unsigned char  ad_chg_pre_state : 1;  /* bit[4-4]: Ԥ���״ָ̬ʾλ��
                                                           0������Ԥ��״̬
                                                           1����Ԥ��״̬ */
        unsigned char  ad_chg_fwd_en    : 1;  /* bit[5-5]: ���ʹ���ź��ϱ���
                                                           0����粻ʹ��
                                                           1�����ʹ�� */
        unsigned char  ad_chg_dpm_state : 1;  /* bit[6-6]: ϵͳdpm״̬��¼�Ĵ���
                                                           0��Normal
                                                           1��In dpm regl */
        unsigned char  ad_chg_acl_state : 1;  /* bit[7-7]: ϵͳacl״̬��¼�Ĵ���
                                                           0��Normal
                                                           1��In acl regl */
    } reg;
} SOC_SCHARGER_CHARGER_RO_REG_10_UNION;
#endif

#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_sft_pd_START     (0)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_sft_pd_END       (0)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_rev_ok_START     (1)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_rev_ok_END       (1)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_rev_en_START     (2)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_rev_en_END       (2)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_pu_btft_START    (3)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_pu_btft_END      (3)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_pre_state_START  (4)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_pre_state_END    (4)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_fwd_en_START     (5)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_fwd_en_END       (5)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_dpm_state_START  (6)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_dpm_state_END    (6)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_acl_state_START  (7)
#define SOC_SCHARGER_CHARGER_RO_REG_10_ad_chg_acl_state_END    (7)

/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_RO_REG_11_UNION
 �ṹ˵��  : CHARGER_RO_REG_11 �Ĵ����ṹ���塣��ַƫ����:0x0B����ֵ:0x00�����:8
 �Ĵ���˵��: CHARGER_ֻ���Ĵ���_11
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_chg_tri_state   : 1;  /* bit[0-0]: ������״ָ̬ʾλ��
                                                             0������������״̬
                                                             1����������״̬ */
        unsigned char  ad_chg_therm_state : 1;  /* bit[1-1]: ϵͳ�ȵ���״̬��¼�Ĵ���
                                                             0��Normal
                                                             1��In Thermal regl */
        unsigned char  reserved           : 6;  /* bit[2-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_RO_REG_11_UNION;
#endif
#define SOC_SCHARGER_CHARGER_RO_REG_11_ad_chg_tri_state_START    (0)
#define SOC_SCHARGER_CHARGER_RO_REG_11_ad_chg_tri_state_END      (0)
#define SOC_SCHARGER_CHARGER_RO_REG_11_ad_chg_therm_state_START  (1)
#define SOC_SCHARGER_CHARGER_RO_REG_11_ad_chg_therm_state_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_USB_OVP_CFG_REG_0_UNION
 �ṹ˵��  : USB_OVP_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x0C����ֵ:0x01�����:8
 �Ĵ���˵��: USB_OVP_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vusb_ovp_resv    : 4;  /* bit[0-3]: <1:0>:sc_auto_ovp��ֵ�����Ĵ������ֱ��Ӧ
                                                              00 1.2V
                                                              01 0.9V
                                                              10 1.8V
                                                              11 1.5V
                                                              <3:2>Ԥ�� */
        unsigned char  da_vusb_ovp_lpf_sel : 1;  /* bit[4-4]: vusb�˲�ʱ�䣻
                                                              0�����˲� Ĭ�ϣ�
                                                              1���˲�100nS */
        unsigned char  da_vusb_force_ovpok : 1;  /* bit[5-5]: ǿ��ovpok
                                                              0����ǿ�� Ĭ�ϣ�
                                                              1��ǿ��ovpok */
        unsigned char  reserved            : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_USB_OVP_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_ovp_resv_START     (0)
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_ovp_resv_END       (3)
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_ovp_lpf_sel_START  (4)
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_ovp_lpf_sel_END    (4)
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_force_ovpok_START  (5)
#define SOC_SCHARGER_USB_OVP_CFG_REG_0_da_vusb_force_ovpok_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_USB_OVP_CFG_REG_1_UNION
 �ṹ˵��  : USB_OVP_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x0D����ֵ:0x1C�����:8
 �Ĵ���˵��: USB_OVP_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vusb_uvlo_sel        : 1;  /* bit[0-0]: vusb uvlo��ֵ����
                                                                  0:3.2V Ĭ��
                                                                  1:3.4V  */
        unsigned char  da_vusb_ovp_shield      : 1;  /* bit[1-1]: ����vusb ovp����
                                                                  ���ԡ���λר�� */
        unsigned char  da_vusb_ovp_set         : 4;  /* bit[2-5]: 0000:OVP��ѹΪ7.5V
                                                                  1100������:OVP��ѹΪ14V
                                                                  Ĭ�ϣ�0111��11V, step=0.5V������1V */
        unsigned char  da_vusb_sc_auto_ovp_enb : 1;  /* bit[6-6]: scģʽ��auto vusb ovp����
                                                                  0:�ù�����Ч��
                                                                  1���ù��ܲ���Ч�� */
        unsigned char  reserved                : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_USB_OVP_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_uvlo_sel_START         (0)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_uvlo_sel_END           (0)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_ovp_shield_START       (1)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_ovp_shield_END         (1)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_ovp_set_START          (2)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_ovp_set_END            (5)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_sc_auto_ovp_enb_START  (6)
#define SOC_SCHARGER_USB_OVP_CFG_REG_1_da_vusb_sc_auto_ovp_enb_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TCPC_CFG_REG_1_UNION
 �ṹ˵��  : TCPC_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x0E����ֵ:0x00�����:8
 �Ĵ���˵��: TCPC_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_typ_cfg0 : 8;  /* bit[0-7]: type_C���üĴ���
                                                      bit<0>��RX����ģʽѡ��0�������ֵ��ⷽʽ��1��������ֵ��ⷽʽ
                                                      bit<1>��CC OVP�ϱ������źţ�0�������Σ�1������
                                                      bit<2>��TX��ATE�޵�ʹ���źţ�0����ʹ�ܣ�1��ʹ��
                                                      bit<3>��RX��ATE�޵�ʹ���źţ�0����ʹ�ܣ�1��ʹ��
                                                      bit<5:4>��CC��OCP��λ����λ��
                                                      00��1x
                                                      01��0.8x
                                                      10��1.4x
                                                      11,1.2x
                                                      bit<6>��fast role swap���ʹ��λ��0����ʹ�ܣ�1��ʹ��
                                                      bit<7>��CC UVP�ϱ������źţ�0�������Σ�1������ */
    } reg;
} SOC_SCHARGER_TCPC_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_TCPC_CFG_REG_1_da_typ_cfg0_START  (0)
#define SOC_SCHARGER_TCPC_CFG_REG_1_da_typ_cfg0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TCPC_CFG_REG_2_UNION
 �ṹ˵��  : TCPC_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x0F����ֵ:0x00�����:8
 �Ĵ���˵��: TCPC_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_typ_cfg1 : 8;  /* bit[0-7]: type_C���üĴ���
                                                      bit<2:0>��RX�����ֵ���ģʽ��ֵ��λѡ��
                                                      000��105mV
                                                      001��113mV
                                                      010��120mV
                                                      011��127mV
                                                      100��97mV
                                                      101��89mV
                                                      110��81mV
                                                      111��73mV
                                                      bit<5:3>��TX��·�������赵λѡ��
                                                      000��31Kohm
                                                      001��41Kohm
                                                      010��51Kohm
                                                      011��61Kohm
                                                      100��21Kohm
                                                      101��11Kohm
                                                      110��6Kohm
                                                      111��1Kohm
                                                      bit<6>��CC OCP�ϱ������źţ�0�������Σ�1������
                                                      bit<7>��reserved */
    } reg;
} SOC_SCHARGER_TCPC_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_TCPC_CFG_REG_2_da_typ_cfg1_START  (0)
#define SOC_SCHARGER_TCPC_CFG_REG_2_da_typ_cfg1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TCPC_CFG_REG_3_UNION
 �ṹ˵��  : TCPC_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x10����ֵ:0x00�����:8
 �Ĵ���˵��: TCPC_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_typ_cfg2 : 8;  /* bit[0-7]: type_C���üĴ���
                                                      bit<2:0>��Tx��·�������ݵ�λѡ��
                                                      000��1pF
                                                      001��1.25pF
                                                      010��1.5pF
                                                      011��1.75pF
                                                      100��1pF
                                                      101��0.75pF
                                                      110��0.5pF
                                                      111��0.25pF
                                                      bit<7:3>��reserved */
    } reg;
} SOC_SCHARGER_TCPC_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_TCPC_CFG_REG_3_da_typ_cfg2_START  (0)
#define SOC_SCHARGER_TCPC_CFG_REG_3_da_typ_cfg2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_TCPC_RO_REG_5_UNION
 �ṹ˵��  : TCPC_RO_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x11����ֵ:0x00�����:8
 �Ĵ���˵��: TCPC_ֻ���Ĵ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_cc_resvi : 4;  /* bit[0-3]: reserve�Ĵ��� */
        unsigned char  reserved    : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_TCPC_RO_REG_5_UNION;
#endif
#define SOC_SCHARGER_TCPC_RO_REG_5_ad_cc_resvi_START  (0)
#define SOC_SCHARGER_TCPC_RO_REG_5_ad_cc_resvi_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_UNION
 �ṹ˵��  : SCHG_LOGIC_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x12����ֵ:0x08�����:8
 �Ĵ���˵��: SCHG_LOGIC_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_regn_ocp_shield : 1;  /* bit[0-0]: regn ocp�źŵ������źţ�
                                                             0��������regn_ocp��
                                                             1������regn_ocp�� */
        unsigned char  da_ovp_wl_en       : 1;  /* bit[1-1]: ovp���߳��ģʽ��
                                                             0�������߳��״̬��
                                                             1�����߳��״̬�� */
        unsigned char  da_otg_wl_en       : 1;  /* bit[2-2]: otg���߳��ģʽ��
                                                             0�������߳��״̬��
                                                             1�����߳��״̬�� */
        unsigned char  da_otg_delay_sel   : 2;  /* bit[3-4]: OTG������ʱʱ��ĵ�λѡ��
                                                             00����ʱ32ms��
                                                             01����ʱ64ms��
                                                             10����ʱ128ms��
                                                             11����ʱ256ms�� */
        unsigned char  da_ldo33_en        : 1;  /* bit[5-5]: LDO33ǿ��ʹ�ܣ�
                                                             0����ǿ��ʹ�ܣ�
                                                             1��ǿ��ʹ�ܣ� */
        unsigned char  reserved           : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_regn_ocp_shield_START  (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_regn_ocp_shield_END    (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_ovp_wl_en_START        (1)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_ovp_wl_en_END          (1)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_otg_wl_en_START        (2)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_otg_wl_en_END          (2)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_otg_delay_sel_START    (3)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_otg_delay_sel_END      (4)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_ldo33_en_START         (5)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_0_da_ldo33_en_END           (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_UNION
 �ṹ˵��  : SCHG_LOGIC_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x13����ֵ:0x00�����:8
 �Ĵ���˵��: SCHG_LOGIC_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sys_resvo1 : 8;  /* bit[0-7]: <0>:Ԥ���Ĵ���
                                                        <5>:irq_vbus_uvp����buckʹ��ʱ���˶�ʱ��ѡ��
                                                        0�����˶���
                                                        1���˶�16us�� */
    } reg;
} SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_da_sys_resvo1_START  (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_1_da_sys_resvo1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_UNION
 �ṹ˵��  : SCHG_LOGIC_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x14����ֵ:0x00�����:8
 �Ĵ���˵��: SCHG_LOGIC_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sys_resvo2 : 8;  /* bit[0-7]: da_sys_resvo2<4>��ADC�����źŲ���ʹ�ܿ��ƣ�
                                                        da_sys_resvo2<3:0>:����ͨ��ѡ��
                                                        0000��NA;
                                                        0001:VUSB��ѹ��
                                                        0010��IBUS������
                                                        0011��VBUS��ѹ��
                                                        0100��VOUT��ѹ��
                                                        0101����ص�ѹ��
                                                        0110����ص�����
                                                        0111��ACR��ѹ��
                                                        1000:NA;
                                                        1001��IBUS�ο�������
                                                        1010��NA;
                                                        1011:apple����������ѹ��
                                                        1100��оƬ���£� */
    } reg;
} SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_da_sys_resvo2_START  (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_2_da_sys_resvo2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_UNION
 �ṹ˵��  : SCHG_LOGIC_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x15����ֵ:0x00�����:8
 �Ĵ���˵��: SCHG_LOGIC_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_wl_en      : 1;  /* bit[0-0]: ���߳��ģʽ��
                                                        0�������߳��״̬��
                                                        1�����߳��״̬�� */
        unsigned char  da_timer_test : 1;  /* bit[1-1]: ��ʱ������ģʽ��
                                                        0��оƬ��������ģʽ��
                                                        1������ģʽ��ͨ�����ź�ѡ���ʱʱ��̵ļ�ʱ��������ԡ� */
        unsigned char  reserved      : 6;  /* bit[2-7]: reserved */
    } reg;
} SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_da_wl_en_START       (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_da_wl_en_END         (0)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_da_timer_test_START  (1)
#define SOC_SCHARGER_SCHG_LOGIC_CFG_REG_3_da_timer_test_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_UNION
 �ṹ˵��  : SCHG_LOGIC_RO_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x16����ֵ:0x00�����:8
 �Ĵ���˵��: SCHG_LOGIC_ֻ���Ĵ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_sys_resvi1 : 8;  /* bit[0-7]: �Ĵ���Ԥ�� */
    } reg;
} SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_UNION;
#endif
#define SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_ad_sys_resvi1_START  (0)
#define SOC_SCHARGER_SCHG_LOGIC_RO_REG_4_ad_sys_resvi1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_CHARGER_BATFET_CTRL_UNION
 �ṹ˵��  : CHARGER_BATFET_CTRL �Ĵ����ṹ���塣��ַƫ����:0x17����ֵ:0x01�����:8
 �Ĵ���˵��: BATFET���üĴ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_batfet_ctrl : 1;  /* bit[0-0]: ���ܿ��ƼĴ�����ϵͳ�ϵ縴λĬ��Ϊ1�������������Ϊ0������dc_plug_nΪ�͵�ƽ��ʱ��Ը�λ�üĴ�����
                                                         0���ر�batfet;
                                                         1����batfet; */
        unsigned char  reserved       : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_CHARGER_BATFET_CTRL_UNION;
#endif
#define SOC_SCHARGER_CHARGER_BATFET_CTRL_sc_batfet_ctrl_START  (0)
#define SOC_SCHARGER_CHARGER_BATFET_CTRL_sc_batfet_ctrl_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VBAT_LV_REG_UNION
 �ṹ˵��  : VBAT_LV_REG �Ĵ����ṹ���塣��ַƫ����:0x18����ֵ:0x00�����:8
 �Ĵ���˵��: VBAT LV�Ĵ���
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  vbat_lv_cfg : 1;  /* bit[0-0]: ��ص�ѹǷѹ��ǼĴ������ϵ縴λֵΪ0���ϵ�������������Ϊ1���ڼ�⵽��ز���λ����irq_vbat_lvΪ1��ʱ���üĴ��������㡣����üĴ�������Ӳ��λpwr_rst_nӰ�졣 */
        unsigned char  reserved    : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_VBAT_LV_REG_UNION;
#endif
#define SOC_SCHARGER_VBAT_LV_REG_vbat_lv_cfg_START  (0)
#define SOC_SCHARGER_VBAT_LV_REG_vbat_lv_cfg_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VDM_CFG_REG_0_UNION
 �ṹ˵��  : VDM_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x1A����ֵ:0x00�����:8
 �Ĵ���˵��: VDM����ģʽ�����üĴ���0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_pmos_en : 1;  /* bit[0-0]: VUSB��VBAT����PMOS����ʹ���źţ�
                                                     0����ʹ��PMOS����
                                                     1��ʹ��PMOS����
                                                     Ĭ��0 */
        unsigned char  reserved_0 : 3;  /* bit[1-3]: reserved */
        unsigned char  sc_chg_lp  : 1;  /* bit[4-4]: ����ģʽ�͹���״̬ʹ��λ��
                                                     0����ʹ�ܵ͹���
                                                     1��ʹ�ܵ͹���
                                                     Ĭ��0 */
        unsigned char  reserved_1 : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_VDM_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_VDM_CFG_REG_0_sc_pmos_en_START  (0)
#define SOC_SCHARGER_VDM_CFG_REG_0_sc_pmos_en_END    (0)
#define SOC_SCHARGER_VDM_CFG_REG_0_sc_chg_lp_START   (4)
#define SOC_SCHARGER_VDM_CFG_REG_0_sc_chg_lp_END     (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VDM_CFG_REG_1_UNION
 �ṹ˵��  : VDM_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x1B����ֵ:0x00�����:8
 �Ĵ���˵��: VDM����ģʽ�����üĴ���1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_uart_en : 1;  /* bit[0-0]: UARTͨ·ʹ���źţ�
                                                     0����ʹ��UARTͨ·
                                                     1��ʹ��UARTͨ·
                                                     Ĭ��0 */
        unsigned char  reserved_0 : 3;  /* bit[1-3]: reserved */
        unsigned char  sc_boot_en : 1;  /* bit[4-4]: BOOTʹ���źţ�
                                                     0����ʹ��BOOT����
                                                     1��ʹ��BOOT����
                                                     Ĭ��0 */
        unsigned char  reserved_1 : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_VDM_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_VDM_CFG_REG_1_sc_uart_en_START  (0)
#define SOC_SCHARGER_VDM_CFG_REG_1_sc_uart_en_END    (0)
#define SOC_SCHARGER_VDM_CFG_REG_1_sc_boot_en_START  (4)
#define SOC_SCHARGER_VDM_CFG_REG_1_sc_boot_en_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VDM_CFG_REG_2_UNION
 �ṹ˵��  : VDM_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x1C����ֵ:0x01�����:8
 �Ĵ���˵��: VDM����ģʽ�����üĴ���2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  sc_regn_en       : 1;  /* bit[0-0]: ldo5ʹ�ܿ��ƣ�
                                                           0����ʹ�ܣ�
                                                           1��ʹ�ܣ�
                                                           Ĭ��1 */
        unsigned char  reserved_0       : 3;  /* bit[1-3]: reserved */
        unsigned char  sc_vusb_off_mask : 1;  /* bit[4-4]: VBUS�µ縴λtest mode����λ��
                                                           0��������
                                                           1������
                                                           Ĭ��0 */
        unsigned char  reserved_1       : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_VDM_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_VDM_CFG_REG_2_sc_regn_en_START        (0)
#define SOC_SCHARGER_VDM_CFG_REG_2_sc_regn_en_END          (0)
#define SOC_SCHARGER_VDM_CFG_REG_2_sc_vusb_off_mask_START  (4)
#define SOC_SCHARGER_VDM_CFG_REG_2_sc_vusb_off_mask_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_0_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x20����ֵ:0x20�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  reserved            : 1;  /* bit[0-0]: reserved */
        unsigned char  da_dp_res_det_iqsel : 2;  /* bit[1-2]: DPLUS�˿��迹������ѡ��
                                                              00��1uA
                                                              01/10:10uA
                                                              11:100uA */
        unsigned char  da_dp_res_det_en    : 1;  /* bit[3-3]: DPLUS�˿��迹��⹦��ʹ�ܣ�
                                                              0����ʹ�ܣ�
                                                              1��ʹ�ܣ� */
        unsigned char  da_bat_gd_shield    : 1;  /* bit[4-4]: ǿ��ʹbat_gdΪ�ߣ�
                                                              0����ǿ�ƣ���ģ���ж�bat_gd�Ƿ�Ϊ�ߵ�ƽ
                                                              1��ǿ��ʹbat_gdΪ�ߵ�ƽ�� */
        unsigned char  da_bat_gd_sel       : 1;  /* bit[5-5]: OTG�����ĵ�ص�ѹ�жϵ�λѡ��
                                                              0��3V
                                                              1��2.8V */
        unsigned char  da_app_det_en       : 1;  /* bit[6-6]: ƻ���������⹦��ʹ�ܣ�
                                                              0����ʹ�ܣ�
                                                              1��ʹ�ܣ� */
        unsigned char  da_app_det_chsel    : 1;  /* bit[7-7]: ƻ����������ͨ��ѡ��
                                                              0��DMINUSͨ����
                                                              1��DPLUSͨ���� */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_dp_res_det_iqsel_START  (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_dp_res_det_iqsel_END    (2)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_dp_res_det_en_START     (3)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_dp_res_det_en_END       (3)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_bat_gd_shield_START     (4)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_bat_gd_shield_END       (4)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_bat_gd_sel_START        (5)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_bat_gd_sel_END          (5)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_app_det_en_START        (6)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_app_det_en_END          (6)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_app_det_chsel_START     (7)
#define SOC_SCHARGER_DET_TOP_CFG_REG_0_da_app_det_chsel_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_1_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x21����ֵ:0x0A�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbat_dc_ovp_t : 1;  /* bit[0-0]: SCC/LVC mode �´���VBAT ��ѹ������ȥ��ʱ��
                                                           1:1ms
                                                           0:0.1ms */
        unsigned char  da_slp_vset      : 1;  /* bit[1-1]: sleep���˳��ĵ�λѡ��
                                                           0: sleep���˳�Ϊ160mV��
                                                           1��sleep���˳�Ϊ120mV�� */
        unsigned char  da_sleep_block   : 1;  /* bit[2-2]: sleep�������źţ�
                                                           1: ����sleep���жϽ����slp_mod���Ϊ1
                                                           0��������sleep���жϽ�� */
        unsigned char  da_io_level      : 1;  /* bit[3-3]: D- ��Դѡ��
                                                           0��D-��VDDIO����(1.8V)
                                                           1��D-��AVDD����(3.3V) */
        unsigned char  reserved         : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_vbat_dc_ovp_t_START  (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_vbat_dc_ovp_t_END    (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_slp_vset_START       (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_slp_vset_END         (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_sleep_block_START    (2)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_sleep_block_END      (2)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_io_level_START       (3)
#define SOC_SCHARGER_DET_TOP_CFG_REG_1_da_io_level_END         (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_2_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x22����ֵ:0x01�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbat_lv_tset : 1;  /* bit[0-0]: ��ص�ѹ�˶���
                                                          0���˶�0.1ms��
                                                          1���˶�1ms;  */
        unsigned char  da_vbat_lv_t    : 1;  /* bit[1-1]: VBAT LV ȥ��ʱ������
                                                          1��1ms
                                                          0��0.1ms */
        unsigned char  reserved        : 6;  /* bit[2-7]: reserved */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_2_da_vbat_lv_tset_START  (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_2_da_vbat_lv_tset_END    (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_2_da_vbat_lv_t_START     (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_2_da_vbat_lv_t_END       (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_3_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x23����ֵ:0x64�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbat_sc_uvp_t : 1;  /* bit[0-0]: SCC mode �·���vbatǷѹȥ��ʱ��
                                                           1��8ms
                                                           0��1ms */
        unsigned char  da_vbat_ov_dc    : 7;  /* bit[1-7]: SCC/LVC ��ѹ������λ����
                                                           4 to 5.27 in 10mV
                                                           Ĭ��4.5V��110010�� */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_3_da_vbat_sc_uvp_t_START  (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_3_da_vbat_sc_uvp_t_END    (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_3_da_vbat_ov_dc_START     (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_3_da_vbat_ov_dc_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_4_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x24����ֵ:0x30�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbus_lvc_ov_t   : 1;  /* bit[0-0]: LVC vbus��ѹ����ȥ��ʱ������
                                                             1��256us
                                                             0��4us */
        unsigned char  da_vbus_hi_ovp_sel : 1;  /* bit[1-1]: VBUS HI OVP ����ѡ��
                                                             1:OVP ����Ϊ16V
                                                             0:OVP ����Ϊ7V */
        unsigned char  da_vbus_bkvset     : 2;  /* bit[2-3]: BUCK mode �����������ѹ��λ���ã�
                                                             00��5V
                                                             01��9V
                                                             10&amp;11:12V */
        unsigned char  da_vbat_uv_dc      : 3;  /* bit[4-6]: SCC/LVC Ƿѹ������λ����
                                                             3.35 to 3.7 in 50mV
                                                             Ĭ��3.5V ��011�� */
        unsigned char  reserved           : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_4_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_lvc_ov_t_START    (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_lvc_ov_t_END      (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_hi_ovp_sel_START  (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_hi_ovp_sel_END    (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_bkvset_START      (2)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbus_bkvset_END        (3)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbat_uv_dc_START       (4)
#define SOC_SCHARGER_DET_TOP_CFG_REG_4_da_vbat_uv_dc_END         (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_5_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x25����ֵ:0x34�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbus_sc_ov_t : 1;  /* bit[0-0]: SC vbus��ѹ����ȥ��ʱ������
                                                          1��256us
                                                          0��4us */
        unsigned char  da_vbus_ov_dc   : 5;  /* bit[1-5]: SCC/LVC �� VBUS��ѹ��ļĴ���ѡ��
                                                          SCC: 8.4 to 11.5 in 100mV
                                                           Ĭ��11V��11010��
                                                          LVC: 4.2 to 5.75 in 50mV
                                                           Ĭ��5.5V��11010�� */
        unsigned char  da_vbus_ov_bk   : 2;  /* bit[6-7]: BUCK mode ��VBUS��ѹ��ļĴ���ѡ��
                                                          00:��ѹ��Ϊ6.5V��
                                                          01:��ѹ��Ϊ10.5V��
                                                          10&amp;11:��ѹ��13.5V�� */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_5_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_sc_ov_t_START  (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_sc_ov_t_END    (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_ov_dc_START    (1)
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_ov_dc_END      (5)
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_ov_bk_START    (6)
#define SOC_SCHARGER_DET_TOP_CFG_REG_5_da_vbus_ov_bk_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DET_TOP_CFG_REG_6_UNION
 �ṹ˵��  : DET_TOP_CFG_REG_6 �Ĵ����ṹ���塣��ַƫ����:0x26����ֵ:0x02�����:8
 �Ĵ���˵��: DET_TOP_���üĴ���_6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vbus_uv_sc : 3;  /* bit[0-2]: SCC/LVCǷѹ��λѡ��
                                                        000: SCC/LVC ���ã�SCCʱΪ7.1V�� LVCʱΪ3V
                                                        ���µ�λֻ��SCC mode��Ч
                                                        001:Ƿѹ��Ϊ7.05V��
                                                        010:Ƿѹ��Ϊ7.0V����default for SCC��
                                                        011:Ƿѹ��Ϊ6.95V��
                                                        100:Ƿѹ��Ϊ6.9V��
                                                        101:Ƿѹ��Ϊ6.85V��
                                                        110:Ƿѹ��Ϊ6.8V��
                                                        111:Ƿѹ��Ϊ6.75V�� */
        unsigned char  da_vbus_uv_bk : 2;  /* bit[3-4]: BUCKģʽǷѹ��λѡ��
                                                        00:Ƿѹ��Ϊ3.8V��
                                                        01:Ƿѹ��Ϊ7.4V��
                                                        10&amp;11:Ƿѹ��9.7V�� */
        unsigned char  reserved      : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_DET_TOP_CFG_REG_6_UNION;
#endif
#define SOC_SCHARGER_DET_TOP_CFG_REG_6_da_vbus_uv_sc_START  (0)
#define SOC_SCHARGER_DET_TOP_CFG_REG_6_da_vbus_uv_sc_END    (2)
#define SOC_SCHARGER_DET_TOP_CFG_REG_6_da_vbus_uv_bk_START  (3)
#define SOC_SCHARGER_DET_TOP_CFG_REG_6_da_vbus_uv_bk_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PSEL_CFG_REG_0_UNION
 �ṹ˵��  : PSEL_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x27����ֵ:0x00�����:8
 �Ĵ���˵��: PSEL_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ldo33_tsmod : 1;  /* bit[0-0]: AVDD OK TestMode ʹ�ܣ�
                                                         0��������AVDD_OK TEST MOD��
                                                         1��������AVDD_OK TEST MOD�� */
        unsigned char  reserved       : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_PSEL_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_PSEL_CFG_REG_0_da_ldo33_tsmod_START  (0)
#define SOC_SCHARGER_PSEL_CFG_REG_0_da_ldo33_tsmod_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_PSEL_RO_REG_1_UNION
 �ṹ˵��  : PSEL_RO_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x28����ֵ:0x00�����:8
 �Ĵ���˵��: PSEL_ֻ���Ĵ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_ldo33_ok : 1;  /* bit[0-0]: psel�����avdd_OK��
                                                      0������psel�ж�avdd_ok=0��
                                                      1������psel�ж�avdd_ok=1�� */
        unsigned char  reserved    : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_PSEL_RO_REG_1_UNION;
#endif
#define SOC_SCHARGER_PSEL_RO_REG_1_ad_ldo33_ok_START  (0)
#define SOC_SCHARGER_PSEL_RO_REG_1_ad_ldo33_ok_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_REF_TOP_CFG_REG_0_UNION
 �ṹ˵��  : REF_TOP_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x29����ֵ:0xB4�����:8
 �Ĵ���˵��: REF_TOP_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ref_ichg_sel     : 2;  /* bit[0-1]: IR��������λ������
                                                              00��+0��
                                                              01��+5%��
                                                              10��+10%��
                                                              11��+15%��
                                                              Ĭ�ϣ�00 */
        unsigned char  da_ref_clk_chop_sel : 2;  /* bit[2-3]: chopperʱ��Ƶ��ѡ��
                                                              00��500KHz��
                                                              01��250KHz��
                                                              10��167KHz��
                                                              11��125KHz��
                                                              Ĭ�ϣ�01 */
        unsigned char  da_ref_chop_en      : 1;  /* bit[4-4]: 0:��ʹ��chopperģʽ
                                                              1:ʹ��chopperģʽ
                                                              Ĭ�ϣ�1 */
        unsigned char  da_otmp_adj         : 2;  /* bit[5-6]: ���¼��У����
                                                              00��+10�ȣ�
                                                              01��+0��
                                                              10��-10�ȣ�
                                                              11��-20�ȡ�
                                                              Ĭ�ϣ�01 */
        unsigned char  da_ibias_switch_en  : 1;  /* bit[7-7]: ����������IBIAS���裬�����л����ܣ��ýӿ���ģ���ڲ����� */
    } reg;
} SOC_SCHARGER_REF_TOP_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_ichg_sel_START      (0)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_ichg_sel_END        (1)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_clk_chop_sel_START  (2)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_clk_chop_sel_END    (3)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_chop_en_START       (4)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ref_chop_en_END         (4)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_otmp_adj_START          (5)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_otmp_adj_END            (6)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ibias_switch_en_START   (7)
#define SOC_SCHARGER_REF_TOP_CFG_REG_0_da_ibias_switch_en_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_REF_TOP_CFG_REG_1_UNION
 �ṹ˵��  : REF_TOP_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x2A����ֵ:0x00�����:8
 �Ĵ���˵��: REF_TOP_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ref_iref_sel : 1;  /* bit[0-0]: ����������IBIAS���裬�ýӿ���ģ���ڲ����� */
        unsigned char  reserved        : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_REF_TOP_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_REF_TOP_CFG_REG_1_da_ref_iref_sel_START  (0)
#define SOC_SCHARGER_REF_TOP_CFG_REG_1_da_ref_iref_sel_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_REF_TOP_CFG_REG_2_UNION
 �ṹ˵��  : REF_TOP_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x2B����ֵ:0x00�����:8
 �Ĵ���˵��: REF_TOP_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ref_reserve : 8;  /* bit[0-7]: Ԥ���Ĵ���
                                                         <0>:0-������Tsense�Զ��л�����,1-����Tsense�Զ��л����ܣ�
                                                         <1>:0-��ʹ��Vrefα����,1-ʹ��Vrefα���أ�
                                                         <3:2>:GAP��������λ������
                                                         00��+0��
                                                         01��+5%��
                                                         10��+10%��
                                                         11��+15%�� */
    } reg;
} SOC_SCHARGER_REF_TOP_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_REF_TOP_CFG_REG_2_da_ref_reserve_START  (0)
#define SOC_SCHARGER_REF_TOP_CFG_REG_2_da_ref_reserve_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_REF_TOP_CFG_REG_3_UNION
 �ṹ˵��  : REF_TOP_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x2C����ֵ:0x1A�����:8
 �Ĵ���˵��: REF_TOP_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_tdie_treg_set  : 2;  /* bit[0-1]: �ȵ�����λѡ��
                                                            00��60�ȣ�
                                                            01��80�ȣ�
                                                            10��100�ȣ�
                                                            11��120�ȡ�
                                                            Ĭ�ϣ�10 */
        unsigned char  da_tdie_alm_set   : 2;  /* bit[2-3]: ����Ԥ����λѡ��
                                                            00��60�ȣ�
                                                            01��80�ȣ�
                                                            10��100�ȣ�
                                                            11��120�ȡ�
                                                            Ĭ�ϣ�10 */
        unsigned char  da_ref_vrc_en     : 1;  /* bit[4-4]: 0:��ʹ�ܲ�ͬ�¶�ϵ����������У��ģʽ
                                                            1:ʹ�ܲ�ͬ�¶�ϵ����������У��ģʽ
                                                            Ĭ�ϣ�1 */
        unsigned char  da_ref_testib_en  : 1;  /* bit[5-5]: 0:��ʹ��ibias����ģʽ
                                                            1:ʹ��ibias����ģʽ
                                                            Ĭ�ϣ�0 */
        unsigned char  da_ref_testbg_en  : 1;  /* bit[6-6]: 0:��ʹ��vbg����ģʽ
                                                            1:ʹ��vbg����ģʽ
                                                            Ĭ�ϣ�0 */
        unsigned char  da_ref_selgate_en : 1;  /* bit[7-7]: 0:��ʹ��vref bufferѡ��avddrc
                                                            1:ʹ��vref bufferѡ��avddrc
                                                            Ĭ�ϣ�0 */
    } reg;
} SOC_SCHARGER_REF_TOP_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_tdie_treg_set_START   (0)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_tdie_treg_set_END     (1)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_tdie_alm_set_START    (2)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_tdie_alm_set_END      (3)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_vrc_en_START      (4)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_vrc_en_END        (4)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_testib_en_START   (5)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_testib_en_END     (5)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_testbg_en_START   (6)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_testbg_en_END     (6)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_selgate_en_START  (7)
#define SOC_SCHARGER_REF_TOP_CFG_REG_3_da_ref_selgate_en_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_0_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x30����ֵ:0xE4�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_ibat_regl_sns_cap : 1;  /* bit[0-0]: ibat regl sns��·�������ݵ��ڼĴ���,
                                                                  Ĭ��0 */
        unsigned char  da_dc_ibat_regl_res     : 2;  /* bit[1-2]: ibat regl��·������ڼĴ���
                                                                  Ĭ��10 */
        unsigned char  da_dc_ibat_regl_hys     : 1;  /* bit[3-3]: ibat regl���͵���Ԥ���Ĵ���
                                                                  Ĭ��0 */
        unsigned char  da_dc_ibat_regl_cap     : 2;  /* bit[4-5]: ibat regl��·���ݵ��ڼĴ���
                                                                  Ĭ��10 */
        unsigned char  da_dc_det_psechop_en    : 1;  /* bit[6-6]: ֱ����αchopʹ���źţ�
                                                                  0����ʹ��
                                                                  1��ʹ�� */
        unsigned char  da_cfly_scp_en          : 1;  /* bit[7-7]: ֱ��sc cfly scp����ʹ���ź�
                                                                  0����ʹ��
                                                                  1��ʹ�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_sns_cap_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_sns_cap_END    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_res_START      (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_res_END        (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_hys_START      (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_hys_END        (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_cap_START      (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_ibat_regl_cap_END        (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_det_psechop_en_START     (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_dc_det_psechop_en_END       (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_cfly_scp_en_START           (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_0_da_cfly_scp_en_END             (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_1_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x31����ֵ:0x12�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_ibus_regl_res      : 2;  /* bit[0-1]: ֱ��ibus regl��·�ȶ��Ե�����ڼĴ��� */
        unsigned char  da_dc_ibus_regl_hys      : 1;  /* bit[2-2]: ibus regl���͵��ڼĴ���
                                                                   Ĭ��0 */
        unsigned char  da_dc_ibus_regl_cap      : 2;  /* bit[3-4]: ibus regl��·���ݵ��ڼĴ���
                                                                   Ĭ��10 */
        unsigned char  da_dc_ibat_sns_ibias_sel : 1;  /* bit[5-5]: ֱ��ibat sns��·ibias���ڼĴ���
                                                                   0��2uA����Ĭ�ϣ�
                                                                   1��4uA�� */
        unsigned char  reserved                 : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_res_START       (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_res_END         (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_hys_START       (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_hys_END         (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_cap_START       (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibus_regl_cap_END         (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibat_sns_ibias_sel_START  (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_1_da_dc_ibat_sns_ibias_sel_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_2_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x32����ֵ:0x29�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_rcpen_delay_adj : 2;  /* bit[0-1]: ֱ���µ�������ʹ��rcpʱ������
                                                                00 0mS;
                                                                01 50mS Ĭ��
                                                                10 100mS
                                                                11 200mS  */
        unsigned char  da_dc_ibus_regl_sel   : 6;  /* bit[2-7]: ֱ��ibus regl��λ���ڼĴ���
                                                                001010��Ӧ1A��110010�����϶�Ӧ5A��step=0.1A��Ĭ��1A */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_2_da_dc_rcpen_delay_adj_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_2_da_dc_rcpen_delay_adj_END    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_2_da_dc_ibus_regl_sel_START    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_2_da_dc_ibus_regl_sel_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_3_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x33����ֵ:0x1E�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_regl_lpf_time : 1;  /* bit[0-0]: regl��·�Ƚ����˲�ʱ����ڼĴ���
                                                              Ĭ��0 */
        unsigned char  da_dc_regl_en       : 4;  /* bit[1-4]: DC regualtor��ʹ���ź�
                                                              0000 �رգ�
                                                              1111 ʹ��
                                                              Ĭ��1111�����ηֱ��Ӧibus/ibat/vout/vbat regl */
        unsigned char  reserved            : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_3_da_dc_regl_lpf_time_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_3_da_dc_regl_lpf_time_END    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_3_da_dc_regl_en_START        (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_3_da_dc_regl_en_END          (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_4_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x34����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_regl_resv : 4;  /* bit[0-3]: regultorԤ���Ĵ��� */
        unsigned char  reserved        : 4;  /* bit[4-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_4_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_4_da_dc_regl_resv_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_4_da_dc_regl_resv_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_5_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x35����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_reserve0 : 8;  /* bit[0-7]: �Ĵ���Ԥ��
                                                         <0>������ֱ�䲻�˲�������0�������Σ�1������
                                                         <2:1>��sc ilim��ֵУ���Ĵ�����00�����䣻01������20%��10������10%��11������20%
                                                         <3>��da_ibus_rcp_nflt_en��0��3A��λ��ʹ�ܲ��˲�RCP������1��3A��λʹ�ܲ��˲�RCP����
                                                         <4>���ӳ�vdrop scc dbt��0�����ӳ���1���ӳ���8u/1000us�ӳ�Ϊ8ms/32ms��
                                                         <5>:ovp_drop��ֵ���ã�0:300mV,1:400mV
                                                         <6>:ovp_drop debunce time��Ĭ��0 1mS,1 20mS
                                                         <7>:s�Ƿ�ʹ��ovp_drop���ܣ�0��ʹ�ܣ�1��ʾʹ�ܣ�Ĭ�ϲ�ʹ�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_5_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_5_da_dc_reserve0_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_5_da_dc_reserve0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_6_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_6 �Ĵ����ṹ���塣��ַƫ����:0x36����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_reserve1 : 8;  /* bit[0-7]: �Ĵ���Ԥ����
                                                         <1:0>:2LSB����SC��һ��MUX��
                                                         00��SC1��mux1 �źű�mux��оƬ��URT_TX�ܽţ�SC1��mux2 �źű�mux��оƬ��URT_RX�ܽ�
                                                         01��SC1��mux1 �źű�mux��оƬ��URT_TX�ܽţ�SC2��mux2 �źű�mux��оƬ��URT_RX�ܽ�
                                                         10��SC2��mux1 �źű�mux��оƬ��URT_TX�ܽţ�SC1��mux2 �źű�mux��оƬ��URT_RX�ܽ�
                                                         11��SC2��mux1 �źű�mux��оƬ��URT_TX�ܽţ�SC2��mux2 �źű�mux��оƬ��URT_RX�ܽ�
                                                         <2>:regultor�ж��Ƿ��ϱ���0���ϱ���1�ϱ�
                                                         <3>:ֱ��ILIM����ģʽѡ��1��1-����128��ILIM���ڹر�SC����������128�κ��ٹر�SC��0-������128��ILIM���ڹر�SC����
                                                         <4>:ֱ��ILIM����ģʽѡ��2��1-����ILIM��һ�μ��ر�SC��0-��ʹ�ܸ�ģʽ
                                                         <7:5>:Ԥ�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_6_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_6_da_dc_reserve1_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_6_da_dc_reserve1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_7_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_7 �Ĵ����ṹ���塣��ַƫ����:0x37����ֵ:0x4A�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_dc_vbat_regl_res   : 2;  /* bit[0-1]: vbat regl��·�����Ĵ���
                                                                Ĭ��10 */
        unsigned char  da_dc_vbat_regl_cap   : 2;  /* bit[2-3]: vbat regl��·�����Ĵ���
                                                                Ĭ��10 */
        unsigned char  da_dc_ucpen_mode      : 1;  /* bit[4-4]: ֱ��ibus ucpʹ�ܷ�ʽѡ��
                                                                0��UCP�ϵ�����20ms��>SC����ʱ�䣩��
                                                                1��������ɺ�100��200��400��800mS delay��ʹ�� */
        unsigned char  da_dc_ucpen_delay_adj : 2;  /* bit[5-6]: ֱ���µ����������������ʱ������ʹ��ucpʱ������
                                                                00 100mS;
                                                                01 200mS
                                                                10 400mS Ĭ��
                                                                11 800mS  */
        unsigned char  da_dc_ss_time         : 1;  /* bit[7-7]: SC/LVC����ʱ�䵵λ��
                                                                0:6.8ms(600KHz CLK)
                                                                1:27.4ms */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_7_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_vbat_regl_res_START    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_vbat_regl_res_END      (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_vbat_regl_cap_START    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_vbat_regl_cap_END      (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ucpen_mode_START       (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ucpen_mode_END         (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ucpen_delay_adj_START  (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ucpen_delay_adj_END    (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ss_time_START          (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_7_da_dc_ss_time_END            (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_8_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_8 �Ĵ����ṹ���塣��ַƫ����:0x38����ֵ:0x4B�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_8
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ibat_ocp_en      : 1;  /* bit[0-0]: ֱ��ibat��������ʹ���ź�
                                                              0����ʹ��
                                                              1��ʹ�� */
        unsigned char  da_ibat_ocp_dbt     : 1;  /* bit[1-1]: ֱ��ibat��������debounce time��
                                                              0��0.1ms
                                                              1��1ms */
        unsigned char  da_dc_vout_regl_res : 2;  /* bit[2-3]: vout regl��·������ڼĴ���
                                                              Ĭ��10 */
        unsigned char  da_dc_vout_regl_hys : 1;  /* bit[4-4]: vout regl��·���͵��ڼĴ���
                                                              Ĭ��0 */
        unsigned char  da_dc_vout_regl_cap : 2;  /* bit[5-6]: vout regl��·���ݵ��ڼĴ���
                                                              Ĭ��10 */
        unsigned char  reserved            : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_8_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_ibat_ocp_en_START       (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_ibat_ocp_en_END         (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_ibat_ocp_dbt_START      (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_ibat_ocp_dbt_END        (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_res_START  (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_res_END    (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_hys_START  (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_hys_END    (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_cap_START  (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_8_da_dc_vout_regl_cap_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_9_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_9 �Ĵ����ṹ���塣��ַƫ����:0x39����ֵ:0x8C�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_9
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ibat_res_sel : 1;  /* bit[0-0]: ֱ��ibat����������ֵ��
                                                          0��2mohm
                                                          1��5mohm */
        unsigned char  da_ibat_ocp_sel : 7;  /* bit[1-7]: ibat��ֵ����������ֵ��ѡ��
                                                          0000000��5A
                                                          0000001��5.1A
                                                          0000010��5.2A

                                                          ��
                                                          1000110:12A
                                                          ��

                                                          1010000��13A
                                                          ���ϵĵ�λ��Ϊ13A */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_9_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_9_da_ibat_res_sel_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_9_da_ibat_res_sel_END    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_9_da_ibat_ocp_sel_START  (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_9_da_ibat_ocp_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_10_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_10 �Ĵ����ṹ���塣��ַƫ����:0x3A����ֵ:0xA0�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_10
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ibus_ocp_dbt     : 1;  /* bit[0-0]: ֱ��ibus��������debounce time��
                                                              0��4us
                                                              1��64us */
        unsigned char  da_ibat_ucp_alm_sel : 5;  /* bit[1-5]: ibat��ֵ����������ֵ��ѡ��
                                                              00000��0.5A
                                                              00001��0.6A
                                                              ��
                                                              10000:2A
                                                              ��
                                                              11110��3.5A
                                                              11111��3.6A */
        unsigned char  da_ibat_ucp_alm_en  : 1;  /* bit[6-6]: ֱ��ibat ucp����ʹ���ź�
                                                              0����ʹ��
                                                              1��ʹ�� */
        unsigned char  da_ibat_ucp_alm_dbt : 1;  /* bit[7-7]: ֱ��ibat��������debounce time��
                                                              0��1ms
                                                              1��96ms */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_10_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibus_ocp_dbt_START      (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibus_ocp_dbt_END        (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_sel_START  (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_sel_END    (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_en_START   (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_en_END     (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_dbt_START  (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_10_da_ibat_ucp_alm_dbt_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_11_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_11 �Ĵ����ṹ���塣��ַƫ����:0x3B����ֵ:0xD0�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_11
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ibus_rcp_sel : 2;  /* bit[0-1]: ֱ��ibus�������������ֵ��ѡ��
                                                          00��65mA
                                                          01��0.5A
                                                          10��1A
                                                          11��3A */
        unsigned char  da_ibus_rcp_en  : 1;  /* bit[2-2]: ֱ��ibus�����������ʹ���ź�
                                                          0����ʹ�ܣ�
                                                          1��ʹ�ܣ� */
        unsigned char  da_ibus_rcp_dbt : 1;  /* bit[3-3]: ֱ��ibus�����������debounce time��
                                                          0��4us
                                                          1��64us */
        unsigned char  da_ibus_ocp_sel : 3;  /* bit[4-6]: ֱ��ibus����������ֵ��ѡ��
                                                          000��2.5A
                                                          001��3A
                                                          010��3.5A
                                                          011��4A
                                                          100��4.5A
                                                          101��5A
                                                          110��5.5A
                                                          111��6A */
        unsigned char  da_ibus_ocp_en  : 1;  /* bit[7-7]: ֱ��ibus��������ʹ���ź�
                                                          0����ʹ��
                                                          1��ʹ�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_11_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_sel_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_sel_END    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_en_START   (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_en_END     (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_dbt_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_rcp_dbt_END    (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_ocp_sel_START  (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_ocp_sel_END    (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_ocp_en_START   (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_11_da_ibus_ocp_en_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_12_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_12 �Ĵ����ṹ���塣��ַƫ����:0x3C����ֵ:0x57�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_12
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_ilim_sc_ocp_en      : 1;  /* bit[0-0]: SCC �¹ܷ�ֵ��������ʹ���ź�
                                                                 0����ʹ��
                                                                 1��ʹ�� */
        unsigned char  da_ilim_bus_sc_ocp_sel : 5;  /* bit[1-5]: SCC ibus��ֵ����������ֵ��ѡ��
                                                                 00000��4.8A
                                                                 00001��5A
                                                                 00010��5.2A
                                                                 00011��5.4A
                                                                 00100��5.6A
                                                                 ��
                                                                 01011:7A
                                                                 ��
                                                                 11110��10.8A
                                                                 11111��11A */
        unsigned char  da_ilim_bus_sc_ocp_en  : 1;  /* bit[6-6]: SCC ibus��ֵ��������ʹ���ź�
                                                                 0����ʹ��
                                                                 1��ʹ�� */
        unsigned char  da_ibus_ucp_en         : 1;  /* bit[7-7]: ֱ��ibusǷ������ʹ���ź�
                                                                 0����ʹ��
                                                                 1��ʹ�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_12_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_sc_ocp_en_START       (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_sc_ocp_en_END         (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_bus_sc_ocp_sel_START  (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_bus_sc_ocp_sel_END    (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_bus_sc_ocp_en_START   (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ilim_bus_sc_ocp_en_END     (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ibus_ucp_en_START          (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_12_da_ibus_ucp_en_END            (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_13_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_13 �Ĵ����ṹ���塣��ַƫ����:0x3D����ֵ:0xD8�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_13
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_drvq1f       : 3;  /* bit[0-2]: Q1 driver �½��ص��ڵ�λ
                                                             2MSB ��reserved
                                                             xx1����xx0 ������λ��7/3 */
        unsigned char  da_ilim_sc_ocp_sel : 5;  /* bit[3-7]: SCC �¹ܷ�ֵ����������ֵ��ѡ��
                                                             00000��4.6A
                                                             00001��5A
                                                             00010��5.2A
                                                             00011��5.4A
                                                             00100��5.6A
                                                             ��
                                                             11011:10A
                                                             ��
                                                             11110��10.8A
                                                             11111��11A */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_13_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_13_da_sc_drvq1f_START        (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_13_da_sc_drvq1f_END          (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_13_da_ilim_sc_ocp_sel_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_13_da_ilim_sc_ocp_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_14_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_14 �Ĵ����ṹ���塣��ַƫ����:0x3E����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_14
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_drvq2f : 3;  /* bit[0-2]: Q2 driver �½��ص��ڵ�λ
                                                       2MSB ��reserved
                                                       xx1����xx0 ������λ��7/3 */
        unsigned char  da_sc_drvq1r : 3;  /* bit[3-5]: Q1 driver �����ص��ڵ�λ
                                                       1MSB ��reserved
                                                       x01��������λΪx00��2��
                                                       x10: ������λΪx00��3��
                                                       x11��������λΪx00��4�� */
        unsigned char  reserved     : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_14_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_14_da_sc_drvq2f_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_14_da_sc_drvq2f_END    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_14_da_sc_drvq1r_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_14_da_sc_drvq1r_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_15_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_15 �Ĵ����ṹ���塣��ַƫ����:0x3F����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_15
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_drvq3f : 3;  /* bit[0-2]: Q3 driver �½��ص��ڵ�λ
                                                       2MSB ��reserved
                                                       xx1����xx0 ������λ��7/3 */
        unsigned char  da_sc_drvq2r : 3;  /* bit[3-5]: Q2 driver �����ص��ڵ�λ
                                                       1MSB ��reserved
                                                       x01��������λΪx00��2��
                                                       x10: ������λΪx00��3��
                                                       x11��������λΪx00��4�� */
        unsigned char  reserved     : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_15_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_15_da_sc_drvq3f_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_15_da_sc_drvq3f_END    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_15_da_sc_drvq2r_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_15_da_sc_drvq2r_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_16_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_16 �Ĵ����ṹ���塣��ַƫ����:0x40����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_16
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_drvq4f : 3;  /* bit[0-2]: Q4 driver �½��ص��ڵ�λ
                                                       1MSB ������Ĵ���
                                                       ��2��bit reserved
                                                       0x1����000 ������λ��7/3
                                                       1xx: �ر�SC1�������û����ţ� */
        unsigned char  da_sc_drvq3r : 3;  /* bit[3-5]: Q3 driver �����ص��ڵ�λ
                                                       1MSB ��reserved
                                                       x01��������λΪx00��2��
                                                       x10: ������λΪx00��3��
                                                       x11��������λΪx00��4�� */
        unsigned char  reserved     : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_16_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_16_da_sc_drvq4f_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_16_da_sc_drvq4f_END    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_16_da_sc_drvq3r_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_16_da_sc_drvq3r_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_17_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_17 �Ĵ����ṹ���塣��ַƫ����:0x41����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_17
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_dtgbga_dly : 2;  /* bit[0-1]: SC ����ʱ�䵵λ��SC ����Ĵ����������û����� */
        unsigned char  da_sc_dt_grev    : 2;  /* bit[2-3]: SC ����ʱ�䱣���Ĵ�����SC ����Ĵ����������û����� */
        unsigned char  da_sc_drvq4r     : 3;  /* bit[4-6]: Q4 driver �����ص��ڵ�λ
                                                           1MSB ������Ĵ���
                                                           x01��������λΪx00��2��
                                                           x10: ������λΪx00��3��
                                                           x11��������λΪx00��4��
                                                           1xx���ر�SC2�������û����ţ� */
        unsigned char  reserved         : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_17_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_dtgbga_dly_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_dtgbga_dly_END    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_dt_grev_START     (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_dt_grev_END       (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_drvq4r_START      (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_17_da_sc_drvq4r_END        (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_18_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_18 �Ĵ����ṹ���塣��ַƫ����:0x42����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_18
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_ts_mod   : 1;  /* bit[0-0]: SC ����ģʽʹ��
                                                         0���رղ���ģʽ��
                                                         1��ʹ�ܲ���ģʽ�� */
        unsigned char  da_sc_ini_dt   : 1;  /* bit[1-1]: Ԥ���Ĵ��� */
        unsigned char  da_sc_gbdt_adj : 1;  /* bit[2-2]: SC ����ʱ�䱣���Ĵ�����SC ����Ĵ����������û����� */
        unsigned char  da_sc_gadt_adj : 1;  /* bit[3-3]: SC ����ʱ�䱣���Ĵ�����SC ����Ĵ����������û����� */
        unsigned char  da_sc_freq_sh  : 1;  /* bit[4-4]: SC ��600KHz������Ƶ
                                                         0������Ƶ
                                                         1: ��Ƶ */
        unsigned char  da_sc_freq     : 3;  /* bit[5-7]: SC ����Ƶ��
                                                         000��600KHz
                                                         001��750KHz
                                                         010: 600KHz
                                                         011: 500KHz
                                                         100: 400KHz
                                                         101��300KHz
                                                         110: 1MHz
                                                         111��1.2MHz */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_18_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_ts_mod_START    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_ts_mod_END      (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_ini_dt_START    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_ini_dt_END      (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_gbdt_adj_START  (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_gbdt_adj_END    (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_gadt_adj_START  (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_gadt_adj_END    (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_freq_sh_START   (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_freq_sh_END     (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_freq_START      (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_18_da_sc_freq_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_19_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_19 �Ĵ����ṹ���塣��ַƫ����:0x43����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_19
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_sc_ts_patha   : 1;  /* bit[0-0]: SC ����ģʽ��ǿ�ƴ�Q1��Q4
                                                           0��Q1��Q4�رգ�
                                                           1��Q1��Q4�򿪣� */
        unsigned char  da_sc_ts_muxselb : 3;  /* bit[1-3]: SC ����ģʽ����MUX��mux2 ѡ��
                                                           000�� DGND
                                                           001��CLK_PWMQ2
                                                           010: CLK_PWMQ3
                                                           011: CLK_GA_ON
                                                           100: CLK_GA_OFF
                                                           101: CLK_PATHA
                                                           110: CLK_BUF
                                                           111: SC_LVCSPLY_RDY_BUF */
        unsigned char  da_sc_ts_muxsela : 3;  /* bit[4-6]: SC ����ģʽ����MUX��mux1 ѡ��
                                                           000�� DGND
                                                           001��CLK_PWMQ1
                                                           010: CLK_PWMQ4
                                                           011: CLK_GB_ON
                                                           100: CLK_GB_OFF
                                                           101: CLK_PATHB
                                                           110: CLK_600KMUXBUF
                                                           111: SC_RDY_BUF */
        unsigned char  da_sc_ts_mux     : 1;  /* bit[7-7]: SC ����ģʽmuxʹ��
                                                           0���رղ���ģʽmux��
                                                           1��ʹ�ܲ���ģʽmux�� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_19_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_patha_START    (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_patha_END      (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_muxselb_START  (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_muxselb_END    (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_muxsela_START  (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_muxsela_END    (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_mux_START      (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_19_da_sc_ts_mux_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_20_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_20 �Ĵ����ṹ���塣��ַƫ����:0x44����ֵ:0x44�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_20
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vdrop_lvc_ovp_nflt_sel : 2;  /* bit[0-1]: LVC Vdrop ��ѹ���˲�������ֵ��ѡ��
                                                                    00��400mV
                                                                    01��500mV
                                                                    10��600mV
                                                                    11��800mV */
        unsigned char  da_vdrop_lvc_ovp_en       : 1;  /* bit[2-2]: LVC Vdrop��ѹ����ʹ���ź�
                                                                    0����ʹ��
                                                                    1��ʹ�� */
        unsigned char  da_vdrop_lvc_ovp_dbt      : 1;  /* bit[3-3]: LVC Vdrop��ѹ����debounce time��
                                                                    0��4us
                                                                    1��64us */
        unsigned char  da_vdrop_lvc_min_nflt_sel : 2;  /* bit[4-5]: LVC Vdrop min���˲�������ֵ��ѡ��
                                                                    00��-400mV
                                                                    01��-500mV
                                                                    10��-600mV
                                                                    11��-800mV */
        unsigned char  da_vdrop_lvc_min_en       : 1;  /* bit[6-6]: LVC Vdrop min����ʹ���ź�
                                                                    0����ʹ�ܣ�
                                                                    1��ʹ�ܣ� */
        unsigned char  da_sc_ts_pathb            : 1;  /* bit[7-7]: SC ����ģʽ��ǿ�ƴ�Q2��Q3
                                                                    0��Q2��Q3�رգ�
                                                                    1��Q2��Q3�򿪣� */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_20_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_nflt_sel_START  (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_nflt_sel_END    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_en_START        (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_en_END          (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_dbt_START       (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_ovp_dbt_END         (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_min_nflt_sel_START  (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_min_nflt_sel_END    (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_min_en_START        (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_vdrop_lvc_min_en_END          (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_sc_ts_pathb_START             (7)
#define SOC_SCHARGER_DC_TOP_CFG_REG_20_da_sc_ts_pathb_END               (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_21_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_21 �Ĵ����ṹ���塣��ַƫ����:0x45����ֵ:0x51�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_21
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vin2vout_sc_en    : 1;  /* bit[0-0]: SCC Vdrop��ѹ����ʹ���ź�
                                                               0����ʹ��
                                                               1��ʹ�� */
        unsigned char  da_vin2vout_sc_dbt   : 1;  /* bit[1-1]: SCC Vin-2Vout��ѹ����debounce time��
                                                               0��4us
                                                               1��256us */
        unsigned char  da_vdrop_lvc_ovp_sel : 5;  /* bit[2-6]: LVC Vdrop��ѹ������ֵ��ѡ��1step=20mV
                                                               00000��0V
                                                               00001��20mV
                                                               00010��40mV
                                                               00011��60mV
                                                               ��
                                                               10100��400mV
                                                               ��
                                                               11110-11111:600mV */
        unsigned char  reserved             : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_21_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vin2vout_sc_en_START     (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vin2vout_sc_en_END       (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vin2vout_sc_dbt_START    (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vin2vout_sc_dbt_END      (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vdrop_lvc_ovp_sel_START  (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_21_da_vdrop_lvc_ovp_sel_END    (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_CFG_REG_22_UNION
 �ṹ˵��  : DC_TOP_CFG_REG_22 �Ĵ����ṹ���塣��ַƫ����:0x46����ֵ:0x11�����:8
 �Ĵ���˵��: DC_TOP_���üĴ���_22
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_vin2vout_sc_min_sel      : 2;  /* bit[0-1]: LVC��SCC VDROP min������ֵ��ѡ��
                                                                      00��80m
                                                                      01��40mV
                                                                      10��0mV
                                                                      11��-40mV */
        unsigned char  da_vin2vout_sc_min_nflt_sel : 2;  /* bit[2-3]: SCC Vin-2Vout��ѹ���˲�����ֵ��ѡ��
                                                                      00��-400mV
                                                                      01��-500mV
                                                                      10��-600mV
                                                                      11��-800mV */
        unsigned char  da_vin2vout_sc_max_sel      : 2;  /* bit[4-5]: SCC Vin-2Vout��ѹ����ֵ��ѡ��
                                                                      00��300mV
                                                                      01��400mV
                                                                      10��500mV
                                                                      11��600mV */
        unsigned char  da_vin2vout_sc_max_nflt_sel : 2;  /* bit[6-7]: SCC Vin-2Vout��ѹ���˲�����ֵ��ѡ��
                                                                      00��400mV
                                                                      01��500mV
                                                                      10��600mV
                                                                      11��800mV */
    } reg;
} SOC_SCHARGER_DC_TOP_CFG_REG_22_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_min_sel_START       (0)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_min_sel_END         (1)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_min_nflt_sel_START  (2)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_min_nflt_sel_END    (3)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_max_sel_START       (4)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_max_sel_END         (5)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_max_nflt_sel_START  (6)
#define SOC_SCHARGER_DC_TOP_CFG_REG_22_da_vin2vout_sc_max_nflt_sel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_DC_TOP_RO_REG_23_UNION
 �ṹ˵��  : DC_TOP_RO_REG_23 �Ĵ����ṹ���塣��ַƫ����:0x47����ֵ:0x00�����:8
 �Ĵ���˵��: DC_TOP_ֻ���Ĵ���_23
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_scc_cflyp_scp : 1;  /* bit[0-0]: SC cflyp��·�����ź�
                                                           0��û�з���SC cflyp��·��
                                                           1������SC cflyp��·�� */
        unsigned char  ad_scc_cflyn_scp : 1;  /* bit[1-1]: SC cflyn��·�����ź�
                                                           0��û�з���SC cflyn��·��
                                                           1������SC cflyn��·�� */
        unsigned char  ad_sc_rdy        : 1;  /* bit[2-2]: SC1 ��������� */
        unsigned char  reserved         : 5;  /* bit[3-7]: reserved */
    } reg;
} SOC_SCHARGER_DC_TOP_RO_REG_23_UNION;
#endif
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_scc_cflyp_scp_START  (0)
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_scc_cflyp_scp_END    (0)
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_scc_cflyn_scp_START  (1)
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_scc_cflyn_scp_END    (1)
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_sc_rdy_START         (2)
#define SOC_SCHARGER_DC_TOP_RO_REG_23_ad_sc_rdy_END           (2)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION0_RO_REG_0_UNION
 �ṹ˵��  : VERSION0_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x49����ֵ:0xF6�����:8
 �Ĵ���˵��: VERSION0_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id0 : 8;  /* bit[0-7]: �汾�Ĵ�����
                                                   11110110&#45;&#45;&#45;&#45;&#45;-SchargerV600 */
    } reg;
} SOC_SCHARGER_VERSION0_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION0_RO_REG_0_chip_id0_START  (0)
#define SOC_SCHARGER_VERSION0_RO_REG_0_chip_id0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION1_RO_REG_0_UNION
 �ṹ˵��  : VERSION1_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x4A����ֵ:0xF0�����:8
 �Ĵ���˵��: VERSION1_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id1 : 8;  /* bit[0-7]: �汾�Ĵ�����
                                                   11110000&#45;&#45;&#45;&#45;&#45;-SchargerV600 */
    } reg;
} SOC_SCHARGER_VERSION1_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION1_RO_REG_0_chip_id1_START  (0)
#define SOC_SCHARGER_VERSION1_RO_REG_0_chip_id1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION2_RO_REG_0_UNION
 �ṹ˵��  : VERSION2_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x4B����ֵ:0x36�����:8
 �Ĵ���˵��: VERSION2_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id2 : 8;  /* bit[0-7]: оƬID�Ĵ�����
                                                   00110110&#45;&#45;&#45;&#45;&#45;-6 */
    } reg;
} SOC_SCHARGER_VERSION2_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION2_RO_REG_0_chip_id2_START  (0)
#define SOC_SCHARGER_VERSION2_RO_REG_0_chip_id2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION3_RO_REG_0_UNION
 �ṹ˵��  : VERSION3_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x4C����ֵ:0x35�����:8
 �Ĵ���˵��: VERSION3_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id3 : 8;  /* bit[0-7]: оƬID�Ĵ�����
                                                   00110101&#45;&#45;&#45;&#45;&#45;-5 */
    } reg;
} SOC_SCHARGER_VERSION3_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION3_RO_REG_0_chip_id3_START  (0)
#define SOC_SCHARGER_VERSION3_RO_REG_0_chip_id3_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION4_RO_REG_0_UNION
 �ṹ˵��  : VERSION4_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x4D����ֵ:0x32�����:8
 �Ĵ���˵��: VERSION4_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id4 : 8;  /* bit[0-7]: оƬID�Ĵ�����
                                                   00110010&#45;&#45;&#45;&#45;&#45;-2 */
    } reg;
} SOC_SCHARGER_VERSION4_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION4_RO_REG_0_chip_id4_START  (0)
#define SOC_SCHARGER_VERSION4_RO_REG_0_chip_id4_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_VERSION5_RO_REG_0_UNION
 �ṹ˵��  : VERSION5_RO_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x4E����ֵ:0x36�����:8
 �Ĵ���˵��: VERSION5_ֻ���Ĵ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  chip_id5 : 8;  /* bit[0-7]: оƬID�Ĵ�����
                                                   00110110&#45;&#45;&#45;&#45;&#45;-6 */
    } reg;
} SOC_SCHARGER_VERSION5_RO_REG_0_UNION;
#endif
#define SOC_SCHARGER_VERSION5_RO_REG_0_chip_id5_START  (0)
#define SOC_SCHARGER_VERSION5_RO_REG_0_chip_id5_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_0_UNION
 �ṹ˵��  : BUCK_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x50����ֵ:0x2B�����:8
 �Ĵ���˵��: BUCK_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_12v_maxduty_en  : 1;  /* bit[0-0]: 12V����ʱ���ռ�ձ��Ƿ�ʹ�� */
        unsigned char  da_buck_12v_maxduty_adj : 2;  /* bit[1-2]: 12V����ʱ���ռ�ձȵ��� */
        unsigned char  da_buck_ilimit          : 5;  /* bit[3-7]: buckƽ���������趨����
                                                                  00000��85mA
                                                                  00001��130mA
                                                                  00010��200mA
                                                                  00011��300mA
                                                                  00100��400mA
                                                                  00101��475mA
                                                                  00110��600mA
                                                                  00111��700mA
                                                                  01000��800mA
                                                                  01001��825mA
                                                                  01010��1.0A
                                                                  01011��1.1A
                                                                  01100��1.2A
                                                                  01101��1.3A
                                                                  01110��1.4A
                                                                  01111��1.5A
                                                                  10000��1.6A
                                                                  10001��1.7A
                                                                  10010��1.8A
                                                                  10011��1.9A
                                                                  10100��2.0A
                                                                  10101��2.1A
                                                                  10110��2.2A
                                                                  10111��2.3A
                                                                  11000��2.4A
                                                                  11001��2.5A
                                                                  11010��2.6A
                                                                  11011��2.7A
                                                                  11100��2.8A
                                                                  11101��2.9A
                                                                  11110��3.0A
                                                                  11111��3.1A */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_12v_maxduty_en_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_12v_maxduty_en_END     (0)
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_12v_maxduty_adj_START  (1)
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_12v_maxduty_adj_END    (2)
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_ilimit_START           (3)
#define SOC_SCHARGER_BUCK_CFG_REG_0_da_buck_ilimit_END             (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_1_UNION
 �ṹ˵��  : BUCK_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x51����ֵ:0x03�����:8
 �Ĵ���˵��: BUCK_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_9v_maxduty_en  : 1;  /* bit[0-0]: 9V����ʱ���ռ�ձ��Ƿ�ʹ�� */
        unsigned char  da_buck_9v_maxduty_adj : 2;  /* bit[1-2]: 9V����ʱ���ռ�ձȵ��� */
        unsigned char  reserved               : 5;  /* bit[3-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_1_da_buck_9v_maxduty_en_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_1_da_buck_9v_maxduty_en_END     (0)
#define SOC_SCHARGER_BUCK_CFG_REG_1_da_buck_9v_maxduty_adj_START  (1)
#define SOC_SCHARGER_BUCK_CFG_REG_1_da_buck_9v_maxduty_adj_END    (2)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_2_UNION
 �ṹ˵��  : BUCK_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x52����ֵ:0x80�����:8
 �Ĵ���˵��: BUCK_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_antibst : 8;  /* bit[0-7]: <7> anti-reverbstģ��ʹ��
                                                          0���ر� 1��ʹ��
                                                          <6> anti-reverbstȥ��ʱ��ѡ��
                                                          0��32ms 1��8ms
                                                          <5:4> anti-reverbst��������
                                                          00:25mv
                                                          01:50mv
                                                          10:75mv
                                                          11:100mv */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_2_da_buck_antibst_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_2_da_buck_antibst_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_3_UNION
 �ṹ˵��  : BUCK_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x53����ֵ:0x19�����:8
 �Ĵ���˵��: BUCK_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_cap2_ea    : 2;  /* bit[0-1]: ���Ͳ������粹������C2��
                                                             00��1.2pF
                                                             01: 2.4pF
                                                             10��3.6pF
                                                             11��4.8pF */
        unsigned char  da_buck_cap1_ea    : 3;  /* bit[2-4]: ���Ͳ������粹������C1��
                                                             000��10pF
                                                             001��20pF
                                                             010��30pF
                                                             011��40pF
                                                             100��50pF
                                                             101��60pF
                                                             110��70pF
                                                             111��80pF */
        unsigned char  da_buck_block_ctrl : 3;  /* bit[5-7]: �������ܿ��� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_cap2_ea_START     (0)
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_cap2_ea_END       (1)
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_cap1_ea_START     (2)
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_cap1_ea_END       (4)
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_block_ctrl_START  (5)
#define SOC_SCHARGER_BUCK_CFG_REG_3_da_buck_block_ctrl_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_4_UNION
 �ṹ˵��  : BUCK_CFG_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x54����ֵ:0x55�����:8
 �Ĵ���˵��: BUCK_���üĴ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_cap7_acl : 2;  /* bit[0-1]: acl��·�������ݣ�
                                                           00��3pF
                                                           01: 6pF
                                                           10��9pF
                                                           11��12pF */
        unsigned char  da_buck_cap6_dpm : 2;  /* bit[2-3]: dpm��·�������ݣ�
                                                           00��1.2pF
                                                           01: 2.4pF
                                                           10��3.6pF
                                                           11��4.8pF */
        unsigned char  da_buck_cap5_cc  : 2;  /* bit[4-5]: cc��·�������ݣ�
                                                           00��3pF
                                                           01: 6pF
                                                           10��9pF
                                                           11��12pF */
        unsigned char  da_buck_cap3_ea  : 2;  /* bit[6-7]: ���Ͳ������粹������C3��
                                                           00��50fF
                                                           01: 100fF
                                                           10��150fF
                                                           11��200fF */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_4_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap7_acl_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap7_acl_END    (1)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap6_dpm_START  (2)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap6_dpm_END    (3)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap5_cc_START   (4)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap5_cc_END     (5)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap3_ea_START   (6)
#define SOC_SCHARGER_BUCK_CFG_REG_4_da_buck_cap3_ea_END     (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_5_UNION
 �ṹ˵��  : BUCK_CFG_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x55����ֵ:0x25�����:8
 �Ĵ���˵��: BUCK_���üĴ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_dmd_ibias     : 2;  /* bit[0-1]: buck_dmdƫ�õ������� */
        unsigned char  da_buck_dmd_en        : 1;  /* bit[2-2]: ѡ���Ƿ���DMD����0���ر� 1������ */
        unsigned char  da_buck_dmd_delay     : 1;  /* bit[3-3]: dmd���NG��ʱ��1������15nS */
        unsigned char  da_buck_dmd_clamp     : 1;  /* bit[4-4]: dmd�Ƚ������Ƕλʹ�� 0��Ƕλ 1����Ƕλ */
        unsigned char  da_buck_cmp_ibias_adj : 1;  /* bit[5-5]: Ԥ�� */
        unsigned char  reserved              : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_5_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_ibias_START      (0)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_ibias_END        (1)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_en_START         (2)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_en_END           (2)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_delay_START      (3)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_delay_END        (3)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_clamp_START      (4)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_dmd_clamp_END        (4)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_cmp_ibias_adj_START  (5)
#define SOC_SCHARGER_BUCK_CFG_REG_5_da_buck_cmp_ibias_adj_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_6_UNION
 �ṹ˵��  : BUCK_CFG_REG_6 �Ĵ����ṹ���塣��ַƫ����:0x56����ֵ:0x0E�����:8
 �Ĵ���˵��: BUCK_���üĴ���_6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_dpm_auto : 1;  /* bit[0-0]: buck DPM���Զ����ڹ��ܿ���
                                                           0���ر�
                                                           1������ */
        unsigned char  da_buck_dmd_sel  : 4;  /* bit[1-4]: DMD��ѡ��
                                                           0000��-250mA
                                                           0001��-200mA
                                                           ����
                                                           0111��120mA
                                                           1000��200mA
                                                           ����
                                                           1111��700mA */
        unsigned char  reserved         : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_6_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_6_da_buck_dpm_auto_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_6_da_buck_dpm_auto_END    (0)
#define SOC_SCHARGER_BUCK_CFG_REG_6_da_buck_dmd_sel_START   (1)
#define SOC_SCHARGER_BUCK_CFG_REG_6_da_buck_dmd_sel_END     (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_7_UNION
 �ṹ˵��  : BUCK_CFG_REG_7 �Ĵ����ṹ���塣��ַƫ����:0x57����ֵ:0x60�����:8
 �Ĵ���˵��: BUCK_���üĴ���_7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_dt_lshs_delay : 2;  /* bit[0-1]: �¹ܵ��Ϲ����������ӳ�5ns
                                                                ��λ��1 ���������Ƽ�5nS
                                                                ��λ��1 ���������Ƽ�5nS */
        unsigned char  da_buck_dt_lshs       : 1;  /* bit[2-2]: �¹ܵ��Ϲܵ�����������ʽѡ��
                                                                0��������ʱ��
                                                                1��������ʱ��(PWM) */
        unsigned char  da_buck_dt_hsls       : 1;  /* bit[3-3]: �Ϲܵ��¹����������ӳ�5nS 0������5nS 1����5nS */
        unsigned char  da_buck_dpm_sel       : 4;  /* bit[4-7]: DPM�����ѹ�趨
                                                                0000: 84%
                                                                0001: 85%
                                                                0010: 86%
                                                                0011: 87%
                                                                0100: 88%
                                                                0101: 89%
                                                                0110: 90%
                                                                0111: 91%
                                                                1000: 92%
                                                                1001: 93%
                                                                1010: 94%
                                                                1011: 95%
                                                                1100~1111:96% */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_7_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_lshs_delay_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_lshs_delay_END    (1)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_lshs_START        (2)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_lshs_END          (2)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_hsls_START        (3)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dt_hsls_END          (3)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dpm_sel_START        (4)
#define SOC_SCHARGER_BUCK_CFG_REG_7_da_buck_dpm_sel_END          (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_8_UNION
 �ṹ˵��  : BUCK_CFG_REG_8 �Ĵ����ṹ���塣��ַƫ����:0x58����ֵ:0x05�����:8
 �Ĵ���˵��: BUCK_���üĴ���_8
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_fullduty_en        : 1;  /* bit[0-0]: buck����ֱͨʱ��������1Ϊ��������ǿ��offһ�Σ�0Ϊ������ */
        unsigned char  da_buck_fullduty_delaytime : 4;  /* bit[1-4]: ����ʱ��û�п��ض�����Ϊֱͨ 0001Ϊ4uS 0010Ϊ8uS 0100Ϊ16uS 1000Ϊ32uS  */
        unsigned char  da_buck_dt_sel             : 2;  /* bit[5-6]: ����ʱ������5nS��Ĭ��00��
                                                                     <0>�ͱ�
                                                                     <1>�߱� */
        unsigned char  reserved                   : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_8_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_fullduty_en_START         (0)
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_fullduty_en_END           (0)
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_fullduty_delaytime_START  (1)
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_fullduty_delaytime_END    (4)
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_dt_sel_START              (5)
#define SOC_SCHARGER_BUCK_CFG_REG_8_da_buck_dt_sel_END                (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_9_UNION
 �ṹ˵��  : BUCK_CFG_REG_9 �Ĵ����ṹ���塣��ַƫ����:0x59����ֵ:0x10�����:8
 �Ĵ���˵��: BUCK_���üĴ���_9
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_gap_auto         : 1;  /* bit[0-0]: buck gapֵ�Զ����ڹ��ܿ���
                                                                   0���ر�
                                                                   1������ */
        unsigned char  da_buck_gap              : 3;  /* bit[1-3]: buck gap��ѹ�޵��Ĵ�������:
                                                                   000: 0
                                                                   001: -30mV
                                                                   010:-60mV
                                                                   011: -90mV
                                                                   100: 0mV
                                                                   101: +30mV
                                                                   110: +60mV
                                                                   111: +90mV */
        unsigned char  da_buck_fullduty_offtime : 2;  /* bit[4-5]: ֱͨ��ǿ�ƹر��Ϲ�ʱ����ڼĴ��� 00Ϊ20nS 01Ϊ30nS 01Ϊ40nS 11Ϊ50n */
        unsigned char  reserved                 : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_9_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_gap_auto_START          (0)
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_gap_auto_END            (0)
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_gap_START               (1)
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_gap_END                 (3)
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_fullduty_offtime_START  (4)
#define SOC_SCHARGER_BUCK_CFG_REG_9_da_buck_fullduty_offtime_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_10_UNION
 �ṹ˵��  : BUCK_CFG_REG_10 �Ĵ����ṹ���塣��ַƫ����:0x5A����ֵ:0x88�����:8
 �Ĵ���˵��: BUCK_���üĴ���_10
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_hmosocp_lpf : 2;  /* bit[0-1]: buck�Ϲ�ocp���ʱ�˶�ʱ�����
                                                              <1>�ȴ�PWM_H�ź�
                                                              <0>�ȴ�HG_ON�ź� */
        unsigned char  da_buck_hmos_rise   : 3;  /* bit[2-4]: �Ϲ���������������ѡ�� */
        unsigned char  da_buck_hmos_fall   : 3;  /* bit[5-7]: �Ϲ��½�����������ѡ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_10_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmosocp_lpf_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmosocp_lpf_END    (1)
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmos_rise_START    (2)
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmos_rise_END      (4)
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmos_fall_START    (5)
#define SOC_SCHARGER_BUCK_CFG_REG_10_da_buck_hmos_fall_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_11_UNION
 �ṹ˵��  : BUCK_CFG_REG_11 �Ĵ����ṹ���塣��ַƫ����:0x5B����ֵ:0xB4�����:8
 �Ĵ���˵��: BUCK_���üĴ���_11
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_lmos_rise   : 3;  /* bit[0-2]: �¹���������������ѡ�� */
        unsigned char  da_buck_lmos_on_sel : 1;  /* bit[3-3]: �¹ܳ�ʱ�䲻�������£�ǿ�ƿ�һ���¹ܵĶ�����ʱ�������Ĭ��Ϊ0
                                                              0:128uS��һ��;
                                                              1:64uS��һ�� */
        unsigned char  da_buck_lmos_on_en  : 1;  /* bit[4-4]: �¹ܳ�ʱ�䲻�������£��Ƿ�ʹ��ǿ�ƿ�һ���¹ܵĶ�����Ĭ��ֵΪ1
                                                              0���ر�
                                                              1�������˹��� */
        unsigned char  da_buck_lmos_fall   : 3;  /* bit[5-7]: �¹��½�����������ѡ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_11_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_rise_START    (0)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_rise_END      (2)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_on_sel_START  (3)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_on_sel_END    (3)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_on_en_START   (4)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_on_en_END     (4)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_fall_START    (5)
#define SOC_SCHARGER_BUCK_CFG_REG_11_da_buck_lmos_fall_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_12_UNION
 �ṹ˵��  : BUCK_CFG_REG_12 �Ĵ����ṹ���塣��ַƫ����:0x5C����ֵ:0x52�����:8
 �Ĵ���˵��: BUCK_���üĴ���_12
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_min_ontime_sel  : 1;  /* bit[0-0]: ��С��ͨʱ������ */
        unsigned char  da_buck_min_ontime      : 2;  /* bit[1-2]: ��С��ͨʱ��ѡ�� */
        unsigned char  da_buck_min_offtime_sel : 1;  /* bit[3-3]: ��С�ض�ʱ������ */
        unsigned char  da_buck_min_offtime     : 2;  /* bit[4-5]: ��С�ض�ʱ��ѡ�� */
        unsigned char  da_buck_lmosocp_lpf     : 2;  /* bit[6-7]: Ԥ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_12_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_ontime_sel_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_ontime_sel_END     (0)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_ontime_START       (1)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_ontime_END         (2)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_offtime_sel_START  (3)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_offtime_sel_END    (3)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_offtime_START      (4)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_min_offtime_END        (5)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_lmosocp_lpf_START      (6)
#define SOC_SCHARGER_BUCK_CFG_REG_12_da_buck_lmosocp_lpf_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_13_UNION
 �ṹ˵��  : BUCK_CFG_REG_13 �Ĵ����ṹ���塣��ַƫ����:0x5D����ֵ:0x01�����:8
 �Ĵ���˵��: BUCK_���üĴ���_13
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_ocp_delay  : 2;  /* bit[0-1]: Ԥ�� */
        unsigned char  da_buck_ocp_cap    : 1;  /* bit[2-2]: buck ocp ����:
                                                             1������
                                                             0������ */
        unsigned char  da_buck_ocp_9_enb  : 1;  /* bit[3-3]: �ر�OCP����
                                                             0�����ر�
                                                             1���ر� */
        unsigned char  da_buck_ocp_300ma  : 2;  /* bit[4-5]: Ԥ�� */
        unsigned char  da_buck_ocp_12vsel : 2;  /* bit[6-7]: Ԥ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_13_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_delay_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_delay_END     (1)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_cap_START     (2)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_cap_END       (2)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_9_enb_START   (3)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_9_enb_END     (3)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_300ma_START   (4)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_300ma_END     (5)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_12vsel_START  (6)
#define SOC_SCHARGER_BUCK_CFG_REG_13_da_buck_ocp_12vsel_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_14_UNION
 �ṹ˵��  : BUCK_CFG_REG_14 �Ĵ����ṹ���塣��ַƫ����:0x5E����ֵ:0x15�����:8
 �Ĵ���˵��: BUCK_���üĴ���_14
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_offtime_judge : 2;  /* bit[0-1]: Ԥ�� */
        unsigned char  da_buck_ocp_vally     : 2;  /* bit[2-3]: buck �¹ܹ�ֵocp�����ڼĴ���
                                                                00��2.1A
                                                                01��2.5A
                                                                10��2.9A
                                                                11��3.3A */
        unsigned char  da_buck_ocp_peak      : 2;  /* bit[4-5]: Ԥ�� */
        unsigned char  da_buck_ocp_mode_sel  : 1;  /* bit[6-6]: Ԥ�� */
        unsigned char  reserved              : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_14_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_offtime_judge_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_offtime_judge_END    (1)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_vally_START      (2)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_vally_END        (3)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_peak_START       (4)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_peak_END         (5)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_mode_sel_START   (6)
#define SOC_SCHARGER_BUCK_CFG_REG_14_da_buck_ocp_mode_sel_END     (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_15_UNION
 �ṹ˵��  : BUCK_CFG_REG_15 �Ĵ����ṹ���塣��ַƫ����:0x5F����ֵ:0x48�����:8
 �Ĵ���˵��: BUCK_���üĴ���_15
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_osc_frq             : 4;  /* bit[0-3]: osc����Ƶ�ʣ����ο���Ƶ�ʻ���trim��ֵӰ�죩��
                                                                      0000��0.9M
                                                                      0001��0.95M
                                                                      ����
                                                                      1000��1.4M
                                                                      1001��1.5M
                                                                      ����
                                                                      1111��1.9M */
        unsigned char  da_buck_offtime_judge_en    : 1;  /* bit[4-4]: CC��·����4M�˲����� */
        unsigned char  da_buck_offtime_judge_delay : 3;  /* bit[5-7]: Ԥ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_15_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_osc_frq_START              (0)
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_osc_frq_END                (3)
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_offtime_judge_en_START     (4)
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_offtime_judge_en_END       (4)
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_offtime_judge_delay_START  (5)
#define SOC_SCHARGER_BUCK_CFG_REG_15_da_buck_offtime_judge_delay_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_16_UNION
 �ṹ˵��  : BUCK_CFG_REG_16 �Ĵ����ṹ���塣��ַƫ����:0x60����ֵ:0x3F�����:8
 �Ĵ���˵��: BUCK_���üĴ���_16
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_pre_op : 8;  /* bit[0-7]:

                                                         <5:0>buck 6����·ʹ�ܿ���λ��
                                                          THEM.CC.CV.SYS.DPM.ACL
                                                          000000����·ȫ���ر�
                                                          111111����·ȫ������ */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_16_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_16_da_buck_pre_op_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_16_da_buck_pre_op_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_17_UNION
 �ṹ˵��  : BUCK_CFG_REG_17 �Ĵ����ṹ���塣��ַƫ����:0x61����ֵ:0x54�����:8
 �Ĵ���˵��: BUCK_���üĴ���_17
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_res1_ea   : 3;  /* bit[0-2]: ���Ͳ������粹������R1��
                                                            000��400K
                                                            001: 650K
                                                            010��900K
                                                            011��1100K
                                                            100��1300K
                                                            101: 1800K
                                                            110��2300K
                                                            111: 3000K */
        unsigned char  da_buck_rdy_en    : 1;  /* bit[3-3]: buck ��״̬�ϱ�����
                                                            0���ر�
                                                            1������ */
        unsigned char  da_buck_rc_thm    : 2;  /* bit[4-5]: thm�˲�rc��
                                                            00�����˲�
                                                            01: 250k 1.2p
                                                            10��500k 2.4p
                                                            11��750k 3.6p */
        unsigned char  da_buck_q1ocp_adj : 2;  /* bit[6-7]: Ԥ�� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_17_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_res1_ea_START    (0)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_res1_ea_END      (2)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_rdy_en_START     (3)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_rdy_en_END       (3)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_rc_thm_START     (4)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_rc_thm_END       (5)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_q1ocp_adj_START  (6)
#define SOC_SCHARGER_BUCK_CFG_REG_17_da_buck_q1ocp_adj_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_18_UNION
 �ṹ˵��  : BUCK_CFG_REG_18 �Ĵ����ṹ���塣��ַƫ����:0x62����ֵ:0x55�����:8
 �Ĵ���˵��: BUCK_���üĴ���_18
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_res13_dpm_pz  : 2;  /* bit[0-1]: dpm��·�������裺
                                                                00��
                                                                01:
                                                                10��
                                                                11��  */
        unsigned char  da_buck_res12_cc_pz   : 2;  /* bit[2-3]: cc��·�������裺
                                                                00��
                                                                01:
                                                                10��
                                                                11�� */
        unsigned char  da_buck_res11_sel     : 2;  /* bit[4-5]: ���� */
        unsigned char  da_buck_res10_cc_rout : 2;  /* bit[6-7]: fast cc regl ��·preamp Rout���ڵ��裺
                                                                00��35K
                                                                01: 70K
                                                                10��105K
                                                                11��140K */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_18_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res13_dpm_pz_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res13_dpm_pz_END     (1)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res12_cc_pz_START    (2)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res12_cc_pz_END      (3)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res11_sel_START      (4)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res11_sel_END        (5)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res10_cc_rout_START  (6)
#define SOC_SCHARGER_BUCK_CFG_REG_18_da_buck_res10_cc_rout_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_19_UNION
 �ṹ˵��  : BUCK_CFG_REG_19 �Ĵ����ṹ���塣��ַƫ����:0x63����ֵ:0x15�����:8
 �Ĵ���˵��: BUCK_���üĴ���_19
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_res2_ea     : 2;  /* bit[0-1]: ���Ͳ������粹������R2��
                                                              00��13K
                                                              01: 26K
                                                              10��39K
                                                              11��91K */
        unsigned char  da_buck_res15_acl_z : 2;  /* bit[2-3]: acl��·��������_��㣺
                                                              00��
                                                              01:
                                                              10��
                                                              11��  */
        unsigned char  da_buck_res14_acl_p : 2;  /* bit[4-5]: acl��·��������_���㣺
                                                              00��
                                                              01:
                                                              10��
                                                              11��  */
        unsigned char  reserved            : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_19_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res2_ea_START      (0)
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res2_ea_END        (1)
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res15_acl_z_START  (2)
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res15_acl_z_END    (3)
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res14_acl_p_START  (4)
#define SOC_SCHARGER_BUCK_CFG_REG_19_da_buck_res14_acl_p_END    (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_20_UNION
 �ṹ˵��  : BUCK_CFG_REG_20 �Ĵ����ṹ���塣��ַƫ����:0x64����ֵ:0x39�����:8
 �Ĵ���˵��: BUCK_���üĴ���_20
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_res5_acl_rout : 2;  /* bit[0-1]: acl��·preamp Rout���ڵ��裺
                                                                00��54K
                                                                01: 108K
                                                                10��162K
                                                                11��216K */
        unsigned char  da_buck_res4_dpm_gm   : 2;  /* bit[2-3]: dpm��·preamp�絼���ڵ��裺
                                                                00��3.5K
                                                                01: 5.3K
                                                                10��7.1K
                                                                11��8.9K */
        unsigned char  da_buck_res3_ea       : 3;  /* bit[4-6]: ���Ͳ������粹������R3��
                                                                000��80K
                                                                001��120K
                                                                010��160K
                                                                011��200K
                                                                100��240K
                                                                101��280K
                                                                110��320K
                                                                111��360K */
        unsigned char  reserved              : 1;  /* bit[7-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_20_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res5_acl_rout_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res5_acl_rout_END    (1)
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res4_dpm_gm_START    (2)
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res4_dpm_gm_END      (3)
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res3_ea_START        (4)
#define SOC_SCHARGER_BUCK_CFG_REG_20_da_buck_res3_ea_END          (6)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_21_UNION
 �ṹ˵��  : BUCK_CFG_REG_21 �Ĵ����ṹ���塣��ַƫ����:0x65����ֵ:0x55�����:8
 �Ĵ���˵��: BUCK_���üĴ���_21
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_res9_cc_gm  : 2;  /* bit[0-1]: fast cc regl ��·preamp�絼���ڵ��裺
                                                              00��6.5K
                                                              01: 13K
                                                              10��17K
                                                              11��30K */
        unsigned char  da_buck_res8_cv_gm  : 2;  /* bit[2-3]: cv regl ��·preamp�絼���ڵ��裺
                                                              00��0.9K
                                                              01: 1.8K
                                                              10��2.7K
                                                              11��3.6K */
        unsigned char  da_buck_res7_sys_gm : 2;  /* bit[4-5]: sys regl ��·preamp�絼���ڵ��裺
                                                              00��1.8K
                                                              01: 2.7K
                                                              10��3.6K
                                                              11��4.5K */
        unsigned char  da_buck_res6_thm_gm : 2;  /* bit[6-7]: themal regl��·preamp�絼���ڵ��裺
                                                              00��3.5K
                                                              01: 5.3K
                                                              10��7.1K
                                                              11��8.9K */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_21_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res9_cc_gm_START   (0)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res9_cc_gm_END     (1)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res8_cv_gm_START   (2)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res8_cv_gm_END     (3)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res7_sys_gm_START  (4)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res7_sys_gm_END    (5)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res6_thm_gm_START  (6)
#define SOC_SCHARGER_BUCK_CFG_REG_21_da_buck_res6_thm_gm_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_22_UNION
 �ṹ˵��  : BUCK_CFG_REG_22 �Ĵ����ṹ���塣��ַƫ����:0x66����ֵ:0x00�����:8
 �Ĵ���˵��: BUCK_���üĴ���_22
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_reserve1 : 8;  /* bit[0-7]: <7>Ԥ���Ĵ���
                                                           <6:5>saw peak
                                                           11: 2.5V
                                                           10: 2.0V
                                                           01: 1.5V
                                                           00:�Զ�����vbus
                                                           <4:3>buck random ת��ʱ��
                                                           00: 64us
                                                           01: 32us
                                                           10: 16us
                                                           11: 8 us
                                                           <2>buck random en
                                                           1: ����
                                                           0: �ر�
                                                           <1:0>sc random ת��ʱ��
                                                           00: 64us
                                                           01: 32us
                                                           10: 16us
                                                           11: 8 us */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_22_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_22_da_buck_reserve1_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_22_da_buck_reserve1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_23_UNION
 �ṹ˵��  : BUCK_CFG_REG_23 �Ĵ����ṹ���塣��ַƫ����:0x67����ֵ:0xFF�����:8
 �Ĵ���˵��: BUCK_���üĴ���_23
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_reserve2 : 8;  /* bit[0-7]: <7:4>buck ocp peak����
                                                           1111�� 3.8A
                                                           1000~1111��-5%
                                                           0001~0111��+5%
                                                           ����ֵֻ���ο���5V��9V��12V������ͬ
                                                           <3>reverbst mode2 �Ƿ��Զ�����dpm������
                                                           1���Զ�
                                                           0���ֶ�
                                                           <2:0>�ֶ�reverbst mode2�ж�ֵ
                                                           111: 4.85
                                                           110��4.75
                                                           101��4.7
                                                           100��4.65
                                                           011��4.6
                                                           010��4.55
                                                           001��4.5
                                                           000��4.45 */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_23_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_23_da_buck_reserve2_START  (0)
#define SOC_SCHARGER_BUCK_CFG_REG_23_da_buck_reserve2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_24_UNION
 �ṹ˵��  : BUCK_CFG_REG_24 �Ĵ����ṹ���塣��ַƫ����:0x68����ֵ:0x00�����:8
 �Ĵ���˵��: BUCK_���üĴ���_24
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buck_ssmode_en      : 1;  /* bit[0-0]: buck����ʱ�Ƿ�Ҫ�ȴ�VC�ٿ���driver 0�����ȴ� 1���ȴ� */
        unsigned char  da_buck_ss_init_dis    : 1;  /* bit[1-1]: Ԥ�� */
        unsigned char  da_buck_sft_scp_enb    : 1;  /* bit[2-2]: buck �������Ƿ���scp���ܣ�0Ϊ������1Ϊ���� */
        unsigned char  da_buck_sft_ocp_en     : 1;  /* bit[3-3]: Ԥ�� */
        unsigned char  da_buck_sft_maxduty_en : 1;  /* bit[4-4]: ������ʱ���ռ�ձ�����ʹ�ܿ��� */
        unsigned char  da_buck_scp_dis        : 1;  /* bit[5-5]: buck scpʹ��������
                                                                 0��������1������ */
        unsigned char  da_buck_saw_vally_adj  : 1;  /* bit[6-6]: saw��ֵ���ڼĴ��� */
        unsigned char  da_buck_saw_peak_adj   : 1;  /* bit[7-7]: saw��ֵ���ڼĴ��� */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_24_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_ssmode_en_START       (0)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_ssmode_en_END         (0)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_ss_init_dis_START     (1)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_ss_init_dis_END       (1)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_scp_enb_START     (2)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_scp_enb_END       (2)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_ocp_en_START      (3)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_ocp_en_END        (3)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_maxduty_en_START  (4)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_sft_maxduty_en_END    (4)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_scp_dis_START         (5)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_scp_dis_END           (5)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_saw_vally_adj_START   (6)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_saw_vally_adj_END     (6)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_saw_peak_adj_START    (7)
#define SOC_SCHARGER_BUCK_CFG_REG_24_da_buck_saw_peak_adj_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_CFG_REG_25_UNION
 �ṹ˵��  : BUCK_CFG_REG_25 �Ĵ����ṹ���塣��ַƫ����:0x69����ֵ:0x1B�����:8
 �Ĵ���˵��: BUCK_���üĴ���_25
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_buckotg_ss_speed   : 2;  /* bit[0-1]: buck������ʱ��������ڣ�
                                                                00��������Լ1.5mS
                                                                11����죬Լ0.5mS */
        unsigned char  da_buckotg_acl_downen : 1;  /* bit[2-2]: buck_otg acl op������·ʹ�ܣ�
                                                                1������
                                                                0���ر� */
        unsigned char  da_buck_sysovp_en     : 1;  /* bit[3-3]: buck sys ovp���ܿ���
                                                                0���ر�
                                                                1������ */
        unsigned char  da_buck_sysmin_sel    : 2;  /* bit[4-5]: Vsys��С�����ѹѡ��
                                                                00:3.4V
                                                                01:3.5V
                                                                10:3.6V
                                                                11:3.7V */
        unsigned char  reserved              : 2;  /* bit[6-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_CFG_REG_25_UNION;
#endif
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buckotg_ss_speed_START    (0)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buckotg_ss_speed_END      (1)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buckotg_acl_downen_START  (2)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buckotg_acl_downen_END    (2)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buck_sysovp_en_START      (3)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buck_sysovp_en_END        (3)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buck_sysmin_sel_START     (4)
#define SOC_SCHARGER_BUCK_CFG_REG_25_da_buck_sysmin_sel_END       (5)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_RO_REG_26_UNION
 �ṹ˵��  : BUCK_RO_REG_26 �Ĵ����ṹ���塣��ַƫ����:0x6A����ֵ:0x00�����:8
 �Ĵ���˵��: BUCK_ֻ���Ĵ���_26
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_buck_ok : 1;  /* bit[0-0]: buck����״ָ̬ʾ��
                                                     0��buckû�й�����
                                                     1��buck�ڹ����� */
        unsigned char  reserved   : 7;  /* bit[1-7]: reserved */
    } reg;
} SOC_SCHARGER_BUCK_RO_REG_26_UNION;
#endif
#define SOC_SCHARGER_BUCK_RO_REG_26_ad_buck_ok_START  (0)
#define SOC_SCHARGER_BUCK_RO_REG_26_ad_buck_ok_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_RO_REG_27_UNION
 �ṹ˵��  : BUCK_RO_REG_27 �Ĵ����ṹ���塣��ַƫ����:0x6B����ֵ:0x00�����:8
 �Ĵ���˵��: BUCK_ֻ���Ĵ���_27
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_buck_state0 : 8;  /* bit[0-7]: <7> AVDD_OK
                                                         <6> REGN_OK_D
                                                         <5> AVDDBAS_OK
                                                         <4> buck_en
                                                         <3> EN_DELAY_60US_BAS
                                                         <2> EN_CC_CV
                                                         <1> BUCK_SSOK
                                                         <0> OTG_SSOK */
    } reg;
} SOC_SCHARGER_BUCK_RO_REG_27_UNION;
#endif
#define SOC_SCHARGER_BUCK_RO_REG_27_ad_buck_state0_START  (0)
#define SOC_SCHARGER_BUCK_RO_REG_27_ad_buck_state0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_BUCK_RO_REG_28_UNION
 �ṹ˵��  : BUCK_RO_REG_28 �Ĵ����ṹ���塣��ַƫ����:0x6C����ֵ:0x00�����:8
 �Ĵ���˵��: BUCK_ֻ���Ĵ���_28
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_buck_state1 : 8;  /* bit[0-7]: <7> ISO_A2D
                                                         <6> REGN_OK_D
                                                         <5> AVDDBAS_OK
                                                         <4> buck_en
                                                         <3> EN_DELAY_60US_BAS
                                                         <2>
                                                         <1>
                                                         <0>  */
    } reg;
} SOC_SCHARGER_BUCK_RO_REG_28_UNION;
#endif
#define SOC_SCHARGER_BUCK_RO_REG_28_ad_buck_state1_START  (0)
#define SOC_SCHARGER_BUCK_RO_REG_28_ad_buck_state1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_0_UNION
 �ṹ˵��  : OTG_CFG_REG_0 �Ĵ����ṹ���塣��ַƫ����:0x6D����ֵ:0x27�����:8
 �Ĵ���˵��: OTG_���üĴ���_0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_hmos     : 1;  /* bit[0-0]: OTG �Ϲܿ���ѡ��
                                                          0��ǿ�ƹر�
                                                          1���������� */
        unsigned char  da_otg_dmd      : 1;  /* bit[1-1]: boost dmdʹ�ܣ�
                                                          0: �رչ�����
                                                          1: ʹ�ܹ����� */
        unsigned char  da_otg_clp_l_en : 1;  /* bit[2-2]: boost EAǯλ��·���ͣ�ʹ�ܣ�
                                                          0: �ر�ǯλ��·
                                                          1: ʹ��ǯλ��· */
        unsigned char  da_otg_clp_h_en : 1;  /* bit[3-3]: boost EAǯλ��·���ߣ�ʹ�ܣ�
                                                          0: �ر�ǯλ��·
                                                          1: ʹ��ǯλ��· */
        unsigned char  da_otg_ckmin    : 2;  /* bit[4-5]: NMOS��С��ͨʱ��
                                                          00: 40ns
                                                          01: 30ns
                                                          10: 20ns
                                                          11: 15ns */
        unsigned char  da_otg_30ma     : 1;  /* bit[6-6]: OTG 30mA���ظ�Чģʽ����
                                                          0���ر�
                                                          1������ */
        unsigned char  da_com_otg_mode : 1;  /* bit[7-7]: 1��OTG��ͨģʽ������ͨ������
                                                          0���ر�OTG����ģʽ��ֻ�ر�OVP�ܣ� */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_0_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_hmos_START      (0)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_hmos_END        (0)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_dmd_START       (1)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_dmd_END         (1)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_clp_l_en_START  (2)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_clp_l_en_END    (2)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_clp_h_en_START  (3)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_clp_h_en_END    (3)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_ckmin_START     (4)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_ckmin_END       (5)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_30ma_START      (6)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_otg_30ma_END        (6)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_com_otg_mode_START  (7)
#define SOC_SCHARGER_OTG_CFG_REG_0_da_com_otg_mode_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_1_UNION
 �ṹ˵��  : OTG_CFG_REG_1 �Ĵ����ṹ���塣��ַƫ����:0x6E����ֵ:0x0B�����:8
 �Ĵ���˵��: OTG_���üĴ���_1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_ocp_en       : 1;  /* bit[0-0]: boost OCP����ѡ��
                                                              0: ����������
                                                              1: ���������� */
        unsigned char  da_otg_lmos_ocp     : 2;  /* bit[1-2]: boost �Ͷ�MOS������ֵ(A)
                                                              00: 2.0
                                                              01: 2.5
                                                              10: 3.0
                                                              11: 4.0 */
        unsigned char  da_otg_lim_set      : 2;  /* bit[3-4]: boostƽ������ѡ��
                                                              00��1.5A
                                                              01��1A
                                                              10��1A
                                                              11��0.5A */
        unsigned char  da_otg_lim_dcoffset : 3;  /* bit[5-7]: otg����ֵdcƫ��,500mA��λƫ��100%��1A��λƫ��50%��1.5A��λƫ��33%
                                                              000:��ƫ��
                                                              001: ƫС6.25%
                                                              010: ƫС12.5%
                                                              011: ƫС18.75%
                                                              100����ƫ��
                                                              101: ƫ��6.25%
                                                              110: ƫ��12.5%
                                                              111: ƫ��18.75%  */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_1_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_ocp_en_START        (0)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_ocp_en_END          (0)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lmos_ocp_START      (1)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lmos_ocp_END        (2)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lim_set_START       (3)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lim_set_END         (4)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lim_dcoffset_START  (5)
#define SOC_SCHARGER_OTG_CFG_REG_1_da_otg_lim_dcoffset_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_2_UNION
 �ṹ˵��  : OTG_CFG_REG_2 �Ĵ����ṹ���塣��ַƫ����:0x6F����ֵ:0xC3�����:8
 �Ĵ���˵��: OTG_���üĴ���_2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_ovp_en    : 1;  /* bit[0-0]: boost ovpʹ�ܣ�
                                                           0: �رչ�ѹ��·�����ϱ���
                                                           1: ʹ�ܹ�ѹ��·���ϱ��� */
        unsigned char  da_otg_osc_ckmax : 2;  /* bit[1-2]: �����ŵ������uA��
                                                           00: 13
                                                           01: 14
                                                           10: 15
                                                           11: 16  */
        unsigned char  da_otg_osc       : 4;  /* bit[3-6]: ����Ƶ�ʵ��ڣ�
                                                           0000:2.5M
                                                           0001:2.3M
                                                           ����
                                                           1000:1.4M
                                                           1001:1.3M
                                                           ����
                                                           0000:0.6M */
        unsigned char  da_otg_ocp_sys   : 1;  /* bit[7-7]: boost OCP����ѡ��
                                                           0: �������ϱ�
                                                           1: �������ϱ� */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_2_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_ovp_en_START     (0)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_ovp_en_END       (0)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_osc_ckmax_START  (1)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_osc_ckmax_END    (2)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_osc_START        (3)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_osc_END          (6)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_ocp_sys_START    (7)
#define SOC_SCHARGER_OTG_CFG_REG_2_da_otg_ocp_sys_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_3_UNION
 �ṹ˵��  : OTG_CFG_REG_3 �Ĵ����ṹ���塣��ַƫ����:0x70����ֵ:0x00�����:8
 �Ĵ���˵��: OTG_���üĴ���_3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_reserve1 : 8;  /* bit[0-7]: <7>��PFM��׼��ѹѡ��
                                                          0��1.05 V
                                                          1: 1.075V
                                                          <6:4>otg �����ϵ��ݣ�ÿ��200f */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_3_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_3_da_otg_reserve1_START  (0)
#define SOC_SCHARGER_OTG_CFG_REG_3_da_otg_reserve1_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_4_UNION
 �ṹ˵��  : OTG_CFG_REG_4 �Ĵ����ṹ���塣��ַƫ����:0x71����ֵ:0x00�����:8
 �Ĵ���˵��: OTG_���üĴ���_4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_reserve2 : 8;  /* bit[0-7]: �Ĵ���Ԥ�� */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_4_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_4_da_otg_reserve2_START  (0)
#define SOC_SCHARGER_OTG_CFG_REG_4_da_otg_reserve2_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_5_UNION
 �ṹ˵��  : OTG_CFG_REG_5 �Ĵ����ṹ���塣��ַƫ����:0x72����ֵ:0xA6�����:8
 �Ĵ���˵��: OTG_���üĴ���_5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  da_otg_slop_set : 2;  /* bit[0-1]: boost б�²�������ѡ��
                                                          00: 1.5p
                                                          01: 2p
                                                          10: 2.5p
                                                          11: 3p */
        unsigned char  da_otg_slop_en  : 1;  /* bit[2-2]: boost б�²���ʹ�ܣ�
                                                          0: �ر�б�²���
                                                          1: ʹ��б�²��� */
        unsigned char  da_otg_skip_set : 1;  /* bit[3-3]: skip������Pmos�Ŀ���״̬��
                                                          0: PMOS�ر�
                                                          1: PMOS���� */
        unsigned char  da_otg_scp_time : 1;  /* bit[4-4]: boost scpʱ��ѡ��
                                                          0: 1ms��VOUT����0.85BAT���ϱ���·
                                                          1: 2ms��VOUT����0.85BAT���ϱ���· */
        unsigned char  da_otg_scp_en   : 1;  /* bit[5-5]: boost scp����ѡ��
                                                          0: ��·��ϵͳ���Զ��رգ����ϱ���
                                                          1: ��·��ϵͳ�Զ��ر� */
        unsigned char  da_otg_rf       : 2;  /* bit[6-7]: ��е���ת��ѹ�迹
                                                          00: 0.5����ʵ��100k����
                                                          01: 0.4����ʵ��80k����
                                                          10: 0.3����ʵ��60k����
                                                          11: 0.2����ʵ��40k���� */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_5_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_slop_set_START  (0)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_slop_set_END    (1)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_slop_en_START   (2)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_slop_en_END     (2)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_skip_set_START  (3)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_skip_set_END    (3)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_scp_time_START  (4)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_scp_time_END    (4)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_scp_en_START    (5)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_scp_en_END      (5)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_rf_START        (6)
#define SOC_SCHARGER_OTG_CFG_REG_5_da_otg_rf_END          (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_CFG_REG_6_UNION
 �ṹ˵��  : OTG_CFG_REG_6 �Ĵ����ṹ���塣��ַƫ����:0x73����ֵ:0x15�����:8
 �Ĵ���˵��: OTG_���üĴ���_6
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  otg_clp_l_set   : 1;  /* bit[0-0]: boost EA���ǯλֵ��VRAMPֱ����ƽֵ��ʧ��
                                                          0��27mv
                                                          1��40mv */
        unsigned char  da_wl_otg_mode  : 1;  /* bit[1-1]: 1��OTG����ģʽ������ͨ������
                                                          0���ر�OTG����ģʽ��ֻ�ر����߿��أ� */
        unsigned char  da_otg_uvp_en   : 1;  /* bit[2-2]: boost uvpʹ�ܣ�
                                                          0: �ر�Ƿѹ��·�����ϱ���
                                                          1: ʹ��Ƿѹ��·���ϱ��� */
        unsigned char  da_otg_switch   : 1;  /* bit[3-3]: OTG SWITCHģʽ����
                                                          0���ر�SWITCH
                                                          1������SWITCH */
        unsigned char  da_otg_pfm_v_en : 1;  /* bit[4-4]: boost pfm_vʹ�ܣ�
                                                          0: �ر�pfm
                                                          1: ʹ��pfm */
        unsigned char  reserved        : 3;  /* bit[5-7]: reserved */
    } reg;
} SOC_SCHARGER_OTG_CFG_REG_6_UNION;
#endif
#define SOC_SCHARGER_OTG_CFG_REG_6_otg_clp_l_set_START    (0)
#define SOC_SCHARGER_OTG_CFG_REG_6_otg_clp_l_set_END      (0)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_wl_otg_mode_START   (1)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_wl_otg_mode_END     (1)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_uvp_en_START    (2)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_uvp_en_END      (2)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_switch_START    (3)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_switch_END      (3)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_pfm_v_en_START  (4)
#define SOC_SCHARGER_OTG_CFG_REG_6_da_otg_pfm_v_en_END    (4)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_RO_REG_7_UNION
 �ṹ˵��  : OTG_RO_REG_7 �Ĵ����ṹ���塣��ַƫ����:0x74����ֵ:0x00�����:8
 �Ĵ���˵��: OTG_ֻ���Ĵ���_7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_otg_on    : 1;  /* bit[0-0]: boost״ָ̬ʾ��
                                                       0��OTG�ر�
                                                       1��OTG���� */
        unsigned char  ad_otg_en_in : 1;  /* bit[1-1]: OTGʹ�ܵ�ָʾ�źţ�
                                                       0��OTGû��ʹ�ܣ�
                                                       1��OTG�Ѿ�ʹ�ܣ� */
        unsigned char  reserved     : 6;  /* bit[2-7]: reserved */
    } reg;
} SOC_SCHARGER_OTG_RO_REG_7_UNION;
#endif
#define SOC_SCHARGER_OTG_RO_REG_7_ad_otg_on_START     (0)
#define SOC_SCHARGER_OTG_RO_REG_7_ad_otg_on_END       (0)
#define SOC_SCHARGER_OTG_RO_REG_7_ad_otg_en_in_START  (1)
#define SOC_SCHARGER_OTG_RO_REG_7_ad_otg_en_in_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_RO_REG_8_UNION
 �ṹ˵��  : OTG_RO_REG_8 �Ĵ����ṹ���塣��ַƫ����:0x75����ֵ:0x00�����:8
 �Ĵ���˵��: OTG_ֻ���Ĵ���_8
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_otg_state0 : 8;  /* bit[0-7]: <7> otg_en_ana
                                                        <6> otg_en_reg
                                                        <5> otg_en_dig
                                                        <4> WEAK_ON
                                                        <3> PWROK
                                                        <2> SSOK
                                                        <1> PWM
                                                        <0> PWM_DE */
    } reg;
} SOC_SCHARGER_OTG_RO_REG_8_UNION;
#endif
#define SOC_SCHARGER_OTG_RO_REG_8_ad_otg_state0_START  (0)
#define SOC_SCHARGER_OTG_RO_REG_8_ad_otg_state0_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_SCHARGER_OTG_RO_REG_9_UNION
 �ṹ˵��  : OTG_RO_REG_9 �Ĵ����ṹ���塣��ַƫ����:0x76����ֵ:0x00�����:8
 �Ĵ���˵��: OTG_ֻ���Ĵ���_9
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned char      value;
    struct
    {
        unsigned char  ad_otg_state1 : 8;  /* bit[0-7]: <7> LVC_MODE_EN
                                                        <6> SC_MODE_EN
                                                        <5> BLK_EN
                                                        <4> DC_OK_SHIELD
                                                        <3> CHG_EN
                                                        <2> OVP_EN
                                                        <1>
                                                        <0>  */
    } reg;
} SOC_SCHARGER_OTG_RO_REG_9_UNION;
#endif
#define SOC_SCHARGER_OTG_RO_REG_9_ad_otg_state1_START  (0)
#define SOC_SCHARGER_OTG_RO_REG_9_ad_otg_state1_END    (7)






/*****************************************************************************
  8 OTHERS����
*****************************************************************************/



/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_scharger_interface.h */
