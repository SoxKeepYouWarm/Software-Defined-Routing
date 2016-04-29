#include "Util.h"

#include <string>
#include <sstream>

std::string toString(int val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}
