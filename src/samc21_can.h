/**
 * @file
 * @author     Scott Price <prices@hugllc.com>
 * @copyright  © 2017 Hunt Utilities Group, LLC
 * @brief      The include file for using the SAMC
 * @details
 */
#ifndef _SAMC_CAN_H_
#define _SAMC_CAN_H_

#include "mcp_can_dfs.h"
#define MAX_CHAR_IN_MESSAGE CAN_MAX_CHAR_IN_MESSAGE

#include <inttypes.h>

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t ext;
    uint8_t buffer[MAX_CHAR_IN_MESSAGE];
    uint8_t ret;
} mcp_can_buf;

class MCP_CAN
{

public:
    /**
    * @brief Checks for the presence of a connector id
    *
    * This function never returns if there is no connector id.
    *
    * @return void
    */
    MCP_CAN(uint8_t _CS);
    uint8_t begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset);
    uint8_t init_Mask(uint8_t num, uint8_t ext, uint32_t ulData);              // Initilize Mask(s)
    uint8_t init_Mask(uint8_t num, uint32_t ulData);                          // Initilize Mask(s)
    uint8_t init_Filt(uint8_t num, uint8_t ext, uint32_t ulData);              // Initilize Filter(s)
    uint8_t init_Filt(uint8_t num, uint32_t ulData); // Initilize Filter(s)
    uint8_t setMode(uint8_t opMode);                                        // Set operational mode
    uint8_t sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);      // Send message to transmit buffer
    uint8_t sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf);
    uint8_t readMsgBuf(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf);
    uint8_t readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf);
    uint8_t checkReceive(void);
    uint8_t checkError(void);
    uint8_t getError(void);
    uint8_t errorCountRX(void);
    uint8_t errorCountTX(void);
    uint8_t enOneShotTX(void);
    uint8_t disOneShotTX(void);
private:

};


#endif //define _SAMC_CAN_H_