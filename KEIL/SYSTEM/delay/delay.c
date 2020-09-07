
#include "Delay.h"

static void Delay_Init(void);
static void Delay_Init(void)
{
  static uint8_t first = 0;
  
  if(first == 0)
  {
    first = 1;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* Disability SysTick counter */
  }
}
 
/****************************************************************
 * Function:    Delay_us
 * Description: Microsecond delay.
 * Input:       nus
 * Output:
 * Return:
*****************************************************************/
void Delay_us(uint64_t nus)
{
  uint32_t temp = 0;
  uint64_t nms = 0;
  
  Delay_Init();
  
  if(nus == 0)
  {
    return;
  }
  
  nms = nus / 1000;
  nus = nus % 1000;
  
  if(nms > 0)
  {
    Delay_ms(nms);
  }
 
  if(nus > 0)
  {
    SysTick->LOAD = SystemCoreClock / 8000000 * nus;  /* Time load (SysTick-> LOAD is 24bit) */ //21
    SysTick->VAL = 0x000000;                          /* Empty counter */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;         /* Start the countdown */
 
    do
    {
      temp = SysTick->CTRL;
    }
    while(temp&0x01 && !(temp&(1<<16)));        /* Wait time is reached */
 
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* Close Counter */
    SysTick->VAL = 0x000000;                    /* Empty counter */
  }
}
 
/****************************************************************
 * Function:    Delay_ms
 * Description: Millisecond delay.
 * Input:       nms
 * Output:
 * Return:
*****************************************************************/
void Delay_ms(uint64_t nms)
{
  uint32_t temp = 0;
  
  Delay_Init();
  
  if(nms == 0)
  {
    return;
  }
  
  while(nms > 500)
  {
    SysTick->LOAD = SystemCoreClock / 8000 * 500; /* Time load (SysTick-> LOAD is 24bit) */
    SysTick->VAL = 0x000000;                      /* Empty counter */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     /* Start the countdown */
 
    do
    {
      temp = SysTick->CTRL;
    }
    while(temp&0x01 && !(temp&(1<<16)));        /* Wait time is reached */
 
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* Close Counter */
    SysTick->VAL = 0x000000;                    /* Empty counter */
    
    nms -= 500;
  }
  
  SysTick->LOAD = SystemCoreClock / 8000 * nms; /* Time load (SysTick-> LOAD is 24bit) */
  SysTick->VAL = 0x000000;                      /* Empty counter */
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     /* Start the countdown */
 
  do
  {
    temp = SysTick->CTRL;
  }
  while(temp&0x01 && !(temp&(1<<16)));        /* Wait time is reached */
 
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* Close Counter */
  SysTick->VAL = 0x000000;                    /* Empty counter */
}