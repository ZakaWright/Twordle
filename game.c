#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <stdbool.h>
#include <ncurses.h>
#include <ctype.h>
#include "file_read.h"
#include "state.h"


void printWelcome();
void gameLoop(struct State* state);
void gameLoopText(char *word);
int* compareWordsText(char* word, char* guess);
void compareWords(struct State* state);
void printScreen(struct State* state);
void printKey(int row, int col, struct Key key);
void printChar(int row, int col, char c, int status);
void printPartialChar(struct State* state, int buff_index, char ch);


int main(int argc, char *argv[]) {
    //char word[6];
    struct State state = {.word = {0}, .turn = 0,
        .guess_1 = {{0}, {0}},
        .guess_2 = {{0}, {0}},
        .guess_3 = {{0}, {0}},
        .guess_4 = {{0}, {0}},
        .guess_5 = {{0}, {0}},
        .keyboard = {{'Q', 0}, {'W', 0}, {'E', 0}, {'R', 0}, {'T', 0}, {'Y', 0}, {'U', 0}, 
            {'I', 0}, {'O', 0}, {'P', 0}, {'A', 0}, {'S', 0}, {'D', 0}, {'F', 0}, {'G', 0}, 
            {'H', 0}, {'J', 0}, {'K', 0}, {'L', 0}, {'Z', 0}, {'X', 0}, {'C', 0}, {'V', 0},
            {'B', 0}, {'N', 0}, {'M', 0}}};
    void* wordPtr = state.word;
    getWord(wordPtr, sizeof(state.word)); 
    // initialize ncurses
    int ch;
    initscr();      // start curses
    raw();          // disables line buffering
    curs_set(0);    // hide the cursor
    keypad(stdscr, TRUE);
    noecho();
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    
    init_pair(8, COLOR_BLACK, COLOR_WHITE);
    init_pair(9, COLOR_RED, COLOR_WHITE);
    init_pair(10, COLOR_YELLOW, COLOR_WHITE);
    init_pair(11, COLOR_GREEN, COLOR_WHITE);

    init_pair(12, COLOR_WHITE, COLOR_GREEN);
    init_pair(13, COLOR_WHITE, COLOR_YELLOW);

    //ch = getch();
    refresh();
    gameLoop(&state);
    ch = getch();
    endwin();       // stop curses



    printf("Word from main: %s\n", &state.word);

    //gameLoop(word);

}

void printWelcome() {

    move(0, 0);
    attron(COLOR_PAIR(2));
    printw(",--------. ,--.   ,--.  ,-----.  ,------.  ,------.   ,--.    ,------.\n");
    printw("'--.  .--' |  |   |  | '  .-.  ' |  .--. ' |  .-.  \\  |  |    |  .---'\n");
    printw("   |  |    |  |.'.|  | |  | |  | |  '--'.' |  |  \\  : |  |    |  `--,\n");
    printw("   |  |    |   ,'.   | '  '-'  ' |  |\\  \\  |  '--'  / |  '--. |  `---.\n");
    printw("   `--'    '--'   '--'  `-----'  `--' '--' `-------'  `-----' `------'\n");
    attroff(COLOR_PAIR(2));

}

