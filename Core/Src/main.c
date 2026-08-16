#include "main.h"

#define MPU6050_ADDR      0x68
#define WHO_AM_I_REG      0x75
#define ACCEL_XOUT_H_REG  0x3B

I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);


/* Attend qu'un événement I2C soit détecté. */
static void i2c_wait(volatile uint32_t *reg, uint32_t mask)
{
  uint32_t timeout = 2000000;

  while (!(*reg & mask))
  {
    if (--timeout == 0)
    {
      /* Clignotement de la LED en cas de blocage I2C. */
      while (1)
      {
        GPIOA->ODR ^= GPIO_ODR_OD5;

        for (volatile uint32_t i = 0; i < 80000; i++)
        {
          __NOP();
        }
      }
    }
  }
}


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* PA5 : LED LD4. */
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  MX_I2C1_Init();

  /* Lecture du registre WHO_AM_I pour vérifier le MPU6050. */
  I2C1->CR2 = 0;
  I2C1->CR2 |= (MPU6050_ADDR << 1);
  I2C1->CR2 |= (1 << 16);
  I2C1->CR2 |= I2C_CR2_START;

  i2c_wait(&I2C1->ISR, I2C_ISR_TXIS);
  I2C1->TXDR = WHO_AM_I_REG;
  i2c_wait(&I2C1->ISR, I2C_ISR_TC);

  I2C1->CR2 = 0;
  I2C1->CR2 |= (MPU6050_ADDR << 1) | I2C_CR2_RD_WRN;
  I2C1->CR2 |= (1 << 16);
  I2C1->CR2 |= I2C_CR2_AUTOEND;
  I2C1->CR2 |= I2C_CR2_START;

  i2c_wait(&I2C1->ISR, I2C_ISR_RXNE);
  uint8_t who_am_i = (uint8_t)I2C1->RXDR;

  i2c_wait(&I2C1->ISR, I2C_ISR_STOPF);
  I2C1->ICR |= I2C_ICR_STOPCF;

  /* Le MPU6050 répond avec 0x68. */
  if (who_am_i == 0x68)
  {
    GPIOA->ODR |= GPIO_ODR_OD5;
  }


  /* PA6 en AF1 : TIM3_CH1 pour la PWM. */
  GPIOA->MODER &= ~GPIO_MODER_MODE6;
  GPIOA->MODER |= GPIO_MODER_MODE6_1;

  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL6_Pos);

  __HAL_RCC_TIM3_CLK_ENABLE();

  /* PWM sur 12 bits : 0 à 4095. */
  TIM3->PSC = 0;
  TIM3->ARR = 4095;
  TIM3->CCR1 = 0;

  TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
  TIM3->CR1   |= TIM_CR1_ARPE;
  TIM3->CCER  |= TIM_CCER_CC1E;
  TIM3->EGR   |= TIM_EGR_UG;
  TIM3->CR1   |= TIM_CR1_CEN;


  while (1)
  {
    /* Sélection du registre de l'accélération X. */
    I2C1->CR2 = 0;
    I2C1->CR2 |= (MPU6050_ADDR << 1);
    I2C1->CR2 |= (1 << 16);
    I2C1->CR2 |= I2C_CR2_START;

    i2c_wait(&I2C1->ISR, I2C_ISR_TXIS);
    I2C1->TXDR = ACCEL_XOUT_H_REG;
    i2c_wait(&I2C1->ISR, I2C_ISR_TC);

    /* Lecture des deux octets de l'accélération X. */
    I2C1->CR2 = 0;
    I2C1->CR2 |= (MPU6050_ADDR << 1) | I2C_CR2_RD_WRN;
    I2C1->CR2 |= (2 << 16);
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;

    i2c_wait(&I2C1->ISR, I2C_ISR_RXNE);
    uint8_t hi = (uint8_t)I2C1->RXDR;

    i2c_wait(&I2C1->ISR, I2C_ISR_RXNE);
    uint8_t lo = (uint8_t)I2C1->RXDR;

    i2c_wait(&I2C1->ISR, I2C_ISR_STOPF);
    I2C1->ICR |= I2C_ICR_STOPCF;

    /* Reconstitution de la valeur signée sur 16 bits. */
    int16_t accel_x = (int16_t)((hi << 8) | lo);

    /* Conversion vers la plage PWM 0 à 4095. */
    TIM3->CCR1 = (uint16_t)(((int32_t)accel_x + 32768) >> 4);
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

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10805D88;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
}


void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
  }
}