#ifndef __LED_H_
#define __LED_H_


#define LED1_PIN GPIO_Pin_12
#define LED2_PIN GPIO_Pin_13
#define LED3_PIN GPIO_Pin_14
#define LED4_PIN GPIO_Pin_15
#define LED5_PIN GPIO_Pin_11

#define ALL_LED LED1_PIN|LED2_PIN|LED3_PIN|LED4_PIN|LED5_PIN

void LED_Init(void);

#endif