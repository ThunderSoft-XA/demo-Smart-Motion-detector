/**
************************************************************
* @file         main.c
* @brief        main
* @author       Gizwits
* @date         2018-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include "gagent_soc.h"
#include "../Gizwits/gizwits_product.h"
#include "log.h"
#include "../driver/gpio/gpio.h"
#include "../driver/adxl345/adxl345.h"


#define QUERY_INTERVAL 2

#define X_AXIS_THRESHOLD		200	
#define Y_AXIS_THRESHOLD		50
#define Z_AXIS_THRESHOLD		50

static SENSOR_DATA_TypeDef old_axis_val;
extern MODULE_PIN_ENUM led_green;

TX_TIMER *userTimer;

//led port
void led_init()
{
	led_gpio_config();
	led_on_off(false,led_green);	//init status is off
	led_on_off(true,GPIO_BLUE);		//init status is off
}

//ADXL345 init
void motion_init()
{ 
	adxl345_init();
}

void ICACHE_FLASH_ATTR userTimerCB(void)
{
    static uint8_t ctime = 0;
    static uint8_t ccount = 0;
    GETAXIS_ERROR_t status = 0;
	static uint8_t num = 0;
	int tmp_x = 0, tmp_y = 0, tmp_z = 0;
	
	int32_t X_axis_Value = 0;
	int32_t Y_axis_Value = 0;
	int32_t Z_axis_Value = 0;

	if (QUERY_INTERVAL < ctime)			
	{
			ctime = 0;
			status = getaxis(&X_axis_Value,&Y_axis_Value,&Z_axis_Value);
			if( status != GETAXIS_OK)
			{
				return;	
			}
			if(num == 0)
			{
				old_axis_val.X =  X_axis_Value;
				old_axis_val.Y =  Y_axis_Value;
				old_axis_val.Z =  Z_axis_Value;
				num += 1;
			}
			else
			{
				tmp_x = old_axis_val.X > X_axis_Value ? old_axis_val.X - X_axis_Value:X_axis_Value - old_axis_val.X;
				tmp_y = old_axis_val.Y > Y_axis_Value ? old_axis_val.Y - Y_axis_Value:Y_axis_Value - old_axis_val.Y;
				tmp_z = old_axis_val.Z > Z_axis_Value ? old_axis_val.Z - Z_axis_Value:Z_axis_Value - old_axis_val.Z;
				gizLog(LOG_INFO, "tmp_x= %d, tmp_y = %d , tmp_z = %d\n",  tmp_x, tmp_y, tmp_z);
				if(tmp_x > X_AXIS_THRESHOLD || tmp_y > Y_AXIS_THRESHOLD || tmp_z > Z_AXIS_THRESHOLD)
				{ 	
					led_on_off(true,led_green);	
					gizLog(LOG_INFO, "LED ON\n");
				}
				else
				{
					led_on_off(false, led_green);
					gizLog(LOG_INFO, "LED OFF\n");
				}
				old_axis_val.X =  X_axis_Value;
				old_axis_val.Y =  Y_axis_Value;
				old_axis_val.Z =  Z_axis_Value;
			}
			currentDataPoint.valueX_axis_Value = X_axis_Value;
			currentDataPoint.valueY_axis_Value = Y_axis_Value;
			currentDataPoint.valueZ_axis_Value = Z_axis_Value;
	}
    ctime++;  
}

void sensorInit(void)
{
    int32 ret = -1;
    
    gizLog(LOG_INFO,"Sensor initialization ...\n"); 

	led_init();
	motion_init();
    txm_module_object_allocate(&userTimer, sizeof(TX_TIMER));
    ret = tx_timer_create(userTimer, "userTimer", userTimerCB, NULL, 1,
                          200, TX_AUTO_ACTIVATE);			
    if(ret != TX_SUCCESS)
    {
        gizLog(LOG_ERROR,"Failed to create UserTimer.\n");
    }
}


void gagentMain( void )
{
    getFreeHeap();
    sensorInit();
    gizwitsInit();
}
