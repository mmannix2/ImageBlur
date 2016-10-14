#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IDENTIFIER "P3"

#define DEBUG

struct Pixel {
    unsigned char red;
    unsigned char blue;
    unsigned char green;
};

struct Image {
    int width;
    int height;
    struct Pixel* pixels;
};

/*  Populates an Image with data from the given filename
    Returns 0 on success.
    Returns -1 if filename could not be opened.
    Returns 1 if filename does not follow .ppm file format.
    */
int read_file(const char* filename, struct Image* image) {
    FILE* img_file = fopen(filename, "r");
    char identifier[2];
    int width, height;
    unsigned char* temp_r = malloc(sizeof(unsigned char));
    unsigned char* temp_g = malloc(sizeof(unsigned char)); 
    unsigned char* temp_b = malloc(sizeof(unsigned char));
    
    if(img_file != NULL) {
        //Read identifier
        fscanf(img_file, "%s", identifier);
        //Read width
        fscanf(img_file, "%d", &width);
        //Read height
        fscanf(img_file, "%d", &height);
        //Read max color value
        fscanf(img_file, "%hhu", temp_r);
        
        //DEBUG
        #ifdef DEBUG
        printf("identifier: %s\n", identifier);
        printf("width: %d, height: %d\n", width, height);
        printf("Max color value: %d\n", *temp_r);
        #endif
        
        //Check identifier, height, width, and max color value
        if( strcmp(identifier, IDENTIFIER) != 0 ||
            width <= 0 ||
            height <= 0 ||
            *temp_r != 255) {
            exit(1);
        }
        
        //Allocate image->pixels
        image->width = width;
        image->height = height;
        image->pixels = (struct Pixel*)malloc(sizeof(struct Pixel*) * (width * height));
        
        //Read in pixels
        #ifdef DEBUG
        printf("Reading in %d pixels... ", width * height); 
        #endif
        for(int i = 0; i < (width * height); i++) {
            //Create a Pixel
            struct Pixel* pixel = malloc(sizeof(struct Pixel*));

            //Read color data
            fscanf(img_file, "%hhu", temp_r);
            fscanf(img_file, "%hhu", temp_g);
            fscanf(img_file, "%hhu", temp_b);
            
            //Store color data into pixels
            pixel->red = *temp_r;
            pixel->green = *temp_g;
            pixel->blue = *temp_b;
            
            //Add pixel to the array of pixels
            image->pixels[i] = *pixel; 
        }
        #ifdef DEBUG
        printf("done.\n"); 
        #endif

        //Close the file
        fclose(img_file);
        return 0;
    }
    else {
        return -1;
    }
}

int write_file( const char* filename, struct Image* image) {
    FILE* img_file = fopen(filename, "w");
    if(img_file != NULL) {
        //Write IDENTIFIER, width, height, and max color value
        fprintf(img_file, "%s\n", IDENTIFIER);
        fprintf(img_file, "%d\n", image->width);
        fprintf(img_file, "%d\n", image->height);
        fprintf(img_file, "%d\n", 255);
        
        //Write pixels
        #ifdef DEBUG
        printf("Writing %d pixels: \n", image->width * image->height); 
        #endif
        for(int i = 0; i < image->width * image->height; i++) {
            fprintf(img_file, "%d %d %d   ",
                image->pixels[i].red,
                image->pixels[i].green,
                image->pixels[i].blue );
        }
        fprintf(img_file, "\n");
        #ifdef DEBUG
        printf("done.\n"); 
        #endif

        fclose(img_file);
    } 
    return 0;
}

int main(int argc, char** argv) {
    //Variables
    struct Image image;

    //Check arguments
    if(argc != 3) {
        //Bad number of arguments, abort.
        printf("Usage: %s input_file.ppm output_file.ppm\n", argv[0]);
        exit(-1);
    }
    
    //Read in pixel data
    read_file(argv[1], &image);
    
    //Blur the image
    if(image.pixels != NULL) {
        for(int row = 0; row < image.height; row++) {
            for(int col = 0; col < image.width; col++) {
                int i = (image.width * row + col);
                #ifdef DEBUG
                printf("[%d] %d %d %d\n",
                        i,
                        image.pixels[i].red,
                        image.pixels[i].green,
                        image.pixels[i].blue );
                #endif
                
                struct Pixel* pixel = malloc(sizeof(struct Pixel*));
                pixel->red = image.pixels[i].red / 2;
                pixel->green = image.pixels[i].green / 2;
                pixel->blue = image.pixels[i].blue / 2;
                
                image.pixels[i] = *pixel;

                #ifdef DEBUG
                printf("[%d] %d %d %d\n",
                        i,
                        image.pixels[i].red,
                        image.pixels[i].green,
                        image.pixels[i].blue );
                #endif
            }
        }
    }
    //Write pixel data
    write_file(argv[2], &image);
    
    return 0;
}
