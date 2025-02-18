#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "TextLine.cpp"

using namespace std;

// Prototypes
void afterCurrentPrint(TextLine *current);

/** Global vars **/
// tooling to get the size of the terminal
struct editorConfig
{
    int cx, cy;
    struct termios orig_termios;
};
struct editorConfig E; // global variable containing the state of the terminal

// global variable that sets the top left of a page
const int TOP_LEFT_X = 1;
const int TOP_LEFT_Y = 3;

// unfinished

void deleteChar(int position)
{
}

void appendLine(TextLine *current)
{
    current->setNext("");
    afterCurrentPrint(current);
}

void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear the screen with J
    write(STDOUT_FILENO, "\x1b[H", 3);  // reposition the cursor with H
    // perror gives a description of the error
    perror(s);
    exit(1);
}

void disableRawMode()
{
    // tcgetattr sets the terminal to the original settints
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    {
        die("tcsetattr");
    }
}

void enableRawMode()
{
    /* Enabling raw mode will disable much of the terminal functions look at the flags below*/
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
    atexit(disableRawMode); // disable raw mode at exit
    struct termios raw = E.orig_termios;

    /*
    ECHO turning off echo mode disables terminal printing
    ICANON turning off canocial mode
    ISIG turns off ctrl + x and ctrl + z
    IXON turns off ctrl + x and ctrl + q
    IEXTEN turns off ctrl + v
    ICRNL turns off ctrl + m
    OPOST turns off automoatic newline and carriage return requiring manual \r and \n
    */
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

void moveCursor(char key, TextLine *&current)
{
    switch (key)
    {
    case 'A': // Up arrow
        if (current->getPrev())
        {
            current = current->getPrev();
            E.cy--;
        }
        break;
    case 'B': // Down arrow
        if (current->getNext())
        {
            E.cy++;
            current = current->getNext();
        }
        break;
    case 'C': // Right arrow
        if (E.cx < current->getText().length())
        {
            E.cx++;
        }
        break;
    case 'D': // Left arrow
        if (E.cx > 0)
        {
            E.cx--;
        }
        break;
    }
    // Sending escape sequence to terminal to move cursor
    int display_x = E.cx + TOP_LEFT_X;
    int display_y = E.cy + TOP_LEFT_Y;
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", display_y, display_x); // Create the escape sequence
    write(STDOUT_FILENO, buffer, length);
}

void printKeys(char k)
{
    printf("%d\r\n", k);
    fflush(stdout);
}

void printEscapeKeys(char k)
{
    printf("Escape key %d\r\n", k);
    fflush(stdout);

    /*Notes:
    up arrow is 65
    down arrow is 66
    left arrow is 68
    rigth arrow is 67

    backspace is int 127*/
}

char readKey(TextLine *&current)
{
    char buffer[3];                                         // To handle multi-byte escape sequences
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer)); // Read up to 3 bytes

    if (n == -1 && errno != EAGAIN)
    {
        die("read");
    }
    // Regular character
    if (n == 1)
    {
        // Insert new line
        if (buffer[0] == '\r')
        {
            E.cx = 0;
            // create a new line
            current->setNext("");
            afterCurrentPrint(current);
            moveCursor('B', current);
        }
        if (buffer[0] == 127)
        {
            char space = 32;
            write(STDOUT_FILENO, &space, 1);
            current->setChar(E.cx, ' ');
            moveCursor('D', current);
        }
        else
        {
            write(STDOUT_FILENO, &buffer[0], 1);
            current->setChar(E.cx, buffer[0]);

            // Even in raw mode the cursor will move one over for write, add one to the E.cx to reflect this
            E.cx++;
        }

        return buffer[0];
    }
    // Multi-byte escape sequence (e.g., arrow keys)
    else if (n == 3 && buffer[0] == '\033' && buffer[1] == '[')
    {
        moveCursor(buffer[2], current);
        return buffer[2];
    }

    // Default return if unknown input
    return '\0';
}

void titleCard()
{
    printf("------- S.T.E 1.0.0-alpha (ctrl + x to exit) -------\r\n");
    fflush(stdout);
}

void refreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);   // clear the screen with J
    write(STDOUT_FILENO, "\x1b[2;1H", 7); // set cursor at the top left one down
}

void clearScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4); // 2J clears the entire screen
}

void resetCursor()
{
    E.cx = 0;
    E.cy = 0;
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", TOP_LEFT_Y, TOP_LEFT_X);
    write(STDOUT_FILENO, buffer, length);
}

// TextLine obj printing
void printAll(TextLine *head)
{
    TextLine *current = head;
    while (current)
    {
        cout << current->getText() << '\r' << '\n'; // Print each line
        current = current->getNext();
    }

    // No need to delete head because it ends as a null pointer
}

void afterCurrentPrint(TextLine *current)
{
    TextLine *walker = current;
    while (walker)
    {
        std::cout << "\033[2K";                    // Clear the entire line and move the cursor to the beginning
        cout << walker->getText() << '\r' << '\n'; // Print each line
        walker = walker->getNext();
    }
}

void debugPrint(string prt)
{
}

int main(int argc, char *argv[])
{

    /*** File to TextLine head ***/
    ifstream file(argv[1]); // Open file
    if (!file.is_open())
    {
        cerr << "Failed to open file.\n";
        return 1; // Exit if file cannot be opened
    }
    // Create the head of the linked list
    TextLine *head = new TextLine(file);
    file.close(); // Close the file after reading

    /*** Setting terminal to raw mode ***/
    enableRawMode();
    refreshScreen();
    titleCard();

    printAll(head);

    resetCursor();

    TextLine *current_line = head;

    while (1) // 24 is the code for ctrl + x1
    {
        char c = readKey(current_line);

        if (c == 24)
            break;
    }

    refreshScreen();
    disableRawMode();

    printAll(head);

    delete head;
    return 0;
}

/*Notes:
the top left cursor limit with a single line of text is x2 y3*/