#include <NRF24.h>
#include <JoyStick.h>

using namespace global;

int map(int value, int oldMin, int oldMax, int newMin, int newMax){
	float temp = (float) (value - oldMin)/(oldMax - oldMin);
	return temp * (newMax - newMin) + newMin;
}

int main(){
	uint8_t reg;
	NRF24 nrf;
	uint8_t size = 4;
	uint8_t data[size];

	System.Init();
	Serial.begin(115200);

	Potentiometer pot1(GPIOA, PA0, 0);
	Potentiometer pot2(GPIOA, PA1, 1);

	Potentiometer pot3(GPIOB, PB0, 8);
	Potentiometer pot4(GPIOB, PB1, 9);


	ADCx_init_t init_struct;
	init_struct.instance = ADC1;
	ADCInit(&init_struct);

	nrf.init(CE_PIN, CSN_PIN);
	// nrf.setTxMode(nrf.defaultAddr, 0);
	nrf.setRxMode(nrf.defaultAddr, 0);

	char read[8];
	read[0] = nrf.readReg(0x10);
	for(int i = 0; i < 7; i++){
		read[i] = nrf.readReg(i);
	}

	uint8_t status = 0xff;
	nrf.receive();
	while(1){
		// data[0] = map(pot1.readPot(), 0, 4095, -45, 45);
		// data[1] = map(pot2.readPot(), 0, 4095, -45, 45);
		// data[2] = map(pot3.readPot(), 0, 4095, -45, 45);
		// data[3] = map(pot4.readPot(), 0, 4095, -45, 45);

		// nrf.transmit(data, size);
		// nrf.transmit((uint8_t *)"Hello world", 13);


		status = nrf.readReg(STATUS);
		if(nrf.isDataAvailable(0)){
			nrf.receive();
			nrf.printData();
		}
		// else {
		// 	Serial.printStrln("No data found");
		// }
	}

  return 0;
}
