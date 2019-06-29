/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "casc",              /* Team name */

    "Harry Q. Bovik",     /* First member full name */
    "bovik@nowhere.edu",  /* First member email address */

    "crazysimpson",                   /* Second member full name (leave blank if none) */
    "hdzyd@foxmail.com"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

char my_rotate_descr[] = "my_rotate: more perfect";

void my_rotate(int dim, pixel *src, pixel *dst)
{
	int i, j, k, srci, disti;
	for(i=0; i< dim; i+=16){
		for(j=0; j<dim ; j+=16)
		{
			for(k=i; k<i+16; k++){
				srci = RIDX(k,j,dim);
				disti = RIDX(dim-1-j,k, dim);
				dst[disti] = src[srci];
				dst[disti-dim] = src[srci+1];
				dst[disti-2*dim] = src[srci+2];
				dst[disti-3*dim] = src[srci+3];
				dst[disti-4*dim] = src[srci+4];
				dst[disti-5*dim] = src[srci+5];
				dst[disti-6*dim] = src[srci+6];
				dst[disti-7*dim] = src[srci+7];
				dst[disti-8*dim] = src[srci+8];
				dst[disti-9*dim] = src[srci+9];
				dst[disti-10*dim] = src[srci+10];
				dst[disti-11*dim] = src[srci+11];
				dst[disti-12*dim] = src[srci+12];
				dst[disti-13*dim] = src[srci+13];
				dst[disti-14*dim] = src[srci+14];
				dst[disti-15*dim] = src[srci+15];
			}	
		}
	}		
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
	

char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    naive_rotate(dim, src, dst);
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&my_rotate,my_rotate_descr);   
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

