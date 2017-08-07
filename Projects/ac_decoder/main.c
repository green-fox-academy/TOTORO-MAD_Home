#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char pelda[15][7] = {"16.csv", "17.csv", "18.csv", "19.csv", "20.csv",
                     "21.csv", "22.csv", "23.csv", "24.csv", "25.csv",
                     "26.csv", "27.csv", "28.csv", "29.csv", "30.csv"};

FILE* files[15];

int main()
{
    int size = 30;
    int c;
    files[0] = fopen(pelda[0], "r");
    files[1] = fopen(pelda[1], "r");
    files[2] = fopen(pelda[2], "r");
    if (files[0] && files[1] && files[2]) {

        fclose(files[0]);
        fclose(files[1]);
        fclose(files[2]);
    }
    char str[] ="- This, a sample string.";
    char * pch;
    printf ("Splitting string \"%s\" into tokens:\n",str);
    pch = strtok (str," ,");
    while (pch != NULL) {
        printf ("%s\n",pch);
        pch = strtok (NULL, " ");
    }
    return 0;
}
