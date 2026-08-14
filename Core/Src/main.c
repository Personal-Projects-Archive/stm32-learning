#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // PA5 (LD4) en sortie, pilotee par les commandes recues sur le terminal
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  // PA2 = USART2_TX (AF1)
  GPIOA->MODER &= ~GPIO_MODER_MODE2;
  GPIOA->MODER |= GPIO_MODER_MODE2_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos);

  // PA3 = USART2_RX (AF1)
  GPIOA->MODER &= ~GPIO_MODER_MODE3;
  GPIOA->MODER |= GPIO_MODER_MODE3_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL3_Pos);

  // USART2 a 115200 bauds, 8 bits, 1 stop, pas de parite (8N1)
  RCC->APBENR1 |= RCC_APBENR1_USART2EN;
  USART2->BRR = 48000000UL / 115200UL;
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
  USART2->CR1 |= USART_CR1_UE;

  // Message de bienvenue, envoye caractere par caractere
  const char msg[] = "Terminal pret. Tape 'o' pour allumer la LED, 'f' pour l'eteindre.\r\n";
  for (uint32_t i = 0; msg[i] != '\0'; i++)
  {
    while (!(USART2->ISR & USART_ISR_TXFE)) { }   // attend que le FIFO TX soit libre
    USART2->TDR = msg[i];
  }

  while (1)
  {
    while (!(USART2->ISR & USART_ISR_RXFF)) { }   // attend un caractere recu
    char c = (char)USART2->RDR;

    // echo : renvoie le caractere tape, pour le voir apparaitre dans le terminal
    while (!(USART2->ISR & USART_ISR_TXFE)) { }
    USART2->TDR = c;

    if (c == 'o') { GPIOA->ODR |= GPIO_ODR_OD5; }
    if (c == 'f') { GPIOA->ODR &= ~GPIO_ODR_OD5; }
  }
}

// Configure l'horloge systeme sur HSI (voir phase 11 pour le detail)
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

// Active l'horloge du port GPIO utilise (LED et USART sont tous les deux sur A)
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