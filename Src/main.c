/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>



/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
UART_HandleTypeDef huart6;
 DMA_HandleTypeDef hdma_usart6_tx;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
 /* Private variables ---------------------------------------------------------*/
#define TX_BUFSIZE 128
//un buffer statique tx_buf dans la RAM.
 static uint8_t tx_buf[TX_BUFSIZE];
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void GPIO_Init(void);
static void USART6_Init(void);
static void DMA_Init(void);

HAL_StatusTypeDef send_dma_nonblocking(uint8_t *buf, uint16_t len);

static volatile bool dma_tx_busy = false;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */


/* helper: envoi DMA non-blocking. Retourne HAL_OK si lancé, HAL_BUSY si DMA occupé. */
HAL_StatusTypeDef send_dma_nonblocking(uint8_t *buf, uint16_t len)
{
    if (len == 0 || buf == NULL)
      return HAL_ERROR;
    if (dma_tx_busy) 
      return HAL_BUSY;
    if (HAL_UART_Transmit_DMA(&huart6, buf, len) != HAL_OK)
    {
        return HAL_ERROR;
    }
    
    dma_tx_busy = true;
    return HAL_OK;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    USART6_Init();  
    DMA_Init();      

    /* Prépare message */
    const char *hello = "Hello from STM32 via HC-05\r\n";
    size_t len = strlen(hello);
    if (len > TX_BUFSIZE) len = TX_BUFSIZE;
    memcpy(tx_buf, hello, len);

    /* Lancement du DMA */
    if (send_dma_nonblocking(tx_buf, (uint16_t)len) != HAL_OK)
    {
        Error_Handler();
    }
  while (1)
  {
    //Pendant que le DMA travaille, ton CPU peut continuer à faire autre chose (ici clignotement LED PD12).
     HAL_Delay(500);
     HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

static void DMA_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  
hdma_usart6_tx.Instance = DMA2_Stream6;
hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
hdma_usart6_tx.Init.Mode = DMA_NORMAL; /* or DMA_CIRCULAR for continuous RX; TX usually NORMAL */
hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;
hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; 
if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
    {
        Error_Handler();
    }

 /* Link DMA handle to UART handle */
__HAL_LINKDMA(&huart6, hdmatx, hdma_usart6_tx);
//Quand le DMA a fini le transfert, l’interruption est déclenchée.
 HAL_NVIC_SetPriority(DMA2_Stream6_IRQn,5,0);
 HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
}

static void USART6_Init (void)
{
  __HAL_RCC_USART6_CLK_ENABLE(); 
    huart6.Instance = USART6;
    huart6.Init.BaudRate = 9600;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;     
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart6) ; 
}
 
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6; 
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
//Quand tout le buffer est envoyé, tu sais que le DMA a terminé et tu peux éventuellement envoyer un nouveau message.
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        dma_tx_busy = false;
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
    }
}

/* DMA IRQ handler forwarding to HAL (must be present) */

static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 
