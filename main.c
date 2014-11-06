#include <stm32f4xx_hal.h>
#include <stm32f4_discovery.h>

uint32_t MY_PIN = GPIO_PIN_1;

void delay (int a)
{
  volatile int i,j;
  for (i=0 ; i < a ; i++) {
    j++;
  }
  return;
}

TIM_HandleTypeDef htim2;

void encoder_init(void) {
  GPIO_InitTypeDef gpio_init;
  gpio_init.Pin = GPIO_PIN_0;
  gpio_init.Pull = GPIO_PULLDOWN;
  gpio_init.Speed = GPIO_SPEED_HIGH;
  gpio_init.Mode = GPIO_MODE_INPUT;

  __GPIOC_CLK_ENABLE();
  HAL_GPIO_Init(GPIOC, &gpio_init);

  gpio_init.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &gpio_init);
}

void TIM2_IRQHandler(void) {
  BSP_LED_Toggle(LED3);
  HAL_TIM_IRQHandler(&htim2);
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

  __TIM2_CLK_ENABLE();


  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (SystemCoreClock/65535) - 1; // 1s
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.ClockDivision = 0;
  htim2.Init.Period = 65535;
  HAL_TIM_Base_Init(&htim2);

  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_ClearPendingIRQ(TIM2_IRQn);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);


  BSP_LED_On(LED4);
  __HAL_TIM_SetCounter(&htim2,0);
  HAL_TIM_Base_Start_IT(&htim2);
  BSP_LED_Off(LED4);

  delay(500000);
  //BSP_LED_On(LED6);

  while(1) {
    //delay(500000);
    //BSP_LED_Toggle(LED4);
  }

  return 0;
}
