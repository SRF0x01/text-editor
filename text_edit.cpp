#include <iostream>
#include <string>
#include <fstream>


int main(int argc, char *argv[]){

    // Open a file via the standard input arguments

    std::ifstream file(argv[1]);
    if(!file){ // Check if file was opened successfully
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    // print file contents
    std::cout << "--- Text Edit --- ctrl + d to exit\n";
    std::string line;
    std::string text;
    while (std::getline(file, line)){
        //std::cout << line << std::endl;
        text += line + "\n";
    }

    
    // append more text to the string
    while(std::getline(std::cin,line)){
        text += line + "\n";
    }

    file.close();
    return 0;
}
