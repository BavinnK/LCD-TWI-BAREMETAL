#include <Arduino.h>
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
   while(!(TWCR&(1<<TWINT)));
}
void setup() {
  i2c_init();

}

void loop() {
  // put your main code here, to run repeatedly:

}
