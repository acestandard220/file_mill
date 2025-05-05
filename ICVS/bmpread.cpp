#include "includes/bmpread.h"
 #include <cstdint>
 
 #include <fstream>
 #include <iostream>
 #include <memory>
 #include <ostream>
 #include <string>
 #include <vector>
 
 bmpimage* image;
 
 bool Initialize()
 {
     image = new bmpimage;
     std::string v;
     std::cout<<"Enter file: \n";
     std::cin>>v;
     std::ifstream file(v,std::ios::binary);
     if(!file.is_open())
     {
         std::cout<<"Could not open file\n";
         return false;
     }
     else{
         std::cout<<"File opened\n";
     }
     file.read(reinterpret_cast<char*>(image->fileheader),sizeof(_bmpfileheader));
     file.read(reinterpret_cast<char*>(image->imageheader),sizeof(_bitmapheader));
     file.seekg(image->fileheader->pixeloffset, std::ios::beg);
    
     int nChannels = image->imageheader->bitsperpixel/8;
     int rowSize = ((image->imageheader->width * nChannels +3)/4)*4;
     
 
     image->data = new _pixeldata[image->imageheader->height * image->imageheader->width];
   
     
     std::vector<uint8_t> rowBuffer(rowSize);
     
     for(int i = 0; i < image->imageheader->height; i++)
     {
         file.read(reinterpret_cast<char*>(rowBuffer.data()), rowSize);
         
         for(int j = 0; j < image->imageheader->width; j++)
         {
             // BMP stores colors in BGR format
             int pixelIndex = (image->imageheader->height - 1- i) * image->imageheader->width +j;
             int bufferIndex = j * nChannels;
             
             image->data[pixelIndex].blue = rowBuffer[bufferIndex];
             image->data[pixelIndex].green = rowBuffer[bufferIndex + 1];
             image->data[pixelIndex].red = rowBuffer[bufferIndex + 2];
             
             if(nChannels == 4) {
                 image->data[pixelIndex].alpha = rowBuffer[bufferIndex + 3];
             } else {
                 image->data[pixelIndex].alpha = 255;  // Full opacity for 24-bit BMPs
             }
         }
     }
 
     file.close();
     ExportBMP();
     std::cout<<"HI\n";
     return true;
 }
 
 void Shutdown()
 {
     return;
 }
 
 uint16_t GetSignature()
 {
     return image->fileheader->signature;
 }
 uint32_t GetFileSize()
 {
     return image->fileheader->bmpfilesize;
 }
 uint32_t GetPixelOffset()
 {
     return image->fileheader->pixeloffset;
 }
 
 uint32_t GetWidth()
 {
     return image->imageheader->width;
 }
 
 uint32_t GetHeight()
 {
     return image->imageheader->height;
 }
 
 uint32_t GetBitsPerPixel()
 {
     return image->imageheader->bitsperpixel;
 }
 
 void ExportBMP()
 {
   std::ofstream file("outfile.ppm");
   file<<"P3\n";
   file<<image->imageheader->width<<" "<<image->imageheader->height<<std::endl;
   file<<"255\n";
 
   for (int i = 0; i < image->imageheader->height; i++)
     {
         for (int j = 0; j < image->imageheader->width; j++)
         {
             _pixeldata* pixel = &image->data[i * image->imageheader->width + j];
             file << static_cast<int>(pixel->red) << " "
                  << static_cast<int>(pixel->green) << " "
                  << static_cast<int>(pixel->blue) << "\n";
         }
     }
     
   
 
   file.close();
   return;
 }
 
 void LogDebugInfo()
 {
           
     std::cout<<"Signature: "<<image->fileheader->signature<<std::endl;
     std::cout<<"File Size: "<<image->fileheader->bmpfilesize<<std::endl;
     std::cout<<"R1: "<<image->fileheader->reserved1<<std::endl;
     std::cout<<"R2: "<<image->fileheader->reserved2<<std::endl;
     std::cout<<"Pixel Offset: "<<image->fileheader->pixeloffset<<std::endl;
     std::cout<<"Header Size: "<<image->imageheader->headersize<<std::endl;
     std::cout<<"Width: "<<image->imageheader->width <<" Height: "<<image->imageheader->height<<std::endl;
     std::cout<<"Color Planes: "<<image->imageheader->colorplanes<<std::endl;
     std::cout<<"BitPerPixel: "<<image->imageheader->bitsperpixel<<std::endl;
 
 
 
     std::cout<<"PIXEL 1: R: "<<static_cast<int>(image->data[1].red)<<" G: "<<static_cast<int>(image->data[1].green)<<" B: "<<static_cast<int>(image->data[1].blue)<<std::endl;
 
 
 
 
 
 
   return;
 }
