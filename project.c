#include "TM4C123.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// FPU settings 
void SystemInit(void) {
    /* FPU settings */
#if (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | /* set CP10 Full Access */
                   (3UL << 11 * 2)); /* set CP11 Full Access */
#endif
}

//Register definitions for ClockEnable
#define SYSCTL_RCGCGPIO_R             (*((volatile unsigned long*)0x400FE608))
#define SYSCTL_RCGC_UART_R			  (*((volatile unsigned long*)0x400FE618))

/* Port F GPIO Registers */
#define GPIO_PORTF_DATA_R             (*((volatile unsigned long*)0x400253FC))
#define GPIO_PORTF_DIR_R              (*((volatile unsigned long*)0x40025400))
#define GPIO_PORTF_DEN_R              (*((volatile unsigned long*)0x4002551C))

/* Port A GPIO Registers for Fans and Buzzer */
#define GPIO_PORTA_DATA_R             (*((volatile unsigned long*)0x400043FC))
#define GPIO_PORTA_DIR_R              (*((volatile unsigned long*)0x40004400))
#define GPIO_PORTA_DEN_R              (*((volatile unsigned long*)0x4000451C))

/* Port E GPIO Registers */
#define GPIO_PORTE_DATA_R             (*((volatile unsigned long*)0x400243FC))
#define GPIO_PORTE_DIR_R              (*((volatile unsigned long*)0x40024400))
#define GPIO_PORTE_DEN_R              (*((volatile unsigned long*)0x4002451C))
#define GPIO_PORTE_AFSEL_R            (*((volatile unsigned long*)0x40024420))
#define GPIO_PORTE_AMSEL_R            (*((volatile unsigned long*)0x40024528))
#define GPIO_PORTE_PCTL_R             (*((volatile unsigned long*)0x4002452C))


//Register definitions for UART5 module
#define 	UART5_CTL_R				  (*((volatile unsigned long*)0x40011030))
#define  	UART5_IBRD_R			  (*((volatile unsigned long*)0x40011024))
#define  	UART5_FBRD_R			  (*((volatile unsigned long*)0x40011028))
#define 	UART5_LCRH_R			  (*((volatile unsigned long*)0x4001102C))
#define 	UART5_CC_R				  (*((volatile unsigned long*)0x40011FC8))
#define 	UART5_FR_R				  (*((volatile unsigned long*)0x40011018))
#define 	UART5_DR_R				  (*((volatile unsigned long*)0x40011000))


//Macros
#define 	UART_FR_TX_FF				0x20								//UART Transmit FIFO Full
#define 	UART_FR_RX_FE				0x10								//UART Receive FIFO Empty
#define FAN_PIN     (1<<2)  // PA2
#define BUZZER0_PIN (1<<3)  // PA3
#define BUZZER1_PIN (1<<4)  // PA4

