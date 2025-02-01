#include <cstdint>
#include <iostream>
#include <fstream>

#pragma pack(push,1)

struct _bmpfileheader
{
    uint16_t signature; 
    uint32_t bmpfilesize;
    uint16_t reserved1{0};
    uint16_t reserved2{0};

    uint32_t pixeloffset;
};

struct _bitmapheader
{
    uint32_t headersize;
    uint32_t width;
    uint32_t height;
    uint16_t colorplanes;
    uint16_t bitsperpixel;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t hres;
    uint32_t vres;
    uint32_t ncolors;
    uint32_t impcolors;
};

struct _pixeldata
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    uint8_t alpha;
};


struct bmpimage
{
    _bmpfileheader* fileheader;
    _bitmapheader* imageheader;

    _pixeldata* data;
    bmpimage()
    {
      fileheader = new _bmpfileheader;
      imageheader = new _bitmapheader;
    }

    ~bmpimage()
    {
        delete fileheader;
        delete imageheader;
        delete[] data;
    }
};

#pragma pack(pop)

bool Initialize();
void Shutdown();


uint16_t GetSignature();
uint32_t GetFileSize();
uint32_t GetPixelOffset();
uint32_t GetWidth();
uint32_t GetHeight();
uint32_t GetBitsPerPixel();



void ExportBMP();
void LogDebugInfo();


