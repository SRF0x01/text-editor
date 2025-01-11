#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <unistd.h>


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
        case 'A': // Up arrow
            if (E.cy > 0) E.cy--;
            break;
        case 'B': // Down arrow
            E.cy++;
            break;
        case 'C': // Right arrow
            E.cx++;
            break;
        case 'D': // Left arrow
            if (E.cx > 0) E.cx--;
            break;
    }
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", E.cy + 0, E.cx + 0); // Create the escape sequence
    write(STDOUT_FILENO, buffer, length); // Write the escape sequence to stdout
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

enum escapeKey {
    UP_ARROW = 65,
    RIGHT_ARROW = 67,
    DOWN_ARROW = 66,
    LEFT_ARROW = 68,
    BACKSPACE = 127
};

char editorReadKey() {
    char buffer[3];  // To handle multi-byte escape sequences
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));  // Read up to 3 bytes

    if (n == -1 && errno != EAGAIN) {
        die("read");
    }

    if (n == 1) {
        // This is delete
        if(buffer[0] == '\r'){
            E.cx = 0;
            editorMoveCursor('B');
        }
        if(buffer[0] == 127){
            // 
            char space = ' ';
            // TODO: fix the issue where a delete requires two keystrokes
            write(STDOUT_FILENO, &space, 1);
            editorMoveCursor('D');
        } else {
            write(STDOUT_FILENO, &buffer[0], 1);
            editorMoveCursor('C');

        }
        return buffer[0];
    } else if (n == 3 && buffer[0] == '\033' && buffer[1] == '[') {
        // Multi-byte escape sequence (e.g., arrow keys)
        //printEscapeKeys(buffer[2]);
        editorMoveCursor(buffer[2]);
        return buffer[2];
    }

    // Default return if unknown input
    return '\0';
}

void titleCard(){
    E.cy = 3;
    printf("--- Text Editor (ctrl + x to exit) %d %d ---\r\n", E.cx, E.cy);
    fflush(stdout);
}

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear the screen with J
    write(STDOUT_FILENO, "\x1b[2;1H", 7); // set cursor at the top left one down
}

int main() {
    enableRawMode();
    refreshScreen();
    titleCard(); 
    while (1) // 24 is the code for ctrl + x1
    {
        char c = editorReadKey();
        if(c == 24) break;
    }
    disableRawMode();
    return 0;
}

