// enigma class to incorporate all the components
class enigma
{
    class plugboard {
    private:
        std::vector<int> map_from;
        std::vector<int> map_to;
    public:
        plugboard(const char*);
        int mapper(int);
    };

    class rotor {
    private:
        std::array<int, 26> map_rtl;
        std::array<int, 26> map_ltr;
        std::vector<int> triggers;
    public:        
        rotor(const char*);
        int mapper(int);
        int rf_mapper(int);
        int position;
        bool triggered;
        void rotate();
    };
    
    class reflector {
    private:
        std::vector<int> map_from;
        std::vector<int> map_to;
    public:
        reflector(const char*);
        int mapper(int);
    };

private:
    int num_rotors;
    void turn_rotors();
    std::vector<rotor> vec_rotors;
    std::vector<int> vec_rot_posn;
    plugboard* pb = NULL;
    reflector* rf = NULL;
public:
    enigma(int, char**);
    char encryptor(char);
    
    static bool is_valid(const std::string&);
    static bool is_one_to_one(const std::vector<int>&, const std::vector<int>&);
};
