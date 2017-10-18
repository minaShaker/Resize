/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    
    
    
    
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy ratio infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // storing the ratio
    float ratio = 0;
    
    //checking the validity of ratio
    if (1 != sscanf(argv[1], "%f", &ratio))
    {
        printf("please insert a number\nUsage: ./copy ratio infile outfile\n");
        return 5;
    }
    if (ratio <= 0)
    {
        printf("the ratio must be higher than 0 \n");
        return 6;
    }
    
    //storing some info about the first file
    int fWidth= bi.biWidth;
    int fPadding = 0;
    int fHeight = bi.biHeight;
    fPadding = (4 - (fWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //rewriting the info header and the file header
    bi.biWidth *= ratio;
    bi.biHeight *= ratio;
    
    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    bi.biSizeImage = (bi.biWidth*sizeof(RGBTRIPLE) + padding)*(abs(bi.biHeight));
    bf.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bi.biSizeImage;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    if (ratio >= 1)
    {
        // iterate over infile's scanlines
        for (int i = 0, biHeight = abs(fHeight); i < biHeight; i++)
        {
            
            //dublicating the same scanline respectively to the ratio
            for (int z =0 ; z<ratio ; z++)
            {
                
                //puting the inptr file pointer in the right place to start copying
                int t = fWidth*sizeof(RGBTRIPLE)+fPadding;
                int c =sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER);
                fseek(inptr,  c + (i * t)  ,SEEK_SET);
            
                // iterate over pixels in scanline to copy it
                for (int j = 0; j < fWidth; j++)
                {
                
                    // temporary storage
                    RGBTRIPLE triple;

                    // read RGB triple from infile
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
    
                    // write RGB triple to outfile , and repeated the same time as ratio
                    for(int x =0 ; x< ratio ; x++)
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                // skip over padding, if any
                fseek(inptr, fPadding, SEEK_CUR);

            
                // then add it back (to demonstrate how)
                for (int k = 0; k < padding; k++)
                    fputc(0x00, outptr);
            }
        }
    }else if (ratio < 1 & ratio > 0){
        // iterate over infile's scanlines
        for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
        {
            // iterate over pixels in scanline

            float c = (pow(ratio,-1)-1);
            for (int j = 0; j < bi.biWidth; j++)
            {
                
                fseek(inptr, c*sizeof(RGBTRIPLE) ,SEEK_CUR);                
            
                // temporary storage
                RGBTRIPLE triple;
                    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
            // skip over padding, if any
            fseek(inptr , fPadding , SEEK_CUR);
            
            for (int q = 0 ; q < c ; q++)
            {
                fseek(inptr , fWidth * sizeof(RGBTRIPLE) , SEEK_CUR);
                fseek(inptr, fPadding ,SEEK_CUR);
                
            }
            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
             fputc(0x00, outptr);
        }
    }
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
