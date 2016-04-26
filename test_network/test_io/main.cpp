
#include <fstream>
#include <iostream>

#include <sys/stat.h>

int main() {

    std::ifstream input;
    input.open("test.txt", std::ios::binary);
    
    struct stat results;
    int size = stat("test.txt", &results);

    std::string line;
    if (input.is_open()) {
    
        while ( getline (input, line) ) {
        
            std::cout << line << std::endl;
        
        }
        input.close();
    
    }
    
    return 0;

}
