/**
 * @file
 * @author     Scott Price <prices@hugllc.com>
 * @copyright  © 2017 Hunt Utilities Group, LLC
 * @brief      The main class for HUGnetCANLib.
 * @details
 */

#include "samc21_can.h"
/**
* @brief Checks for the presence of a connector id
*
* This function never returns if there is no connector id.
*
* @return void
*/
MCP_CAN::MCP_CAN(uint8_t _CS)
{
};
uint8_t MCP_CAN::begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset)
{
    return 0;
};
uint8_t MCP_CAN::init_Mask(uint8_t num, uint8_t ext, uint32_t ulData)
{
    return 0;
};              // Initilize Mask(s)
uint8_t MCP_CAN::init_Mask(uint8_t num, uint32_t ulData)
{
    return 0;
};                          // Initilize Mask(s)
uint8_t MCP_CAN::init_Filt(uint8_t num, uint8_t ext, uint32_t ulData)
{
    return 0;
};              // Initilize Filter(s)
uint8_t MCP_CAN::init_Filt(uint8_t num, uint32_t ulData)
{
    return 0;
}; // Initilize Filter(s)
uint8_t MCP_CAN::setMode(uint8_t opMode)
{
    return 0;
};                                        // Set operational mode
uint8_t MCP_CAN::sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
{
    return 0;
};      // Send message to transmit buffer
uint8_t MCP_CAN::sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf)
{
    return 0;
};                 // Send message to transmit buffer
uint8_t MCP_CAN::readMsgBuf(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf)
{
    return 0;
};   // Read message from receive buffer
uint8_t MCP_CAN::readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf)
{
    return CAN_NOMSG;
};               // Read message from receive buffer
uint8_t MCP_CAN::checkReceive(void)
{
    return 0;
};                                           // Check for received data
uint8_t MCP_CAN::checkError(void)
{
    return 0;
};                                             // Check for errors
uint8_t MCP_CAN::getError(void)
{
    return 0;
};                                               // Check for errors
uint8_t MCP_CAN::errorCountRX(void)
{
    return 0;
};                                           // Get error count
uint8_t MCP_CAN::errorCountTX(void)
{
    return 0;
};                                           // Get error count
uint8_t MCP_CAN::enOneShotTX(void)
{
    return 0;
};                                            // Enable one-shot transmission
uint8_t MCP_CAN::disOneShotTX(void)
{
    return 0;
};                                           // Disable one-shot transmission
