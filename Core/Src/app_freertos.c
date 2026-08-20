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

/* USER CODE END Variables */
/* Definitions for TaskButton */
osThreadId_t TaskButtonHandle;
const osThreadAttr_t TaskButton_attributes = {
  .name = "TaskButton",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 256 * 4
};
/* Definitions for TaskBlink */
osThreadId_t TaskBlinkHandle;
const osThreadAttr_t TaskBlink_attributes = {
  .name = "TaskBlink",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for QueueButton */
osMessageQueueId_t QueueButtonHandle;
const osMessageQueueAttr_t QueueButton_attributes = {
  .name = "QueueButton"
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

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of QueueButton */
  QueueButtonHandle = osMessageQueueNew (1, sizeof(uint32_t), &QueueButton_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of TaskButton */
  TaskButtonHandle = osThreadNew(StartDefaultTask, NULL, &TaskButton_attributes);

  /* creation of TaskBlink */
  TaskBlinkHandle = osThreadNew(StartTaskBlink, NULL, &TaskBlink_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  if (TaskButtonHandle == NULL || TaskBlinkHandle == NULL)
  {
    // Une creation de tache a echoue (tas insuffisant, tres probablement)
    while (1) { }
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the TaskButton thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN TaskButton */
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  uint32_t recu;
  /* Infinite loop */
  for(;;)
  {
    //osDelay(1);
    // Bloque ici tant que rien n'arrive dans la queue -> aucun CPU consomme en attendant
    if (osMessageQueueGet(QueueButtonHandle, &recu, NULL, osWaitForever) == osOK)
    {
      GPIOA->ODR ^= GPIO_ODR_OD5;
    }
  }
  /* USER CODE END TaskButton */
}

/* USER CODE BEGIN Header_StartTaskBlink */
/**
* @brief Function implementing the TaskBlink thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBlink */
void StartTaskBlink(void *argument)
{
  /* USER CODE BEGIN TaskBlink */
  GPIOA->MODER &= ~GPIO_MODER_MODE6;
  GPIOA->MODER |= GPIO_MODER_MODE6_0;
  /* Infinite loop */
  for(;;)
  {
    //osDelay(1);
    GPIOA->ODR ^= GPIO_ODR_OD6;
    osDelay(500);   // rend la main au scheduler, pas d'attente active
  }
  /* USER CODE END TaskBlink */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

