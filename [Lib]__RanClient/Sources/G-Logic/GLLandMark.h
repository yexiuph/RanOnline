#ifndef GLLANDMARK_H_
#define GLLANDMARK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../[Lib]__Engine/Sources/Common/BaseStream.h"
#include "../[Lib]__Engine/Sources/DxTools/Collision.h"
#include "../[Lib]__Engine/Sources/DxTools/DxMethods.h"

struct GLLANDMARK : public DXAFFINEMATRIX, public OBJAABB
{
	enum { VERSION = 0x0001, };
	const static D3DXVECTOR3	m_vHIGHER;
	const static float			m_fSPHERE_S1;

	std::string			m_strMARK;

	D3DXVECTOR3			m_vMax;					//	AABB에 사용.
	D3DXVECTOR3			m_vMin;					//	AABB에 사용.

	GLLANDMARK*			m_pNext;

public:
	void operator= ( const GLLANDMARK &sVALUE );
	void CALC_BOX ();

public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, DXAFFINEMATRIX::m_pAffineParts->vTrans );
	}

	virtual OBJAABB* GetNext ()
	{
		return m_pNext;
	}

	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

public:
	BOOL SAVE ( CSerialFile &SFile );
	BOOL LOAD ( basestream &SFile );

public:
	GLLANDMARK ();
	~GLLANDMARK ();
};

typedef GLLANDMARK* PLANDMARK;

typedef std::vector<PLANDMARK>	VECLANDMARK;
typedef VECLANDMARK::iterator	VECLANDMARK_ITER;

namespace LANDMARK
{
	extern VECLANDMARK vecLANDMARK_DISP;
};

#endif // GLLANDMARK_H_