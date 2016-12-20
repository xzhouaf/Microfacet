/* CS580 Homework 5 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include	"brdf.h"
#include	"microfacet_glitter.h"

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	float radian = GetRadian(degree);
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = 1;
	mat[1][1] = cos(radian);
	mat[1][2] = -sin(radian);
	mat[2][1] = sin(radian);
	mat[2][2] = cos(radian);


	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	float radian = GetRadian(degree);
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = cos(radian);
	mat[1][1] = 1;
	mat[0][2] = sin(radian);
	mat[2][0] = -sin(radian);
	mat[2][2] = cos(radian);
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	float radian = GetRadian(degree);
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = cos(radian);
	mat[0][1] = -sin(radian);
	mat[1][0] = sin(radian);
	mat[1][1] = cos(radian);
	mat[2][2] = 1;
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			mat[i][j] = 0;
		}
		mat[i][i] = 1;
	}
	mat[0][3] = translate[X];
	mat[1][3] = translate[Y];
	mat[2][3] = translate[Z];
	

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = scale[X];
	mat[1][1] = scale[Y];
	mat[2][2] = scale[Z];
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
	/*
	- malloc a renderer struct
	- setup Xsp and anything only done once
	- save the pointer to display
	- init default camera
	*/
	*render = new GzRender;

	(*render)->matlevel = 0;

	(*render)->display = display;

	GzSetXsp(*render, display->xres, display->yres);

	if (*render == NULL) {
		return GZ_FAILURE;
	}
	(*render)->camera.lookat[X] = 0;
	(*render)->camera.lookat[Y] = 0;
	(*render)->camera.lookat[Z] = 0;

	(*render)->camera.position[X] = DEFAULT_IM_X;
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
	(*render)->camera.position[Z] = DEFAULT_IM_Z;

	(*render)->camera.worldup[X] = 0;
	(*render)->camera.worldup[Y] = 1;
	(*render)->camera.worldup[Z] = 0;

	(*render)->camera.FOV = DEFAULT_FOV;

	(*render)->interp_mode = GZ_FLAT;
	(*render)->numlights = 0;
	
	GzColor Ka = DEFAULT_AMBIENT;

	(*render)->Ka[0] = Ka[0];
	(*render)->Ka[1] = Ka[1];
	(*render)->Ka[2] = Ka[2];

	GzColor Kd = DEFAULT_DIFFUSE;

	(*render)->Kd[0] = Kd[0];
	(*render)->Kd[1] = Kd[1];
	(*render)->Kd[2] = Kd[2];
	
	GzColor Ks = DEFAULT_SPECULAR;
	
	(*render)->Ks[0] = Ks[0];
	(*render)->Ks[1] = Ks[1];
	(*render)->Ks[2] = Ks[2];

	(*render)->spec = DEFAULT_SPEC;

	(*render)->tex_fun = NULL;

	(*render)->Xoffset = 0.0f;
	(*render)->Yoffset = 0.0f;

	(*render)->roughness = DEFAULT_ROUGHNESS;
	(*render)->refractiveIndex = DEFAULT_R_INDEX;

	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
	/*
	-free all renderer resources
	*/
	delete render;
	render = NULL;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
	/*
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	if (GzInitDisplay(render->display) == GZ_FAILURE) {
		return GZ_FAILURE;
	}
	GzSetXpiXiw(render);

	GzPushMatrix(render, render->Xsp);
	GzPushMatrix(render, render->camera.Xpi);
	GzPushMatrix(render, render->camera.Xiw);

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
	/*
	- overwrite renderer camera structure with new camera definition
	*/
	render->camera.FOV = camera->FOV;

	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];

	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];

	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];

	return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (render->matlevel == 0) {
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				render->Ximage[render->matlevel][i][j] = matrix[i][j];
				render->Xnorm[render->matlevel][i][j] = 0;
			}
		}
		render->Xnorm[render->matlevel][0][0] = 1;
		render->Xnorm[render->matlevel][1][1] = 1;
		render->Xnorm[render->matlevel][2][2] = 1;
		render->Xnorm[render->matlevel][3][3] = 1;
	}
	// stack overflow
	else if (render->matlevel >= MATLEVELS)
		return GZ_FAILURE;
	else {
		GzMultipyMatrix(render->Ximage[render->matlevel], render->Ximage[render->matlevel - 1], matrix);
		
		if (render->matlevel > 1) {
			GzMultipyMatrix(render->Xnorm[render->matlevel], render->Xnorm[render->matlevel - 1], matrix);
		}
		else {
			for (int i = 0;i < 4;i++) {
				for (int j = 0;j < 4;j++) {
					render->Xnorm[render->matlevel][i][j] = 0;
				}
			}
			render->Xnorm[render->matlevel][0][0] = 1;
			render->Xnorm[render->matlevel][1][1] = 1;
			render->Xnorm[render->matlevel][2][2] = 1;
			render->Xnorm[render->matlevel][3][3] = 1;
		}
	}	
	(render->matlevel)++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/	
	if (render->matlevel <= 0) return GZ_FAILURE;

	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i]) {
		case GZ_RGB_COLOR: {
			GzColor* colorptr = (GzColor*)(valueList[i]);

			render->flatcolor[RED] = (*colorptr)[RED];
			render->flatcolor[GREEN] = (*colorptr)[GREEN];
			render->flatcolor[BLUE] = (*colorptr)[BLUE];
			break;
		}

		case GZ_INTERPOLATE: {
			int* istyleptr = (int*)(valueList[i]);

			render->interp_mode = *istyleptr;
			break;
		}

		case GZ_DIRECTIONAL_LIGHT: {
			GzLight* dirlightptr = (GzLight*)(valueList[i]);

			render->lights[render->numlights].direction[X] = dirlightptr->direction[X];
			render->lights[render->numlights].direction[Y] = dirlightptr->direction[Y];
			render->lights[render->numlights].direction[Z] = dirlightptr->direction[Z];
			render->lights[render->numlights].color[RED] = dirlightptr->color[RED];
			render->lights[render->numlights].color[GREEN] = dirlightptr->color[GREEN];
			render->lights[render->numlights].color[BLUE] = dirlightptr->color[BLUE];
			render->numlights++;
			break;
		}

		case GZ_AMBIENT_LIGHT: {
			GzLight* amblightptr = (GzLight*)(valueList[i]);

			//render->ambientlight.direction[X] = amblightptr->direction[X];
			//render->ambientlight.direction[Y] = amblightptr->direction[Y];
			//render->ambientlight.direction[Z] = amblightptr->direction[Z];
			render->ambientlight.color[RED] = amblightptr->color[RED];
			render->ambientlight.color[GREEN] = amblightptr->color[GREEN];
			render->ambientlight.color[BLUE] = amblightptr->color[BLUE];
			break;
		}

		case GZ_AMBIENT_COEFFICIENT: {
			GzColor* kaptr = (GzColor*)(valueList[i]);

			render->Ka[RED] = (*kaptr)[RED];
			render->Ka[GREEN] = (*kaptr)[GREEN];
			render->Ka[BLUE] = (*kaptr)[BLUE];
			break;
		}

		case GZ_DIFFUSE_COEFFICIENT: {
			GzColor* kdptr = (GzColor*)(valueList[i]);

			render->Kd[RED] = (*kdptr)[RED];
			render->Kd[GREEN] = (*kdptr)[GREEN];
			render->Kd[BLUE] = (*kdptr)[BLUE];
			break;
		}

		case GZ_SPECULAR_COEFFICIENT: {
			GzColor* ksptr = (GzColor*)(valueList[i]);

			render->Ks[RED] = (*ksptr)[RED];
			render->Ks[GREEN] = (*ksptr)[GREEN];
			render->Ks[BLUE] = (*ksptr)[BLUE];
			break;
		}
		
		case GZ_DISTRIBUTION_COEFFICIENT: {
			render->spec = *((float*)(valueList[i]));
			break;
		}

		case GZ_TEXTURE_MAP: {
			render->tex_fun = (GzTexture)(valueList[i]);
			break;
		}

		case GZ_AASHIFTX: {
			render->Xoffset = *((float*)(valueList[i]));
			break;
		}

		case GZ_AASHIFTY: {
			render->Yoffset = *((float*)(valueList[i]));
			break;
		}

		case GZ_SURFACE_ROUGHNESS: {
			render->roughness = *((float*)(valueList[i]));
			break;
		}

		case GZ_REFRACTIVE_INDEX: {
			render->refractiveIndex = *((float*)(valueList[i]));
			break;
		}

		default:
			break;
		}
	}
	
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	GzDisplay * display = render->display;
	Vertex* vtxs = new Vertex[3];

	for (int i = 0; i < numParts; i++) {
		switch (nameList[i]) {
		case GZ_NULL_TOKEN:
			break;

		case GZ_POSITION: {
			
			GzCoord* coordptr = (GzCoord*)(valueList[i]);

			GzCoord* copy = new GzCoord[3];
			
			for (int i = 0; i < 3; i++) {
				copy[i][X] = coordptr[i][X];
				copy[i][Y] = coordptr[i][Y];
				copy[i][Z] = coordptr[i][Z];
			}
			
			GzTransformTri(render->Ximage[render->matlevel-1], copy, false, 3);
			
			PutVertexesPos(render, vtxs, copy);
						
			delete[] copy;
			copy = NULL;

			bool NEGATIVE_Z = CheckVertexesZ(vtxs);
			if (NEGATIVE_Z) {
				return GZ_SUCCESS;
			}			
			break;
		}

		case GZ_NORMAL: {
			
			GzCoord* coordptr = (GzCoord*)(valueList[i]);

			GzCoord* copy = new GzCoord[3];

			for (int i = 0; i < 3; i++) {
				copy[i][X] = coordptr[i][X];
				copy[i][Y] = coordptr[i][Y];
				copy[i][Z] = coordptr[i][Z];
			}

			GzTransformTri(render->Xnorm[render->matlevel - 1], copy, true, 3);

			PutVertexesNorm(vtxs, copy);
			
			delete[] copy;
			copy = NULL;

			break;
		}

		case GZ_TEXTURE_INDEX: {

			GzTextureIndex* textptr = (GzTextureIndex*)(valueList[i]);

			PutVertexesText(vtxs, textptr);

			if (DO_P_COR)
			{
				transToP(vtxs);
			}

			break;

		}

		default:
			break;
		}
		
	}

	if (render->interp_mode == GZ_FLAT && true) {
		GzShade(render, vtxs);
		render->flatcolor[RED] = vtxs[0].r;
		render->flatcolor[GREEN] = vtxs[0].g;
		render->flatcolor[BLUE] = vtxs[0].b;
		
	}
	std::vector<Vertex> vertexVector(vtxs, vtxs + 3);

	SortVertexes(vertexVector);
	
	vtxs[0] = vertexVector[0];
	vtxs[1] = vertexVector[1];
	vtxs[2] = vertexVector[2];

	

	if (render->interp_mode == GZ_COLOR) {

		if (render->tex_fun)
		{
			GzShade(render, vtxs, GzColor{ 1.0f, 1.0f, 1.0f });
			GzShade(render, vtxs + 1, GzColor{ 1.0f, 1.0f, 1.0f });
			GzShade(render, vtxs + 2, GzColor{ 1.0f, 1.0f, 1.0f });
		}
		
		else
		{
			GzShade(render, vtxs);
			GzShade(render, vtxs + 1);
			GzShade(render, vtxs + 2);
		}
	}
	Edge* edges = new Edge[3];

	int triType = SetTriangle(edges, vtxs);

	RunScanLine(render, vtxs, edges, triType);

	if (edges != NULL)
	{
		delete[] edges;
	}
		

	if(vtxs!=NULL)
		delete[] vtxs;

	return GZ_SUCCESS;
}

