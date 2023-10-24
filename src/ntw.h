/*H***********************************************************************
* FILENAME :        ntw.c
*
* DESCRIPTION :
* Networking Interface of the AIOT Board
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

#ifndef __NTW_H
#define __NTW_H

#include <stdbool.h>
#include <stdint.h>

//=========================== define ==========================================

//=========================== typedef =========================================

typedef void (*ntw_receive_cbt)(uint8_t* buf, uint8_t bufLen);

//=========================== variables =======================================

//=========================== prototypes ======================================

void ntw_init(ntw_receive_cbt ntw_receive_cb);
bool ntw_transmit(uint8_t* buf, uint8_t bufLen);

#endif