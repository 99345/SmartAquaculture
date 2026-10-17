#ifndef __LED_H
#define __LED_H

void GPIOA_Init(void);
void motor_ON(void);
void motor_OFF(void);
void motor_Turn(int motor);
void bee_ON(void);
void bee_OFF(void);
void bee_Turn(int bee);

#endif
