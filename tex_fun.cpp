/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

#define M_PI       3.14159265358979323846

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

#define	ARRAY(x,y)	(x+(y*xs))	
void hsv2rgb(GzColor in, GzColor out);

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */

  //if (u > 1 || u < 0 || v > 1 || v < 0)
	//  return GZ_FAILURE;

  if (u > 1) u = 1;
  if (u < 0) u = 0;
  if (v > 1) v = 1;
  if (v < 0) v = 0;

/* determine texture cell corner values and perform bilinear interpolation */

  float x = u * (xs - 1);
  float y = v * (ys - 1);

  float s = x - floor(x);
  float t = y - floor(y);

/* set color to interpolated GzColor value and return */

  color[RED] = s * t * image[ARRAY((int)ceil(x), (int)ceil(y))][RED] 
	  + (1 - s) * t * image[ARRAY((int)floor(x), (int)ceil(y))][RED]
	  + s * (1 - t) * image[ARRAY((int)ceil(x), (int)floor(y))][RED] 
	  + (1 - s) * (1 - t) * image[ARRAY((int)floor(x), (int)floor(y))][RED];

  color[GREEN] = s * t * image[ARRAY((int)ceil(x), (int)ceil(y))][GREEN] 
	  + (1 - s) * t * image[ARRAY((int)floor(x), (int)ceil(y))][GREEN]
	  + s * (1 - t) * image[ARRAY((int)ceil(x), (int)floor(y))][GREEN] 
	  + (1 - s) * (1 - t) * image[ARRAY((int)floor(x), (int)floor(y))][GREEN];

  color[BLUE] = s * t * image[ARRAY((int)ceil(x), (int)ceil(y))][BLUE] 
	  + (1 - s) * t * image[ARRAY((int)floor(x), (int)ceil(y))][BLUE]
	  + s * (1 - t) * image[ARRAY((int)ceil(x), (int)floor(y))][BLUE] 
	  + (1 - s) * (1 - t) * image[ARRAY((int)floor(x), (int)floor(y))][BLUE];

  return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	//if (u > 1 || u < 0 || v > 1 || v < 0)
		//return GZ_FAILURE;
	
	if (u > 1) u = 1;
	if (u < 0) u = 0;
	if (v > 1) v = 1;
	if (v < 0) v = 0;

	if (false)
	{
		//cos * sin
		GzColor color1{ 0.2023f, 0.3945f, 0.1898f };
		GzColor color2{ 0.3531f, 0.5359f, 0.7721f };

		int scale = 3;

		color[RED] = fabs(cos(scale * M_PI * u)) * fabs(sin(scale * M_PI * v)) * color2[RED]
			+ color1[RED];
		color[GREEN] = fabs(cos(scale * M_PI * u)) * fabs(sin(scale * M_PI * v)) * color2[GREEN]
			+ color1[GREEN];
		color[BLUE] = fabs(cos(scale * M_PI * u)) * fabs(sin(scale * M_PI * v)) * color2[BLUE]
			+ color1[BLUE];
	}

	else
	{
		//julia set

		float oldRe, oldIm, newRe, newIm, cRe, cIm;

		newRe = (2 * u - 1);
		newIm = (2 * v - 1);

		cRe = -0.7;
		cIm = 0.27015;

		int i, N;
		N = 300;

		for (i = 0; i < N; i++)
		{

			oldRe = newRe;
			oldIm = newIm;

			newRe = oldRe * oldRe - oldIm * oldIm + cRe;
			newIm = 2 * oldRe * oldIm + cIm;

			if ((newRe * newRe + newIm * newIm) > 4) break;
		}

		GzColor hsv{ (i % 256) / 255.0f, 1, 1 * (i < N) };

		hsv2rgb(hsv, color);
	}
	
	return GZ_SUCCESS;
}

void hsv2rgb(GzColor in, GzColor out)
{
	float      hh, p, q, t, ff;
	long        i;

	for (int i = 0; i < 3; i++) {
		in[i] *= 255.0f;
	}

	if (in[0] < 0.0) {       // < is bogus, just shuts up warnings
		out[RED] = in[2] / 256;
		out[GREEN] = in[2] / 256;
		out[BLUE] = in[2] / 256;
		return;
	}
	hh = in[0];
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in[2] * (1.0 - in[1]);
	q = in[2] * (1.0 - (in[1] * ff));
	t = in[2] * (1.0 - (in[1] * (1.0 - ff)));

	switch (i) {
	case 0:
		out[RED] = in[2];
		out[GREEN] = t;
		out[BLUE] = p;
		break;
	case 1:
		out[RED] = q;
		out[GREEN] = in[2];
		out[BLUE] = p;
		break;
	case 2:
		out[RED] = p;
		out[GREEN] = in[2];
		out[BLUE] = t;
		break;

	case 3:
		out[RED] = p;
		out[GREEN] = q;
		out[BLUE] = in[2];
		break;
	case 4:
		out[RED] = t;
		out[GREEN] = p;
		out[BLUE] = in[2];
		break;
	case 5:
	default:
		out[RED] = in[2];
		out[GREEN] = p;
		out[BLUE] = q;
		break;
	}
	out[RED] /= 256.0;
	out[GREEN] /= 256.0;
	out[BLUE] /= 256.0;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

