#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class plugboard {
public:
    std::vector<int> map_from;
    std::vector<int> map_to;
};

// plugboard constructor
plugboard::plugboard(const char* pb_filename) {
    std::ifstream pb_file;
    pb_file.open(pb_filename);
    
    int pb_num;
    int i = 0;
    while(pb_file >> pb_num) {
        if(i % 2 == 0) {
            map_from.push_back(pb_num);
        }
        else if(i % 2 != 0) {
            map_to.push_back(pb_num);
        }
        i++;
    }
    
    // Check if number maps to itself
    // Check if odd number of numbers
    // Check if number is a valid alphabet (0 to 25)
    // Check if inputs are all numbers
    
}

class rotor {
public:
    int map_to[26];
    std::vector<int> triggers;
};

// rotor constructor
rotor::rotor(const char* rot_filename) {
    std::ifstream rot_file;
    rot_file.open(rot_filename);
    
    int rot_num;
    int i = 0;
    while(rot_file >> rot_num) {
        if(i <= 25) {
            map_to[i] = rot_num
        }
        else {
            triggers.push_back(rot_num)
        }
        i++
    }
    
    // Check input mapping is one to one, and all inputs are mapped
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    // Check rotor starting position
}

class reflector {
public:
    int map_from[13];
    int map_to[13];
};

// reflector constructor
reflector::reflector(const char* rf_filename) {
    std::ifstream rf_file;
    rf_file.open(rf_filename);
    
    int rf_num;
    int i = 0;
    while(rf_file >> rf_num) {
        if(i % 2 == 0) {
            map_from[i] = rf_num;
        }
        else if(i % 2 != 0) {
            map_to[i] = rf_num;
        }
        i++;
    }
    
    // Check input is not mapped to itself and is one to one
    // Check 13 pairs
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    
}



int main() {
    plugboard pb;
    pb.pb_importer("/home/bowen/ipl/mcslab_2_bf420/plugboards/V.pb");
    for(int i=0; i<pb.map_from.size();i++) {
        std::cout << pb.map_from[i] << std::endl;
    }
}
