
#define INFINITY 0xFFFF

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main() {

    unsigned short inf = INFINITY;

    char buffer [20];
    itoa(inf, buffer, 10);

    std::cout << "inf is: " << buffer << std::endl;
    return 0;

}
