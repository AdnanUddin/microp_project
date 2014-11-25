#ifndef osObjectsPublic
#define osObjectsPublic

#include "osObjects.h"                      // RTOS object definitions

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "background16bpp.h"

#include <stdio.h>
#include <string.h>
#include "CC2500.h"
#include "Utilities.h"

void Receive_Wireless(void const *argument);
void Proximity_Sensor_Reading(void const *argument);
//void example_0 (void const *argument);
//void example_1a(void const *argument);
//void example_1b(void const *argument);
//void example_1c(void const *argument);


#endif
