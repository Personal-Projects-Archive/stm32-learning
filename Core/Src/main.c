#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* PA6 en Alternate Function, AF1 = TIM3_CH1 */
  GPIOA->MODER &= ~GPIO_MODER_MODE6;
  GPIOA->MODER |= GPIO_MODER_MODE6_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL6_Pos);

  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM3->PSC = 47;
  TIM3->ARR = 999;
  TIM3->CCR1 = 500;                                  /* 50% */
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /* PWM mode 1 (110) */
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                     /* preload sur CCR1 */
  TIM3->CR1   |= TIM_CR1_ARPE;
  TIM3->CCER  |= TIM_CCER_CC1E;                       /* sortie activee */
  TIM3->EGR   |= TIM_EGR_UG;                          /* charge PSC/ARR/CCR immediatement */
  TIM3->CR1   |= TIM_CR1_CEN;

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