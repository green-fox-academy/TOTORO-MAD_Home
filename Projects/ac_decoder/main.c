#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char pelda[16][7] = {"16.csv", "17.csv", "18.csv", "19.csv", "20.csv",
                     "21.csv", "22.csv", "23.csv", "24.csv", "25.csv",
                     "26.csv", "27.csv", "28.csv", "29.csv", "30.csv",
                     "onoff.csv"};

FILE* files[15];

int main()
{
    int size = 30;
    char string[50];
    char *time;
    char *voltage;
    float time_1;
    float time_2;
    float voltage_1;
    float voltage_2;
    for (int i = 0; i < 3; i++) {
        if ((files[i] = fopen(pelda[i], "r")) != NULL)
            printf("file %s opened\n", pelda[i]);
    }
    for (int i = 0; i < 3; i++) {
        while (fgets(string, 50, (FILE*)files[i])) {
            time = strtok (string, ",");
            time_1 = atof(time);
            voltage = strtok (NULL, "\n");
            voltage_1 = atof(voltage);
            printf("time: %f\t\t voltage: %f\n", time_1, voltage_1);
        }
    }
    for (int i = 0; i < 3; i++) {
        fclose(files[i]);
    }
    return 0;
}