void PutVertexesPos(GzRender* render, Vertex* vtxs, GzCoord* coordptr) {
	for (int i = 0; i < 3; i++) {
		
		vtxs[i].x = (*(coordptr + i))[X];
		vtxs[i].y = (*(coordptr + i))[Y];
		vtxs[i].z = (*(coordptr + i))[Z];
		vtxs[i].x -= render -> Xoffset;
		vtxs[i].y -= render -> Yoffset;
	}

}

void PutVertexesNorm(Vertex* vtxs, GzCoord* coordptr) {
	for (int i = 0; i < 3; i++) {

		vtxs[i].n_x = (*(coordptr + i))[X];
		vtxs[i].n_y = (*(coordptr + i))[Y];
		vtxs[i].n_z = (*(coordptr + i))[Z];
	}

}

void PutVertexesText(Vertex* vtxs, GzTextureIndex* textptr) {
	for (int i = 0; i < 3; i++) {
		vtxs[i].u = (*(textptr + i))[U];
		vtxs[i].v = (*(textptr + i))[V];
	}
}

void transToP(Vertex* vtxs) {

	float Vz;
	for (int i = 0; i < 3; i++) {

		Vz = vtxs[i].z / (MAXINT - vtxs[i].z);
		vtxs[i].u /= (Vz + 1);
		vtxs[i].v /= (Vz + 1);
	}
}
void transToA(Vertex vtx, GzTextureIndex index) {

	float Vz = vtx.z / (MAXINT - vtx.z);
	index[U] = vtx.u * (Vz + 1);
	index[V] = vtx.v * (Vz + 1);
}
void transToA(float z, GzTextureIndex index) {

	float Vz = z / (MAXINT - z);
	index[U] = index[U] * (Vz + 1);
	index[V] = index[V] * (Vz + 1);
}

void SortVertexes(std::vector<Vertex>& vtxs) {
	
	std::sort(vtxs.begin(), vtxs.end(), Vertex::SortY);

}


