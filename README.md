# JPEG File Format [^1.1]
Metadata of a JPEG file is stored in multiple *Application Marker Segments* (**APP**).

| Segment    | Marker        | Length   |
| ---------- | ------------- | -------- |
| SOI        | `FF D8`[^1.2] | 0[^1.3]  |
| APP0       | `FF E0`       | variable |
| APP1       | `FF E1`       | variable |
| ...        | ...           | ...      |
| APPn       | `FF En`       | variable |
| DQT        | `FF DB`       | variable |
| DHT1       | `FF C4`       | variable |
| DHT2       | `FF C4`       | variable |
| ...        | ...           | ...      |
| DHTn       | `FF C4`       | variable |
| DRI        | `FF DD`       | 4        |
| SOS        | `FF DA`       | variable |
| Image Data | -             | variable |
| EOI        | `FF D9`       | variable |

[^1.1]: Specified in Exif v2.32, p19 and ISO 10918-1, p33
[^1.2]: Unless otherwise stated, all monospace texts are in hexadecimal
[^1.3]: Unless otherwise stated, all lengths are in bytes

We only care about the information stored in **APP0** and **APP1**.


## APP0 [^2.1]
| Description | Length   | Value                  |
| ----------- | -------- | ---------------------- |
| Marker      | 2        | `FF E0`                |
| Length      | 2        | [^2.2]                 |
| Identifier  | 5        | `4A 46 49 46 00`[^2.3] |
| Version     | 2        |                        |
| Unit        | 1        |                        |
| Xdensity    | 2        |                        |
| Ydensity    | 2        |                        |
| Xthumbnail  | 2        |                        |
| Ythumbnail  | 2        |                        |

[^2.1]: Specified in JFIF v1.02, p5
[^2.2]: All values of **APP0** are in big-endian
[^2.3]: ASCII string "JFIF" terminated by a null byte


## APP1 [^3.1]
**APP1** is composed of a *Image File Header* (**IFH**) and multiple *Image File Directories* (**IFD**s).

| Description           | Length   | Value                     |
| --------------------- | -------- | ------------------------- |
| Marker                | 2        | `FF E1`                   |
| Length                | 2        | [^3.2]                    |
| Identifier            | 6        | `45 78 69 66 00 00`[^3.3] |
| IFH                   | 8        | See [IFH](#ifh)           |
| 0th IFD               | variable | See [IFD](#ifd)           |
| 1st IFD               | variable |                           |
| ...                   | ...      |                           |
| nth IFD               | variable |                           |

[^3.1]: Specified in Exif v2.32, p19
[^3.2]: Value of Length is in big-endian
[^3.3]: ASCII string "Exif" terminated by two null bytes


## IFH [^4.1]
| Description             | Length   | Value   |
| ----------------------- | -------- | ------- |
| Byte Order[^4.2]        | 2        | [^4.3]  |
| Magic Number            | 2        | `00 2A` |
| Offset of 0th IFD[^4.4] | 4        |         |

[^4.1]: Specified in TIFF rev6.0, p13
[^4.2]: Byte Order dictates the endianess of the following values
[^4.3]: `49 49` for small endian and `4D 4D` for big endian
[^4.4]: Offset is from the first byte of IFH


## IFD [^5.1]
Each **IFD** is composed of multiple *Directory Entries* (**DE**s).

| Description              | Length   | Value   |
| ------------------------ | -------- | ------- |
| DE Count                 | 2        |         |
| DE 1                     | 12       |         |
| ...                      | ...      | ...     |
| DE n                     | 12       |         |
| Offset of Next IFD[^5.2] | 4        | [^5.3]  |
| IFD Value                | variable |         |

[^5.1]: Specified in Exif v2.32, p24
[^5.2]: Offset is from the first byte of IFH
[^5.3]: `00 00 00 00` for the last IFD


## DE [^6.1]
Each **DE** is 12-byte.

| Description        | Length   | Value  |
| ------------------ | -------- | ------ |
| Tag                | 2        |        |
| Type               | 2        |        |
| Value Count        | 4        |        |
| Value/Offset[^6.2] | 4        |        |

[^6.1]: Specified in TIFF rev6.0, p14
[^6.2]: If 4 bytes are not enough to hold all the values, this value indicates the offset of the actual values from the first byte of IFH

# Workflow Overview
![alt text](/assets/flowchart.png)

# Construct Exif Segment
![alt text](/assets/flowchart_exif.png)

# Data Structure

## `Marker_Segment`
Struct `Marker_Segment` is composed of
```c
typedef struct __attribute__((packed)) {
    uint8_t  Marker[2];
    uint16_t Length;
    union {
        JFIF_Segment *jfif_segment;
        Exif_Segment *exif_segment;
    };
} Marker_Segment;
```

## `JFIF_Segment`
Struct `JFIF_Segment` is composed of
```c
typedef struct __attribute__((packed)) {
    uint8_t  Identifier[5]; 
    uint16_t Version;
    uint8_t  Unit;
    uint16_t XDensity;
    uint16_t YDensity;
    uint8_t  XThumbnail;
    uint8_t  YThumbnail;
    uint8_t  *RGBn;
    uint8_t  AMPF[4];
} JFIF_Segment;
```

## `Exif_Segment`
Struct `Exif_Segment` is composed of
```c
typedef struct __attribute__((packed)) {
    uint8_t                      Identifier[6];
    Image_File_Header            IFH;
    struct Image_File_Directory  *IFDs;
} Exif_Segment;
```

## `Image_File_Header`
Struct `Image_File_Header` is composed of
```c
typedef struct __attribute__((packed)) {
    uint16_t Byte_Order;
    uint16_t Magic_Number;
    uint32_t IFD_Offset;
} Image_File_Header;
```

## `Image_File_Directory`
Struct `Image_File_Directory` is composed of
```c
struct Image_Field_Directory {
    uint16_t                     DE_Count;
    Directory_Entry              *DE;
    uint32_t                     IFD_Offset;
    struct Image_Field_Directory *Next_IFD;
};
```

## `Directory_Entry`
Struct `Directory_Entry` is composed of
```c
typedef struct __attribute__((packed)) {
    uint16_t Tag;
    uint16_t Type;
    uint32_t Value_Count;
    union {
        uint32_t Value_Offset;
        uint8_t  *uValue1;
        uint16_t *uValue2;
        uint32_t *uValue4;
        int8_t   *sValue1;
        int16_t  *sValue2;
        int32_t  *sValue4;
        float    *fValue4;
        double   *fValue8;
    };
} Directory_Entry;
```
