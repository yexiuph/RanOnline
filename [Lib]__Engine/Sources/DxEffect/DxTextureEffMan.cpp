#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"
#include "./TextureManager.h"
#include "./DxTexEffMan.h"

#include "./DxTextureEffMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Note : DxTextureEffMan
DxTextureEffMan::DxTextureEffMan()
{
}

DxTextureEffMan::~DxTextureEffMan()
{
	// Note : 이거 Texture 때문에 지우면 뻑 ~!!
	//m_mapTextureEff.clear();
}

void DxTextureEffMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxTextureEffMan::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxTextureEffMan::OnDestroyDevice()
{
	m_mapTexList.clear();

	std::for_each( m_mapTexEff.begin(), m_mapTexEff.end(), std_afunc::DeleteMapObject() );
	m_mapTexEff.clear();
}

void DxTextureEffMan::InsertTexList( const char* szName )
{
	m_mapTexList.insert( std::make_pair(szName,0L) );
}

void DxTextureEffMan::InsertTextureEff( MAPTEXEFF& mapData, const char* szName, DxTexEffBase* pBase )
{
	MAPTEXEFF_ITER iter = mapData.find( szName );
	if( iter==mapData.end() )
	{
		// Note : 새로운 것 생성
		mapData.insert( std::make_pair(szName,pBase) );
	}
	else
	{
		// Note : 데이터의 추가
		if( !pBase )
		{
			(*iter).second = pBase;
		}
		else
		{
			pBase->m_pNext = (*iter).second;
			(*iter).second = pBase;
		}
	}
}

DxTexEffBase* DxTextureEffMan::GetEffRoot( const char* szName )
{
	MAPTEXEFF_ITER iter = m_mapTexEff.find( szName );
	if( iter!=m_mapTexEff.end() )	return (*iter).second;
	return NULL;
}

BOOL DxTextureEffMan::IsGetTextureEFFDXT( DWORD i, LPCSTR& szName, DWORD& dwFlag )
{
	if( i >= m_mapTexList.size() )	return FALSE;	// ERROR

	MAPTEXLIST_ITER iter = m_mapTexList.begin();
	for( DWORD loop=0; loop<i; ++loop )	++iter;
	
	szName = iter->first.c_str();
	dwFlag = (DWORD)iter->second;

	return TRUE;
}

void DxTextureEffMan::DeleteTexEff( const char* pName, DxTexEffBase* pBase )
{
	DxTexEffBase* pPrev = NULL;
	DxTexEffBase* pCur;
	MAPTEXEFF_ITER iter = m_mapTexEff.find( pName );
	if( iter!=m_mapTexEff.end() )
	{
		pCur = (*iter).second;
		while( pCur )
		{
			if( pCur==pBase )
			{
				if( !pPrev )	// 처음 걸리면
				{
					(*iter).second = pCur->m_pNext;

					pCur->m_pNext = NULL;
					SAFE_DELETE( pCur );
				}
				else			// 중간에 있을때
				{
					pPrev->m_pNext = pCur->m_pNext;

					pCur->m_pNext = NULL;
					SAFE_DELETE( pCur );
				}

				// Note : 데이터가 없다면 삭제를 해준다.
				if( !(*iter).second )
				{
					m_mapTexEff.erase( iter );
				}

				return;
			}

			pPrev = pCur;
			pCur = pCur->m_pNext;
		}
	}
}

void DxTextureEffMan::CloneMapTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPTEXEFF& mapSrc, MAPTEXEFF& mapDest )
{
	PBYTE			pProp;
	std::string		strName;
	DxTexEffBase*	pCur;
	DxTexEffBase*	pNew;

	MAPTEXEFF_ITER iter = mapSrc.begin();
	for( ; iter!=mapSrc.end(); ++iter )
	{
		strName = (*iter).first.c_str();

		pCur = (*iter).second;
		while( pCur )
		{
			// Note : 만들어야지.
			pCur->GetProperty( pProp );
			pNew = DxTexEffMan::GetInstance().AdaptToDxFrame( pCur->GetTypeID(), pd3dDevice, pProp );

			// Note : TextureEff 삽입
			InsertTextureEff( mapDest, strName.c_str(), pNew );

			pCur = pCur->m_pNext;
		}
	}
}

void DxTextureEffMan::SavePSF( CSerialFile& SFile )
{
	// Note : TexList
	{
		DWORD dwSize = (DWORD)m_mapTexList.size();
		SFile << dwSize;

		MAPTEXLIST_ITER iter = m_mapTexList.begin();
		for( ; iter!=m_mapTexList.end(); ++iter )
		{
			SFile << (*iter).first;
			SFile << (*iter).second;
		}
	}


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

void DxTextureEffMan::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	// Note : TexList
	{
		DWORD dwSize;
		SFile >> dwSize;

		DWORD			dwFlag;
		std::string		strName;
		MAPTEXLIST_ITER iter;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> dwFlag;

			iter = m_mapTexList.find( strName.c_str() );
			if( iter!=m_mapTexList.end() )	(*iter).second = dwFlag;
		}
	}


	// Note : Texture Effect List
	{
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
				InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );
			}
		}
	}
}


