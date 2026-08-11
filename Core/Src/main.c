#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM3->PSC = 47999;              /* 48 MHz / 48000 = 1 kHz */
  TIM3->ARR = 999;                /* 1 kHz / 1000 = 1 Hz */
  TIM3->EGR |= TIM_EGR_UG;        /* precharge PSC/ARR immediatement */
  TIM3->SR &= ~TIM_SR_UIF;        /* efface le flag que l'update forcee vient de mettre */
  TIM3->DIER |= TIM_DIER_UIE;     /* autorise l'interruption d'update */
  TIM3->CR1 |= TIM_CR1_CEN;       /* demarre le compteur */
  NVIC_EnableIRQ(TIM3_IRQn);      /* autorise cote coeur (NVIC) */

  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

/* USER CODE BEGIN 4 */
void TIM3_IRQHandler(void)
{
  if (TIM3->SR & TIM_SR_UIF)
  {
    TIM3->SR &= ~TIM_SR_UIF;
    GPIOA->ODR ^= GPIO_ODR_OD5;
  }
}
/* USER CODE END 4 */