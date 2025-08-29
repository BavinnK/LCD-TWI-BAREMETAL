#include <util/delay.h>
#include <Arduino.h>
#include "MyUSART.h"
#define clk_speed 16000000
#define baud 9600
#define my_ubrr (clk_speed/16/baud-1)
#define slave_add    0b01001110
#define backLight    0b00001000 //third bit for display on/off
#define command_flag 0b00000000 //if RS is zero it means command 
#define data_flag    0b00000001 //else if its 1 it means we want to display data
//we initilize the I2C protocol
void i2c_init(void){
  //we set the TWSR to zero the default bc the datasheet says so lol so basically we set the prescaler to zero
  TWSR=0x00;
  //after that we gonna set up the  TWBR register to 72 why? bc of this formula
  //SCL freq = 16MHz / (16 + 2 * TWBR * 1) => TWBR = 72
  TWBR=72;
  //after wee need to enable the TWI BY setting the third bit of the TWCR the third bit of that register
  TWCR=(1<<TWEN);
}
//in here we gonna send the start command to a specific I2C device
void i2c_start(void){
  //in this line of code we want todo 3 things first we set the TWINT bit to 1  so we can make it to zero why is that ??? bc when we set that bit to 1
  //basiccally it clears the bit to zero by this the hardware says alr i will start the communication to that specific device
  //TWEA bit controls the generation of the acknowledge pulse
  //TWEN: TWI Enable Bit  //for more info look at the data sheet
  TWCR=(1<<TWSTA)|(1<<TWINT)|(1<<TWEN);
  //and here for this loop in the comments we said when its 0 "TWINT" bit it means the hardware is not finished and its sending a start to the device how do we know
  //it send it ?? by a loop
  while(!(TWCR&(1<<TWINT)));
}
//this function sends the data via I2C protocol
void i2c_write(uint8_t data){
  TWDR=data;
  TWCR=(1<<TWINT)|(1<<TWEN);
  while(!(TWCR&(1<<TWINT)));
  
}
void i2c_stop(){
  TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
   //while(!(TWCR&(1<<TWINT)));
}
////////////////////////////////////////////////////////////////////////////////////////////Layer 2
void lcd_sendNibble(uint8_t nibble,uint8_t flag){
  uint16_t lcd_byte=(nibble|flag|backLight);
  i2c_write(lcd_byte|1<<2);
  _delay_ms(5);
  i2c_write(lcd_byte);
}
void lcd_send_data(uint8_t data,uint8_t flag){
  i2c_start();
  i2c_write(slave_add);
  lcd_sendNibble((data&0xF0),  flag);//sending 4 high bit or 4 most significant bit
  lcd_sendNibble((data<<4)&0xF0,  flag);//sending low 4 bit or 4 least significant bit
  i2c_stop();
}
////////////////////////////////////////////////////////////////////////////LAYER 3
void lcd_cmd(uint8_t command){
  lcd_send_data(command, command_flag);

}
void lcd_char(char character){
  lcd_send_data(character, data_flag);
}
void lcd_init(void){
  //a special wake up sequence
  uint8_t wakeUP=0x30;
  i2c_start();
  _delay_ms(40);
  i2c_write(slave_add);//sending slave add
  _delay_ms(1);
  lcd_sendNibble(wakeUP,command_flag); 
  _delay_ms(1);
  lcd_sendNibble(wakeUP,command_flag); 
  _delay_ms(1);
  lcd_sendNibble(wakeUP,command_flag); 
  _delay_ms(1);
  lcd_sendNibble(wakeUP,command_flag); 
  _delay_ms(1);
  lcd_sendNibble(wakeUP,command_flag); 
  _delay_ms(5);

  //now that we woke up the chip from the lcd now we gonna do our configration look at the HD44780U (LCD-II), (Dot Matrix Liquid Crystal Display Controller/Driver) datasheet for  more info
  lcd_cmd(0b00000010);//return home add
  _delay_ms(5);
  lcd_cmd(0b00101000);//funstion set: DL=4 bit | N=2lines | F=5x8 dots
  _delay_ms(5);
  lcd_cmd(0b00001100);//display control: display on \ cursor move\ no blink
  _delay_ms(5);
  lcd_cmd(0b00000001);//clearing the disp
  _delay_ms(5);
  lcd_cmd(0b00000110);//entry mode set
  _delay_ms(5);
  i2c_stop();
}
//now we need a function to display strings on the lcd
void lcd_print_string(const char* str) {
    while (*str) {
        lcd_char(*str++);
        USART_strTransmit(str);
    }
}//done


void setup() {
  USART_init(my_ubrr);

  
  i2c_init();
  lcd_init();
  lcd_print_string("Bavin, Engineer");
  

}

void loop() {
  if (UCSR0A & (1 << RXC0)) { 
    unsigned char data = UDR0;
    USART_TX(data);
    }
   
  
}