int SetTriangle(Edge* edges, Vertex* vtxs) {
	if (vtxs[0].y == vtxs[1].y) {
		if (vtxs[0].x < vtxs[1].x) {
			edges[0].Initialize(vtxs[0], vtxs[1]);
			edges[1].Initialize(vtxs[1], vtxs[2]);
			edges[2].Initialize(vtxs[0], vtxs[2]);
		}
		else {
			edges[0].Initialize(vtxs[1], vtxs[0]);
			edges[1].Initialize(vtxs[0], vtxs[2]);
			edges[2].Initialize(vtxs[1], vtxs[2]);
		}
		return INV;
	}
	else if (vtxs[1].y == vtxs[2].y) {
		if (vtxs[1].x < vtxs[2].x) {
			edges[0].Initialize(vtxs[0], vtxs[2]);
			edges[1].Initialize(vtxs[2], vtxs[1]);
			edges[2].Initialize(vtxs[0], vtxs[1]);

		}
		else {
			edges[0].Initialize(vtxs[0], vtxs[1]);
			edges[1].Initialize(vtxs[1], vtxs[2]);
			edges[2].Initialize(vtxs[0], vtxs[2]);
		}
		return NORM;
	}
	else {
		edges[0].Initialize(vtxs[0], vtxs[1]);
		edges[1].Initialize(vtxs[1], vtxs[2]);
		edges[2].Initialize(vtxs[0], vtxs[2]);
		if (edges[0].GetSlopeX() > edges[2].GetSlopeX()) {
			return RIGHT;
		}
		else {
			return LEFT;
		}
	}
}

