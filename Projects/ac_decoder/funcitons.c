#include "functions.h"

uint16_t correction(uint16_t diff)
{
    if (diff < 370 && diff > 320)
        diff = LOW_TIME;
    else if (diff < 455 && diff > 400)
        diff = HIGH_TIME_ZERO;
    else if (diff < 980 && diff > 1030)
        diff = HIGH_TIME_ONE;

    return diff;
}

uint8_t filling_binary(uint16_t diff)
{
    return
}
