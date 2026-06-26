/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
// Mảng mã HEX hiển thị từ 0-9 cho LED 7 đoạn Cathode chung (Common Cathode)
const uint8_t numSegments[] = {
  0x3F, // Số 0 (Thanh a,b,c,d,e,f sáng)
  0x06, // Số 1
  0x5B, // Số 2
  0x4F, // Số 3
  0x66, // Số 4
  0x6D, // Số 5
  0x7D, // Số 6
  0x07, // Số 7
  0x7F, // Số 8
  0x6F  // Số 9
};
const uint16_t LED_PINS[8] = {
  GPIO_PIN_0, // Thanh A
  GPIO_PIN_1, // Thanh B
  GPIO_PIN_2, // Thanh C
  GPIO_PIN_3, // Thanh D
  GPIO_PIN_4, // Thanh E
  GPIO_PIN_5, // Thanh F
  GPIO_PIN_6, // Thanh G
  GPIO_PIN_7  // Thanh DP
};
int count = 0;          // Biến đếm thời gian từ 0 -> 99
uint8_t isRunning = 0;  // Trạng thái đếm (0: dừng, 1: đang đếm)

uint32_t previousMillis = 0;
const uint32_t interval = 1000; // Khoảng thời gian tăng 1 đơn vị (1000ms = 1 giây)

// Biến lưu trạng thái nút nhấn trước đó để bắt sườn xuống (nút nhấn xuống GND)
uint8_t lastBtnStateSP = 1;
uint8_t lastBtnStateRst = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void displayDigit(int num);
void displayNumber(int num);
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  // 1. XỬ LÝ NÚT NHẤN START / PAUSE / RESUME (Chân PA0)
	      uint8_t currentBtnStateSP = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	      if (currentBtnStateSP == GPIO_PIN_RESET && lastBtnStateSP == GPIO_PIN_SET) {
	        HAL_Delay(20); // Chống nhiễu phím bấm (Debounce)
	        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
	          if (count < 99) { // Nếu đã đến 99 thì nút này không có tác dụng nữa, phải Reset
	            isRunning = !isRunning; // Đảo trạng thái Chạy <-> Dừng
	          }
	        }
	      }
	      lastBtnStateSP = currentBtnStateSP;

	      // 2. XỬ LÝ NÚT NHẤN RESET (Chân PA1)
	      uint8_t currentBtnStateRst = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
	      if (currentBtnStateRst == GPIO_PIN_RESET && lastBtnStateRst == GPIO_PIN_SET) {
	        HAL_Delay(20); // Chống nhiễu phím bấm
	        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET) {
	          count = 0;       // Đưa biến đếm về 0
	          isRunning = 0;   // Dừng đếm
	          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // Tắt còi Buzzer
	        }
	      }
	      lastBtnStateRst = currentBtnStateRst;

	      // 3. LOGIC ĐẾM THỜI GIAN (Sử dụng hàm HAL_GetTick() để không làm đứng mạch)
	      uint32_t currentMillis = HAL_GetTick();
	      if (isRunning && (currentMillis - previousMillis >= interval)) {
	        previousMillis = currentMillis;
	        count++;

	        // Kiểm tra khi chạm mốc 99
	        if (count >= 99) {
	          count = 99;
	          isRunning = 0; // Ngừng đếm
	          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Bật còi Buzzer liên tục
	        }
	      }

	      // 4. QUÉT LED LIÊN TỤC ĐỂ HIỂN THỊ SỐ
	      displayNumber(count);

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 PC6 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Hàm xuất mã 8-bit của chữ số ra các chân từ PC0 đến PC7
void displayDigit(int num) {
  uint8_t segments = numSegments[num];

    for (int i = 0; i < 8; i++) {

      int bitStatus = (segments >> i) & 0x01;

      // Ghi trạng thái ra chân tương ứng trong mảng
      HAL_GPIO_WritePin(GPIOC, LED_PINS[i], bitStatus ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

// Hàm thực hiện quét LED
void displayNumber(int num) {
  int tens = num / 10;   // Lấy chữ số hàng chục
  int units = num % 10;  // Lấy chữ số hàng đơn vị

  // --- BƯỚC 1: HIỂN THỊ HÀNG CHỤC ---
    // Tắt LED đơn vị trước để chống bóng mờ
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    // Xuất dữ liệu hàng chục và BẬT LED hàng chục
    displayDigit(tens);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_Delay(9);

    // --- BƯỚC 2: HIỂN THỊ HÀNG ĐƠN VỊ ---
    // Tắt LED hàng chục trước
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // Xuất dữ liệu hàng đơn vị và BẬT LED hàng đơn vị
    displayDigit(units);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(9);
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
#ifdef USE_FULL_ASSERT
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
