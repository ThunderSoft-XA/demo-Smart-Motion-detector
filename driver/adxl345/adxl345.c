#include "adxl345.h"
#include "../spi/spi.h"
#include "../gpio/gpio.h"

#define SLAVE_ADDR	 		0x53
#define DATA_ADDR 			0X32
#define INT_ADDR 			0x30
#define DEVICE_ADDR 		0x00


void adxl345_init()
{
	static uint8 data;
	uint8 *data_addr;

	spi_init();
	data_addr = spi_reg_read(0x00);
	qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_TICK, true);
	data = *data_addr;
	gizLog(LOG_INFO, "Device id is 0x%02x\n", data);

	spi_reg_write(0x31, 0x2B);				//测量范围,正负16g，13位模式,output low at interrupts
	spi_reg_write(0x2C, 0x08); 				//速率设定为12.5 参考pdf13页
	spi_reg_write(0x2D, 0x08); 				//选择电源模式 参考pdf24页
	spi_reg_write(0x2E, 0x00); 				//disable all interrupts
	spi_reg_write(0x2F, 0x00); 				//map interrupts to INT1
	spi_reg_write(0x2E, 0x02); 				//使能 watermark 中断
	spi_reg_write(0x38, 0x9F); 				//FIFO in streaming mode
}

SENSOR_DATA_TypeDef ADXL345_AVERAGE(SENSOR_DATA_TypeDef *data, uint8_t len)
{
	SENSOR_DATA_TypeDef result = {0};
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		result.X += data[i].X;
		result.Y += data[i].Y;
		result.Z += data[i].Z;
	}

	result.X /= len;
	result.Y /= len;
	result.Z /= len;

	return result;
}

SENSOR_DATA_TypeDef ADXL345_DATA_CONVERT(uint16_t *data)
{
	int8_t sign = 1;
	uint16_t temp = 0;
	uint8_t i = 0;
	uint32_t val = 0;

	SENSOR_DATA_TypeDef converted_data;

	for (i = 0; i < 3; i++)
	{
		sign = 1;
		temp = data[i];

		if ((temp & 0xF000) == 0xF000)
		{
			sign = -1;
			temp = -temp;
		}

		if (i == 0)
		{
			converted_data.X = ((temp & 0x0FFF) * 4) * sign; //calibrate out x axis offset
		}
		else if (i == 1)
		{
			converted_data.Y = ((temp & 0x0FFF) * 4) * sign; //calibrate out y axis offset
		}
		else if (i == 2)
		{
			converted_data.Z = ((temp & 0x0FFF) * 4) * sign; //calibrate out z axis offset
		}
	}

	gizLog(LOG_INFO, "converted_data.X = %d, converted_data.Y = %d, converted_data.Z = %d\n", converted_data.X, converted_data.Y, converted_data.Z);

	return converted_data;
}

qapi_Status_t ADXL345_READ_FIFO(SENSOR_DATA_TypeDef *axis_converted_avg)
{
	uint16_t axis_raw_data[10][3];
	static uint8_t data_buf[6];
	uint8 *data_addr;
	SENSOR_DATA_TypeDef axis_converted[10];

	for (uint8_t i = 0; i < 10; i++)
	{
		for(uint8_t m = 0; m < 6; m++) {
			data_addr = spi_reg_read(0x32 + m);
			qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_TICK, true);
			data_buf[m] = *data_addr;
		}

		/*x, y, z raw val*/
		for(uint8_t n = 0; n < 6; n++){
			gizLog(LOG_INFO, "data_buf[%d] = 0x%x\n", n, data_buf[n]);
		}

		axis_raw_data[i][0] = data_buf[0] | (((uint16_t)data_buf[1]) << 8);
		axis_raw_data[i][1] = data_buf[2] | (((uint16_t)data_buf[3]) << 8);
		axis_raw_data[i][2] = data_buf[4] | (((uint16_t)data_buf[5]) << 8);
		gizLog(LOG_INFO, "dtat[%d].x = 0x%x, dtat[%d].y = 0x%x, dtat[%d].z = 0x%x\n", i, axis_raw_data[i][0], i, axis_raw_data[i][1], i, axis_raw_data[i][2]);
		axis_converted[i] = ADXL345_DATA_CONVERT(axis_raw_data[i]);	
	}

	*axis_converted_avg = ADXL345_AVERAGE(axis_converted, 10);

	return QAPI_OK;
}

int8_t getaxis(int32_t *X_axis_Value, int32_t *Y_axis_Value, int32_t *Z_axis_Value)
{
	qapi_Status_t res = QAPI_ERROR;
	uint8_t data;
	uint8_t *data_addr;
	SENSOR_DATA_TypeDef axis_converted_avg;

	data_addr = spi_reg_read(0x30);

	if (data_addr == NULL) {
		gizLog(LOG_ERROR, "read 0x30 err\n");
		return GETAXIS_ERR;
	}

	qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_TICK, true);
	data = *data_addr;

	if (data & 0x02)
	{
		res = ADXL345_READ_FIFO(&axis_converted_avg);
		if (res != QAPI_OK)
			return GETAXIS_ERR;

		*X_axis_Value = axis_converted_avg.X;
		*Y_axis_Value = axis_converted_avg.Y;
		*Z_axis_Value = axis_converted_avg.Z;
		return GETAXIS_OK;
	}
	else
	{
		return GETAXIS_NO_INT;
	}
}


