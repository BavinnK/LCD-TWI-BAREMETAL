#include <Arduino.h>
//we initilize the I2C protocol
void I2C_init(void){
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
  //in this line of code we want todo 3 things first we set the TWINT bit to 1 to make
  TWCR=(1<<TWSTA)|(1<<TWINT)|(1<<TWEA);
}
void setup() {
  I2C_init();

}

void loop() {
  // put your main code here, to run repeatedly:

}
