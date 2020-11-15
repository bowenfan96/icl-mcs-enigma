#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

class plugboard {
public:
    plugboard(const char*);
    std::vector<int> map_from;
    std::vector<int> map_to;
    
    int mapper(int);
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

// plugboard mapper function
int plugboard::mapper(int input) {
    std::vector<int>::iterator iter = std::find(map_from.begin(), map_from.end(), input);
    
    if(iter != map_from.cend()) {
        int index = std::distance(map_from.begin(), iter);
        return map_to.at(index);
    }
    
    else {
        return input;
    }
}


class rotor {
public:
    rotor(const char*);
    int map_to[26];
    std::vector<int> triggers;
    
    void rotate();
    int mapper(int);
};

// rotor constructor
rotor::rotor(const char* rot_filename) {
    std::ifstream rot_file;
    rot_file.open(rot_filename);
    
    int rot_num;
    int i = 0;
    while(rot_file >> rot_num) {
        if(i <= 25) {
            map_to[i] = rot_num;
        }
        else {
            triggers.push_back(rot_num);
        }
        i++;
    }
    
    // Check input mapping is one to one, and all inputs are mapped
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    // Check rotor starting position
}

// rotor rotate function
void rotor::rotate() {
    int swap = map_to[0];
    for(int i=0; i<25; i++) {
        map_to[i] = map_to[i+1];
    }
    map_to[25] = swap;
}

// rotor mapper function
int rotor::mapper(int input) {
    return map_to[input];
}

class reflector {
public:
    reflector(const char*);
    int map_from[13];
    int map_to[13];
    
    int mapper(int);
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

// reflector mapper function
int reflector::mapper(int input) {
    int index = std::distance(map_from, std::find(map_from, map_from + 13, input));
    return map_to[index];
}

/* Press A
 * plugboard makes A --> G
 * rotor 1 makes A --> B default, and in default position 0 (not displaced)
 * rotor 1 eg. BCDEFG
 * rotor 1 gets turned
 * now A --> C (CDEFGB)
 * map_to[i] = map_to[i+1]
 * 
 * so rotor 1 in position 2 (displaced 3 notches)
 * rotor 2 takes in F, unless rotor 1 displaces it
 * e.g F --> X in rotor 2
 * reflector maps X to N
 * N goes into reverse mapping rotor
 * 
 * Reverse mapping rotor:
 * search rotor index corresponding to the letter
 * mapped to same alphabet index
*/




int main() {
    
    // load plugboard
    plugboard pb("/home/bowen/ipl/mcslab_2_bf420/plugboards/V.pb");
    
    // create a vector of rotors
    // std::vector<rotor> rotors;
    
    // get num_rotors from input parameter at the start
    // int num_rotors;
    // rotors.resize(num_rotors);
    
    // load rotors
    
    
    // call plugboard with input integer representing letter
    
    // test with input = 21
    // should output to 1
    int input = 21;
    
    input = pb.mapper(input);
    
    std::cout << input << std::endl;
    
    
    
}
