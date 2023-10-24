/*H***********************************************************************
* FILENAME :        board.h
*
* DESCRIPTION :
* Board Configuration file for the AIOT
*
* LICENSE AND COPYRIGHT:
* SPDX-FileCopyrightText: (c) 2022 INRIA
* SPDX-License-Identifier: BSD-3-Clause
*
* This file is copied from:
* - aiot_play_fw - INRIA - https://github.com/aiotsystems/aiot_play_fw.git 
* 
* NOTES :
* This application is part of the OpenSwarm Project.
*
* AUTHOR :
* Thomas Watteyne - thomas.watteyne@inria.fr
*
* CHANGES :
* VERSION DATE    WHO     DETAIL
* 0       27Oct22 TW      Initial Commit
*
*H***********************************************************************/

#ifndef __BOARD_H
#define __BOARD_H

#include <nrf52833.h>

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

void board_init(void);
void board_sleep(void);

#endif
