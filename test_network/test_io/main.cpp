
#include <fstream>
#include <iostream>

#include <sys/stat.h>

int main() {

    std::ifstream input;
    input.open("test_file.bin", std::ios::binary);
    
    struct stat results;
    stat("test_file.bin", &results);

    int size = results.st_size;

    int buffer_size = 16;
    char line[buffer_size];
    
    if (input.is_open()) {
    
        while (input.read(line, buffer_size)) {
        
            std::cout << line << std::endl;
            std::ofstream output;
            output.open("copy.bin", std::ios::binary);
            output.write(line, buffer_size);      
        }
        
        input.close();
    
    }
    
    return 0;

}
