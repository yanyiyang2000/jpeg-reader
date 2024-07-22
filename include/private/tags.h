/**
 * @file   tags.h
 * 
 * @author Yiyang Yan
 * 
 * @date   2024/07/20
 * 
 * @brief  Enumeration of tags used in JPEG files.
 */

#ifndef TAGS_H
#define TAGS_H

#include <stdint.h>

/**
 * @brief Tag representation
 */
struct Tag {
    char     *Name;   /* The tag name */
    uint16_t Number;  /* The tag number in hexadecimal */
};

static struct Tag tags[] = {
    /* Baseline and Extension Tags [TIFF Rev 6.0, pp.117-118] */
    {"Image Width",                     0x0100},    // The image width
    {"Image Height",                    0x0101},    // The image height
    {"Bits Per Sample",                 0x0102},    // The number of bits per component
    {"Compression",                     0x0103},    // 1 = Uncompressed. 2 = CCITT 1D. 3 = Group 3 fax. 4 = Group 4 fax. 5 = LZW. 6 = JPEG. 32773 = PackBits
    {"Photometric Interpretation",      0x0106},    // 0 = WhiteIsZero. 1 = BlackIsZero. 2 = RGB. 3 = Palette color. 4 = Transparency mask. 5 = Seperated. 6 = YCbCr. 
    {"Image Description",               0x010E},    // The image description
    {"Make",                            0x010F},    // The manufacturer of the scanner, video digitizer, or other type of equipment used to generate the image
    {"Model",                           0x0110},    // The model name or number of the scanner, video digitizer, or other type of equipment used to generate the image
    {"Orientation",                     0x0112},    // The orientation of the image with respect to the rows and columns
    {"Samples Per Pixel",               0x0115},    // The number of components per pixel
    {"X Resolution",                    0x011A},    // The number of pixels per Resolution Unit in the Image Width
    {"Y Resolution",                    0x011B},    // The number of pixels per Resolution Unit in the Image Length
    {"Resolution Unit",                 0x0128},    // 1 = No absolute unit of measurement. 2 = Inch. 3 = Centimeter.
    {"Software",                        0x0131},    // The name and version number of the software package(s) used to create the image
    {"DateTime",                        0x0132},    // The date and time of the image creation
    {"Host Computer",                   0x013C},    // The computer and/or operating system in use at the time of image creation
    {"Tile Width",                      0x0142},    // The tile width in pixels
    {"Tile Length",                     0x0143},    // The tile length (height) in pixels
    {"JPEG Interchange Format",         0x0201},    // The offset of JPEG SOI in bytes
    {"JPEG Interchange Format Length",  0x0202},    // The length of JPEG data in bytes
    {"Y Cb Cr Positioning",             0x0213},    // The positioning of subsampled chrominance components relative to luminance samples

    /* Private Tags [Exif v3.0, pp.38-39] */
    {"Exif IFD Offset",                 0x8769},    // The offset of Exif IFD from IFH in bytes
    {"GPS Info IFD Offset",             0x8825},    // The offset of GPS Info IFD from IFH in bytes
    {"Interoperability IFD Offset",     0xA005},    // The offset of Interoperability IFD from IFH in bytes

