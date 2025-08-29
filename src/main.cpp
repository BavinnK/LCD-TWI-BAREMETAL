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
  lcd_send_data((data<<4)&0xF0,  flag);//sending low 4 bit or 4 least significant bit
  i2c_stop();
}
////////////////////////////////////////////////////////////////////////////LAYER 3



void setup() {
  USART_init(my_ubrr);

  
  i2c_init();
  

}

void loop() {
  if (UCSR0A & (1 << RXC0)) { 
    unsigned char data = UDR0;
    USART_TX(data);
    }
  
}
