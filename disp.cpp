/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"
#include <iostream>


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	
	*framebuffer = (char*)malloc(sizeof(char) * width * height * 3);
	
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/	
	
	GzPixel *buf = (GzPixel *)malloc(sizeof(GzPixel) * xRes*yRes); 
	*display = new GzDisplay();
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = buf;

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	
	free(display->fbuf);
	display->fbuf = NULL;
	delete display;
	display = NULL;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	
	*xRes = display->xres;
	*yRes = display->yres;

	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	
	int x = display->xres;
	int y = display->yres;
	
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			display->fbuf[ARRAY(i, j)].red = (150 << 4);
			display->fbuf[ARRAY(i, j)].green = (150 << 4);
			display->fbuf[ARRAY(i, j)].blue = (150 << 4);
			display->fbuf[ARRAY(i, j)].alpha = 1;
			display->fbuf[ARRAY(i, j)].z = MAXINT;
		}
	}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	/*Pixel coords bound check*/
	if (i < 0 || i >= display->xres || j < 0 || j >= display->yres)
		return GZ_SUCCESS;
	/*GzIntensity bound check*/
	if (r > 4095) r = 4095;
	if (g > 4095) g = 4095;
	if (b > 4095) b = 4095;
	if (a > 4095) a = 4095;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (a < 0) a = 0;
	
	display->fbuf[ARRAY(i, j)].red = r;
	display->fbuf[ARRAY(i, j)].green = g;
	display->fbuf[ARRAY(i, j)].blue = b;
	display->fbuf[ARRAY(i, j)].alpha = a;
	display->fbuf[ARRAY(i, j)].z = z;
	
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	
	*r = display->fbuf[ARRAY(i, j)].red;
	*g = display->fbuf[ARRAY(i, j)].green;
	*b = display->fbuf[ARRAY(i, j)].blue;
	*a = display->fbuf[ARRAY(i, j)].alpha;
	*z = display->fbuf[ARRAY(i, j)].z;
	
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	/*write header*/
	fprintf(outfile, "P6 %d %d 255\n", display->xres, display->yres);
	
	GzPixel		*temp = display->fbuf;
	
	char* outbuffer = new char[3];
	/*write data*/
	for (int i = 0; i < (display->xres)*(display->yres); i++) {
		outbuffer[0] = (temp[i].red >> 4);
		outbuffer[1] = (temp[i].green >> 4);
		outbuffer[2] = (temp[i].blue >> 4);
		fwrite(outbuffer, sizeof(char), 3, outfile);
	}
		
	delete outbuffer;
	outbuffer = NULL;

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/	
	GzPixel		*temp = display->fbuf;
	
	for (int i = 0; i < (display->xres)*(display->yres); i++) {
		framebuffer[i * 3] = (temp[i].blue >> 4);
		framebuffer[i * 3 + 1] = (temp[i].green >> 4);
		framebuffer[i * 3 + 2] = (temp[i].red >> 4);
	}

	temp = NULL;
	
	return GZ_SUCCESS;
}