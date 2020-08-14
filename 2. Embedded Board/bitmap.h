#ifndef __BITMAP_H__
#define __BITMAP_H__

// BMP File Structure (windows version 3)

// File Header
typedef struct {
//    unsigned char   bfType;         // 2 byte
    unsigned int    bfSize;         // 4 byte
    unsigned short  bfReserved1;    // 2 byte
    unsigned short  bfReserved2;    // 2 byte
    unsigned int    bfOffBits;      // 4 byte 
} __attribute__((packed)) BITMAPFILEHEADER;


typedef struct {
    unsigned short  bfType;         // 2 byte
    unsigned int    bfSize;         // 4 byte
    unsigned short  bfReserved1;    // 2 byte
    unsigned short  bfReserved2;    // 2 byte
    unsigned int    bfOffBits;      // 4 byte
} __attribute__((packed)) BITMAPFILEHEADER_B;

// __attribute__((packed)) on non-Intel arch may cause some unexpected error, plz be informed.

// Image Header


typedef struct {
    unsigned int    biSize;             // 4 byte
    int    biWidth;            // 4 byte
    int    biHeight;           // 4 byte
    unsigned short  biPlanes;           // 2 byte
    unsigned short  biBitCount;         // 2 byte
    unsigned int    biCompression;      // 4 byte
    unsigned int    biSizeImage;        // 4 byte
    int    biXPelsPerMeter;    // 4 byte
    int    biYPelsPerMeter;    // 4 byte
    unsigned int    biClrUsed;          // 4 byte
    unsigned int    biClrImportant;     // 4 byte
} __attribute__((packed)) BITMAPINFOHEADER;
// Color Table
typedef struct {
// windows version 3
    unsigned char   rgbBlue;        // 1 byte
    unsigned char   rgbGreen;       // 1 byte
    unsigned char   rgbRed;         // 1 byte
    unsigned char   rgbReserved;    // 1 byte
} RGBQUAD;

// Pixel Data
typedef struct {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

#endif
