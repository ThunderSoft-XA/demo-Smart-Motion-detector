/**
************************************************************
* @file         gizwits_product.c
* @brief        Control protocol processing, and platform-related hardware initialization
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"

/** User area The current device state structure */
dataPoint_t currentDataPoint;

/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t ICACHE_FLASH_ATTR gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t * dataPointPtr = (dataPoint_t *)data;
    moduleStatusInfo_t * wifiData = (moduleStatusInfo_t *)data;

    if((NULL == info) || (NULL == data))
    {
        gizLog(LOG_WARNING,"!!! gizwitsEventProcess Error \r\r\n");
        return -1;
    }

    for(i = 0; i < info->num; i++)
    {
        switch(info->event[i])
        {
        case EVENT_LedValue :
            currentDataPoint.valueLedValue = dataPointPtr->valueLedValue;
            gizLog(LOG_INFO, "Evt: EVENT_LedValue %d \r\r\n", currentDataPoint.valueLedValue);
            if(0x01 == currentDataPoint.valueLedValue)
            {
                //user handle
				//set_gpio_value(0x00);	
            }
            else
            {
                //user handle
				//set_gpio_value(0x01);	
            }
            break;
        case EVENT_MotorValue :
            currentDataPoint.valueMotorValue = dataPointPtr->valueMotorValue;
            gizLog(LOG_INFO,"Evt: EVENT_MotorValue %d \r\r\n", currentDataPoint.valueMotorValue);
            if(0x01 == currentDataPoint.valueMotorValue)
            {
                //ledsetvalue(0x00);	
            }
            else
            {
               // ledsetvalue(0x00);	
            }
            break;



        case WIFI_SOFTAP:
            break;
        case WIFI_AIRLINK:
            break;
        case WIFI_STATION:
            break;
        case WIFI_CON_ROUTER:
            gizLog(LOG_INFO,"@@@@ connected router\r\r\n");
 
            break;
        case WIFI_DISCON_ROUTER:
            gizLog(LOG_INFO,"@@@@ disconnected router\r\r\n");
 
            break;
        case WIFI_CON_M2M:
            gizLog(LOG_INFO,"@@@@ connected m2m\r\r\n");
			setConnectM2MStatus(0x01);
 
            break;
        case WIFI_DISCON_M2M:
            gizLog(LOG_INFO,"@@@@ disconnected m2m\r\r\n");
			setConnectM2MStatus(0x00);
 
            break;
        case WIFI_RSSI:
            gizLog(LOG_INFO,"@@@@ RSSI %d\r\r\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA:
            gizLog(LOG_INFO,"TRANSPARENT_DATA \r\r\n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case MODULE_INFO:
            gizLog(LOG_INFO,"MODULE INFO ...\r\r\n");
            break;
            
        default:
            break;
        }
    }
    gizSendQueue(SIG_UPGRADE_DATA);
    
    return 0; 
}


/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void ICACHE_FLASH_ATTR userHandle(void)
{
    /*
    currentDataPoint.valuehumidity = ;//Add Sensor Data Collection

    */
	 
    //system_os_post(USER_TASK_PRIO_2, SIG_UPGRADE_DATA, 0);
}


/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void ICACHE_FLASH_ATTR userInit(void)
{
    gizMemset((uint8_t *)&currentDataPoint, 0, sizeof(dataPoint_t));

 	/** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    
   		currentDataPoint.valueLedValue = 0;
   		currentDataPoint.valueMotorValue = 0;
   		currentDataPoint.valueHeartRateValue = 0;
   		currentDataPoint.valueX_axis_Value = 0;
   		currentDataPoint.valueY_axis_Value = 0;
   		currentDataPoint.valueZ_axis_Value = 0;
   		currentDataPoint.valuePressure_Value = 0;
  
}


