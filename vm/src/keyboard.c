#include <SFML/Window/Keyboard.h>
#include <stdbool.h>

/* returns the currently pressed key */
char get_key() {
    /* letters */
    if     (sfKeyboard_isKeyPressed(sfKeyA))     return 'a';
    else if(sfKeyboard_isKeyPressed(sfKeyB))     return 'b';
    else if(sfKeyboard_isKeyPressed(sfKeyC))     return 'c';
    else if(sfKeyboard_isKeyPressed(sfKeyD))     return 'd';
    else if(sfKeyboard_isKeyPressed(sfKeyE))     return 'e';
    else if(sfKeyboard_isKeyPressed(sfKeyF))     return 'f';
    else if(sfKeyboard_isKeyPressed(sfKeyG))     return 'g';
    else if(sfKeyboard_isKeyPressed(sfKeyH))     return 'h';
    else if(sfKeyboard_isKeyPressed(sfKeyI))     return 'i';
    else if(sfKeyboard_isKeyPressed(sfKeyJ))     return 'j';
    else if(sfKeyboard_isKeyPressed(sfKeyK))     return 'k';
    else if(sfKeyboard_isKeyPressed(sfKeyL))     return 'l';
    else if(sfKeyboard_isKeyPressed(sfKeyM))     return 'm';
    else if(sfKeyboard_isKeyPressed(sfKeyN))     return 'n';
    else if(sfKeyboard_isKeyPressed(sfKeyO))     return 'o';
    else if(sfKeyboard_isKeyPressed(sfKeyP))     return 'p';
    else if(sfKeyboard_isKeyPressed(sfKeyQ))     return 'q';
    else if(sfKeyboard_isKeyPressed(sfKeyR))     return 'r';
    else if(sfKeyboard_isKeyPressed(sfKeyS))     return 's';
    else if(sfKeyboard_isKeyPressed(sfKeyT))     return 't';
    else if(sfKeyboard_isKeyPressed(sfKeyU))     return 'u';
    else if(sfKeyboard_isKeyPressed(sfKeyV))     return 'v';
    else if(sfKeyboard_isKeyPressed(sfKeyW))     return 'w';
    else if(sfKeyboard_isKeyPressed(sfKeyX))     return 'x';
    else if(sfKeyboard_isKeyPressed(sfKeyY))     return 'y';
    else if(sfKeyboard_isKeyPressed(sfKeyZ))     return 'z';
    /* numbers */
    else if(sfKeyboard_isKeyPressed(sfKeyNum1))  return '1';
    else if(sfKeyboard_isKeyPressed(sfKeyNum2))  return '2';
    else if(sfKeyboard_isKeyPressed(sfKeyNum3))  return '3';
    else if(sfKeyboard_isKeyPressed(sfKeyNum4))  return '4';
    else if(sfKeyboard_isKeyPressed(sfKeyNum5))  return '5';
    else if(sfKeyboard_isKeyPressed(sfKeyNum6))  return '6';
    else if(sfKeyboard_isKeyPressed(sfKeyNum7))  return '7';
    else if(sfKeyboard_isKeyPressed(sfKeyNum8))  return '8';
    else if(sfKeyboard_isKeyPressed(sfKeyNum9))  return '9';
    else if(sfKeyboard_isKeyPressed(sfKeyNum0))  return '0';
    /* misc */
    else if(sfKeyboard_isKeyPressed(sfKeySpace)) return ' ';
    else return '\0';
}
