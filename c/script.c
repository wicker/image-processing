// Image convolution
// Jenner Hanni <jeh.wicker.gmail.com>
// 3-clause BSD license
//
// Script version that takes more inputs from the command line
// Allows custom output file name.
// Does not prompt the user, just takes inputs.

// to do
// update user inputs
// take coefficients for all sizes
// allow inputs for 1D separated, |*--

#define ARGS 9

#include <stdio.h>
#include <stdlib.h>

#define TRUE 0
#define FALSE 1

int exit_program(int);
void convolution(int *inpixels,int *params,int *outpixels);
void fill_kernel(int op, int *params);

// main()
// 1. receives args and error checks inputs
// 2. opens the text file and saves input pixels
// 3. polls user for convolution type and kernel size
// 4. performs the convolution
// 5. outputs the pixels to a text file
// 6. uses the output text file as an input and repeats steps 2-5 
//    if more than one operation in a row

int main(int argc, char *argv[]) {

  // check that we received the correct number of arguments
  if (argc != 9) {
    printf("Input should look like this:\n");
    printf("./script input/input-brokentop.txt 240 320 <op> <kernel> <op> <kernel> output.txt\n");
    printf("where <op> is blur (1), max-viz (2), and custom (3) to be entered by the user.\n");
    printf("Kernel has to be N in NxN and it must be an odd number between 1 and 17.\n");
    exit_program(1);
    return -1;
  }

  FILE * fileout;

  // determine if the input file is even valid 
  FILE * filein = fopen(argv[1],"r");
  if (filein == NULL) {
    printf("File did not open properly! Does it exist?\n");
    exit_program(1);
    return -1;
  }
  
  // get width and height, then determine if they're in range
  int width = atoi(argv[2]);
  int height = atoi(argv[3]); 
  if (width <= 0 || width > 900 || height <= 0 || height > 900) {
    printf("Width and height values must be in range 0-900.\n");
    exit_program(1);
    return -1;
  }

  // create inpixels and outpixels arrays from the input height and width
  int inpixels[height*width];
  int outpixels[height*width];

  int reps,i,params[300],firstflag,secondflag,whichop;
  int firstop = atoi(argv[4]);
  int firstN = atoi(argv[5]);
  int secondop = atoi(argv[6]);
  int secondN = atoi(argv[7]);

  // set operation order flags
  if (firstop == 0) firstflag = FALSE;
  else firstflag = TRUE;
  if (secondop == 0) secondflag = FALSE;
  else secondflag = TRUE;

  if (firstflag == TRUE && secondflag == TRUE)
    whichop = 2;
  else if (firstflag == TRUE && secondflag == FALSE)
    whichop = 1;
  else if (firstflag == FALSE)
    exit_program(5);

  // error check the inputs for each operation and return errors to user if invalid
  if (firstflag == TRUE) {
    if (firstop < 1 || firstop > 3)
      exit_program(2);
    else if (firstN %2 == 0)
      exit_program(3);
    else if (firstN < 1 || firstN > 17)
      exit_program(4);
  }
  else if (secondflag == TRUE) {
    if (secondop < 1 || secondop > 3)
      exit_program(2);
    else if (secondN %2 == 0)
      exit_program(3);
    else if (secondN < 1 || secondN > 17)
      exit_program(4);
  }

  // now for each operation, 
  // prep params with its inputs, including reading appropriate input file
  // then perform the convolution and produce the output file
  if (whichop == 2) {
    // store parameter values
    params[0] = secondop;
    params[1] = height;
    params[2] = width;
    params[3] = height*width;
    params[4] = secondN;
    params[5] = secondN*secondN;

    // read in input pixels
    filein = fopen(argv[8],"r");    
    for (i = 0; i < params[3]; i++) 
      fscanf(filein, "%d ", &inpixels[i]);
    fclose(filein);

    // read in coefficients from user and perform convolution
    fill_kernel(params[0],params);
    convolution(inpixels,params,outpixels);

    // prep output file
    fileout = fopen(argv[8],"w");
    if (fileout == NULL) {
      printf("File did not open properly! Does it exist?\n");
      exit_program(1);
    }  
    for (i = 0; i < params[3]; i++) 
      fprintf(fileout, "%d ", outpixels[i]);
    fclose(fileout);

    whichop--;
  }
  if (whichop == 1) {
    // store parameter values
    params[0] = firstop;
    params[1] = height;
    params[2] = width;
    params[3] = height*width;
    params[4] = firstN;
    params[5] = firstN*firstN;

    // read in input pixels 
    filein = fopen(argv[1],"r");
    for (i = 0; i < params[3]; i++) 
      fscanf(filein, "%d ", &inpixels[i]);
    fclose(filein);

    // read in coefficients from user and perform convolution
    fill_kernel(params[0],params);
    convolution(inpixels,params,outpixels);

    // prep output file
    fileout = fopen(argv[8],"w");
    if (fileout == NULL) {
      printf("File did not open properly! Does it exist?\n");
      exit_program(1);
    }  
    for (i = 0; i < params[3]; i++) 
      fprintf(fileout, "%d ", outpixels[i]);
    fclose(fileout);
  }

  exit_program(0);
  return 0;
}


