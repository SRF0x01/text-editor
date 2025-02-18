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

            TextLine *new_line = new TextLine(line);
            current->next = new_line;
            new_line->prev = current;
            // cout << current->prev << " <- prev " << "current: " << current << " text: " << current->text_line << " next -> " << current->next << "\n";
            current = current->next;
        }
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
    string getText()
    {
        return text_line;
    }

    void setText(string line)
    {
        text_line = line;
    }

    void setChar(int pos, char c)
    {
        if (pos >= text_line.size())
        {
            text_line.push_back(c);
        }
        else
        {
            text_line.insert(text_line.begin() + pos, c);
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