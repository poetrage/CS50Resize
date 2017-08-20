/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])//main function, asking to count arguments and accept arguments as a string
{
    // ensure proper usage ** ensure that the user gives; argv[0]-the name of the file to run; argv[1]-the resize factor; argv[2]-name of the file to resize; argv[3]- name of the new outfile
    if (argc != 4)//changed
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");//standard error to print if user did not give proper information
        return 1;//error code
    }

    // remember filenames
    char *infile = argv[2];//file to resize
    char *outfile = argv[3];//new file with resized image
    
    //change resize factor to an int and define argv[1]
    int refactor= atoi (argv[1]);//added
    
    //ensure that argv[1] is a positive integer less than or equal to 100
    if (refactor<0 || refactor>100)//added
    {
        fprintf(stderr, "Usage: positive integer less than 100\n");//added in case of incorrect usage//standard error to print if user did not give proper information
        return 5;//error code   
    }

    // open input file 
    FILE *inptr = fopen(infile, "r");//open the file as read only
    if (inptr == NULL)//if file is null
    {
        fprintf(stderr, "Could not open %s.\n", infile);//standard error to print if user did not give proper information
        return 2;//error code
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");//open and write file with resized image
    if (outptr == NULL)//if memory resturns null
    {
        fclose(inptr);//close the pointer 
        fprintf(stderr, "Could not create %s.\n", outfile);//standard error to print if user did not give proper information
        return 3;//error code
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;//bf is the file header
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);//read the size of the file header from the inpointer
    
    //define outfile BITMAPFILEHEADER
    BITMAPFILEHEADER bfr=bf;//added, 

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;//bi is the info header
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);//read the size of the info header
    
    //define outfile BITMAPINFOHEADER
    BITMAPINFOHEADER bir=bi;//added
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || //if fileheader is not 0x4d42 or fileheader off bits is not 54 OR infoheader size is not 40 or
        bi.biBitCount != 24 || bi.biCompression != 0)//infoheader bit count is not 24 or infoheader compression is not 0
    {
        fclose(outptr);//close the pointer to outfile
        fclose(inptr);//close the pointer to infile
        fprintf(stderr, "Unsupported file format.\n");//print standar error
        return 4;//error code
    }
    // from Bob, You are writing the outfile headers without reflecting the changes due to resize
    // write outfile's BITMAPFILEHEADER
    //fwrite(&bfr, sizeof(BITMAPFILEHEADER), 1, outptr);//changed

    // write outfile's BITMAPINFOHEADER
    //fwrite(&bir, sizeof(BITMAPINFOHEADER), 1, outptr);//changed
    
    //new width and height
    bir.biWidth=bi.biWidth*refactor;
    bir.biHeight=bi.biHeight*refactor;
    
   

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padresize=(4 - (bir.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;//added
 //  printf("padding = %d padresize = %d\n",padding,padresize);
     
    //memory
    //void * malloc(size_t size);
    
    //size of new image 
    //long offset = bir.biWidth * sizeof(RGBTRIPLE);//stackoverflow help with runtime error
    long offset = bi.biWidth * sizeof(RGBTRIPLE);//Bob the offset should be calced on input file 
    
    //bir.biSizeImage=(offset+padresize)*(bir.biHeight);//bob wrong image size
    bir.biSizeImage=((bi.biWidth * sizeof(RGBTRIPLE)*refactor)+padresize)*abs(bir.biHeight);//bob correct image size
    bfr.bfSize = sizeof(bfr) + sizeof(bir) + bir.biSizeImage;//added
    
     // write outfile's BITMAPFILEHEADER moved by bob to after changes to bir and bfr
    fwrite(&bfr, sizeof(BITMAPFILEHEADER), 1, outptr);//changed

    // write outfile's BITMAPINFOHEADER
    fwrite(&bir, sizeof(BITMAPINFOHEADER), 1, outptr);//changed
    
    
    // iterate over infile's scanlines (height)
    // bob changed to abs bi.biheight from bir
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // each row will be printed out resize factor times
        int rowcount = 0;
        
        while (rowcount < refactor)//while the row count is less than the number of times to resize
        {    
            // iterate over pixels in scanline (width)
            //for (int j = 0; j < abs(bir.biWidth); j++)
            for (int j = 0; j < bi.biWidth; j++) // changed by bob
            {
                // temporary storage
                RGBTRIPLE triple;
                
                //each pixel will be printed out resize factor times                
                int pixcount=0;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                while (pixcount<refactor)//while the pixel count is less than the number of times to resize 
                { 
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    pixcount++; //increase pixel count
                }
            }

            // then add it back (to demonstrate how)

            if (rowcount < (refactor-1))
            {
                //fseek(inptr, offset, SEEK_CUR);  //see below
                 fseek(inptr, -offset, SEEK_CUR);
            }   
                rowcount++;
        
            for (int k = 0; k < padresize; k++)
            {
                fputc(0x00, outptr);
            }
        }    // moved this from before the for k loop to correct padding problem
            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);
    }    
    //free memory
    //void free(void * ptr);
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
