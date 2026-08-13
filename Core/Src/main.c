#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // PA0 en entree analogique (potentiometre)
  GPIOA->MODER |= GPIO_MODER_MODE0;   // force les 2 bits a 1 = analogique

  // PA6 en Alternate Function 1 = TIM3_CH1 (LED, sortie PWM)
  GPIOA->MODER &= ~GPIO_MODER_MODE6;
  GPIOA->MODER |= GPIO_MODER_MODE6_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL6_Pos);

  // TIM3 en PWM, ARR = 4095 pour matcher directement la resolution 12 bits
  // de l'ADC (pas de mise a l'echelle necessaire entre les deux)
  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM3->PSC = 0;
  TIM3->ARR = 4095;
  TIM3->CCR1 = 0;
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
  TIM3->CR1   |= TIM_CR1_ARPE;
  TIM3->CCER  |= TIM_CCER_CC1E;
  TIM3->EGR   |= TIM_EGR_UG;
  TIM3->CR1   |= TIM_CR1_CEN;

  // Horloge de l'ADC
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;

  // Regulateur interne de l'ADC, obligatoire avant calibration sur cette famille
  ADC1->CR |= ADC_CR_ADVREGEN;
  for (volatile uint32_t i = 0; i < 240; i++) { __NOP(); }   // t_ADCVREG_STUP

  // Calibration
  ADC1->CR |= ADC_CR_ADCAL;
  while (ADC1->CR & ADC_CR_ADCAL) { }

  // Activation de l'ADC
  ADC1->CR |= ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY)) { }

  // Canal 0 = PA0, temps d'echantillonnage maximal
  ADC1->CHSELR |= ADC_CHSELR_CHSEL0;
  ADC1->SMPR = 0x7;

  while (1)
  {
    // Lance une conversion et attend la fin (EOC = End Of Conversion)
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC)) { }

    // Recopie directe : la valeur brute ADC devient le rapport cyclique PWM
    TIM3->CCR1 = ADC1->DR;
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