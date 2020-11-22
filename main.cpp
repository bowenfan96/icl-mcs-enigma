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

/*
 * The main function is kept compact to make the Enigma class as standalone as possible.
 * The Enigma class contains all the processing and validity checking functions.
 * 
 * Throw-catch is used to catch errors within object constructors,
 * because it is the most elegant method I can find:
 * it avoids having to construct zombie objects
 * in order to pass back the error code using an object attribute.
 * 
 * Best effort was made to match the error messages on the LabTS tests verbatim.
 */

int main(int argc, char** argv) 
{
    try {
        Enigma en(argc, argv);
        
        char input;
        while(std::cin >> std::ws >> input) {
            std::cout << en.encryptor(input) << std::flush;
        }
    }
    
    catch(int error) {
        return error;
    }
    
    return(NO_ERROR);
}
