#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	BEEP_Init();
	while(1)
	{	  	    	
	}
}
