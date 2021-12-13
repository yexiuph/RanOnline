// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./StlFunctions.h"

#include "./TextureManager.h"
#include "./SerialFile.h"

#include "./DxEffectMan.h"

#include "./DxEffectTiling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxEffectTiling::SaveBuffer ( CSerialFile &SFile )
{
	char strTexture[255];

	SFile.BeginBlock();
	{
		POINTEX sPointEX;
		DWORD	dwCountEX = 0;
		for ( DWORD i=0; i<m_dwPointEX; ++i )
		{
			if ( m_pPointEX[i].dwMaterial )	++dwCountEX;
		}

		SFile << dwCountEX;													//	SAVE
		for ( DWORD i=0; i<m_dwPointEX; ++i )
		{
			if ( m_pPointEX[i].dwMaterial )
			{
				sPointEX.vPos		= m_pPointEX[i].vPos;
				sPointEX.dwMaterial = m_pPointEX[i].dwMaterial;
				sPointEX.dwColor	= m_pPointEX[i].dwColor;

				SFile.WriteBuffer ( &sPointEX, sizeof(POINTEX) );			//	SAVE
			}
		}
	}
	SFile.EndBlock();

	SFile.BeginBlock();
	{
		DWORD	dwMaterialsNUM;
		dwMaterialsNUM = (DWORD)m_mapMaterials.size();
		SFile << dwMaterialsNUM;											//	SAVE

		DWORD				dwFirst;
		MATERIALMAP_ITER	iter		= m_mapMaterials.begin();
		MATERIALMAP_ITER	iter_end	= m_mapMaterials.end();
		for ( ; iter!=iter_end; ++iter )
		{
			dwFirst		= (*iter).first;
			StringCchCopy( strTexture, 255, (*iter).second->strTexture.c_str() );
			SFile << dwFirst;											//	SAVE
			SFile.WriteBuffer ( strTexture, sizeof(char)*255 );			//	SAVE

			DWORD dwSize = (DWORD)(*iter).second->listFace.size();
			SFile << dwSize;
			FACELIST_ITER _iter = (*iter).second->listFace.begin();
			for ( ; _iter!=(*iter).second->listFace.end(); ++_iter )
			{
				SFile.WriteBuffer ( &(*_iter), sizeof(SEPARATEOBJ) );
			}
		}
	}
	SFile.EndBlock();

	SFile.BeginBlock();
	{
		MATERIALMAP_ITER iter = m_mapMaterials.begin();
		for ( ; iter!=m_mapMaterials.end(); ++iter )
		{
			SFile << (*iter).first;

			BOOL bUse = (*iter).second->m_pMeshList ? TRUE : FALSE;
			SFile << bUse;
			if ( bUse )
			{
				(*iter).second->m_pMeshList->Save( SFile, FALSE );
			}
		}
	}
	SFile.EndBlock();
}

