#include "i2c.h"

void *i2c_handle = NULL;
uint8_t i2c_rbuffer[128] = {};
uint8_t i2c_wbuffer[128] = {};
I2C_CB_Status_t CB_Parameter = I2C_CB_IDLE;
// Transfer completed

void i2c_callback (const uint32_t status, void *cb_para)  //  uint32
{
	CB_Parameter = I2C_CB_IDLE;
	gizLog(LOG_INFO,"[I2C] i2c_callback,CB_Parameter = %d\r\n", CB_Parameter);
 	//qt_uart_dbg(uart_conf.hdlr,"i2c_callback: status[%d] \r\n", status);
}

/******************************************** 
** <S>  - START bit
** <P>  - STOP  bit
** <Sr> - Repeat Start bit
** <A>  - Acknowledge bit
** <N>  - Not-Acknowledge bit
** <R>  - Read Transfer
** <W> - Write Transfer
********************************************/
qapi_Status_t i2c_write_reg(uint8_t device_Address, uint16_t reg, uint8_t data, I2C_REG_TYPE_t reg_type)		//写寄存器
{
	qapi_Status_t res = QAPI_OK;
	static qapi_I2CM_Descriptor_t desc[2];	/* 必须静态 */
	qapi_I2CM_Config_t config;				/* 绝对不能静态 */
	static unsigned int transferred1 = 0;	/* 必须静态 */

	gizLog(LOG_INFO,"[I2C] i2c_write_reg: start. CB: %d\r\n", CB_Parameter);

	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = 50; 
	config.slave_Address	 = device_Address;
	config.SMBUS_Mode		 = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;

	if(reg_type == I2C_REG_16BIT)
	{
		i2c_wbuffer[0] = (uint8)(reg>>8);
		i2c_wbuffer[1] = (uint8)reg;
		i2c_wbuffer[2] = data;
		desc[0].length = 3;
	}
	else
	{
		i2c_wbuffer[0] = (uint8)reg;
		i2c_wbuffer[1] = data;
		desc[0].length = 2;
	}
	
	desc[0].buffer		= i2c_wbuffer;
//	desc[0].length		= 2;
	desc[0].transferred = (uint32)&transferred1;
	desc[0].flags		= QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
	
	//qt_uart_dbg(uart_conf.hdlr,"[I2C] qapi_I2CM_Transfer: start \r\n");
	gizLog(LOG_INFO,"[I2C] qapi_I2CM_Transfer: start \r\n");
	res = qapi_I2CM_Transfer(i2c_handle, &config, desc, 1, i2c_callback, &CB_Parameter, 100);

	if(res == QAPI_OK)
	{
		CB_Parameter = I2C_CB_BUSY;
	}

	return res;
}

qapi_Status_t i2c_write_reg_block(uint8_t device_Address, uint16_t reg, uint8_t data, I2C_REG_TYPE_t reg_type)		//写寄存器 + 判断传输是否完成
{
	qapi_Status_t res = QAPI_ERROR;
	int cnt;
	res = i2c_write_reg(device_Address, reg, data,reg_type);

	if(res == QAPI_OK)
	{
		 /* Block to check the callback parameter, max 500ms */
		for(cnt=0;cnt<500;cnt++)
		{
			if(CB_Parameter == I2C_CB_IDLE)		//判断是否进入回调；（即传输是否完成）
			{
				gizLog(LOG_INFO,"QAPI_OK...\r\n");
				return QAPI_OK;
			}
			qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_MSEC, true);
		}
	}
	return QAPI_ERROR;
}

#if 1
qapi_Status_t i2c_write_mem(uint8_t device_Address, uint8_t len, uint8_t *reg_buff)				
{
	qapi_Status_t res = QAPI_OK;
	static qapi_I2CM_Descriptor_t desc[2];	/* 必须静态 */
	qapi_I2CM_Config_t config;				/* 绝对不能静态 */
	static unsigned int transferred1 = 0;	/* 必须静态 */
	int i=0;

	//gizLog(LOG_INFO,"[I2C] i2c_write_mem: start. CB: %d\r\n", CB_Parameter);

	if(len >= 127)
	{
		return QAPI_ERROR;
	}
	else
	{
		for(i = 0;i < len;i++)
		{
			i2c_wbuffer[i] = reg_buff[i];
			gizLog(LOG_INFO,"[I2C] i2c_write_mem: i2c_wbuffer[0] = 0x%x\r\n", i2c_wbuffer[0]);
		}
			
	}
	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = 50; 
	config.slave_Address	 = device_Address;
	config.SMBUS_Mode		 = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;
		
	// Write transfer for one reg, one data
	// <S><slave_address><W><A><data1><A><data2><A>...<dataN><A><P>
	desc[0].buffer		= i2c_wbuffer;
	desc[0].length		= len;
	desc[0].transferred = (uint32)&transferred1;
	desc[0].flags		= QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;

	res = qapi_I2CM_Transfer(i2c_handle, &config, desc, 1, i2c_callback, &CB_Parameter, 100);
	if(res == QAPI_OK)
	{
		CB_Parameter = I2C_CB_BUSY;
	}
	return res;
}
#endif

