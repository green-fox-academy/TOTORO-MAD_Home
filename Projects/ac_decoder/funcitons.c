#include "functions.h"

uint16_t correction(uint16_t diff)
{
    if (diff < 370 && diff > 320)
        return LOW_TIME;
    else if (diff < 455 && diff > 400)
        return HIGH_TIME_ZERO;
    else if (diff < 980 && diff > 1030)
        return HIGH_TIME_ONE;
    else
        return diff;
}

uint8_t filling_binary(uint16_t diff)
{
    if (diff == HIGH_TIME_ZERO)
        return 0;
    else if (diff == HIGH_TIME_ONE)
        return 1;
    else
        return 'X';
}
