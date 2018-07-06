//
// Created by sefab on 8.04.2018.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

unsigned char *xorOperation(unsigned char *plain, unsigned char *key, int *pInt) {


    unsigned char *temp = (unsigned char *) malloc(*pInt * sizeof(unsigned char));
    int count = 0;
    unsigned char result = 0;
    for (int i = 0; i < *pInt; i = i + 128) {
        for (int j = 0; j < 128; ++j) {
            if (plain[count] == key[j])
                result = '0';
            if (plain[count] != key[j])
                result = '1';

            temp[count] = result;
            count++;
        }


    }

    return temp;

}