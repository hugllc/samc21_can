#include "board_driver_can.h"


struct mcan_set mcan;
volatile bool rx_ded_buffer_data;
uint32_t mcan_msg_ram[MSG_RAM_SIZE] __attribute__((aligned(4)));
uint32_t mcan_msg_ram_size = ARRAY_SIZE(mcan_msg_ram);
uint8_t _idmode;
uint8_t _mode;
uint8_t _cs;


uint8_t can_init(uint8_t idmodeset, uint32_t speedset, uint8_t clockset)
{
    uint8_t ret;
    _idmode = idmodeset;
    const struct mcan_config mcan_cfg = {

id :
        ID_CAN0,
regs :
        CAN0,
msg_ram :
        mcan_msg_ram,

array_size_filt_std :
        RAM_ARRAY_SIZE_FILT_STD,
array_size_filt_ext :
        RAM_ARRAY_SIZE_FILT_EXT,
fifo_size_rx0 :
        RAM_FIFO_SIZE_RX0,
        fifo_size_rx1 : 0,
array_size_rx :
        RAM_ARRAY_SIZE_RX,
        fifo_size_tx_evt : 0,
array_size_tx :
        RAM_ARRAY_SIZE_TX,
fifo_size_tx :
        RAM_FIFO_SIZE_TX,

        buf_size_rx_fifo0 : 64,
        buf_size_rx_fifo1 : 0,
        buf_size_rx : 64,
        buf_size_tx : 64,

        /*
        using values from AT6493 (SAMC21 app note); the plus values are to add on what the MCAN driver subtracts back off
        */
bit_rate :
        speedset,
        quanta_before_sp : 10 + 2,
        quanta_after_sp : 3 + 1,

        /*
        AT6493 (SAMC21 app note) 'fast' values were unhelpfully the same as normal speed; these are for double (1MBit)
                the maximum peripheral clock of 48MHz on the SAMC21 does restrict us from very high rates
        */
bit_rate_fd :
        speedset,
        quanta_before_sp_fd : 10 + 2,
        quanta_after_sp_fd : 3 + 1,

        quanta_sync_jump : 3 + 1,
        quanta_sync_jump_fd : 3 + 1,
    };
    switch (mcan_cfg.id) {
        case ID_CAN0:
            PORT->Group[0].DIRSET.reg = PORT_PA24;
            PORT->Group[0].DIRCLR.reg = PORT_PA25;
            PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
            PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
            PORT->Group[0].PMUX[24 / 2].reg = PORT_PMUX_PMUXE(6 /* CAN0 G */) | PORT_PMUX_PMUXO(6 /* CAN0 G */); /* have to write odd and even at once */
            GCLK->PCHCTRL[CAN0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
            MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN0;
            //NVIC_EnableIRQ(CAN0_IRQn);
            break;
        default:
            return CAN_FAIL;
    }
    if (mcan_configure_msg_ram(&mcan_cfg, &mcan_msg_ram_size)) {
        //Serial.println("RAM configuration succeeded");
    } else {
        return CAN_FAIL;
    }
    ret = mcan_initialize(&mcan, &mcan_cfg);
    if (ret == 0) {
        //Serial.println("CAN initialized");
    } else {
        //Serial.print("CAN init failed, code ");
        //Serial.println(ret);
        return CAN_FAIL;
    }
    mcan_set_tx_queue_mode(&mcan);
    if (_mode == MCP_LOOPBACK) {
        mcan_loopback_on(&mcan);
    } else {
        mcan_loopback_off(&mcan);
    }
    mcan_set_mode(&mcan, MCAN_MODE_CAN);
    mcan_enable(&mcan);
    
    // Enable chip standby
    //pinMode(_cs, OUTPUT);
    //digitalWrite(_cs, LOW);
    
    //mcan_enable_rx_array_flag(&mcan, 0);
    // MCP_ANY means filters don't matter
    if (_idmode == MCP_ANY) {
        can_init_Mask(FILTER_0, (CAN_EXT_MSG_ID | MSG_ID_ALLOW_ALL_MASK));
        can_init_Mask(FILTER_1, (CAN_STD_MSG_ID | MSG_ID_ALLOW_ALL_MASK));
    }
    if (mcan_is_enabled(&mcan)) {
        //Serial.println("MCAN is enabled!");
        return CAN_OK;
    }
    //Serial.println("Something went wrong!!!!!!!!");
    return CAN_FAIL;
};

uint8_t can_read(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf)
{
    struct mcan_msg_info msg;
    msg.data = buf;
    msg.data_len = 8;
    uint8_t fifo_entries;
    if (mcan_is_tx_complete(&mcan)) {
        mcan_clear_tx_flag(&mcan);
    }
    fifo_entries = mcan_dequeue_received_msg(&mcan, 0, &msg);
    if (fifo_entries > 0) {
        *id = mcan_get_id(msg.id);
        *len = msg.data_len;
        *ext = (msg.id & CAN_EXT_MSG_ID) == CAN_EXT_MSG_ID;
        return CAN_OK;
    }
    return CAN_NOMSG;
};   // Read message from receive buffer


uint8_t can_write(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
{
    uint8_t ret = 0xff;
    if (!mcan_is_enabled(&mcan)) {
        return CAN_CTRLERROR;
    }
    if (ext) {
        id |= CAN_EXT_MSG_ID;
    }
    ret = mcan_enqueue_outgoing_msg(&mcan, id, len, buf);
    if (ret != 0xFF) {
        return CAN_OK;
    }
    return CAN_FAILTX;
};      // Send message to transmit buffer


uint8_t can_init_Mask(uint8_t num, uint32_t ulData)
{
    uint32_t id;
    if (CAN_EXT) {
        id = 0x1fffffff;
        ulData &= id;
        id |= CAN_EXT_MSG_ID;
        if (CAN_EXT >= mcan.cfg.array_size_filt_ext) {
            return MCP2515_FAIL;
        }
    } else {
        id = 0x7ff;
        ulData &= id;
        id |= CAN_STD_MSG_ID;
        if (CAN_EXT >= mcan.cfg.array_size_filt_std) {
            return MCP2515_FAIL;
        }
    }
    mcan_filter_id_mask(&mcan, 0, num, id, ulData);
    return MCP2515_OK;
};              // Initilize Mask(s)
