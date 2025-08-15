#include <fstream>
#include <iostream>

int main(){
    std::fstream file("log.txt");

    file << "This is a test log entry." << std::endl;
    file << "This is a test log entry." << std::endl;

    file.close();
    return 0;
}