void RunScanLine(GzRender* render, Vertex* vtxs, Edge* edges, int triType) {

	Span spanline;
	float deltaY12, deltaY23, deltaY13, deltaX;
	GzIntensity red, green, blue, alpha;
	GzDepth bufferz;
	int currentx, currenty;
	GzTextureIndex indexInA;
	GzColor texColor;

	edges[2].current.y = ceil(edges[2].start.y);
	if (edges[2].current.y < 0) edges[2].current.y = 0;
	deltaY13 = edges[2].current.y - edges[2].start.y;
	edges[2].current.x = edges[2].start.x + edges[2].GetSlopeX()*deltaY13;
	edges[2].current.z = edges[2].start.z + edges[2].GetSlopeZ()*deltaY13;
	edges[2].current.n_x = edges[2].start.n_x + edges[2].GetSlopeNX()*deltaY13;
	edges[2].current.n_y = edges[2].start.n_y + edges[2].GetSlopeNY()*deltaY13;
	edges[2].current.n_z = edges[2].start.n_z + edges[2].GetSlopeNZ()*deltaY13;
	/*edges[2].current.r = edges[2].start.r + edges[2].GetSlopeRed()*deltaY13;
	edges[2].current.g = edges[2].start.g + edges[2].GetSlopeGreen()*deltaY13;
	edges[2].current.b = edges[2].start.b + edges[2].GetSlopeBlue()*deltaY13;*/
	edges[2].current.u = edges[2].start.u + edges[2].GetSlopeU()*deltaY13;
	edges[2].current.v = edges[2].start.v + edges[2].GetSlopeV()*deltaY13;

	if (triType != INV) {

		edges[0].current.y = ceil(edges[0].start.y);
		if (edges[0].current.y < 0) edges[0].current.y = 0;
		deltaY12 = edges[0].current.y - edges[0].start.y;
		edges[0].current.x = edges[0].start.x + edges[0].GetSlopeX()*deltaY12;
		edges[0].current.z = edges[0].start.z + edges[0].GetSlopeZ()*deltaY12;
		edges[0].current.n_x = edges[0].start.n_x + edges[0].GetSlopeNX()*deltaY12;
		edges[0].current.n_y = edges[0].start.n_y + edges[0].GetSlopeNY()*deltaY12;
		edges[0].current.n_z = edges[0].start.n_z + edges[0].GetSlopeNZ()*deltaY12;
		/*edges[0].current.r = edges[0].start.r + edges[0].GetSlopeRed()*deltaY12;
		edges[0].current.g = edges[0].start.g + edges[0].GetSlopeGreen()*deltaY12;
		edges[0].current.b = edges[0].start.b + edges[0].GetSlopeBlue()*deltaY12;*/
		edges[0].current.u = edges[0].start.u + edges[0].GetSlopeU()*deltaY12;
		edges[0].current.v = edges[0].start.v + edges[0].GetSlopeV()*deltaY12;

		while (edges[0].current.y <= edges[0].end.y && edges[0].current.y < render->display->yres) {
			if (triType == LEFT)
				spanline.Initialize(edges[0].current, edges[2].current);
			else
				spanline.Initialize(edges[2].current, edges[0].current);
			if (edges[0].current.y != edges[2].current.y) break;

			spanline.current.x = ceil(spanline.start.x);
			if (spanline.current.x < 0) spanline.current.x = 0;
			spanline.current.y = edges[0].current.y;
			deltaX = spanline.current.x - spanline.start.x;
			spanline.current.z = spanline.start.z + spanline.GetSlopeZ()*deltaX;
			spanline.current.n_x = spanline.start.n_x + spanline.GetSlopeNX()*deltaX;
			spanline.current.n_y = spanline.start.n_y + spanline.GetSlopeNY()*deltaX;
			spanline.current.n_z = spanline.start.n_z + spanline.GetSlopeNZ()*deltaX;
			/*spanline.current.r = spanline.start.r + spanline.GetSlopeRed()*deltaX;
			spanline.current.g = spanline.start.g + spanline.GetSlopeGreen()*deltaX;
			spanline.current.b = spanline.start.b + spanline.GetSlopeBlue()*deltaX;*/
			spanline.current.u = spanline.start.u + spanline.GetSlopeU()*deltaX;
			spanline.current.v = spanline.start.v + spanline.GetSlopeV()*deltaX;

			while (spanline.current.x <= spanline.end.x && spanline.current.x < render->display->xres) {
				currentx = (int)spanline.current.x;
				currenty = (int)spanline.current.y;
				GzGetDisplay(render->display, currentx, currenty,
					&red, &green, &blue, &alpha, &bufferz);
				if (spanline.current.z < (float)bufferz && spanline.current.z > 0) {
					if (render->interp_mode == GZ_COLOR)
					{
						indexInA[U] = spanline.current.u;
						indexInA[V] = spanline.current.v;
						if (DO_P_COR)
							transToA(spanline.current, indexInA);

						//GzColor texColor;
						texColor[RED] = 1.0f;
						texColor[GREEN] = 1.0f;
						texColor[BLUE] = 1.0f;
						if (render->tex_fun)
							render->tex_fun(indexInA[U], indexInA[V], texColor);

						GzPutDisplay(render->display, currentx, currenty, ctoi(texColor[RED] * spanline.current.r), 
							ctoi(texColor[GREEN] * spanline.current.g), ctoi(texColor[BLUE] * spanline.current.b),
							1, (GzDepth)spanline.current.z);
					}						
					else if (render->interp_mode == GZ_NORMALS) {
						
						GzTextureIndex indexInA;
						indexInA[U] = spanline.current.u;
						indexInA[V] = spanline.current.v;
						if (DO_P_COR)
							transToA(spanline.current, indexInA);

						if (DO_GLITTER) {
							GzColor texColor;
							GzTextureIndex lefttopUV;
							GzTextureIndex bottomrightUV;
							lefttopUV[U] = spanline.current.u - spanline.GetSlopeU()*0.5;
							lefttopUV[V] = spanline.current.v - spanline.GetSlopeV()*0.5;
							bottomrightUV[U] = spanline.current.u + spanline.GetSlopeU()*0.5;
							bottomrightUV[V] = spanline.current.v + spanline.GetSlopeV()*0.5;
							transToA(spanline.current.z, lefttopUV);
							transToA(spanline.current.z, bottomrightUV);

							render->tex_fun(indexInA[U], indexInA[V], texColor);
							GzShade_Glitter(render, &spanline.current, texColor, lefttopUV, bottomrightUV);

						}
						else {
							if (render->tex_fun) {
								GzColor texColor;
								render->tex_fun(indexInA[U], indexInA[V], texColor);
								GzShade(render, &spanline.current, texColor);
							}
							else
								GzShade(render, &spanline.current);
						}

						GzPutDisplay(render->display, currentx, currenty, ctoi(spanline.current.r), ctoi(spanline.current.g),
							ctoi(spanline.current.b), 1, (GzDepth)spanline.current.z);
					}
					else
						GzPutDisplay(render->display, currentx, currenty, ctoi(render->flatcolor[RED]), ctoi(render->flatcolor[GREEN]),
						ctoi(render->flatcolor[BLUE]), 1, (GzDepth)spanline.current.z);
				}
				spanline.current.x++;
				deltaX = spanline.current.x - spanline.start.x;
				spanline.current.z = spanline.start.z + spanline.GetSlopeZ()*deltaX;
				spanline.current.n_x = spanline.start.n_x + spanline.GetSlopeNX()*deltaX;
				spanline.current.n_y = spanline.start.n_y + spanline.GetSlopeNY()*deltaX;
				spanline.current.n_z = spanline.start.n_z + spanline.GetSlopeNZ()*deltaX;
				/*spanline.current.r = spanline.start.r + spanline.GetSlopeRed()*deltaX;
				spanline.current.g = spanline.start.g + spanline.GetSlopeGreen()*deltaX;
				spanline.current.b = spanline.start.b + spanline.GetSlopeBlue()*deltaX;*/
				spanline.current.u = spanline.start.u + spanline.GetSlopeU()*deltaX;
				spanline.current.v = spanline.start.v + spanline.GetSlopeV()*deltaX;
			}

			edges[0].current.y++;
			deltaY12 = edges[0].current.y - edges[0].start.y;
			edges[0].current.x = edges[0].start.x + edges[0].GetSlopeX()*deltaY12;
			edges[0].current.z = edges[0].start.z + edges[0].GetSlopeZ()*deltaY12;
			edges[0].current.n_x = edges[0].start.n_x + edges[0].GetSlopeNX()*deltaY12;
			edges[0].current.n_y = edges[0].start.n_y + edges[0].GetSlopeNY()*deltaY12;
			edges[0].current.n_z = edges[0].start.n_z + edges[0].GetSlopeNZ()*deltaY12;
			/*edges[0].current.r = edges[0].start.r + edges[0].GetSlopeRed()*deltaY12;
			edges[0].current.g = edges[0].start.g + edges[0].GetSlopeGreen()*deltaY12;
			edges[0].current.b = edges[0].start.b + edges[0].GetSlopeBlue()*deltaY12;*/
			edges[0].current.u = edges[0].start.u + edges[0].GetSlopeU()*deltaY12;
			edges[0].current.v = edges[0].start.v + edges[0].GetSlopeV()*deltaY12;

			edges[2].current.y++;
			deltaY13 = edges[2].current.y - edges[2].start.y;
			edges[2].current.x = edges[2].start.x + edges[2].GetSlopeX()*deltaY13;
			edges[2].current.z = edges[2].start.z + edges[2].GetSlopeZ()*deltaY13;
			edges[2].current.n_x = edges[2].start.n_x + edges[2].GetSlopeNX()*deltaY13;
			edges[2].current.n_y = edges[2].start.n_y + edges[2].GetSlopeNY()*deltaY13;
			edges[2].current.n_z = edges[2].start.n_z + edges[2].GetSlopeNZ()*deltaY13;
			/*edges[2].current.r = edges[2].start.r + edges[2].GetSlopeRed()*deltaY13;
			edges[2].current.g = edges[2].start.g + edges[2].GetSlopeGreen()*deltaY13;
			edges[2].current.b = edges[2].start.b + edges[2].GetSlopeBlue()*deltaY13;*/
			edges[2].current.u = edges[2].start.u + edges[2].GetSlopeU()*deltaY13;
			edges[2].current.v = edges[2].start.v + edges[2].GetSlopeV()*deltaY13;
		}
	}
	if (triType != NORM) {

		edges[1].current.y = ceil(edges[1].start.y);
		if (edges[1].current.y < 0) edges[1].current.y = 0;
		deltaY23 = edges[1].current.y - edges[1].start.y;
		edges[1].current.x = edges[1].start.x + edges[1].GetSlopeX()*deltaY23;
		edges[1].current.z = edges[1].start.z + edges[1].GetSlopeZ()*deltaY23;
		edges[1].current.n_x = edges[1].start.n_x + edges[1].GetSlopeNX()*deltaY23;
		edges[1].current.n_y = edges[1].start.n_y + edges[1].GetSlopeNY()*deltaY23;
		edges[1].current.n_z = edges[1].start.n_z + edges[1].GetSlopeNZ()*deltaY23;
		/*edges[1].current.r = edges[1].start.r + edges[1].GetSlopeRed()*deltaY23;
		edges[1].current.g = edges[1].start.g + edges[1].GetSlopeGreen()*deltaY23;
		edges[1].current.b = edges[1].start.b + edges[1].GetSlopeBlue()*deltaY23;*/
		edges[1].current.u = edges[1].start.u + edges[1].GetSlopeU()*deltaY23;
		edges[1].current.v = edges[1].start.v + edges[1].GetSlopeV()*deltaY23;

		while (edges[1].current.y <= edges[1].end.y && edges[1].current.y < render->display->yres) {
			if (triType == LEFT)
				spanline.Initialize(edges[1].current, edges[2].current);
			else
				spanline.Initialize(edges[2].current, edges[1].current);
			if (edges[1].current.y != edges[2].current.y) break;
			spanline.current.x = ceil(spanline.start.x);
			if (spanline.current.x < 0) spanline.current.x = 0;
			spanline.current.y = edges[1].current.y;
			deltaX = spanline.current.x - spanline.start.x;
			spanline.current.z = spanline.start.z + spanline.GetSlopeZ()*deltaX;
			spanline.current.n_x = spanline.start.n_x + spanline.GetSlopeNX()*deltaX;
			spanline.current.n_y = spanline.start.n_y + spanline.GetSlopeNY()*deltaX;
			spanline.current.n_z = spanline.start.n_z + spanline.GetSlopeNZ()*deltaX;
			/*spanline.current.r = spanline.start.r + spanline.GetSlopeRed()*deltaX;
			spanline.current.g = spanline.start.g + spanline.GetSlopeGreen()*deltaX;
			spanline.current.b = spanline.start.b + spanline.GetSlopeBlue()*deltaX;*/
			spanline.current.u = spanline.start.u + spanline.GetSlopeU()*deltaX;
			spanline.current.v = spanline.start.v + spanline.GetSlopeV()*deltaX;

			while (spanline.current.x <= spanline.end.x && spanline.current.x < render->display->xres) {
				currentx = (int)spanline.current.x;
				currenty = (int)spanline.current.y;
				GzGetDisplay(render->display, currentx, currenty,
					&red, &green, &blue, &alpha, &bufferz);
				if (spanline.current.z < (float)bufferz && spanline.current.z > 0) {
					if (render->interp_mode == GZ_COLOR)
					{
						indexInA[U] = spanline.current.u;
						indexInA[V] = spanline.current.v;
						if (DO_P_COR)
							transToA(spanline.current, indexInA);

						//GzColor texColor{ 1.0f, 1.0f, 1.0f };
						//GzColor texColor;
						texColor[RED] = 1.0f;
						texColor[GREEN] = 1.0f;
						texColor[BLUE] = 1.0f;

						if (render->tex_fun)
							render->tex_fun(indexInA[U], indexInA[V], texColor);

						GzPutDisplay(render->display, currentx, currenty, ctoi(texColor[RED] * spanline.current.r),
							ctoi(texColor[GREEN] * spanline.current.g), ctoi(texColor[BLUE] * spanline.current.b),
							1, (GzDepth)spanline.current.z);
					}
					else if (render->interp_mode == GZ_NORMALS) {
						
						GzTextureIndex indexInA;
						indexInA[U] = spanline.current.u;
						indexInA[V] = spanline.current.v;
						if (DO_P_COR)
							transToA(spanline.current, indexInA);

						if (DO_GLITTER) {
							GzColor texColor;
							GzTextureIndex lefttopUV;
							GzTextureIndex bottomrightUV;
							lefttopUV[U] = spanline.current.u - spanline.GetSlopeU()*0.5;
							lefttopUV[V] = spanline.current.v - spanline.GetSlopeV()*0.5;
							bottomrightUV[U] = spanline.current.u + spanline.GetSlopeU()*0.5;
							bottomrightUV[V] = spanline.current.v + spanline.GetSlopeV()*0.5;
							transToA(spanline.current.z, lefttopUV);
							transToA(spanline.current.z, bottomrightUV);

							render->tex_fun(indexInA[U], indexInA[V], texColor);
							GzShade_Glitter(render, &spanline.current, texColor, lefttopUV, bottomrightUV);

						}
						else {
							if (render->tex_fun) {
								GzColor texColor;
								render->tex_fun(indexInA[U], indexInA[V], texColor);
								GzShade(render, &spanline.current, texColor);
							}
							else
								GzShade(render, &spanline.current);
						}

						GzPutDisplay(render->display, currentx, currenty, ctoi(spanline.current.r), ctoi(spanline.current.g),
							ctoi(spanline.current.b), 1, (GzDepth)spanline.current.z);
					}
					else
						GzPutDisplay(render->display, currentx, currenty, ctoi(render->flatcolor[RED]), ctoi(render->flatcolor[GREEN]),
							ctoi(render->flatcolor[BLUE]), 1, (GzDepth)spanline.current.z);
				}
				spanline.current.x++;
				deltaX = spanline.current.x - spanline.start.x;
				spanline.current.z = spanline.start.z + spanline.GetSlopeZ()*deltaX;
				spanline.current.n_x = spanline.start.n_x + spanline.GetSlopeNX()*deltaX;
				spanline.current.n_y = spanline.start.n_y + spanline.GetSlopeNY()*deltaX;
				spanline.current.n_z = spanline.start.n_z + spanline.GetSlopeNZ()*deltaX;
				/*spanline.current.r = spanline.start.r + spanline.GetSlopeRed()*deltaX;
				spanline.current.g = spanline.start.g + spanline.GetSlopeGreen()*deltaX;
				spanline.current.b = spanline.start.b + spanline.GetSlopeBlue()*deltaX;*/
				spanline.current.u = spanline.start.u + spanline.GetSlopeU()*deltaX;
				spanline.current.v = spanline.start.v + spanline.GetSlopeV()*deltaX;
			}

			edges[1].current.y++;
			deltaY23 = edges[1].current.y - edges[1].start.y;
			edges[1].current.x = edges[1].start.x + edges[1].GetSlopeX()*deltaY23;
			edges[1].current.z = edges[1].start.z + edges[1].GetSlopeZ()*deltaY23;
			edges[1].current.n_x = edges[1].start.n_x + edges[1].GetSlopeNX()*deltaY23;
			edges[1].current.n_y = edges[1].start.n_y + edges[1].GetSlopeNY()*deltaY23;
			edges[1].current.n_z = edges[1].start.n_z + edges[1].GetSlopeNZ()*deltaY23;
			/*edges[1].current.r = edges[1].start.r + edges[1].GetSlopeRed()*deltaY23;
			edges[1].current.g = edges[1].start.g + edges[1].GetSlopeGreen()*deltaY23;
			edges[1].current.b = edges[1].start.b + edges[1].GetSlopeBlue()*deltaY23;*/
			edges[1].current.u = edges[1].start.u + edges[1].GetSlopeU()*deltaY23;
			edges[1].current.v = edges[1].start.v + edges[1].GetSlopeV()*deltaY23;

			edges[2].current.y++;
			deltaY13 = edges[2].current.y - edges[2].start.y;
			edges[2].current.x = edges[2].start.x + edges[2].GetSlopeX()*deltaY13;
			edges[2].current.z = edges[2].start.z + edges[2].GetSlopeZ()*deltaY13;
			edges[2].current.n_x = edges[2].start.n_x + edges[2].GetSlopeNX()*deltaY13;
			edges[2].current.n_y = edges[2].start.n_y + edges[2].GetSlopeNY()*deltaY13;
			edges[2].current.n_z = edges[2].start.n_z + edges[2].GetSlopeNZ()*deltaY13;
			/*edges[2].current.r = edges[2].start.r + edges[2].GetSlopeRed()*deltaY13;
			edges[2].current.g = edges[2].start.g + edges[2].GetSlopeGreen()*deltaY13;
			edges[2].current.b = edges[2].start.b + edges[2].GetSlopeBlue()*deltaY13;*/
			edges[2].current.u = edges[2].start.u + edges[2].GetSlopeU()*deltaY13;
			edges[2].current.v = edges[2].start.v + edges[2].GetSlopeV()*deltaY13;

		}
	}
}

