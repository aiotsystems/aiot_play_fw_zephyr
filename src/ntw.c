/*H**********************************************************************
* FILENAME :        ntw.c
*
* DESCRIPTION :
*       Networking API of the AIOT Play
*
* LICENSE:
*       Parts are copied from:
*       - aiot_play_fw - INRIA
*
* NOTES :
*       This application is part of the OpenSwarm Project.
*
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

#include <zephyr/kernel.h>
#include <stdint.h>
#include <nrf52833.h>
#include "sm_clib-REL-1.0.2.9/sm_clib/dn_ipmt.h"
#include "ntw.h"

//=========================== defines =========================================

// mote state
#define MOTE_STATE_IDLE           0x01
#define MOTE_STATE_SEARCHING      0x02
#define MOTE_STATE_NEGOCIATING    0x03
#define MOTE_STATE_CONNECTED      0x04
#define MOTE_STATE_OPERATIONAL    0x05

// service types
#define SERVICE_TYPE_BW           0x00

// timings
#define SERIAL_RESPONSE_TIMEOUT   16384          // 16384@32kHz = 500ms
#define CMD_PERIOD                32768          // 32768@32kHz = 1s

// api
#define SRC_PORT                  0xf0b8
#define DST_PORT                  0xf0b8

//=========================== typedefs ========================================

typedef void (*fsm_timer_callback)(void);
typedef void (*fsm_reply_callback)(void);

//=========================== variables =======================================

typedef struct {
    // interface
    ntw_receive_cbt     ntw_receive_cb;
    // ipmt
    bool                isOper;                            // true iff is operational
    uint8_t             socketId;                          // ID of the mote's UDP socket
    uint8_t             replyBuf[MAX_FRAME_LENGTH];        // holds notifications from ipmt
    uint8_t             notifBuf[MAX_FRAME_LENGTH];        // notifications buffer internal to ipmt
    // fsm
    fsm_timer_callback  fsmCb;
    // reply
    fsm_reply_callback  replyCb;
} ntw_vars_t;

ntw_vars_t ntw_vars;

typedef struct {
    uint32_t            num_calls_api_getMoteStatus;
    uint32_t            num_calls_api_getMoteStatus_reply;
    uint32_t            num_calls_api_openSocket;
    uint32_t            num_calls_api_openSocket_reply;
    uint32_t            num_calls_api_bindSocket;
    uint32_t            num_calls_api_bindSocket_reply;
    uint32_t            num_calls_api_join;
    uint32_t            num_calls_api_join_reply;
    uint32_t            num_calls_api_sendTo;
    uint32_t            num_calls_api_sendTo_reply;
    uint32_t            num_notif_EVENTS;
    uint32_t            num_notif_RECEIVE;
    uint32_t            num_ISR_RTC1_IRQHandler;
    uint32_t            num_ISR_RTC1_IRQHandler_COMPARE0;
} ntw_dbg_t;

ntw_dbg_t ntw_dbg;
struct k_timer my_timer;

//=========================== prototypes ======================================

// fsm
void     fsm_scheduleEvent(uint16_t delay, fsm_timer_callback cb);
void     fsm_cancelEvent(void);
void     fsm_setCallback(fsm_reply_callback cb);
// ipmt
void     dn_ipmt_notif_cb(uint8_t cmdId, uint8_t subCmdId);
void     dn_ipmt_reply_cb(uint8_t cmdId);
// api
void     api_response_timeout(void);
void     api_getMoteStatus(void);
void     api_getMoteStatus_reply(void);
void     api_openSocket(void);
void     api_openSocket_reply(void);
void     api_bindSocket(void);
void     api_bindSocket_reply(void);
void     api_join(void);
void     api_join_reply(void);
bool     api_sendTo(uint8_t* buf, uint8_t bufLen);
void     api_sendTo_reply(void);
// bsp
void     lfxtal_start(void);
void     hfclock_start(void);
// irq
void     RTC1_IRQHandler(void);

//=========================== public ==========================================

void ntw_init(ntw_receive_cbt ntw_receive_cb) {

    // reset variables
    memset(&ntw_vars,0x00,sizeof(ntw_vars_t));
    ntw_vars.socketId = 22; // default value
    memset(&ntw_dbg, 0x00,sizeof(ntw_dbg_t));

    // store params
    ntw_vars.ntw_receive_cb = ntw_receive_cb;

    // initialize the ipmt module
    dn_ipmt_init(
        dn_ipmt_notif_cb,                // notifCb
        ntw_vars.notifBuf,               // notifBuf
        sizeof(ntw_vars.notifBuf),       // notifBufLen
        dn_ipmt_reply_cb                 // replyCb
    );

    // initialize timer
    k_timer_init(&my_timer, RTC1_IRQHandler, NULL);

    // schedule the first event
    fsm_scheduleEvent(CMD_PERIOD, &api_getMoteStatus);
}

bool ntw_transmit(uint8_t* buf, uint8_t bufLen) {
    return api_sendTo(buf, bufLen);
}

//=========================== private =========================================

//=== fsm

void fsm_scheduleEvent(uint16_t delay, fsm_timer_callback cb) {
   
    // remember what function to call
    ntw_vars.fsmCb                     = cb;
    k_timer_start(&my_timer, K_MSEC(125), K_MSEC(125));
    
}

void fsm_cancelEvent(void) {
   k_timer_stop(&my_timer);
   
   // clear function to call
   ntw_vars.fsmCb                      = NULL;
}

void fsm_setCallback(fsm_reply_callback cb) {
   ntw_vars.replyCb                    = cb;
}

//=== ipmt

void dn_ipmt_notif_cb(uint8_t cmdId, uint8_t subCmdId) {
    dn_ipmt_events_nt*   dn_ipmt_events_notif;
    dn_ipmt_receive_nt*  dn_ipmt_receive_notif;

    switch (cmdId) {
      case CMDID_EVENTS:
   
          // debug
          ntw_dbg.num_notif_EVENTS++;

          // parse
          dn_ipmt_events_notif = (dn_ipmt_events_nt*)ntw_vars.notifBuf;

          // handle
          switch (dn_ipmt_events_notif->state) {
              case MOTE_STATE_IDLE:
                  ntw_vars.isOper = false;
                  fsm_scheduleEvent(CMD_PERIOD,api_getMoteStatus);
                  break;
              case MOTE_STATE_OPERATIONAL:
                  ntw_vars.isOper = true;
                  break;
              default:
                  // nothing to do
                  break;
          }
          break;
      case CMDID_RECEIVE:
          
          // debug
          ntw_dbg.num_notif_RECEIVE++;

          // parse
          dn_ipmt_receive_notif = (dn_ipmt_receive_nt*)ntw_vars.notifBuf;

          // notify app
          ntw_vars.ntw_receive_cb(
              dn_ipmt_receive_notif->payload,
              dn_ipmt_receive_notif->payloadLen
          );

      default:
         // nothing to do
         break;
    }
}

void dn_ipmt_reply_cb(uint8_t cmdId) {
   ntw_vars.replyCb();
}

//=== api

void api_response_timeout(void) {
   
   // issue cancel command
   dn_ipmt_cancelTx();
   
   // schedule first event
   fsm_scheduleEvent(CMD_PERIOD, &api_getMoteStatus);
}

// getMoteStatus

void api_getMoteStatus(void) {
   
   // debug
   ntw_dbg.num_calls_api_getMoteStatus++;

   // arm callback
   fsm_setCallback(api_getMoteStatus_reply);
   
   // issue function
   dn_ipmt_getParameter_moteStatus(
      (dn_ipmt_getParameter_moteStatus_rpt*)(ntw_vars.replyBuf)
   );

   // schedule timeout event
   fsm_scheduleEvent(SERIAL_RESPONSE_TIMEOUT, api_response_timeout);
}

void api_getMoteStatus_reply(void) {
   dn_ipmt_getParameter_moteStatus_rpt* reply;
   
   // debug
   ntw_dbg.num_calls_api_getMoteStatus_reply++;

   // cancel timeout
   fsm_cancelEvent();
   
   // parse reply
   reply = (dn_ipmt_getParameter_moteStatus_rpt*)ntw_vars.replyBuf;
   
   // choose next step
   switch (reply->state) {
      case MOTE_STATE_IDLE:
         ntw_vars.isOper = false;
         fsm_scheduleEvent(CMD_PERIOD, &api_openSocket);
         break;
      case MOTE_STATE_OPERATIONAL:
         ntw_vars.isOper = true;
         break;
      default:
         ntw_vars.isOper = false;
         fsm_scheduleEvent(CMD_PERIOD, api_getMoteStatus);
         break;
   }
}

// openSocket

void api_openSocket(void) {
  
   // debug
   ntw_dbg.num_calls_api_openSocket++;

   // arm callback
   fsm_setCallback(api_openSocket_reply);
   
   // issue function
   dn_ipmt_openSocket(
      0,                                              // protocol
      (dn_ipmt_openSocket_rpt*)(ntw_vars.replyBuf)    // reply
   );
   
   // schedule timeout event
   fsm_scheduleEvent(SERIAL_RESPONSE_TIMEOUT, api_response_timeout);
}

void api_openSocket_reply(void) {
   dn_ipmt_openSocket_rpt* reply;
   
   // debug
   ntw_dbg.num_calls_api_openSocket_reply++;

   // cancel timeout
   fsm_cancelEvent();
   
   // parse reply
   reply = (dn_ipmt_openSocket_rpt*)ntw_vars.replyBuf;
   
   // store the socketID
   ntw_vars.socketId = reply->socketId;
   
   // choose next step
   fsm_scheduleEvent(CMD_PERIOD, api_bindSocket);
}

// bindSocket

void api_bindSocket(void) {
   
   // debug
   ntw_dbg.num_calls_api_bindSocket++;

   // arm callback
   fsm_setCallback(api_bindSocket_reply);
   
   // issue function
   dn_ipmt_bindSocket(
      ntw_vars.socketId,                              // socketId
      SRC_PORT,                                       // port
      (dn_ipmt_bindSocket_rpt*)(ntw_vars.replyBuf)    // reply
   );

   // schedule timeout event
   fsm_scheduleEvent(SERIAL_RESPONSE_TIMEOUT, api_response_timeout);
}

void api_bindSocket_reply(void) {
   
   // debug
   ntw_dbg.num_calls_api_bindSocket_reply++;

   // cancel timeout
   fsm_cancelEvent();
   
   // choose next step
   fsm_scheduleEvent(CMD_PERIOD, api_join);
}

// join

void api_join(void) {
   
   // debug
   ntw_dbg.num_calls_api_join++;

   // arm callback
   fsm_setCallback(api_join_reply);
   
   // issue function
   dn_ipmt_join(
      (dn_ipmt_join_rpt*)(ntw_vars.replyBuf)     // reply
   );

   // schedule timeout event
   fsm_scheduleEvent(SERIAL_RESPONSE_TIMEOUT, api_response_timeout);
}

void api_join_reply(void) {

   // debug
   ntw_dbg.num_calls_api_join_reply++;

   // cancel timeout
   fsm_cancelEvent();
   
   // choose next step
   // no next step at this point. FSM will advance when we received a "joined"
   // notification
}

// sendTo

bool api_sendTo(uint8_t* buf, uint8_t bufLen) {
   bool     returnVal;
   uint8_t  dest_addr[16];
   dn_err_t dn_err;
   
   if (ntw_vars.isOper==true) {
      // mote is operational

      // debug
      ntw_dbg.num_calls_api_sendTo++;

      // arm callback
      fsm_setCallback(api_sendTo_reply);

      // populate address
      memcpy(dest_addr,ipv6Addr_manager,16);

      // issue function
      dn_err = dn_ipmt_sendTo(
        ntw_vars.socketId,                                   // socketId
        dest_addr,                                           // destIP
        DST_PORT,                                            // destPort
        SERVICE_TYPE_BW,                                     // serviceType
        0,                                                   // priority
        0xffff,                                              // packetId
        buf,                                                 // payload
        bufLen,                                              // payloadLen
        (dn_ipmt_sendTo_rpt*)(ntw_vars.replyBuf)             // reply
      );

      // prepare returnVal
      if (dn_err==DN_ERR_NONE) {
          returnVal = true;
      } else {
          returnVal = false;
      }

      // schedule timeout event
      fsm_scheduleEvent(SERIAL_RESPONSE_TIMEOUT, api_response_timeout);
   } else {
      // mote is NOT operational

      // cannot proceed
      returnVal = false;
   }

   return returnVal;
}

void api_sendTo_reply(void) {
   
   // debug
   ntw_dbg.num_calls_api_sendTo_reply++;

   // cancel timeout
   fsm_cancelEvent();
}

//=========================== interrupt handlers ==============================

void RTC1_IRQHandler(void) {

    // debug
    ntw_dbg.num_ISR_RTC1_IRQHandler++;
    ntw_vars.fsmCb();
}