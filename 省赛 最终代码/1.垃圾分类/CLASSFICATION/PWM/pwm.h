#ifndef  __PWM_H
#define  __PWM_H
#include "sys.h"

void time_init(u32 arr,u32 psc);
void Turn_around1(int angle,int period);  //  正转
void Turn_around2(int angle,int period);  //  反转
#endif
