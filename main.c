
#include "init.h"
  
int main()
{ 
  //call function of configuration of ports, adc, dma and timer modules	
  gpio_init();
  dma_init();
  adc_init();
  tim_init();  
  
  
  while(1);
   
}
