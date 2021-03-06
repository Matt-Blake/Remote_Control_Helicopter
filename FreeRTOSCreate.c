/* ****************************************************************
 * FreeRTOSCreate.c
 *
 * Source file for the OLED module
 * Create FreeRTOS variables and tasks, as part of the FreeRTOS
 * scheduler.
 *
 * ENCE464 Assignment 1 Group 2
 * Creators: Grayson Mynott      56353855
 *           Ryan Earwaker       12832870
 *           Matt Blake          58979250
 * Last modified: 19/08/2020
 *
 * ***************************************************************/

#include "FreeRTOSCreate.h"

TaskHandle_t FSMTask;
TaskHandle_t OLEDDisp;
TaskHandle_t UARTDisp;
TaskHandle_t StatLED;
TaskHandle_t BtnCheck;
TaskHandle_t SwiCheck;
TaskHandle_t ADCTrig;
TaskHandle_t ADCMean;
TaskHandle_t MainPWM;
TaskHandle_t TailPWM;

QueueHandle_t xAltMeasQueue;
QueueHandle_t xAltDesQueue;
QueueHandle_t xYawMeasQueue;
QueueHandle_t xYawDesQueue;
QueueHandle_t xYawSlotQueue;
QueueHandle_t xFSMQueue;

SemaphoreHandle_t xUARTMutex;
SemaphoreHandle_t xUpBtnSemaphore;
SemaphoreHandle_t xYawFlipSemaphore;

EventGroupHandle_t xFoundAltReference;
EventGroupHandle_t xFoundYawReference;

TimerHandle_t xUpBtnTimer;
TimerHandle_t xDownBtnTimer;
TimerHandle_t xLandingTimer;


/*
 * Function:    createTasks
 * -------------------------
 * Creates all FreeRTOS tasks used in the system.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
static void
createTasks(void)
{
    xTaskCreate(StatusLED,      "LED Task",     LED_STACK_DEPTH,        NULL,       LED_TASK_PRIORITY,      &StatLED);
    xTaskCreate(OLEDDisplay,    "OLED Task",    OLED_STACK_DEPTH,       NULL,       OLED_TASK_PRIORITY,     &OLEDDisp);
    xTaskCreate(UARTDisplay,    "UART Task",    UART_STACK_DEPTH,       NULL,       UART_TASK_PRIORITY,     &UARTDisp);
    xTaskCreate(ButtonsCheck,   "Btn Poll",     BTN_STACK_DEPTH,        NULL,       BTN_TASK_PRIORITY,      &BtnCheck);
    xTaskCreate(SwitchesCheck,  "Switch Poll",  SWITCH_STACK_DEPTH,     NULL,       SWI_TASK_PRIORITY,      &SwiCheck);
    xTaskCreate(TriggerADC,     "ADC Handler",  ADC_STACK_DEPTH,        NULL,       ADC_TASK_PRIORITY,      &ADCTrig);
    xTaskCreate(MeanADC,        "ADC Mean",     MEAN_STACK_DEPTH,       NULL,       MEAN_TASK_PRIORITY,     &ADCMean);
    xTaskCreate(SetMainDuty,    "Main PWM",     MAIN_PWM_STACK_DEPTH,   NULL,       MAIN_PWM_TASK_PRIORITY, &MainPWM);
    xTaskCreate(SetTailDuty,    "Tail PWM",     TAIL_PWM_STACK_DEPTH,   NULL,       TAIL_PWM_TASK_PRIORITY, &TailPWM);
    xTaskCreate(FSM,            "FSM",          FSM_STACK_DEPTH,        NULL,       FSM_TASK_PRIORITY,      &FSMTask);
}


/*
 * Function:    createQueues
 * -------------------------
 * Creates all FreeRTOS queues used in the system and
 * initialises the values to zero.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
static void
createQueues(void)
{
    int32_t queue_init = 0; // Value used to initalise queuea

    // Create queues
    xAltMeasQueue   = xQueueCreate(1, sizeof( int32_t ) );
    xAltDesQueue    = xQueueCreate(1, sizeof( int32_t ) );
    xYawMeasQueue   = xQueueCreate(1, sizeof( int32_t ) );
    xYawDesQueue    = xQueueCreate(1, sizeof( int32_t ) );
    xFSMQueue       = xQueueCreate(1, sizeof( int32_t ) );
    xYawSlotQueue   = xQueueCreate(1, sizeof( int32_t ) );

    // Initalise queues
    xQueueOverwrite(xAltMeasQueue,       &queue_init);
    xQueueOverwrite(xAltDesQueue,        &queue_init);
    xQueueOverwrite(xYawMeasQueue,       &queue_init);
    xQueueOverwrite(xYawDesQueue,        &queue_init);
    xQueueOverwrite(xFSMQueue,           &queue_init);
    xQueueOverwrite(xYawSlotQueue,       &queue_init);
}


/*
 * Function:    createSemaphores
 * ------------------------------
 * Creates all FreeRTOS semaphores and mutexes used in the system.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
static void
createSemaphores(void)
{
    // Create mutexs to avoid race conditions
    xUARTMutex =  xSemaphoreCreateMutex();

    // Create semaphores used to count button presses
    xUpBtnSemaphore = xSemaphoreCreateCounting(SEMAPHORE_SIZE, 0);
    xYawFlipSemaphore = xSemaphoreCreateCounting(SEMAPHORE_SIZE, 0);
}


/*
 * Function:    createEventGroups
 * -------------------------------
 * Creates all FreeRTOS Event groups used as flags in the system.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
static void
createEventGroups(void)
{
    int event_init = 0; //  Value used to initalise event groups

    // Create event groups to act as flags
    xFoundAltReference = xEventGroupCreate();
    xFoundYawReference = xEventGroupCreate();

    // Initalise event groups to zero
    xEventGroupSetBits(xFoundAltReference, event_init);
    xEventGroupSetBits(xFoundYawReference, event_init);
}

/*
 * Function:    createTimers
 * --------------------------
 * Creates all FreeRTOS timers used in the system.
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
static void
createTimers(void)
{
    xUpBtnTimer     = xTimerCreate( "Button Timer", DBL_BTN_TMR_PERIOD
                                    / portTICK_RATE_MS, pdFALSE, ( void * ) 0, vDblBtnTimerCallback );
    xDownBtnTimer   = xTimerCreate( "Yaw Flip Timer", YAW_FLIP_TMR_PERIOD
                                    / portTICK_RATE_MS, pdFALSE, ( void * ) 0, vDblBtnTimerCallback );
    xLandingTimer   = xTimerCreate( "Land Timer", LAND_TMR_PERIOD
                                    / portTICK_RATE_MS, pdTRUE,  ( void * ) 0, vLandTimerCallback );
}


/*
 * Function:    initFreeRTOS
 * --------------------------
 * Call to all initialize functions for FreeRTOS components
 *
 * @params:
 *      - NULL
 * @return:
 *      - NULL
 * ---------------------
 */
void
initFreeRTOS(void)
{
    createTasks();
    createQueues();
    createSemaphores();
    createEventGroups();
    createTimers();
}


