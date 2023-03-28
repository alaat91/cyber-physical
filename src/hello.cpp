#include <iostream>
#include "PrimeChecker.hpp"

int main(int argc, char** argv){
    if(argc == 2){
        int number = std::stoi(argv[1]);
        PrimeChecker pc;
        std::cout << "2023-group-06; " << number << " is a prime number? " << pc.isPrime(number) << std::endl;
    } else {
        std::cout << "Usage: " << argv[0] << " <n>" << std::endl;
    }
}
