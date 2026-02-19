/**
 * main.c - PQC Demo Firmware
 * Proof of Concept: Initialize HAL + UART, print hello string via USART1 (ST-Link VCP)
 *
 * STM32F769I-DISCO:
 *   USART1 TX = PA9  (connected to ST-Link VCP -> COM5 on host)
 *   USART1 RX = PA10
 *   Default USART1 clock source: APB2 (max 108 MHz on F769)
 */

#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "cycles.h"
#include "crypto_harness.h"

/* ------------------------------------------------------------------
 * Private function prototypes
 * ----------------------------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(UART_HandleTypeDef *huart);
static void Error_Handler(void);

/* ------------------------------------------------------------------
 * Global handle
 * ----------------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* ------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------*/
int main(void)
{
    /* MCU Configuration ----------------------------------------*/
    HAL_Init();
    SystemClock_Config();

    /* Initialize peripherals -----------------------------------*/
    MX_GPIO_Init();
    MX_USART1_UART_Init(&huart1);

    cycles_init();

    /* Transmit hello message -----------------------------------*/
    char msg[128];
    sprintf(msg, "\r\n--- PQC Demo: Milestone 1 Execution ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);

    /* Main loop ------------------------------------------------*/
    while (1) {
        benchmark_rsa();
        benchmark_pqc();
        
        HAL_Delay(5000); // 5 second pause between benchmark runs
    }
}

/* ------------------------------------------------------------------
 * SystemClock_Config
 * Configures: HSE=25MHz, PLL -> SYSCLK=216MHz, APB1=54MHz, APB2=108MHz
 * (Standard for STM32F769I-DISCO)
 * ----------------------------------------------------------------*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable power clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Configure oscillator: HSE on, PLL on */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 25;   /* HSE/25 = 1 MHz */
    RCC_OscInitStruct.PLL.PLLN       = 432;  /* 1 * 432 = 432 MHz VCO */
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;  /* 432/2 = 216 MHz */
    RCC_OscInitStruct.PLL.PLLQ       = 9;   /* 432/9 = 48 MHz USB */
    RCC_OscInitStruct.PLL.PLLR       = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    /* Activate overdrive for 216 MHz */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) { Error_Handler(); }

    /* Configure clocks */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                        RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;    /* HCLK = 216 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;      /* APB1 = 54 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;      /* APB2 = 108 MHz */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) { Error_Handler(); }
}

/* ------------------------------------------------------------------
 * MX_GPIO_Init
 * Enable clocks for GPIO ports used (GPIOA for USART1)
 * ----------------------------------------------------------------*/
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* USART1 TX=PA9, RX=PA10 as Alternate Function */
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* LED1 (PI1) for visual indication */
    __HAL_RCC_GPIOI_CLK_ENABLE();
    GPIO_InitStruct.Pin   = GPIO_PIN_1;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
}

/* ------------------------------------------------------------------
 * MX_USART1_UART_Init
 * USART1 @ 115200-8N1 (connected to ST-Link VCP on F769I-DISCO)
 * ----------------------------------------------------------------*/
static void MX_USART1_UART_Init(UART_HandleTypeDef *huart)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    huart->Instance          = USART1;
    huart->Init.BaudRate     = 115200;
    huart->Init.WordLength   = UART_WORDLENGTH_8B;
    huart->Init.StopBits     = UART_STOPBITS_1;
    huart->Init.Parity       = UART_PARITY_NONE;
    huart->Init.Mode         = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(huart) != HAL_OK) { Error_Handler(); }
}

/* ------------------------------------------------------------------
 * Error_Handler
 * ----------------------------------------------------------------*/
static void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}

/* ------------------------------------------------------------------
 * SysTick_Handler (required for HAL_Delay)
 * ----------------------------------------------------------------*/
void SysTick_Handler(void)
{
    HAL_IncTick();
}
