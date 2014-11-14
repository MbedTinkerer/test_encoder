#include <stm32f4_discovery.h>
#include <stm32f4xx_hal.h>

#define ENC_GPIO GPIOC
uint32_t ENC_PIN_CHA = GPIO_PIN_6;
uint32_t ENC_PIN_CHB = GPIO_PIN_7;

static void SystemClock_Config(void);

void Error_Handler(void) {
  BSP_LED_On(LED6);
}

void delay (int a)
{
  volatile int i,j;
  for (i=0 ; i < a ; i++) {
    j++;
  }
  return;
}

TIM_HandleTypeDef hTIM3;
TIM_HandleTypeDef hTIM5;

void encoder_init(void) {
  GPIO_InitTypeDef gpio_init;
  gpio_init.Pin = ENC_PIN_CHA;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Speed = GPIO_SPEED_HIGH;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Alternate = GPIO_AF2_TIM3;

  __GPIOC_CLK_ENABLE();
  HAL_GPIO_Init(ENC_GPIO, &gpio_init);

  gpio_init.Pin = ENC_PIN_CHB;
  HAL_GPIO_Init(ENC_GPIO, &gpio_init);

  __TIM3_CLK_ENABLE();

  hTIM3.Instance = TIM3;
//  hTIM3.Init.Prescaler = (SystemCoreClock/65535) - 1; // 1s
//  hTIM3.Init.CounterMode = TIM_COUNTERMODE_UP;
//  hTIM3.Init.ClockDivision = 0;
  hTIM3.Init.Period = 65535;
  //hTIM3.Channel = HAL_TIM_ACTIVE_CHANNEL_1 | HAL_TIM_ACTIVE_CHANNEL_2;
  //HAL_TIM_Base_Init(&hTIM3);

  TIM_Encoder_InitTypeDef enc_init;
  enc_init.EncoderMode = TIM_ENCODERMODE_TI12;

  enc_init.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  enc_init.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;

  enc_init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  enc_init.IC2Selection = TIM_ICSELECTION_DIRECTTI;

  enc_init.IC1Prescaler = TIM_ICPSC_DIV1;
  enc_init.IC2Prescaler = TIM_ICPSC_DIV1;

  enc_init.IC1Filter = 0;
  enc_init.IC2Filter = 0;

  HAL_TIM_Encoder_Init(&hTIM3, &enc_init);

  __HAL_TIM_SetCounter(&hTIM3,0);
  HAL_TIM_Encoder_Start(&hTIM3, TIM_CHANNEL_1 | TIM_CHANNEL_2);
}

void motor_init(void) {
  GPIO_InitTypeDef gpio_init;
  gpio_init.Pin = GPIO_PIN_0;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_HIGH;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Alternate = GPIO_AF2_TIM5;

  __GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &gpio_init);

  gpio_init.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOA, &gpio_init);

  __TIM5_CLK_ENABLE();

  hTIM5.Instance = TIM5;
  hTIM5.Init.Prescaler = (SystemCoreClock/65535/60) - 1; // 1s
  hTIM5.Init.CounterMode = TIM_COUNTERMODE_UP;
  hTIM5.Init.ClockDivision = 0;
  hTIM5.Init.Period = 65535;
  //HAL_TIM_Base_Init(&hTIM5);
  HAL_TIM_OC_Init(&hTIM5);

  TIM_OC_InitTypeDef oc_enable;
  oc_enable.OCMode = TIM_OCMODE_PWM1;
  oc_enable.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc_enable.OCFastMode = TIM_OCFAST_DISABLE;
  oc_enable.Pulse = 0;
  HAL_TIM_OC_ConfigChannel(&hTIM5, &oc_enable, TIM_CHANNEL_1);
  HAL_TIM_OC_ConfigChannel(&hTIM5, &oc_enable, TIM_CHANNEL_2);

  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
  HAL_NVIC_ClearPendingIRQ(TIM5_IRQn);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);

  __HAL_TIM_SetCounter(&hTIM5,0);
  HAL_TIM_OC_Start(&hTIM5, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&hTIM5, TIM_CHANNEL_2);
  //HAL_TIM_OC_Start_IT(&hTIM5, TIM_CHANNEL_1);
}

