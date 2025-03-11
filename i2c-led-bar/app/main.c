#include <msp430.h>
#include <stdbool.h>


/** Jack Glutting-Gilsdorf, Nick Haldar , Ryan Adolph
* Project #4
* 3/9/2025
* EELE465
*/

/* Because this is on its own MSP 430 board I can use timers to make the patterns move as much as I want, also then I can 
update the value of the timer's CCR0 to have a higher and lower frequency. 

*/


// SLAVE ADDRESS 0x69



int current_value;

int P1_Lightbar_array0 = 0b10100000;
int P2_Lightbar_array0 = 0b00001010;

int P1_Lightbar_array1[] = {0b10100010, 0b01010001};
int P2_Lightbar_array1[] = {0b10000000, 0b01000000};
int LIGHTBAR1_LENGTH = 1;
int LIGHTBAR1_POS;

int P1_Lightbar_value2;
int P2_Lightbar_value2;
int LIGHTBAR2_LENGTH = 15;
int LIGHTBAR2_POS; // instead of doing this just increment P6 by one 16 times, and increment P4 by 16 every 16 times

int P1_Lightbar_array3[] = {0b00010000, 0b00100000, 0b01000010, 0b10000001, 0b01000010, 0b00100000};
int P2_Lightbar_array3[] = {0b10000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000};
int LIGHTBAR3_LENGTH = 5;
int LIGHTBAR3_POS;

int P1_Lightbar_value4;
int P2_Lightbar_value4;
int LIGHTBAR4_LENGTH = 15;
int LIGHTBAR4_POS; // instead of doing this just decrement P6 by one 16 times, and decrement P4 by 16 every 16 times