float GetRadian(const float degree) {
	return (degree / 180.0) * M_PI;
}

float GzDotProduct(const GzCoord a, const GzCoord b) {
	return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];
}

void GzCrossProduct(GzCoord result, const GzCoord a, const GzCoord b) {
	
	result[X] = a[Y] * b[Z] - a[Z] * b[Y];
	result[Y] = a[Z] * b[X] - a[X] * b[Z];
	result[Z] = a[X] * b[Y] - a[Y] * b[X];

}

void GzNormalize(GzCoord a) {
	float vectorLength = sqrt((a[X]) * (a[X]) + (a[Y]) * (a[Y]) + (a[Z]) * (a[Z]));
	a[X] = a[X] / vectorLength;
	a[Y] = a[Y] / vectorLength;
	a[Z] = a[Z] / vectorLength;
}
//matrix multiplication
void GzMultipyMatrix(GzMatrix result, const GzMatrix left, const GzMatrix right) {
	for (int i = 0; i < 4;i++) {
		for (int j = 0;j < 4;j++) {
			result[i][j] = 0;
		}
	}
	for (int i = 0; i < 4;i++) {
		for (int j = 0;j < 4;j++) {
			for (int k = 0;k < 4; k++) {
				result[i][j] = result[i][j]+left[i][k]*right[k][j];
			}			
		}
	}
}

