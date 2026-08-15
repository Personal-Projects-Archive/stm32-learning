#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // PA4 en sortie classique : signal de verrouillage (STCP) du 74HC595
  GPIOA->MODER &= ~GPIO_MODER_MODE4;
  GPIOA->MODER |= GPIO_MODER_MODE4_0;

  // PA5 = SPI1_SCK (AF0)
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_1;

  // PA7 = SPI1_MOSI (AF0)
  GPIOA->MODER &= ~GPIO_MODER_MODE7;
  GPIOA->MODER |= GPIO_MODER_MODE7_1;
  // AF0 = valeur par defaut du registre AFR, rien a ecrire explicitement

  // Configuration de SPI1 : maitre, CS logiciel, trame 8 bits
  RCC->APBENR2 |= RCC_APBENR2_SPI1EN;

  SPI1->CR1 = 0;
  SPI1->CR1 |= SPI_CR1_MSTR;                // mode maitre
  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;   // pas de NSS materiel, gere par logiciel
  SPI1->CR1 |= (2 << SPI_CR1_BR_Pos);       // 48MHz / 8 = 6 MHz

  SPI1->CR2 = 0;
  SPI1->CR2 |= (7 << SPI_CR2_DS_Pos);       // trame de 8 bits (0111)
  SPI1->CR2 |= SPI_CR2_FRXTH;               // seuil de reception adapte a 8 bits

  SPI1->CR1 |= SPI_CR1_SPE;                 // active SPI1

  uint8_t pattern = 0x01;

  while (1)
  {
    // envoie un octet
    while (!(SPI1->SR & SPI_SR_TXE)) { }
    *(volatile uint8_t *)&SPI1->DR = pattern;
    while (SPI1->SR & SPI_SR_BSY) { }

    // impulsion de verrouillage : fait apparaitre l'octet sur les sorties Q0-Q7
    GPIOA->ODR |= GPIO_ODR_OD4;
    GPIOA->ODR &= ~GPIO_ODR_OD4;

    // fait avancer le bit d'une position (retombe a 0x01 apres Q3)
    pattern <<= 1;
    if (pattern > 0x08) { pattern = 0x01; }

    for (volatile uint32_t i = 0; i < 500000; i++) { __NOP(); }
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

// Active l'horloge du port GPIO utilise
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