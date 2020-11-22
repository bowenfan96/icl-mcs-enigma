/* Enigma class to incorporate all the components
 * 
 * Each Enigma machine contains:
 * Plugboard - defined in Plugboard class
 * Rotor(s) - defined in Rotor class
 * Reflector - defined in Reflector class
 * 
 * Methods to:
 * Load rotors in the right order and starting positions
 * Turn the rotors
 * Encrypt / Decrypt when an input is sent
 * 
 * Static functions to:
 * Check input is numeric
 * Check input is valid alphabet letter
 * Check two arrays map 1 to 1
 */
class Enigma
{
    class Plugboard {
    private:
        std::vector<int> map_from;
        std::vector<int> map_to;
    public:
        Plugboard(const char*);
        int mapper(int);
    };

    class Rotor {
    private:
        std::array<int, 26> map_rtl;
        std::array<int, 26> map_ltr;
        std::vector<int> triggers;
    public:        
        Rotor(const char*);
        int mapper(int);
        int rf_mapper(int);
        int position;
        bool triggered;
        void rotate();
    };
    
    class Reflector {
    private:
        std::vector<int> map_from;
        std::vector<int> map_to;
    public:
        Reflector(const char*);
        int mapper(int);
    };

private:
    int num_rotors;
    void turn_rotors();
    std::vector<Rotor> vec_rotors;
    std::vector<int> vec_rot_posn;
    Plugboard* pb = NULL;
    Reflector* rf = NULL;
public:
    Enigma(int, char**);
    char encryptor(char);
    
    static bool is_numeric(const std::string&);
    static bool is_valid(const std::string&);
    static bool is_one_to_one(const std::vector<int>&, const std::vector<int>&);
};
