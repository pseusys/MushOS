#include "../drivers/screen.h"

int kek() {
    char* video_memory = (char*) 0xb8000;
    video_memory[0] = 'A';
    return 0;
}

void _start() {
    clear_screen();
    for (int i = 0; i < 2099; ++i) {
        print_char_color('a', YELLOW, LOW_BLUE);
    }
}