static pixel avg_my(int dim, int i, int j, pixel *src)
{
	int  p;
	pixel_sum sum;
	pixel current_pixel;
	initialize_pixel_sum(&sum);
	p = RIDX(i,j,dim);	

	if(i-1>=0){
		accumulate_sum(&sum, src[p-dim]);
		if(j-1>=0)
			accumulate_sum(&sum, src[p-dim-1]);
		if(j+1<dim)
			accumulate_sum(&sum, src[p-dim+1]);	
	}

	accumulate_sum(&sum, src[p]);
	if(j+1<dim)
		accumulate_sum(&sum, src[p+1]);
	if(j-1>=0)
		accumulate_sum(&sum, src[p-1]);

	if(i+1<dim){
		accumulate_sum(&sum, src[p+dim]);
		if(j-1>=0)
			accumulate_sum(&sum, src[p+dim-1]);
		if(j+1<dim)
			accumulate_sum(&sum, src[p+dim+1]);
	}
	assign_sum_to_pixel(&current_pixel, sum);
	return current_pixel;
	
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    	int i, j, p, red=0, blue=0, green=0, num=0;
	dst[0].red = (unsigned short)(((int)src[0].red+(int)src[1].red+(int)src[dim].red+(int)src[dim+1].red)/4);
	dst[0].blue = (unsigned short)(((int)src[0].blue+(int)src[1].blue+(int)src[dim].blue+(int)src[dim+1].blue)/4);
	dst[0].green = (unsigned short)(((int)src[0].green+(int)src[1].green+(int)src[dim].green+(int)src[dim+1].green)/4);
	
	dst[dim*dim-1].red = (unsigned short)(((int)src[dim*dim-1].red+(int)src[dim*dim-2].red+(int)src[dim*dim-dim-1].red+(int)src[dim*dim-dim-2].red)/4);
	dst[dim*dim-1].blue = (unsigned short)(((int)src[dim*dim-1].blue+(int)src[dim*dim-2].blue+(int)src[dim*dim-dim-1].blue+(int)src[dim*dim-dim-2].blue)/4);	
	dst[dim*dim-1].green = (unsigned short)(((int)src[dim*dim-1].green+(int)src[dim*dim-2].green+(int)src[dim*dim-dim-1].green+(int)src[dim*dim-dim-2].green)/4);

	dst[dim-1].red = (unsigned short)(((int)src[dim-1].red+(int)src[dim-2].red+(int)src[dim+dim-1].red+(int)src[dim+dim-2].red)/4);
	dst[dim-1].blue = (unsigned short)(((int)src[dim-1].blue+(int)src[dim-2].blue+(int)src[dim+dim-1].blue+(int)src[dim+dim-2].blue)/4);
	dst[dim-1].green = (unsigned short)(((int)src[dim-1].green+(int)src[dim-2].green+(int)src[dim+dim-1].green+(int)src[dim+dim-2].green)/4);

	dst[dim*dim-dim].red = (unsigned short)(((int)src[dim*dim-dim].red+(int)src[dim*dim-dim+1].red+(int)src[dim*dim-dim-dim].red+(int)src[dim*dim-dim-dim+1].red)/4); 
	dst[dim*dim-dim].blue = (unsigned short)(((int)src[dim*dim-dim].blue+(int)src[dim*dim-dim+1].blue+(int)src[dim*dim-dim-dim].blue+(int)src[dim*dim-dim-dim+1].blue)/4);
	dst[dim*dim-dim].green = (unsigned short)(((int)src[dim*dim-dim].green+(int)src[dim*dim-dim+1].green+(int)src[dim*dim-dim-dim].green+(int)src[dim*dim-dim-dim+1].green)/4);

    for(i = 1; i<dim-1; i++)
	for(j=1; j<dim-1; j++)
	{
		red=0; blue=0;green=0;num=0;
		p = RIDX(i,j,dim);
		red +=(int) src[p-dim].red;
		blue += (int)src[p-dim].blue;
		green += (int)src[p-dim].green;
		num++;

		red +=(int) src[p-dim-1].red;
		blue += (int)src[p-dim-1].blue;
		green += (int)src[p-dim-1].green;
		num++;
	
		red +=(int) src[p-dim+1].red;
		blue += (int)src[p-dim+1].blue;
		green += (int)src[p-dim+1].green;
		num++;


		red +=(int) src[p].red;
		blue += (int)src[p].blue;
		green += (int)src[p].green;
		num++;
			
		red +=(int) src[p+1].red;
		blue += (int)src[p+1].blue;
		green += (int)src[p+1].green;
		num++;

		red +=(int) src[p-1].red;
		blue += (int)src[p-1].blue;
		green += (int)src[p-1].green;
		num++;

		red +=(int) src[p+dim].red;
		blue += (int)src[p+dim].blue;
		green += (int)src[p+dim].green;
		num++;

		red +=(int) src[p+dim-1].red;
		blue += (int)src[p+dim-1].blue;
		green += (int)src[p+dim-1].green;
		num++;

		red +=(int) src[p+dim+1].red;
		blue += (int)src[p+dim+1].blue;
		green += (int)src[p+dim+1].green;
		num++;

		dst[p].red =(unsigned short) (red/num);
		dst[p].blue = (unsigned short) (blue/num);
		dst[p].green = (unsigned short) (green/num);
	}
	i=dim-1;
	for(j=1; j<dim-1;j++){
		red=0; blue=0;green=0;num=0;
		p = RIDX(i,j,dim);
		red +=(int) src[p-dim].red;
		blue += (int)src[p-dim].blue;
		green += (int)src[p-dim].green;
		num++;
		
		red +=(int) src[p-dim-1].red;
		blue += (int)src[p-dim-1].blue;
		green += (int)src[p-dim-1].green;
		num++;
	
		red +=(int) src[p-dim+1].red;
		blue += (int)src[p-dim+1].blue;
		green += (int)src[p-dim+1].green;
		num++;

		red +=(int) src[p].red;
		blue += (int)src[p].blue;
		green += (int)src[p].green;
		num++;

		red +=(int) src[p+1].red;
		blue += (int)src[p+1].blue;
		green += (int)src[p+1].green;
		num++;

		red +=(int) src[p-1].red;
		blue += (int)src[p-1].blue;
		green += (int)src[p-1].green;
		num++;

		dst[p].red =(unsigned short) (red/num);
		dst[p].blue = (unsigned short) (blue/num);
		dst[p].green = (unsigned short) (green/num);

	}	

	i=0;
	for(j=1; j<dim-1;j++){
		red=0; blue=0;green=0;num=0;
		p = RIDX(i,j,dim);
		red +=(int) src[p+dim].red;
		blue += (int)src[p+dim].blue;
		green += (int)src[p+dim].green;
		num++;
		
		red +=(int) src[p+dim-1].red;
		blue += (int)src[p+dim-1].blue;
		green += (int)src[p+dim-1].green;
		num++;
	
		red +=(int) src[p+dim+1].red;
		blue += (int)src[p+dim+1].blue;
		green += (int)src[p+dim+1].green;
		num++;

		red +=(int) src[p].red;
		blue += (int)src[p].blue;
		green += (int)src[p].green;
		num++;

		red +=(int) src[p+1].red;
		blue += (int)src[p+1].blue;
		green += (int)src[p+1].green;
		num++;

		red +=(int) src[p-1].red;
		blue += (int)src[p-1].blue;
		green += (int)src[p-1].green;
		num++;

		dst[p].red =(unsigned short) (red/num);
		dst[p].blue = (unsigned short) (blue/num);
		dst[p].green = (unsigned short) (green/num);

	}

	j=0;
	for(i=1; i<dim-1;i++){
		red=0; blue=0;green=0;num=0;
		p = RIDX(i,j,dim);
		red +=(int) src[p-dim].red;
		blue += (int)src[p-dim].blue;
		green += (int)src[p-dim].green;
		num++;
		
		red +=(int) src[p-dim+1].red;
		blue += (int)src[p-dim+1].blue;
		green += (int)src[p-dim+1].green;
		num++;

		red +=(int) src[p].red;
		blue += (int)src[p].blue;
		green += (int)src[p].green;
		num++;

		red +=(int) src[p+1].red;
		blue += (int)src[p+1].blue;
		green += (int)src[p+1].green;
		num++;

		red +=(int) src[p+dim].red;
		blue += (int)src[p+dim].blue;
		green += (int)src[p+dim].green;
		num++;

	
		red +=(int) src[p+dim+1].red;
		blue += (int)src[p+dim+1].blue;
		green += (int)src[p+dim+1].green;
		num++;
		dst[p].red =(unsigned short) (red/num);
		dst[p].blue = (unsigned short) (blue/num);
		dst[p].green = (unsigned short) (green/num);
	}

	j=dim-1;
	for(i=1; i<dim-1;i++){
		red=0; blue=0;green=0;num=0;
		p = RIDX(i,j,dim);
		red +=(int) src[p-dim].red;
		blue += (int)src[p-dim].blue;
		green += (int)src[p-dim].green;
		num++;
				
		red +=(int) src[p-dim-1].red;
		blue += (int)src[p-dim-1].blue;
		green += (int)src[p-dim-1].green;
		num++;


		red +=(int) src[p].red;
		blue += (int)src[p].blue;
		green += (int)src[p].green;
		num++;

		red +=(int) src[p-1].red;
		blue += (int)src[p-1].blue;
		green += (int)src[p-1].green;
		num++;

		red +=(int) src[p+dim].red;
		blue += (int)src[p+dim].blue;
		green += (int)src[p+dim].green;
		num++;

	
		red +=(int) src[p+dim-1].red;
		blue += (int)src[p+dim-1].blue;
		green += (int)src[p+dim-1].green;
		num++;
		dst[p].red =(unsigned short) (red/num);
		dst[p].blue = (unsigned short) (blue/num);
		dst[p].green = (unsigned short) (green/num);
	}
	
}
	


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

