/* SPDX-License-Identifier: GPL-2.0 */
/*
 * stm32g031_ufcs.h
 *
 * stm32g031 ufcs header file
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#ifndef _STM32G031_UFCS_H_
#define _STM32G031_UFCS_H_

#include "stm32g031.h"

#define STM32G031_UFCS_WAIT_RETRY_CYCLE                 40
#define STM32G031_UFCS_HANDSHARK_RETRY_CYCLE            10
#define STM32G031_UFCS_TX_BUFFER_SIZE                   32
#define STM32G031_UFCS_RX_BUFFER_SIZE                   128

/* CTL1 reg=0x40 */
#define STM32G031_UFCS_CTL1_REG                         0x40
#define STM32G031_UFCS_CTL1_SOFT_RESET_MASK             BIT(7)
#define STM32G031_UFCS_CTL1_SOFT_RESET_SHIFT            7
#define STM32G031_UFCS_CTL1_EN_UFCS_MASK                BIT(6)
#define STM32G031_UFCS_CTL1_EN_UFCS_SHIFT               6
#define STM32G031_UFCS_CTL1_EN_UFCS_HANDSHAKE_MASK      BIT(5)
#define STM32G031_UFCS_CTL1_EN_UFCS_HANDSHAKE_SHIFT     5
#define STM32G031_UFCS_CTL1_BAUD_RATE_MASK              (BIT(4) | BIT(3))
#define STM32G031_UFCS_CTL1_BAUD_RATE_SHIFT             3
#define STM32G031_UFCS_CTL1_SEND_MASK                   BIT(2)
#define STM32G031_UFCS_CTL1_SEND_SHIFT                  2
#define STM32G031_UFCS_CTL1_CABLE_HARDRESET_MASK        BIT(1)
#define STM32G031_UFCS_CTL1_CABLE_HARDRESET_SHIFT       1
#define STM32G031_UFCS_CTL1_SOURCE_HARDRESET_MASK       BIT(0)
#define STM32G031_UFCS_CTL1_SOURCE_HARDRESET_SHIFT      0

/* ISR1 reg=0x41 */
#define STM32G031_UFCS_ISR1_REG                         0x41
#define STM32G031_UFCS_ISR1_HANDSHAKE_FLAG_MASK         BIT(5)
#define STM32G031_UFCS_ISR1_HANDSHAKE_FLAG_SHIFT        5
#define STM32G031_UFCS_ISR1_BAUD_RATE_ERROR_MASK        BIT(4)
#define STM32G031_UFCS_ISR1_BAUD_RATE_ERROR_SHIFT       4
#define STM32G031_UFCS_ISR1_CRC_ERROR_MASK              BIT(3)
#define STM32G031_UFCS_ISR1_CRC_ERROR_SHIFT             3
#define STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_MASK   BIT(2)
#define STM32G031_UFCS_ISR1_SEND_PACKET_COMPLETE_SHIFT  2
#define STM32G031_UFCS_ISR1_DATA_READY_MASK             BIT(1)
#define STM32G031_UFCS_ISR1_DATA_READY_SHIFT            1
#define STM32G031_UFCS_ISR1_HARD_RESET_MASK             BIT(0)
#define STM32G031_UFCS_ISR1_HARD_RESET_SHIFT            0

/* MASK1 reg=0x42 */
#define STM32G031_UFCS_MASK1_REG                        0x42
#define STM32G031_UFCS_MASK1_HANDSHAKE_FLAG_MASK        BIT(5)
#define STM32G031_UFCS_MASK1_HANDSHAKE_FLAG_SHIFT       5
#define STM32G031_UFCS_MASK1_BAUD_RATE_ERROR_MASK       BIT(4)
#define STM32G031_UFCS_MASK1_BAUD_RATE_ERROR_SHIFT      4
#define STM32G031_UFCS_MASK1_CRC_ERROR_MASK             BIT(3)
#define STM32G031_UFCS_MASK1_CRC_ERROR_SHIFT            3
#define STM32G031_UFCS_MASK1_SEND_PACKET_COMPLETE_MASK  BIT(2)
#define STM32G031_UFCS_MASK1_SEND_PACKET_COMPLETE_SHIFT 2
#define STM32G031_UFCS_MASK1_DATA_READY_MASK            BIT(1)
#define STM32G031_UFCS_MASK1_DATA_READY_SHIFT           1
#define STM32G031_UFCS_MASK1_HARD_RESET_MASK            BIT(0)
#define STM32G031_UFCS_MASK1_HARD_RESET_SHIFT           0

/* RECEIVE_BAUD_RATE reg=0x43 */
#define STM32G031_UFCS_RECEIVE_BAUD_RATE_REG            0x43
#define STM32G031_UFCS_RECEIVE_BAUD_RATE_UNIT           500

/* TX_HEADER_H reg=0x44 */
#define STM32G031_UFCS_TX_HEADER_H_REG                  0x44

/* TX_HEADER_L reg=0x45 */
#define STM32G031_UFCS_TX_HEADER_L_REG                  0x45

/* TX_CMD reg=0x46 */
#define STM32G031_UFCS_TX_CMD_REG                       0x46

/* TX_LENGTH reg=0x41 */
#define STM32G031_UFCS_TX_LENGTH_REG                    0x47

/* TX_BUFFER_0 reg=0x48 */
#define STM32G031_UFCS_TX_BUFFER_0_REG                  0x48

/* RX_HEADER_H reg=0x68 */
#define STM32G031_UFCS_RX_HEADER_H_REG                  0x68

/* RX_HEADER_L reg=0x69 */
#define STM32G031_UFCS_RX_HEADER_L_REG                  0x69

/* RX_CMD reg=0x6a */
#define STM32G031_UFCS_RX_CMD_REG                       0x6a

/* RX_LENGTH reg=0x6b */
#define STM32G031_UFCS_RX_LENGTH_REG                    0x6b

/* RX_BUFFER_0 reg=0x6c */
#define STM32G031_UFCS_RX_BUFFER_0_REG                  0x6c

int stm32g031_hwufcs_register(struct stm32g031_device_info *di);

#endif /* _STM32G031_UFCS_H_ */
