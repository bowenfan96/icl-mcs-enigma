#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>

#include "errors.h"

// enigma class to incorporate all the components
class enigma {
public:

    
    class plugboard {
    public:
        plugboard(const char*);
        std::vector<int> map_from;
        std::vector<int> map_to;
        int mapper(int);
    };


    class rotor {
    public:
        
        rotor(const char*);
        
        
        int position;
        int map_rtl[26];
        int map_ltr[26];
        
        std::vector<int> triggers;
        bool triggered;
        
        void rotate();
        int mapper(int);
        int rf_mapper(int);
    };
    
    
    class reflector {
    public:
        reflector(const char*);
        int map_from[26];
        int map_to[26];
        
        int mapper(int);
    };
    
    
    enigma(int, char**);
    
    char encryptor(char);
    void turn_rotors();
    
    int num_rotors;
    
    // rotors loaded as leftmost being element 0
    std::vector<rotor> vec_rotors;
    
    // vector of rotor positions
    std::vector<int> vec_rot_posn;
    
    plugboard* pb = NULL;
    reflector* rf = NULL;
    
    std::string output_string;
};


// plugboard constructor
enigma::plugboard::plugboard(const char* pb_filename) {
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
    
    // mirror the mapping arrays
    std::vector<int> swap = map_from;
    map_from.insert(map_from.end(), map_to.begin(), map_to.end());
    map_to.insert(map_to.end(), swap.begin(), swap.end());
    
    // Check if number maps to itself
    // Check if odd number of numbers
    // Check if number is a valid alphabet (0 to 25)
    // Check if inputs are all numbers
    
}

// plugboard mapper function
int enigma::plugboard::mapper(int input) {
    
    auto iter = std::find(map_from.cbegin(), map_from.cend(), input);
    
    if(iter != map_from.cend()) {
        int index = std::distance(map_from.cbegin(), iter);
        return map_to.at(index);
    }
    
    else {
        return input;
    }
}


// rotor constructor
enigma::rotor::rotor(const char* rot_filename) {
    std::ifstream rot_file;
    rot_file.open(rot_filename);
    
    int rot_num;
    int i = 0;
    while(rot_file >> rot_num) {
        if(i <= 25) {
            map_rtl[i] = rot_num;
            map_ltr[rot_num] = i;
        }
        else {
            triggers.push_back(rot_num);
        }
        i++;
    }
    
    // do offset by position
    // init position to 0 for now
    position = 0;
    
    triggered = false;
    
    
    // Check input mapping is one to one, and all inputs are mapped
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    // Check rotor starting position
}

// rotor rotate function
void enigma::rotor::rotate() {
    position = (position + 1) % 26;
    
    // std::cout << "Rotated" << std::endl;
    
    if(std::find(triggers.cbegin(), triggers.cend(), position) != triggers.cend()) {
        
        // std::cout << "Triggered" << std::endl;
        triggered = true;
    }
    
    else {
        
        // std::cout << "NOT triggered" << std::endl;
        triggered = false;
    }
}

// rotor mapper function
int enigma::rotor::mapper(int input) {
    
    // adjust for position of the rotor during input
    input = (input + position) % 26;
    
    // int index = std::distance(map_rtl, std::find(map_rtl, map_rtl + 26, input));
    
    input = map_rtl[input];
    
    // adjust for position of the rotor during output
    input = (input - position) % 26;
    
    // account for rollover if input - position is negative
    if(input < 0) {
        input = input + 26;
    }
    
    // std::cout << "Forward mapped to: " << input << std::endl;
    
    return input;

}


// rotor reflect mapper function
int enigma::rotor::rf_mapper(int input) {
    
    // adjust for position of the rotor during input
    input = (input + position) % 26;
    
    // int index = std::distance(map_ltr, std::find(map_ltr, map_ltr + 26, input));
    
    input = map_ltr[input];
    
    // adjust for position of the rotor during output
    
    input = (input - position) % 26;
    
    // account for rollover if input - position is negative
    if(input < 0) {
        input = input + 26;
    }
    
    // std::cout << "Reflected index: " << input << std::endl;
    
    return input;
    
}


