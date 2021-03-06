/* ****************************************************************
 * OLED.h
 *
 * Header file for the OLED module
 * Print's to the Orbit Boosterpack's OLED display to indicate the
 * helicopter program's status.
 *
 * ENCE464 Assignment 1 Group 2
 * Creators: Grayson Mynott      56353855
 *           Ryan Earwaker       12832870
 *           Matt Blake          58979250
 * Last modified: 19/08/2020
 *
 * ***************************************************************/

#ifndef OLED_H_
#define OLED_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/pwm.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "inc/hw_memmap.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOSCreate.h"
#include "uart.h"

#define ROW_ZERO                0       // Row zero on the OLED display
#define ROW_ONE                 1       // Row one on the OLED display
#define ROW_TWO                 2       // Row two on the OLED display
#define ROW_THREE               3       // Row three on the OLED display
#define COLUMN_ZERO             0       // Column zero on the OLED display
#define DISPLAY_SIZE            17      // Size of strings for the OLED display
#define NUM_STATES              4       // The number of helicopter states


/*
 * Function:    OLEDDisplay
 * -------------------------
 * FreeRTOS task that periodically displays flight
 * information on the Orbit BoosterPack OLED display.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
void OLEDDisplay (void *pvParameters);


#endif /* OLED_H_ */
