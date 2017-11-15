/**
 * @file
 * @author     Scott Price <prices@hugllc.com>
 * @copyright  © 2017 Hunt Utilities Group, LLC
 * @brief      The main class for HUGnetCANLib.
 * @details
 */

#include "samc21_can.h"
#include "Arduino.h"

MCP_CAN *use_object;


/**
* @brief Checks for the presence of a connector id
*
* This function never returns if there is no connector id.
*
* @return void
*/
MCP_CAN::MCP_CAN(uint8_t _CS)
: rx_ded_buffer_data(false)
{
    use_object = this;
};

uint8_t MCP_CAN::begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset)
{
    uint8_t ret;
    const struct mcan_config mcan_cfg = {

        id : ID_CAN0,
        regs : CAN0,
        msg_ram : mcan_msg_ram,

        array_size_filt_std : RAM_ARRAY_SIZE_FILT_STD,
        array_size_filt_ext : RAM_ARRAY_SIZE_FILT_EXT,
        fifo_size_rx0 : RAM_FIFO_SIZE_RX0,
        fifo_size_rx1 : 0,
        array_size_rx : RAM_ARRAY_SIZE_RX,
        fifo_size_tx_evt : 0,
        array_size_tx : RAM_ARRAY_SIZE_TX,
        fifo_size_tx : RAM_FIFO_SIZE_TX,

        buf_size_rx_fifo0 : MAX_CHAR_IN_MESSAGE,
        buf_size_rx_fifo1 : 0,
        buf_size_rx : MAX_CHAR_IN_MESSAGE,
        buf_size_tx : MAX_CHAR_IN_MESSAGE,

        /*
        using values from AT6493 (SAMC21 app note); the plus values are to add on what the MCAN driver subtracts back off
        */
        bit_rate : 125000,
        quanta_before_sp : 10 + 2,
        quanta_after_sp : 3 + 1,

        /*
        AT6493 (SAMC21 app note) 'fast' values were unhelpfully the same as normal speed; these are for double (1MBit)
                the maximum peripheral clock of 48MHz on the SAMC21 does restrict us from very high rates
        */
        bit_rate_fd : 125000,
        quanta_before_sp_fd : 10 + 2,
        quanta_after_sp_fd : 3 + 1,

        quanta_sync_jump : 3 + 1,
        quanta_sync_jump_fd : 3 + 1,
    };
    
    
    switch (mcan_cfg.id)
    {
    case ID_CAN0:
        PORT->Group[0].DIRSET.reg = PORT_PA24;
        PORT->Group[0].DIRCLR.reg = PORT_PA25;
        PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
        PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
        PORT->Group[0].PMUX[24 / 2].reg = PORT_PMUX_PMUXE(6 /* CAN0 G */) | PORT_PMUX_PMUXO(6 /* CAN0 G */); /* have to write odd and even at once */

        GCLK->PCHCTRL[CAN0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
        MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN0;

        NVIC_EnableIRQ(CAN0_IRQn);
        break;
    }
    
    if (mcan_configure_msg_ram(&mcan_cfg, &mcan_msg_ram_size)) {
        Serial.println("RAM configuration succeeded");
    } else {
        return CAN_FAIL;
    }
    ret = mcan_initialize(&mcan, &mcan_cfg);
    if (ret == 0) {
        Serial.println("CAN initialized");
    } else {
        Serial.print("CAN init failed, code ");
        Serial.println(ret);
        return CAN_FAIL;
    }
    //mcan_set_tx_queue_mode(&mcan);
    mcan_loopback_off(&mcan);
    mcan_set_mode(&mcan, MCAN_MODE_CAN);
    mcan_enable(&mcan);

    
    if (mcan_is_enabled(&mcan)) {
        Serial.println("MCAN is enabled!");
        return CAN_OK;
    }

    Serial.println("Something went wrong!!!!!!!!");
    return CAN_FAIL;
    
    
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
    return MCP_NORMAL;
};                                        // Set operational mode

uint8_t MCP_CAN::sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
{
    uint8_t ret = 0xff;
    if (!mcan_is_enabled(&mcan)) {
        Serial.println("CAN not enabled!");
        return CAN_FAILTX;
    }
    /*
    txbuf = mcan_prepare_tx_buffer(&mcan, 0, id, len);
    if (txbuf != NULL) {
        memcpy(txbuf, buf, len);
        mcan_send_tx_buffer(&mcan, 0);
        ret = 0;
    }
    */
    if (ext) {
        id |= CAN_EXT_MSG_ID;
    }
    ret = mcan_enqueue_outgoing_msg(&mcan, id, len, buf);
    Serial.print("Sending id: 0x");
    Serial.print(id, HEX);
    Serial.print("  len:");
    Serial.print(len);
    Serial.print("  ret:");
    Serial.println(ret);
    if (ret != 0xFF) {
        return CAN_OK;
    }
    return CAN_FAILTX;
};      // Send message to transmit buffer
uint8_t MCP_CAN::sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf)
{
    return 0;
};                 // Send message to transmit buffer
uint8_t MCP_CAN::readMsgBuf(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf)
{
    uint8_t msg_data[64];
    struct mcan_msg_info msg;
    msg.data = msg_data;
    bool got_data;
    if (mcan_is_tx_complete(&mcan)) {
        mcan_clear_tx_flag(&mcan);
        Serial.println("Clearing TX Complete Flag");
    }
    __disable_irq();
    if ((got_data = rx_ded_buffer_data))
        rx_ded_buffer_data = false;
    __enable_irq();
    
    if (got_data) {
        mcan_read_rx_buffer(&mcan, RX_BUFFER_0, &msg);
        *id = mcan_get_id(msg.id);
        *len = msg.data_len;
        memcpy(buf, msg.data, *len);
        return CAN_OK;
    }
    return CAN_NOMSG;
};   // Read message from receive buffer
uint8_t MCP_CAN::readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf)
{
    uint8_t ext;
    return readMsgBuf(id, &ext, len, buf);
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


void CAN0_Handler(void)
{
    if (mcan_rx_array_data(&(use_object->mcan))) {
        mcan_clear_rx_array_flag(&(use_object->mcan));
        use_object->rx_ded_buffer_data = true;
        Serial.println("Got a Packet 0!");
    }
}


void CAN1_Handler(void)
{
    if (mcan_rx_array_data(&(use_object->mcan))) {
        mcan_clear_rx_array_flag(&(use_object->mcan));
        use_object->rx_ded_buffer_data = true;
        Serial.println("Got a Packet 1!");
    }
}

