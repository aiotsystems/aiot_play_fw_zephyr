/*
Copyright (c) 2015, Dust Networks. All rights reserved.

Port of Zephyr for the AIOT Play.

\license See attached DN_LICENSE.txt.
*/

#include "../../dn_uart.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>


//=========================== defines =========================================
// In case we are using UART0:
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

//=========================== variables =======================================

typedef struct {
   dn_uart_rxByte_cbt   ipmt_uart_rxByte_cb;
} dn_uart_vars_t;

dn_uart_vars_t dn_uart_vars;
uint8_t rxByte;

// In case we are using UART0:
static const struct device *const uart_dev0 = DEVICE_DT_GET(UART_DEVICE_NODE);

//=========================== prototypes ======================================

//=========================== interrupt handlers ==============================

void isr_handler_uart0(){
   if (!uart_irq_update(uart_dev0)) {
      return;
   }
   // hand over byte to ipmt module
   while (uart_irq_rx_ready(uart_dev0)) {
      uart_fifo_read(uart_dev0, &rxByte, 1);
      dn_uart_vars.ipmt_uart_rxByte_cb(rxByte);
   }
}

//=========================== public ==========================================

void dn_uart_init(dn_uart_rxByte_cbt rxByte_cb){
   
   // call back function
   dn_uart_vars.ipmt_uart_rxByte_cb = rxByte_cb;
   // Define ISR for reception
   uart_irq_callback_user_data_set(uart_dev0, isr_handler_uart0, NULL);
   uart_irq_rx_enable(uart_dev0);
   if (device_is_ready(uart_dev0)) {
      printk("UART0 device ready!\n");
   }

}

void dn_uart_txByte(uint8_t byte){

  uart_poll_out(uart_dev0, byte);
}

void dn_uart_txFlush(){
   // nothing to do since nRF driver is byte-oriented
}

//=========================== private =========================================

//=========================== helpers =========================================

