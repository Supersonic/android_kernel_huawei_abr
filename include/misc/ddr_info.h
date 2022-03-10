#ifndef DDR_INFO_H
#define DDR_INFO_H

#define SAMSUNG_ID    1
#define ELPIDA_ID    3
#define HYNIX_ID    6
#define FC_ID    19
#define MICRON_ID    255
#define DDR_SIZE_ORDER_MAX    36 // 64Gbit,8GB
#define DDR_SIZE_ORDER_MIN    30 // 1Gbit,125MB
#define DDR_SIZE_MAX    (0x1 << (DDR_SIZE_ORDER_MAX - 30))
#define DDR_SIZE_MIN    (0x1 << (DDR_SIZE_ORDER_MIN - 30))
#define HW_FUNC_RET_FAIL    (-1)
#define HW_FUNC_RET_SUCC    1
#define HW_BUF_LEN_OFFSET_5    5
#define HW_BUF_LEN_8    8
#define HW_RIGHT_SHIFT_8    8
#define HW_RIGHT_SHIFT_24    24
#define HW_MASK    (0xFF)

/* DDR types. */
typedef enum
{
	DDR_TYPE_NODDR = 0,
	DDR_TYPE_LPDDR1 = 1,           /**< Low power DDR1. */
	DDR_TYPE_LPDDR2 = 2,       /**< Low power DDR2  set to 2 for compatibility*/
	DDR_TYPE_PCDDR2 = 3,           /**< Personal computer DDR2. */
	DDR_TYPE_PCDDR3 = 4,           /**< Personal computer DDR3. */

	DDR_TYPE_LPDDR3 = 5,           /**< Low power DDR3. */
	DDR_TYPE_LPDDR4 = 6,           /**< Low power DDR4. */
	DDR_TYPE_LPDDR4X = 7,            /**< Low power DDR4x. */
	DDR_TYPE_LPDDR5 = 8,            /**< Low power DDR5. */
	DDR_TYPE_LPDDR5X = 9,           /**< Low power DDR5x. */

	DDR_TYPE_UNUSED = 0x7FFFFFFF  /**< For compatibility with deviceprogrammer(features not using DDR). */
} DDR_TYPE;

void app_info_print_smem(void);

#endif
