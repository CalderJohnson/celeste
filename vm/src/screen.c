#include <SFML/Graphics.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static const sfVideoMode mode = { //video settings
    .width = 256,
    .height = 256,
};

static const sfVector2f pixel_size = { //pixel size
    .x = 1,
    .y = 1,
};

static sfRenderWindow* window; //window

bool init_screen () { //initializes the screen
    window = sfRenderWindow_create(mode, "Celeste", sfResize | sfClose, NULL);
    if (window == NULL) return false;
    return true;
}

void draw (uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) { //draw a pixel
    sfRectangleShape* shape = sfRectangleShape_create();
    sfColor color = sfColor_fromRGB(r, g, b);
    sfVector2f position = {
        .x = x,
        .y = y,
    };
    sfRectangleShape_setSize(shape, pixel_size);
    sfRectangleShape_setFillColor(shape, color);
    sfRectangleShape_setPosition(shape, position);
    sfRenderWindow_drawRectangleShape(window, shape, NULL);
}

void update () { //write to the display buffer
    sfRenderWindow_display(window);
}

void clear () { //clear the display buffer
    sfRenderWindow_clear(window, sfBlack);
}
