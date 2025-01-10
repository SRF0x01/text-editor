#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>



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


struct termios orig_termios;
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

char editorReadKey() {
    int nread;
    char c;
    char buffer[3]; 
    ssize_t n = read(STDIN_FILENO, buffer, 3);
    printf("%d ", n);
    
    
    //while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
      //  if (nread == -1 && errno != EAGAIN) die("read");
    //}
    return c;
}
/*** input ***/

void editorMoveCursor(char key) {
    switch (key) {
        case 'a':
            E.cx--;
            break;
        case 'd':
            E.cx++;
            break;
        case 'w':
            E.cy--;
            break;
        case 's':
            E.cy++;
            break;
    }
    printf("\033[%d;%dH", E.cy, E.cx); // ANSI escape code to move cursor
    fflush(stdout);
}

/*
Key	Escape Sequence
Up Arrow	\033[A
Down Arrow	\033[B
Right Arrow	\033[C
Left Arrow	\033[D

#define CTRL_KEY(k) ((k) & 0x1f)


void editorProcessKeypress() {
    char c = editorReadKey();
    switch (c) {
        case CTRL_KEY('x'):
        exit(0);
        break;
        case '\033[A':
        case '\033[B':
        case '\033[C':
        case '\033[D':
            editorMoveCursor(c);
            break;
    }
}
*/

void refreshScreen(){
    printf("\033[2J"); // clear the screen
}


int main() {
    enableRawMode();
    refreshScreen(); 
    while (1) {
        editorReadKey();
    }

    return 0;
}



/* Notes

crtl + x is code 24

*/