void DxEffectTiling::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 여기서 로드 하는 것을 주의 해야 한다.
	// 왜냐면 X 파일을 변경할 수도 있기 때문에 이곳에는 값을 지정해서는 안된다.
	// < X 파일에 속해있는 효과, X 파일이 변하면 자신도 변해야 하기 때문에 >

	DWORD	dwBuffSize_Base;
	DWORD	dwBuffSize_Blend;
	BOOL	bExt;
	DWORD	dwMaterialsNUM;

	DWORD	dwFirst;
	char	strTexture[255];
	MATERIALMAP_ITER iter;

	std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	if ( (VERSION==dwVer) || (dwVer==0x00000101) )
	{
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX) );
			}
		}

		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			MATERIALEX* pMaterials = new MATERIALEX;

			pMaterials->strTexture = strTexture;
			TextureManager::LoadTexture ( strTexture, pd3dDevice, pMaterials->pTexture, 0, 0 );

			pMaterials->m_bAlpha = (BOOL)TextureManager::GetTexType( strTexture );	// 알파 쓰는지 안쓰는지 체크

			m_mapMaterials.insert ( std::make_pair(dwFirst,pMaterials) );		// 삽입
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
					SFile.ReadBuffer ( &sOBJ, sizeof(SEPARATEOBJ) );
					(*iter).second->listFace.push_back ( sOBJ );
				}
			}

			CreateAABBOctree ( pd3dDevice, pMaterials );
		}

		SFile >> dwBuffSize_Blend;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Blend );

		return;
	}
	else if ( dwVer == 0x00000100 )
	{
		SFile >> dwBuffSize_Base; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX) );
			}
		}

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			MATERIALEX* pMaterials = new MATERIALEX;

			pMaterials->strTexture = strTexture;
			TextureManager::LoadTexture ( strTexture, pd3dDevice, pMaterials->pTexture, 0, 0 );

			pMaterials->m_bAlpha = (BOOL)TextureManager::GetTexType( strTexture );	// 알파 쓰는지 안쓰는지 체크

			m_mapMaterials.insert ( std::make_pair(dwFirst,pMaterials) );		// 삽입
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				struct TEMP
				{
					D3DXVECTOR3			vMax;
					D3DXVECTOR3			vMin;
					VERTEXCOLOR			vVertex[3];
				};
				TEMP sTemp;

				SFile >> bExt;
				while ( bExt )
				{
					SFile >> sTemp.vMax;
					SFile >> sTemp.vMin;

					SFile.ReadBuffer ( &(sTemp.vVertex[0]), sizeof(VERTEXCOLOR) );
					SFile.ReadBuffer ( &(sTemp.vVertex[1]), sizeof(VERTEXCOLOR) );
					SFile.ReadBuffer ( &(sTemp.vVertex[2]), sizeof(VERTEXCOLOR) );
					SFile >> bExt;

					SEPARATEOBJ sOBJ;
					sOBJ.sVertex[0] = sTemp.vVertex[0];
					sOBJ.sVertex[1] = sTemp.vVertex[1];
					sOBJ.sVertex[2] = sTemp.vVertex[2];
					(*iter).second->listFace.push_back ( sOBJ );
				};
			}

			CreateAABBOctree ( pd3dDevice, pMaterials );
		}

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Base );

		return;
	}
	else
	{
		SFile >> dwBuffSize_Base; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Base );
		return;
	}
}

void DxEffectTiling::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD	dwBuffSize_Base;
	DWORD	dwBuffSize_Blend;
	DWORD	dwMaterialsNUM;

	DWORD	dwFirst;
	char	strTexture[255];
	MATERIALMAP_ITER iter;

	std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	if ( VERSION==dwVer )
	{
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX) );
			}
		}

		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			MATERIALEX* pMaterials = new MATERIALEX;

			pMaterials->strTexture = strTexture;
			TextureManager::LoadTexture ( strTexture, pd3dDevice, pMaterials->pTexture, 0, 0 );

			pMaterials->m_bAlpha = (BOOL)TextureManager::GetTexType( strTexture );	// 알파 쓰는지 안쓰는지 체크

			m_mapMaterials.insert ( std::make_pair(dwFirst,pMaterials) );		// 삽입
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
					SFile.ReadBuffer ( &sOBJ, sizeof(SEPARATEOBJ) );
					(*iter).second->listFace.push_back ( sOBJ );
				}
			}

			CreateAABBOctree ( pd3dDevice, pMaterials );
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load( pd3dDevice, SFile, TRUE, FALSE );
				}
			}
		}

		return;
	}
	else if ( dwVer == 0x00000101 )
	{
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX) );
			}
		}

		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			MATERIALEX* pMaterials = new MATERIALEX;

			pMaterials->strTexture = strTexture;
			TextureManager::LoadTexture ( strTexture, pd3dDevice, pMaterials->pTexture, 0, 0 );

			pMaterials->m_bAlpha = (BOOL)TextureManager::GetTexType( strTexture );	// 알파 쓰는지 안쓰는지 체크

			m_mapMaterials.insert ( std::make_pair(dwFirst,pMaterials) );		// 삽입
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
					SFile.ReadBuffer ( &sOBJ, sizeof(SEPARATEOBJ) );
					(*iter).second->listFace.push_back ( sOBJ );
				}
			}

			CreateAABBOctree ( pd3dDevice, pMaterials );
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load_VER100 ( pd3dDevice, SFile );
				}
			}
		}

		return;
	}
	else if ( dwVer == 0x00000100 )
	{
		SFile >> dwBuffSize_Base;
		SFile >> dwBuffSize_Blend;

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Base+dwBuffSize_Blend );

		return;
	}
	else
	{
		SFile >> dwBuffSize_Base;

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Base );
		return;
	}
}