#include "LineFollower.h"

LineFollower lineFollower;

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

void receive(){
	float numbers[5];
	string str = Serial.recieve();
	int n = 5;
	// Serial.printStrln((char*) str.c_str());
	vector<string> values = split(str, ',');
	if(values.size() == n){
		for(int i = 0; i < n; i++)
			numbers[i] = stof(values[i]);

		lineFollower.direct = numbers[0];
		// lineFollower.desired_roll = numbers[1];
		lineFollower.pid.kp = numbers[2];
		lineFollower.pid.kd = numbers[3];
		lineFollower.pid.ki = numbers[4];
	}
}


int main(){
	System.Init();
	Serial.begin(115200);
	lineFollower = LineFollower();


	System.reset_cyclic_counter();
	System.enable_cyclic_counter();

	lineFollower.calibration();
	while(1){
		lineFollower.readIrSensors();
		// lineFollower.printRawValues();
		lineFollower.printValues();
		// lineFollower.printBinary();
		// lineFollower.printError();
	
		lineFollower.control();

		// if(Serial.isAvailable()){
		// 	receive();
		// }
	}
}
