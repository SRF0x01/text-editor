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

/** Global vars **/
// tooling to get the size of the terminal
struct editorConfig
{
    int cx, cy;
    struct termios orig_termios;
};
struct editorConfig E; // global variable containing the state of the terminal

// global variable that sets the top left of a page
const int TOP_LEFT_X = 0;
const int TOP_LEFT_Y = 3;

// unfinished

void insertChar(int position, char c)
{
}

void deleteChar(int position)
{
}

void appendChar(char c)
{
}

void appendString(const string &str)
{
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

/*** input ***/
/*
void editorMoveCursor(char key)
{
    switch (key)
    {
    case 'A': // Up arrow
        if (E.cy > 0)
        {
            E.cy--;
        }
        if (string_vec[E.cy].size() < E.cx)
        {
            E.cx = string_vec[E.cy].size();
        }
        break;
    case 'B': // Down arrow
        if (E.cy < string_vec.size())
        {
            E.cy++;
        }
        if (string_vec[E.cy].size() < E.cx)
        {
            E.cx = string_vec[E.cy].size();
        }
        break;
    case 'C': // Right arrow
        if (E.cx < string_vec[E.cy].size())
        {
            E.cx++;
        }
        else if (E.cx == string_vec[E.cy].size() && E.cy + 1 <= string_vec.size())
        {
            E.cx = 0;
            E.cy++;
        }
        break;
    case 'D': // Left arrow
        if (E.cx > 0)
        {
            E.cx--;
        }
        else if (E.cx == 0 && E.cy - 1 >= 0)
        {
            E.cy--;
            E.cx = string_vec[E.cy].size();
        }
        break;
    }

    // add the topleft limits to the display not the actual limits of the verctors

    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", E.cy + TOP_LEFT_Y, E.cx + TOP_LEFT_X); // Create the escape sequence
    write(STDOUT_FILENO, buffer, length);                                                               // Write the escape sequence to stdout

    // for dev
    cursorLineation();
}
*/

void moveCursor(char key)
{
    switch (key)
    {
    case 'A': // Up arrow
        if (E.cy > 0)
        {
            E.cy--;
        }
        break;
    case 'B': // Down arrow
        E.cy++;
        break;
    case 'C': // Right arrow
        E.cx++;
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

char readKey()
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
        if (buffer[0] == '\r')
        {
            write(STDOUT_FILENO, "\r\n", 2); // Ensure newline properly moves down
            E.cx = 0;
            E.cy++;

            // Move the cursor to the correct new line position
            char buffer[32];
            int length = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", E.cy + TOP_LEFT_Y, TOP_LEFT_X);
            write(STDOUT_FILENO, buffer, length);
        }
        if (buffer[0] == 127)
        {
            char space = 32;
            write(STDOUT_FILENO, &space, 1);
            moveCursor('D');
        }
        else
        {
            write(STDOUT_FILENO, &buffer[0], 1);

            // Even in raw mode the cursor will move one over for write, add one to the E.cx to reflect this
            E.cx++;
        }
        return buffer[0];
    }
    // Multi-byte escape sequence (e.g., arrow keys)
    else if (n == 3 && buffer[0] == '\033' && buffer[1] == '[')
    {
        moveCursor(buffer[2]);
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
    int count = 1;
    TextLine *current = head;
    while (current)
    {
        cout << current->getText() << '\r' << '\n'; // Print each line
        current = current->getNext();
    }
}

void debugPrint(string prt)
{
}

int main(int argc, char *argv[])
{
    enableRawMode();
    refreshScreen();
    titleCard();

    ifstream file(argv[1]); // Open file
    if (!file.is_open())
    {
        cerr << "Failed to open file.\n";
        return 1; // Exit if file cannot be opened
    }

    // Create the head of the linked list
    TextLine *head = new TextLine(file);
    file.close(); // Close the file after reading

    printAll(head);

    resetCursor();

    while (1) // 24 is the code for ctrl + x1
    {
        char c = readKey();

        if (c == 24)
            break;
    }

    delete head;

    refreshScreen();
    disableRawMode();

    return 0;
}

/*Notes:
the top left cursor limit with a single line of text is x2 y3*/