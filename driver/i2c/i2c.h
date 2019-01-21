#ifndef __I2C_H__
#define __I2C_H__

#include "stdio.h"
#include "txm_module.h"
#include "qapi_diag.h"
#include "qapi_timer.h"
#include "qapi_uart.h"
//#include "quectel_utils.h"
//#include "quectel_uart_apis.h"
#include "qapi_fs_types.h"
#include "qapi_fs.h"
#include "../../main/log.h"
#include "qapi_i2c_master.h"

#define GPIO_BYTE_POOL_SIZE		16*8*1024

typedef enum
{
	I2C_REG_8BIT = 0,
	I2C_REG_16BIT
}I2C_REG_TYPE_t;

typedef enum
{
	I2C_CB_IDLE = 0,
	I2C_CB_BUSY
}I2C_CB_Status_t;



qapi_Status_t i2c_write_reg(uint8 device_Address, uint16 reg, uint8 data, I2C_REG_TYPE_t reg_type);
qapi_Status_t i2c_write_reg_block(uint8 device_Address, uint16 reg, uint8 data, I2C_REG_TYPE_t reg_type);
qapi_Status_t i2c_write_mem(uint8 device_Address, uint8 len, uint8 *data_buff);
//qapi_Status_t i2c_write_mem(uint8_t device_Address, uint8_t* data_buff, I2C_REG_TYPE_t reg_type);
qapi_Status_t i2c_write_mem_block(uint8 device_Address,uint8 len, uint8 *data_buff);
qapi_Status_t i2c_read_mem (uint8 device_Address, uint8 len, uint8 *buff);
qapi_Status_t i2c_read_mem_block(uint8_t device_Address, uint8_t len, uint8_t *buff);
qapi_Status_t i2c_read_reg_block(uint8 device_Address, uint16 reg, uint8 *buff, I2C_REG_TYPE_t reg_type);
int i2c_init(void);

extern int i2c_module_cb_manager(UINT cb_id, void *app_cb, 
									 UINT cb_param1, UINT cb_param2, 
									 UINT cb_param3, UINT cb_param4,
									 UINT cb_param5, UINT cb_param6,
									 UINT cb_param7, UINT cb_param8);

extern void *i2c_handle;

#endif /* __EXAMPLE_I2C_H__ */
