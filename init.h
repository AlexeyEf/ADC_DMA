
#include <stm32l1xx.h>
#include <system_stm32l1xx.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_adc.h>
#include <stm32l1xx_dma.h>
#include <stm32l1xx_tim.h>


static uint16_t value_adc[10]; //array for adc values

//function of configuration of ports, adc, dma and timer modules
void gpio_init(void);
void adc_init(void);
void tim_init(void);
void dma_init(void);

