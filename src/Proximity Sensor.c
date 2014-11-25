#include "Proximity Sensor.h"


void Proximity_Sensor_Configuration(void){
	
	//TODO: Conigure GPIO pin check which channel and configure the channel and it should work
	GPIO_InitTypeDef gpio_init_s;
	/*	Define ADC_InitTypeDef and ADC_CommonInitTypeDef*/
	ADC_InitTypeDef adc_init_s;
	ADC_CommonInitTypeDef adc_common_init_s;
	
	/*	Clock gating	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE);
	
	/*	Configure GPIO pin PA1	*/
	gpio_init_s.GPIO_Pin = GPIO_Pin_1;
	gpio_init_s.GPIO_Mode = GPIO_Mode_AN;
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &gpio_init_s);
	
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent;
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2;
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init_s); 
	
	//ADC_DeInit();
	
	/*	Configure the ADC */
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	adc_init_s.ADC_ScanConvMode = DISABLE;
	adc_init_s.ADC_ContinuousConvMode = ENABLE;
	adc_init_s.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init_s.ADC_NbrOfConversion = 1;
	adc_init_s.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &adc_init_s); 
	
  /*	Enable Module */
	ADC_Cmd(ADC1, ENABLE);
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	/*	Setting Channel and ADC	*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_144Cycles); 
}

void Proximity_Sensor_Read(void)
{
	/* Result available in ADC1->DR	*/
	uint16_t ADC_value = ADC_GetConversionValue(ADC1);
	//distance = skdflsfjlsdfj((ADC_value*(Vref/maximum_value_of_resolution_range))-V25)/Avg_Slope + 25;
}