
#include <fstream>
#include <iostream>

#include <sys/stat.h>

int main() {

    std::ifstream input;
    input.open("sample.txt", std::ios::binary);
    
    struct stat results;
    stat("test.txt", &results);

    int size = results.st_size;

    int buffer_size = 10;
    char line[buffer_size];
    
    if (input.is_open()) {
    
        while (input.read(line, buffer_size)) {
        
            std::cout << line << std::endl;
        
        }
        
        input.close();
    
    }
    
    return 0;

}
