/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile uint32_t valeur_partagee = 0;
/* USER CODE END Variables */
/* Definitions for TaskSensor */
osThreadId_t TaskSensorHandle;
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for TaskControl */
osThreadId_t TaskControlHandle;
const osThreadAttr_t TaskControl_attributes = {
  .name = "TaskControl",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 256 * 4
};
/* Definitions for TaskMonitor */
osThreadId_t TaskMonitorHandle;
const osThreadAttr_t TaskMonitor_attributes = {
  .name = "TaskMonitor",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for MutexShared */
osMutexId_t MutexSharedHandle;
const osMutexAttr_t MutexShared_attributes = {
  .name = "MutexShared"
};
/* Definitions for QueueSensorControl */
osMessageQueueId_t QueueSensorControlHandle;
const osMessageQueueAttr_t QueueSensorControl_attributes = {
  .name = "QueueSensorControl"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* creation of MutexShared */
  MutexSharedHandle = osMutexNew(&MutexShared_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of QueueSensorControl */
  QueueSensorControlHandle = osMessageQueueNew (1, sizeof(uint32_t), &QueueSensorControl_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of TaskSensor */
  TaskSensorHandle = osThreadNew(StartTaskSensor, NULL, &TaskSensor_attributes);

  /* creation of TaskControl */
  TaskControlHandle = osThreadNew(StartTaskControl, NULL, &TaskControl_attributes);

  /* creation of TaskMonitor */
  TaskMonitorHandle = osThreadNew(StartTaskMonitor, NULL, &TaskMonitor_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  if (TaskSensorHandle == NULL || TaskControlHandle == NULL || TaskMonitorHandle == NULL)
  {
    // Une creation de tache a echoue (tas insuffisant, tres probablement)
    while (1) { }
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartTaskSensor */
/**
* @brief Function implementing the TaskSensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSensor */
void StartTaskSensor(void *argument)
{
  /* USER CODE BEGIN TaskSensor */

  GPIOA->MODER |= GPIO_MODER_MODE0;   // PA0 analogique

  RCC->APBENR2 |= RCC_APBENR2_ADCEN;
  ADC1->CR |= ADC_CR_ADVREGEN;
  osDelay(2);

  uint32_t timeout;

  timeout = 100000;
  ADC1->CR |= ADC_CR_ADCAL;
  while ((ADC1->CR & ADC_CR_ADCAL) && --timeout) { }

  timeout = 100000;
  ADC1->CR |= ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY) && --timeout) { }

  ADC1->CHSELR |= ADC_CHSELR_CHSEL0;
  ADC1->SMPR = 0x7;

  for (;;)
  {
    uint32_t valeur;

    timeout = 100000;
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC) && --timeout) { }

    if (timeout == 0)
    {
      valeur = 0;
    }
    else
    {
      valeur = ADC1->DR;
    }

    osMessageQueuePut(QueueSensorControlHandle, &valeur, 0, 0);
    osDelay(200);
  }
  /* USER CODE END TaskSensor */
}

/* USER CODE BEGIN Header_StartTaskControl */
/**
* @brief Function implementing the TaskControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskControl */
void StartTaskControl(void *argument)
{
  /* USER CODE BEGIN TaskControl */
  // PA6 en Alternate Function 1 = TIM3_CH1 (PWM)
  GPIOA->MODER &= ~GPIO_MODER_MODE6;
  GPIOA->MODER |= GPIO_MODER_MODE6_1;
  GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6;
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL6_Pos);

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
  /* Infinite loop */
  for(;;)
  {
    //osDelay(1);
    uint32_t valeur;
    if (osMessageQueueGet(QueueSensorControlHandle, &valeur, NULL, osWaitForever) == osOK)
    {
      TIM3->CCR1 = valeur;

      osMutexAcquire(MutexSharedHandle, osWaitForever);
      valeur_partagee = valeur;
      osMutexRelease(MutexSharedHandle);
    }
  }
  /* USER CODE END TaskControl */
}

/* USER CODE BEGIN Header_StartTaskMonitor */
/**
* @brief Function implementing the TaskMonitor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMonitor */
void StartTaskMonitor(void *argument)
{
  /* USER CODE BEGIN TaskMonitor */
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;
  /* Infinite loop */
  for(;;)
  {
    //osDelay(1);
    osMutexAcquire(MutexSharedHandle, osWaitForever);
    uint32_t valeur_locale = valeur_partagee;
    osMutexRelease(MutexSharedHandle);

    if (valeur_locale > 2048)
    {
      GPIOA->ODR |= GPIO_ODR_OD5;
    }
    else
    {
      GPIOA->ODR &= ~GPIO_ODR_OD5;
    }

    osDelay(300);
  }
  /* USER CODE END TaskMonitor */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

