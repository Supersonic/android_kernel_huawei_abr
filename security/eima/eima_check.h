/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: the eima_check.h for api of send and get eima data
 * Create: 2021-03-22
 */

#ifndef _EIMA_CHECK_H_
#define _EIMA_CHECK_H_

#include "eima_agent_api.h"

/*
 * Description:
 *    send EIMA measure list, measure baseline or runtime measure data to handle.
 * Input value:
 *    type is the type of send data, see enum eima_msg_type for more information
 *    mlist_msg is the the list or hash all process EIMA measured
 * Return Vaslue:
 *    0: send data success
 *    other value: send data failed
 */

int send_eima_data(int type, const struct m_list_msg *mlist_msg);

/*
 * Description:
 *    get the latest eima measured result.
 * Return Value:
 *    0: all the process eima measured is safe
 *    others: one of the process eima measured is risk, the risk process path
 *            can get by get_hash_error_file_path
 */
int get_eima_status(void);

/*
 * Description:
 *    get the tampered file path when the measurement of one process failed.
 * Return Value:
 *    one c style string, either "" or "[static]/path/to/file:"
 */
char *get_hash_error_file_path(void);

#endif