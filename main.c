/*
 * File:   main.c
 * Author: Hesham Banafa
 * Author: Mohammad Alghamdi
 * Author: Abduallah Alawi
 *
 * Created on April 17, 2021, 2:56 AM
 */


#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pic18.h>

#include "config.h"
#include "lcd.h"

#define MOTD_S "Vault System"

#define PORTB_NOMINAL 0xF0

#define UNLCOKED_LED_PIN LATA7
#define LOCKED_LED_PIN LATA6

#define PASSWD_LENGTH 5 // Pass + null char
#define PASSWD "1234"

void setup(void);
char get_input_char(void);
void keypad_isr(void);
short check_password(void);
void unlock(void);
void delay(void);
void reset_attempt(void);
void lock_down(void);
void reset_pass(void);

short letters = 0;
char input[PASSWD_LENGTH]; //User input
short new_char = 0; // used as flag
short attempts = 0; // Remember count of wrong attempts to delay or lock down.
char password[PASSWD_LENGTH];
 

void __interrupt() interrupts()
{
    /* Using all INTx for same job. Should use PORTB change interrupts*/
    if (INT0IF 
        || INT1IF
        || INT2IF
        || INT3IF){
        //letters--;
        keypad_isr();
        INT0IF = 0;        
        INT1IF = 0;
        INT2IF = 0;
        INT3IF = 0;
    }
    return;
}

void main(void)
{
    
    setup();
    Lcd_Write_String(MOTD_S);
    __delay_ms(1000);
    Lcd_Clear();
    PORTB = PORTB_NOMINAL;
    LOCKED_LED_PIN = 1;
    UNLCOKED_LED_PIN = 0;
    strcpy(password, PASSWD);
    while(1) {
        
        if (new_char == 1) {
            GIE = 0; //Disable interrupts
            Lcd_Write_Char('*');
            __delay_ms(50);
            new_char = 0;
            GIE = 1; //Enable interrupts

        }

        if (letters == 4) {
            attempts++;
            Lcd_Clear();
            __delay_ms(50);
            short password_correct = check_password();
            if (password_correct == 1) {
                Lcd_Write_String("Correct Pass!");
                __delay_ms(1500);
                unlock();
            }
            else {
                Lcd_Write_String("Wrong Pass!");
                __delay_ms(1500);
                delay();
            }
            Lcd_Clear();
            reset_attempt();// Attempt done.
        }
       
    }
    return;
}

void setup(void)
{
    
    ANCON0 = 0;
    ANCON1 = 0;
    TRISD = 0b00001111;
    TRISE0 = 0;
    TRISE2 = 0;
    TRISB = 0b00000111;
    INTCON = 0b10010000;
    INTCON2 = 0b11111000;
    INTCON3 = 0b00111000;
    TRISA = 0b00111111;
    Lcd_Init();
}

void keypad_isr(void)
{
    /*
     ISR Adds the inputed char into input[] in general scope.
     */
    char input_char = get_input_char();
    for (int i = 0; i < sizeof(input) -1; i++){
        if (input[i] == '\0') {
            input[i] = input_char;
            letters++;
            new_char = 1;
            break;
        }
    }
}

char get_input_char(void)
{

    __delay_ms(100);
    char input_char = 0;
    PORTB = 0x10; // Test Row A
    if (PORTBbits.RB0) {
        // Row A Col 1 
        input_char = '1';
    } else if (PORTBbits.RB1) {
        //ROW A Col 2
        input_char = '2';
    } else if (PORTBbits.RB2) {
        //ROW A Col 3
        input_char = '3';
    }
    
    PORTB = 0x20; // Test Row B
    if (PORTBbits.RB0) {
        // Row B Col 1 
        input_char = '4';
    } else if (PORTBbits.RB1) {
        //ROW B Col 2
        input_char = '5';
    } else if (PORTBbits.RB2) {
        //ROW B Col 3
        input_char = '6';
    }
    
    PORTB = 0x40; // Test Row C
    if (PORTBbits.RB0) {
        // Row C Col 1 
        input_char = '7';
    } else if (PORTBbits.RB1) {
        //ROW C Col 2
        input_char = '8';
    } else if (PORTBbits.RB2) {
        //ROW C Col 3
        input_char = '9';
    }
    
     PORTB = 0x80; // Test Row C
    if (PORTBbits.RB0) {
        // Row D Col 1 
        input_char = '*';
    } else if (PORTBbits.RB1) {
        //ROW D Col 2
        input_char = '0';
    } else if (PORTBbits.RB2) {
        //ROW D Col 3
        input_char = '#';
    }
     __delay_ms(100);
     PORTB = PORTB_NOMINAL;
     return input_char;
      
}

short check_password(void)
{
    int result = strcmp(password, input);
    if (result == 0) return 1; // Equal
    else return 0;
}

void unlock(void)
{
    //Unlock and hold until a pin changes (door)
    attempts = 0;
    UNLCOKED_LED_PIN = 1;
    LOCKED_LED_PIN = 0;
    reset_attempt(); //clear input buffer and letters count
    __delay_ms(5000);// Give chance
    if (new_char == 1 && input[0] == '*') {
        new_char = 0;
        reset_pass();
    }
    
    
    UNLCOKED_LED_PIN = 0;
    LOCKED_LED_PIN = 1;
    

}

void delay(void)
{
    //Delay with respect to 'attempts'
    //T0CON = 0x04; //16bit 1:32 pre-scaler OFF
    if (attempts == 1) return; //no  delay after 1
    else if (attempts < 6) __delay_ms(5000); // Five second
    else lock_down();
}

void reset_attempt(void)
{
    letters = 0;
    for (int i = 0; i < sizeof(input); i++){
        input[i] = 0;
    }
    new_char = 0;
}

void lock_down(void)
{
    /*
     For multiple wrong attempts, This should lock down
     the system until power is reset.
     */
    __delay_ms(50);
    Lcd_Clear();
    __delay_ms(50);
    Lcd_Write_String("Locked down");
    while(1);

}

void reset_pass(void)
{
    reset_attempt();
    Lcd_Clear();
    __delay_ms(50);
    Lcd_Write_String("Pass reset");
    __delay_ms(1000);
    Lcd_Clear();
    __delay_ms(50);
    while(letters != 4) {
        if (new_char == 1){
            Lcd_Write_Char('*');
            __delay_ms(50);
            new_char = 0;
        }
    }
    // Here should have new pass in input
    strcpy(password, input); // New pass set
    Lcd_Clear();
    __delay_ms(50);
    
    Lcd_Write_String("Password set");
    __delay_ms(1500);
    Lcd_Clear();
}