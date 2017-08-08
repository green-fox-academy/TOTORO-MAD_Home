#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char pelda[16][7] = {"16.csv", "17.csv", "18.csv", "19.csv", "20.csv",
                     "21.csv", "22.csv", "23.csv", "24.csv", "25.csv",
                     "26.csv", "27.csv", "28.csv", "29.csv", "30.csv",
                     "onoff.csv"};

FILE* files[15];

int main()
{
    int ups_and_downs[320];
    char string[50];
    float time_1;
    float time_2;
    float voltage_1;
    float voltage_2;
    uint8_t voltage_status = 0;
    uint16_t j = 0;
// opens the files
    for (int i = 0; i < 3; i++) {
        if ((files[i] = fopen(pelda[i], "r")) != NULL)
            printf("file %s opened\n", pelda[i]);
    }
// array filling algorithm
    for (int i = 0; i < 3; i++) {
        printf("\n\n\n\n\n\n\n\n");
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
                        ups_and_downs[j] = (time_2 - time_1) * 1000;
                        printf("%d\n", ups_and_downs[j]);
                        j++;

                        break;
                    }
                }//while
            }//if
        }//while
    }//for

// closes the files
    for (int i = 0; i < 3; i++) {
        fclose(files[i]);
    }
    return 0;
}
