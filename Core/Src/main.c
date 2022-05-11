/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "stm32f1xx_it.h"
#include "ESP8226.h"
#include "DHT22.h"
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

/* USER CODE BEGIN PV */
uint8_t sendbuf[] = "Looping...\r\n";
uint8_t startMsg[] = "Stm32F103C8T6 started.\r\n";
uint16_t Temp = 0;
uint16_t Humi = 0;
uint8_t err;

float Tempf;
float Humif;
char TempChar[8]; //存放转换结果
char HumiChar[8]; //存放转换结果
char THMsg[31] = ""; //5+8+2+5+8+2 Temp = xxxx
char THMsg2[30] = "Temp:00000000, Humi:00000000\r\n"; //5+8+2+5+8+2 Temp = xxxx
uint8_t senddata[4];

uint8_t aTxStartMessages[] = "\r\n******UART communication using IT******\r\nPlease enter 10 characters:\r\n";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
//  USB_Plug_Init(); // unplug and plug the usb to allow re-detection of MCU
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

//  memset(ESP_RxBuffer,0,sizeof(ESP_RxBuffer));
  HAL_Delay(1000);
  ESP_Init(1);
  ESP_Init_TT();
  HAL_Delay(200);
  DHT22_Init();

//  CDC_Transmit_FS(startMsg,sizeof(startMsg));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
   {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  HAL_Delay(3000);


 	  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1);

 	 err = DHT22_Get_Data(&Temp,&Humi);
 	 Tempf = Temp/10.0f;
 	 Humif = Humif/10.0f;
 	 senddata[0] = (Temp>>8) & 0xff;
 	 senddata[1] = (Temp) & 0xff;
 	 senddata[2] = (Humi>>8) & 0xff;
 	 senddata[3] = (Humi) & 0xff;
 	 ESP_Send_Data_TT(senddata,4); // transparent transmission
// 	 ESP_Send_Data_4Bytes(senddata); // 单次传输


	DHT22_THMsg(Temp,Humi,THMsg);

 	CDC_Transmit_FS((uint8_t*)THMsg,sizeof(THMsg));
 	HAL_Delay(20);
// 	CDC_Transmit_FS((uint8_t*)THMsg2,sizeof(THMsg2));


//	CDC_Transmit_FS((uint8_t*)TempChar,sizeof(TempChar));
//	CDC_Transmit_FS(" ",1);
//	CDC_Transmit_FS((uint8_t*)HumiChar,sizeof(HumiChar));
//	HAL_Delay(20);
//	CDC_Transmit_FS("\r\n",2);
//
// 	  err = DHT22_Get_Data(Temp,Hum);
// 	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
// 	  CDC_Transmit_FS(sendbuf,sizeof(sendbuf));
// 	 HAL_UART_Transmit_DMA(&huart2 ,(uint8_t*)AT_Test,sizeof(AT_Test)); //test at




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

/* USER CODE BEGIN 4 */




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
