#ifndef BRDF_H_
#define BRDF_H_


//BRDF microfacet functions
float FresnelApprox(GzCoord* light, GzCoord* view, float fresnel_0);
float BeckMannDis(GzCoord* n, GzCoord* h, float roughness);
float BlinnPhongdis(GzCoord* n, GzCoord* h, float roughness);
float IGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness);
float CGeomerty(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness);
float SGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness);
float WGeometry(GzCoord* n, GzCoord* h, GzCoord* v, GzCoord* l, float roughness);
float MicrofacetBRDF(GzCoord* light, GzCoord* view, GzCoord* macroNormal, float rough, float fresnel_0);

unsigned int step(float a, float b);

float diffuseEnergyRatio(GzCoord* l, GzCoord* n, float fresnel_0);

#endif