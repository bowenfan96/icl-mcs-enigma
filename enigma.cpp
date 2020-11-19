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
 * map_ltr[i] = map_ltr[i+1]
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
        std::vector<int> map_rtl;
        std::vector<int> map_ltr;
        int mapper(int);
    };


    class rotor {
    public:
        
        int position;
        
        rotor(const char*);
        
        int map_rtl[26];
        int map_ltr[26];
        std::vector<int> triggers;
        
        bool rightmost;
        
        void rotate();

        int mapper(int);
        int rf_mapper(int);
    };
    
    
    class reflector {
    public:
        reflector(const char*);
        int map_rtl[26];
        int map_ltr[26];
        
        int mapper(int);
    };
    
    enigma(int, char**);
    
    char encryptor(char);
    
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
            map_rtl.push_back(pb_num);
        }
        else if(i % 2 != 0) {
            map_ltr.push_back(pb_num);
        }
        i++;
    }
    
    // mirror the mapping arrays
    std::vector<int> swap = map_rtl;
    map_rtl.insert(map_rtl.end(), map_ltr.begin(), map_ltr.end());
    map_ltr.insert(map_ltr.end(), swap.begin(), swap.end());
    
    // Check if number maps to itself
    // Check if odd number of numbers
    // Check if number is a valid alphabet (0 to 25)
    // Check if inputs are all numbers
    
}

// plugboard mapper function
int enigma::plugboard::mapper(int input) {
    std::vector<int>::iterator iter_fr = std::find(map_rtl.begin(), map_rtl.end(), input);
    
    if(iter_fr != map_rtl.cend()) {
        int index = std::distance(map_rtl.begin(), iter_fr);
        return map_ltr.at(index);
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
    
    /*
    for(int i=0; i<position; i++) {
        int swap = map_rtl[25];
        for(int j=0; j<25; j++) {
            map_rtl[j+1] = map_rtl[j];
        }
        map_ltr[0] = swap;
    }
    */
    
    rightmost = false;
    
    
    // Check input mapping is one to one, and all inputs are mapped
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    // Check rotor starting position
}

// rotor rotate function
void enigma::rotor::rotate() {
    /*
    int swap = map_rtl[25];
    for(int j=0; j<25; j++) {
        map_rtl[j+1] = map_rtl[j];
    }
    map_rtl[0] = swap;
    */
    position++;
    
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
            map_rtl[i/2] = rf_num;
        }
        else if(i % 2 != 0) {
            map_ltr[i/2] = rf_num;
        }
        i++;
    }
    
    // mirror map_rtl and map_ltr
    for(int i=0; i<13; i++) {
        map_rtl[i + 13] = map_ltr[i];
        map_ltr[i + 13] = map_rtl[i];
    }
    
    
    // Check input is not mapped to itself and is one to one
    // Check 13 pairs
    // Check all numbers are 0 to 25
    // Check if inputs are all numbers
    
}

// reflector mapper function
int enigma::reflector::mapper(int input) {
    
    int index = std::distance(map_rtl, std::find(map_rtl, map_rtl + 26, input));
    
    return map_ltr[index];
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
            vec_rot_posn.push_back(vec_rotors[i].position);
        }
        
        // set flag on rightmost rotor
        vec_rotors[num_rotors - 1].rightmost = true;
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
        
        for(std::vector<enigma::rotor>::reverse_iterator rot = vec_rotors.rbegin(); rot != vec_rotors.rend(); ++rot) {
            
            // int rel_posn = rot->position - (rot-1)->position;
            // std::cout << (rot-1)->position << std::endl;
            
            
            if(rot->rightmost) {
                rot->rotate();
                // std::cout << "Hi rotated" << std::endl;
                
                // input = rot->mapper(input);
            }
            
            else {
                if( std::find(std::begin((rot-1)->triggers), std::end((rot-1)->triggers), 
                        rot->position) != std::end((rot-1)->triggers) ) {
                    
                    rot->rotate();
                    // std::cout << "Hi I rotated forwards" << std::endl;
                
                    }
                    
                    // input = rot->mapper(input - (rot - 1)->position);
            }
            
            
            input = rot->mapper(input);
            
            // input = (*rot).mapper(input, (*(rot - 1)).position);
        }
    }
                
    // std::cout << "After right to left rotors: " << input << std::endl;
    
    input = rf->mapper(input);
    
    // std::cout << "After reflector: " << input << std::endl;
    
    if(num_rotors > 0) {
        // rotor after reflectors
        
        for(std::vector<enigma::rotor>::iterator rot = vec_rotors.begin(); rot != vec_rotors.end(); ++rot) {
            
            input = rot->rf_mapper(input);
            
            /*
            if(rot->rightmost) {
                input = rot->rf_mapper(input);
            }
            
            else {
                input = rot->rf_mapper(input + rot->position);
            }
            */
        }
    }
    
    // std::cout << "After left to right rotors, FINAL: " << input << std::endl;
    // std::cout << "Input is " << input << std::endl;
    
    char letter_out = (char)('A' + input);
    return letter_out;
}


int main(int argc, char** argv) {
    
    // load components into enigma
    enigma en(argc, argv);
    
    
    std::string line_in;
    
    while(std::getline(std::cin, line_in)) {
        
        for(size_t i = 0; i != line_in.size(); i++) {
            
            en.output_string.push_back(en.encryptor(line_in[i]));
            
        }
        
        std::cout << en.output_string << std::endl;
        en.output_string.clear();
        
    }
    
    
}
