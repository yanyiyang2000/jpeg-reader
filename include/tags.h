/* Baseline Tags [TIFF Rev 6.0, pp.117-118] */
#define Compression                 0x0103  // 1 = Uncompressed. 2 = CCITT 1D. 3 = Group 3 fax. 4 = Group 4 fax. 5 = LZW. 6 = JPEG. 32773 = PackBits
#define Make                        0x010F  // The manufacturer of the scanner, video digitizer, or other type of equipment used to generate the image
#define Model                       0x0110  // The model name or number of the scanner, video digitizer, or other type of equipment used to generate the image
#define Orientation                 0x0112  // The orientation of the image with respect to the rows and columns
#define XResolution                 0x011A  // The number of pixels per ResolutionUnit in the ImageWidth
#define YResolution                 0x011B  // The number of pixels per ResolutionUnit in the ImageLength
#define ResolutionUnit              0x0128  // 1 = No absolute unit of measurement. 2 = Inch. 3 = Centimeter.
#define Software                    0x0131  // The name and version number of the software package(s) used to create the image
#define DateTime                    0x0132  // The date and time of the image creation
#define HostComputer                0x013C  // The computer and/or operating system in use at the time of image creation

/* Extension Tags [TIFF Rev 6.0, pp.117-118] */
#define TileWidth                   0x0142  // The tile width in pixels
#define TileLength                  0x0143  // The tile length (height) in pixels
#define JPEGInterchangeFormat       0x0201  // The offset to JPEG SOI
#define JPEGInterchangeFormatLength 0x0202  // The length of JPEG data in bytes
#define YCbCrPositioning            0x0213  // The positioning of subsampled chrominance components relative to luminance samples

/* Private Tags [Exif v3.0, pp.38-39] */
#define Exif_IFD_Pointer            0x8769  // A pointer to the Exif IFD <TODO>
#define GPS_Info_IFD_Pointer        0x8825  // A pointer to the GPS Info IFD <TODO>

/* Private IFD (Exif IFD) Tags [Exif v3.0, pp.54-57] */
#define ExposureTime                0x829A  // The exposure time in seconds
#define FNumber                     0x829D  // The F number
#define ExposureProgram             0x8822  // 0 = Not define. 1 = Manual. 2 = Normal program. 3 = Aperture priority. 4 = Shutter priority. 5 = Creative program. 6 = Action program. 7 = Portrait mode. 8 = Landscape mode.
#define PhotographicSensitivity     0x8827  // The sensitivity of the camera or input device. It could be standard output sensitivity (SOS), recommended exposure index (REI) or ISO speed.
#define DateTimeOriginal            0x9003  // The date and time when the image was generated
#define DateTimeDigitized           0x9004  // The date and time when the image was stored as digital date
#define OffsetTime                  0x9010  // The time difference from UTC including daylight saving time of the DateTime tag
#define OffsetTimeOriginal          0x9011  // The time difference from UTC including daylight saving time of the DateTimeOriginal tag
#define OffsetTimeDigitized         0x9012  // The time difference from UTC including daylight saving time of the DateTimeDigitized tag
#define ShutterSpeedValue           0x9201  // The shutter speed in APEX value
#define ApertureValue               0x9202  // The lens aperture in APEX value
#define BrightnessValue             0x9203  // The brightness in APEX value
#define ExposureBiasValue           0x9204  // The exposure bias in APEX value
#define MeteringMode                0x9207  // 0 = Unknown. 1 = Average. 2 = Center-weighted average. 3 = Spot. 4 = Multi-spot. 5 = Pattern. 6 = Partial. 7 = Other.
#define Flash                       0x9209  // The status of flash
#define FocalLength                 0x920A  // The actual focal length in mm
#define SubjectArea                 0x9214  // The location and area of the main subject in the overall scene
#define MakerNote                   0x927C  // The maker note
#define SubsecTimeOriginal          0x9291  // The subseconds of the DateTimeOriginal tag
#define SubsecTimeDigitized         0x9292  // The subseconds of the DateTimeDigitize tag
#define FlashpixVersion             0xA000  // The Flashpix format version
#define ColorSpace                  0xA001  // 1 = sRGB. 2 = Adobe RGB. 65533 = Wide Gamut RGB. 65534 = ICC profile. 65535 = Uncalibrated
#define PixelXDimension             0xA002  // The width of the image
#define PixelYDimension             0xA003  // The height of the image
#define SensingMethod               0xA217  // 1 = Not defined. 2 = One-chip color area sensor. 3 = Two-chip color area sensor. 4 = Three-chip color area sensor. 5 = Color sequential area sensor. 7 = Trilinear sensor. 8 = Color sequential linear sensor.
#define SceneType                   0xA301  // 1 = A directly photographed image.
#define ExposureMode                0xA402  // 0 = Auto exposure. 1 = Manual exposure. 2 = Auto bracket.
#define WhiteBalance                0xA403  // 0 = Auto white balance. 1 = Manual white balance.
#define FocalLengthIn35mmFilm       0xA405  // The equivalent focal length of a 35mm film camera
#define SceneCaptureType            0xA406  // 0 = Standard. 1 = Landscape. 2 = Portrait. 3 = Night scene.
#define LensSpecification           0xA432  // Value 1: minimum focal length in mm. Value 2: maximum focal length in mm. Value 3: minimum F number in the minimum focal length. Value 4: minimum F number in the maximum focal length.
#define LensMake                    0xA433  // The manufacturer of the lens
#define LensModel                   0xA434  // The model name or number of the lens
#define CompositeImage              0xA460  // 0 = Unknown. 1 = Not a composite image. 2 = General composite image. 3 = Composite image captured while shooting.

/* Private IFD (GPS Info IFD) Tags [Exif v3.0, pp.90-91] */
#define GPSLatitudeRef              0x0001  // N = North latitude. S = South latitude.
#define GPSLatitude                 0x0002  // The latitude
#define GPSLongitudeRef             0x0003  // E = East longitude. W = West longitude.
#define GPSLongitude                0x0004  // The longitude
#define GPSAltitudeRef              0x0005  // 0 = Above sea level. 1 = Below se level.
#define GPSAltitude                 0x0006  // The altitude
#define GPSTimeStamp                0x0007  // The GPS time in UTC
#define GPSSpeedRef                 0x000C  // K = Kilometers per hour. M = Miles per hour. N = knots
#define GPSSpeed                    0x000D  // The speed of GPS receiver movement
#define GPSImgDirectionRef          0x0010  // T = True direction. M = Magnetic direction
#define GPSImgDirection             0x0011  // The direction of the image
#define GPSDestBearingRef           0x0017  // T = True direction. M = Magnetic direction
#define GPSDestBearing              0x0018  // The bearing of destination
#define GPSDateStamp                0x001D  // The GPS date in UTC
#define GPSHPositioningError        0x001F  // The horizontal positioning error
