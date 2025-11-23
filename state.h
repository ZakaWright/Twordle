#ifndef STATE_H

#define STATE_H

struct Guess {
    char guess[7];
    //int * result;
    int result[5];
};

struct Key {
    char key;
    int status;
};

struct State{
    char word[6];
    struct Guess guess_1;
    struct Guess guess_2;
    struct Guess guess_3;
    struct Guess guess_4;
    struct Guess guess_5;
    int turn;
    struct Key keyboard[26];

    // keyboard and game starting points
    int start_key_row;
    int start_key_col;
    int start_words_row;
    int start_words_col;
};

int add_guess(struct State* state, char guess[7]);

#endif