UART_HandleTypeDef my_uart;

void uart_init(int baud) {
  GPIO_InitTypeDef gpio_init;
  gpio_init.Pin = GPIO_PIN_5;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_HIGH;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Alternate = GPIO_AF7_USART2;

  __GPIOD_CLK_ENABLE();
  HAL_GPIO_Init(GPIOD, &gpio_init);

  gpio_init.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &gpio_init);

//  GPIO_InitTypeDef gpio_init;
//  gpio_init.Pin = GPIO_PIN_2;
//  gpio_init.Pull = GPIO_NOPULL;
//  gpio_init.Speed = GPIO_SPEED_HIGH;
//  gpio_init.Mode = GPIO_MODE_AF_OD;
//  gpio_init.Alternate = GPIO_AF7_USART2;

//  __GPIOA_CLK_ENABLE();
//  HAL_GPIO_Init(GPIOA, &gpio_init);

//  gpio_init.Pin = GPIO_PIN_3;
//  HAL_GPIO_Init(GPIOA, &gpio_init);

  __USART2_CLK_ENABLE();

  my_uart.Instance = USART2;
  my_uart.Init.BaudRate = baud;
  my_uart.Init.WordLength = UART_WORDLENGTH_8B;
  my_uart.Init.StopBits = UART_STOPBITS_1;
  my_uart.Init.Parity = UART_PARITY_NONE;
  my_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  my_uart.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&my_uart);
}

void TIM3_IRQHandler(void) {
  BSP_LED_Toggle(LED5);
  HAL_TIM_IRQHandler(&hTIM3);
}

void TIM5_IRQHandler(void) {
  BSP_LED_Toggle(LED4);
  HAL_TIM_IRQHandler(&hTIM5);
}

int abs(int x) {
  if(x < 0) return -x;
  return x;
}

int min(int a, int b) {
  if(a < b) return a;
  return b;
}


uint8_t TxBuffer[128]="         >>>           hello world !         <<<      ";
uint16_t TxSize = 0;
uint16_t InitSize = sizeof("         >>>           hello world !         <<<      ")/sizeof(uint8_t);

//uint8_t TxBuffer[] = "Hello World !!";
//uint16_t TxSize = sizeof(TxBuffer)/sizeof(uint8_t);

uint8_t RxBuffer[128];
uint16_t RxSize = 128;

void write_int(int val) {
  int i = InitSize;
  while(val != 0) {
    TxBuffer[i++] = (val%10) + '0';
    val /= 10;
  }
  TxBuffer[i] = '\0';
  TxSize = i;
}

int main(void) {
  HAL_Init();

  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  SystemClock_Config();

  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);

  encoder_init();
  //motor_init();
  uart_init(330);
  // BAUD 330 = 9600

  delay(500000);
  BSP_LED_On(LED5);

  volatile int count = 0;
  int baud = 5173;
  while(1) {
    //HAL_UART_DeInit(&my_uart);
    //uart_init(baud);
    write_int(baud++);
    uint8_t c = 0;
    HAL_UART_Receive(&my_uart, &c, 1, 10000);

    HAL_UART_Transmit(&my_uart, &c, 1, 10000);
    delay(2);
    if(HAL_GPIO_ReadPin(ENC_GPIO, ENC_PIN_CHA) == GPIO_PIN_SET) {
      BSP_LED_On(LED6);
    }
    else  {
      BSP_LED_Off(LED6);
    }

    //delay(100000);
    TIM5->CCR1 = min(count*100, 65535/2);
    TIM5->CCR2 = min(count*100, 65535/2);
    //delay(500000);
    //BSP_LED_Toggle(LED4);
    //delay(1000000);
    if((int)(count - (int)TIM3->CNT) > 0) {
      BSP_LED_Off(LED4);
      BSP_LED_On(LED3);
    }
    else if((int)(count - (int)TIM3->CNT) < 0) {
      BSP_LED_On(LED4);
      BSP_LED_Off(LED3);
    }
    else {
//      BSP_LED_Off(LED4);
//      BSP_LED_Off(LED3);
    }

    count = TIM3->CNT;
    (void)count;
  }

  return 0;
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