//apply transformations to triangle vertexes
void GzTransformTri(GzMatrix matrix, GzCoord* vertexes ,bool isNormal, int count) {
	float* Temp = new float[4];
	float temp_v = 0.0;
	for (int i = 0; i < count;i++) {
		Temp[X] = vertexes[i][X];
		Temp[Y] = vertexes[i][Y];
		Temp[Z] = vertexes[i][Z];
		if (isNormal)
			Temp[3] = 0.0f;
		else
			Temp[3] = 1.0f;

		for (int j = 0;j < 3;j++) {
			vertexes[i][j] = matrix[j][X] * Temp[X] + matrix[j][Y] * Temp[Y] + matrix[j][Z] * Temp[Z] + matrix[j][3] * Temp[3];
		}

		if (!isNormal) {
			temp_v = matrix[3][X] * Temp[X] + matrix[3][Y] * Temp[Y] + matrix[3][Z] * Temp[Z] + matrix[3][3] * Temp[3];

			vertexes[i][X] /= temp_v;
			vertexes[i][Y] /= temp_v;
			vertexes[i][Z] /= temp_v;
		}
		else {
			GzNormalize(vertexes[i]);
		}
		
	}

	delete Temp;
}

//set up xsp
int GzSetXsp(GzRender* render, const unsigned short xres, const unsigned short yres) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			render->Xsp[i][j] = 0;
		}
	}


	render->Xsp[0][0] = xres / 2.0;
	render->Xsp[0][3] = xres / 2.0;
	render->Xsp[1][1] = -yres/ 2.0;
	render->Xsp[1][3] = yres / 2.0;
	render->Xsp[2][2] = MAXINT;
	render->Xsp[3][3] = 1;

	return GZ_SUCCESS;
}

//set up xpi adn xiw
int GzSetXpiXiw(GzRender* render) {
	float FOV_radian =GetRadian(render->camera.FOV);
	for (int i = 0; i < 4;i++) {
		for (int j = 0;j < 4;j++) {
			render->camera.Xpi[i][j] = 0;
		}
	}

	float one_d = tan(FOV_radian/2);
	
	render->camera.Xpi[0][0] = 1.0;
	render->camera.Xpi[1][1] = 1.0;
	render->camera.Xpi[3][3] = 1.0;
	render->camera.Xpi[2][2] = one_d;
	render->camera.Xpi[3][2] = one_d;

	GzCoord* worldCoord = new GzCoord[3];
	worldCoord[Z][X] = render->camera.lookat[X] - render->camera.position[X];
	worldCoord[Z][Y] = render->camera.lookat[Y] - render->camera.position[Y];
	worldCoord[Z][Z] = render->camera.lookat[Z] - render->camera.position[Z];
	GzNormalize(worldCoord[Z]);

	worldCoord[Y][X] = render->camera.worldup[X] - GzDotProduct(render->camera.worldup, worldCoord[Z])*(worldCoord[Z][X]);
	worldCoord[Y][Y] = render->camera.worldup[Y] - GzDotProduct(render->camera.worldup, worldCoord[Z])*(worldCoord[Z][Y]);
	worldCoord[Y][Z] = render->camera.worldup[Z] - GzDotProduct(render->camera.worldup, worldCoord[Z])*(worldCoord[Z][Z]);
	GzNormalize(worldCoord[Y]);

	GzCrossProduct(worldCoord[X], worldCoord[Y], worldCoord[Z]);

	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			render->camera.Xiw[i][j] = worldCoord[i][j];
		}
	}

	render->camera.Xiw[X][3] = -GzDotProduct(render->camera.position, worldCoord[X]);
	render->camera.Xiw[Y][3] = -GzDotProduct(render->camera.position, worldCoord[Y]);
	render->camera.Xiw[Z][3] = -GzDotProduct(render->camera.position, worldCoord[Z]);
	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;
	render->camera.Xiw[3][3] = 1;

	delete[] worldCoord;

	return GZ_SUCCESS;
}

//check if there are any vertexes behind the camera
bool CheckVertexesZ(Vertex* vertexes) {
	for (int i = 0;i < 3;i++) {
		if (vertexes[i].z < 0) return true;
	}
	return false;
}

