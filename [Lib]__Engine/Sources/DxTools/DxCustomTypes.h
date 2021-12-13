#ifndef	__DXCUSTOMTYPES__H__
#define	__DXCUSTOMTYPES__H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

struct OCTREEVERTEX
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			D3DXVECTOR3 vPos;
		};
	};

	union
	{
		struct
		{
			float nx, ny, nz;
		};
		struct
		{
			D3DXVECTOR3 vNormal;
		};
	};

	float tu, tv;
	
	OCTREEVERTEX() :
		x(0),y(0),z(0),
		nx(0.5f),ny(0.5f),nz(0.5f),
		tu(0),tv(0)
	{
	}
	OCTREEVERTEX(float X , float Y, float Z) :
		nx(0.5f),ny(0.5f),nz(0.5f),
		tu(0),tv(0)
	{ 
	}
	const static DWORD FVF;
};

struct CLIPVOLUME
{
	D3DXPLANE pLeft, pRight;
	D3DXPLANE pTop, pBottom;
	D3DXPLANE pNear, pFar;

	CLIPVOLUME() {}
};

struct	DIFFUSEVERTEX
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			D3DXVECTOR3 vPos;
		};
	};
    
	DWORD	Diffuse;

public:
	DIFFUSEVERTEX ()
	{		
	}

	DIFFUSEVERTEX ( float _x, float _y, float _z, DWORD diffuse )
	{
		x = _x; y = _y; z = _z; Diffuse = diffuse;
	}
	DIFFUSEVERTEX ( D3DXVECTOR3 pos, DWORD diffuse )
	{
		vPos = pos; Diffuse = diffuse;
	}

	const static DWORD FVF;
};

typedef std::list<DWORD>			LOADINGDATALIST;
typedef std::list<DWORD>::iterator	LOADINGDATALIST_ITER;

#endif	//	__DXCUSTOMTYPES__H__