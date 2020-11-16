#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
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
int plugboard::mapper(int input) {
    std::vector<int>::iterator iter_fr = std::find(map_from.begin(), map_from.end(), input);
    
    if(iter_fr != map_from.cend()) {
        int index = std::distance(map_from.begin(), iter_fr);
        return map_to.at(index);
    }
    
    else {
        return input;
    }
}






class rotor {
    bool reflected;
    
public:
    int position;
    
    rotor(const char*);
    int map_to[26];
    std::vector<int> triggers;
    
    bool rightmost;
    
    void rotate();
    void reflect();
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
    
    // do offset by position
    // init position to 0 for now
    position = 0;
    
    for(int i=0; i<position; i++) {
        int swap = map_to[0];
        for(int i=0; i<25; i++) {
            map_to[i] = map_to[i+1];
        }
        map_to[25] = swap;
    }
    
    
    reflected = false;
    rightmost = false;
    
    
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
    
    position++;
}

// rotor mapper function
int rotor::mapper(int input) {
    if(!reflected) {
        
        std::cout << "Forward mapped to: " << map_to[input] << std::endl;
        
        return map_to[input];
    }
    
    else {
        int index = std::distance(map_to, std::find(map_to, map_to + 26, input));
        
        std::cout << "Reflected index: " << index << std::endl;
        
        return index;
    }
}

// set reflected flag on rotor
void rotor::reflect() {
    reflected = true;
}





class reflector {
public:
    reflector(const char*);
    int map_from[26];
    int map_to[26];
    
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
int reflector::mapper(int input) {
    
    int index = std::distance(map_from, std::find(map_from, map_from + 26, input));
    
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
    plugboard pb("/home/bowen/ipl/mcslab_2_bf420/plugboards/I.pb");
    
    // call plugboard with input integer representing letter
    
    // test plugboard with input = 23
    // should output to 22
    int input = 0;
    
    input = pb.mapper(input);
    
    std::cout << "After plugboard: " << input << std::endl;
    
    
    
    
    // create a vector of rotors
    std::vector<rotor> vec_rotors;
    
    // get num_rotors from input parameter at the start
    
    // test with 2 rotors now
    int num_rotors = 3;
    // vec_rotors.resize(num_rotors); NOT NEEDED COS VEC_ROTORS ALREADY USE PUSHBACK
    
    // load rotors using a for loop
    
    // make a filename vector for the rotors
    std::vector<std::string> rot_filenames;
    
    /*
    for(int i=0; i<num_rotors; i++) {
        rot_filenames.push_back("/home/bowen/ipl/mcslab_2_bf420/rotors/V.rot");
        
    }*/
    
    rot_filenames.push_back("/home/bowen/ipl/mcslab_2_bf420/rotors/test.rot");
    rot_filenames.push_back("/home/bowen/ipl/mcslab_2_bf420/rotors/test.rot");
    rot_filenames.push_back("/home/bowen/ipl/mcslab_2_bf420/rotors/test.rot");
    
    for(int i=0; i<num_rotors; i++) {
        vec_rotors.push_back(rotor(rot_filenames.at(i).c_str()));
    }
    
    // set flag on rightmost rotor
    vec_rotors[num_rotors - 1].rightmost = true;
    
    // test rotor with input = 0
    // input = 0;
    
    
    // for rotor in rotors - but starting from rightmost
    // a reverse iterator is used
    
    for(std::vector<rotor>::reverse_iterator rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
        if((*rot).rightmost) {
            (*rot).rotate();
        }
        
        else {
            if( std::find(std::begin((*(rot - 1)).triggers), std::end((*(rot - 1)).triggers), 
                     (*(rot - 1)).position) != std::end((*(rot - 1)).triggers) ) {
                
                (*rot).rotate();
            
                }
        }
        
        input = (*rot).mapper(input);
        (*rot).reflect();
    }
    
    std::cout << "After right to left rotors: " << input << std::endl;
    
    
    
    // load reflector
    reflector rf("/home/bowen/ipl/mcslab_2_bf420/reflectors/II.rf");
    
    // test reflector with input = 0
    // should map to 17
    
    // input = 18;
    
    input = rf.mapper(input);
    
    std::cout << "After reflector: " << input << std::endl;
    
    
    // rotor after reflectors
    
    for(std::vector<rotor>::iterator rot = vec_rotors.begin(); rot != vec_rotors.end(); ++rot) {
        input = (*rot).mapper(input);
    }
    
    
    std::cout << "After left to right rotors, FINAL: " << input << std::endl;
    
    
    /*
     * test with input 23 (W)
     * v.pb: 23 > 22
     * v.rot: 22 > 5     rotor rotates - all values shift left
     * iv.rot: 5 > 25
     * v.rf: 25 > 16
     * iv.rot: 16 > 9
     * v.rot: 9 > 19
    
    
    */
    
    
    std::string line;
    
    while (std::getline(std::cin, line)) {
        std::cout << line << std::endl;
        
    }
    
}
