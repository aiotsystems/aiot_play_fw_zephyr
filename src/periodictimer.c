/*H**********************************************************************
* FILENAME :        periodictimer.c
*
* DESCRIPTION :
*       Periodic Timer in Zephyr for an AIOT sample application
*
* LICENSE:
*       Parts are copied from:
*       - aiot_play_fw - INRIA
*
* NOTES :
*       This application is part of the OpenSwarm Project.
* 
* COPYRIGHT:
*       Copyright Siemens AG and Inria  - All rights reserved.
*
* AUTHOR :    Fabian Graf       START DATE :    26 Sep 2023
*
* CHANGES :
*
* VERSION DATE    WHO     DETAIL
* 0       26Sep23 FG      Initial Commit
*
*H*/

#include <string.h>
#include <nrf52833.h>
#include <zephyr/kernel.h>
#include "periodictimer.h"

//=========================== define ==========================================

//=========================== variables =======================================

typedef struct {
    uint16_t        period_s;
    periodtimer_cbt periodtimer_cb;
    uint16_t        countdown_s;
} periodictimer_vars_t;

periodictimer_vars_t periodictimer_vars;

typedef struct {
    uint32_t        num_ISR_RTC0_IRQHandler;
    uint32_t        num_ISR_RTC0_IRQHandler_COMPARE0;
} periodictimer_dbg_t;

periodictimer_dbg_t periodictimer_dbg;
struct k_timer periodic_zephyr_timer;

//=========================== prototypes ======================================

void RTC0_IRQHandler(void);

//=========================== public ==========================================

void periodictimer_init(uint16_t period_s, periodtimer_cbt periodtimer_cb) {

    // reset variables
    memset(&periodictimer_vars,0x00,sizeof(periodictimer_vars_t));
    // store params
    periodictimer_vars.period_s        = period_s;
    periodictimer_vars.periodtimer_cb  = periodtimer_cb;
    // initialize timer
    k_timer_init(&periodic_zephyr_timer, RTC0_IRQHandler, NULL);
    periodictimer_vars.countdown_s     = periodictimer_vars.period_s;
    k_timer_start(&periodic_zephyr_timer, K_MSEC(period_s*1000), K_MSEC(period_s*1000));
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

void RTC0_IRQHandler(void) {
   // debug
   periodictimer_dbg.num_ISR_RTC0_IRQHandler++;
   periodictimer_vars.countdown_s--;
   periodictimer_vars.countdown_s = periodictimer_vars.period_s;
   periodictimer_vars.periodtimer_cb();
}