void Delay(unsigned long counter); // used to add delay
void HC05_init(void); // Initialize UART5 module for HC-05
char Bluetooth_Read(void); //Read data from Rx5 pin of TM4C123
void Bluetooth_Write(unsigned char data); // Transmit a character to HC-05 over Tx5 pin 
void Bluetooth_Write_String(char *str); // Transmit a string to HC-05 over Tx5 pin 
void Bluetooth_Read_String_Buffer(char *buf, uint32_t max_len);
unsigned int delay_clk;
int main(void) {
    char buffer[32]; // Buffer to store incoming words
    
    HC05_init(); 
    // Clock Enable For Port A and F 
    SYSCTL_RCGCGPIO_R |= 0x21;            
    delay_clk = SYSCTL_RCGCGPIO_R; 

    /* Port F Initialization for LEDs */
    GPIO_PORTF_DIR_R  |= 0x0E;       // Set LED_PINS as output     
    GPIO_PORTF_DEN_R  |= 0x0E;       // Enable digital functionality    
    GPIO_PORTF_DATA_R &= ~0x0E;      // Start with LEDs OFF
    

    /* Port A Initialization for Fan and buzzers */
    GPIO_PORTA_DIR_R  |= 0x1C;         // Set BUZZERS_PIN FAN_PIN as output
    GPIO_PORTA_DEN_R  |= 0x1C;         // Enable digital functionality
    GPIO_PORTA_DATA_R &= ~0x1C;        // Start with Fan and buzzer OFF
    Bluetooth_Write_String("Bluetooth Voice Control Ready...\n");

    while(1) {
        // This function waits until you send a full string from your phone
        Bluetooth_Read_String_Buffer(buffer, 32); 
//===============================================================================================
//                   RBG SINGLE BULB ON OFF
//===============================================================================================
        if (strcmp(buffer, "red on") == 0) {
            GPIO_PORTF_DATA_R |= (1<<1);
            Bluetooth_Write_String("RED LED is now ON\n");
        }
        else if (strcmp(buffer, "red off") == 0) {
            GPIO_PORTF_DATA_R &= ~(1<<1);
            Bluetooth_Write_String("RED LED is now OFF\n");
        }
        else if (strcmp(buffer, "blue on") == 0) {
            GPIO_PORTF_DATA_R |= (1<<2);
            Bluetooth_Write_String("BLUE LED is now ON\n");
        }
        else if (strcmp(buffer, "blue off") == 0) {
            GPIO_PORTF_DATA_R &= ~(1<<2);
            Bluetooth_Write_String("BLUE LED is now OFF\n");
        }
        else if (strcmp(buffer, "green on") == 0) {
            GPIO_PORTF_DATA_R |= (1<<3);
            Bluetooth_Write_String("GREEN LED is now ON\n");
        }
        else if (strcmp(buffer, "green off") == 0) {
            GPIO_PORTF_DATA_R &= ~(1<<3);
            Bluetooth_Write_String("GREEN LED is now OFF\n");
        }
//===============================================================================================
//                   RBG DOUBLE BULB ON OFF SIMULTANOULY
//===============================================================================================

		else if (strcmp(buffer, "red on green on") == 0) {
            // Set both bit 1 and bit 3 to HIGH
            GPIO_PORTF_DATA_R |= (1<<1) | (1<<3); 
            Bluetooth_Write_String("Red and Green are now ON (Yellow/Orange mix)\n");
        }
		else if (strcmp(buffer, "blue on green on") == 0) {
			GPIO_PORTF_DATA_R |= (1<<2) | (1<<3);
			Bluetooth_Write_String("BLUE and GREEN are ON (Cyan)\n");
        }	
		else if (strcmp(buffer, "red on blue on") == 0) {
            GPIO_PORTF_DATA_R |= (1<<1) | (1<<2);
            Bluetooth_Write_String("RED and BLUE are ON (Magenta)\n");
        }
//===============================================================================================
//                   FAN SINGLE ON OFF
//===============================================================================================
        else if(strcmp(buffer, "fan on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            Bluetooth_Write_String("Fan ON\n");
        }
        else if(strcmp(buffer, "fan off") == 0) {
            GPIO_PORTA_DATA_R &= ~FAN_PIN;
            Bluetooth_Write_String("Fan OFF\n");
        }
        
//===============================================================================================
//                   BUZZER 1 WITH BUZZER 2  SINGLE ON OFF
//===============================================================================================
        else if (strcmp(buffer, "buzzer 1 on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;
            Bluetooth_Write_String("Buzzer 1 is ON\n");
        }
        else if (strcmp(buffer, "buzzer 1 off") == 0) {
            GPIO_PORTA_DATA_R &= ~BUZZER0_PIN;
            Bluetooth_Write_String("Buzzer 1  OFF\n");
        }

        else if (strcmp(buffer, "buzzer 2 on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;
            Bluetooth_Write_String("Buzzer 2 is ON\n");
        }
        else if (strcmp(buffer, "buzzer 2 off") == 0) {
            GPIO_PORTA_DATA_R &= ~BUZZER1_PIN;
            Bluetooth_Write_String("Buzzer 2 is OFF\n");
        }

        
//===============================================================================================
//                   FAN WITH SINGLE RGB 
//===============================================================================================
        else if (strcmp(buffer, "fan on and red on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;        // Fan ON
            GPIO_PORTF_DATA_R |= (1 << 1);      // Red ON (Keeps others on)
            Bluetooth_Write_String("Fan and Red LED are ON\n");
        }
        else if (strcmp(buffer, "fan on and blue on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 2);      // Blue ON
            Bluetooth_Write_String("Fan and Blue LED are ON\n");
        }
        else if (strcmp(buffer, "fan on and green on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 3);      // Green ON
            Bluetooth_Write_String("Fan and Green LED are ON\n");
}        

//===============================================================================================
//                   FAN WITH TWO RGB SIMULTANEOULY
//===============================================================================================
        else if (strcmp(buffer, "fan on red on and blue on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2); 
            Bluetooth_Write_String("Fan, Red, and Blue added\n");
        }        
        else if (strcmp(buffer, "fan on red on and green on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 3); 
            Bluetooth_Write_String("Fan, Red, and Green added\n");
        }
        else if (strcmp(buffer, "fan on blue on and green on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 2) | (1 << 3); 
            Bluetooth_Write_String("Fan, Blue, and Green added\n");
        }


//===============================================================================================
//                   FAN WITH Three RGB SIMULTANEOULY
//===============================================================================================
        else if (strcmp(buffer, "fan on red on blue on and green on") == 0) {
            GPIO_PORTA_DATA_R |= FAN_PIN;
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2) | (1 << 3); 
            Bluetooth_Write_String("Full Fan and White Light ON\n");
        }
//===============================================================================================
//                   BUZZER 1 WITH SINGLE RGB 
//===============================================================================================

        else if (strcmp(buffer, "buzzer 1 and red on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;   // PA3 High
            GPIO_PORTF_DATA_R |= (1 << 1);      // PF1 High (Red)
            Bluetooth_Write_String("Buzzer 1 and Red LED are ON\n");
        }
        
        else if (strcmp(buffer, "buzzer 1 and blue on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;   // PA3 High
            GPIO_PORTF_DATA_R |= (1 << 2);      // PF2 High (Blue)
            Bluetooth_Write_String("Buzzer 1 and Blue LED are ON\n");
        }
        
        else if (strcmp(buffer, "buzzer 1 and green on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;   // PA3 High
            GPIO_PORTF_DATA_R |= (1 << 3);      // PF3 High (Green)
            Bluetooth_Write_String("Buzzer 1 and Green LED are ON\n");
        }
//===============================================================================================
//                   BUZZER 2 WITH SINGLE RGB 
//===============================================================================================
        else if (strcmp(buffer, "buzzer 2 and red on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;   // PA4 High
            GPIO_PORTF_DATA_R |= (1 << 1);      // PF1 High (Red)
            Bluetooth_Write_String("Buzzer 2 and Red LED are ON\n");
        }
        else if (strcmp(buffer, "buzzer 2 and blue on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;   // PA4 High
            GPIO_PORTF_DATA_R |= (1 << 2);      // PF2 High (Blue)
            Bluetooth_Write_String("Buzzer 2 and Blue LED are ON\n");
        }
        else if (strcmp(buffer, "buzzer 2 and green on") == 0) {
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;   // PA4 High
            GPIO_PORTF_DATA_R |= (1 << 3);      // PF3 High (Green)
            Bluetooth_Write_String("Buzzer 2 and Green LED are ON\n");
        }
//===============================================================================================
//                   BUZZER 1 WITH TWO RGB SIMULTANOUSLY
//===============================================================================================
        else if (strcmp(buffer, "buzzer 1 on blue on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs (PF1, PF2, PF3)
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;            // Buzzer 1 ON
            GPIO_PORTF_DATA_R |= (1 << 2) | (1 << 3);    // Blue and Green ON
            Bluetooth_Write_String("Buzzer 1, Blue, and Green are ON\n");
        }
        else if (strcmp(buffer, "buzzer 1 on red on and blue on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;            // Buzzer 1 ON
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2);    // Red and Blue ON
            Bluetooth_Write_String("Buzzer 1, Red, and Blue are ON\n");
        }
        else if (strcmp(buffer, "buzzer 1 on red on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;            // Buzzer 1 ON
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 3);    // Red and Green ON
            Bluetooth_Write_String("Buzzer 1, Red, and Green are ON\n");
        }
//===============================================================================================
//                   BUZZER 2 WITH TWO RGB SIMULTANOUSLY
//===============================================================================================

        else if (strcmp(buffer, "buzzer 2 on blue on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs (PF1, PF2, PF3)
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;            // Buzzer 2 ON (PA4)
            GPIO_PORTF_DATA_R |= (1 << 2) | (1 << 3);    // Blue and Green ON
            Bluetooth_Write_String("Buzzer 2, Blue, and Green are ON\n");
        }
    
        else if (strcmp(buffer, "buzzer 2 on red on and blue on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;            // Buzzer 2 ON (PA4)
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2);    // Red and Blue ON
            Bluetooth_Write_String("Buzzer 2, Red, and Blue are ON\n");
        }
        
        else if (strcmp(buffer, "buzzer 2 on red on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear all LEDs
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;            // Buzzer 2 ON (PA4)
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 3);    // Red and Green ON
            Bluetooth_Write_String("Buzzer 2, Red, and Green are ON\n");
        }
//===============================================================================================
//                   BUZZER 1 WITH Three RGB SIMULTANOUSLY
//===============================================================================================
        else if (strcmp(buffer, "buzzer 1 on red on blue on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear LEDs
            GPIO_PORTA_DATA_R |= BUZZER0_PIN;            // Buzzer 1 ON
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2) | (1 << 3); // Red + Blue + Green ON
            Bluetooth_Write_String("Buzzer 1 and All LEDs are ON\n");
        }
        
//===============================================================================================
//                   BUZZER 2 WITH Three RGB SIMULTANOUSLY
//===============================================================================================
        else if (strcmp(buffer, "buzzer 2 on red on blue on and green on") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;                  // Clear LEDs
            GPIO_PORTA_DATA_R |= BUZZER1_PIN;            // Buzzer 2 ON
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2) | (1 << 3); // Red + Blue + Green ON
            Bluetooth_Write_String("Buzzer 2 and All LEDs are ON\n");      
        }        
//===============================================================================================
//                   BUZZER 2 WITH FAN 
//===============================================================================================
        else if (strcmp(buffer, "buzzer 1 on and fan on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | FAN_PIN); 
            Bluetooth_Write_String("Buzzer 1 and Fan are ON\n");
        }         
        else if (strcmp(buffer, "buzzer 2 on and fan on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER1_PIN | FAN_PIN); 
            Bluetooth_Write_String("Buzzer 2 and Fan are ON\n");
        }
//===============================================================================================
//                   BOTH BUZZERs WITH SINLE RBG
//===============================================================================================

        else if (strcmp(buffer, "all buzzers and red on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 1); 
            Bluetooth_Write_String("Both Buzzers and Red LED are ON\n");
        }

        else if (strcmp(buffer, "all buzzers and blue on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 2); 
            Bluetooth_Write_String("Both Buzzers and Blue LED are ON\n");
        }
        else if (strcmp(buffer, "all buzzers and green on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 3); 
            Bluetooth_Write_String("Both Buzzers and Green LED are ON\n");
        }
//===============================================================================================
//                   BOTH BUZZERs WITH TWO RBG
//===============================================================================================
        else if (strcmp(buffer, "all buzzers red on and blue on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2); 
            Bluetooth_Write_String("Both Buzzers and Magenta (R+B) are ON\n");
        }

        else if (strcmp(buffer, "all buzzers red on and green on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 3); 
            Bluetooth_Write_String("Both Buzzers and Yellow (R+G) are ON\n");
        }

        else if (strcmp(buffer, "all buzzers blue on and green on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= (1 << 2) | (1 << 3); 
            Bluetooth_Write_String("Both Buzzers and Cyan (B+G) are ON\n");
        }
//===============================================================================================
//                   BOTH BUZZERs WITH  ALL RBG
//===============================================================================================
        else if (strcmp(buffer, "all buzzers and all leds on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            GPIO_PORTF_DATA_R |= 0x0E; // Red + Blue + Green
            Bluetooth_Write_String("Both Buzzers and White Light are ON\n");
        }        
//===============================================================================================
//                   BOTH BUZZERs WITH  FAN
//===============================================================================================

        else if (strcmp(buffer, "fan and all buzzers on") == 0) {
            GPIO_PORTA_DATA_R |= (FAN_PIN | BUZZER0_PIN | BUZZER1_PIN);
            Bluetooth_Write_String("Fan and Both Buzzers are ON\n");
        }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++         ALL APPLIANCES SHUT DOWN           +++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===============================================================================================
//                   RBG ALL BULB ON OFF SIMULTANOULY
//===============================================================================================

        else if (strcmp(buffer, "all led on") == 0) {
            GPIO_PORTF_DATA_R |= 0x0E;
            Bluetooth_Write_String("All LEDs are ON\n");
        }
        else if (strcmp(buffer, "all led off") == 0) {
            GPIO_PORTF_DATA_R &= ~0x0E;
            Bluetooth_Write_String("All LEDs are OFF\n");
        }
//===============================================================================================
//                   ALL BUZZERS OFF ON 
//===============================================================================================

        else if (strcmp(buffer, "all buzzers on") == 0) {
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN);
            Bluetooth_Write_String("Both Buzzers are ON\n");
        }
        else if (strcmp(buffer, "all buzzers off") == 0) {
            GPIO_PORTA_DATA_R &= ~(BUZZER0_PIN | BUZZER1_PIN);
            Bluetooth_Write_String("Both Buzzers are OFF\n"); 
        }

//===============================================================================================
//                   WHOLE SYSTEM ON OFF 
//===============================================================================================

        else if (strcmp(buffer, "all on") == 0) {
            GPIO_PORTA_DATA_R |= (FAN_PIN | BUZZER0_PIN | BUZZER1_PIN);  // Turn on Fan, Buzzer 1, and Buzzer 2
            GPIO_PORTF_DATA_R |= (1 << 1) | (1 << 2) | (1 << 3);         //Turn on Red, Blue, and Green LEDs
            Bluetooth_Write_String("All systems: Fan, Buzzers, and LEDs are ON\n");
        }
        else if (strcmp(buffer, "all off") == 0) {
            GPIO_PORTA_DATA_R &= ~(FAN_PIN | BUZZER0_PIN | BUZZER1_PIN);  // Port A: Turn off Fan and both Buzzers
            GPIO_PORTF_DATA_R &= ~((1 << 1) | (1 << 2) | (1 << 3));       //Turn off all RGB LEDs
            Bluetooth_Write_String("All systems: Fan, Buzzers, and LEDs are OFF\n");
        } 
//===============================================================================================
//                  ALERT CONDITION 
//===============================================================================================
        else if (strcmp(buffer, "alert") == 0) {
            int i = 0;
            Bluetooth_Write_String("ALERT STARTING...\n");
            GPIO_PORTA_DATA_R |= (BUZZER0_PIN | BUZZER1_PIN );//Turn on Buzzers
            for(i = 0; i < 100; i++) {      //Blink Red LED 100 times
                GPIO_PORTF_DATA_R |= (1 << 1);  // Red ON
                Delay(800000);                  // Approximately 0.5 second delay
                GPIO_PORTF_DATA_R &= ~(1 << 1); // Red OFF
                Delay(800000);
            }
            Bluetooth_Write_String("Alert sequence complete.\n");
}        
//===============================================================================================
//                   UNKNOWN COMMAND 
//===============================================================================================
        else {
            Bluetooth_Write_String("Unknown: ");
            Bluetooth_Write_String(buffer);
            Bluetooth_Write_String("\n");
        }

    }
}
void HC05_init(void)
{   
    SYSCTL_RCGC_UART_R |= 0x20;      /* enable clock to UART5 */
	delay_clk = SYSCTL_RCGC_UART_R;  /* Wait until UART5 is ready */
    SYSCTL_RCGCGPIO_R |= 0x10;       /* enable clock to PORTE */
    delay_clk = SYSCTL_RCGCGPIO_R;    /* wait for Port E ready */

    UART5_CTL_R &= ~0x01;            /* disable UART5 for config */
    UART5_IBRD_R = 104;              /* 9600 baud @ 16MHz */
    UART5_FBRD_R = 11;
    UART5_CC_R = 0;                  /* use system clock */
    UART5_LCRH_R = 0x70;             /* 8-bit, FIFO enabled, no parity */
    UART5_CTL_R = 0x301;             /* Enable UART5, RXE, TXE */

    GPIO_PORTE_AFSEL_R |= 0x30;      /* PE4, PE5 alt function */
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0x00110000;
    GPIO_PORTE_DEN_R |= 0x30;        /* digital enable */
    GPIO_PORTE_AMSEL_R &= ~0x30;     /* disable analog */
}




//This function retrieves a single byte (character) 
// of data that has been sent from your phone to the microcontroller.
char Bluetooth_Read(void)  
{
    char data;
	while((UART5_FR_R & (1<<4)) != 0);   /* wait until RX FIFO is NOT empty */
    data = UART5_DR_R ;  	             /* before giving it another byte */
    return (unsigned char) data; 
}


//This function sends a single byte from the microcontroller to your phone.
void Bluetooth_Write(unsigned char data)  
{
    while((UART5_FR_R & (1<<5)) != 0); /* wait until Tx buffer not full */
    UART5_DR_R = data;                  /* before giving it another byte */
}


void Bluetooth_Write_String(char *str)
{
  while(*str)
	{
		Bluetooth_Write(*(str++));
	}
}

void Delay(unsigned long counter)
{
	unsigned long i = 0;
	for(i=0; i< counter; i++);
}


// 2. Function to read a full string until 'enter' is pressed
void Bluetooth_Read_String_Buffer(char *buf, uint32_t max_len) {
    uint32_t i = 0;
    char c;
    while (i < max_len - 1) {
        c = Bluetooth_Read();
        if (c == '\r' || c == '\n') break; // Stop at Newline
        buf[i++] = c;
    }
    buf[i] = '\0'; // Null-terminate the string
		
}