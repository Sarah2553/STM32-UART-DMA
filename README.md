# STM32F407 Bluetooth Communication with DMA

A button-triggered Bluetooth data transmission system using STM32F407 Discovery board, HC-05 module, and DMA-based UART communication.

##  Project Overview

This project demonstrates efficient wireless communication between an STM32F407 microcontroller and a Bluetooth terminal application using Direct Memory Access (DMA) for non-blocking data transfer.

##  Features

- **Button-Triggered Transmission**: Press user button (PA0) to send data
- **DMA-Based UART**: Non-blocking communication using DMA2 Stream6
- **Visual Feedback**: Multi-color LED indicators
  - 🟢 GREEN (PD12): Transmission in progress
  - 🔵 BLUE (PD15): Transmission complete (blinks 3×)
  - 🔴 RED (PD14): Error indication
- **Interrupt-Driven Architecture**:  Efficient CPU utilization
- **Software Debouncing**: Reliable button input handling

## Hardware Requirements

- STM32F407VG Discovery Board
- HC-05 Bluetooth Module
- Jumper wires
- USB cable for programming
- Smartphone/PC with Bluetooth terminal app

## Hardware Connections

```
STM32F407          HC-05
──────────────────────────
PC6 (TX)    →     RX
PC7 (RX)    ←     TX
GND         →     GND
3. 3V/5V     →     VCC
```

## Software Requirements

- IAR Embedded Workbench for ARM (or STM32CubeIDE)
- STM32CubeF4 HAL Library v1.27.0
- Bluetooth Serial Terminal App (e.g., Serial Bluetooth Terminal for Android)

##  How It Works

1. **Initialization**: System configures clocks, GPIO, DMA, and USART6
2. **Idle State**: CPU waits for button press interrupt
3. **Button Press**: Triggers EXTI0 interrupt
4. **Transmission Start**: GREEN LED turns ON, DMA begins transfer
5. **DMA Transfer**: 29 bytes transferred from memory to USART6
6. **Completion**: DMA and UART interrupts trigger callback
7. **Indication**: GREEN LED OFF, BLUE LED blinks 3 times

## System Architecture

```
User Button (PA0) → EXTI Interrupt → DMA Transfer → USART6 → HC-05 → Bluetooth
                                           ↓
                                    TX Complete Callback
                                           ↓
                                      LED Feedback
```

## Code Structure

```
stm32-bluetooth-dma/
├── Inc/
│   ├── main.h
│   ├── stm32f4xx_it.h
│   └── stm32f4xx_hal_conf.h
├── Src/
│   ├── main.c              # Main application logic
│   ├── stm32f4xx_it.c      # Interrupt handlers
│   ├── stm32f4xx_hal_msp.c # HAL MSP initialization
│   └── system_stm32f4xx. c  # System initialization
└── README.md
```

## Key Functions

- `HAL_UART_Transmit_DMA()`: Initiates DMA transfer
- `HAL_UART_TxCpltCallback()`: Called when transmission completes
- `HAL_GPIO_EXTI_Callback()`: Handles button press events
- `DMA2_Stream6_IRQHandler()`: DMA interrupt handler
- `USART6_IRQHandler()`: UART interrupt handler

## Technical Specifications

- **MCU**: STM32F407VGT6 (ARM Cortex-M4)
- **Clock**: 168 MHz system clock
- **Communication**: USART6 at 9600 baud, 8N1
- **DMA**: DMA2 Stream6, Channel 5
- **Interrupts**:  EXTI0, DMA2_Stream6, USART6

## Learning Outcomes

- Understanding DMA controllers and configuration
- Interrupt-driven embedded systems design
- UART communication protocols
- NVIC (Nested Vectored Interrupt Controller) management
- Efficient peripheral utilization

## Troubleshooting

**LED doesn't blink after transmission:**
- Ensure USART6 interrupt is enabled in `USART6_Init()`
- Verify `USART6_IRQHandler()` exists in `stm32f4xx_it.c`

**No data received in Bluetooth terminal:**
- Check HC-05 baud rate configuration (should be 9600)
- Verify TX/RX connections (STM32 TX → HC-05 RX)
- Ensure HC-05 is powered and paired
 
