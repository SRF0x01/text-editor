#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;

class TextLine
{
private:
    // private members
    string text_line;
    TextLine *next;
    TextLine *prev;

public:
    /* First line is the head of the list, the constructor only contains the next */
    // Constructor

    /* The head will always have the prev point to null*/
    TextLine(string line)
    {
        text_line = line;
        next = nullptr;
        prev = nullptr;
    }

    TextLine()
    {
        next = nullptr;
        prev = nullptr;
    }

    string getText()
    {
        return text_line;
    }

    void setNext(string new_line)
    {
        TextLine *new_obj = new TextLine(new_line);

        // if at the tail
        if (next == nullptr)
        {

            next = new_obj;
            new_obj->prev = this;
        }
    }

    void toTextLine(string file_name)
    {
        // Turns the object this method is calling into the head
        ifstream file(file_name);
        if (!file.is_open())
        { // Check if the file was opened successfully
            cerr << "Failed to open file.\n";
        }
        string line;
        TextLine *node = this;
        while (std::getline(file, line))
        { // Read line by line
            //printf("%s\r\n",line.c_str());
            this->text_line = line;
            node->setNext(line);
        }

        file.close();
    }

    TextLine *getNext()
    {
        return next;
    }

    ~TextLine()
    {
        delete next; // Recursively delete the next node in the linked list
    }
};

/*
Example class


#include <iostream>
using namespace std;

// Class Definition
class MyClass {
private:
    // Private data members (only accessible within the class)
    int privateData;

public:
    // Constructor (runs when an object is created)
    MyClass(int value) {
        privateData = value;
        cout << "Constructor called: privateData set to " << privateData << endl;
    }

    // Public method (can be accessed outside the class)
    void showData() {
        cout << "Private Data: " << privateData << endl;
    }

    // Setter (modifies private data)
    void setData(int value) {
        privateData = value;
    }

    // Getter (returns private data)
    int getData() {
        return privateData;
    }

    // Destructor (runs when an object is destroyed)
    ~MyClass() {
        cout << "Destructor called for privateData = " << privateData << endl;
    }
};

int main() {
    // Creating an object of MyClass
    MyClass obj(10);

    // Accessing public methods
    obj.showData();

    // Modifying private data using setter
    obj.setData(20);
    cout << "Updated Data: " << obj.getData() << endl;

    // Object goes out of scope here, so destructor is called
    return 0;
}
*/