int P1_Lightbar_array5[] = {0b00000001, 0b00000010, 0b00000000, 0b00000000, 0b00010000, 0b00100000, 0b01000000, 0b10000000};
int P2_Lightbar_array5[] = {0b00000000, 0b00000000, 0b01000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
int LIGHTBAR5_LENGTH = 7;
int LIGHTBAR5_POS;

int P1_Lightbar_array6[] = {0b01110011, 0b10110011, 0b11010011, 0b11100011, 0b11110011, 0b11110011, 0b11110001, 0b11110010};
int P2_Lightbar_array6[] = {0b11000000, 0b1100000, 0b11000000, 0b11000000, 0b01000000, 0b10000000, 0b11000000, 0b11000000};
int LIGHTBAR6_LENGTH = 7;
int LIGHTBAR6_POS;

int P1_Lightbar_array7[] = {0b00000001, 0b00000011, 0b00000011, 0b00000011, 0b00010011, 0b00110011, 0b01110011, 0b11110011};
int P2_Lightbar_array7[] = {0b00000000, 0b0000000, 0b01000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000};
int LIGHTBAR7_LENGTH = 7;
int LIGHTBAR7_POS;

int delay_var;

char input = '6'; 

int rx_val; 



void delay_loop(){ 
    for(delay_var = 0; delay_var < 20000; delay_var++);
}


void Initialize_LEDBAR()
{
   // Stop watchdog timer
   WDTCTL = WDTPW | WDTHOLD;

    //----- Setup I2C to be in SLAVE mode (0x69)--------

        UCB0CTLW0 = UCSWRST;                      // Software reset enabled
        UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
        UCB0I2COA0 = 0x69 | UCOAEN;               // Own Address and enable
        UCB0CTLW0 &= ~UCSWRST;                    // clear reset register
        //UCB0IE |= UCRXIE + UCSTPIE;

    //----- Finished I2C INIT--------


    //----- Setup light bar output pins ((BIT0 & BIT1) P1.0 -> P1.1, (BIT2 & BIT3) P2.6 -> P2.7, (Upper 4 BITS) P1.4 -> P1.7 )--------
    
        //P6SEL0 |= 0b00011111;   // P2.6 -> P2.7
        //P6SEL1 |= 0b00011111;   // make IO
        P2DIR |= 0b11000001;    // make it output
        //P2REN &= ~0b11000000;   // give pull down resistor
        P2OUT &= ~0b11000001;   // clear output
        
        //P3SEL0 |= 0b10001101;   // P1.4 -> P1. for upper. P1.0 and P1.1 for lower
        //P3SEL1 |= 0b10001101;
        P1DIR |= 0b11110011;
        //P1REN &= ~0b11110011;
        P1OUT &= ~0b11110011;


        PM5CTL0 &=  ~LOCKLPM5;  // Turn on GPIO

    // ------- light bar done -----

    //----- Setup Timers--------
        TB0CTL |= TBCLR;            // Clear Timers and Dividers
        TB0CTL |= TBSSEL__SMCLK;    // Source = SMCLK
        TB0CTL |= MC__UP;           // Mode   = Up
        TB0CCR0 = 50000;            // TB0CCR0 = 1000
        TB0CCR1 = 250;              // TB0CCR1 = 0.25 * TB0CCR0 = 250

    //----- Setup Timers Overflow IQR--------
        TB0CCTL0 &= ~CCIFG;           //Clear CCR0 flag
        TB0CCTL0 |= CCIE;             //Enable CCR0 Overflow IQR
        TB0CCTL1 &= ~CCIFG;           //Clear CCR1 flag
        TB0CCTL1 |= CCIE;             //Enable CCR1 Overflow IQR

    __enable_interrupt();       //Enable Maskable IQRs
    
}


int main()
{

    // clear R3 - 6
    //R3 |= 0b00000000;
    //R4 |= 0b00000000;
    //R5 |= 0b00000000;
    //R6 |= 0b00000000;

Initialize_LEDBAR();

while(true){

    }
}

/*
 * -------Interrupt Service Routines--------
*/


/*
* Timer0_B0 service routine
*/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){
    switch(input) {
            case '0':
                P2OUT = P2_Lightbar_array0;
                P1OUT = P1_Lightbar_array0;

                break;
           case '1':
                if(LIGHTBAR1_POS == LIGHTBAR1_LENGTH){ 
                    P2OUT = P2_Lightbar_array1[LIGHTBAR1_POS];
                    P1OUT = P1_Lightbar_array1[LIGHTBAR1_POS];
                    LIGHTBAR1_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_array1[LIGHTBAR1_POS];
                P1OUT = P1_Lightbar_array1[LIGHTBAR1_POS];
                LIGHTBAR1_POS++;
                break;
           case '2':    //----------------- FIX
                if(LIGHTBAR2_POS == LIGHTBAR2_LENGTH){ 
                    if(P1_Lightbar_value2 == 0b11110000){
                        P1_Lightbar_value2 = 0;
                    }else{P1_Lightbar_value2 += 16;}
                    P2_Lightbar_value2 = 0;
                    P2OUT = P2_Lightbar_value2;
                    P1OUT = P1_Lightbar_value2;
                    LIGHTBAR2_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_value2;
                P2_Lightbar_value2 ++;
                LIGHTBAR2_POS++;
                break;

           case '3':
                if(LIGHTBAR3_POS == LIGHTBAR3_LENGTH){ 
                    P2OUT = P2_Lightbar_array3[LIGHTBAR3_POS];
                    P1OUT = P1_Lightbar_array3[LIGHTBAR3_POS];
                    LIGHTBAR3_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_array3[LIGHTBAR3_POS];
                P1OUT = P1_Lightbar_array3[LIGHTBAR3_POS];
                LIGHTBAR3_POS++;
                break;

            case '4':  //----------------- FIX
                if(P2_Lightbar_value4 == 0b00000000 && LIGHTBAR4_POS == 0 && P1_Lightbar_value4 == 0b11110000){
                     P1_Lightbar_value4 = 0b11110000;}
                if(LIGHTBAR4_POS == LIGHTBAR4_LENGTH){ 
                    if(P2_Lightbar_value4 == 0b00000000){
                        P1_Lightbar_value4 = 0b11110000;
                    }else{P1_Lightbar_value4 -= 16;}
                    P2_Lightbar_value4 = 0b00001111;
                    P2OUT = P2_Lightbar_value4;
                    P1OUT = P2_Lightbar_value4;
                    LIGHTBAR4_POS = 0;
                    break;
                }
                P2_Lightbar_value4 --;
                P2OUT = P2_Lightbar_value4;
                LIGHTBAR4_POS++;
                break;
            case '5':
                if(LIGHTBAR5_POS == LIGHTBAR5_LENGTH){ 
                    P2OUT = P2_Lightbar_array5[LIGHTBAR5_POS];
                    P1OUT = P1_Lightbar_array5[LIGHTBAR5_POS];
                    LIGHTBAR5_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_array5[LIGHTBAR5_POS];
                P1OUT = P1_Lightbar_array5[LIGHTBAR5_POS];
                LIGHTBAR5_POS++;
                break;

            case '6':
                if(LIGHTBAR6_POS == LIGHTBAR6_LENGTH){ 
                    P2OUT = P2_Lightbar_array6[LIGHTBAR6_POS];
                    P1OUT = P1_Lightbar_array6[LIGHTBAR6_POS];
                    LIGHTBAR6_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_array6[LIGHTBAR6_POS];
                P1OUT = P1_Lightbar_array6[LIGHTBAR6_POS];
                LIGHTBAR6_POS++;
                break;
            case '7':
                if(LIGHTBAR7_POS == LIGHTBAR7_LENGTH){ 
                    P2OUT = P2_Lightbar_array7[LIGHTBAR7_POS];
                    P1OUT = P1_Lightbar_array7[LIGHTBAR7_POS];
                    LIGHTBAR7_POS = 0;
                    break;
                }
                P2OUT = P2_Lightbar_array7[LIGHTBAR7_POS];
                P1OUT = P1_Lightbar_array7[LIGHTBAR7_POS];
                LIGHTBAR7_POS++;
                break;

            default:
                break;
       };
    TB0CCTL0 &= ~CCIFG; // clearing CCR0 ISR flag
}
/*
* END Timer0_B0 ISR
*/


/*
* Timer0_B1 service routine
*/
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void){
    P2OUT ^ BIT0; 
    TB0CCTL1 &= ~CCIFG; // clearing CCR1 ISR flag
}
/*
* END Timer0_B1 ISR
*/




#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
  //Must read from UCB0RXBUF
  rx_val = 0;
  rx_val = UCB0RXBUF;
}



/*
 * END Interrupt Service Routines
 */

