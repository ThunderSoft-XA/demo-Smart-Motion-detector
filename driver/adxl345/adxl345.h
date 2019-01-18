#ifndef  __ADXL345_H__
#define  __ADXL345_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../main/log.h"
#include <qapi_status.h>

 typedef struct
{
	int X;
	int Y;
	int Z;
} SENSOR_DATA_TypeDef;

typedef enum
{
	I2C_REG_8BIT = 0,
	I2C_REG_16BIT
}I2C_REG_TYPE_t;

typedef enum
{
	GETAXIS_ERR = -1,
	GETAXIS_OK,
	GETAXIS_NO_INT
}GETAXIS_ERROR_t;

void adxl345_init();																//ADXL345 init
int8_t getaxis(int32_t *X_axis_Value,int32_t *Y_axis_Value,int32_t *Z_axis_Value);	//axis from ADXL345
qapi_Status_t ADXL345_READ_FIFO(SENSOR_DATA_TypeDef *axis_converted_avg);			//read fifo data
SENSOR_DATA_TypeDef ADXL345_DATA_CONVERT(uint16_t *data);							//change raw data to real data
SENSOR_DATA_TypeDef ADXL345_AVERAGE(SENSOR_DATA_TypeDef *data, uint8_t len);		//averag;			

#endif //__ADXL345_H__
