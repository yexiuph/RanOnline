#include "pch.h"
#include "./DxEffKeepCTColor.h"

#include "./SerialFile.h"

#include "./DxSkinChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffKeepData_CTColor::TYPEID	= EFFKT_CT_COLOR;
DWORD		DxEffKeepData_CTColor::VERSION	= 0x0100;
char		DxEffKeepData_CTColor::NAME[64]	= "Toon Color Change";

EFFKEEP_PROPERTY_CTCOLOR::EFFKEEP_PROPERTY_CTCOLOR() :
	m_fRoopTime(0.5f),
	m_dwFlag(0L)
{
}

DxEffKeepData_CTColor::DxEffKeepData_CTColor()
{
}

DxEffKeepData_CTColor::~DxEffKeepData_CTColor()
{
}

DxEffKeep* DxEffKeepData_CTColor::NEWOBJ()
{
	DxEffKeepCTColor*	pEff = new DxEffKeepCTColor;

	pEff->m_sProp.m_dwFlag = m_sProp.m_dwFlag;
	pEff->m_sProp.m_fRoopTime = m_sProp.m_fRoopTime;

	pEff->m_sProp.m_vecColor.clear();
	std::copy( m_sProp.m_vecColor.begin(), m_sProp.m_vecColor.end(), std::back_inserter(pEff->m_sProp.m_vecColor) );

	pEff->Create ( m_pd3dDevice );

	return	pEff;
}

void DxEffKeepData_CTColor::InsertColor( DWORD dwColor )
{
	KEEP_COLORARGB	sColor;
	sColor.dwColor = dwColor;
	sColor.wA = 255;
	sColor.wR = (WORD)((dwColor&0xff0000)>>16);
	sColor.wG = (WORD)((dwColor&0xff00)>>8);
	sColor.wB = (WORD)(dwColor&0xff);

	m_sProp.m_vecColor.push_back( sColor );
}

void DxEffKeepData_CTColor::DeleteColor( int nIndex )
{
	int nCount(0);
	VEC_COLORARGB_ITER iter = m_sProp.m_vecColor.begin();
	for( ; iter!=m_sProp.m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_sProp.m_vecColor.erase( iter );
		}
	}
}

void DxEffKeepData_CTColor::ChageColor( int nIndex, DWORD dwColor )
{
	int nCount(0);
	VEC_COLORARGB_ITER iter = m_sProp.m_vecColor.begin();
	for( ; iter!=m_sProp.m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			KEEP_COLORARGB	sColor;
			sColor.dwColor = dwColor;
			sColor.wA = 255;
			sColor.wR = (WORD)((dwColor&0xff0000)>>16);
			sColor.wG = (WORD)((dwColor&0xff00)>>8);
			sColor.wB = (WORD)(dwColor&0xff);

			m_sProp.m_vecColor[nIndex] = sColor;
		}
	}
}

HRESULT	DxEffKeepData_CTColor::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		SFile >> m_sProp.m_dwFlag;
		SFile >> m_sProp.m_fRoopTime;

		DWORD dwSize, dwColor;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> dwColor;

			KEEP_COLORARGB	sColor;
			sColor.dwColor = dwColor;
			sColor.wA = 255;
			sColor.wR = (WORD)((dwColor&0xff0000)>>16);
			sColor.wG = (WORD)((dwColor&0xff00)>>8);
			sColor.wB = (WORD)(dwColor&0xff);
			m_sProp.m_vecColor.push_back(sColor);
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffKeepData_CTColor::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		SFile << m_sProp.m_dwFlag;
		SFile << m_sProp.m_fRoopTime;

		SFile << (DWORD)m_sProp.m_vecColor.size();
		for( DWORD i=0; i<m_sProp.m_vecColor.size(); ++i )
		{
			SFile << m_sProp.m_vecColor[i].dwColor;
		}
	}
	SFile.EndBlock( EMBLOCK_00 );

	return S_OK;
}

//	-----------------------------------------------	--------------------------------------------------

DxEffKeepCTColor::DxEffKeepCTColor(void) :
	DxEffKeep(),
	m_fIndexCoolTime(0.f),
	m_dwIndexCUR(0)
{
}

DxEffKeepCTColor::~DxEffKeepCTColor(void)
{
	CleanUp ();
}

HRESULT DxEffKeepCTColor::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTColor::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTColor::FrameMove ( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	m_fIndexCoolTime += fElapsedTime;
	if( m_fIndexCoolTime > m_sProp.m_fRoopTime )
	{
		m_fIndexCoolTime = 0.f;
		++m_dwIndexCUR;
	}

	float fRate(0.f);
	float fInvRate(0.f);
	DWORD dwR(0);
	DWORD dwG(0);
	DWORD dwB(0);
	DWORD dwSize = (DWORD)m_sProp.m_vecColor.size();
	switch( dwSize )
	{
	case 0:
		sSKINEFFDATA.m_dwCTShadeColor = 0x00000000;
		break;

	case 1:
		sSKINEFFDATA.m_dwCTShadeColor = m_sProp.m_vecColor[0].dwColor;
		break;

	default:
		if( m_dwIndexCUR >= dwSize )	m_dwIndexCUR = 0;

		fRate = m_fIndexCoolTime/m_sProp.m_fRoopTime;
		fInvRate = 1.f - fRate;

		if( m_dwIndexCUR == dwSize-1 )
		{
			dwR = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wR * fInvRate) + (m_sProp.m_vecColor[0].wR * fRate));
			dwG = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wG * fInvRate) + (m_sProp.m_vecColor[0].wG * fRate));
			dwB = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wB * fInvRate) + (m_sProp.m_vecColor[0].wB * fRate));
			
			sSKINEFFDATA.m_dwCTShadeColor = 0xff000000 + (dwR<<16) + (dwG<<8) + dwB;
		}
		else
		{
			dwR = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wR * fInvRate) +  (m_sProp.m_vecColor[m_dwIndexCUR+1].wR * fRate));
			dwG = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wG * fInvRate) +  (m_sProp.m_vecColor[m_dwIndexCUR+1].wG * fRate));
			dwB = (DWORD)((m_sProp.m_vecColor[m_dwIndexCUR].wB * fInvRate) +  (m_sProp.m_vecColor[m_dwIndexCUR+1].wB * fRate));
			
			sSKINEFFDATA.m_dwCTShadeColor = 0xff000000 + (dwR<<16) + (dwG<<8) + dwB;
		}
		break;
	}

	return S_OK;
}

HRESULT	DxEffKeepCTColor::Render ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

