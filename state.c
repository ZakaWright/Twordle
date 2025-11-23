#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"

int add_guess(struct State* state, char guess[7]) {

    switch(state->turn) {
        case 0:
            strncpy(state->guess_1.guess, guess, 5);
        case 1:
            strncpy(state->guess_2.guess, guess, 5);
        case 2:
            strncpy(state->guess_3.guess, guess, 5);
        case 3:
            strncpy(state->guess_4.guess, guess, 5);
        case 4:
            strncpy(state->guess_5.guess, guess, 5);

        default:
            return 0;
    }
}
