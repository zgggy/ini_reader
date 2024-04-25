#include <string>
#include <vector>

#include "../ini_reader.hpp"

int main() {
    auto ini = IniReader{"./demo.ini"};
    auto v   = ini.GetVec<std::string>("test", "v");
    for(auto vv:v)
    std::cout << "v: " << vv << std::endl;
}