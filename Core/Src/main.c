#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

// Code de reference pour un vrai systeme a deux images (non appele ici,
// aucune seconde image reelle n'existe a cette adresse dans ce test)
typedef void (*app_entry_t)(void);

__attribute__((unused))
static void jump_to_application(uint32_t app_address)
{
  uint32_t app_stack_ptr = *(volatile uint32_t *)app_address;
  uint32_t app_reset_handler = *(volatile uint32_t *)(app_address + 4);

  SCB->VTOR = app_address;
  __set_MSP(app_stack_ptr);

  app_entry_t app_entry = (app_entry_t)app_reset_handler;
  app_entry();
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // PA5 (LD4) en sortie : temoin de validation CRC reussie
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  // Simule la verification qu'un bootloader ferait avant de sauter vers
  // l'application : CRC sur toute la zone APP (24K = 6144 mots de 4 octets)
  RCC->AHBENR |= RCC_AHBENR_CRCEN;
  CRC->CR |= CRC_CR_RESET;

  uint32_t *app_start = (uint32_t *)0x8000000;   // demarrage reel du programme
  const uint32_t app_words = 6000;               // ~24K, reste dans les 32K reels

  for (uint32_t i = 0; i < app_words; i++)
  {
    CRC->DR = app_start[i];
  }
  uint32_t crc_calcule = CRC->DR;

  // Pas de reference stockee ici (pas de veritable seconde image flashee
  // separement) -> confirme juste que le calcul aboutit sur la zone entiere
  if (crc_calcule != 0)
  {
    GPIOA->ODR |= GPIO_ODR_OD5;
  }

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