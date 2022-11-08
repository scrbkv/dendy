/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "CPU.h"
#include "PPU.h"
#include "BUS.h"

#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
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
/* Definitions for Default_Task */
osThreadId_t Default_TaskHandle;
uint32_t defaultTaskBuffer[ 4096 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t Default_Task_attributes = {
  .name = "Default_Task",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Start_Default_Task(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Default_Task */
  Default_TaskHandle = osThreadNew(Start_Default_Task, NULL, &Default_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Start_Default_Task */
/**
  * @brief  Function implementing the Default_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_Default_Task */
void Start_Default_Task(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN Start_Default_Task */
  CPU_reset();
  PPU_reset();

  LcdInit();
  	FindCartridges();
  	LoadSelectedCatrirdge();
  	if (cartridgeIsLoaded == 0)
  		while (1) osDelay(1000);


  	  for (int i = 0; i < 10; ++i) {
  		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
  		  HAL_Delay(100);
  	  }

  	controller_state[0] = 0xFF;
  	controller_state[1] = 0xFF;

  while (1) {
	  CPU_reset();
	  PPU_reset();

	      while(1) {
	          if(dma_transfer) {
	              dma_clock();
	          } else {
	              CPU_clock();
	          }


	          PPU_clock();
	          PPU_clock();
	          PPU_clock();

	          if(nmi){
	              nmi = 0;
	              CPU_NMI();
	          }
	      }
  }

  /* USER CODE END Start_Default_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

