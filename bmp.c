#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEADER 0x0000
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define DATA 0x0002

const int BYTES_PER_PIXEL = 3;
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);


typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

int read(byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel) {

    FILE *bmp;
    bmp = fopen("bitmapImage.bmp", "rb");
    byte header;
    fseek(bmp, HEADER, SEEK_SET);
    fread(&header, 2, 1, bmp);
    int32 data;
    fseek(bmp, DATA, SEEK_SET);
    fread(&data, 4, 1, bmp);
    int32 dataOffset;
    fseek(bmp, DATA_OFFSET_OFFSET, SEEK_SET);
    fread(&dataOffset, 4, 1, bmp);
    fseek(bmp, WIDTH_OFFSET, SEEK_SET);
    fread(width, 4, 1, bmp);
    fseek(bmp, HEIGHT_OFFSET, SEEK_SET);
    fread(height, 4, 1, bmp);
    int16 bitsPerPixel;
    fseek(bmp, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bitsPerPixel, 2, 1, bmp);
    printf("\n Size: %d\n", data);
    printf(" Offset: %d\n", dataOffset);
    printf(" Header: %c\n", header);
    printf(" BitsPP: %d\n", bitsPerPixel);
    printf(" Width: %d\n\n", *width);

    int paddedRowSize = (int) (4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
    int unpaddedRowSize = (*width)*(*bytesPerPixel);
    int totalSize = unpaddedRowSize*(*height);
    *pixels = (byte*)malloc(totalSize);
    int i = 0;
    byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
    for(i = 0; i < *height; i++) {
        fseek(bmp, dataOffset+(i*paddedRowSize), SEEK_SET);
        fread(currentRowPointer, 1, unpaddedRowSize, bmp);
        currentRowPointer -= unpaddedRowSize;
    }
    fclose(bmp);

    return 0;
}

void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}

int gerarBMP() {
    int height = 10;
    int width = 15;
    unsigned char image[height][width][BYTES_PER_PIXEL];
    char* imageFileName = (char*) "bitmapImage.bmp";

    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            image[i][j][2] = (unsigned char) ( i * 0 / height );             //red
            image[i][j][1] = (unsigned char) ( j * 255 / width );              //green
            image[i][j][0] = (unsigned char) ( (i+j) * 0 / (height+width) ); //blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Image generated!!");
}

int main(void) {
    byte *pixels;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    read(&pixels, &width, &height, &bytesPerPixel);

    //gerarBMP();
}