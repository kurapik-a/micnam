;/**
; * ???? (C)2020, LINKO SEMICONDUCTOR Co.ltd
; *
; * ????: startup_mun.s
; * ????:
; * ????: ????
; * ????: ?
; * ????: V 1.0
; * ?    ?: ??
; * ????: 2015?11?5?
; *
; * ????1:
; * ????:2020?7?28?
; * ? ? ?:V 1.1
; * ? ? ?:???
; * ????:??
; *
; *
;*/
;* <<< Use Configuration Wizard in Context Menu >>>   
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000800

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     MemManageFault_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler               ; Bus Fault Handler
                DCD     UsageFault_Handler             ; Usage Fault Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     DebugMon_Handler               ; Debug Monitor Handler
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler

                ; External Interrupts
                ; 0-3 
                DCD     I2C0_IRQHandler             ;  0
                DCD     I2C1_IRQHandler             ;  1
                DCD     HALL0_IRQHandler            ;  2
                DCD     HALL1_IRQHandler            ;  3
                DCD     UART0_IRQHandler            ;  4
                DCD     UART1_IRQHandler            ;  5
                DCD     UART2_IRQHandler            ;  6
                DCD     CMP0_IRQHandler             ;  7
    DCD     CMP1_IRQHandler             ;  8
    DCD     CMP2_IRQHandler             ;  9
    DCD     CMP3_IRQHandler             ; 10
    DCD     CMP4_IRQHandler             ; 11
    DCD     CMP5_IRQHandler             ; 12
                DCD     MCPWM00_IRQHandler          ; 13
                DCD     MCPWM01_IRQHandler          ; 14
    DCD     MCPWM10_IRQHandler          ; 15
                DCD     MCPWM11_IRQHandler          ; 16
                DCD     TIMER0_IRQHandler           ; 17
                DCD     TIMER1_IRQHandler           ; 18
                DCD     TIMER2_IRQHandler           ; 19
                DCD     TIMER3_IRQHandler           ; 20
                DCD     TIMER4_IRQHandler           ; 21
                DCD     QEP0_IRQHandler             ; 22
                DCD     QEP1_IRQHandler             ; 23
                DCD     QEP2_IRQHandler             ; 24
                DCD     QEP3_IRQHandler             ; 25
                DCD     EXTI_IRQHandler             ; 26
                DCD     ADC0_IRQHandler             ; 27
                DCD     ADC1_IRQHandler             ; 28
                DCD     ADC2_IRQHandler             ; 29
                DCD     CAN_IRQHandler              ; 30
                DCD     SPI0_IRQHandler             ; 31
    DCD     SPI1_IRQHandler             ; 32
                DCD     WWDG_IRQHandler             ; 33
                DCD     DMA_IRQHandler              ; 34
                DCD     WAKEUP_IRQHandler           ; 35
                DCD     PWRDN_IRQHandler            ; 36
                DCD     FMAC_IRQHandler             ; 37
                DCD     SW_IRQHandler               ; 38
                
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler routine
Reset_Handler    PROC
                 EXPORT  Reset_Handler                 [WEAK]
        IMPORT  __main
        IMPORT  SystemInit



        LDR     R0, =__initial_sp          ; set stack pointer 
        MSR     MSP, R0  


