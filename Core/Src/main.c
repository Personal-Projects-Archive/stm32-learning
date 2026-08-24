#include "main.h"
#include <string.h>

#define FW_STAGING_ADDR   0x08007000UL   // 4 derniers Ko de la Flash (pages 14-15)
#define FW_STAGING_PAGE   14U
#define FW_STAGING_PAGES  2U
#define FW_MAX_SIZE       4096U

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

static uint8_t uart_read_byte(void)
{
  while (!(USART2->ISR & USART_ISR_RXFF)) { }
  return (uint8_t)USART2->RDR;
}

static void uart_send_byte(uint8_t b)
{
  while (!(USART2->ISR & USART_ISR_TXFE)) { }
  USART2->TDR = b;
}

static void uart_send_string(const char *s)
{
  for (uint32_t i = 0; s[i] != '\0'; i++)
  {
    uart_send_byte((uint8_t)s[i]);
  }
}

// CRC32 materiel sur un buffer en RAM
static uint32_t compute_crc(const uint8_t *data, uint32_t size)
{
  RCC->AHBENR |= RCC_AHBENR_CRCEN;
  CRC->CR |= CRC_CR_RESET;

  uint32_t words = size / 4;
  const uint32_t *data32 = (const uint32_t *)data;
  for (uint32_t i = 0; i < words; i++)
  {
    CRC->DR = data32[i];
  }
  return CRC->DR;
}

// CRC32 materiel directement sur une zone Flash (lecture d'adresse physique)
static uint32_t compute_crc_flash(uint32_t address, uint32_t size)
{
  RCC->AHBENR |= RCC_AHBENR_CRCEN;
  CRC->CR |= CRC_CR_RESET;

  uint32_t words = size / 4;
  const uint32_t *data32 = (const uint32_t *)address;
  for (uint32_t i = 0; i < words; i++)
  {
    CRC->DR = data32[i];
  }
  return CRC->DR;
}

static uint8_t firmware_buffer[FW_MAX_SIZE];

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // PA5 (LD4) : temoin de reussite de la mise a jour
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  // PA2 = USART2_TX (AF1), PA3 = USART2_RX (AF1)
  GPIOA->MODER &= ~GPIO_MODER_MODE2;
  GPIOA->MODER |= GPIO_MODER_MODE2_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos);

  GPIOA->MODER &= ~GPIO_MODER_MODE3;
  GPIOA->MODER |= GPIO_MODER_MODE3_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL3_Pos);

  RCC->APBENR1 |= RCC_APBENR1_USART2EN;
  USART2->BRR = 48000000UL / 115200UL;
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
  USART2->CR1 |= USART_CR1_UE;

  uart_send_string("Pret a recevoir un firmware (taille 4o, donnees, CRC32 4o)\r\n");

  // Etape 1 : taille (4 octets, little-endian)
  uint32_t fw_size = 0;
  for (int i = 0; i < 4; i++)
  {
    fw_size |= ((uint32_t)uart_read_byte()) << (8 * i);
  }

  if (fw_size == 0 || fw_size > FW_MAX_SIZE || (fw_size % 8) != 0)
  {
    uart_send_string("Taille invalide (non nulle, <=4096, multiple de 8)\r\n");
    while (1) { }
  }

  // Etape 2 : donnees, dans un buffer RAM
  for (uint32_t i = 0; i < fw_size; i++)
  {
    firmware_buffer[i] = uart_read_byte();
  }

  // Etape 3 : CRC de reference (4 octets, little-endian)
  uint32_t crc_recu = 0;
  for (int i = 0; i < 4; i++)
  {
    crc_recu |= ((uint32_t)uart_read_byte()) << (8 * i);
  }

  // Etape 4 : verification AVANT d'ecrire quoi que ce soit en Flash
  uint32_t crc_calcule_ram = compute_crc(firmware_buffer, fw_size);
  if (crc_calcule_ram != crc_recu)
  {
    uart_send_string("CRC invalide, mise a jour annulee, Flash non touchee\r\n");
    while (1) { }
  }

  // Etape 5 : ecriture Flash (zone reservee, ne touche jamais au code en cours d'execution)
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef erase_config;
  erase_config.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_config.Page = FW_STAGING_PAGE;
  erase_config.NbPages = FW_STAGING_PAGES;

  uint32_t page_error;
  if (HAL_FLASHEx_Erase(&erase_config, &page_error) != HAL_OK)
  {
    HAL_FLASH_Lock();
    uart_send_string("Echec d'effacement, mise a jour annulee\r\n");
    while (1) { }
  }

  HAL_StatusTypeDef write_status = HAL_OK;
  for (uint32_t i = 0; i < fw_size; i += 8)
  {
    uint64_t mot_double;
    memcpy(&mot_double, &firmware_buffer[i], 8);
    write_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FW_STAGING_ADDR + i, mot_double);
    if (write_status != HAL_OK) { break; }
  }

  HAL_FLASH_Lock();

  if (write_status != HAL_OK)
  {
    uart_send_string("Echec d'ecriture en cours de route\r\n");
    while (1) { }
  }

  // Etape 6 : revalidation APRES ecriture, lue directement depuis la Flash
  uint32_t crc_final = compute_crc_flash(FW_STAGING_ADDR, fw_size);
  if (crc_final == crc_recu)
  {
    uart_send_string("Mise a jour ecrite et validee avec succes\r\n");
    GPIOA->ODR |= GPIO_ODR_OD5;
  }
  else
  {
    uart_send_string("Echec de validation post-ecriture\r\n");
  }

  while (1) { }
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