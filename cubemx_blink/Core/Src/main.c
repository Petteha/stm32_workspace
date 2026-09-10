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

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

#define GPIO_BASE (0x48000000U)
#define ODR_OFFSET (0x14U)
#define GPIO_ODR (0x48000000U + ODR_OFFSET)
#define RCC_BASE (0x40021000U)
#define RCC_AHB2ENR (RCC_BASE + 0x4C)

#define TIM2 		 (0x40000000U)
#define RCC_APB1ENR1 (RCC_BASE + 0x58U)
#define TIM2_PSC 	 (TIM2 + 0x028U)
#define TIM2_ARR 	 (TIM2 + 0x02CU)
#define TIM2_CNT 	 (TIM2 + 0x024U)
#define TIM2_SR 	 (TIM2 + 0x010U)
#define TIM2_CR1 	 (TIM2 + 0x00U)
#define TIM2_EGR     (TIM2 + 0x014U)
#define TIM2_DIER    (TIM2 + 0x00CU)

#define NVIC_BASE           (0xE000E100U)


/* void delay_ms(uint32_t milliseconds)
	{
		volatile uint32_t *tim2_SR =(volatile uint32_t *)TIM2_SR;
	    uint32_t elapsed = 0;

	    volatile uint32_t *CNT = (volatile uint32_t*)TIM2_CNT;

	    while (elapsed < milliseconds)
	    {

	    	if ((*tim2_SR & (1U << 0)) != 0) {
	    		elapsed = elapsed + 1;
	    		*tim2_SR &= ~(1U << 0);
	    	}
	    }
	}
*/

volatile uint32_t irq_count = 0;

void TIM2_IRQHandler(void)
{
	volatile uint32_t *LED2_GREEN = (volatile uint32_t *)GPIO_ODR;

	volatile uint32_t *tim2_SR = (volatile uint32_t*)TIM2_SR;

	if ((*tim2_SR & (1U << 0)) != 0) {
			*tim2_SR &= ~(1U << 0);   // clear UIF first

			irq_count++;

			if (irq_count == 500)
			{
				*LED2_GREEN |= (1U << 5);
			}

			if (irq_count >= 1000)
			{
				*LED2_GREEN &= ~(1U << 5);
				irq_count = 0;
			}
		}
}

int main(void)
{

	  HAL_Init();

	  /* Configure the system clock */
	  SystemClock_Config();


	/* enable tim2 register*/
	/*
	 1. Enable TM2 Clock
	 2. Set PSC
	 3. Set ARR
	 4. Set CNT = 0
	 5. SET EGR = 1
	 */
	volatile uint32_t *enable_tim2 = (volatile uint32_t*)RCC_APB1ENR1;

	*enable_tim2 |= (1U << 0);

	volatile uint32_t *tim2_PSC = (volatile uint32_t*)TIM2_PSC;

	volatile uint32_t *tim2_ARR = (volatile uint32_t*)TIM2_ARR;

	*tim2_PSC = 169U;
	*tim2_ARR = 999U;

	volatile uint32_t *tim2_CNT = (volatile uint32_t*)TIM2_CNT;

	*tim2_CNT = 0U;

	volatile uint32_t *tim2_EGR = (volatile uint32_t*)TIM2_EGR;

	volatile uint32_t *tim2_CR1 = (volatile uint32_t*)TIM2_CR1;

	volatile uint32_t *tim2_SR = (volatile uint32_t*)TIM2_SR;

	*tim2_EGR = (1U << 0);    // force update
	*tim2_SR &= ~(1U << 0);   // clear UIF caused by UG
	/*
	 Enable UIE (bit 0) in TIM2_DIER - Update interrupt enable
	*/

	volatile uint32_t *tim2_DIER = (volatile uint32_t*)TIM2_DIER;

	*tim2_DIER |= (1U << 0);

	/* Enable Global TIM2 Interrupt in NVIC_ISER*/

	volatile uint32_t *NVIC_ISER = (volatile uint32_t*)NVIC_BASE;

	*NVIC_ISER = (1U << 28);

	*tim2_CR1 &= ~(1U << 3);   // OPM = 0
	*tim2_CR1 &= ~(1U << 1);   // UDIS = 0
	*tim2_CR1 |=  (1U << 0);   // CEN = 1

  /* USER CODE END 1

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* USER CODE BEGIN Init */


	volatile uint32_t *GPIOAEN = (volatile uint32_t *)RCC_AHB2ENR;

	volatile uint32_t *GPIO_MODER_PTR = (volatile uint32_t *)GPIO_BASE;

	volatile uint32_t *LED2_GREEN = (volatile uint32_t *)GPIO_ODR;

	*GPIOAEN |= (1U << 0);

	/* needed to clear bits to 0, "|=" can only set bits to 1. */

	*GPIO_MODER_PTR &= ~(0b11U << (5U * 2U));

	/* ------ */

	*GPIO_MODER_PTR |= (0b01U << (5 * 2));
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


  while (1)
  	{
  		/*
  		*GPIOA5_ODR |= (1U << 5);
  		delay_ms(1000);
  		*GPIOA5_ODR &= ~(1U << 5);  // clear bit 5
  		delay_ms(1000);
  		*/
  	};
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