    /* Private IFD (Exif IFD) Tags [Exif v3.0, pp.54-57] */
    {"Exposure Time",                   0x829A},    // The exposure time in seconds
    {"F Number",                        0x829D},    // The F number
    {"Exposure Program",                0x8822},    // 0 = Not define. 1 = Manual. 2 = Normal program. 3 = Aperture priority. 4 = Shutter priority. 5 = Creative program. 6 = Action program. 7 = Portrait mode. 8 = Landscape mode.
    {"Photographic Sensitivity",        0x8827},    // The sensitivity of the camera or input device. It could be standard output sensitivity (SOS), recommended exposure index (REI) or ISO speed.
    {"Exif Version",                    0x9000},    // The Exif version (4-byte ASCII string "0300" for Exif version 3.0)
    {"DateTime Original",               0x9003},    // The date and time when the image was generated
    {"DateTime Digitized",              0x9004},    // The date and time when the image was stored as digital date
    {"Offset Time",                     0x9010},    // The time difference from UTC including daylight saving time of the DateTime tag
    {"Offset Time Original",            0x9011},    // The time difference from UTC including daylight saving time of the DateTimeOriginal tag
    {"Offset Time Digitized",           0x9012},    // The time difference from UTC including daylight saving time of the DateTimeDigitized tag
    {"Components Configuration",        0x9101},    // 0 = Does not exist. 1 = Y. 2 = Cb. 3 = Cr. 4 = R. 5 = G. 6 = B.
    {"Shutter Speed Value",             0x9201},    // The shutter speed in APEX value
    {"Aperture Value",                  0x9202},    // The lens aperture in APEX value
    {"Brightness Value",                0x9203},    // The brightness in APEX value
    {"Exposure Bias Value",             0x9204},    // The exposure bias in APEX value
    {"Max Aperture Value",              0x9205},    // The max aperture value
    {"Metering Mode",                   0x9207},    // 0 = Unknown. 1 = Average. 2 = Center-weighted average. 3 = Spot. 4 = Multi-spot. 5 = Pattern. 6 = Partial. 7 = Other.
    {"Light Source",                    0x9208},    // The light source
    {"Flash",                           0x9209},    // The status of flash
    {"Focal Length",                    0x920A},    // The actual focal length in mm
    {"Subject Area",                    0x9214},    // The location and area of the main subject in the overall scene
    {"Maker Note",                      0x927C},    // The maker note (the offset of maker note in bytes)
    {"User Comment",                    0x9286},    // The user comment
    {"Subsec Time Original",            0x9291},    // The subseconds of the DateTimeOriginal tag
    {"Subsec Time Digitized",           0x9292},    // The subseconds of the DateTimeDigitize tag
    {"Flashpix Version",                0xA000},    // The Flashpix format version
    {"Color Space",                     0xA001},    // 1 = sRGB. 2 = Adobe RGB. 65533 = Wide Gamut RGB. 65534 = ICC profile. 65535 = Uncalibrated
    {"Pixel X Dimension",               0xA002},    // The width of the image
    {"Pixel Y Dimension",               0xA003},    // The height of the image
    {"Sensing Method",                  0xA217},    // 1 = Not defined. 2 = One-chip color area sensor. 3 = Two-chip color area sensor. 4 = Three-chip color area sensor. 5 = Color sequential area sensor. 7 = Trilinear sensor. 8 = Color sequential linear sensor.
    {"File Source",                     0xA300},    // 1 = Scanner of transparent type. 2 = Scanner of reflex type. 3 = DSC.
    {"Scene Type",                      0xA301},    // 1 = A directly photographed image.
    {"Custom Rendered",                 0xA401},    // 0 = Normal process. 1 = Custom process.
    {"Exposure Mode",                   0xA402},    // 0 = Auto exposure. 1 = Manual exposure. 2 = Auto bracket.
    {"White Balance",                   0xA403},    // 0 = Auto white balance. 1 = Manual white balance.
    {"Digital Zoom Ratio",              0xA404},    // The digital zoom ratio
    {"Focal Length In 35mm Film",       0xA405},    // The equivalent focal length of a 35mm film camera
    {"Scene Capture Type",              0xA406},    // 0 = Standard. 1 = Landscape. 2 = Portrait. 3 = Night scene.
    {"Gain Control",                    0xA407},    // 0 = None. 1 = Low gain up. 2 = High gain up. 3 = Low gain down. 4 = High gain down.
    {"Contrast",                        0xA408},    // 0 = Normal. 1 = Soft. 2 = Hard.
    {"Saturation",                      0xA409},    // 0 = Normal. 1 = Low saturation. 2 = High saturation.
    {"Sharpness",                       0xA40A},    // 0 = Normal. 1 = Soft. 2 = Hard.
    {"Lens Specification",              0xA432},    // Value 1: minimum focal length in mm. Value 2: maximum focal length in mm. Value 3: minimum F number in the minimum focal length. Value 4: minimum F number in the maximum focal length.
    {"Lens Make",                       0xA433},    // The manufacturer of the lens
    {"Lens Model",                      0xA434},    // The model name or number of the lens
    {"Composite Image",                 0xA460},    // 0 = Unknown. 1 = Not a composite image. 2 = General composite image. 3 = Composite image captured while shooting.

    /* Private IFD (GPS Info IFD) Tags [Exif v3.0, pp.90-91] */
    {"GPS Latitude Ref",                0x0001},    // N = North latitude. S = South latitude.
    {"GPS Latitude",                    0x0002},    // The latitude
    {"GPS Longitude Ref",               0x0003},    // E = East longitude. W = West longitude.
    {"GPS Longitude",                   0x0004},    // The longitude
    {"GPS Altitude Ref",                0x0005},    // 0 = Above sea level. 1 = Below se level.
    {"GPS Altitude",                    0x0006},    // The altitude
    {"GPS Time Stamp",                  0x0007},    // The GPS time in UTC
    {"GPS Speed Ref",                   0x000C},    // K = Kilometers per hour. M = Miles per hour. N = knots
    {"GPS Speed",                       0x000D},    // The speed of GPS receiver movement
    {"GPS Img Direction Ref",           0x0010},    // T = True direction. M = Magnetic direction
    {"GPS Img Direction",               0x0011},    // The direction of the image
    {"GPS Dest Bearing Ref",            0x0017},    // T = True direction. M = Magnetic direction
    {"GPS Dest Bearing",                0x0018},    // The bearing of destination
    {"GPS Date Stamp",                  0x001D},    // The GPS date in UTC
    {"GPS H Positioning Error",         0x001F}     // The horizontal positioning error
};

#endif /* TAGS_H */