#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <iostream>


std::vector<char> text;

void insertChar(int position, char c) {
    text.insert(text.begin() + position, c);
}

void deleteChar(int position) {
    text.erase(text.begin() + position);
}

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)


/*** data ***/

// tooling to get the size of the terminal
struct editorConfig {
    int cx, cy;
    int screenrows;
    int screencols;
    struct termios orig_termios;
};
struct editorConfig E; // global variable containing the state of the terminal

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear the screen with J
    write(STDOUT_FILENO, "\x1b[H", 3); // reposition the cursor with H
    // perror gives a description of the error
    perror(s);
    exit(1);
}


void disableRawMode() {
    // tcgetattr sets the terminal to the original settints
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1){
        die("tcsetattr");
    }
}

void enableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("tcsetattr");
    atexit(disableRawMode); //disable raw mode at exit
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
    raw.c_iflag &= ~(ICRNL |IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}


/*** input ***/

void editorMoveCursor(char key) {
    switch (key) {
        case 'D':
            E.cx--;
            break;
        case 'C':
            E.cx++;
            break;
        case 'B':
            E.cy--;
            break;
        case 'A':
            E.cy++;
            break;
    }
    printf("\033[%d;%dH", E.cy, E.cx); // ANSI escape code to move cursor
    fflush(stdout);
}


void printKeys(char k){
    printf("%d\r\n",k);
    fflush(stdout);
}

void printEscapeKeys(char k){
    printf("Escape key %d\r\n",k);
    fflush(stdout);

    /*Notes:
    up arrow is 65
    down arrow is 66
    left arrow is 68
    rigth arrow is 67

    backspace is int 127*/
}

char editorReadKey() {
    char buffer[3];  // To handle multi-byte escape sequences
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));  // Read up to 3 bytes

    if (n == -1 && errno != EAGAIN) {
        die("read");
    }

    if (n == 1) {
        // Single-byte input (e.g., regular keypress like 'a', 'b', etc.)
        char c = buffer[0];
        printKeys(c);
        
        //editorMoveCursor('C');
        return buffer[0];
    } else if (n == 3 && buffer[0] == '\033' && buffer[1] == '[') {
        // Multi-byte escape sequence (e.g., arrow keys)
        int k = buffer[2];
        
        printEscapeKeys(buffer[2]);
        //editorMoveCursor(buffer[2]);
        return buffer[2];
        
        
    }

    // Default return if unknown input
    return '\0';
}


void refreshScreen(){
    printf("\033[2J"); // clear the screen
}


int main() {
    enableRawMode();
    refreshScreen(); 
    while (1) // 24 is the code for cntrl + x1
    {
        char c = editorReadKey();
        if(c == 24) break;
    }
    disableRawMode();
    return 0;
}



/* Notes

crtl + x is code 24

*/