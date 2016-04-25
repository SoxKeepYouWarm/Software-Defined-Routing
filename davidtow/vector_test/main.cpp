
#include <stdio.h>
#include <vector>


int main() {

    std::vector< std::vector<int> > matrix(10, std::vector<int>(10, -1));
    matrix.at(0).at(0) = 5;
    return 0;

}
