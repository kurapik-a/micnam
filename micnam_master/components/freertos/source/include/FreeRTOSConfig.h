/*
 * FreeRTOS V10.4.3
 * AT32F435ZMT7 / Cortex-M4F with FPU / 288MHz / NVIC Group 4
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

#define configCPU_CLOCK_HZ                      ( 288000000UL )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 2000 )
#define configMAX_PRIORITIES                    ( 16 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 30 * 1024 ) )
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_TRACE_FACILITY                1
#define configUSE_PREEMPTION                    1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( 3 )
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            ( 128 )

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configAPPLICATION_ALLOCATED_HEAP        0

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1

/* Run time and task runtime stats */
#define configGENERATE_RUN_TIME_STATS           0

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
    #undef __NVIC_PRIO_BITS
#endif
#define __NVIC_PRIO_BITS                        4

/*
 * NVIC_PRIORITY_GROUP_4: 4-bit preemption priority, 0-bit subpriority.
 *
 * Kernel interrupt priorities (lowest = 15, value shifted by 4 = 0xF0):
 *   SysTick, PendSV, SVC must run at the lowest priority so they
 *   do not preempt ISRs that use FreeRTOS API.
 *
 * configMAX_SYSCALL_INTERRUPT_PRIORITY = 0x50 (priority 5):
 *   ISRs at priority 0~4 are above kernel masking and must NOT use
 *   FreeRTOS API from ISR.
 *   ISRs at priority 5~15 CAN call FreeRTOS FromISR() API.
 */
#define configKERNEL_INTERRUPT_PRIORITY         ( 0xF0 )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 0x50 )

/*
 * Map FreeRTOS portable layer handlers to CMSIS vector names.
 * port.c implements vPortSVCHandler / xPortPendSVHandler / xPortSysTickHandler.
 * These macros redirect the startup vector table entries.
 */
#define vPortSVCHandler                         SVC_Handler
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler

/* ARM Cortex-M4F specific */
#define configENABLE_FPU                        1
#define configENABLE_MPU                        0
#define configENABLE_TRUSTZONE                  0

/* Optional functions - most optimised way */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1

/* Assert */
#define configASSERT( x )                       if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

// 自定义微秒转 ticks 宏 (configTICK_RATE_HZ = 2000) 微秒级的延时
#define pdUS_TO_TICKS(us)    ( ( TickType_t ) ( ( us ) * configTICK_RATE_HZ / 1000000UL ) )



#endif /* FREERTOS_CONFIG_H */
