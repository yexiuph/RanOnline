#include "pch.h"
#include "./GLLandMark.h"

#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/DxTools/EditMeshs.h"
#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLLANDMARK::GLLANDMARK () :
	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_pNext(NULL)
{
	UseAffineMatrix ();
}

GLLANDMARK::~GLLANDMARK ()
{
	SAFE_DELETE(m_pNext);
}

const D3DXVECTOR3 GLLANDMARK::m_vHIGHER = D3DXVECTOR3(0,4,0);
const float GLLANDMARK::m_fSPHERE_S1 = 3.0f;

namespace LANDMARK
{
	VECLANDMARK vecLANDMARK_DISP;
};

void GLLANDMARK::operator= ( const GLLANDMARK &sVALUE )
{
	DXAFFINEMATRIX::SetAffineValue ( sVALUE.m_pAffineParts );

	m_strMARK = sVALUE.m_strMARK;

	CALC_BOX ();
}

void GLLANDMARK::CALC_BOX ()
{
	m_vMax = m_vMin = m_pAffineParts->vTrans;
	
	m_vMax += D3DXVECTOR3 ( 3, 12, 3 );
	m_vMin += D3DXVECTOR3 ( -3, 0, -3 );
}

HRESULT GLLANDMARK::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	if ( !COLLISION::IsCollisionVolume(*pCV,m_vMax,m_vMin) )	return S_FALSE;

	if ( pd3dDevice )
	{
		D3DXVECTOR3 vOldPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER;
		EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S1 );

		D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vOldPos, NULL );
		CDebugSet::ToPos ( vScr0.x, vScr0.y, m_strMARK.c_str() );
	}
	else
	{
		LANDMARK::vecLANDMARK_DISP.push_back ( this );
	}

	return S_OK;
}

BOOL GLLANDMARK::SAVE ( CSerialFile &SFile )
{
	SFile << (DWORD)VERSION;

	SFile << m_strMARK;

	SFile.WriteBuffer ( m_pAffineParts, sizeof(DXAFFINEPARTS) );
	SFile << m_vMax;
	SFile << m_vMin;

	return TRUE;
}

BOOL GLLANDMARK::LOAD ( basestream &SFile )
{
	DWORD dwVER(0);

	SFile >> dwVER;

	SFile >> m_strMARK;

	SFile.ReadBuffer ( m_pAffineParts, sizeof(DXAFFINEPARTS) );
	SFile >> m_vMax;
	SFile >> m_vMin;

	DXAFFINEMATRIX::SetAffineValue ( m_pAffineParts );

	return TRUE;
}
