#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NUMBER_OF_FILES 16
#define MULTIPLIER 1000
#define EDGE_CORRECTION 40

FILE* files[NUMBER_OF_FILES];

char pelda[NUMBER_OF_FILES][10] = {"16.csv", "17.csv", "18.csv", "19.csv", "20.csv",
                                  "21.csv", "22.csv", "23.csv", "24.csv", "25.csv",
                                  "26.csv", "27.csv", "28.csv", "29.csv", "30.csv",
                                  "onoff.csv"};

int ups_and_downs[NUMBER_OF_FILES][320];

int main()
{
    char string[50];
    float time_1;
    float time_2;
    float voltage_1;
    float voltage_2;
    uint8_t voltage_status = 0;
    uint16_t j = 0;
// opens the files
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        if ((files[i] = fopen(pelda[i], "r")) != NULL)
            printf("file %s opened\n", pelda[i]);
    }
// array filling algorithm
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        printf("\n\n\n\n%s\n\n", pelda[i]);
        j = 0;
        while (fgets(string, 50, files[i])) {
            time_1 = atof(strtok (string, ","));
            voltage_1 = atof(strtok (NULL, "\n"));
                //printf("time: %f\t\t voltage: %f\n", time_1, voltage_1);
            if (time_1 > 0 ) {
                while (1) {
                    fgets(string, 50, files[i]);
                    time_2 = atof(strtok (string, ","));
                    voltage_2 = atof(strtok (NULL, "\n"));
                    if (abs(voltage_2 - voltage_1) > 2.5) {
                        voltage_status = !voltage_status;
                        ups_and_downs[i][j] = (time_2 - time_1) * MULTIPLIER + EDGE_CORRECTION;
                        printf("%d\n", ups_and_downs[i][j]);
                        j++;
                        break;
                    }//if
                }//while
            }//if
        }//while
    }//for

// closes the files
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        fclose(files[i]);
    }
    return 0;
}
