/*
 * File:   main.c
 * Author: hesham
 *
 * Created on April 17, 2021, 2:56 AM
 */


#include <xc.h>
#include "config.h"
#include "lcd.h"

#define MOTD_S "Vault System"

void setup();
char const keyPadMatrix[] =
{
    '1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#',
    0xFF
};
 
void __interrupt() interrupts() {
    if (RBIF && RBIE){
        Lcd_Write_String("Key press");
    }
    return;
}
void main(void) {
    
    setup();
    Lcd_Write_String(MOTD_S);
    __delay_ms(1000);
    Lcd_Clear();
 
    while(1);
    return;
}

void setup() {
    
    ANCON0 = 0;
    ANCON1 = 0;
    TRISD = 0b00001111;
    TRISE0 = 0;
    TRISE2 = 0;
    TRISB = 0xF6;
    INTCON = 0b10001000;
    Lcd_Init();
}
