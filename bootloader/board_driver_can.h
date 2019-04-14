
#ifndef _BOARD_DRIVER_CAN_H_
#define _BOARD_DRIVER_CAN_H_

#include <sam.h>
#include <stdint.h>
#include <stdlib.h>
#include "mcan.h"
#include "mcp_can_dfs.h"
#include "mcan_helper.h"

/* size of our custom Rx and Tx Buffer Elements, in words */
#define RAM_BUF_SIZE                  (MCAN_RAM_BUF_HDR_SIZE + 64u / 4)

#define RAM_ARRAY_SIZE_FILT_STD       (8u)
#define RAM_ARRAY_SIZE_FILT_EXT       (8u)
#define RAM_FIFO_SIZE_RX0             (12u)
/* no Rx FIFO 1 in our Message RAM */
#define RAM_ARRAY_SIZE_RX             (4u)
/* no Tx Event FIFO in our Message RAM */
#define RAM_ARRAY_SIZE_TX             (0u)
#define RAM_FIFO_SIZE_TX              (4u)


/* size of our custom Message RAM, in words */
#define MSG_RAM_SIZE      ( \
                            RAM_ARRAY_SIZE_FILT_STD * MCAN_RAM_FILT_STD_SIZE \
                            + RAM_ARRAY_SIZE_FILT_EXT * MCAN_RAM_FILT_EXT_SIZE \
                            + RAM_FIFO_SIZE_RX0 * RAM_BUF_SIZE \
                            + RAM_ARRAY_SIZE_RX * RAM_BUF_SIZE \
                            + RAM_ARRAY_SIZE_TX * RAM_BUF_SIZE \
                            + RAM_FIFO_SIZE_TX * RAM_BUF_SIZE )

#define MSG_LEN_1_CAN     8
#define MSG_LEN_1_CAN_FD  64
#define MSG_LEN_2_CAN     7
#define MSG_LEN_2_CAN_FD  48
#define MSG_ID_ALLOW_ALL_MASK     0x000ul     /* bits 0 & 1 are don't care */
#define RX_BUFFER_0       0
#define RX_BUFFER_1       1
#define FILTER_0          0
#define FILTER_1          1
#define CAN_EXT           1


uint8_t can_init(uint8_t idmodeset, uint32_t speedset, uint8_t clockset);
uint8_t can_read(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf);
uint8_t can_write(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);
uint8_t can_init_Mask(uint8_t num, uint32_t ulData);


#endif
