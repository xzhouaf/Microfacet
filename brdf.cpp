#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"rend.h"
#include	"brdf.h"
#include <stdio.h>      
#include <math.h>

unsigned int step(float _Y, float _X)
{
	return _X >= _Y;
}

float FresnelApprox(GzCoord* l, GzCoord* h, float fresnel_0)
{	
	float costheta = GzDotProduct(*l, *h);
	return fresnel_0 + (1 - fresnel_0) * pow(1 - costheta, 5);
}

float BeckMannDis(GzCoord* n, GzCoord* h, float roughness)
{	
	float cos = max(GzDotProduct(*n, *h), 0.0f);

	if (roughness < 0) roughness = 0;
	
	float param = (cos * cos - 1) / (roughness * roughness * cos *cos);

	return exp(param) / (M_PI * roughness * roughness * pow(cos, 4));

}

float BlinnPhongdis(GzCoord* n, GzCoord* h, float roughness)
{
	float cos = max(GzDotProduct(*n, *h), 0.0f);

	float alpha = 2 / (roughness * roughness) - 2;
	
	return (alpha + 2) / (2 * M_PI) * pow(cos, alpha);
}

float IGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness)
{
	float NdotL = GzDotProduct(*n, *l);
	float NdotV = GzDotProduct(*n, *v);

	float NdotL_clamped = max(NdotL, 0.0);
	float NdotV_clamped = max(NdotV, 0.0);

	return NdotL_clamped * NdotV_clamped;
}

float CGeomerty(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness)
{
	float NdotH = GzDotProduct(*n, *h);
	float NdotL = GzDotProduct(*n, *l);
	float NdotV = GzDotProduct(*n, *v);
	float VdotH = GzDotProduct(*v, *h);

	float NdotL_clamped = max(NdotL, 0.0);
	float NdotV_clamped = max(NdotV, 0.0);

	return min(min(2.0 * NdotH * NdotV_clamped / VdotH, 2.0 * NdotH * NdotL_clamped / VdotH), 1.0);
}

float SGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness)
{
	float NdotV = GzDotProduct(*n, *v);
	float NdotL = GzDotProduct(*n, *l);

	float NdotL_clamped = max(NdotL, 0.0);
	float NdotV_clamped = max(NdotV, 0.0);
	float k = roughness * sqrt(2.0 / M_PI);
	float one_minus_k = 1.0 - k;
	return (NdotL_clamped / (NdotL_clamped * one_minus_k + k)) * (NdotV_clamped / (NdotV_clamped * one_minus_k + k));
}

float WGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness)
{
	float NdotV = GzDotProduct(*n, *v);
	float NdotL = GzDotProduct(*n, *l);
	float HdotV = GzDotProduct(*h, *v);
	float HdotL = GzDotProduct(*h, *l);

	float NdotL_clamped = max(NdotL, 0.0);
	float NdotV_clamped = max(NdotV, 0.0);
	
	float a0 = 1.0 / (roughness * tan(acos(NdotV_clamped)));
	float a_Sq0 = a0 * a0;
	float a_term0;
	if (a0<1.6)
		a_term0 = (3.535 * a0 + 2.181f * a_Sq0) / (1.0f + 2.276f * a0 + 2.577f * a_Sq0);
	else
		a_term0 = 1.0;
	
	float a1 = 1.0 /  (roughness * tan(acos(NdotL_clamped)));
	float a_Sq1 = a1 * a1;
	float a_term1;
	if (a1<1.6)
		a_term1 = (3.535f * a1 + 2.181f * a_Sq1) / (1.0f + 2.276f * a1 + 2.577f * a_Sq1);
	else
		a_term1 = 1.0;

	return  (step(0.0f, HdotL / NdotL) * a_term1) * (step(0.0f, HdotV / NdotV) * a_term0);
}

float MicrofacetBRDF(GzCoord* light, GzCoord* view, GzCoord* macroNormal, float roughness, float fresnel_0)
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

	float distributionTerm = BeckMannDis(&flippedNormal, &h, roughness);

	float geomertyTerm = WGeometry(&flippedNormal, &h, view, light, roughness);

	

	return fresnelTerm * distributionTerm * geomertyTerm / (4.0f * NdotL_clamped * NdotV_clamped);
}

float diffuseEnergyRatio(GzCoord* l, GzCoord* n, float fresnel_0)
{
	return 1 - FresnelApprox(l, n, fresnel_0);
	// return 1 - fresnel_0;
	//return 1;
}

void gammaCorrection(GzCoord* color)
{

}