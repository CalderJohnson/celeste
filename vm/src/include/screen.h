#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stdbool.h>

bool init_screen (); //initializes the screen
void draw (uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b); //draw a pixel
void update (); //write to the display buffer
void clear (); //clear the display buffer

#endif