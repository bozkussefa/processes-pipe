//
// Created by sefab on 13.04.2018.
//

#include <stdio.h>
#include <malloc.h>


void permutation(unsigned char plain[], int *pInt) {


    unsigned char *temp = (unsigned char *) malloc(*pInt * sizeof(unsigned char));

    int count = 0;
    for (int i = 0; i < *pInt; i = i + 128) {
        for (int j = 0; j < 64; j++) {
            unsigned char temp = plain[i + j];
            plain[i + j] = plain[i + j + 64];
            plain[i + j + 64] = temp;

        }
    }


}