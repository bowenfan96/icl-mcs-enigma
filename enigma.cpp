#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <array>
#include <unordered_set>

#include "errors.h"


// enigma class to incorporate all the components
class enigma 
{
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
        
        std::array<int, 26> map_rtl;
        std::array<int, 26> map_ltr;
        
        // int map_rtl[26];
        // int map_ltr[26];
        
        std::vector<int> triggers;
        bool triggered;
        
        void rotate();
        int mapper(int);
        int rf_mapper(int);
    };
    
    
    class reflector {
    public:
        reflector(const char*);
        
        std::vector<int> map_from;
        std::vector<int> map_to;

        // int map_from[26];
        // int map_to[26];
        
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
    
    static bool is_valid(const std::string&);
    static bool is_one_to_one(const std::vector<int>&, const std::vector<int>&);

};


// plugboard constructor
enigma::plugboard::plugboard(const char* pb_filename) 
{
    std::ifstream pb_file;
    pb_file.open(pb_filename);
    
    if(pb_file.is_open()) {
        
        std::string index;
        int pb_num;
        int i = 0;
        size_t sz;
        
        while(std::getline(pb_file, index, ' ')) {
            
            if(is_valid(index)) {
                
                pb_num = std::stoi(index, &sz);
                
                if(i % 2 == 0) {
                    map_from.push_back(pb_num);
                }
                else {
                    map_to.push_back(pb_num);
                }
                i++;
            }
        }
        
        std::cout << "map_from size: " << map_from.size() << std::endl;
        
        std::cout << "map_to size: " << map_to.size() << std::endl;
        
        // check correct config (1 to 1 mapping, no value to itself)
        if(map_from.size() != 0) {
            if(map_to.size() == 0) {
                throw INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
            }
            if(!is_one_to_one(map_from, map_to)) {
                throw IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
            }
        }
        
        // check odd number of parameters or more than 13 pairings
        if(map_from.size() != map_to.size() || map_from.size() > 13) {
            throw INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
        }
        
        // mirror the mapping vectors
        std::vector<int> swap = map_from;
        map_from.insert(map_from.end(), map_to.begin(), map_to.end());
        map_to.insert(map_to.end(), swap.begin(), swap.end());
    }
    
    // check if file open failed
    else {
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
    
    
    
    // Check if number maps to itself - done
    // Check if odd number of numbers - done
    // Check if number is a valid alphabet (0 to 25) - done
    // Check if inputs are all numbers - done
    
}

// plugboard mapper function
int enigma::plugboard::mapper(int input) 
{
    
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
enigma::rotor::rotor(const char* rot_filename) 
{
    std::ifstream rot_file;
    rot_file.open(rot_filename);
    
    if(rot_file.is_open()) {
        
        std::string index;
        int rot_num;
        int i = 0;
        size_t sz;
        
        while(std::getline(rot_file, index, ' ')) {
            
            if(is_valid(index)) {
                
                rot_num = std::stoi(index, &sz);
                
                if(i <= 25) {
                    map_rtl[i] = rot_num;
                    map_ltr[rot_num] = i;
                }
                else {
                    triggers.push_back(rot_num);
                }
                i++;
            }
        }
        
        // check mapping is correct
        // check 26 numbers given in rotor file
        if(i < 26) {
            throw INVALID_ROTOR_MAPPING;
        }
        // check all numbers are unique
        std::unordered_set<int> set;
        for(int j=0; j < 26; j++) {
            if(!set.insert(map_rtl[j]).second) {
                throw INVALID_ROTOR_MAPPING;
            }
        }
        
        // set position 0 until enigma constructor turns the rotor after reading pos file
        position = 0;
        
        triggered = false;
    }
    
    // check if file open failed
    else {
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
    
    // Check input mapping is one to one, and all inputs are mapped - done
    // Check all numbers are 0 to 25 - done
    // Check if inputs are all numbers - done
    // Check rotor starting position
}

// rotor rotate function
void enigma::rotor::rotate() 
{
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
int enigma::rotor::mapper(int input) 
{
    
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
int enigma::rotor::rf_mapper(int input) 
{
    
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
enigma::reflector::reflector(const char* rf_filename) 
{
    std::ifstream rf_file;
    rf_file.open(rf_filename);
    
    if(rf_file.is_open()) {
        
        std::string index;
        int rf_num;
        int i = 0, j = 0, k = 0;
        size_t sz;
        
        while(std::getline(rf_file, index, ' ')) {
            
            if(is_valid(index)) {
                
                rf_num = std::stoi(index, &sz);
                
                if(i % 2 == 0) {
                    map_from.push_back(rf_num);
                    j++;
                }
                else {
                    map_to.push_back(rf_num);
                    k++;
                }
                i++;
            }
        }
        
        // check reflector is not empty or contain only 1 parameter
        if(map_from.size() == 0 || map_to.size() == 0) {
            throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
        
        // check correct config (1 to 1 mapping, no value to itself)
        if(!is_one_to_one(map_from, map_to)) {
            throw INVALID_REFLECTOR_MAPPING;
        }
        
        // check exactly 13 pairings
        if(map_from.size() != 13 || map_to.size() != 13) {
            throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
        
        // mirror the mapping vectors
        std::vector<int> swap = map_from;
        map_from.insert(map_from.end(), map_to.begin(), map_to.end());
        map_to.insert(map_to.end(), swap.begin(), swap.end());
    }
    
    // check if file open failed
    else {
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
    
    
    
    // Check input is not mapped to itself and is one to one - done
    // Check 13 pairs - done
    // Check all numbers are 0 to 25 - done
    // Check if inputs are all numbers - done
    
}

// reflector mapper function
int enigma::reflector::mapper(int input) 
{
    
    int index = std::distance(map_from.cbegin(), std::find(map_from.cbegin(), map_from.cend(), input));
    
    return map_to[index];
}


// enigma constructor
enigma::enigma(int argc, char** argv) 
{
    
    // Minimum 3 parameters: 0 rotors, 1 plugboard, 1 reflector, 1 position
    // Check if there are 3 or more parameters supplied: argc >= 4
    if(argc < 4) {
        throw INSUFFICIENT_NUMBER_OF_PARAMETERS;
    }
    
    std::string pb_filename;
    std::string rf_filename;
    std::vector<std::string> rot_filenames;
    std::string pos_filename;
        
    pb_filename = argv[1];
    rf_filename = argv[2];
    
    // check if there are rotor(s), then load them
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
}


// enigma encryptor function
char enigma::encryptor(char letter) 
{
    
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
void enigma::turn_rotors() 
{
    
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


bool enigma::is_valid(const std::string& index)
{
    for(auto ch : index) {
        if(!isdigit(ch) && !isspace(ch)) {
            throw NON_NUMERIC_CHARACTER;
        }
    }
    
    size_t sz;
    int num = std::stoi(index, &sz);
    if(num < 0 || num > 25) {
        throw INVALID_INDEX;
    }
    
    return true;
}


bool enigma::is_one_to_one(const std::vector<int>& map_from, const std::vector<int>& map_to) 
{
    // check if both arrays contain only unique elements
    int sz = map_to.size();
    
    std::unordered_set<int> set1;
    for(int i=0; i < sz; i++) {
        if(!set1.insert(map_from[i]).second) {
            return false;
        }
    }
    std::unordered_set<int> set2;
    for(int i=0; i < sz; i++) {
        if(!set2.insert(map_to[i]).second) {
            return false;
        }
    }
    
    // check if an index maps to itself
    for(int i=0; i < sz; i++) {
        if(map_from.at(i) == map_to.at(i)) {
            return false;
        }
    }
    
    return true;
}


int main(int argc, char** argv) 
{
    // load components into enigma
    try {
        enigma en(argc, argv);
        
        char input;
        while(std::cin >> std::ws >> input) {
            if(isupper(input)) {
                std::cout << en.encryptor(input) << std::flush;
            }
            else {
                throw INVALID_INPUT_CHARACTER;
            }
        }
    }
    
    catch(int error) {
        // std::cout << "Error code: " << error << std::endl;
        return error;
    }
    
    

    /*
    std::string line_in;
    
    while(std::getline(std::cin >> std::ws, line_in)) {
        
        for(size_t i = 0; i != line_in.size(); i++) {
            
            std::cout << en.encryptor(line_in[i]) << std::flush;
            // en.output_string.push_back(en.encryptor(line_in[i]));
            
        }
        
        std::cout << std::endl;
        en.output_string.clear();
        
    }
    */
    
    return(NO_ERROR);
    
    
}
