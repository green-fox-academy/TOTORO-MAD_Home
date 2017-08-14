#include "functions.h"

uint16_t correction(uint16_t diff)
{
    if (diff < 370 && diff > 320)
        return LOW_TIME;
    else if (diff < 455 && diff > 400)
        return HIGH_TIME_ZERO;
    else if (diff > 980 && diff < 1030)
        return HIGH_TIME_ONE;
    else
        return diff;
}

char filling_binary(uint16_t diff)
{
    if (diff < 500 && diff > 300)
        return '0';
    else if (diff > 900 && diff < 1100)
        return '1';
    else
        return 'X';
}
