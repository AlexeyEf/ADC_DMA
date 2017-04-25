
#include <init.h>
#include <stm32l1xx_dma.h>

static uint16_t value_adc[10]; //array for adc values

void gpio_init(void)
  {

  GPIO_InitTypeDef port;  //definition of variable of GPIO_InitTypeDef type to access to elements of structure GPIO_InitTypeDef to configure port GPIOC
    
  //clock configuration
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE); //enable clock of port C
    
  //Port configuration
  //configuration of elements of structure GPIO_InitTypeDef for port C
  port.GPIO_Pin=GPIO_Pin_0;//change pins
  port.GPIO_Mode=GPIO_Mode_AN;//this ports use in analog mode
  port.GPIO_Speed=GPIO_Speed_40MHz;//set max rate via this pins
  port.GPIO_OType=GPIO_OType_PP;
  //call function to configure GPIOC Pin 0
  GPIO_Init(GPIOC,&port);
  
  }


void adc_init(void)
  {
  
  ADC_InitTypeDef adc;  //definition of variable of ADC_InitTypeDef type to access to elements of structure ADC_InitTypeDef to configure adc ADC1
  
  RCC->CR |= RCC_CR_HSION;  //turn on oscillator HSI for clocking ADC1
  while(!(RCC->CR&RCC_CR_HSIRDY)); //waiting for stabilization of HSI
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); //enable clock of ADC1
  
  //ADC configuration
  //configuration of elements of structure ADC_InitTypeDef
  adc.ADC_Resolution=ADC_Resolution_12b; //ADC Resolution = 12 bits
  adc.ADC_ScanConvMode=DISABLE; //use not scan mode - use only one channel
  adc.ADC_ContinuousConvMode=DISABLE; //use discontinuous mode
  adc.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_Rising; //start adc of rising edge from trigger of timer TIM2
  adc.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_TRGO;////start adc of trigger of timer TIM2
  adc.ADC_DataAlign=ADC_DataAlign_Right;//tADC data alignment is righ
  adc.ADC_NbrOfConversion=1; // one conversion
  
  //call function to configure ADC1
  ADC_Init(ADC1, &adc);
  //configuration of regular channels
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_16Cycles); //use channel 10, 1 conversion, sample time is 16 cycles 
  ADC_DiscModeCmd(ADC1, ENABLE);//enable discontinuous mode
  
  //configuration of interrupt
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);// enable interrupt of end of conversion
  NVIC_EnableIRQ(ADC1_IRQn);//enable interrupt from ADC in NVIC controller
  //turn on ADC
  ADC_DMACmd(ADC1, ENABLE);// enable adc request to dma
  ADC_Cmd(ADC1, ENABLE);//start adc
  
  
  }


	
void tim_init(void)
  {

  TIM_TimeBaseInitTypeDef timer; //definition of variable of TimeBaseInitTypeDef type to access to elements of structure TimeBaseInitTypeDef to configure timer TIM2
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //enable clock of basic timer TIM2
      
  //Timer configuration
  //configuration of elements of structure TimeBaseInitTypeDef
  timer.TIM_Prescaler=2096; //prescaller of system clock MSI 2097 MHz = 2096
  timer.TIM_Period=1000; // period=1 sec
  //call function to configure timer TIM6
  TIM_TimeBaseInit(TIM2,&timer);
      
  TIM2->CR2 |= TIM_CR2_MMS_1; //enable clock for adc from this timer through TRG0
  TIM_Cmd(TIM2,ENABLE);  //start counting
  
  }
  
void dma_init(void)
  {
    DMA_InitTypeDef dma; //definition of variable of DMA_InitTypeDef type to access to elements of structure GPIO_InitTypeDef to configure DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//enable clock of DMA
    
    dma.DMA_PeripheralBaseAddr=(uint32_t) (&(ADC1->DR)); //Specifies the peripheral base address of data spi register ADC1->DR for DMA1 Channel1

    dma.DMA_MemoryBaseAddr=(uint32_t) (&(value_adc));      //Specifies the memory base address of first element of array value_adc[] for DMA1 Channel1

    dma.DMA_DIR=DMA_DIR_PeripheralSRC;     //Specifies if the peripheral is destination

    dma.DMA_BufferSize=10;         //Specifies the buffer size=10 of 16 bits words

    dma.DMA_PeripheralInc=DMA_PeripheralInc_Disable;      //Specifies the Peripheral address register is not incremented

    dma.DMA_MemoryInc=DMA_MemoryInc_Enable;           //Specifies the memory address register is incremented 

    dma.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;           // Specifies the ADC data width - 16 bit

    dma.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;     // Specifies the Memory data width - 16 bit

    dma.DMA_Mode=DMA_Mode_Normal;               // Specifies the operation mode of the DMA1 Channel3 is not Circular mode

    dma.DMA_Priority=DMA_Priority_High;          // Specifies the software priority for the DMA1 Channel1 - high priority

    dma.DMA_M2M=DMA_M2M_Disable;         //Specifies if the DMA1 Channel3 will be used in memory-to-memory transfer - disable
      
  DMA_Init( DMA1_Channel1, &dma);
  
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//enable interrupt if full data was transmitted

  DMA_Cmd(DMA1_Channel1, ENABLE);//start dma
  
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);//enable interrupt from DMA in NVIC controller
  }

void ADC1_IRQHandler(void)
  {    
  
      ADC_ClearITPendingBit(ADC1, ADC_IT_EOC); //Clear IT Pending Bit
  
  }

void DMA1_Channel1_IRQHandler(void)
  {    
    DMA_Cmd(DMA1_Channel1, DISABLE);//stop dma
		ADC_Cmd(ADC1, DISABLE);//stop adc
		TIM_Cmd(TIM2,DISABLE);  //stop timer
    DMA_ClearITPendingBit(DMA1_IT_TC1);//Clear IT Pending Bit
  }