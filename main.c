#include <stm32f4xx_hal.h>
#include <stm32f4_discovery.h>

#define ENC_GPIO GPIOC
uint32_t ENC_PIN_CHA = GPIO_PIN_6;
uint32_t ENC_PIN_CHB = GPIO_PIN_7;

void delay (int a)
{
  volatile int i,j;
  for (i=0 ; i < a ; i++) {
    j++;
  }
  return;
}

TIM_HandleTypeDef hTIM3;

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
}

void TIM3_IRQHandler(void) {
  BSP_LED_Toggle(LED3);
  HAL_TIM_IRQHandler(&hTIM3);
}

int main(void) {
  HAL_Init();

  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);

  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);

  encoder_init();

  __TIM3_CLK_ENABLE();

  hTIM3.Instance = TIM3;
  hTIM3.Init.Prescaler = (SystemCoreClock/65535) - 1; // 1s
  hTIM3.Init.CounterMode = TIM_COUNTERMODE_UP;
  hTIM3.Init.ClockDivision = 0;
  hTIM3.Init.Period = 65535;
  //HAL_TIM_Base_Init(&hTIM3);

  TIM_Encoder_InitTypeDef enc_init;
  enc_init.EncoderMode = TIM_ENCODERMODE_TI12;
  enc_init.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  enc_init.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  HAL_TIM_Encoder_Init(&hTIM3, &enc_init);

  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_ClearPendingIRQ(TIM3_IRQn);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);


  BSP_LED_On(LED4);
  __HAL_TIM_SetCounter(&hTIM3,0);
  //HAL_TIM_Base_Start_IT(&hTIM3);
  HAL_TIM_Encoder_Start_IT(&hTIM3, TIM_CHANNEL_1);
  BSP_LED_Off(LED4);

  delay(500000);
  BSP_LED_On(LED5);

  volatile int count = 0;
  while(1) {
    if(HAL_GPIO_ReadPin(ENC_GPIO, ENC_PIN_CHA) == GPIO_PIN_SET) {
      BSP_LED_On(LED6);
    }
    else  {
      BSP_LED_Off(LED6);
    }

    //delay(500000);
    //BSP_LED_Toggle(LED4);
    count = TIM3->CNT;
    (void)count;
  }

  return 0;
}