void printScreen(struct State* state) {
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);
    printWelcome();
    //int start_key_row = max_row - 11;
    //int start_key_col = (max_col / 2) - 20;
    state->start_key_row = max_row - 11;
    state-> start_key_col = (max_col / 2) - 20;
    
    // print keyboard
    move(state->start_key_row, state->start_key_col);
    int keyboard_length = 26; 
    int row, col;
    for (int i = 0; i < keyboard_length; i++) {
        getyx(stdscr, row, col);
        struct Key key = state->keyboard[i];
        if (i == 10) {
            row = state->start_key_row + 3;
            col = state->start_key_col +2;
        } else if (i == 19) {
            row = state->start_key_row + 6;
            col = state->start_key_col +6;
        }
        
        printKey(row, col, key);
        // reset the row and move the column
        move(row, (col+4));

    }
    refresh();

    // WORDS
    state->start_words_row = state->start_key_row - 25;
    state->start_words_col = (max_col / 2) - 17;

    // print words
    for (int i = 0; i < state->turn; i++) {
        struct Guess* guess;

        switch (i) {
            case(0):
                guess = &state->guess_1; break;
            case(1):
                guess = &state->guess_2; break;
            case(2):
                guess = &state->guess_3; break;
            case(3):
                guess = &state->guess_4; break;
            case(4):
                guess = &state->guess_5; break;
        }
        for (int j = 0; j < 5; j++) {
            row = state->start_words_row + (5 * i);
            col = state->start_words_col + (7 * j);
            printChar(row, col, guess->guess[j], guess->result[j]);
        }
    }
    // print empty
    for (int i = state->turn; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            row = state->start_words_row + (5 * i);
            col = state->start_words_col + (7 * j);
            move(row, col);
            printw("-----");
            mvprintw(++row, col, "|   |");
            mvprintw(++row, col, "-----");
        }
    }
    refresh();
}

void printKey(int row, int col, struct Key key) {
    /*
        init_pair(8, COLOR_BLACK, COLOR_WHITE); // unplayed (0)
        init_pair(9, COLOR_RED, COLOR_WHITE);   // invalid letter (3)
        init_pair(10, COLOR_YELLOW, COLOR_WHITE); // wrong place (2)
        init_pair(11, COLOR_GREEN, COLOR_WHITE); // correct place (1)
        */
    if (key.status == 0) {
        // not played
        attron(COLOR_PAIR(8));
    } else if (key.status == 3) {
        // invalid letter
        attron(COLOR_PAIR(9));
    } else if (key.status == 2) {
        // wrong place
        attron(COLOR_PAIR(10));
    } else if (key.status == 1) {
        // correct place
        attron(COLOR_PAIR(11));
    }
    mvprintw(row, col, "---");
    mvprintw(++row, col, "|%c|", key.key);
    mvprintw(++row, col, "---");
    
    if (key.status == 0) {
        // not played
        attroff(COLOR_PAIR(8));
    } else if (key.status == 3) {
        // invalid letter
        attroff(COLOR_PAIR(9));
    } else if (key.status == 2) {
        // wrong place
        attroff(COLOR_PAIR(10));
    } else if (key.status == 1) {
        // correct place
        attroff(COLOR_PAIR(11));
    }
}

void printChar(int row, int col, char c, int status) {
    switch(status) {
        case (0):
            attron(COLOR_PAIR(7)); break;
        case (1):
            attron(COLOR_PAIR(12)); break;
        case (2):
            attron(COLOR_PAIR(13)); break;
    }
    move(row, col);
    printw("-----");
    mvprintw(++row, col, "| %c |", c);
    mvprintw(++row, col, "-----");
    switch(status) {
        case (0):
            attroff(COLOR_PAIR(7)); break;
        case (1):
            attroff(COLOR_PAIR(12)); break;
        case (2):
            attroff(COLOR_PAIR(13)); break;
    }
}

void printPartialChar(struct State *state, int buff_index, char ch) {
    int row = state->start_words_row + (5 * state->turn + 1);
    int col = state->start_words_col + (7 * buff_index + 2);
    mvprintw(row, col, "%c", ch);
    refresh();
}

