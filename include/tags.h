/* TIFF fields [TIFF Rev 6.0] */
#define Make             0x010F /* Manufacturer of the scanner, video digitizer, or other type of equipment used to generate the image */
#define Model            0x0110 /* The model name or number of the scanner, video digitizer, or other type of equipment used to generate the image */
#define Orientation      0x0112 /* The orientation of the image with respect to the rows and columns. */
#define XResolution      0x011A /* The number of pixels per ResolutionUnit in the ImageWidth */
#define YResolution      0x011B /* The number of pixels per ResolutionUnit in the ImageLength */
#define ResolutionUnit   0x0128 /* 1 = No absolute unit of measurement. 2 = Inch. 3 = Centimeter. */
#define Software         0x0131 /* Name and version number of the software package(s) used to create the image */
#define DateTime         0x0132 /* Date and time of image creation */
#define HostComputer     0x013c /* The computer and/or operating system in use at the time of image creation */
#define TileWidth        0x0142 /* The tile width in pixels */
#define TileLength       0x0143 /* The tile length (height) in pixels */
#define YCbCrPositioning 0x0213 /* Specifies the positioning of subsampled chrominance components relative to luminance samples */

/* Exif-specific fields [Exif v2.32] */
#define Exif_IFD_Pointer     0x8769 /* A pointer to the Exif IFD */
#define GPS_Info_IFD_Pointer 0x8825 /* A pointer to the GPS Info IFD */