int GzShade(GzRender *render, Vertex* vtx, GzColor textColor) {

	GzColor* shadeResult = new GzColor[1];

	GzCoord* eyeDir = new GzCoord[1]; //minus z direction
	(*eyeDir)[X] = 0.0;
	(*eyeDir)[Y] = 0.0;
	(*eyeDir)[Z] = -1.0;

	GzCoord* normal = new GzCoord[1];
	(*normal)[X] = (*vtx).n_x;
	(*normal)[Y] = (*vtx).n_y;
	(*normal)[Z] = (*vtx).n_z;

	GzNormalize(*normal);

	//ambient componet
	if (textColor == NULL)
	{
		(*shadeResult)[RED] = render->ambientlight.color[RED] * render->Ka[RED];
		(*shadeResult)[GREEN] = render->ambientlight.color[GREEN] * render->Ka[GREEN];
		(*shadeResult)[BLUE] = render->ambientlight.color[BLUE] * render->Ka[BLUE];
	}
	
	else
	{
		(*shadeResult)[RED] = render->ambientlight.color[RED] * textColor[RED];
		(*shadeResult)[GREEN] = render->ambientlight.color[GREEN] * textColor[GREEN];
		(*shadeResult)[BLUE] = render->ambientlight.color[BLUE] * textColor[BLUE];
	}

	//diffuse and specular component
	GzCoord* reflection = new GzCoord[1];

	GzCoord* lightDir = new GzCoord[1];

	GzCoord MicroBRDF, fresnel, diffuseRatioV;

	float NdotL, NdotE, RdotE, RESpec, MicroBRDF0, diffuseRatio;

	for (int i = 0; i < render->numlights; i++) {

		(*lightDir)[X] = render->lights[i].direction[X];
		(*lightDir)[Y] = render->lights[i].direction[Y];
		(*lightDir)[Z] = render->lights[i].direction[Z];
 
		//GzTransformTri(render->Xnorm[render->matlevel - 1], lightDir, true, 1);

		NdotL = GzDotProduct(*normal, *lightDir);
		NdotE = GzDotProduct(*normal, *eyeDir);

		if (NdotL*NdotE <= 0)
			continue;

		float fresnel_0 = pow((1 - render->refractiveIndex) / (1 + render->refractiveIndex), 2);

		GzCoord fresnel;
		fresnel[RED] = 0.98f * fresnel_0;
		fresnel[GREEN] = 0.82f * fresnel_0;
		fresnel[BLUE] = 0.76f * fresnel_0;

		MicroBRDF0 = MicrofacetBRDF(lightDir, eyeDir, normal, render->roughness, fresnel_0);
		MicroBRDF[RED] = MicrofacetBRDF(lightDir, eyeDir, normal, render->roughness, fresnel[RED]);
		MicroBRDF[GREEN] = MicrofacetBRDF(lightDir, eyeDir, normal, render->roughness, fresnel[GREEN]);
		MicroBRDF[BLUE] = MicrofacetBRDF(lightDir, eyeDir, normal, render->roughness, fresnel[BLUE]);

		diffuseRatio = diffuseEnergyRatio(lightDir, normal, fresnel_0);
		diffuseRatioV[RED] = diffuseEnergyRatio(lightDir, normal, fresnel[RED]);
		diffuseRatioV[GREEN] = diffuseEnergyRatio(lightDir, normal, fresnel[GREEN]);
		diffuseRatioV[BLUE] = diffuseEnergyRatio(lightDir, normal, fresnel[BLUE]);



		if (NdotL < 0) {
			(*normal)[X] = -(*normal)[X];
			(*normal)[Y] = -(*normal)[Y];
			(*normal)[Z] = -(*normal)[Z];
			GzNormalize(*normal);
			NdotL = -NdotL;
			//NdotE = -NdotE;
		}

		//diffuse component
		if (textColor == NULL)
		{
			(*shadeResult)[RED] += render->lights[i].color[RED] * NdotL * render->Kd[RED] * diffuseRatioV[RED];
			(*shadeResult)[GREEN] += render->lights[i].color[GREEN] * NdotL * render->Kd[GREEN] * diffuseRatioV[GREEN];
			(*shadeResult)[BLUE] += render->lights[i].color[BLUE] * NdotL * render->Kd[BLUE] * diffuseRatioV[BLUE];
		}
		else
		{
			(*shadeResult)[RED] += render->lights[i].color[RED] * NdotL * textColor[RED] * diffuseRatio;
			(*shadeResult)[GREEN] += render->lights[i].color[GREEN] * NdotL * textColor[GREEN] * diffuseRatio;
			(*shadeResult)[BLUE] += render->lights[i].color[BLUE] * NdotL * textColor[BLUE] * diffuseRatio;
		}
		

		(*reflection)[X] = 2 * NdotL * (*normal)[X] - (*lightDir)[X];
		(*reflection)[Y] = 2 * NdotL * (*normal)[Y] - (*lightDir)[Y];
		(*reflection)[Z] = 2 * NdotL * (*normal)[Z] - (*lightDir)[Z];
		
		GzNormalize(*reflection);

		RdotE = GzDotProduct(*reflection, *eyeDir);

		if (RdotE <= 0) {
			continue;
		}

		RESpec = pow(RdotE, render->spec);
	
		//specular component

		if (textColor != NULL && render->interp_mode == GZ_COLOR)
		{
			(*shadeResult)[RED] += NdotL * render->lights[i].color[RED] * MicroBRDF[RED] * M_PI; //RESpec 
			(*shadeResult)[GREEN] += NdotL * render->lights[i].color[GREEN] * MicroBRDF[GREEN] * M_PI;
			(*shadeResult)[BLUE] += NdotL * render->lights[i].color[BLUE] * MicroBRDF[BLUE] * M_PI;
		}
		else
		{
			(*shadeResult)[RED] += NdotL * render->lights[i].color[RED] * (render->Ks[RED]) * MicroBRDF[RED] * M_PI; //RESpec
			(*shadeResult)[GREEN] += NdotL * render->lights[i].color[GREEN] * (render->Ks[GREEN]) * MicroBRDF[GREEN] * M_PI;
			(*shadeResult)[BLUE] += NdotL * render->lights[i].color[BLUE] * (render->Ks[BLUE]) * MicroBRDF[BLUE] * M_PI;
		}
		

	}

	if ((*shadeResult)[RED] > 1)
		(*shadeResult)[RED] = 1.0;

	if ((*shadeResult)[RED] < 0)
		(*shadeResult)[RED] = 0.0;

	if ((*shadeResult)[GREEN] > 1)
		(*shadeResult)[GREEN] = 1;

	if ((*shadeResult)[GREEN] < 0)
		(*shadeResult)[GREEN] = 0;

	if ((*shadeResult)[BLUE] > 1)
		(*shadeResult)[BLUE] = 1;

	if ((*shadeResult)[BLUE] < 0)
		(*shadeResult)[BLUE] = 0;

	(*vtx).r = (*shadeResult)[RED];
	(*vtx).g = (*shadeResult)[GREEN];
	(*vtx).b = (*shadeResult)[BLUE];

	delete[] eyeDir;
	delete[] lightDir;
	delete[] reflection;
	delete[] shadeResult;
	delete[] normal;
	
	return GZ_SUCCESS;
}


/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

