# Digital_Clock

Components: Tiva C Series TM4C123 Microcontroller, two 7-segment hex displays


https://youtu.be/eejo2VQqQwc


# General explanation of code

Within while loop the values seconds, minutes, and hours are send to their corresponding functions which display them on hex display.

The seconds, minutes, and hours are incrementing and checking for rollover values which sets each back to 0 when necessary. 

```
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
```

Each function for seconds, minutes, and hours takes the overall time "counted" and parses the value into digits to be displayed in the corresponding hex display digit. The value is then sent to the display function. 

```
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
```

Display function cycles through turning off the LEDs and sending the number to the correct digit. 

```
void Display(int digit, int number){
GPIO_PORTB_DATA_R = 0x00; // Turns off LEDs
GPIO_PORTE_DATA_R = digit; // Selects digit
GPIO_PORTB_DATA_R = numbers[number]; // Turns on number in selected digit
}
```
