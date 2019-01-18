#include "spi.h"
#include "../../main/log.h"
#include "qapi_spi_master.h"

qapi_SPIM_Config_t spi_config;
void *spi_hdl = NULL;

/*spi callback func*/
uint32 cb_para = 0;

struct spi_status_t {
    uint32 status;
    uint8 *read_addr;
    uint8 len;
} spi_status;

static uint8 rx_buf[128], tx_buf[128];


/**************************************************************************
*                                 FUNCTION
***************************************************************************/
void qapi_spi_cb_func(uint32 status, void *cb_para)
{
    if (QAPI_SPI_COMPLETE == status)
    {//The transaction is complete.
        //gizLog(LOG_INFO,"[==spim_cb_func==]: transfer success, status: %d, cb_p: %d", status, *((int*)cb_para));
    }
    else if (QAPI_SPI_QUEUED == status || QAPI_SPI_IN_PROGRESS == status)
    {//The transaction is processing.
        //gizLog(LOG_INFO,"[==spim_cb_func==]: transfer in progress, status: %d, cb_p: %d", status, *((int*)cb_para));
    }
    else
    {//An error occured in the transaction.
        //gizLog(LOG_INFO,"[==spim_cb_func==]: transfer failed, status: %d, cb_p: %d", status, *((int*)cb_para));
    }
    spi_status.status = status;
}

void spi_init(void)
{
    qapi_SPIM_Open(QAPI_SPIM_INSTANCE_6_E, &spi_hdl); 
    qapi_SPIM_Power_On(spi_hdl);

    //spi interface config
    spi_config.SPIM_Mode = QAPI_SPIM_MODE_3_E;                      // set the spi mode, determined by slave device
    spi_config.SPIM_CS_Polarity = QAPI_SPIM_CS_ACTIVE_LOW_E;        // set CS low as active, determined by slave device
    spi_config.SPIM_endianness  = SPI_LITTLE_ENDIAN;
    spi_config.SPIM_Bits_Per_Word = 8;
    spi_config.SPIM_Slave_Index = 0;
    spi_config.Clk_Freq_Hz = 1000000;                               //config spi clk about 1Mhz
    spi_config.SPIM_CS_Mode = QAPI_SPIM_CS_KEEP_ASSERTED_E;
    spi_config.CS_Clk_Delay_Cycles = 0;                             // don't care, set 0 is ok.
    spi_config.Inter_Word_Delay_Cycles = 0;                         // don't care, set 0 is ok.
    spi_config.loopback_Mode = 0;
}

uint8 *spi_reg_read(uint8 reg)
{
    qapi_Status_t res = QAPI_OK; 
    qapi_SPIM_Descriptor_t spi_desc;
    int num;

    tx_buf[0] = reg | 0x80 & 0xbf;
    tx_buf[1] = 0xff;

    rx_buf [0] = 0;
    rx_buf [1] = 0;

    spi_desc.rx_buf = rx_buf;
    spi_desc.tx_buf = tx_buf;
    spi_desc.len = 2;
    spi_status.status = 0;
    spi_status.read_addr = NULL;
    spi_status.len = 1;

    res = qapi_SPIM_Full_Duplex(spi_hdl, &spi_config, &spi_desc, 1, qapi_spi_cb_func, &spi_status, false);

    gizLog(LOG_INFO,"read spi end\n");

    if (res != QAPI_OK)
        return NULL;

    return rx_buf + 1;
}

qapi_Status_t spi_reg_write(uint8 reg, uint8 data)
{
    qapi_Status_t res = QAPI_OK; 
    qapi_SPIM_Descriptor_t spi_desc;
    int num;

    tx_buf[0] = reg & 0x3f;
    tx_buf[1] = data;

    spi_desc.rx_buf = rx_buf;
    spi_desc.tx_buf = tx_buf;
    spi_desc.len = 2;
    spi_status.status = 0;
    spi_status.read_addr = NULL;
    spi_status.len = 1;

    res = qapi_SPIM_Full_Duplex(spi_hdl, &spi_config, &spi_desc, 1, qapi_spi_cb_func, &spi_status, false);

    if (res != QAPI_OK)
        return res;

    for(num = 0; num < 100; num++) {
        gizLog(LOG_INFO,"num = %d\n", num);
        if (spi_status.status == QAPI_SPI_COMPLETE) {
            gizLog(LOG_INFO,"spi write reg(0x%02x) = 0x%02x\n", reg, data);
            return QAPI_OK;
        } else if (spi_status.status != 0) {
            gizLog(LOG_INFO,"spi write err\n");
            return QAPI_ERROR;
        }
        qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, true);
    }

    return QAPI_ERR_TIMEOUT;
}

uint32 spi_check_transfer_status(void)
{
    return spi_status.status;
}