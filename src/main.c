#include "Threads.h"


//osThreadDef(example_0, osPriorityNormal, 1, 0);
//osThreadDef(example_1a, osPriorityNormal, 1, 0);
//osThreadDef(example_1b, osPriorityNormal, 1, 0);
//osThreadDef(example_1c, osPriorityNormal, 1, 0);
osThreadDef(Receive_Wireless, osPriorityNormal, 1, 0);
osThreadDef(Proximity_Sensor_Reading, osPriorityNormal, 1, 0);

// ID for theads
//osThreadId example_1a_thread;
//osThreadId example_1b_thread;
//osThreadId example_1c_thread;
//osThreadId example_0_thread;
osThreadId Receive_Wireless_thread;
osThreadId Proximity_Sensor_Reading_thread;


int main (void) {
	osKernelInitialize ();                    // initialize CMSIS-RTOS
  // initialize peripherals here
	/* LCD initiatization */
  //LCD_Init();
	//Proximity_Sensor_Configuration();
	/*	CC2500 Initialization	*/
  CC2500_Init();
	
  /* LCD Layer initiatization */
  //LCD_LayerInit();
    
  /* Enable the LTDC controler */
  //LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer as the current layer */
  //LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	//example_1a_thread = osThreadCreate(osThread(example_1a), NULL);
	//example_1b_thread = osThreadCreate(osThread(example_1b), NULL);
	//example_1c_thread = osThreadCreate(osThread(example_1c), NULL);
	//example_0_thread = osThreadCreate(osThread(example_0), NULL);
	Receive_Wireless_thread = osThreadCreate(osThread(Receive_Wireless), NULL);
	//Proximity_Sensor_Reading_thread = osThreadCreate(osThread(Proximity_Sensor_Reading), NULL);
	
	/*	Start thread execution	*/
	osKernelStart (); 
	//while	(1);
	
	//uint8_t varnum = CC2500_Read_From_Register(CC2500REG_PKTLEN);
	//printf("%d\n", varnum);
}
