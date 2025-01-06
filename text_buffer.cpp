#include <iostream>
#include <string>
#include <fstream>


int main(int argc, char *argv[]){
  std::string text;
  std::string line;

  std::cout << argv[1];
  std::cout << "--- Enter your text --- use crtl + d to exit";
  std::cout << "";


  while(std::getline(std::cin,line)){
    text += line + "\n";
  }

  std::cout << "\nYou Entered:\n" << text << std::endl;

  return 0;
}