void gameLoop(struct State* state) {
    char guess[7] = {0};
    int ch;
    int buff_index = 0;
    bool valid_guess = false;

    printScreen(state);
    for (state->turn; state->turn < 5; state->turn++) {
        printScreen(state);
        while (!valid_guess) {
            ch = getch();

            if (isalnum(ch) == 0) {
                if (ch == 3) {
                    // Ctrl+C
                    //exit(1);
                    return;
                } else if (ch == KEY_BACKSPACE) {
                    // TODO delete from buffer and screen
                    if (buff_index >= 0) {
                        if (buff_index > 0) {
                            // the buff_index will always be 1 more than the index that needs to be deleted
                            // just make sure it's not 0
                            buff_index --;
                        }
                        guess[buff_index] = '\0';
                        printPartialChar(state, buff_index, ' ');
                    }
                    continue;
                } else if (ch == KEY_ENTER || ch == '\n' || ch == '\r') {
                    // TODO validate buffer

                    if (buff_index != 5) {
                        // invalid
                        continue;
                    }
                    // valid guess (5 characters)
                    valid_guess = true;
                    add_guess(state, guess);
                    compareWords(state);
                    buff_index = 0;
                    //if (state->turn != 5)
                        //state->turn++;
                }
                continue;
            }
            if (ch >= 'a' && ch <= 'z')
                ch = toupper(ch);
            // print to screen
            if (buff_index <= 4) {
                printPartialChar(state, buff_index, ch);
                guess[buff_index] = ch;
                buff_index++;
            }
        }
        valid_guess = false;
        //printScreen(state);
    }
}
void gameLoopText(char *word) {
    char guess[7] = {0};
    for (int turn = 1; turn < 5; turn++) {
        printf("> ");
        fgets(guess, sizeof(guess), stdin);
        // strip new line from guess
        guess[strcspn(guess, "\n")] = '\0';

        if (strcmp(word, guess) == 0) {
            // win
            printf("WIN!");
        } else {
            int* result = compareWordsText(word, guess);
            for (int i = 0; i < 5; i++) {
                if (result[i] == 1) {
                    printf("*%c*\t", guess[i]);
                } else if (result[i] == 2) {
                    printf(" %c*\t", guess[i]);
                } else {
                    printf(" %c \t", guess[i]);
                }
            }
            printf("\n _ \t _ \t _ \t _ \t _ \t\n");
            free(result);
        }

    }
}

void compareWords(struct State* state) {
    struct Guess *guess;
    struct Key *keys = state->keyboard;

    switch (state-> turn) {
        case(0):
            guess = &state->guess_1;
            break;
        case(1):
            guess = &state->guess_2;
            break;
        case(2):
            guess = &state->guess_3;
            break;
        case(3):
            guess = &state->guess_4;
            break;
        case(4):
            guess = &state->guess_5;
            break;
    }
    //guess.result = malloc(5 * sizeof(int));
    //memset(guess->result, 0, sizeof(guess->result));

    for (int i = 0; i < 5; i++) {
        //guess->result[i] = 0;
        mvprintw(i, 0, "in for loop");
        refresh();
        if (state->word[i] == guess->guess[i]) {
            // correct location
            guess->result[i] = 1;
            mvprintw(i, 20, "in assign loop");
            refresh();
        } else {
            for (int j = 0; j < 5; j++) {
                // don't need to rule out if guess[i] == word[i]
                mvprintw(i, 40, "in inner loop");
                refresh();
                if (guess->guess[i] == state->word[j]) {
                    // incorrect location
                    guess->result[i] = 2;
                    break;
                }
            }
        }
        // Update the status of the keyboard
        // look at a better way to handle this since it's not very efficient
        for (int j = 0; j < 26; j++) {
            if (guess->guess[i] == keys[j].key) {
                keys[j].status = guess->result[i];
                mvprintw(0, 0, "Status of key %c: %d", keys->key, keys->status);
                refresh();
            }

        }
    }
    //return result;
}
int* compareWordsText(char *word, char *guess) {
    //int result[5] = {0};
    int *result = malloc(5 * sizeof(int));

    for (int i = 0; i < 5; i++) {
        result[i] = 0;
        if (word[i] == guess[i]) {
            result[i] = 1;
        } else {
            for (int j = 0; j < 5; j++) {
                // don't need to rule out if guess[i] == word[i]
                if (guess[i] == word[j]) {
                    result[i] = 2;
                    break;
                }
            }
        }
    }

    return result;
}
