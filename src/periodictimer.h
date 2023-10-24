/*H***********************************************************************
* FILENAME :        periodictimer.h
*
* DESCRIPTION :
* Periodic Timer in Zephyr for an AIOT sample application
*
* LICENSE AND COPYRIGHT:
* SPDX-FileCopyrightText: (c) 2023 Siemens AG and INRIA
* SPDX-License-Identifier: BSD-3-Clause
* 
* Parts are copied from:
* - aiot_play_fw - INRIA - https://github.com/aiotsystems/aiot_play_fw.git 
*
* NOTES :
* This application is part of the OpenSwarm Project.
*
* AUTHOR :
* Fabian Graf - fabian.graf@siemens.com
*
* CHANGES :
* VERSION DATE    WHO     DETAIL
* 0       26Sep23 FG      Initial Commit
*
*H***********************************************************************/

#ifndef __PERIODICTIMER_H
#define __PERIODICTIMER_H

#include "nrf52833.h"

//=========================== define ==========================================

//=========================== typedef =========================================

typedef void (*periodtimer_cbt)(void);

//=========================== variables =======================================

//=========================== prototypes ======================================

void periodictimer_init(uint16_t period_s, periodtimer_cbt periodtimer_cb);

#endif