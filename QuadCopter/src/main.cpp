#include <QuadCopter.h>


using namespace global;
using namespace std;

#define n	5

void print_data();
void receive();
QuadCopter copter;

// Function to remove all spaces from a given string
void trim(string &str) {
    // To keep track of non-space character count
    int count = 0;
 
    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t')
            str[count++] = str[i]; // here count is
                                   // incremented
    str[count] = '\0';
}

vector<string> split(string &str, char delimiter){
	int index = 0;
	vector<string> split_str;
	while(true){
		index = str.find(delimiter);
		int len = str.length();
		if(index == -1){
			if(str.length() != 0) split_str.push_back(str);
			str.clear();
			break;
		}
		split_str.push_back(str.substr(0, index));
		str = str.substr(index+1, len-index+1);
	}
	for(int i = 0; i < split_str.size(); i++)
		trim(split_str[i]);

	return split_str;
}


typedef union {
	uint64_t value;
	struct {
		uint32_t lower, higher;
	};
} big_int;

int main(){
	System.Init();
	Systick.Init();
	Serial.begin(115200);

	copter.Init();

	Madgwick madgwick(0.10f);

	System.reset_cyclic_counter();
	System.enable_cyclic_counter();
	while(1){
		// // receive data from nrf
		// copter.NRF_Read(false);
		//read the IMU
		copter.IMU_Update(madgwick, false);
		//controllers
		copter.Control();

		if(Serial.isAvailable()){
			receive();
		}
		// copter.mpu.printGyro();
		// copter.mpu.printAcc();
		copter.mpu.printQuaternion();
		// print_data();
	}
}

void receive(){
	vector<float> numbers;
	string str = Serial.recieve();
	// Serial.printStrln((char*) str.c_str());
	vector<string> values = split(str, ',');
	if(values.size() == n){
		for(int i = 0; i < n; i++)
			numbers.push_back(stof(values[i]));

		copter.direct = numbers[0];
		copter.desired_roll = numbers[1];
		copter.controllers.Roll.kp = numbers[2];
		copter.controllers.Roll.kd = numbers[3];
		copter.controllers.Roll.ki = numbers[4];
	}
}

void print_data(){
		Serial.printFloat(copter.controllers.Roll.error);
		Serial.printStr(",\t");
		Serial.printFloat(copter.controllers.Roll.propotional);
		Serial.printStr(",\t");
		Serial.printFloat(copter.controllers.Roll.derivative);
		Serial.printStr(",\t");
		Serial.printFloat(copter.controllers.Roll.integral);
		Serial.printStr(",\t");
		Serial.printFloat(copter.motors[0].current_thrust);
		Serial.printStr(",\t");
		Serial.printFloat(copter.motors[1].current_thrust);
		Serial.printStr(",\t");
		Serial.printFloat(copter.motors[2].current_thrust);
		Serial.printStr(",\t");
		Serial.printFloat(copter.motors[3].current_thrust);
		Serial.printStrln("");
}