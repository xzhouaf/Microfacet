#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"rend.h"
#include	"brdf.h"
#include	"microfacet_glitter.h"
#include <stdio.h>      
#include <math.h>

#define PARTICLE_PERCENT 0.9
#define GLITTER_RATIO	0.95
#define PARTICLE_NUMBER 3000
#define ROUGHNESS	0.5

// calculate base^p%mod
int power_mod(int base, int p, int mod) {
	if (p == 0)
		return 1;
	else if (p == 1) {
		int mode_value = base%mod;
		return mode_value<0 ? mode_value + mod : mode_value;
	}

	int half_p = p / 2;
	int product = power_mod(base, half_p, mod)*power_mod(base, p - half_p, mod);
	int mode_value = product%mod;
	return mode_value<0?mode_value+mod:mode_value;
}

// procedual judge whether there is a particle or not, and genearte its direction
bool ParticleGeneration(int position, float percent, GzCoord* macroNormal, GzCoord* normal) {
	// input : h: the macro normal
	// output : normal of the particle
	int seed = position;
	int pq = 43 * 59;
	int rand1 = power_mod(seed, 13,pq);
	int percentile = pq*percent;
	bool isParticle = rand1 > percentile;
	if (isParticle)
	{
		// sample its direction
		// find a direction		
		GzCoord p;
		int pq2 = 53 * 59;
		int rand2 = power_mod(seed, 17, pq2);
		p[X] = (float)rand2 / (float)pq2;

		int pq3 = 53 * 83;
		int rand3 = power_mod(seed, 19, pq3);
		p[Y] = (float)rand3 / (float)pq3;

		p[Z] = 1;

		GzCoord perturb;
		perturb[X] = (*macroNormal)[Y] * p[Z] - (*macroNormal)[Z] * p[Y];
		perturb[Y] = (*macroNormal)[Z] * p[X] - (*macroNormal)[X] * p[Z];
		perturb[Z] = (*macroNormal)[X] * p[Y] - (*macroNormal)[Y] * p[Z];
		GzNormalize(perturb);
		float r1 = pow((float)power_mod(seed, 19, 59 * 83) / float(59 * 83), 4.0);
		float len = (sqrt(-log(1-r1)*ROUGHNESS*ROUGHNESS));
		//len = 0;

		(*normal)[X] = (*macroNormal)[X] + perturb[X]*len;
		(*normal)[Y] = (*macroNormal)[Y] + perturb[Y]*len;
		(*normal)[Z] = (*macroNormal)[Z] + perturb[Z]*len;

		GzNormalize(*normal);
	}

	return isParticle;
}


float Discrete_Facet_Distibution(GzCoord* light, GzCoord* view, GzCoord* macroNormal, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV)
{
	float N = PARTICLE_NUMBER;
	float u_res = 1 / N;
	int left, right, bottom, top;
	left = min(lefttopUV[U]/ u_res, bottomrightUV[U] / u_res);
	top = min(lefttopUV[V] / u_res, bottomrightUV[V] / u_res);
	right = max(lefttopUV[U] / u_res, bottomrightUV[U] / u_res);
	bottom = max(lefttopUV[V] / u_res, bottomrightUV[V] / u_res);
	float phi, theta;
	int nPts = 0;
	for (int i = max(left,0); i < min(right,N); i++) {
		for (int j = max(top,0); j < min(bottom,N); j++) {
			GzCoord nfacet, o;
			if (ParticleGeneration(i*N + j, PARTICLE_PERCENT, macroNormal, &nfacet)) {

				// get the reflected direction o
				float n_dot_l = GzDotProduct(nfacet, *light);
				o[X] = 2 * n_dot_l*nfacet[X] - (*light)[X];
				o[Y] = 2 * n_dot_l*nfacet[Y] - (*light)[Y];
				o[Z] = 2 * n_dot_l*nfacet[Z] - (*light)[Z];
				GzNormalize(o);

				// judge whehter the reflected direction o is visible
				float epsilon = GLITTER_RATIO;
				if (GzDotProduct(o, *view) > epsilon)
					nPts++;
			}
		}
	}
	float area = abs((lefttopUV[U] - bottomrightUV[U])*(lefttopUV[V] - bottomrightUV[V]));
	return nPts/(1-GLITTER_RATIO)/PARTICLE_PERCENT/area/ PARTICLE_NUMBER/700;
}

float Microfacet_Glitter_BRDF(GzCoord* light, GzCoord* view, GzCoord* macroNormal, float roughness, float fresnel_0, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV)
{
	GzCoord h;
	h[X] = (*light)[X] + (*view)[X];
	h[Y] = (*light)[Y] + (*view)[Y];
	h[Z] = (*light)[Z] + (*view)[Z];
	GzNormalize(h);

	GzNormalize(*macroNormal);

	float NdotL = GzDotProduct(*macroNormal, *light);
	float NdotV = GzDotProduct(*macroNormal, *view);
	GzCoord flippedNormal;
	if (NdotV < 0 && NdotL < 0)
	{
		NdotV = -NdotV;
		NdotL = -NdotL;
		flippedNormal[X] = -(*macroNormal)[X];
		flippedNormal[Y] = -(*macroNormal)[X];
		flippedNormal[Z] = -(*macroNormal)[X];
	}
	else
	{
		flippedNormal[X] = (*macroNormal)[X];
		flippedNormal[Y] = (*macroNormal)[Y];
		flippedNormal[Z] = (*macroNormal)[Z];
	}
	float NdotL_clamped = max(NdotL, 0.0f);
	float NdotV_clamped = max(NdotV, 0.0f);

	//float fresnel_0 = pow((1 - reflectiveIndex) / (1 + reflectiveIndex), 2);
	float fresnelTerm = FresnelApprox(light, &h, fresnel_0);

	float distributionTerm = Discrete_Facet_Distibution(light, view, macroNormal,lefttopUV, bottomrightUV);
	//float distributionTerm = BeckMannDis(&flippedNormal, &h, roughness);
	if (distributionTerm > 1)
		distributionTerm += 0;

	float geomertyTerm = WGeometry(&flippedNormal, &h, view, light, roughness);

	

	return fresnelTerm * distributionTerm * geomertyTerm / (4.0f * NdotL_clamped * NdotV_clamped);
}
