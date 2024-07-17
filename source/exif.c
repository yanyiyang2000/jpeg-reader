#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpeg.h"
#include "exif.h"


static bool need_byte_swap = false;

void exif_construct(struct EXIF_Segment *seg, uint8_t **ptr) {
    uint32_t ifd_offset = 0;
    struct Image_File_Directory *curr_ifd = NULL;

    /* Skip Marker field, now pointing at Length field */
    *ptr += 2;

    /* Skip Length field, now pointing at Identifier field */
    *ptr += 2;

    /* Parse Identifier field */
    if (strncmp(*(const char **)ptr, "Exif", 5) != 0) {
        printf("Unknown identifier\n");
        return;
    }

    /* Skip Identifier field, now pointing at Byte Order field of IFH */
    *ptr += 6;
    seg->IFH_Base = *ptr;
    
    /* Parse Byte Order field */
    switch (**(uint16_t **)ptr) {
        case BYTE_ORDER_BIG_ENDIAN: {
            need_byte_swap = true;
            break;
        }
        case BYTE_ORDER_LITTLE_ENDIAN: {
            need_byte_swap = false;
            break;
        }
        default: {
            printf("Unknown endianess\n");
            return;
        }
    }

    /* Skip Byte Order field, now pointing at Magic Number field of IFH */
    *ptr += 2;

    /* Skip Magic Number field, now pointing at IFD Offset field of IFH */
    *ptr += 2;

    /* Construct EXIF Segment and IFDs */
    seg->Next_IFD = calloc(1, sizeof(struct Image_File_Directory));
    curr_ifd = seg->Next_IFD;
    while (1) {
        /* Parse IFD Offset field */
        if (need_byte_swap) {
            ifd_offset = __builtin_bswap32(**(uint32_t **)ptr);
        } else {
            ifd_offset = **(uint32_t **)ptr;
        }

        /* Abort if there is no more IFD */
        if (ifd_offset == 0) {
            break;
        }

        /* Skip IFH, now pointing at DE Count field of IFD */
        *ptr = seg->IFH_Base + ifd_offset;
        curr_ifd->IFD_Base = *ptr;

        /* Parse DE Count field */
        if (need_byte_swap) {
            curr_ifd->DE_Count = __builtin_bswap16(**(uint16_t **)ptr);
        } else {
            curr_ifd->DE_Count = **(uint16_t **)ptr;
        }

        /* Skip DE Count field, now pointing at DE entries */
        *ptr += 2;

        /* Skip DE entries, now pointing at IFD Offset field */
        *ptr += curr_ifd->DE_Count * 12;

        /* Move on to the next IFD */
        curr_ifd->Next_IFD = calloc(1, sizeof(struct Image_File_Directory));
        curr_ifd = curr_ifd->Next_IFD;
    }
    
    /* Skip IFD Offset field of the last IFD, now pointing at the Marker field of the next Marker Segment */
    *ptr += 4;
}

void exif_parse(struct EXIF_Segment *seg) {
    struct Image_File_Directory *curr_ifd = seg->Next_IFD;
    
    while (curr_ifd->DE_Count != 0) {
        printf("De Count: %"PRId16"\n", curr_ifd->DE_Count);
        curr_ifd = curr_ifd->Next_IFD;
    }
}

void exif_free(struct EXIF_Segment *seg) {
    struct Image_File_Directory *prev_ifd = NULL;
    struct Image_File_Directory *curr_ifd = seg->Next_IFD;

    while (curr_ifd != NULL) {
        prev_ifd = curr_ifd;
        curr_ifd = curr_ifd->Next_IFD;
        free(prev_ifd);
    }

    if (seg->EXIF_IFD != NULL) {
        free(seg->EXIF_IFD);
    }

    if (seg->GPS_IFD != NULL) {
        free(seg->GPS_IFD);
    }
}
