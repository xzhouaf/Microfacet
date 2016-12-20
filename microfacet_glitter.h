#ifndef MICRIO_GLITTER_H_
#define MICRIO_GLITTER_H_


//BRDF microfacet functions
float Discrete_Facet_Distibution(GzCoord* light, GzCoord* view, GzCoord* macroNormal, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV);
float Microfacet_Glitter_BRDF(GzCoord* light, GzCoord* view, GzCoord* macroNormal, float roughness, float fresnel_0, GzTextureIndex lefttopUV, GzTextureIndex bottomrightUV);
#endif