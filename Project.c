#include <stdint.h>
#include <stdio.h>

// Create definition for PORT B registers
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// Create definitions for Port E registers
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))

// Create definitions for timer0 A and B 
#define TIMER0_CFG_R            (*((volatile unsigned long *)0x40030000))
#define TIMER0_TAMR_R           (*((volatile unsigned long *)0x40030004))
#define TIMER0_TBMR_R           (*((volatile unsigned long *)0x40030008))
#define TIMER0_CTL_R            (*((volatile unsigned long *)0x4003000C))
#define TIMER0_RIS_R            (*((volatile unsigned long *)0x4003001C))
#define TIMER0_ICR_R            (*((volatile unsigned long *)0x40030024))
#define TIMER0_TAILR_R          (*((volatile unsigned long *)0x40030028))
#define TIMER0_TBILR_R          (*((volatile unsigned long *)0x4003002C))
#define TIMER0_TAPR_R           (*((volatile unsigned long *)0x40030038)) 
#define TIMER0_TBPR_R           (*((volatile unsigned long *)0x4003003C))
  

// Port F button Registers
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))

#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_PRI1_R             (*((volatile unsigned long *)0xE000E404))

// System control registers
#define SYSCTL_RCGCTIMER_R      (*((volatile unsigned long *)0x400FE604))
#define SYSCTL_RCC_R            (*((volatile unsigned long *)0x400FE060))

// Funtion Prototypes
void timer0A_delayMs(int ttime);
void timer0_InIt(void);
void PortBE_Init(void);
void GPIOPortF_Init(void);
void Display(int digit, int number);
void NumSplitSec(unsigned long counted);
void NumSplitMin(unsigned long counted);
void NumSplitHrs(unsigned long counted);
void init_sysClock_16M(void);
void EnableInterrupts(void);
void GPIOF_Handler(void);

// Subroutine to initialize ports B, E
void PortBE_Init(void){ 
  SYSCTL_RCGC2_R |= 0x00000002;     // Port B clock initialized
  GPIO_PORTB_CR_R = 0x7F;           // Allow changes to PB6-PB0       
  GPIO_PORTB_AMSEL_R = 0x00;        // Disable analog function
  GPIO_PORTB_PCTL_R = 0x00000000;   // GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R = 0x7F;          // Set PB6-PB0 outputs   
  GPIO_PORTB_PUR_R = 0x00;          // Enable pullup resistors on PB4,PF0       
  GPIO_PORTB_DEN_R = 0x7F;          // 7) Enable digital pins PB6-PB0        

  SYSCTL_RCGC2_R |= 0x00000010;     // Port E clock initialized
  GPIO_PORTE_CR_R = 0x3F;           // Allow changes to PE5-0       
  GPIO_PORTE_AMSEL_R = 0x00;        // Disable analog function
  GPIO_PORTE_PCTL_R = 0x00000000;   // GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R = 0x3F;          // PE5-PE0 output   
  GPIO_PORTE_PUR_R = 0x00;          // Disable pullup resistors       
  GPIO_PORTE_DEN_R = 0x3F;          // Enable digital pins PE5-PE0  
}

int numbers[10] = {0x40,0x79,0x24,0x30,0x19, // Each value turns on bits needed
0x12,0x02,0x78,0x00,0x10};                           // to show numbers in display
int digit1, digit2, digit3, digit4, digit5, digit6;  // Number to be displayed in each digit

volatile unsigned long seconds = 40; // seconds 
volatile unsigned long minutes = 12; // minutes
volatile unsigned long hours = 12;   // hours

unsigned long SW1, SW2;  // switch state containers

int main(){

  // set clock speed  
  init_sysClock_16M();

  // initialize pins/ports
  PortBE_Init();
  GPIOPortF_Init(); 
  timer0_InIt();

  while(1){
    
    // display time
    NumSplitSec(seconds);
    NumSplitMin(minutes);
    NumSplitHrs(hours);
    
    // check/update time
    if ((TIMER0_RIS_R & 0x100) == 0x100) { // if 1 second timer has triggered
      
      if(seconds < 59){      
          seconds = seconds + 1; 
      } else { 
        seconds = 0;
        if (minutes < 59) {
          minutes += 1;
        } else {
          minutes = 0;
          if (hours < 23) {
            hours += 1;
          } else {
            hours = 0;
          }
        }
      }
      TIMER0_ICR_R = 0x100;      /* clear the Timer B timeout flag */  
    }
  }
}

void Display(int digit, int number){
GPIO_PORTB_DATA_R = 0x00; // Turns off LEDs
GPIO_PORTE_DATA_R = digit; // Selects digit
GPIO_PORTB_DATA_R = numbers[number]; // Turns on number in selected digit
//timer0A_delayMs(2); // unneded with slower clock?
}

// Splits number in counter into separate numbers for each digit
void NumSplitSec(unsigned long counted){
  
    digit1 = counted%10; //Copies value in counter, divides it by 10 and then keeps remainder
    counted /= 10;           //Dividing value in counter by 10 shifts it by one decimal
    digit2 = counted%10;
    
    Display(16,digit2);
    timer0A_delayMs(2);
    Display(32,digit1);
    timer0A_delayMs(2);
}

