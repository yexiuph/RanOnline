#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"

#include "./DxTexEffMan.h"
#include "./DxFrameMesh.h"
#include "./DxTexEff.h"

#include "./DxPieceTexEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		T	e	x		E	f	f
// -----------------------------------------------------------------------------------------------------------------------------------------
DxPieceTexEff::DxPieceTexEff()
{
}

DxPieceTexEff::~DxPieceTexEff()
{
	CleanUp();
}

void DxPieceTexEff::CleanUp()
{
	std::for_each( m_mapTexEff.begin(), m_mapTexEff.end(), std_afunc::DeleteMapObject() );
	m_mapTexEff.clear();
}

void DxPieceTexEff::FrameMove( const float fElapsedTime )
{
	DxTextureEffMan::MAPTEXEFF_ITER iter = m_mapTexEff.begin();
	for( ; iter!=m_mapTexEff.end(); ++iter )
	{
		DxTexEffBase* pTexEff = (*iter).second;
		while( pTexEff )
		{
			pTexEff->FrameMove( fElapsedTime );	// 예전엔 DEF_TEXEFF_DIFFUSE	< switch 써서 씀 >
			pTexEff = pTexEff->m_pNext;
		}
	}
}

void DxPieceTexEff::Save( CSerialFile& SFile )
{
	// Note : Texture Effect List
	{
		DWORD dwSize = (DWORD)m_mapTexEff.size();
		SFile << dwSize;

		DxTexEffBase* pCur;
		MAPTEXEFF_ITER iter = m_mapTexEff.begin();
		for( ; iter!=m_mapTexEff.end(); ++iter )
		{
			SFile << (*iter).first;

			dwSize = 0;
			pCur = (*iter).second;
			while( pCur )
			{
				++dwSize;
				pCur = pCur->m_pNext;
			}

			SFile << dwSize;

			pCur = (*iter).second;
			while( pCur )
			{
				SFile << pCur->GetTypeID();
				pCur->SavePSF( SFile );

				pCur = pCur->m_pNext;
			}
		}
	}
}

void DxPieceTexEff::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	// Note : Texture Effect List
	{
		DxTextureEffMan pTexEffMan;


		DWORD dwSize, dwEffSize;
		SFile >> dwSize;

		DxTexEffBase*	pTexEffBase;
		DWORD			dwTypeID;
		std::string		strName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;

			SFile >> dwEffSize;
			for( DWORD j=0; j<dwEffSize; ++j )
			{
				SFile >> dwTypeID;

				// Note : 만들어야지.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				pTexEffBase->LoadPSF( pd3dDevice, SFile );

				// Note : TextureEff 삽입
				pTexEffMan.InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );
			}
		}
	}
}

void DxPieceTexEff::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh )
{
	CleanUp();

	if( !pFrameMesh )	return;

	DxTextureEffMan* pTexEffMan = pFrameMesh->GetTextureEffMan();
	pTexEffMan->CloneMapTexEff( pd3dDevice, *pTexEffMan->GetMapTexEff(), m_mapTexEff );
}

void DxPieceTexEff::Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceTexEff* pSrc )
{
	CleanUp();

	if( !pSrc )	return;

	DxTextureEffMan pTexEffMan;
	pTexEffMan.CloneMapTexEff( pd3dDevice, pSrc->m_mapTexEff, m_mapTexEff );
}
