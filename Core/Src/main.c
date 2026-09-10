/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "gpio.h"

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
//滤波器编号
#define CAN_FILTER(x) ((x) <<3)
//FIFO选择
#define CAN_FIFO_0 (0 <<2)
#define CAN_FIFO_1 (1 <<2)
//标准帧标志
#define CAN_STDID (0 <<1)
#define CAN_EXTID (1 <<1)
//数据帧标志
#define CAN_DATA_TYPE (0 <<0)
#define CAN_REMOTE_TYPE (1 <<0)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//初始化CAN模块
void CAN_Init(CAN_HandleTypeDef *hcan) {
  HAL_CAN_Start(hcan);//开启CAN通信
  __HAL_CAN_ENABLE_IT(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
  __HAL_CAN_ENABLE_IT(hcan,CAN_IT_RX_FIFO1_MSG_PENDING);//开启FIFO接收中断

}

void CAN_Filter_Mask_Config(CAN_HandleTypeDef *hcan, uint8_t Object_Para,uint32_t IDA,uint32_t MaskA,uint32_t IDB,uint32_t MaskB) {
  CAN_FilterTypeDef CAN_Filter_InitStruct;

  CAN_Filter_InitStruct.FilterIdLow = (IDA & 0x7FF) << 5;
  CAN_Filter_InitStruct.FilterIdHigh = (IDB & 0x7FF) << 5;
  CAN_Filter_InitStruct.FilterMaskIdLow = (MaskA & 0x7FF) << 5;
  CAN_Filter_InitStruct.FilterMaskIdHigh = (MaskB & 0x7FF) << 5;

  //滤波器序号
  CAN_Filter_InitStruct.FilterBank = (Object_Para >> 3) & 0x1F;
  //滤波器模式
  CAN_Filter_InitStruct.FilterMode = CAN_FILTERMODE_IDMASK;
  //32位滤波
  CAN_Filter_InitStruct.FilterScale = CAN_FILTERSCALE_16BIT;
  //使能滤波器
  CAN_Filter_InitStruct.FilterActivation = ENABLE;
  //滤波器绑定FIFO
  CAN_Filter_InitStruct.FilterFIFOAssignment = (Object_Para >> 2) & 0x01;
  HAL_CAN_ConfigFilter(hcan, &CAN_Filter_InitStruct);
}


uint8_t CAN_Transmit(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Data,uint16_t Length) {
  CAN_TxHeaderTypeDef TxHeader;
  uint32_t TxMailbox;

  TxHeader.StdId =ID;
  TxHeader.DLC = Length;
  TxHeader.IDE = 0;
  TxHeader.RTR = 0;
  TxHeader.ExtId = 0;


  return    (HAL_CAN_AddTxMessage(hcan, &TxHeader, Data, &TxMailbox));

}


void LED_Control(uint8_t Data) {

  if (Data%2 ==1) {
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
  }
else {
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
}
}


void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t Data = 0;
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, &Data);

    LED_Control(Data);

}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  uint8_t Send_Data = 0;
  CAN_Init(&hcan);
  CAN_Filter_Mask_Config(&hcan,CAN_FILTER(13)|CAN_FIFO_1|CAN_STDID|CAN_DATA_TYPE,0x114,0x7ff,0x200,0x7ff);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    Send_Data++;
    CAN_Transmit(&hcan,0x200,&Send_Data,1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(300);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL15;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
