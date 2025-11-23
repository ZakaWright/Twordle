#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void getWord(char* word, int buff_size) {
    FILE *fptr = fopen("wordlist.txt", "r");
    if (fptr == NULL) {
        perror("Erorr opening file\n");
    }

    // get file size
    if (fseek(fptr, 0, SEEK_END)) {
        perror("Error seeking file\n");
        fclose(fptr);
        return;
    }
    long size = ftell(fptr);

    // reset pointer
    rewind(fptr);

    // choose random word
    long word_options = size / 6;

    srand(time(NULL));
    int word_index = rand() % word_options;

    // get word
    if (fseek(fptr, (word_index * 6), SEEK_CUR)) {
        perror("Error seeking file\n");
        fclose(fptr);
        return;
    }
    char buff[6] = {0};
    fgets(buff, 6, fptr);
    // make capitalized
    for (int i = 0; i < 5; i++) {
        buff[i] = toupper(buff[i]);
    }
    strncpy(word, buff, 5);
    word[buff_size -1] = '\0';

    fclose(fptr);
    return;
}