#if 0
qapi_Status_t i2c_write_mem(uint8_t device_Address, uint8_t* reg_buff, I2C_REG_TYPE_t reg_type)		//写寄存器
{
	qapi_Status_t res = QAPI_OK;
	static qapi_I2CM_Descriptor_t desc[2];	/* 必须静态 */
	qapi_I2CM_Config_t config;				/* 绝对不能静态 */
	static unsigned int transferred1 = 0;	/* 必须静态 */

	gizLog(LOG_INFO,"[I2C] i2c_write_reg: start. CB: %d\r\n", CB_Parameter);

	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = 100; 
	config.slave_Address	 = device_Address;
	config.SMBUS_Mode		 = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;

	if(reg_type == I2C_REG_16BIT)
	{
		//i2c_wbuffer[0] = (uint8)(reg>>8);
		//i2c_wbuffer[1] = (uint8)reg;

		i2c_wbuffer[0] = reg_buff[0];
		i2c_wbuffer[1] = reg_buff[1];
		desc[0].length = 2;
	}
	else
	{
		i2c_wbuffer[0] = reg_buff[0];
		desc[0].length = 1;
	}
	
	desc[0].buffer		= i2c_wbuffer;
//	desc[0].length		= 2;
	desc[0].transferred = (uint32)&transferred1;
	desc[0].flags		= QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
	
	//qt_uart_dbg(uart_conf.hdlr,"[I2C] qapi_I2CM_Transfer: start \r\n");
	gizLog(LOG_INFO,"[I2C] qapi_I2CM_Transfer: start,  i2c_wbuffer[0] = \r\n");
	res = qapi_I2CM_Transfer(i2c_handle, &config, desc, 1, i2c_callback, &CB_Parameter, 100);

	if(res == QAPI_OK)
	{
		CB_Parameter = I2C_CB_BUSY;
	}

	return res;
}
#endif

qapi_Status_t i2c_write_mem_block(uint8_t device_Address,uint8_t len, uint8_t *reg_buff)			
{
	qapi_Status_t res = QAPI_ERROR;
	int cnt;
	res = i2c_write_mem(device_Address, len, reg_buff);
	//res = i2c_write_mem(device_Address, reg_buff, I2C_REG_8BIT);
	//gizLog(LOG_INFO,"[I2C] i2c_write_mem, res = %d\r\n",res);		//加句log，导致卡死，res = 0；
	if(res == QAPI_OK)
	{
		 /* Block to check the callback parameter, max 500ms */
		for(cnt=0;cnt<500;cnt++)									//500ms延时，也不能进入回调
		{
			if(CB_Parameter == I2C_CB_IDLE)				
			{
				gizLog(LOG_INFO,"i2c_write_mem_block OK...\r\n");
				return QAPI_OK;
			}
			qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_MSEC, true);
		}
	}
	return QAPI_ERROR;
}

qapi_Status_t i2c_read_mem(uint8_t device_Address, uint8_t len, uint8_t *buff)						//读内存
{
	qapi_Status_t res = QAPI_OK;
	static qapi_I2CM_Descriptor_t desc[2];	/* 必须静态 */
	qapi_I2CM_Config_t config;				/* 绝对不能静态 */
	static unsigned int transferred1 = 0;	/* 必须静态 */

	gizLog(LOG_INFO,"i2c_read: slave_addr[0x%x] \r\n", device_Address);

	// Configure the bus speed and slave address
	config.bus_Frequency_KHz = 50; 
	config.slave_Address	 = device_Address;
	config.SMBUS_Mode		 = 0;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;

	if(len >= 127)
	{
		return QAPI_ERROR;
	}

	// Read N bytes from a register 0x01 on a sensor device
	// <S><slave_address><W><A><0x01><A><S><slave_address><R><A>
	//					   <data1><A><data2><A>...<dataN><N><P>
	desc[0].buffer	   = buff;
	desc[0].length	   = len;
	desc[0].transferred = (uint32)&transferred1;
	desc[0].flags	   = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ | QAPI_I2C_FLAG_STOP;


	//gizLog(LOG_INFO,"in i2c_read_mem,out:desc[0].buffer = %x\r\n",desc[0].buffer[0]);
	res = qapi_I2CM_Transfer(i2c_handle, &config, desc, 1, i2c_callback, &CB_Parameter, 100);

	if(res == QAPI_OK)
	{
		CB_Parameter = I2C_CB_BUSY;
	}
	return res;
}

