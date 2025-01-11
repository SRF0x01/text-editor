#include <iostream>
#include <fstream>

using namespace std;


void openFileContents(char* file_name){
    ifstream file(file_name);
    if (!file.is_open()) {            // Check if the file was opened successfully
        cerr << "Failed to open file.\n";
    }
    string line;
    while (std::getline(file, line)) { // Read line by line
        std::cout << line << '\n';    // Print each line
    }
    file.close();
}

int main(int argc, char* argv[]) {

    
    ifstream file(argv[1]); // file to open 

    if (!file.is_open()) {            // Check if the file was opened successfully
        cerr << "Failed to open file.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) { // Read line by line
        std::cout << line << '\n';    // Print each line
    }

    file.close(); // Close the file
    return 0;
}