// fill kernel()
// description
void fill_kernel(int op, int *params) {

  int i;

  // blur
  if (op == 1) {
    for (i = 6; i < 6+params[5]; i++)
      params[i] = 1;
  }
  // separable coeffs
  else if (op == 2) {
    exit_program(6);
  }
  // custom coeffs 
  else if (op == 3) {
    printf("Please enter the coefficients for your custom operation\n");
    printf("from left to right with an enter after each.\n",params[5]);
    for (i = 6; i < 6+params[5]; i++) 
      scanf("%d",&params[i]);
  }
}

// convolution()
// uses the given params array to iterate through each image pixel
// and calculate a new output based on the pixel's nearby neighbors
void convolution(int *inpixels,int *params,int *outpixels) {

  int t = 0;
  int i,irow,icol,krow,kcol,kx,ky,kcount,discardpix;
  int irows = params[1];
  int icols = params[2]; 
  int krows = params[4];
  int kcols = params[4];

  // first, orient the source image's pixel of interest with irow and icol
  // then use the params array which contains the kernel matrix coefficients to the
  // image pixels around our image pixel to calculate output value
  for (i = 0,irow = 1; irow <= irows; irow++) {
    for (icol = 1; icol <= icols; icol++,i++) {

      // reset the output for this pixel
      t = 0; 
      discardpix = 0;

      for (kcount = 6,krow = 1,kx = -kcols/2; krow <= krows; krow++,kx++) {
        for (kcol = 1,ky = -kcols/2; kcol <= kcols; kcol++,kcount++,ky++) {
          if (kx + irow <= 0 || ky + icol <= 0 || kx + irow > irows || ky + icol > icols)
            discardpix++;
          else
              t += params[kcount]*inpixels[i+kx*icols+ky];

        }
      }

      // if this is a blur coefficients=1 operation,
      // get the average of the summed result of the kernel coefficients over the input pixels
      // discardpix is the number of pixels that were off the edge of the image and unusable
      if (params[0] == 1)
        t = t/(params[5]-discardpix);
      else 
        t = t;

      if (t < 0)
        outpixels[i] = 0;
      else if (t > 255)
        outpixels[i] = 255;
      else
        outpixels[i] = t;
    }
  }
}

int exit_program(int result) {
  switch (result) {
    case 0:
      printf("Program successfully completed.\n"); 
      break;
    case 1:
      printf("Something's gone wrong but no idea what it was, sorry!.\n");
      break;
    case 2:
      printf("Empty or illegal entry for operation choice.\n");
      break;
    case 3:
      printf("Kernel matrix N in NxN must be an odd number.\n");
      break;
    case 4:
      printf("N in kernel matrix NxN must be between 1 and 17.\n");
      break;
    case 5:
      printf("First operation must be a valid operation.\n");
    default:
      return 0;
      break;
  }
  printf("Program will now exit.\n");
  exit(0);
}
