#include <stm32f4_discovery.h>
#include <stm32f4xx_hal.h>

#include "init.h"
#include "encoder.h"
#include "motor.h"
#include "uart.h"



int abs(int x) {
  if(x < 0) return -x;
  return x;
}

int min(int a, int b) {
  if(a < b) return a;
  return b;
}

void delay(int t) {
  while(0 < t) {
    --t;
  }
}

int main(void) {

  init();
  encoder1_init();
  encoder2_init();
  motor_init();
  uart_init(330); // 9600 baud ?!?

  BSP_LED_On(LED5);

  int count = 0;
  char c = 0;
  while(1) {
    short enc1 = encoder1_get_value_short();
    short enc2 = encoder2_get_value_short();
    motor2_set_value(-50*enc1);
    motor1_set_value(50*enc2);

    uart_write_int(enc2);
    uart_putchar('\n');
  }

  return 0;
}
