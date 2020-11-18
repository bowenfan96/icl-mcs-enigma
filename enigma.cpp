#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>






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
        bool reflected;
        bool rotated = false;
    
        int position;
        
        rotor(const char*);
        
        int map_from[26];
        int map_to[26];
        std::vector<int> triggers;
        
        bool rightmost;
        
        void rotate(bool);
        void reflect();
        int mapper(int, int);
    };
    
    
    class reflector {
    public:
        reflector(const char*);
        int map_from[26];
        int map_to[26];
        
        int mapper(int);
    };
    
    enigma(int, char**);
    int num_rotors;
    std::vector<rotor> vec_rotors;
    plugboard* pb = NULL;
    reflector* rf = NULL;
    
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
    std::vector<int>::iterator iter_fr = std::find(map_from.begin(), map_from.end(), input);
    
    if(iter_fr != map_from.cend()) {
        int index = std::distance(map_from.begin(), iter_fr);
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
void enigma::rotor::rotate(bool fwd) {
    
    if(fwd) {
        int swap = map_to[0];
        for(int i=0; i<25; i++) {
            map_to[i] = map_to[i+1];
        }
        map_to[25] = swap;
        
        position++;
        
        rotated = true;
    }
    
    else {
        int swap = map_to[25];
        for(int i=0; i<25; i++) {
            map_to[i+1] = map_to[i];
        }
        map_to[0] = swap;
        
    }
}

// rotor mapper function
int enigma::rotor::mapper(int input, int prev_rot_posn) {
    if(!reflected) {
        
        std::cout << "Forward mapped to: " << map_to[input - prev_rot_posn] << std::endl;
        
        return map_to[input- prev_rot_posn];
    }
    
    else {
        int index = std::distance(map_to, std::find(map_to, map_to + 26, input - prev_rot_posn));
        
        std::cout << "Reflected index: " << index << std::endl;
        
        return index;
    }
}

// set reflected flag on rotor
void enigma::rotor::reflect() {
    reflected = true;
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
    
    
    // load rotors
    if(num_rotors > 0) {
        for(int i=0; i<num_rotors; i++) {
            vec_rotors.push_back(rotor(rot_filenames.at(i).c_str()));
        }
        
        // set flag on rightmost rotor
        vec_rotors[num_rotors - 1].rightmost = true;
    }
    
    
    // load reflector
    rf = new reflector(rf_filename.c_str());
}


int main(int argc, char** argv) {
    
    // load components into enigma
    enigma en(argc, argv);
    
    char letter_in;
    
    while (std::cin >> std::ws >> letter_in) {
        
        int input = letter_in - 'A';
        std::cout << "Input is " << input << std::endl;
        
        input = en.pb->mapper(input);
    
        std::cout << "After plugboard: " << input << std::endl;
        
        if(en.num_rotors > 0) {
            // for rotor in rotors - but starting from rightmost
            // a reverse iterator is used
            
            for(std::vector<enigma::rotor>::reverse_iterator rot = en.vec_rotors.rbegin(); rot != en.vec_rotors.rend(); ++rot) {
                if((*rot).rightmost) {
                    (*rot).rotate(1);
                    
                    input = (*rot).mapper(input, 0);
                }
                
                else {
                    if( std::find(std::begin((*(rot - 1)).triggers), std::end((*(rot - 1)).triggers), 
                            (*(rot)).position) != std::end((*(rot - 1)).triggers) ) {
                        
                        (*rot).rotate(1);
                        std::cout << "Hi I rotated forwards" << std::endl;
                    
                        }
                        
                        input = (*rot).mapper(input, (*(rot - 1)).position);
                }
                
                // input = (*rot).mapper(input, (*(rot - 1)).position);
                (*rot).reflect();
            }
        }
        
        
        std::cout << "After right to left rotors: " << input << std::endl;
        
        input = en.rf->mapper(input);
        
        std::cout << "After reflector: " << input << std::endl;
        
        if(en.num_rotors > 0) {
            // rotor after reflectors
            
            for(std::vector<enigma::rotor>::iterator rot = en.vec_rotors.begin(); rot != en.vec_rotors.end(); ++rot) {
                
                if((*rot).rotated) {
                    (*rot).rotate(0);
                    std::cout << "Hi I rotated back" << std::endl;
                }
                
                input = (*rot).mapper(input, (*rot).position);
            }
        }
        
        std::cout << "After left to right rotors, FINAL: " << input << std::endl;
        // std::cout << "Input is " << input << std::endl;
        char letter_out = (char)('A' + input);
        
        std::cout << letter_out;
        
    }    
    
}
