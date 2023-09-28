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

/**
\}
\}
*/

#endif