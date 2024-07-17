#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpeg.h"

int main(int argc, char *argv[]) {
    FILE        *fd   = NULL;
    uint8_t     *buf  = NULL;
    struct JPEG *jpeg = NULL;

    /* Allocate dynamic memory */
    buf  = calloc(1024000, 1);
    jpeg = calloc(1, sizeof(struct JPEG));

    /* Assert argument */
    // if (argc != 2) {
    //     printf("Usage: test <FILE_NAME>");
    //     return 1;
    // }

    /* Read image data */
    // fd = fopen(argv[1], "rb");
    fd = fopen("./images/test.jpeg", "rb");
    fread(buf, 1024000, 1, fd);
    fclose(fd);

    /* Construct JPEG struct */
    jpeg_construct(jpeg, buf);

    /* Parse JPEG struct */
    jpeg_parse(jpeg);

    /* Free the dynamically allocated memory */
    jpeg_free(jpeg);
    free(jpeg);
    free(buf);
}