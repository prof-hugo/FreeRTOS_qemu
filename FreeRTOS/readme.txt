FreeRTOS version 10.4.0 on QEMU ARM 2.8.0-9.2:

* The Cortex-M3 port had to be used for STM32407 (a Cortex-M4F) because FPU is not implemented in QEMU-ARM.

* __NVIC_PRIO_BITS had to be redefined to 8 (in FreeRTOSConfig.h) because PRIGROUP is not implemented in QEMU-ARM (causes a compiler warning).

Hugo Vieira Neto, October 2020