int GzShade_Glitter(GzRender * render, Vertex* vtx, GzColor textColor, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV)
{
	textColor = NULL;
	GzColor* shadeResult = new GzColor[1];

	GzCoord* eyeDir = new GzCoord[1]; //minus z direction
	(*eyeDir)[X] = 0.0;
	(*eyeDir)[Y] = 0.0;
	(*eyeDir)[Z] = -1.0;

	GzCoord* normal = new GzCoord[1];
	(*normal)[X] = (*vtx).n_x;
	(*normal)[Y] = (*vtx).n_y;
	(*normal)[Z] = (*vtx).n_z;

	GzNormalize(*normal);

	//ambient componet
	if (true)//if (textColor == NULL)
	{
		(*shadeResult)[RED] = render->ambientlight.color[RED] * render->Ka[RED];
		(*shadeResult)[GREEN] = render->ambientlight.color[GREEN] * render->Ka[GREEN];
		(*shadeResult)[BLUE] = render->ambientlight.color[BLUE] * render->Ka[BLUE];
	}

	else
	{
		(*shadeResult)[RED] = render->ambientlight.color[RED] * textColor[RED];
		(*shadeResult)[GREEN] = render->ambientlight.color[GREEN] * textColor[GREEN];
		(*shadeResult)[BLUE] = render->ambientlight.color[BLUE] * textColor[BLUE];
	}

	//diffuse and specular component
	GzCoord* reflection = new GzCoord[1];

	GzCoord* lightDir = new GzCoord[1];

	GzCoord MicroBRDF, fresnel, diffuseRatioV;

	float NdotL, NdotE, RdotE, RESpec, MicroBRDF0, diffuseRatio;

	for (int i = 0; i < render->numlights; i++) {

		(*lightDir)[X] = render->lights[i].direction[X];
		(*lightDir)[Y] = render->lights[i].direction[Y];
		(*lightDir)[Z] = render->lights[i].direction[Z];

		//GzTransformTri(render->Xnorm[render->matlevel - 1], lightDir, true, 1);

		NdotL = GzDotProduct(*normal, *lightDir);
		NdotE = GzDotProduct(*normal, *eyeDir);

		if (NdotL*NdotE <= 0)
			continue;

		float fresnel_0 = pow((1 - render->refractiveIndex) / (1 + render->refractiveIndex), 2);

		GzCoord fresnel;
		fresnel[RED] = 0.98f * fresnel_0;
		fresnel[GREEN] = 0.82f * fresnel_0;
		fresnel[BLUE] = 0.76f * fresnel_0;

		MicroBRDF0 = Microfacet_Glitter_BRDF(lightDir, eyeDir, normal, render->roughness, fresnel_0, lefttopUV, bottomrightUV);
		MicroBRDF[RED] = Microfacet_Glitter_BRDF(lightDir, eyeDir, normal, render->roughness, fresnel[RED], lefttopUV, bottomrightUV);
		MicroBRDF[GREEN] = Microfacet_Glitter_BRDF(lightDir, eyeDir, normal, render->roughness, fresnel[GREEN], lefttopUV, bottomrightUV);
		MicroBRDF[BLUE] = Microfacet_Glitter_BRDF(lightDir, eyeDir, normal, render->roughness, fresnel[BLUE], lefttopUV, bottomrightUV);

		diffuseRatio = diffuseEnergyRatio(lightDir, normal, fresnel_0);
		diffuseRatioV[RED] = diffuseEnergyRatio(lightDir, normal, fresnel[RED]);
		diffuseRatioV[GREEN] = diffuseEnergyRatio(lightDir, normal, fresnel[GREEN]);
		diffuseRatioV[BLUE] = diffuseEnergyRatio(lightDir, normal, fresnel[BLUE]);



		if (NdotL < 0) {
			(*normal)[X] = -(*normal)[X];
			(*normal)[Y] = -(*normal)[Y];
			(*normal)[Z] = -(*normal)[Z];
			GzNormalize(*normal);
			NdotL = -NdotL;
			//NdotE = -NdotE;
		}

		//diffuse component
		if (textColor == NULL)
		{
			(*shadeResult)[RED] += render->lights[i].color[RED] * NdotL * render->Kd[RED] * diffuseRatioV[RED];
			(*shadeResult)[GREEN] += render->lights[i].color[GREEN] * NdotL * render->Kd[GREEN] * diffuseRatioV[GREEN];
			(*shadeResult)[BLUE] += render->lights[i].color[BLUE] * NdotL * render->Kd[BLUE] * diffuseRatioV[BLUE];
		}
		else
		{
			(*shadeResult)[RED] += render->lights[i].color[RED] * NdotL * textColor[RED] * diffuseRatio;
			(*shadeResult)[GREEN] += render->lights[i].color[GREEN] * NdotL * textColor[GREEN] * diffuseRatio;
			(*shadeResult)[BLUE] += render->lights[i].color[BLUE] * NdotL * textColor[BLUE] * diffuseRatio;
		}


		(*reflection)[X] = 2 * NdotL * (*normal)[X] - (*lightDir)[X];
		(*reflection)[Y] = 2 * NdotL * (*normal)[Y] - (*lightDir)[Y];
		(*reflection)[Z] = 2 * NdotL * (*normal)[Z] - (*lightDir)[Z];

		GzNormalize(*reflection);

		RdotE = GzDotProduct(*reflection, *eyeDir);

		if (RdotE <= 0) {
			continue;
		}

		RESpec = pow(RdotE, render->spec);

		//specular component

		if (textColor != NULL && render->interp_mode == GZ_COLOR)
		{
			(*shadeResult)[RED] += NdotL * render->lights[i].color[RED] * MicroBRDF[RED] * M_PI; //RESpec 
			(*shadeResult)[GREEN] += NdotL * render->lights[i].color[GREEN] * MicroBRDF[GREEN] * M_PI;
			(*shadeResult)[BLUE] += NdotL * render->lights[i].color[BLUE] * MicroBRDF[BLUE] * M_PI;
		}
		else
		{
			(*shadeResult)[RED] += NdotL * render->lights[i].color[RED] * (render->Ks[RED]) * MicroBRDF[RED] * M_PI; //RESpec
			(*shadeResult)[GREEN] += NdotL * render->lights[i].color[GREEN] * (render->Ks[GREEN]) * MicroBRDF[GREEN] * M_PI;
			(*shadeResult)[BLUE] += NdotL * render->lights[i].color[BLUE] * (render->Ks[BLUE]) * MicroBRDF[BLUE] * M_PI;
		}


	}

	if ((*shadeResult)[RED] > 1)
		(*shadeResult)[RED] = 1.0;

	if ((*shadeResult)[RED] < 0)
		(*shadeResult)[RED] = 0.0;

	if ((*shadeResult)[GREEN] > 1)
		(*shadeResult)[GREEN] = 1;

	if ((*shadeResult)[GREEN] < 0)
		(*shadeResult)[GREEN] = 0;

	if ((*shadeResult)[BLUE] > 1)
		(*shadeResult)[BLUE] = 1;

	if ((*shadeResult)[BLUE] < 0)
		(*shadeResult)[BLUE] = 0;


	(*vtx).r = (*shadeResult)[RED];
	(*vtx).g = (*shadeResult)[GREEN];
	(*vtx).b = (*shadeResult)[BLUE];

	delete eyeDir, lightDir, reflection, shadeResult;

	return GZ_SUCCESS;
}

