#include <iostream>
#include <string>


int main(){
  std::string text;
  std::string line;

  std::cout << "Enter your text";

  while(std::getline(std::cin,line)){
    text += line + "\n";
  }

  std::cout << "\nYou Entered:\n" << text << std::endl;

  return 0;
}
