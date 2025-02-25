/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "stdio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t usb_rx_buffer;  // Single-byte buffer for receiving data
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
uint16_t read_ring_oscillator(void);
uint16_t get_avg(uint16_t num);
uint32_t key_gen32(uint16_t trng);
void encrypt(uint8_t msg[], uint8_t buffer[], uint16_t len, uint32_t key);
void decrypt(uint8_t cipher[], uint8_t buffer[], uint16_t len, uint32_t key);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(10000);
  uint16_t avg_number = 50;
  char usb_tx_avg_buffer[30];

  uint16_t avg = get_avg(avg_number);

  sprintf(usb_tx_avg_buffer, "Average: %d\r\n", avg);
  CDC_Transmit_FS((uint16_t *)usb_tx_avg_buffer, strlen(usb_tx_avg_buffer));

  uint8_t msg[] = {0x1, 0x2, 0x3, 0x4};
  uint8_t encrypted[4] = {0};
  uint8_t decrypted[4] = {0};
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    CDC_Transmit_FS(NULL,0);
    HAL_Delay(100);

    if (usb_rx_buffer == 'G') {

      usb_rx_buffer = 0;

      HAL_Delay(2000);

      uint16_t random_number = read_ring_oscillator();

      char usb_tx_buffer[30];

      sprintf(usb_tx_buffer, "Random number: %u\r\n", random_number);
      CDC_Transmit_FS((uint16_t *)usb_tx_buffer, strlen(usb_tx_buffer));

      HAL_Delay(2000);

      uint32_t key = key_gen32(random_number);

      sprintf((char*)usb_tx_buffer, "Key: 0x%X\r\n", key);
      CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));

      HAL_Delay(2000);

      encrypt(msg, encrypted, 4, key);
      decrypt(encrypted, decrypted, 4, key);

      char usb_tx11_buffer[30];
      char usb_tx22_buffer[30];

      sprintf(usb_tx11_buffer, "Size Of Encryption: %u\r\n", sizeof(encrypted));
      for (int i = 0; i < 4; i++) {
        sprintf(usb_tx11_buffer + strlen(usb_tx11_buffer), "%02x ", encrypted[i]);
      }

      sprintf(usb_tx11_buffer + strlen(usb_tx11_buffer), "\r\n");

      CDC_Transmit_FS((uint16_t *)usb_tx11_buffer, strlen(usb_tx11_buffer));

      HAL_Delay(2000);

      sprintf(usb_tx22_buffer, "Size of Decryption: %u\r\n", sizeof(decrypted));
      for (int i = 0; i < 4; i++) {
        sprintf(usb_tx22_buffer + strlen(usb_tx22_buffer), "%02x ", decrypted[i]);
      }

      CDC_Transmit_FS((uint16_t *)usb_tx22_buffer, strlen(usb_tx22_buffer));
    

    }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5
                           PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB4 PB5 PB6
                           PB7 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len)
{
    usb_rx_buffer = Buf[0];
}

uint16_t read_ring_oscillator(void)
{
  uint16_t random_value = 0;

  random_value |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) << 0;
  random_value |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) << 1;
  random_value |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) << 2;
  random_value |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) << 3;
  random_value |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) << 4;
  random_value |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) << 5;
  random_value |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) << 6;
  random_value |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 7;
  random_value |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) << 8;

  return random_value;
}

uint16_t get_avg(uint16_t num) {


    uint16_t sum = 0;
    uint16_t n = 0;
    char usb_tx2_avg_buffer[30];


    for (int i = 0; i < num; i++) {

      uint16_t random_value = read_ring_oscillator();
      sprintf(usb_tx2_avg_buffer, "TRNG Number: %d\r\n", random_value);
      CDC_Transmit_FS((uint16_t *)usb_tx2_avg_buffer, strlen(usb_tx2_avg_buffer));
      sum += random_value;
      n++;
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      HAL_Delay(2000);

    }

    return (float) sum / n;
}


uint32_t key_gen32(uint16_t trng) {
    uint32_t key = 0;
    for (int i = 0; i < 4; i++) {
        key |= (trng << (i * 8));
    }
    return key;
}


void encrypt(uint8_t msg[], uint8_t buffer[], uint16_t len, uint32_t key) {
    int j = 0;
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = msg[i] ^ (key >> (j * 8));
        j++;
        if (j == 4) {
            j = 0;
        }
    }
}

void decrypt(uint8_t cipher[], uint8_t buffer[], uint16_t len, uint32_t key) {
    int j = 0;
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = cipher[i] ^ (key >> (j * 8));
        j++;
        if (j == 4) {
            j = 0;
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
