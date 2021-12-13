#ifndef STL_FUNC_H_
#define STL_FUNC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLDefine.h"
#include "GLCrow.h"
#include "GLChar.h"
#include "GLSummon.h"
#include "GLSummonField.h"

namespace std_afunc
{
	struct CHARDIST
	{
		float	fLeng;
		PGLCHAR	pChar;

		CHARDIST () {}
		CHARDIST ( D3DXVECTOR3 vCenter, PGLCHAR _pChar ) :
			pChar(_pChar)
		{
			D3DXVECTOR3 vLeng = pChar->GetPosition() - vCenter;
			fLeng = D3DXVec3LengthSq ( &vLeng );
		}

		CHARDIST ( float _fLeng, PGLCHAR _pChar ) :
			fLeng(_fLeng),
			pChar(_pChar)
		{
		}
	};	

	class CCompareCharDist
	{
	public:
		CCompareCharDist ()
		{
		}

	public:
		bool operator() ( const CHARDIST &CharA, const CHARDIST &CharB )
		{
			return CharA.fLeng < CharB.fLeng;
		}
	};

	struct CROWDIST
	{
		float	fLeng;
		PGLCROW	pCrow;

		CROWDIST () {}
		CROWDIST ( D3DXVECTOR3 vCenter, PGLCROW _pCrow ) :
			pCrow(_pCrow)
		{
			D3DXVECTOR3 vLeng = pCrow->GetPosition() - vCenter;
			fLeng = D3DXVec3LengthSq ( &vLeng );
		}

		CROWDIST ( float _fLeng, PGLCROW _pCrow ) :
			fLeng(_fLeng),
			pCrow(_pCrow)
		{
		}
	};

	class CCompareCrowDist
	{
	public:
		CCompareCrowDist ()
		{
		}

	public:
		bool operator() ( const CROWDIST &CharA, const CROWDIST &CharB )
		{
			return CharA.fLeng < CharB.fLeng;
		}
	};

	class CCompareTargetDist
	{
	public:
		D3DXVECTOR3 vFromPt;

	public:
		CCompareTargetDist ( D3DXVECTOR3 vFrom ) : vFromPt(vFrom) {}

	public:
		bool operator() ( const STARGETID &TarA, const STARGETID &TarB )
		{
			D3DXVECTOR3 vLengA = TarA.vPos - vFromPt;
			D3DXVECTOR3 vLengB = TarB.vPos - vFromPt;
			float fLengA = D3DXVec3Length ( &vLengA );
			float fLengB = D3DXVec3Length ( &vLengB );
			
			return fLengA < fLengB;
		}

		bool operator() ( const STARGETID *pTarA, const STARGETID *pTarB )
		{
			D3DXVECTOR3 vLengA = pTarA->vPos - vFromPt;
			D3DXVECTOR3 vLengB = pTarB->vPos - vFromPt;
			float fLengA = D3DXVec3Length ( &vLengA );
			float fLengB = D3DXVec3Length ( &vLengB );
			
			return fLengA < fLengB;
		}
	};
}

#endif // STL_FUNC_H_