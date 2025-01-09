
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


struct termios orig_termios;
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // set normal terminal settings back
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios); // store curent terminal settings to struct
    atexit(disableRawMode); //disable raw mode at exit

    struct termios raw = orig_termios;

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
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // apply modified settings
}
int main() {
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) {
        printf("%d\r\n", c);
        } else {
        printf("%d ('%c')\r\n", c, c);
        }
    }
    return 0;
}



/* Notes

crtl + x is code 24

*/