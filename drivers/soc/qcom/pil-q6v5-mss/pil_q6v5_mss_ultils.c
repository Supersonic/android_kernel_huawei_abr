/*
 * Copyright (C) 2013 Huawei Device Co.Ltd
 * License terms: GNU General Public License (GPL) version 2
 *
 */

#include "pil_q6v5_mss_ultils.h"

void restart_oem_qmi(void)
{
    struct task_struct *tsk = NULL;

    for_each_process(tsk)
    {
        if (tsk->comm && !strcmp(tsk->comm, OEM_QMI))
        {
            send_sig(SIGKILL, tsk, 0);
            return;
        }
    }
}