#define MEMORY_GAME_TEST
#include "MemoryGame.c"
#include <assert.h>
#include <stdio.h>

int main(void) {
    /* Every deal contains exactly two of each emoji. */
    srand(42);
    for (int n = 0; n < 1000; ++n) {
        int counts[8] = {0};
        new_game();
        for (int i = 0; i < 16; ++i) {
            assert(deck[i] >= 0 && deck[i] < 8);
            ++counts[deck[i]];
        }
        for (int i = 0; i < 8; ++i) assert(counts[i] == 2);
    }
    for (int i = 0; i < 16; ++i) deck[i] = i % 8;
    assert(open_card(-1) == 0 && open_card(16) == 0);
    assert(open_card(0) == 1);
    assert(open_card(0) == 0 && moves == 0);
    assert(open_card(1) == 3 && moves == 1);
    assert(open_card(2) == 0 && second == 1 && moves == 1);
    close_pair();
    assert(open_card(0) == 1 && open_card(8) == 2);
    assert(pairs == 1 && found[0] && found[8]);
    assert(open_card(0) == 0 && moves == 2);
    for (int i = 1; i < 8; ++i) {
        assert(open_card(i) == 1 && open_card(i + 8) == 2);
    }
    assert(pairs == 8 && moves == 9);
    for (int i = 0; i < 16; ++i) assert(found[i] && open_card(i) == 0);
    new_game();
    assert(open_card(0) == 1);
    int different = 1;
    while (deck[different] == deck[0]) ++different;
    assert(open_card(different) == 3);
    new_game(); /* Restart during the mismatch delay. */
    assert(first == -1 && second == -1 && moves == 0 && pairs == 0);
    for (int i = 0; i < 16; ++i) assert(!found[i]);
    assert(open_card(0) == 1);
    puts("PASS: shuffle, repeated clicks, mismatch lock, matches, win, restart");
    return 0;
}
