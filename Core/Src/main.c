#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();              // init HAL, Flash, SysTick
  SystemClock_Config();    // horloge HSI 48 MHz
  MX_GPIO_Init();          // active l'horloge du port GPIOA

  // PA8 = TIM1_CH1 (AF2) : sortie principale
  GPIOA->MODER &= ~GPIO_MODER_MODE8;
  GPIOA->MODER |= GPIO_MODER_MODE8_1;      // mode alternate function
  GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8;
  GPIOA->AFR[1] |= (2 << GPIO_AFRH_AFSEL8_Pos);  // AF2 = TIM1_CH1

  // PA7 = TIM1_CH1N (AF2) : sortie complementaire
  GPIOA->MODER &= ~GPIO_MODER_MODE7;
  GPIOA->MODER |= GPIO_MODER_MODE7_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7;
  GPIOA->AFR[0] |= (2 << GPIO_AFRL_AFSEL7_Pos);  // AF2 = TIM1_CH1N

  // Horloge de TIM1 (registre different de TIM3, meme domaine PCLK1)
  RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

  TIM1->PSC = 47;    // 48 MHz / 48 = 1 MHz
  TIM1->ARR = 999;   // 1 MHz / 1000 = 1 kHz (frequence du PWM)
  TIM1->CCR1 = 500;  // 500 / 1000 = 50% de rapport cyclique

  // Mode PWM 1 sur le canal 1
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE;   // preload sur CCR1
  TIM1->CR1   |= TIM_CR1_ARPE;      // preload sur ARR

  // Active la sortie principale ET la sortie complementaire
  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

  // Temps mort : delai de securite entre CH1 et CH1N (quelques microsecondes)
  TIM1->BDTR |= (100 << TIM_BDTR_DTG_Pos);

  // MOE : autorise physiquement les sorties du timer avance (obligatoire sur TIM1)
  TIM1->BDTR |= TIM_BDTR_MOE;

  TIM1->EGR |= TIM_EGR_UG;   // charge PSC/ARR/CCR immediatement
  TIM1->CR1 |= TIM_CR1_CEN;  // demarre le compteur

  while (1)
  {
    // rien a faire ici : le timer pilote les deux sorties tout seul
  }
}

// Configure l'horloge systeme sur HSI (voir phase 03 pour le detail)
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;    // 48 MHz
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

// Active l'horloge du port GPIO utilise (PA7 et PA8 sont tous les deux sur GPIOA)
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

// Boucle d'erreur : coupe les interruptions et bloque le programme
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}