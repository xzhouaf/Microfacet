#if !defined(REND_H_)
#define REND_H_
#include "disp.h" /* include your own disp.h file (e.g. hw1)*/

#include <algorithm>   
#include <vector>	

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	DEFAULT_ROUGHNESS		1.0
#define	DEFAULT_R_INDEX		20.0

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

#define LEFT   0
#define RIGHT  1
#define NORM   2
#define INV    3

#define M_PI       3.14159265358979323846   // pi

#define DO_P_COR		true
#define DO_GLITTER		true

#ifndef GZRENDER
#define GZRENDER
typedef struct {			/* define a renderer */
  GzDisplay		*display;
  GzCamera		camera;
  short		    matlevel;	        /* top of stack - current xform */
  GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
  GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
  GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
  GzColor		flatcolor;          /* color state for flat shaded triangles */
  int			interp_mode;
  int			numlights;
  GzLight		lights[MAX_LIGHTS];
  GzLight		ambientlight;
  GzColor		Ka, Kd, Ks;
  float		    spec, roughness, refractiveIndex;		/* specular power */
  GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */

  float			Xoffset;
  float			Yoffset;
  //float			weight;
}  GzRender;
#endif

#ifndef VERTEX
#define VERTEX
class Vertex
{
public:
	float x;
	float y;
	float z;
	float n_x;
	float n_y;
	float n_z;
	float r;
	float g;
	float b;
	float u;
	float v;

	Vertex() {};

	~Vertex() {};

	static bool SortY(const Vertex& v1, const Vertex& v2)
	{
		return v1.y <= v2.y;
	}

	void copy(const Vertex& s)
	{
		x = s.x;
		y = s.y;
		z = s.z;
		n_x = s.n_x;
		n_y = s.n_y;
		n_z = s.n_z;
		r = s.r;
		g = s.g;
		b = s.b;
		u = s.u;
		v = s.v;
	}
};
#endif

//define a Edge class that including start vertex and end vertex
#ifndef EDGE
#define EDGE
class Edge
{
public:
	Vertex start;
	Vertex end;
	Vertex current;

	Edge() {};
	~Edge() {};
	void Initialize(const Vertex& s, const Vertex& e)
	{
		start.copy(s);
		end.copy(e);
	}

	float GetSlopeX() {
		return (end.x - start.x) / (end.y - start.y);
	}

	float GetSlopeZ() {
		return (end.z - start.z) / (end.y - start.y);
	}

	float GetSlopeNX() {
		return (end.n_x - start.n_x) / (end.y - start.y);
	}

	float GetSlopeNY() {
		return (end.n_y - start.n_y) / (end.y - start.y);
	}

	float GetSlopeNZ() {
		return (end.n_z - start.n_z) / (end.y - start.y);
	}

	float GetSlopeRed() {
		return (end.r - start.r) / (end.y - start.y);
	}

	float GetSlopeGreen() {
		return (end.g - start.g) / (end.y - start.y);
	}

	float GetSlopeBlue() {
		return (end.b - start.b) / (end.y - start.y);
	}

	float GetSlopeU() {
		return (end.u - start.u) / (end.y - start.y);
	}

	float GetSlopeV() {
		return (end.v - start.v) / (end.y - start.y);
	}

};
#endif

//define a Span class that including start vertex and end vertex
#ifndef SPAN
#define SPAN
class Span
{
public:
	Vertex start;
	Vertex end;
	Vertex current;

	Span() {};
	~Span() {};

	void Initialize(const Vertex& s, const Vertex& e)
	{
		start.copy(s);
		end.copy(e);
	}
	float GetSlopeZ() {
		return (end.z - start.z) / (end.x - start.x);
	}

	float GetSlopeNX() {
		return (end.n_x - start.n_x) / (end.x - start.x);
	}

	float GetSlopeNY() {
		return (end.n_y - start.n_y) / (end.x - start.x);
	}

	float GetSlopeNZ() {
		return (end.n_z - start.n_z) / (end.x - start.x);
	}

	float GetSlopeRed() {
		return (end.r - start.r) / (end.x - start.x);
	}

	float GetSlopeGreen() {
		return (end.g - start.g) / (end.x - start.x);
	}

	float GetSlopeBlue() {
		return (end.b - start.b) / (end.x - start.x);
	}

	float GetSlopeU() {
		return (end.u - start.u) / (end.x - start.x);
	}

	float GetSlopeV() {
		return (end.v - start.v) / (end.x - start.x);
	}
};
#endif

// Function declaration
// HW2
int GzNewRender(GzRender **render, GzDisplay *display);
int GzFreeRender(GzRender *render);
int GzBeginRender(GzRender	*render);
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList);
int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList);
short	ctoi(float color);
void PutVertexesPos(GzRender* render, Vertex* vtxs, GzCoord* coordptr);
void SortVertexes(std::vector<Vertex>& vtxs);
int SetTriangle(Edge* edges, Vertex* vtxs);
void RunScanLine(GzRender* render, Vertex* vtxs, Edge* edges, int triType);

// HW3
int GzPutCamera(GzRender *render, GzCamera *camera);
int GzPushMatrix(GzRender *render, GzMatrix	matrix);
int GzPopMatrix(GzRender *render);
int GzSetXsp(GzRender* render, unsigned short xres, unsigned short yres);
int GzSetXpiXiw(GzRender* render);
void GzTransformTri(GzMatrix matrix, GzCoord* vertexes, bool isNormal, int count);
void GzMultipyMatrix(GzMatrix result, const GzMatrix left, const GzMatrix right);
bool CheckVertexesZ(Vertex* vertexes);
float GetRadian(const float degree);

// HW4

int GzShade(GzRender *render, Vertex* vtx, GzColor textColor = NULL);
void PutVertexesNorm(Vertex* vtxs, GzCoord* coordptr);
int GzShade_Glitter(GzRender * render, Vertex* vtx, GzColor textColor, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV);

// HW5
int GzFreeTexture();
void PutVertexesText(Vertex* vtxs, GzTextureIndex* textptr);
void transToP(Vertex* vtxs);
void transToA(Vertex vtx, GzTextureIndex index);
void transToA(float z, GzTextureIndex index);

// Object Translation
int GzRotXMat(float degree, GzMatrix mat);
int GzRotYMat(float degree, GzMatrix mat);
int GzRotZMat(float degree, GzMatrix mat);
int GzTrxMat(GzCoord translate, GzMatrix mat);
int GzScaleMat(GzCoord scale, GzMatrix mat);

void GzNormalize(GzCoord a);
float GzDotProduct(const GzCoord a, const GzCoord b);
#endif