void NumSplitMin(unsigned long counted){
  
    digit3 = counted%10;
    counted /= 10;
    digit4 = counted%10; 
    counted /=10; 
    
    Display(4,digit4);
    timer0A_delayMs(2);
    Display(8,digit3);
    timer0A_delayMs(2);
}

void NumSplitHrs(unsigned long counted){
  
    digit5 = counted%10;
    counted /= 10;
    digit6 = counted%10; 
    counted /=10; 
    
    Display(1,digit6);
    timer0A_delayMs(2);
    Display(2,digit5);
    timer0A_delayMs(2);
}

void GPIOF_Handler(void)
{
  volatile int readback;
  SW2 = GPIO_PORTF_DATA_R&0x01; // read PF0 into SW2
  SW1 = GPIO_PORTF_DATA_R&0x10; // read PF0 into SW2

  GPIO_PORTF_ICR_R = 0x11;        /* clear PF4 int */

  if (SW2 == 0x00){
    if (hours < 23) {
      hours += 1;
    } else {
      hours = 0;
    }
    
  }

  if (SW1 == 0x00){
    if (minutes < 59) {
      minutes += 1;
    } else {
      minutes = 0;
      if (hours < 23) {
        hours += 1;
      } else {
        hours = 0;
      }
    }
  } 
  readback = GPIO_PORTF_ICR_R;    /* a read to force clearing of interrupt flag */
  readback = readback;
}

void timer0A_delayMs(int ttime)
{
  //int i;

  //for(i = 0; i < ttime; i++) { 
    while ((TIMER0_RIS_R & 0x01) == 0);      /* wait for TimerA timeout flag */
      TIMER0_ICR_R = 0x01;      /* clear the TimerA timeout flag */
  //}
}


void init_sysClock_16M(void) {
  
  SYSCTL_RCC_R = 0x2D51; // set clock to 16mhz internal precision oscillator, disable PLL subdivider
    
}

void timer0_InIt(void){
  

  SYSCTL_RCGCTIMER_R |= 0x01;     /* enable clock to Timer0 */

  TIMER0_CTL_R = 0x00;         // disable Timer before initialization
  TIMER0_CFG_R = 0x04;         // 16-bit option
  TIMER0_TAMR_R = 0x02;        // periodic mode and down-counter
  TIMER0_TBMR_R = 0x02;        // periodic mode and down-counter 
  TIMER0_TAILR_R = (1600 - 1); // Timer A interval load value register
  TIMER0_TBILR_R = 64000;      // Timer B interval load value register 
  TIMER0_TAPR_R = 1;           // Prescalar value.. Can extend the cycle time max 256 times
  TIMER0_TBPR_R = 250;         // Prescalar value.. 64000 * 250 = 16000000 or 1 sec
  TIMER0_CTL_R = 0x01;         // enable Timer A after initialization
  TIMER0_CTL_R |= 0x100;       // enable Timer B after initialization 
  TIMER0_ICR_R = 0x1;          // clear the TimerA timeout flag
  TIMER0_ICR_R |= 0x100;       // clear the TimerB timeout flag

}

void GPIOPortF_Init(void)
{

  SYSCTL_RCGC2_R |= 0x00000034;   /* 1) activate clock for Port F and C */
  
  /////////// Initialize PORT F  ///////////
  GPIO_PORTF_LOCK_R = 0x4C4F434B; /* 2) unlock GPIO Port F */
  GPIO_PORTF_CR_R = 0x1F;         /* allow changes to PF4-0 */
  GPIO_PORTF_AMSEL_R = 0x00;      /* 3) disable analog on PF */
  GPIO_PORTF_PCTL_R = 0x00000000; /* 4) PCTL GPIO on PF4-0 */
  GPIO_PORTF_DIR_R = 0x0E;        /* 5) PF4,PF0 in, PF3-1 out */
  GPIO_PORTF_AFSEL_R = 0x00;      /* 6) disable alt funct on PF7-0 */
  GPIO_PORTF_PUR_R = 0x11;        /* enable pull-up on PF0 and PF4 */
  GPIO_PORTF_DEN_R = 0x1F;        /* 7) enable digital I/O on PF4-0 */

  GPIO_PORTF_IS_R &= ~0x11;       /*  PF4 is edge-sensitive */
  GPIO_PORTF_IBE_R &= ~0x11;      /*  PF4 is not both edges */
  GPIO_PORTF_IEV_R &= ~0x11;      /*  PF4 falling edge event */
  GPIO_PORTF_ICR_R = 0x11;        /*  Clear flag4 */
  GPIO_PORTF_IM_R |= 0x11;        /*  arm interrupt on PF4 */
  NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF) | 0x00A00000; /*  priority 5 */
  NVIC_EN0_R = 0x40000000;        /*  Enable interrupt 30 in NVIC */

  //PRIORITY REGISTERS
  NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF1F) | 0x60; /*  priority 3 */
  NVIC_EN0_R |= 0x10;        /*  Enable interrupt in NVIC */

  /////////// Enable Global Interrupt Flag ///
  EnableInterrupts();             /* Enable global Interrupt flag (I) */

}


void EnableInterrupts(void)
{
    __asm  ("    CPSIE  I\n");
}

