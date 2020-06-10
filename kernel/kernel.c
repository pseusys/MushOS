int kek() {
    char* video_memory = (char*) 0xb8000;
    video_memory[0] = 'A';
    return 0;
}

void gate() {
    char* video_memory = (char*) 0xb8000;
    video_memory[0] = 'X';
    //video_memory[1] = 'A';
    //video_memory[2] = 'X';
    //video_memory[3] = 'A';
    //video_memory[4] = 'X';
}
