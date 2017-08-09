#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NUMBER_OF_FILES         16
#define MULTIPLIER              1000
#define EDGE_CORRECTION         40
#define NUMBERS_IN_A_ROW        16
#define VOLT_DIFF_THRESHOLD     2.5
#define BUFFER_SIZE             50
#define NUMBER_OF_DATA          268




int main()
{
    FILE* source_files[NUMBER_OF_FILES];
    FILE* text_files[NUMBER_OF_FILES];

    char source[NUMBER_OF_FILES][10] = {"16.csv", "17.csv", "18.csv", "19.csv", "20.csv",
                                        "21.csv", "22.csv", "23.csv", "24.csv", "25.csv",
                                        "26.csv", "27.csv", "28.csv", "29.csv", "30.csv",
                                        "onoff.csv"};

    char texts[NUMBER_OF_FILES][10] = {"16.txt", "17.txt", "18.txt", "19.txt", "20.txt",
                                       "21.txt", "22.txt", "23.txt", "24.txt", "25.txt",
                                       "26.txt", "27.txt", "28.txt", "29.txt", "30.txt",
                                       "onoff.txt"};

    char string_buffer[BUFFER_SIZE];
    float time_1;
    float time_2;
    float voltage_1;
    float voltage_2;
    uint16_t difference;
    uint8_t new_line = 0;
    uint16_t data_counter;

// open the files
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        if ((source_files[i] = fopen(source[i], "r")) != NULL)
            printf("file %s opened\n", source[i]);
        if ((text_files[i] = fopen(texts[i], "w")) != NULL)
            printf("file %s opened\n", texts[i]);
    }

// text filling algorithm
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        data_counter = 0;
        if (i < 15)
            fprintf(text_files[i], "uint16_t degree_%d[] = {", i + NUMBER_OF_FILES);
        else
            fprintf(text_files[i], "uint16_t onoff[] = {");

        while (fgets(string_buffer, BUFFER_SIZE, source_files[i])) {

            time_1 = atof(strtok (string_buffer, ","));
            voltage_1 = atof(strtok (NULL, "\n"));

            if (time_1 > 0 ) {
                while (1) {
                    fgets(string_buffer, BUFFER_SIZE, source_files[i]);
                    time_2 = atof(strtok (string_buffer, ","));
                    voltage_2 = atof(strtok (NULL, "\n"));

                    if (abs(voltage_2 - voltage_1) > VOLT_DIFF_THRESHOLD) {
                        difference = (time_2 - time_1) * MULTIPLIER + EDGE_CORRECTION;

                        if (data_counter != NUMBER_OF_DATA - 1)
                            fprintf(text_files[i], "%d, ", difference);
                        else
                            fprintf(text_files[i], "%d};", difference);

                        new_line++;

                        if (new_line == NUMBERS_IN_A_ROW) {
                            fprintf(text_files[i], "\n");
                            new_line = 0;
                        }
                        data_counter++;
                        break;
                    }//if
                }//while
            }//if
        }//while
    }//for

// close the files
    for (int i = 0; i < NUMBER_OF_FILES; i++) {
        fclose(source_files[i]);
        fclose(text_files[i]);
    }

    return 0;
}
