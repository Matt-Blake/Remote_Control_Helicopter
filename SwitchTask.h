/*
 * SwitchTask.h
 *
 *  Created on: 30/07/2020
 *      Author: grayson-uc
 */

#ifndef SWITCHTASK_H_
#define SWITCHTASK_H_

extern QueueHandle_t xButtonQueue;
extern SemaphoreHandle_t xTokenMutex;

uint32_t SwitchTaskInit(void);

void SwitchTask(void *pvParameters);


#endif /* SWITCHTASK_H_ */
