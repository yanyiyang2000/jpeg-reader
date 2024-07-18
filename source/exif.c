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
    uint8_t  *seg_base = NULL;
    uint16_t seg_len   = 0;
    uint32_t ifd_ofst  = 0;

    /* Skip MARKER, now pointing at LENGTH */
    *ptr += 2;
    seg_base = *ptr;

    /* Parse LENGTH */
    seg_len = __builtin_bswap16(**(uint16_t **)ptr);

    /* Skip LENGTH, now pointing at IDENTIFIER */
    *ptr += 2;

    /* Parse IDENTIFIER */
    if (strncmp(*(const char **)ptr, "Exif", 5) != 0) {
        printf("Unknown identifier\n");
        return;
    }

    /* Skip IDENTIFIER, now pointing at BYTE ORDER of IFH */
    *ptr += 6;
    seg->IFH_Base = *ptr;
    
    /* Parse BYTE ORDER */
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

    /* Skip BYTE ORDER, now pointing at MAGIC NUMBER of IFH */
    *ptr += 2;

    /* Skip MAGIC NUMBER, now pointing at IFD OFFSET of IFH */
    *ptr += 2;

    /* Parse IFD OFFSET */
    ifd_ofst = (need_byte_swap) ? __builtin_bswap32(**(uint32_t **)ptr) : **(uint32_t **)ptr;

    /* Construct IFDs */
    seg->Next_IFD = calloc(1, sizeof(struct Image_File_Directory));
    ifd_construct(seg, 0, ifd_ofst);

    /* Skip EXIF Segment */
    *ptr = seg_base + seg_len;
}

void exif_parse(struct EXIF_Segment *seg) {
    /* Parse TIFF IFD */
    ifd_parse(seg, 0);

    /* Parse EXIF IFD */
    ifd_parse(seg, 1);

    /* Parse GPS IFD */
    ifd_parse(seg, 2);
}

void exif_free(struct EXIF_Segment *seg) {
    struct Image_File_Directory *prev_ifd = NULL;
    struct Image_File_Directory *curr_ifd = seg->Next_IFD;

    while (curr_ifd != NULL) {
        prev_ifd = curr_ifd;
        curr_ifd = curr_ifd->Next_IFD;
        free(prev_ifd->DEs);
        free(prev_ifd);
    }

    if (seg->EXIF_IFD != NULL) {
        free(seg->EXIF_IFD->DEs);
        free(seg->EXIF_IFD);
    }

    if (seg->GPS_IFD != NULL) {
        free(seg->GPS_IFD->DEs);
        free(seg->GPS_IFD);
    }
}

void ifd_construct(struct EXIF_Segment *seg, uint8_t idx, uint32_t ifd_ofst) {
    uint8_t                     *ptr      = seg->IFH_Base + ifd_ofst;
    uint16_t                    val_ofst  = 0;
    struct Image_File_Directory *curr_ifd = NULL;
    struct Directory_Entry      *curr_de  = NULL;

    switch (idx) {
        case 0: {
            curr_ifd = seg->Next_IFD;
            break;
        }
        case 1: {
            curr_ifd = seg->EXIF_IFD;
            break;
        }
        case 2: {
            curr_ifd = seg->GPS_IFD;
            break;
        }
        default: {
            printf("Unknown IFD index");
            return;
        }
    }

    while (1) {
        /* Parse DE COUNT */
        curr_ifd->DE_Count = (need_byte_swap) ? __builtin_bswap16(*(uint16_t *)ptr) : *(uint16_t *)ptr;

        /* Skip DE COUNT, now pointing at the first DE */
        ptr += 2;

        /* Construct DEs */
        curr_ifd->DEs = calloc(curr_ifd->DE_Count, sizeof(struct Directory_Entry));
        for (uint16_t i = 0; i < curr_ifd->DE_Count; i++) {
            /* Point to the current DE */
            curr_de = &(curr_ifd->DEs[i]);

            /* Parse and skip TAG */
            curr_de->Tag = (need_byte_swap) ? __builtin_bswap16(*(uint16_t *)ptr) : *(uint16_t *)ptr;
            ptr += 2;

            /* Parse and skip VALUE TYPE */
            curr_de->Value_Type = (need_byte_swap) ? __builtin_bswap16(*(uint16_t *)ptr) : *(uint16_t *)ptr;
            ptr += 2;

            /* Parse and skip VALUE COUNT */
            curr_de->Value_Count = (need_byte_swap) ? __builtin_bswap32(*(uint32_t *)ptr) : *(uint32_t *)ptr;
            ptr += 4;

            /* Parse VALUE OFFSET */
            val_ofst = (need_byte_swap) ? __builtin_bswap32(*(uint32_t *)ptr) : *(uint32_t *)ptr;

            /* Determine the source of values */
            switch (curr_de->Value_Type) {
                case BYTE:
                case SBYTE:
                case ASCII: 
                case UNDEFINED: {
                    curr_de->Value = (curr_de->Value_Count <= 4) ? ptr : seg->IFH_Base + val_ofst;
                    break;
                }

                case SHORT:
                case SSHORT: {
                    curr_de->Value = (curr_de->Value_Count <= 2) ? ptr : seg->IFH_Base + val_ofst;
                    break;
                }

                case LONG:
                case SLONG: {
                    curr_de->Value = (curr_de->Value_Count <= 1) ? ptr : seg->IFH_Base + val_ofst;
                    break;
                }

                case RATIONAL:
                case SRATIONAL:
                case FLOAT:
                case DOUBLE: {
                    curr_de->Value = seg->IFH_Base + val_ofst;
                    break;
                }

                default: {
                    printf("Unknown VALUE TYPE\n");
                    return;
                }
            }

            /* Skip VALUE OFFSET, now pointing at TAG of the next DE */
            ptr += 4;
        }

        /* Parse IFD OFFSET */
        ifd_ofst = (need_byte_swap) ? __builtin_bswap32(*(uint32_t *)ptr) : *(uint32_t *)ptr;

        /* Abort if there is no more IFD */
        if (ifd_ofst == 0) {
            break;
        } else {
            curr_ifd->Next_IFD = calloc(1, sizeof(struct Image_File_Directory));
            curr_ifd = curr_ifd->Next_IFD;
        }

        /* Now pointing at DE COUNT of the next IFD */
        ptr = seg->IFH_Base + ifd_ofst;
    }
}

void ifd_parse(struct EXIF_Segment *seg, uint8_t idx) {
    struct Image_File_Directory *curr_ifd = NULL;

    switch (idx) {
        case 0: {
            curr_ifd = seg->Next_IFD;
            break;
        }
        case 1: {
            curr_ifd = seg->EXIF_IFD;
            break;
        }
        case 2: {
            curr_ifd = seg->GPS_IFD;
            break;
        }
        default: {
            printf("Unknown IFD index\n");
            return;
        }
    }

    while (curr_ifd != NULL && curr_ifd->DE_Count != 0) {
        for (uint16_t i = 0; i < curr_ifd->DE_Count; i++) {
            printf("Tag: 0x%04"PRIX16", Type: %2"PRIX16", Val Cnt: %2"PRId16"\n", curr_ifd->DEs[i].Tag, curr_ifd->DEs[i].Value_Type, curr_ifd->DEs[i].Value_Count);
        }

        curr_ifd = curr_ifd->Next_IFD;
    }
}
