#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <array>
#include <unordered_set>

#include "enigma.h"
#include "errors.h"

/* Plugboard constructor
 * Run these checks:
 * 1. file can be opened
 * 2. remove whitespace and check index is not just whitespace (in case of double whitespace or trailing whitespace)
 * 3. index is numeric
 * 4. index is 0 - 25 using stoi (make sure is numeric before passing to stoi)
 * 5. check 1 to 1 mapping
 * 6. check even number of indices
 */
Enigma::Plugboard::Plugboard(const char* pb_filename) 
{
    std::ifstream pb_file;
    pb_file.open(pb_filename);
    
    if(pb_file.is_open()) {
        
        std::string index;
        int pb_num;
        int i = 0;
        size_t sz;
        
        while(std::getline(pb_file, index, ' ')) {
            
            index.erase(std::remove_if(index.begin(), index.end(), ::isspace), index.end());
            
            if(!index.empty()) {
                if(is_numeric(index)) {
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
                    else {
                        std::cerr << "Index in plugboard file " << pb_filename << " does not correspond to an alphabet letter" << "\n";
                        throw INVALID_INDEX;
                    }
                }
                else {
                    std::cerr << "Non-numeric character in plugboard file " << pb_filename << "\n";
                    throw NON_NUMERIC_CHARACTER;
                }
            }
        }
        
        // check correct config (1 to 1 mapping, no value to itself)
        if(map_from.size() != 0) {
            if(map_to.size() == 0) {
                std::cerr << "Incorrect number of parameters in plugboard file " << pb_filename << "\n";
                throw INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
            }
            if(!is_one_to_one(map_from, map_to)) {
                std::cerr << "Malformed contact points (not linked 1 to 1) in plugboard file " << pb_filename << "\n";
                throw IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
            }
        }
        
        // check odd number of parameters or more than 13 pairings
        if(map_from.size() != map_to.size() || map_from.size() > 13) {
            std::cerr << "Incorrect number of parameters in plugboard file " << pb_filename << "\n";
            throw INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
        }
        
        // mirror the mapping vectors
        std::vector<int> swap = map_from;
        map_from.insert(map_from.end(), map_to.begin(), map_to.end());
        map_to.insert(map_to.end(), swap.begin(), swap.end());
    }
    
    // check if file open failed
    else {
        std::cerr << "Plugboard file " << pb_filename << " supplied cannot be opened" << "\n";
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
}

/* Plugboard mapper function
 * auto keyword is used for vector iterator for simplicity - 
 * the actual type is: std::vector<int>::iterator
 * The function finds the index of the input in the map_from vector, and
 * returns the element at this index in the map_to vector
 */
int Enigma::Plugboard::mapper(int input) 
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


/* Rotor constructor
 * Run these checks:
 * 1. file can be opened
 * 2. remove whitespace and check index is not just whitespace (in case of double whitespace or trailing whitespace)
 * 3. index is numeric
 * 4. index is 0 - 25 using stoi (make sure is numeric before passing to stoi)
 * 5. check 1 to 1 mapping - 
 * 1 to 1 mapping is verified by checking all 26 numbers defining the map are unique
 */
Enigma::Rotor::Rotor(const char* rot_filename) 
{
    std::ifstream rot_file;
    rot_file.open(rot_filename);
    
    if(rot_file.is_open()) {
        
        std::string index;
        int rot_num;
        int i = 0;
        size_t sz;
        
        while(std::getline(rot_file, index, ' ')) {
            
            index.erase(std::remove_if(index.begin(), index.end(), ::isspace), index.end());
            
            if(!index.empty()) {
                if(is_numeric(index)) {
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
                    else {
                        std::cerr << "Index in rotor file " << rot_filename << " does not correspond to an alphabet letter" << "\n";
                        throw INVALID_INDEX;
                    }
                }
                else {
                    std::cerr << "Non-numeric character for mapping in rotor file " << rot_filename << "\n";
                    throw NON_NUMERIC_CHARACTER;
                }
            }
        }
        
        // check mapping is correct, 26 numbers given in rotor file
        if(i < 26) {
            std::cerr << "Not all inputs mapped in rotor file: " << rot_filename << "\n";
            throw INVALID_ROTOR_MAPPING;
        }
        // check all numbers are unique
        std::unordered_set<int> set;
        
        for(int j=0; j<26; j++) {
            if(!set.insert(map_rtl[j]).second) {
                auto repeat_val = std::find(map_rtl.cbegin(), map_rtl.cend(), map_rtl[j]);
                
                std::cerr << "Invalid mapping of input " << j << " to output " << map_rtl[j] << " (output " << map_rtl[j] << " is already mapped to from input " << std::distance(map_rtl.cbegin(), repeat_val) << ") in rotor file: " << rot_filename << "\n";
                
                throw INVALID_ROTOR_MAPPING;
            }
        }
        
        // set position to 0
        // Enigma constructor will turn the rotors after reading pos file
        position = 0;
        
        // rotor is triggered if it rotates to its notch
        triggered = false;
    }
    
    // check if file open failed
    else {
        std::cerr << "Rotor file " << rot_filename << " supplied cannot be opened" << "\n";
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
}

/* Rotor rotate function
 * % 26 is used to correct for complete turns
 * Engage the triggered state if the rotor is turned onto its notch(es)
 */
void Enigma::Rotor::rotate() 
{
    position = (position + 1) % 26;
    
    if(std::find(triggers.cbegin(), triggers.cend(), position) != triggers.cend()) {
        triggered = true;
    }
    else {
        triggered = false;
    }
}

/* Rotor mapper (right to left) function
 * Adjust for position offset during input and output
 */
int Enigma::Rotor::mapper(int input) 
{
    // adjust for offset of the rotor during input
    input = (input + position) % 26;
    
    input = map_rtl[input];
    
    // adjust for offset of the rotor during output
    input = (input - position) % 26;
    
    // account for rollover if input - position is negative
    if(input < 0) {
        input = input + 26;
    }
    
    return input;
}

/* Rotor reflect mapper (left to right) function
 * Adjust for position offset during input and output
 */
int Enigma::Rotor::rf_mapper(int input) 
{
    // adjust for offset of the rotor during input
    input = (input + position) % 26;
    
    input = map_ltr[input];
    
    // adjust for offset of the rotor during output
    input = (input - position) % 26;
    
    // account for rollover if input - position is negative
    if(input < 0) {
        input = input + 26;
    }
    
    return input;
}


/* Reflector constructor
 * Run these checks:
 * 1. file can be opened
 * 2. remove whitespace and check index is not just whitespace (in case of double whitespace or trailing whitespace)
 * 3. index is numeric
 * 4. index is 0 - 25 using stoi (make sure is numeric before passing to stoi)
 * 5. check 1 to 1 mapping
 * 6. check even number of indices
 */
Enigma::Reflector::Reflector(const char* rf_filename) 
{
    std::ifstream rf_file;
    rf_file.open(rf_filename);
    
    if(rf_file.is_open()) {
        
        std::string index;
        int rf_num;
        int i = 0;
        size_t sz;
        
        while(std::getline(rf_file, index, ' ')) {
            
            index.erase(std::remove_if(index.begin(), index.end(), ::isspace), index.end());
            
            if(!index.empty()) {
                if(is_numeric(index)) {
                    if(is_valid(index)) {
                        rf_num = std::stoi(index, &sz);
                        if(i % 2 == 0) {
                            map_from.push_back(rf_num);
                        }
                        else {
                            map_to.push_back(rf_num);
                        }
                        i++;
                    }
                    else {
                        std::cerr << "Index in reflector file " << rf_filename << " does not correspond to an alphabet letter" << "\n";
                        throw INVALID_INDEX;
                    }
                }
                else {
                    std::cerr << "Non-numeric character in reflector file " << rf_filename << "\n";
                    throw NON_NUMERIC_CHARACTER;
                }
            }
        }
        
        // check reflector is not empty or contain only 1 parameter
        if(map_from.size() == 0 || map_to.size() == 0) {
            std::cerr << "Insufficient number of mappings in reflector file: " << rf_filename << "\n";
            throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
        
        // check correct config (1 to 1 mapping, no value to itself)
        if(!is_one_to_one(map_from, map_to)) {
            std::cerr << "Malformed contact points (not linked 1 to 1) in reflector file " << rf_filename << "\n";
            throw INVALID_REFLECTOR_MAPPING;
        }
        
        // check exactly 13 pairings
        if(map_from.size() != map_to.size()) {
            std::cerr << "Incorrect (odd) number of parameters in reflector file " << rf_filename << "\n";
            throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
            
        if(map_from.size() < 13 || map_to.size() < 13) {
            std::cerr << "Insufficient number of mappings in reflector file: " << rf_filename << "\n";
            throw INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
        
        // mirror the mapping vectors
        std::vector<int> swap = map_from;
        map_from.insert(map_from.end(), map_to.begin(), map_to.end());
        map_to.insert(map_to.end(), swap.begin(), swap.end());
    }
    
    // check if file open failed
    else {
        std::cerr << "Reflector file " << rf_filename << " supplied cannot be opened" << "\n";
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
}

/* Reflector mapper function
 * The reflector is already checked to be well-formed, so the mapped_to element can be found for any element in map_from
 */
int Enigma::Reflector::mapper(int input) 
{
    int index = std::distance(map_from.cbegin(), std::find(map_from.cbegin(), map_from.cend(), input));
    
    return map_to[index];
}


/* Enigma constructor
 * Run these checks:
 * 1. enough number of parameters given
 * 2. attempt to load rotors only if there is any
 * 3. check rotors all have starting positions given -
 * I have chosen to ignore excessive positions instead of throwing an exception
 * e.g. 3 rotors with 4 positions specified will still load with the 4th position ignored
 */
Enigma::Enigma(int argc, char** argv) 
{    
    // Minimum 3 parameters: 0 rotors, 1 plugboard, 1 reflector, 1 position
    // Check if there are 3 or more parameters supplied: argc >= 4
    if(argc < 4) {
        std::cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>)* rotor-positions" << "\n";
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
    
    // load Plugboard
    pb = new Plugboard(pb_filename.c_str());
    
    // load rotor positions
    std::ifstream pos_file;
    pos_file.open(pos_filename);
    
    if(pos_file.is_open()) {
        
        std::string index;
        int pos_num;
        int num_pos = 0;
        size_t sz;
        
        while(std::getline(pos_file, index, ' ')) {
            
            index.erase(std::remove_if(index.begin(), index.end(), ::isspace), index.end());
            
            if(!index.empty()) {
                if(is_numeric(index)) {
                    if(is_valid(index)) {
                        pos_num = std::stoi(index, &sz);
                        vec_rot_posn.push_back(pos_num);
                        num_pos++;
                    }
                    else {
                        std::cerr << "Index in rotor positions file " << pos_filename << " does not correspond to an alphabet letter" << "\n";
                        throw INVALID_INDEX;
                    }
                }
                else {
                    std::cerr << "Non-numeric character in rotor positions file " << pos_filename << "\n";
                    throw NON_NUMERIC_CHARACTER;
                }
            }
        }
        
        // check every rotor has a starting position defined
        // excessive positions are ignored
        if(num_pos < num_rotors) {
            std::cerr << "No starting position for rotor " << num_pos << " in rotor position file: " << pos_filename << "\n";
            throw NO_ROTOR_STARTING_POSITION;
        }
    }
    
    // check if position file open failed
    else {
        std::cerr << "Rotor positions file " << pos_filename << " supplied cannot be opened" << "\n";
        throw ERROR_OPENING_CONFIGURATION_FILE;
    }
    
    // load rotors
    if(num_rotors > 0) {
        for(int i=0; i<num_rotors; i++) {
            vec_rotors.push_back(Rotor(rot_filenames.at(i).c_str()));
            vec_rotors.at(i).position = vec_rot_posn.at(i);
        }
    }
    
    // load Reflector
    rf = new Reflector(rf_filename.c_str());
}

/* Enigma encryptor function
 * auto keyword is used for the Rotor vector iterators for simplicity - 
 * the actual type is: std::vector<Enigma::Rotor>::iterator
 */
char Enigma::encryptor(char letter) 
{
    // check input letter is upper case letter
    if(!isupper(letter)) {
        std::cerr << letter << " is not a valid input character (input characters must be upper case letters A-Z)!" << "\n";
        throw INVALID_INPUT_CHARACTER;
    }
    
    int input = letter - 'A';
    
    // send signal through Plugboard
    input = pb->mapper(input);
    
    // send signal through rotors, right to left
    if(num_rotors > 0) {
        turn_rotors();
        
        // use reverse iterator for right to left signal flow
        for(auto rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
            input = rot->mapper(input);
        }
    }
    
    // send signal through reflector
    input = rf->mapper(input);
    
    // reflect signal through rotors, left to right
    if(num_rotors > 0) {
        for(auto rot = vec_rotors.begin(); rot != vec_rotors.end(); ++rot) {
            input = rot->rf_mapper(input);
        }
    }
    
    // reflect signal through Plugboard
    input = pb->mapper(input);
    
    // cast int to corresponding letter
    char letter_out = (char)('A' + input);
    
    return letter_out;
}

/* Enigma turn rotors function
 * A reverse iterator is used, auto is used - the actual type is std::vector<Enigma::Rotor>::iterator
 * Turn a rotor if the rotor to its right is triggered
 */
void Enigma::turn_rotors() 
{
    for(auto rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
        
        if(rot == vec_rotors.rbegin()) {
            rot->rotate();
        }
        else if ((rot-1)->triggered) {
            rot->rotate();
        }
    }
}

/* Static function in Enigma class to check if input is numeric
 * Range-based for loop is used for simplicity to check if every char in index string is numeric
 */
bool Enigma::is_numeric(const std::string& index)
{
    for(auto ch : index) {
        if(!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

/* Static function in Enigma class to check if input is valid alphabet letter (0 to 25)
 * Convert index string to integer using stoi
 * stoi will fail if string is whitespace, so we have checked for this before calling the function
 */
bool Enigma::is_valid(const std::string& index) 
{
    size_t sz;
    int num = std::stoi(index, &sz);
    if(num < 0 || num > 25) {
        return false;
    }
    return true;
}

/* Static function in Enigma class to check if 2 vectors map to each other in a one-to-one relationship
 * If two vector arrays are of equal size (checked before calling this function),
 * and if both arrays contain only unique elements,
 * and if no element maps to itself,
 * then these two vector arrays are in a valid one-to-one mapping relationship
 */
bool Enigma::is_one_to_one(const std::vector<int>& map_from, const std::vector<int>& map_to) 
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

/* Enigma destructor
 * Frees up heap memory
 */
Enigma::~Enigma() {
    delete pb;
    delete rf;
}
