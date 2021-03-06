#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define HEADER 0x0000
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define DATA 0x0002
#define PIXEL 0x0036

const int BYTES_PER_PIXEL = 3;
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);
unsigned long binaryToDecimal(char *binary, int length);
char* textToBinary(char* text);


typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

int read(byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel) {

    FILE *bmp;
    bmp = fopen("bitmapImage.bmp", "r+");
    byte header;
    fseek(bmp, HEADER, SEEK_SET);
    fread(&header, 2, 1, bmp);
    byte pixel;
    fseek(bmp, PIXEL, SEEK_SET);
    fread(&pixel, 3, 1, bmp);
    fseek(bmp, DATA, SEEK_SET);
    int32 data;
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
    printf(" Disp.: %d\n", data - dataOffset);
    printf(" Header: %c\n", header);
    printf(" BitsPP: %d\n", bitsPerPixel);
    printf(" Width: %d\n", *width);
    printf(" Pixel: %d\n\n", pixel);

    int paddedRowSize = (int) (4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
    printf("pad: %d\n", paddedRowSize);
    int unpaddedRowSize = (*width)*(*bytesPerPixel);
    int totalSize = unpaddedRowSize*(*height);
    *pixels = (byte*)malloc(totalSize);
    int i = 0;
    byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
    printf("Pix0: %d\n", *currentRowPointer);

    char *text = "Alguma coisa";
    char *textBin = textToBinary(text);
    printf("textBin: %s\n", textBin);

    int textLength = strlen(textBin);
    int end = textLength;
    int disponivel = data - dataOffset;

    char msg[textLength + 5];

    if(textLength < disponivel) {
        while(end != 0) {
        fseek(bmp, PIXEL+i, SEEK_SET);
        fread(&pixel, 3, 1, bmp);
        printf("Pixel[%d]: %d\n", i, pixel);

        if(pixel%2 == 0) {
            pixel += textBin[i] - '0';
            printf("Pixel2[%d]: %d\n", i, pixel);
            fwrite(&pixel, 3, 1, bmp);
        }

    /*
        //Recuperar msg

        sprintf(msg, "%d", pixel%2);
        printf("Msg: %s\n", msg);
    */
        end--;
        i++;
        }
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
            image[i][j][2] = (unsigned char) ( i * 150 / height );             //red
            image[i][j][1] = (unsigned char) ( j * 255 / width );              //green
            image[i][j][0] = (unsigned char) ( (i+j) * 100 / (height+width) ); //blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Image generated!!");
}

unsigned long binaryToDecimal(char *binary, int length) {
    unsigned long decimal = 0;
    unsigned long weight = 1;
    binary += length - 1;

    for(int i = 0; i < length; i++, binary--) {
        if(*binary == '1') {
            decimal += weight;
        }
        weight *= 2;
    }
    return decimal;
}

char* textToBinary(char* text) {
    if(text == NULL) 
        return 0;
    size_t len = strlen(text);
    char *binary = malloc(len*8 + 1);
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = text[i];
        for(int j = 7; j >= 0; --j) {
            if(ch & (1 << j)) {
                strcat(binary, "1");
            } else {
                strcat(binary, "0");
            }
        }
    }

    return binary;
}

void BinaryToText(char *binary, int binaryLength, char *text, int symbolCount) {
    for(int i = 0; i < binaryLength; i+= 8, binary += 8) {
        char *byte = binary;
        byte[8] = '\0';
        *text++ = binaryToDecimal(byte, 8);
        // printf("Decimal: %ld\n", binaryToDecimal(byte, 8));
    }
    text -= symbolCount;
}

int BinaryToInt(const char *s) {
    // return (int) strtol(s, NULL, 2);
    int bLen =  strlen(s);
    char input[bLen + 1];
    input[bLen+1] = '\0';
    int c, i = 0;
    for(i; i < bLen; i++) {
        if((c = s[i]) != EOF && '\n' != c ){
            input[i] = (char)c;
        } else {
            break;
        }
    }

    int result = 0;

    for(int j = 0; j < i; j++) {
        result <<= 1;
        if(input[j] == '1'){
            result |= 1;
        }
    }

    printf("Bin: %s -> Int: %d\n", s, result);

    return 0;
}

int main(void) {
    byte *pixels;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    read(&pixels, &width, &height, &bytesPerPixel);

    char *bin = textToBinary("texto");
    printf("Bin: %s\n", bin);

    char *text;
    int binaryLength, symbolCount;

    binaryLength = strlen(bin);
    symbolCount = binaryLength / 8 + 1;
    text = malloc(symbolCount +1);
    BinaryToText(bin, binaryLength, text, symbolCount);
    printf("Text: %s\n", text);

    BinaryToInt(bin);
    printf("Num: %d \n", bin[1] - '0');

    // gerarBMP();

    return 0;
}