ApplicationStart         
                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                    [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler              [WEAK]
                B       .
                ENDP
MemManageFault_Handler\
                PROC
                EXPORT MemManageFault_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT BusFault_Handler                [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT UsageFault_Handler              [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                    [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT DebugMon_Handler                [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                 [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  I2C0_IRQHandler               [WEAK] ;  0
                EXPORT  I2C1_IRQHandler               [WEAK] ;  1
                EXPORT  HALL0_IRQHandler              [WEAK] ;  2
                EXPORT  HALL1_IRQHandler              [WEAK] ;  3
                EXPORT  UART0_IRQHandler              [WEAK] ;  4
                EXPORT  UART1_IRQHandler              [WEAK] ;  5
                EXPORT  UART2_IRQHandler              [WEAK] ;  6
                EXPORT  CMP0_IRQHandler               [WEAK] ;  7
    EXPORT  CMP1_IRQHandler               [WEAK] ;  8
    EXPORT  CMP2_IRQHandler               [WEAK] ;  9
    EXPORT  CMP3_IRQHandler               [WEAK] ; 10
    EXPORT  CMP4_IRQHandler               [WEAK] ; 11
    EXPORT  CMP5_IRQHandler               [WEAK] ; 12
                EXPORT  MCPWM00_IRQHandler            [WEAK] ; 13
                EXPORT  MCPWM01_IRQHandler            [WEAK] ; 14
    EXPORT  MCPWM10_IRQHandler            [WEAK] ; 15
                EXPORT  MCPWM11_IRQHandler            [WEAK] ; 16
                EXPORT  TIMER0_IRQHandler             [WEAK] ; 17
                EXPORT  TIMER1_IRQHandler             [WEAK] ; 18
                EXPORT  TIMER2_IRQHandler             [WEAK] ; 19
                EXPORT  TIMER3_IRQHandler             [WEAK] ; 20
                EXPORT  TIMER4_IRQHandler             [WEAK] ; 21
                EXPORT  QEP0_IRQHandler               [WEAK] ; 22
                EXPORT  QEP1_IRQHandler               [WEAK] ; 23
                EXPORT  QEP2_IRQHandler               [WEAK] ; 24
                EXPORT  QEP3_IRQHandler               [WEAK] ; 25
                EXPORT  EXTI_IRQHandler               [WEAK] ; 26
                EXPORT  ADC0_IRQHandler               [WEAK] ; 27
                EXPORT  ADC1_IRQHandler               [WEAK] ; 28
                EXPORT  ADC2_IRQHandler               [WEAK] ; 29
                EXPORT  CAN_IRQHandler                [WEAK] ; 30
                EXPORT  SPI0_IRQHandler               [WEAK] ; 31
    EXPORT  SPI1_IRQHandler               [WEAK] ; 32
                EXPORT  WWDG_IRQHandler               [WEAK] ; 33
                EXPORT  DMA_IRQHandler                [WEAK] ; 34
                EXPORT  WAKEUP_IRQHandler             [WEAK] ; 35
                EXPORT  PWRDN_IRQHandler              [WEAK] ; 36
                EXPORT  FMAC_IRQHandler               [WEAK] ; 37
                EXPORT  SW_IRQHandler                 [WEAK] ; 38

                    
I2C0_IRQHandler               ;  0
I2C1_IRQHandler               ;  1
HALL0_IRQHandler              ;  2
HALL1_IRQHandler              ;  3
UART0_IRQHandler              ;  4
UART1_IRQHandler              ;  5
UART2_IRQHandler              ;  6
CMP0_IRQHandler               ;  7
CMP1_IRQHandler               ;  8
CMP2_IRQHandler               ;  9
CMP3_IRQHandler               ; 10
CMP4_IRQHandler               ; 11
CMP5_IRQHandler               ; 12
MCPWM00_IRQHandler            ; 13
MCPWM01_IRQHandler            ; 14
MCPWM10_IRQHandler            ; 15
MCPWM11_IRQHandler            ; 16
TIMER0_IRQHandler             ; 17
TIMER1_IRQHandler             ; 18
TIMER2_IRQHandler             ; 19
TIMER3_IRQHandler             ; 20
TIMER4_IRQHandler             ; 21
QEP0_IRQHandler               ; 22
QEP1_IRQHandler               ; 23
QEP2_IRQHandler               ; 24
QEP3_IRQHandler               ; 25
EXTI_IRQHandler               ; 26
ADC0_IRQHandler               ; 27
ADC1_IRQHandler               ; 28
ADC2_IRQHandler               ; 29
CAN_IRQHandler                ; 30
SPI0_IRQHandler               ; 31
SPI1_IRQHandler               ; 32
WWDG_IRQHandler               ; 33
DMA_IRQHandler                ; 34
WAKEUP_IRQHandler             ; 35
PWRDN_IRQHandler              ; 36
FMAC_IRQHandler               ; 37
SW_IRQHandler                 ; 38

                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;************************ (C) COPYRIGHT LINKO Semiconductor *****END OF FILE*****
