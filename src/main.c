#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <limits.h>
#include <math.h>
#include <sys/wait.h>
#include "process-1.h"
#include "process-2.h"
#include "process-3.h"

int getDecimalFromBinary(unsigned char *result, int startIndex, int endIndex) {
    int count = 0;
    int digit = 7;
    for (int i = startIndex; i < endIndex; i++) {
        int digitAtIndex = result[i];
        if (digitAtIndex == 48) {
            digitAtIndex = 0;
        } else if (digitAtIndex == 49) {
            digitAtIndex = 1;
        }
        count = count + (digitAtIndex * pow(2, digit));
        digit--;
    }
    return count;
}

void decToBinary(unsigned char *bits, char *hex) {
    int val;
    char str[2000];
    strcpy(str, hex);
    val = atoi(str);
    int c, k;
    for (c = 7; c >= 0; c--) {
        k = val >> c;

        if (k & 1)
            bits[c] = 1;
        else
            bits[c] = 0;
    }

}

unsigned char *readKeyFile(char *fileName) {

    static unsigned char buf[128];
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *pch;
    int i, tmp = 0;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {

        pch = strtok(line, " -");
        while (pch != NULL) {
            unsigned char temp[8];
            decToBinary(temp, pch);
            for (i = tmp; i < sizeof(buf); i++) {
                for (int j = 7; j >= 0; j--) {
                    buf[tmp] = temp[j];
                    tmp++;
                }
                break;
            }
            pch = strtok(NULL, " -");
        }
    }


    fclose(fp);
    if (line)
        free(line);


    return buf;


}

unsigned char *readPlainFile(char fileName[33], int *pInt) {

    static unsigned char *str = NULL;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *pch;
    int i, tmp = 0;
    int size = 0;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        pch = strtok(line, " -\n");
        while (pch != NULL) {
            size += 8;
            str = (unsigned char *) realloc(str, size * sizeof(unsigned char));
            unsigned char temp[8];
            decToBinary(temp, pch);
            for (i = tmp; i < size; i++) {
                for (int j = 7; j >= 0; j--) {
                    str[tmp] = temp[j];
                    *pInt = *pInt + 1;
                    tmp++;
                }
                break;
            }
            pch = strtok(NULL, " -\n");
        }
    }

    fclose(fp);
    if (line)
        free(line);

    return str;

}

int main() {
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe
    int fd3[2];  // Used to store two ends of third pipe
    int fd4[2];  // Used to store two ends of fourth pipe
    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd3) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd4) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    int n1 = fork();
    int n2 = fork();


    if (n1 > 0 && n2 > 0) {
        //parent
        unsigned char *key;
        unsigned char *plain;
        int size = 0;
        key = readKeyFile("key.txt");
        plain = readPlainFile("plain.txt", &size);

        int count = 0;
        write(fd1[1], &size, sizeof(int));
        while (count < 2) {
            if (count == 0) {
                write(fd1[1], key, 128 * sizeof(unsigned char));
            } else {
                write(fd1[1], plain, size * sizeof(unsigned char));
            }
            count++;
        }
        unsigned char *final = malloc(size * sizeof(unsigned char));
        read(fd4[0], final, size * sizeof(unsigned char));

        int count1 = 0;

        FILE *f = fopen("encrypt.txt", "w");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        for (int i = 0; i < size; i = i + 8) {
            int a = getDecimalFromBinary(final, i, i + 8);
            fprintf(f, "%d", a);
            if (i + 8 != size)
                fprintf(f, "-");
            count1++;
            if (count1 != 0 && count1 % 16 == 0)
                fprintf(f, "\n");
        }
        fclose(f);
    } else if (n1 == 0 && n2 > 0) {
        //child 1
        int size = 0;
        int count = 0;
        read(fd1[0], &size, sizeof(int));
        unsigned char *key = malloc(128 * sizeof(unsigned char));
        unsigned char *plain = malloc(size * sizeof(unsigned char));
        unsigned char *result = malloc(size * sizeof(unsigned char));
        while (count < 2) {
            if (count == 0) {
                read(fd1[0], key, 128 * sizeof(unsigned char));
            } else {
                read(fd1[0], plain, size * sizeof(unsigned char));
            }
            count++;
        }

        result = xorOperation(plain, key, &size);
        int count1 = 0;
        FILE *f = fopen("process-1.txt", "w");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        for (int i = 0; i < size; i = i + 8) {
            int a = getDecimalFromBinary(result, i, i + 8);
            fprintf(f, "%d", a);
            if (i + 8 != size)
                fprintf(f, "-");
            count1++;
            if (count1 != 0 && count1 % 16 == 0)
                fprintf(f, "\n");
        }
        fclose(f);


        write(fd2[1], &size, sizeof(int));
        write(fd2[1], result, size * sizeof(unsigned char));

    } else if (n1 > 0 && n2 == 0) {

        int size = 0;
        read(fd2[0], &size, sizeof(int));
        unsigned char *result = malloc(size * sizeof(unsigned char));
        unsigned char *permutate = malloc(size * sizeof(unsigned char));
        read(fd2[0], result, size * sizeof(unsigned char));

        permutation(result, &size);


        int count1 = 0;

        FILE *f = fopen("process-2.txt", "w");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        for (int i = 0; i < size; i = i + 8) {
            int a = getDecimalFromBinary(result, i, i + 8);
            fprintf(f, "%d", a);
            if (i + 8 != size)
                fprintf(f, "-");
            count1++;
            if (count1 != 0 && count1 % 16 == 0)
                fprintf(f, "\n");
        }
        fclose(f);

        write(fd3[1], &size, sizeof(int));
        write(fd3[1], permutate, size * sizeof(unsigned char));


    } else if (n1 == 0 && n2 == 0) {

        int size = 0;
        read(fd3[0], &size, sizeof(int));
        unsigned char *permutate = malloc(size * sizeof(unsigned char));
        read(fd3[0], permutate, size * sizeof(unsigned char));
        subBox(permutate, size);
        int count1 = 0;

        FILE *f = fopen("process-3.txt", "w");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        for (int i = 0; i < size; i = i + 8) {
            int a = getDecimalFromBinary(permutate, i, i + 8);
            fprintf(f, "%d", a);
            if (i + 8 != size)
                fprintf(f, "-");
            count1++;

            if (count1 != 0 && count1 % 16 == 0)
                fprintf(f, "\n");
        }
        fclose(f);


        write(fd4[1], permutate, size * sizeof(unsigned char));
    }
}

