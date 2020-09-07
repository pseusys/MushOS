#include <stdio.h>
#include <stdlib.h>
#include "adapter/adapter.h"
#include "file_system/file_input.h"
#include "file_system/file_output.h"

int main() {
    printf("\n\nCreating disk image!\n");

    printf("Creating FS...\n");
    create_fs();
    printf("Done!\n");

    printf("Formatting drive...\n");
    format_drive();
    printf("Done!\n");

    printf("Checking root...\n");
    printf("Drive valid: %d\n", check_drive());
    printf("Done!\n");

    file* stasis = create_file_global("/foo/bar/stasis.lo");
    write_file(stasis, (byte*) "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc magna lorem, lobortis sed mattis eu, posuere sit amet eros placerat.", 131);
    free(stasis);

    byte* sputnik;
    int sputnik_size = read_external("../MushDome/Sputnik/build/main.elf", &sputnik);
    file* orbit = create_file_global("/orbit/sputnik.elf");
    write_file(orbit, sputnik, sputnik_size);
    free(orbit);
    free(sputnik);



    stasis = open_file_global("/foo/bar/stasis.lo");
    printf("\nStasis (%d):\n", stasis->header->size);
    byte* content = malloc(stasis->header->size);
    read_bytes(stasis, content, stasis->header->size, 0);
    printf("%s\n", content);
    close_file(stasis);
    free(content);
    printf("\n\n");

    stasis = open_file_global("/orbit/sputnik.elf");
    printf("\nSputnik (%d):\n", stasis->header->size);
    content = malloc(stasis->header->size);
    read_bytes(stasis, content, stasis->header->size, 0);
    for (int i = 0; i < stasis->header->size; ++i) printf("%d ", content[i]);
    printf("\n");
    for (int i = 0; i < stasis->header->size; ++i) printf("%c", content[i]);
    printf("\n");
    close_file(stasis);
    free(content);



    int inserted = insert_header("../MushCore/images/floppy.img");
    printf("Header inserted: %d\n", inserted);
    if (inserted != 1) {
        printf("Failed to create disk image!\n\n");
        return inserted;
    } else {
        printf("Disk image creation finished successfully!\n\n");
        return 0;
    }
}
