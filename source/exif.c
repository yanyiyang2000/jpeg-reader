#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpeg.h"
#include "exif.h"
#include "tags.h"


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
        case BYTE_ORDER_MM: {
            need_byte_swap = true;
            break;
        }
        case BYTE_ORDER_II: {
            need_byte_swap = false;
            break;
        }
        default: {
            printf("Unknown endianess\n");
            return;
        }
    }

    /* Skip BYTE ORDER and MAGIC NUMBER, now pointing at IFD OFFSET of IFH */
    *ptr += 4;

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

            /* Construct EXIF IFD and GPS IFD if exist */
            switch (curr_de->Tag) {
                case 0x8769: {
                    seg->EXIF_IFD = calloc(1, sizeof(struct Image_File_Directory));
                    ifd_construct(seg, 1, val_ofst);
                    break;
                }

                case 0x8825: {
                    seg->GPS_IFD = calloc(1, sizeof(struct Image_File_Directory));
                    ifd_construct(seg, 2, val_ofst);
                    break;
                }

                default: break;
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
    struct Directory_Entry      *curr_de  = NULL;
    char *tag_name = NULL;

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

        printf("┌────────────────────────────────┬───────────┬───────┬───────────────────────────────────────────────────┐\n");
        printf("│             Tag                │   Type    │ Count │                       Value                       │\n");
        printf("├────────────────────────────────┼───────────┼───────┼───────────────────────────────────────────────────┤\n");

        for (uint16_t i = 0; i < curr_ifd->DE_Count; i++) {
            curr_de = &(curr_ifd->DEs[i]);

            /* Obtain TAG description */
            for (uint16_t i = 0; i < (sizeof(tags)/sizeof(struct Tag)); i++) {
                if (tags[i].Number == curr_de->Tag) {
                    tag_name = tags[i].Name;
                    break;
                }
            }

            /* In case of unknown TAG, print the TAG in hex */
            if (tag_name == NULL) {
                printf("│ 0x%-25"PRIX16" │ %-9s │ %-5s │ %-49s │\n", curr_de->Tag, "", "", "");
            }
            
            switch (curr_de->Value_Type) {
                case BYTE: {
                    uint8_t *ptr = (uint8_t *)(curr_de->Value);
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49"PRIu8" │\n", tag_name, "BYTE", curr_de->Value_Count, *ptr);
                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRIu8" │\n", "", "", "", *(ptr + i));
                    }
                    break;
                }

                case SBYTE: {
                    int8_t *ptr = (int8_t *)(curr_de->Value);
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49"PRId8" │\n", tag_name, "SBYTE", curr_de->Value_Count, *ptr);
                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRId8" │\n", "", "", "", *(ptr + i));
                    }
                    break;
                }

                case ASCII: {
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "ASCII", curr_de->Value_Count, (char *)(curr_de->Value));
                    break;
                }

                case SHORT: {
                    uint16_t *ptr = (uint16_t *)(curr_de->Value);
                    uint16_t val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap16(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49"PRIu16" │\n", tag_name, "SHORT", curr_de->Value_Count, val);

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap16(*ptr) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRIu16" │\n", "", "", "", val);
                    }

                    break;
                }

                case SSHORT: {
                    int16_t *ptr = (int16_t *)(curr_de->Value);
                    int16_t val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap16(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49"PRId16" │\n", tag_name, "SSHORT", curr_de->Value_Count, val);

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap16(*ptr) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRId16" │\n", "", "", "", val);
                    }

                    break;
                }

                case LONG: {
                    uint32_t *ptr = (uint32_t *)(curr_de->Value);
                    uint32_t val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap32(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49"PRIu32" │\n", tag_name, "LONG", curr_de->Value_Count, val);

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap32(*ptr) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRIu32" │\n", "", "", "", val);
                    }

                    break;
                }

                case SLONG: {
                    int32_t *ptr = (int32_t *)(curr_de->Value);
                    int32_t val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap32(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRId32" │ %-49"PRId32" │\n", tag_name, "SLONG", curr_de->Value_Count, val);

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap32(*ptr) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49"PRId32" │\n", "", "", "", val);
                    }

                    break;
                }

                case RATIONAL: {
                    uint32_t *ptr = (uint32_t *)(curr_de->Value);
                    uint32_t val1 = 0;
                    uint32_t val2 = 0;

                    val1 = (need_byte_swap) ? __builtin_bswap32(*ptr) : *ptr;
                    val2 = (need_byte_swap) ? __builtin_bswap32(*(ptr + 1)) : *(ptr + 1);
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "RATIONAL", curr_de->Value_Count, (double)(val1 / val2));

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val1 = (need_byte_swap) ? __builtin_bswap32(*(ptr + i)) : *(ptr + i);
                        val2 = (need_byte_swap) ? __builtin_bswap32(*(ptr + 2*i + 1)) : *(ptr + 2*i + 1);
                        printf("│ %-30s | %-9s │ %-5s │ %-49f │\n", "", "", "", (double)(val1 / val2));
                    }

                    break;
                }

                case SRATIONAL: {
                    int32_t *ptr = (int32_t *)(curr_de->Value);
                    int32_t val1 = 0;
                    int32_t val2 = 0;

                    val1 = (need_byte_swap) ? __builtin_bswap32(*ptr) : *ptr;
                    val2 = (need_byte_swap) ? __builtin_bswap32(*(ptr + 1)) : *(ptr + 1);
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "SRATIONAL", curr_de->Value_Count, (double)(val1 / val2));

                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val1 = (need_byte_swap) ? __builtin_bswap32(*(ptr + i)) : *(ptr + i);
                        val2 = (need_byte_swap) ? __builtin_bswap32(*(ptr + 2*i + 1)) : *(ptr + 2*i + 1);
                        printf("│ %-30s | %-9s │ %-5s │ %-49f │\n", "", "", "", (double)(val1 / val2));
                    }

                    break;
                }

                case FLOAT: {
                    float *ptr = (float *)(curr_de->Value);
                    float val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap32(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "FLOAT", curr_de->Value_Count, val);
                    
                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap32(*(ptr + i)) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49f │\n", "", "", "", val);
                    }

                    break;
                }

                case DOUBLE: {
                    double *ptr = (double *)(curr_de->Value);
                    double val  = 0;

                    val = (need_byte_swap) ? __builtin_bswap64(*ptr) : *ptr;
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "DOUBLE", curr_de->Value_Count, val);
                    
                    for (uint16_t i = 0; i < curr_de->Value_Count - 1; i++) {
                        val = (need_byte_swap) ? __builtin_bswap64(*(ptr + i)) : *(ptr + i);
                        printf("│ %-30s | %-9s │ %-5s │ %-49f │\n", "", "", "", val);
                    }

                    break;
                }

                case UNDEFINED: {
                    printf("│ %-30s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "UNDEFINED", curr_de->Value_Count, "");
                    break;
                }

                default: {
                    printf("Unknown VALUE TYPE\n");
                    return;
                }
            }
        
            if (i == curr_ifd->DE_Count - 1) {
                printf("└────────────────────────────────┴───────────┴───────┴───────────────────────────────────────────────────┘\n");
            } else {
                printf("├────────────────────────────────┼───────────┼───────┼───────────────────────────────────────────────────┤\n");
            }
        }

        curr_ifd = curr_ifd->Next_IFD;
    }
}
