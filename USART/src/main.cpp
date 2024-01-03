#include <USART.h>
#include <GPIO.h>
#include <RCC.h>
#include <System.h>

using namespace global;

int main(){
	//init USART2 and Use HSI 16 for the clock
	System.Init();

	Serial.begin(115200);

	Serial.printStrln("Started");
	while (1) {
		Serial.printStrln("Hello World");

		// if(Serial.isAvailable()){
		// 	string str = Serial.recieve();
		// 	Serial.printInt(str.length());
		// 	Serial.printStrln("");
		// 	Serial.printStrln((char*) str.c_str());
		// }
	}
}