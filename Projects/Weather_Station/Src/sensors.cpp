#include "sensors.h"

void sensor_inits()
{
	BSP_TSENSOR_Init();
	BSP_HSENSOR_Init();
	BSP_PSENSOR_Init();
}

float get_temperature()
{
	return BSP_TSENSOR_ReadTemp();
}

float get_humidity()
{
	return BSP_HSENSOR_ReadHumidity();
}

float get_pressure()
{
	return BSP_PSENSOR_ReadPressure();
}
