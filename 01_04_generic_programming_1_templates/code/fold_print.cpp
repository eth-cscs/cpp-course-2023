#include <iostream>

template<typename... Ts>
void print1(Ts... vals)
{
    (std::cout << ... << vals);
}

template<typename... Ts>
void print2(const char *delim, Ts... vals)
{
    auto showdelim = [](const char *delim, const auto& param) -> const auto& {
      std::cout << delim;
      return param;
    };

    (std::cout << ... << showdelim(delim, vals) ) << std::endl ;
}

void print3(const char *delim) {}

template<typename T, typename... Ts>
void print3(const char *delim, T first, Ts... vals)
{
    auto showdelim = [](const char *delim, const auto& param) -> const auto& {
      std::cout << delim;
      return param;
    };
    std::cout << first;
    (std::cout << ... << showdelim(delim, vals) ) << std::endl ;
}

int main()
{
    std::cout << std::endl << "method 1:" << std::endl;
    print1(", ", 1, "charlike", 3.14, std::string("stringlike"), static_cast<char>(65), 6);
    std::cout << std::endl << "method 2:" << std::endl;
    print2(", ", 1, "charlike", 3.14, std::string("stringlike"), static_cast<char>(65), 6);
    std::cout << std::endl << "method 3:" << std::endl; 
    print3(", ", 1, "charlike", 3.14, std::string("stringlike"), static_cast<char>(65), 6);

    std::cout << std::endl << "method 1:" << std::endl;
    print1(", ");
    std::cout << std::endl << "method 2:" << std::endl;
    print2(", ");
    std::cout << std::endl << "method 3:" << std::endl; 
    print3(", ");
}
