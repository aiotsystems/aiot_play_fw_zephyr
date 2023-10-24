/*H***********************************************************************
* FILENAME :        board.c
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

#include "board.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void board_init(void) {
    
    // start 32kHz XTAL
    NRF_CLOCK->LFCLKSRC                = 0x00000001; // 1==XTAL
    NRF_CLOCK->EVENTS_LFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_LFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    
    // start HFCLK
    NRF_CLOCK->EVENTS_HFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void board_sleep(void) {
    // wait for event
    __SEV(); // set event
    __WFE(); // wait for event
    __WFE(); // wait for event
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================
