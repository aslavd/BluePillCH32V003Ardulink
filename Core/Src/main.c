/* USER CODE BEGIN Header */

/*

 Ardulink 1-Wire WCH (CH32V003) based on STM32F103C6T6 USB CDC, made with STM32CubeIDE 1.7.0
 VSlaon variant

 SDI pin at GPIOB GPIO_PIN_9

 TIM3 used for SWIO delays
 APB1 clock 24Mhz
 TIM3 clock 24MHz, 41.67ns
*/


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "VSL_Buffer.h"
#include "VSL_WCH_SDI.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PROTOCOL_START     '!'
#define PROTOCOL_ACK       '+'
#define PROTOCOL_TEST      '?'
#define PROTOCOL_POWER_ON  'p'
#define PROTOCOL_POWER_OFF 'P'
#define PROTOCOL_WRITE_REG 'w'
#define PROTOCOL_READ_REG  'r'

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
extern uint8_t cnt2Hz;
extern uint8_t flag2Hz;
extern uint8_t usbRxFlag;
extern uint8_t usbRxLen;

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[APP_RX_DATA_SIZE];

uint8_t fFirst0 = 0;
uint8_t tmp01 = 0;

RingBuffer rxBuffA;
uint8_t rxCmd = 0;
uint8_t rxCmdData = 0;
uint8_t rxCmdDataLen = 0;
uint8_t rxCmdDataCnt = 0;
uint8_t rxCmdBuff[8];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */


uint32_t u8tou32(uint8_t *p);
void u32tou8(uint32_t u32In, uint8_t *p);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t c1, len1;
  uint8_t *ptr1;
  uint8_t swioReg;
  uint32_t swioVal;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  HAL_InitTick(TICK_INT_PRIORITY);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);	// LED1 = ON
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim3);

  rb_init(&rxBuffA, 256);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);	// LED = ON
  WCH_SDI_Init();
  UserTxBufferFS[0] = PROTOCOL_START;
  CDC_Transmit_FS(&UserTxBufferFS[0], 1);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);		// LED = OFF

  /*
  WCH_Delay_Ms(1000);
  WCH_Delay_Ms(1000);


  __disable_irq();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  //WCH_SDI_Write(0xF0, 0xF0F1F2F3);
  swioVal = WCH_SDI_Read(0xF0);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
  __enable_irq();
 */


  while (1)
  {
	  if (usbRxFlag == 1)	// some data received over USB CDC
	  {
		  usbRxFlag = 0;
		  rb_write(&rxBuffA, &UserRxBufferFS[0], usbRxLen);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);	// LED = ON

		  if (rxCmd == 0)	// no command, means ready for command
		  {
			  c1 = rb_read_byte(&rxBuffA);

			  if (c1 == PROTOCOL_TEST)	// single byte command, execute right here
			  {
				  rxCmd = c1;
				  //send answer here ...
				  UserTxBufferFS[0] = PROTOCOL_ACK;
				  CDC_Transmit_FS(&UserTxBufferFS[0], 1);
				  rxCmd = 0;

			  }

			  if (c1 == PROTOCOL_POWER_ON)	// single byte command, execute right here
			  {
				  rxCmd = c1;
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
				  //send answer ...
				  UserTxBufferFS[0] = PROTOCOL_ACK;
				  CDC_Transmit_FS(&UserTxBufferFS[0], 1);
				  rxCmd = 0;

			  }

			  if (c1 == PROTOCOL_POWER_OFF)	// single byte command, execute right here
			  {
				  rxCmd = c1;
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  //send answer ...
				  UserTxBufferFS[0] = PROTOCOL_ACK;
				  CDC_Transmit_FS(&UserTxBufferFS[0], 1);
				  rxCmd = 0;
			  }

			  if (c1 == PROTOCOL_WRITE_REG)	// need to read 1uint8 + 1uint32 bytes
			  {
				  if (rxCmdData == 0)
				  {
					  rxCmd = c1;
					  rxCmdDataLen = 5;
					  rxCmdDataCnt = 0;
					  rxCmdData = 1;
				  }
			  }

			  if (c1 == PROTOCOL_READ_REG)	// need to read 1uint8, bytes
			  {
				  if (rxCmdData == 0)
				  {
					  rxCmd = c1;
					  rxCmdDataLen = 1;
					  rxCmdDataCnt = 0;
					  rxCmdData = 1;
				  }
			  }
		  }

			if (rxCmdData == 1)
			{
				if (rxBuffA.count > 0) // have available data
				{
					len1 = rb_read(&rxBuffA, &rxCmdBuff[0]+rxCmdDataCnt, rxCmdDataLen-rxCmdDataCnt);
					rxCmdDataCnt += len1;
				}

				if (rxCmdDataCnt == rxCmdDataLen)
				{
					// command data received for rxCmd
					//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);	// LED1 = ON
					if (rxCmd == PROTOCOL_WRITE_REG)
					{
						// do command
						swioReg = rxCmdBuff[0];
						ptr1 = &rxCmdBuff[1];
						swioVal = u8tou32(ptr1);
						// disable interrupts ???
						__disable_irq();
						WCH_SDI_Write(swioReg, swioVal);
						__enable_irq();
						// enable interrupts ???
						UserTxBufferFS[0] = PROTOCOL_ACK;
						//send answer ...
						CDC_Transmit_FS(&UserTxBufferFS[0], 1);

					}

					if (rxCmd == PROTOCOL_READ_REG)
					{
						// do command
						//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);	// LED1 = ON
						swioReg = rxCmdBuff[0];
						// disable interrupts ???
						__disable_irq();
						swioVal = WCH_SDI_Read(swioReg);
						__enable_irq();
						// enable interrupts ???

						//u32tou8(swioVal, &UserTxBufferFS[0]);
						//send answer ...
						u32tou8(swioVal, &UserTxBufferFS[0]);
						CDC_Transmit_FS(&UserTxBufferFS[0], 4);

					}

					rxCmd = 0;
					rxCmdData = 0;
				}
			  }

		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);	// LED = OFF
	  }


	  if (flag2Hz == 1)
	  {
	  	flag2Hz = 0;
	  	if (fFirst0 == 0)
	  	{
	  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);	// LED1 = OFF
	  		fFirst0 = 1;
	  	}

	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

uint32_t u8tou32(uint8_t *p)
{
    // buff: L0 H0 L1 H1
	uint32_t u32Tmp;

    u32Tmp = p[3];
    u32Tmp <<= 8;
    u32Tmp += p[2];
    u32Tmp <<= 8;
    u32Tmp += p[1];
    u32Tmp <<= 8;
    u32Tmp += p[0];

    return u32Tmp;
}

void u32tou8(uint32_t u32In, uint8_t *p)
{
	// buff: L0 H0 L1 H1
	p[0] = u32In;
    u32In >>= 8;
    p[1] = u32In;
    u32In >>= 8;
    p[2] = u32In;
    u32In >>= 8;
    p[3] = u32In;
}

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
