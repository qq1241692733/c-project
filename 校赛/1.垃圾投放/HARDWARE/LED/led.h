#ifndef  __LED_H
#define  __LED_H
#include "sys.h"

#define led0 PFout(9)
#define led1 PFout(10)

void time_init(u32 arr,u32 psc);
void gpio_init();

#endif
