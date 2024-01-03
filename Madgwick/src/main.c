#include <MPU.h>
#include <RCC.h>
#include <USART.h>
#include <math.h>
#include <string.h>
#include <Madgwick.h>

I2C_TypeDef *I2C1_Base = I2C1;
RCC_TypeDef *RCC_Base = RCC;
GPIO_TypeDef *GPIOA_Base = GPIOA;
GPIO_TypeDef *GPIOB_Base = GPIOB;
GPIO_TypeDef *GPIOC_Base = GPIOC;
USART_TypeDef *USART2_Base = USART2;
SPI_TypeDef *SPI1_Base = SPI1;
NVIC_Type* NVIC_Base = NVIC;


int main(){
	SystemInit();
	USART2Init(115200, HSI16);
	mpu_init();
	MadgwickInit(0.041, 0.1);
	Madgwick_t madgwick = *getMadgwick();  
	MPU_t mpu = *getMpu();
	
	while(1){
		readAcc();
		readGyro();
		readMag();
		calculateAngles();
		MadgwickQuaternionUpdate(mpu.acc[0], mpu.acc[1], mpu.acc[2], mpu.gyro[0], mpu.gyro[1], mpu.gyro[2]);
		printValues();
	}

	return 0;
}