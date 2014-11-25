#include "Threads.h"

//extern const uint32_t background[38400];
//extern uint8_t data[(uint8_t)SMARTRF_SETTING_PKTLEN];


void Receive_Wireless(void const *argument)
{
	while (1)
	{
		//uint8_t NUMBYTES = CC2500_SendByte(DUMMY_BYTE);
		//printf("%d", NUMBYTES);
		//if(NUMBYTES > 10)
		//{
		CC2500_Receive_Packet();
		//}
		osDelay(250);
	}
}

void Proximity_Sensor_Reading(void const *argument)
{
	double v;
	double distance;
	while (1)
	{
		//	timer/signal
		//	Starting Conversion, waiting for it to finish, clearing the flag, reading the result
		ADC_SoftwareStartConv(ADC1);
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

		//EOC means End Of Conversion
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		v = (double) ADC_GetConversionValue(ADC1);
		
		//y = 7*10^-12x^4 - 6*10^-08x^3 + 0.0002x^2 - 0.3238x + 252.07
		distance = 7.0*(v/1000.0)*(v/1000.0)*(v/1000.0)*(v/1000.0) - 6.0*(v/100.0)*(v/1000.0)*(v/1000.0) + 2.0*(v/10.0)*(v/1000.0) - 0.3238*v + 252.07;
		
		printf("%3.2f\t\t%3.2f\n", v, distance);
		//test
		osDelay(250);
	}
}		


