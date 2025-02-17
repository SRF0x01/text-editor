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

    TextLine(ifstream &file)
    {
        string line;
        if (!file.is_open())
        {
            cerr << "Failed to open file.\n";
            return;
        }

        // Read first line into this head node
        if (getline(file, line))
        {
            this->text_line = line;
            this->next = nullptr;
            this->prev = nullptr;
        }
        else
        {
            return; // Empty file case
        }

        TextLine *current = this; // Keep track of the current node

        // Read and create new nodes for remaining lines
        while (getline(file, line))
        {
            current->next = new TextLine(line);
            current->next->prev = current;
            current = current->next; // Move current forward
        }
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
        else
        {
            new_obj->prev = this;       // New node’s previous points to current node
            new_obj->next = this->next; // New node’s next points to what this->next was

            this->next->prev = new_obj; // Update next node's prev pointer
            this->next = new_obj;       // Actually insert the new node in the list
        }
    }

    TextLine *getNext()
    {
        return next;
    }

    TextLine *getPrev()
    {
        return prev;
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