// reflector constructor
enigma::reflector::reflector(const char* rf_filename) {
    std::ifstream rf_file;
    rf_file.open(rf_filename);
    
    int rf_num;
    int i = 0;
    while(rf_file >> rf_num) {
        if(i % 2 == 0) {
            map_from[i/2] = rf_num;
        }
        else if(i % 2 != 0) {
            map_to[i/2] = rf_num;
        }
        i++;
    }
    
    // mirror map_from and map_to
    for(int i=0; i<13; i++) {
        map_from[i + 13] = map_to[i];
        map_to[i + 13] = map_from[i];
    }
    
    
    // Check input is not mapped to itself and is one to one
    // Check 13 pairs
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    
}

// reflector mapper function
int enigma::reflector::mapper(int input) {
    
    int index = std::distance(map_from, std::find(map_from, map_from + 26, input));
    
    return map_to[index];
}


// enigma constructor
enigma::enigma(int argc, char** argv) {
    
    std::string pb_filename;
    std::string rf_filename;
    std::vector<std::string> rot_filenames;
    std::string pos_filename;
        
    pb_filename = argv[1];
    rf_filename = argv[2];
    
    num_rotors = 0;
    
    if(argc > 4) {
        for(int i=3; i < (argc - 1); i++) {
            rot_filenames.push_back(argv[i]);
            num_rotors++;
        }
    }
    
    pos_filename = argv[argc - 1];
    
    // load plugboard
    pb = new plugboard(pb_filename.c_str());
    
    
    // load rotor positions
    std::ifstream pos_file;
    pos_file.open(pos_filename);
    
    int rot_pos;
    while(pos_file >> rot_pos) {
        vec_rot_posn.push_back(rot_pos);
    }
    
    // load rotors
    if(num_rotors > 0) {
        for(int i=0; i<num_rotors; i++) {
            vec_rotors.push_back(rotor(rot_filenames.at(i).c_str()));
            vec_rotors.at(i).position = vec_rot_posn.at(i);
        }
    }
    
    
    // load reflector
    rf = new reflector(rf_filename.c_str());
    
    output_string.clear();
}


// enigma encryptor function
char enigma::encryptor(char letter) {
    
    int input = letter - 'A';
    
    // std::cout << "Input is " << input << std::endl;
        
    input = pb->mapper(input);
    
    // std::cout << "After plugboard: " << input << std::endl;
        
    if(num_rotors > 0) {
        // for rotor in rotors - but starting from rightmost
        // a reverse iterator is used
        
        //std::vector<enigma::rotor>::reverse_iterator
        turn_rotors();
        
        for(auto rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
            
            input = rot->mapper(input);
        }
    }
                
    // std::cout << "After right to left rotors: " << input << std::endl;
    
    input = rf->mapper(input);
    
    // std::cout << "After reflector: " << input << std::endl;
    
    if(num_rotors > 0) {
        // rotor after reflectors
        
        for(auto rot = vec_rotors.begin(); rot != vec_rotors.end(); ++rot) {
            
            input = rot->rf_mapper(input);
            
        }
    }
    
    input = pb->mapper(input);
    
    // std::cout << "After left to right rotors, FINAL: " << input << std::endl;
    
    char letter_out = (char)('A' + input);
    return letter_out;
}


// enigma turn rotors function
void enigma::turn_rotors() {
    
    // vec_rotors.rbegin()->rotate();
    
    for(auto rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
        
        if(rot == vec_rotors.rbegin()) {
            rot->rotate();
            
            // std::cout << "Rotated 1" << std::endl;
        }
        
        else if ((rot-1)->triggered) {
            rot->rotate();
            
            // std::cout << "Rotated 2" << std::endl;
        }
    }
}


int main(int argc, char** argv) {
    
    // load components into enigma
    enigma en(argc, argv);
    
    
    std::string line_in;
    
    while(std::getline(std::cin, line_in)) {
        
        for(size_t i = 0; i != line_in.size(); i++) {
            
            std::cout << en.encryptor(line_in[i]) << std::flush;
            // en.output_string.push_back(en.encryptor(line_in[i]));
            
        }
        
        std::cout << std::endl;
        en.output_string.clear();
        
    }
    
    return (NO_ERROR);
    
    
}
