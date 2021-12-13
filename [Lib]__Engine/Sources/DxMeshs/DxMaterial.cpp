#include "pch.h"
#include "./DxMaterial.h"
#include "./TextureManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DXMATERIAL::RELEASE ()
{
	if( pTexture )	TextureManager::ReleaseTexture ( strTexture.c_str(), pTexture );
}

DXMATERIAL_SPECULAR::~DXMATERIAL_SPECULAR()
{
	if( pSpecTex )	TextureManager::ReleaseTexture ( szSpecTex, pSpecTex );
}

DXMATERIAL_CHAR_EFF::~DXMATERIAL_CHAR_EFF()
{
	if( pEffTex )	TextureManager::ReleaseTexture( szEffTex, pEffTex );
}