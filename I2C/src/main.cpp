#include <I2C.h>
#include <GPIO.h>
#include <RCC.h>
#include <USART.h>
#include <System.h>
#include <MPU.h>

using namespace global;


void readAcc();
void readGyro();
void printValues();




#define MPU 0x68

float acc[3];
float gyro[3];


static I2C_Handle_t i2c_handle;
int main(){
	System.Init();
	Serial.begin(115200);

	//configure pins 
	gpio_pin_conf_t pinB6, pinB7;
	pinB6.pin = PB6;
	pinB7.pin = PB7;
	//set mode
	pinB6.mode = GPIO_PIN_ALT_FUN_MODE;
	pinB7.mode = GPIO_PIN_ALT_FUN_MODE;
	//output type
	pinB6.op_type = GPIO_PIN_OP_TYPE_OPEN_DRAIN;
	pinB7.op_type = GPIO_PIN_OP_TYPE_OPEN_DRAIN;
	////set pullups for pin 9 and 10
	pinB6.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinB7.pupd = GPIO_PIN_PUPD_NO_PUPD;
	//set speed
	pinB6.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	pinB7.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	//set alternate function
	pinB6.alternate_function = 4;
	pinB7.alternate_function = 4;

	//enable clock for GPIOA
	RCC_GPIOB_CLK_ENABLE();
	gpio_init(GPIOB, &pinB6);
	gpio_init(GPIOB, &pinB7);

	uint8_t buffer[6];


	RCC_I2C1_CLK_ENABLE();
	i2c_handle.instance = I2C1;
 	I2CInit(&i2c_handle);

	buffer[0] = 0x80;
	i2c_master_rx(&i2c_handle, 0x68, 0x75, buffer, 1);
	for(int i = 0; i < 1000; i++);
	buffer[0] = 0x80;
	i2c_master_tx(&i2c_handle, 0x68, 0x6B, buffer, 1);
	for(int i = 0; i < 1000; i++);
	buffer[0] = 0x00;
	i2c_master_tx(&i2c_handle, 0x68, 0x6B, buffer, 1);
	for(int i = 0; i < 1000; i++);
	buffer[0] = 0x01;
	i2c_master_tx(&i2c_handle, 0x68, 0x6B, buffer, 1);
	for(int i = 0; i < 1000; i++);

	System.reset_cyclic_counter();
	System.enable_cyclic_counter();
	float dt;
	uint32_t prev_micros = 0;
	while(1){
		dt = System.getDeltaT(prev_micros);
		prev_micros = System.getMicros();

		readAcc();
		readGyro();

		string s;
		s.append(Serial.floatToStr(acc[0]));
		s.append("\t");
		s.append(Serial.floatToStr(acc[1]));
		s.append("\t");
		s.append(Serial.floatToStr(acc[2]));
		s.append("\t");
		s.append(Serial.floatToStr(gyro[0]));
		s.append("\t");
		s.append(Serial.floatToStr(gyro[1]));
		s.append("\t");
		s.append(Serial.floatToStr(gyro[2]));
		s.append("\t");
		s.append(Serial.floatToStr(dt));
		Serial.printStrln((char*) s.c_str());

	}

	return 0;
}

void readAcc(){
	uint8_t buffer[6];
	i2c_master_rx(&i2c_handle, 0x68, 0x3B, buffer, 6);

	acc[0] = ((int16_t) ((buffer[0] << 8) | buffer[1]))  / 16384.0f;
	acc[1] = ((int16_t) ((buffer[2] << 8) | buffer[3]))  / 16384.0f;
	acc[2] = ((int16_t) ((buffer[4] << 8) | buffer[5]))  / 16384.0f;
}
void readGyro(){
	uint8_t buffer[6];
	i2c_master_rx(&i2c_handle, 0x68, 0x43, buffer, 6);

	gyro[0] = ((int16_t) ((buffer[0] << 8) | buffer[1])) / 131.0f;
	gyro[1] = ((int16_t) ((buffer[2] << 8) | buffer[3])) / 131.0f;
	gyro[2] = ((int16_t) ((buffer[4] << 8) | buffer[5])) / 131.0f;
}

//void printValues(){
//	printFloat(acc[0]);
//	printStr(",\t\t");
//	printFloat(acc[1]);
//	printStr(",\t\t");
//	printFloat(acc[2]);
//	printStr(",\t\t\t\t");
//	printFloat(gyro[0]);
//	printStr(",\t\t");
//	printFloat(gyro[1]);
//	printStr(",\t\t");
//	printFloat(gyro[2]);
//	printStrln("");
//}