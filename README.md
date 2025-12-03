# STM32 UART DMA Bluetooth Communication

This project demonstrates how to send data from an **STM32F4 Discovery** board to an **HC-05 Bluetooth module** using **UART with DMA (non-blocking transmission)**.

The microcontroller sends a message ("Hello from STM32 via HC-05") to a paired smartphone while toggling LEDs to show CPU availability during DMA transfer.

---

## Hardware Used
- **STM32F4 Discovery Board**
- **HC-05 Bluetooth Module**
- Jumper wires (TX/RX + 3.3V + GND)
- Smartphone with **Serial Bluetooth Terminal** app

---

## How It Works

###  UART6 configured for TX
The STM32 sends data on:
- **PC6 → USART6_TX**
- Baud rate: **9600**

###  DMA2 Stream 6 handles non-blocking transmission
- CPU is free while the DMA transfers bytes
- When DMA finishes, an interrupt triggers `HAL_UART_TxCpltCallback()`

###  HC-05 receives the data and sends it to a smartphone
After pairing with a phone, any Bluetooth terminal app will show the message.

---

##  Project Features
- **Non-blocking UART transmission using DMA**
- **LED heartbeat while DMA runs**
- **LED notification on DMA transfer complete**
- Clean code architecture (HAL + callbacks)
- Easy to extend (e.g., receive data, send commands, etc.)