qapi_Status_t i2c_read_mem_block(uint8_t device_Address, uint8_t len, uint8_t *buff)
{
	qapi_Status_t res = QAPI_ERROR;
	int cnt;
	res = i2c_read_mem(device_Address, len, buff);
	//gizLog(LOG_INFO,"[I2C] i2c_read_mem, res = %d\r\n",res);	
	if(res == QAPI_OK)
	{
		 /* Block to check the callback parameter, max 500ms */
		for(cnt=0;cnt<500;cnt++)
		{
			if(CB_Parameter == I2C_CB_IDLE)				
			{
				gizLog(LOG_INFO,"i2c_write_mem_block OK...\r\n");
				return QAPI_OK;
			}
			qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_MSEC, true);
		}
	}
	return QAPI_ERROR;
}

qapi_Status_t i2c_read_reg_block(uint8_t device_Address, uint16_t reg, uint8_t *buff, I2C_REG_TYPE_t reg_type)				//读内存 + 判断传输是否完成
{
	//gizLog(LOG_INFO,"in i2c_read_reg_block start...\r\n");
	qapi_Status_t res = QAPI_ERROR;
	static uint8_t r_reg[2];
	int cnt;

	if(reg_type == I2C_REG_16BIT)
	{
		r_reg[0] = (uint8)(reg>>8);
		r_reg[1] = (uint8)reg;

		//res = i2c_write_mem(device_Address, 2, r_reg);
		res = i2c_write_mem_block(device_Address, 2, r_reg);
	}
	else
	{
		r_reg[0] = (uint8)reg;

		gizLog(LOG_INFO,"in i2c_read_reg_block, r_reg[0] = 0x%x\r\n",r_reg[0]);
		//res = i2c_write_mem(device_Address, 1, r_reg);
		res = i2c_write_mem_block(device_Address, 1, r_reg);
		//gizLog(LOG_INFO,"after i2c_read_reg_block ,res = %d\r\n",res);
	}

	if(res == QAPI_OK)				//调用i2c_write_mem_block返回结果判断
	{
		if(r_reg[0] == 0x31)		//读取中断
		{
			//gizLog(LOG_INFO,"before i2c_read_mem ,case 0x31...\r\n");
			//res = i2c_read_mem(device_Address, 1, buff);
			res = i2c_read_mem_block(device_Address, 1, buff);
			gizLog(LOG_INFO,"after i2c_read_mem, read 1 byte, buff[0] = %#x, res = %d...\r\n", buff[0], res);
		}
		else						//读取x,y,z三轴数据
		{
			//gizLog(LOG_INFO,"before i2c_read_mem\r\n");
			//res = i2c_read_mem(device_Address, 6, buff);	//读取6字节数据
			res = i2c_read_mem_block(device_Address, 6, buff);
			gizLog(LOG_INFO,"after i2c_read_mem, read 6 bytes, buff[0] = %#x, res = %d...\r\n", buff[0], res);
		}
	}
	else
	{
		gizLog(LOG_INFO,"i2c_write_mem_block error,res = %d\r\n",res);
		return QAPI_ERROR;
	}
	
	if(res == QAPI_OK)			//调用i2c_read_mem_block返回结果判断
	{
		return QAPI_OK;
	}
	else
	{
		gizLog(LOG_INFO,"i2c_read_mem_block error,res = %d\r\n",res);
		return QAPI_ERROR;
	}
	
}

int i2c_init(void)
{
	qapi_Status_t res = QAPI_ERROR;
	res = qapi_I2CM_Open(QAPI_I2CM_INSTANCE_004_E, &i2c_handle);
	gizLog(LOG_INFO,"[I2C] qapi_I2CM_Open: res = %d \r\n", res);
	qapi_I2CM_Power_On(i2c_handle);
	qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
	return 0;
}


