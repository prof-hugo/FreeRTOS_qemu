/*==================================================================================================

  blink_leds_qemu.c

  Used hardware: STM32F4-Discovery evaluation board (QEMU)

  Example of using the FreeRTOS Real-Time Operating System on the STM32F4-Discovery board. It is
  used the GNU Cross-toolchain for ARM microprocessors.

  ==================================================================================================

  Author: Henrique Persico Rossi
  e-mail: henriqueprossi@gmail.com

  ================================================================================================*/

/*==================================================================================================
  INCLUDE FILES
==================================================================================================*/

#include "stm32f4_discovery.h"
#include "FreeRTOS.h"
#include "task.h"

/*=================================================================================================
  LOCAL DEFINITIONS
=================================================================================================*/

/*==================================================================================================
  LOCAL FUNCTIONS
==================================================================================================*/

static void blink_leds_task(void * parameters);

/*==================================================================================================
  LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
  Function    : vApplicationIdleHook


  Description : The idle task can optionally call an application defined hook (or callback)
                function - the idle hook. The idle task runs at the very lowest priority, so such
                an idle hook function will only get executed when there are no tasks of higher
                priority that are able to run. This makes the idle hook function an ideal place to
                put the processor into a low power state - providing an automatic power saving
                whenever there is no processing to be performed.

                The idle hook will only get called if configUSE_IDLE_HOOK is set to 1 within
                FreeRTOSConfig.h.

                The idle hook is called repeatedly as long as the idle task is running. It is
                paramount that the idle hook function does not call any API functions that could
                cause it to block. Also, if the application makes use of the vTaskDelete() API
                function then the idle task hook must be allowed to periodically return (this is
                because the idle task is responsible for cleaning up the resources that were
                allocated by the RTOS kernel to the task that has been deleted).

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationIdleHook(void)
{
  // TODO: Put the processor into a low power state.
}

/*==================================================================================================
  Function    : vApplicationStackOverflowHook


  Description : Stack overflow is a very common cause of application instability. FreeRTOS therefore
                provides two optional mechanisms that can be used to assist in the detection and
                correction of just such an occurrence. The option used is configured using the
                configCHECK_FOR_STACK_OVERFLOW configuration constant. The application must provide
                a stack overflow hook function if configCHECK_FOR_STACK_OVERFLOW is not set to 0.

                The xTask and pcTaskName parameters pass to the hook function the handle and name of
                the offending task respectively. Note however, depending on the severity of the
                overflow, these parameters could themselves be corrupted, in which case the
                pxCurrentTCB variable can be inspected directly.

                Stack overflow checking introduces a context switch overhead so its use is only
                recommended during the development or testing phases.

  Parameters  : xTask       Handle of the offending task.
                pcTaskName  Name of the offending task.

  Returns     : None
==================================================================================================*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  // TODO: It is a place to add stack overflow processing.
}

/*==================================================================================================
  Function    : vApplicationTickHook


  Description : The tick interrupt can optionally call an application defined hook (or callback)
                function - the tick hook. The tick hook provides a convenient place to implement
                timer functionality.

                The tick hook will only get called if configUSE_TICK_HOOK is set to 1 within
                FreeRTOSConfig.h.

                vApplicationTickHook() executes from within an ISR so must be very short, not use
                much stack, and not call any API functions that don't end in "FromISR" or "FROM_ISR".

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationTickHook(void)
{
  // TODO: It is a place to add timer processing.
}

/*==================================================================================================
  Function    : vApplicationMallocFailedHook


  Description : The memory allocation schemes implemented by heap_1.c, heap_2.c, heap_3.c and heap_4
                can optionally include a malloc() failure hook (or callback) function that can be
                configured to get called if pvPortMalloc() ever returns NULL.

                Defining the malloc() failure hook will help identify problems caused by lack of
                heap memory - especially when a call to pvPortMalloc() fails within an API function.

                The malloc failed hook will only get called if configUSE_MALLOC_FAILED_HOOK is set
                to 1 within FreeRTOSConfig.h.

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationMallocFailedHook(void)
{
  // TODO: It is a place to add processing for error on allocating memory.
}

/*==================================================================================================
  Function    : main


  Description : Entry point of the user application. It only set/clear the LD3, LD4, LD5 and LD6 leds
                on the board.

  Parameters  : None

  Returns     : EXIT_SUCCESS:	Successful termination

                Otherwise:		Error
==================================================================================================*/

int main(void)
{
  /* At this point the microcontroller clock is already set, which is done through SystemInit()
   * function. This is done by means of the C runtime initialization (crt.c), which, afterwards,
   * call the main application.
   */

  /* Update SystemCoreClock variable according to Clock Register Values. The tick frequency is
   * set by the scheduler.
   */
  SystemCoreClockUpdate();

  /* Most systems default to the wanted configuration, with the noticeable exception of the STM32
   * driver library. If you are using an STM32 with the STM32 driver library then ensure all the
   * priority bits are assigned to be preempt priority bits by calling
   * NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4) before the RTOS is started.
   */
  //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); // !PRIGROUP is unimplemented in QEMU ARM 2.8.0-9.2

  // GPIOD Peripheral clock enable.
  __HAL_RCC_GPIOD_CLK_ENABLE();
   
  /* LED configurations:
   * LD4: GPIO PD12 (0: OFF, 1: ON)
   * LD3: GPIO PD13 (0: OFF, 1: ON)
   * LD5: GPIO PD14 (0: OFF, 1: ON)
   * LD6: GPIO PD15 (0: OFF, 1: ON)
   */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  // The task for blinking the leds is created. It is called just after the scheduler is started.
  xTaskCreate(  blink_leds_task           ,
                "blink_leds_task"         ,
                configMINIMAL_STACK_SIZE  ,
                NULL                      ,
                tskIDLE_PRIORITY          ,
                NULL                      );

  // It is started the scheduler.
  vTaskStartScheduler();

  /* If all is well, the scheduler will now be running, and the following line will never be reached.
   * If the following line does execute, then there was insufficient FreeRTOS heap memory available
   * for the idle and/or timer tasks to be created. See the memory management section on the
   * FreeRTOS web site for more details.
   */
  for(;;);
  
  return 0;
}

/*==================================================================================================
  Function    : blink_leds_task


  Description : This is the task which controls the leds (LED3, LED4, LED5 and LED6) on the board
                by lighting them individually and sequentially every second. Once all of them are
                on, their states are set to off, and the cycle restarts.

  Parameters  : parameters: A parameter which is passed to the task when it started.

  Returns     : None
==================================================================================================*/

static void blink_leds_task(void * parameters)
{
  for (;;) {
	BSP_LED_On(LED4); // LED4 ON
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    BSP_LED_On(LED3); // LED3 ON
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    BSP_LED_On(LED5); // LED5 ON
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    BSP_LED_On(LED6); // LED6 ON
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    BSP_LED_Off(LED4); BSP_LED_Off(LED3); BSP_LED_Off(LED5); BSP_LED_Off(LED6);  // All LEDs are OFF.
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
