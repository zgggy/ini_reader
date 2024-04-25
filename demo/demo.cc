#include <string>
#include <vector>

#include "../ini_reader.hpp"

int main() {
    auto ini = IniReader{"./demo.ini"};
    auto v   = ini.GetVec<int>("test", "v");
    for (auto vv : v) std::cout << "v: " << vv << std::endl;
     v   = ini.GetVec<int>("test", "v2");
    for (auto vv : v) std::cout << "v: " << vv << std::endl;
     v   = ini.GetVec<int>("test", "v3");
    for (auto vv : v) std::cout << "v: " << vv << std::endl;
}