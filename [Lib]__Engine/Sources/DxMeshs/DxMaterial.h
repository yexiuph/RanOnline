//
//	[struct DXMATERIAL], JDH, (2002.12.02)
//		메터리얼과 텍스쳐, 메터리얼(텍스쳐) 속성값을 가지고 있는 구조체.
//		이들 데이터는 같이 있을 경우가 많으므로 이렇게 처리함.
//
#pragma once
#include <string>

struct DXMATERIAL
{
	enum
	{
		FLAG_ALPHA_HARD		= 0x0001,
		FLAG_ALPHA_SOFT		= 0x0002,
	};

	DWORD				dwFlags;
	LPDIRECT3DTEXTUREQ	pTexture;
	std::string			strTexture;

	DXMATERIAL () :
		dwFlags(NULL),
		pTexture(NULL)
	{
		strTexture = "";
	}

	void RELEASE ();
};

struct DXMATERIAL_SPECULAR
{
	BOOL				bSpecUse;
	LPDIRECT3DTEXTUREQ	pSpecTex;
	char				szTexture[MAX_PATH];
	char				szSpecTex[MAX_PATH];

	DXMATERIAL_SPECULAR () :
		bSpecUse(FALSE),
		pSpecTex(NULL)
	{
		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szSpecTex, 0, sizeof(char) * (MAX_PATH));
	}

	~DXMATERIAL_SPECULAR();
};

struct DXMATERIAL_CHAR_EFF_100
{
	BOOL				bEffUse;
	float				fMaterial_Power;
	LPDIRECT3DTEXTUREQ	pEffTex;
	char				szTexture[MAX_PATH];
	char				szEffTex[MAX_PATH];

	DXMATERIAL_CHAR_EFF_100()
		: pEffTex(NULL)
	{
		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
	}
};

struct DXMATERIAL_CHAR_EFF
{
	BOOL				bEffUse;
	D3DMATERIALQ		d3dMaterial;
	LPDIRECT3DTEXTUREQ	pEffTex;
	char				szTexture[MAX_PATH];
	char				szEffTex[MAX_PATH];

	DXMATERIAL_CHAR_EFF() :
		bEffUse(FALSE),
		pEffTex(NULL)
	{
		d3dMaterial.Diffuse.r	= 1.f;
		d3dMaterial.Diffuse.g	= 1.f;
		d3dMaterial.Diffuse.b	= 1.f;
		d3dMaterial.Diffuse.a	= 1.f;
		d3dMaterial.Ambient		= d3dMaterial.Diffuse;
		d3dMaterial.Specular	= d3dMaterial.Diffuse;
		d3dMaterial.Emissive	= d3dMaterial.Diffuse;
		d3dMaterial.Power		= 14.f;

		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
	}

	~DXMATERIAL_CHAR_EFF();
};

//
//struct DXMATERIAL_CHAR_EFF
//{
//	BOOL				bEffUse;
//	D3DMATERIALQ		d3dMaterial;
//	DWORD				dwDiffuse;		// Optimize - 최적화 때문에 쓰인다.
//	LPDIRECT3DTEXTUREQ	pEffTex;
//	char				szTexture[MAX_PATH];
//	char				szEffTex[MAX_PATH];
//
//	DXMATERIAL_CHAR_EFF () :
//		bEffUse(FALSE),
//		pEffTex(NULL)
//	{
//		d3dMaterial.Diffuse.r	= 1.f;
//		d3dMaterial.Diffuse.g	= 1.f;
//		d3dMaterial.Diffuse.b	= 1.f;
//		d3dMaterial.Diffuse.a	= 1.f;
//		d3dMaterial.Ambient		= d3dMaterial.Diffuse;
//		d3dMaterial.Specular	= d3dMaterial.Diffuse;
//		d3dMaterial.Emissive	= d3dMaterial.Diffuse;
//		d3dMaterial.Power		= 14.f;
//
//		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
//		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
//	}
//
//	~DXMATERIAL_CHAR_EFF();
//};

