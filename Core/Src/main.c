#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();              // init HAL, Flash, SysTick (utilise par HAL_GetTick)
  SystemClock_Config();    // configure l'horloge (HSI, voir phase 3)
  MX_GPIO_Init();          // active les horloges des ports GPIOA et GPIOC

  // PA5 (LD4, LED embarquee) en sortie push-pull
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  // PC13 (bouton externe) en entree, pull-up interne active
  GPIOC->MODER &= ~GPIO_MODER_MODE13;
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
  GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;

  // Route la ligne EXTI13 sur le port C (pas de SYSCFG sur cette famille)
  EXTI->EXTICR[3] &= ~(0xFFUL << 8);
  EXTI->EXTICR[3] |= (2UL << 8);     // 2 = port C

  EXTI->FTSR1 |= (1UL << 13);        // declenche sur front descendant uniquement
  EXTI->IMR1  |= (1UL << 13);        // demasque la ligne 13 cote peripherique
  NVIC_EnableIRQ(EXTI4_15_IRQn);     // autorise la ligne cote coeur (NVIC)

  // Rien a faire ici : toute la logique se passe dans l'interruption
  // (voir Core/Src/stm32c0xx_it.c, fonction EXTI4_15_IRQHandler)
  while (1)
  {
  }
}

// Configure l'horloge systeme sur HSI (voir phase 3 pour le detail)
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;              // 48 MHz
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

// Active les horloges des ports GPIO utilises (LED sur A, bouton sur C)
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
}

// Boucle d'erreur : coupe les interruptions et bloque le programme
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}