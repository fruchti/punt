.syntax unified
.thumb
.fpu softvfp

.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
init_data:
	ldr	r2, =_start_init_data
	ldr	r3, =_start_data
	ldr	r1, =_end_data

init_data_loop:
	cmp	r3, r1
	bhs	zero_bss
	ldr	r0, [r2], #4
	str	r0, [r3], #4
	b init_data_loop

zero_bss:
	ldr	r3, =_start_bss
	ldr	r1, =_end_bss
    movs r0, #0

zero_bss_loop:
	cmp	r3, r1
	bhs	init_finished
	str	r0, [r3], #4
    b zero_bss_loop

init_finished:
    bl main

infinite_loop:
    b infinite_loop

.size Reset_Handler, .-Reset_Handler

// Only the core interrupt vectors, since the bootloader code does not use
// interrupts. Saves a few bytes to omit the interrupt vector table.
.section .cortex_vectors, "a"
.word _end_stack
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler
.word MemManage_Handler
.word BusFault_Handler
.word UsageFault_Handler
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word SVC_Handler
.word DebugMon_Handler
.word 0x00000000
.word PendSV_Handler
.word SysTick_Handler

.type Dummy_Handler, %function
Dummy_Handler:
    b Dummy_Handler

.weak  NMI_Handler
.thumb_set NMI_Handler, Dummy_Handler

.weak  HardFault_Handler
.thumb_set HardFault_Handler, Dummy_Handler

.weak  MemManage_Handler
.thumb_set MemManage_Handler, Dummy_Handler

.weak  BusFault_Handler
.thumb_set BusFault_Handler, Dummy_Handler

.weak  UsageFault_Handler
.thumb_set UsageFault_Handler, Dummy_Handler

.weak  SVC_Handler
.thumb_set SVC_Handler, Dummy_Handler

.weak  DebugMon_Handler
.thumb_set DebugMon_Handler, Dummy_Handler

.weak  PendSV_Handler
.thumb_set PendSV_Handler, Dummy_Handler

.weak  SysTick_Handler
.thumb_set SysTick_Handler, Dummy_Handler
