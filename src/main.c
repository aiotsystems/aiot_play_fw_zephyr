/*H**********************************************************************
* FILENAME :        main.c
*
* DESCRIPTION :
*       A Zephyr application using SmartMesh IP for networking and
*       Memfault for monitoring.
*
*
* NOTES :
*       This application is part of the OpenSwarm Project.
*
*       Copyright Siemens AG and Inria.  All rights reserved.
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
#include <stdbool.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <nrf52833.h>
#include "ntw.h"
#include "board.h"
#include "periodictimer.h"
#include <drivers/hwinfo.h>
#include <memfault_ncs.h>
#include <memfault/metrics/metrics.h>
#include <memfault/components.h>
#include "memfault/core/data_packetizer.h"

//=========================== defines =========================================

#define DATA_PERIOD_S 1

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    uint8_t        txCounter;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t       numReceive;
    uint32_t       numTransmit;
    uint32_t       numTransmit_success;
    uint32_t       numTransmit_fail;
} app_dbg_t;

app_dbg_t app_dbg;

//=========================== prototypes ======================================

void _periodtimer_cb(void);
void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen);
static void set_device_id(void);
static void memfault_init(void);
bool send_memfault_data_multi_part(void);

//=========================== main ============================================

int main(void) {

	  set_device_id();
  	memfault_init();
    
    // initialize variables
    memset(&app_vars,0x00,sizeof(app_vars));
    memset(&app_dbg, 0x00,sizeof(app_dbg));

    // bsp
    board_init();
    
    // ntw
    ntw_init(_ntw_receive_cb);

    // initialize the periodic timer
    periodictimer_init(
        DATA_PERIOD_S,       // period_s
        _periodtimer_cb      // periodtimer_cb
    );

    // main loop
    while(1) {
        // wait for event
        board_sleep();
    }
}

//=========================== private =========================================

void _periodtimer_cb(void) {
    bool success = 0;

    // increment
    app_vars.txCounter++;

	  // Memfault dummy value
	  memfault_metrics_heartbeat_set_unsigned(MEMFAULT_METRICS_KEY(testMetric), app_vars.txCounter);
	  
    // Export
    success = send_memfault_data_multi_part();

    // debug
    app_dbg.numTransmit++;
    if (success==true) {
        app_dbg.numTransmit_success++;
    } else {
        app_dbg.numTransmit_fail++;
    }
}

void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen) {
    
    // debug
    app_dbg.numReceive++;
}

static void set_device_id(void) {
  uint8_t dev_id[16];
  char dev_id_str[33];
  char *dev_str = "UNKNOWN";

  (void)memset(dev_id, 0x0, sizeof(dev_id));

  /* Obtain the device id */
  size_t length = hwinfo_get_device_id(dev_id, sizeof(dev_id));

  /* If this fails for some reason, use a fixed ID instead */
  if (length <= 0) {
    dev_str = CONFIG_SOC_SERIES "-test";
    length = strlen(dev_str);
  } else {
    /* Render the obtained serial number in hexadecimal representation */
    for (size_t i = 0; i < length; i++) {
      //sprintf(&dev_id_str[i * 2], "%02x", dev_id[i]);
    }
    dev_str = dev_id_str;
  }

  printk("Device ID: %s\n", dev_str);

  memfault_ncs_device_id_set(dev_str, length * 2);
}


static void memfault_init(void) {
  static uint8_t s_event_storage[1024];
  const sMemfaultEventStorageImpl *evt_storage =
    memfault_events_storage_boot(s_event_storage, sizeof(s_event_storage));

  // configure trace events to store into the buffer
  memfault_trace_event_boot(evt_storage);

  // record the current reboot reason
  memfault_reboot_tracking_collect_reset_info(evt_storage);

  // configure the metrics component to store into the buffer
  sMemfaultMetricBootInfo boot_info = {
    .unexpected_reboot_count = memfault_reboot_tracking_get_crash_count(),
  };
  memfault_metrics_boot(evt_storage, &boot_info);
}


bool send_memfault_data_multi_part(void) {
  const sPacketizerConfig cfg = {
    // Enable multi packet chunking. This means a chunk may span multiple calls to
    // memfault_packetizer_get_next().
    .enable_multi_packet_chunk = true,
  };

  sPacketizerMetadata metadata;
  bool data_available = memfault_packetizer_begin(&cfg, &metadata);
  if (!data_available) {
    // there are no more chunks to send
    MEMFAULT_LOG_INFO("All data has been sent!");
    return false;
  }

  // Note: metadata.single_chunk_message_length contains the entire length of the chunk
  //
  // This is the "Content-Length" for the HTTP POST request to the Memfault "chunks" endpoint
  //
  // When using HTTP directly on your device, this is where you would start the http request and
  // build the headers

  while (1) {
    uint8_t buffer[80];
    size_t read_size = sizeof(buffer);

    // We pass in the buffer to fill and the size of the buffer.
    // On return, read_size will be populated with how much data was actually written
    eMemfaultPacketizerStatus packetizer_status = memfault_packetizer_get_next(buffer, &read_size);
    if (packetizer_status == kMemfaultPacketizerStatus_NoMoreData) {
      // We know data is available from the memfault_packetizer_begin() call above
      // so _NoMoreData is an unexpected result
      MEMFAULT_LOG_ERROR("Unexpected packetizer status: %d", (int)packetizer_status);
      break;
    }

    // this is the call to a system specific function for sending data over the transport
    ntw_transmit(buffer, read_size);

    if (packetizer_status == kMemfaultPacketizerStatus_EndOfChunk) {
      // we have reached the end of the chunk
      break;
    }
  }

  return true;
}