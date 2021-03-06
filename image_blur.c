#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <omp.h>

#define IDENTIFIER "P3"
#define BLUR_AMOUNT 50
#define PIXELS_PER_LINE 6 //The number of pixels to write to file before \n

//#define DEBUG
#define TIME

// Holds the data for one pixel
struct Pixel {
    unsigned char red;
    unsigned char blue;
    unsigned char green;
};

// Holds data for the entire image
struct Image {
    int width;
    int height;
    struct Pixel* pixels;
};

/*  Populates an Image with data from the given filename
 *  Returns 0 on success.
 *  Returns -1 if filename could not be opened.
 *  Returns 1 if filename does not follow .ppm file format.
 */
int Image_read(struct Image* image, const char* filename) {
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
            return(1);
        }
        
        //Allocate image->pixels
        image->width = width;
        image->height = height;
        image->pixels = (struct Pixel*)malloc(sizeof(struct Pixel*) *
            (width * height));
        
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

/*  Writes a struct Image to the given filename
 *  Returns 0 on success.
 *  Returns -1 if writing to filename failed.
 */
int Image_write(struct Image* image, const char* filename) {
    FILE* img_file = fopen(filename, "w");
    if(img_file != NULL) {
        //Write IDENTIFIER, width, height, and max color value
        fprintf(img_file, "%s\n", IDENTIFIER);
        fprintf(img_file, "%d %d\n", image->width, image->height);
        fprintf(img_file, "%d\n", 255);
        
        //Write pixels
        #ifdef DEBUG
        printf("Writing %d pixels... ", image->width * image->height); 
        #endif
        for(int i = 0; i < image->width * image->height; i++) {
            fprintf(img_file, "%d %d %d ",
                image->pixels[i].red,
                image->pixels[i].green,
                image->pixels[i].blue );
            if(i % PIXELS_PER_LINE  == 0) {
                fprintf(img_file, "\n");
            }
        }
        #ifdef DEBUG
        printf("done.\n"); 
        #endif

        fclose(img_file);
        return 0;
    } 
    else {
        //Somthing went wrong :(
        return -1;
    }
}

/*  Blurs each struct Pixel in given struct Image
 *  image->pixels must not be null.
 *  Returns 0 on success.
 *  Returns -1 if image is NULL.
 */
int Image_blur(struct Image* image) {
    if(image->pixels == NULL) {
        return -1;
    }
    else {
        #ifdef THREADS
        #pragma omp parallel for num_threads(THREADS)
        #endif
        for(int row = 0; row < image->height; row++) {
            for(int col = 0; col < image->width; col++) {
                int i = (image->width * row + col);
                
                double red, green, blue;
                //Halve the red, green, and blue values.
                red = image->pixels[i].red / 2;
                green = image->pixels[i].green / 2;
                blue = image->pixels[i].blue / 2;
                
                //Find the number of pixels to the right to use
                int blur = BLUR_AMOUNT;
                if(col + blur >= image->width) {
                    blur = image->width - col - 1;
                }
                
                //Blur the pixels
                for(int j = 1; j <= blur; j++) {
                    red += (image->pixels[i+j].red * (0.5 / blur));
                    green += (image->pixels[i+j].green * (0.5 / blur));
                    blue += (image->pixels[i+j].blue * (0.5 / blur));
                }
                
                #ifdef VERBOSE
                printf("[%d] %d %d %d -> %.02f %.02f %.02f\n",
                        i,
                        image->pixels[i].red,
                        image->pixels[i].green,
                        image->pixels[i].blue,
                        red,
                        green,
                        blue );
                #endif
                
                //Update colors
                image->pixels[i].red = (unsigned char) red;
                image->pixels[i].green = (unsigned char) green;
                image->pixels[i].blue = (unsigned char) blue;
            }
        }
        return 0;
    }
}

int main(int argc, char** argv) {
    //Variables
    struct Image image;
    int status;
    #ifdef TIME
    time_t start, current;
    #endif

    //Check arguments
    if(argc != 3) {
        //Bad number of arguments, abort.
        printf("Usage: %s input_file.ppm output_file.ppm\n", argv[0]);
        exit(-1);
    }
    
    #ifdef TIME 
    time(&start);
    #endif
    
    //Read in pixel data from argv[1]
    status = Image_read(&image, argv[1]);
    
    #ifdef TIME
    time(&current);
    printf("Time to read file: \t%fs.\n", difftime(current, start)); 
    #endif

    if(status == 0) {
        #ifdef TIME 
        time(&start);
        #endif
        
        //Blur the image
        Image_blur(&image);
        
        #ifdef TIME
        time(&current);
        printf("Time to blur Image: \t%fs.\n", difftime(current, start)); 
        #endif
        
        #ifdef TIME
        time(&start);
        #endif

        //Write pixel data to argv[2]
        status = Image_write(&image, argv[2]);
        
        #ifdef TIME
        printf("Time to write file:\t%fs.\n", difftime(time(NULL), start)); 
        #endif
        
        if( status == 0) {
            return 0;
        }
        else {
            printf("ERROR: Unable to write to %s.\n", argv[2]);
            return -1;
        }
    }
    else {
        if(status == -1) {
            printf("ERROR: Unable to open %s.\n", argv[1]);
        }
        else {
            printf("ERROR: %s does not follow .ppm format.\n", argv[1]);
        }
        return -1;
    }
}
