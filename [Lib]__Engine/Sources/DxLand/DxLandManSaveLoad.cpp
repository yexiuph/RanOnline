#include "pch.h"

#include "DxLandMan.h"
#include "./SerialFile.h"
#include "./DxReplaceContainer.h"
#include "./DxEffectFrame.h"
#include "./DxEffectMan.h"
#include "./DxLightMan.h"
#include "../NaviMesh/NavigationMesh.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/StaticSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
//
//	LAND.MAN 파일 버전 변화.
//	ver.113 - Weather를 마지막에 뿌리기 위해 변경
//	ver.112 - DxPieceManager 추가
//	ver.111 - Replace_Single After 추가
//	ver.110 - Replace_Single After 추가
//	ver.109 - Octree 최적화로 데이터 변경.
//	ver.108 - Fog 추가로 변화. Sky 추가로 변화.
//	ver.107 - 날씨 file mark 위치 추가로 인한 변화.
//
//
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD	DxLandMan::VERSION = 0x0114;
const char	DxLandMan::FILEMARK[128] = "LAND.MAN";

BOOL DxLandMan::SaveFile ( const char *szFile )
{
	CSerialFile SFile;
	BOOL	hr = FALSE;

	SFile.SetFileType ( (char*)FILEMARK, DxLandMan::VERSION );

	if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
	{
		//	Note : 맵 ID 저장.
		SFile << m_MapID.dwID;
		SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

		//	Note : 파일중 일부 데이터를 랜덤 접근하기 위한 파일 마크 위치 저장.
		//
		long nFILEMARK = SFile.GetfTell ();
		m_sFILEMARK.SaveSet ( SFile );

		//	Note	:	세이브
		//
		hr = m_DxOctree.SaveFile ( SFile );
		if ( !hr )
		{
			return hr;
		}		

		SFile << BOOL ( m_pAniManHead!=NULL );
		if ( m_pAniManHead )
		{
			m_pAniManHead->SaveFile ( SFile );
		}
        
		SFile << BOOL ( m_pPieceObjHead!=NULL );
        if ( m_pPieceObjHead )
		{
			m_pPieceObjHead->SaveFile ( SFile );
		}

		// Replace
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Replace - AFTER
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList_AFTER;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList_AFTER;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Replace - AFTER_1
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList_AFTER_1;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList_AFTER_1;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Replace - Weather
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pList_Weather;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pList_Weather;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		m_LightMan.Save ( SFile );


		m_sFILEMARK.dwNAVI_MARK = SFile.GetfTell ();
		SFile << BOOL ( m_pNaviMesh!=NULL );
		if ( m_pNaviMesh )
		{
			m_pNaviMesh->SaveFile ( SFile );
		}

		DxSoundMan::GetInstance().SaveSet ( SFile );

		SFile << BOOL ( m_pStaticSoundMan!=NULL );
		if ( m_pStaticSoundMan )
		{
			m_pStaticSoundMan->SaveSet ( SFile );
		}

		SFile << BOOL ( m_pLandEffList!=NULL );
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->Save ( SFile );

			pCur = pCur->m_pNext;
			SFile << BOOL ( pCur!=NULL );
		}

		SFile << BOOL ( TRUE );
		m_CameraAniMan.Save ( SFile );


		//	Note : Land 에 삽입된 SkinObject
		//
		SFile << BOOL ( m_pLandSkinObjList!=NULL );

		PLANDSKINOBJ pLandSkinObj = m_pLandSkinObjList;
		while ( pLandSkinObj )
		{
			pLandSkinObj->Save ( SFile );
				
			pLandSkinObj = pLandSkinObj->m_pNext;
			SFile << BOOL ( pLandSkinObj!=NULL );
		}

		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwGATE_MARK = SFile.GetfTell ();
		m_LandGateMan.Save ( SFile );

		//	Note : Weather
		//
		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwWEATHER_MARK = SFile.GetfTell ();
		m_WeatherMan.SaveSet ( SFile );

		//	Note : Bgm Sound
		//
		SFile << BOOL ( TRUE );
		m_BGMDATA.SaveSet ( SFile );

		//	Note : ColiisonMap
		//
		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwCOLL_MARK = SFile.GetfTell ();
		SFile.BeginBlock();
			m_CollisionMap.SaveFile ( SFile );		
		SFile.EndBlock();

		//	Note : Fog
		SFile << BOOL ( TRUE );
		m_FOG_PROPERTY.SaveSet ( SFile );

		//	Note : Sky
		SFile << BOOL ( TRUE );
		m_SKY_PROPERTY.SaveSet ( SFile );

		//	Note : StaticMesh
		SFile << BOOL ( TRUE );
		if ( m_pStaticMesh )	m_pStaticMesh->Save( szFile );		// 자체 저장

		// Note : DxPieceManager
		SFile << BOOL ( TRUE );
		m_PieceManager.SaveWLD( SFile );

		//	Note : 다음에 추가될 데이터를 위해서 마킹.
		//
		SFile << BOOL ( FALSE );

		//	Note : 각종 Data의 파일저장 위치를 파일 헤더 부분에 저장.
		//		( 주의 ) 모든 파일의 정보를 저장후 가장 마지막에 행한다.
		//
		SFile.SetOffSet ( nFILEMARK );
		m_sFILEMARK.SaveSet ( SFile );
	}

	return hr;
}

//	Note : 치환 조각으로 저장하기 위해서 사용하는 Save 함수.
//		치환 조각은 형상 + 종속효과 + 에니메이션 으로 이루어진다.
//
BOOL DxLandMan::SavePieceFile ( const char *szFile )
{
	CSerialFile SFile;
	BOOL	hr = FALSE;

	if ( !m_pStaticMesh && !m_pAniManHead )
	{
		MessageBox ( NULL, "형상이 존재 하지 않습니다.\nExport 후에 저장하세요.", "치환조각 저장 - 오류", MB_OK );
		return FALSE;
	}

	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);

	D3DXVECTOR3 vMaxCur, vMinCur;

	if ( m_pStaticMesh )
	{
		vMax = m_pStaticMesh->GetAABBMax();
		vMin = m_pStaticMesh->GetAABBMin();
	}

	DxAnimationMan *pAinManCur = m_pAniManHead;
	while(pAinManCur)
	{
		DxFrame *pFrame = pAinManCur->GetDxFrameRoot ();
		if ( pFrame )
		{
			vMaxCur = pFrame->vTreeMax;
			vMinCur = pFrame->vTreeMin;

			if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
			if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;

			if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
			if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;

			if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
			if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
		}

		pAinManCur = pAinManCur->m_pNext;
	}

	DxEffectBase* pEffCur = m_pEffectList;
	while ( pEffCur )
	{
		pEffCur->GetAABBSize ( vMaxCur, vMinCur );

		if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
		if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;

		if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
		if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;

		if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
		if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;

		pEffCur = pEffCur->pEffectNext;
	}

	if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
	{
		SFile << vMax;
		SFile << vMin;


		// 원래는 옥트리 형상이었는데 Version Up 으로 안 쓰인다.
		SFile << FALSE;

		if ( m_pAniManHead )
		{
			SFile << BOOL ( TRUE );
			m_pAniManHead->SaveFile ( SFile );
		}
		else
		{
			SFile << BOOL ( FALSE );
		}

		SFile << TRUE; // bRendAni;

		DxEffectBase* pEffCur = NULL;
		DWORD dwEffCount = 0;

		pEffCur = m_pEffectList;
		while ( pEffCur )
		{
			dwEffCount++;
			pEffCur = pEffCur->pEffectNext;
		}

		SFile << dwEffCount;

		pEffCur = m_pEffectList;
		while ( pEffCur )
		{
			pEffCur->SaveFile ( SFile );
			pEffCur = pEffCur->pEffectNext;
		}

		if ( m_pLandEffList )	SFile << BOOL ( TRUE );
		else					SFile << BOOL ( FALSE );

		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->Save ( SFile );

			pCur = pCur->m_pNext;
			if ( pCur )		SFile << BOOL ( TRUE );
			else			SFile << BOOL ( FALSE );
		}

		if ( m_pStaticMesh )
		{
			SFile << TRUE;
			m_pStaticMesh->Save( SFile, TRUE );
		}
		else
		{
			SFile << FALSE;
		}

		//	Note : 다음 새이브 될 
		//
		SFile << BOOL ( FALSE );
	}
	//CSerialFile SFile;
	//BOOL	hr = FALSE;

	//if ( m_DxOctree.IsSubDivision() )
	//{
	//	MessageBox ( NULL, "Export 시에 페이스 분할 개수가 과소하게 정해졌습니다.\n"
	//		"분할 개수를 더 크게 설정하여 하나로 분할되게 하십시요.", "치환조각 저장 - 오류", MB_OK );
	//	return FALSE;
	//}
	//
	//if ( !m_DxOctree.GetDxFrameHead() && !m_pAniManHead )
	//{
	//	MessageBox ( NULL, "형상이 존재 하지 않습니다.\nExport 후에 저장하세요.", "치환조각 저장 - 오류", MB_OK );
	//	return FALSE;
	//}

	//D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	//D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);

	//D3DXVECTOR3 vMaxCur, vMinCur;

	//if ( m_DxOctree.GetDxFrameHead() )
	//{
	//	vMax = m_DxOctree.GetMax();
	//	vMin = m_DxOctree.GetMin();
	//}

	//DxAnimationMan *pAinManCur = m_pAniManHead;
	//while(pAinManCur)
	//{
	//	DxFrame *pFrame = pAinManCur->GetDxFrameRoot ();
	//	if ( pFrame )
	//	{
	//		vMaxCur = pFrame->vTreeMax;
	//		vMinCur = pFrame->vTreeMin;

	//		if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
	//		if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;

	//		if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
	//		if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;

	//		if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
	//		if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
	//	}

	//	pAinManCur = pAinManCur->m_pNext;
	//}

	//DxEffectBase* pEffCur = m_pEffectList;
	//while ( pEffCur )
	//{
	//	pEffCur->GetAABBSize ( vMaxCur, vMinCur );

	//	if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
	//	if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;

	//	if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
	//	if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;

	//	if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
	//	if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;

	//	pEffCur = pEffCur->pEffectNext;
	//}

	//if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
	//{
	//	SFile << vMax;
	//	SFile << vMin;

	//	DxFrame* pDxFrameHead = m_DxOctree.GetDxFrameHead();

	//	if ( pDxFrameHead )
	//	{
	//		SFile << TRUE;
	//		pDxFrameHead->SaveFile(SFile);
	//	}
	//	else
	//	{
	//		SFile << FALSE;
	//	}

	//	if ( m_pAniManHead )
	//	{
	//		SFile << BOOL ( TRUE );
	//		m_pAniManHead->SaveFile ( SFile );
	//	}
	//	else
	//	{
	//		SFile << BOOL ( FALSE );
	//	}

	//	SFile << TRUE; // bRendAni;

	//	DxEffectBase* pEffCur = NULL;
	//	DWORD dwEffCount = 0;

	//	pEffCur = m_pEffectList;
	//	while ( pEffCur )
	//	{
	//		dwEffCount++;
	//		pEffCur = pEffCur->pEffectNext;
	//	}

	//	SFile << dwEffCount;

	//	pEffCur = m_pEffectList;
	//	while ( pEffCur )
	//	{
	//		pEffCur->SaveFile ( SFile );
	//		pEffCur = pEffCur->pEffectNext;
	//	}

	//	if ( m_pLandEffList )	SFile << BOOL ( TRUE );
	//	else					SFile << BOOL ( FALSE );

	//	PLANDEFF pCur = m_pLandEffList;
	//	while ( pCur )
	//	{
	//		pCur->Save ( SFile );

	//		pCur = pCur->m_pNext;
	//		if ( pCur )		SFile << BOOL ( TRUE );
	//		else			SFile << BOOL ( FALSE );
	//	}

	//	//	Note : 다음 새이브 될 
	//	//
	//	SFile << BOOL ( FALSE );
	//}

	return TRUE;
}

HRESULT DxLandMan::EffectLoadToList ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return E_FAIL;

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	//	Note : Insert To List.
	//
	pEffect->pEffectNext = pEffList;
	pEffList = pEffect;

	// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
	DxEffectBase* pTemp = NULL;
	DxEffectBase* pCur	= pEffList;
	DxEffectBase* pThis = pEffList;
	while ( pCur )
	{
		pThis = pCur;
		pCur = pCur->pEffectNext;

		pThis->pEffectNext = pTemp;
		pTemp = pThis;
	}
	pEffList = pTemp;

	return S_OK;
}

HRESULT DxLandMan::EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList, PDXEFFECTBASE &pEffList_AFTER,
									 DWORD& dwCount, DWORD& dwCount_AFTER )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return E_FAIL;

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	if( pEffect->GetFlag()&(_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0|_EFF_SINGLE_AFTER_1|_EFF_SINGLE_AFTER_2) )
	{
		pEffect->pEffectNext = pEffList_AFTER;
		pEffList_AFTER = pEffect;

		++dwCount;
	}
	else
	{
		//	Note : Insert To List.
		//
		pEffect->pEffectNext = pEffList;
		pEffList = pEffect;

		// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
		DxEffectBase* pTemp = NULL;
		DxEffectBase* pCur	= pEffList;
		DxEffectBase* pThis = pEffList;
		while ( pCur )
		{
			pThis = pCur;
			pCur = pCur->pEffectNext;

			pThis->pEffectNext = pTemp;
			pTemp = pThis;
		}
		pEffList = pTemp;

		++dwCount_AFTER;
	}

	return S_OK;
}

BOOL DxLandMan::LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad )
{
	//	Note : 앳 데이타 모두 제거.
	CleanUp();

	// Note : 현재 맵의 설정으로 바꿈
	ActiveMap();

	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, m_szPath );
	StringCchCat( szPathName, MAX_PATH, szFile );
	
	char szFileType[FILETYPESIZE] = "";
	DWORD dwVersion;

	if ( hr = m_SFileMap.OpenFile ( FOT_READ, szPathName ) )
	{
		//	Note : 파일 형식 버전확인.
		//
		m_SFileMap.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  맵 파일 형식이 아닙니다.", szFile );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion>=0x0108 && dwVersion<=DxLandMan::VERSION )	{}
		else
		{
			CString Str;
			Str.Format ( "[%s]  Error Load Map File!!!", szFile );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion == 0x0108 )
		{
			return LoadFile_VER108 ( szFile, pd3dDevice, bUseDynamicLoad );
		}
		if ( dwVersion == 0x0109 )
		{
			return LoadFile_VER109 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );
		}
		if ( dwVersion == 0x0110 )
		{
			return LoadFile_VER110 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );
		}
		if( (dwVersion==0x0112) || (dwVersion==0x0111) )
		{
			return LoadFile_VER112( szFile, pd3dDevice, bUseDynamicLoad, szPathName );
		}

		m_SFileMap >> m_MapID.dwID;
		m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

		m_sFILEMARK.LoadSet ( m_SFileMap );

		//	Note	:	로드
		//
		hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
		if ( !hr )	return hr;

		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_pAniManHead = new DxAnimationMan;
			m_pAniManHead->LoadFile ( m_SFileMap, pd3dDevice );
		}

		COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

		m_SFileMap >> bExist;
		while ( bExist )
		{
			float	fCurTime;
			m_SFileMap >> fCurTime;		
			
			char*	szFileName = NULL;
			m_SFileMap >> bExist;
			if ( bExist )
			{
                int	StrLength = 0;
				m_SFileMap >> StrLength;
				szFileName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
			}

			char*	szFrameName = NULL;
            m_SFileMap >> bExist;
			if ( bExist )
			{
				m_SFileMap >> bExist;
				if ( bExist )
				{
					int StrLength = 0;
					m_SFileMap >> StrLength;
					szFrameName = new char [ StrLength ];
					m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
				}
			}

			D3DXMATRIX		matComb;
			m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

			//	Note : 치환 조각 가져오기.
			//
			DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
			if ( !pPiece )		return FALSE;

			DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
			pNewPieceObj->SetPiece ( pPiece );
			pNewPieceObj->matComb = matComb;

			if ( pPiece->bRendAni )
			{
				srand(GetTickCount());
				pNewPieceObj->fCurTime = fCurTime;
			}

			AddPieceObj ( pNewPieceObj );

			SAFE_DELETE_ARRAY ( szFileName );
			SAFE_DELETE_ARRAY ( szFrameName );

			m_SFileMap >> bExist;
		}

		// Replace
		{
			DWORD dwMaxCount = 0;
			m_SFileMap >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwEffectCount++;
			}
		
			//	Note : AABB Tree make
			//
			COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
		}

		// Replace - AFTER
		{
			DWORD dwMaxCount = 0;
			m_SFileMap >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwEffectCount_AFTER++;
			}
		
			//	Note : AABB Tree make
			COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
		}

		// Replace - AFTER_1
		{
			DWORD dwMaxCount = 0;
			m_SFileMap >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwEffectCount_AFTER_1++;
			}
		
			//	Note : AABB Tree make
			//
			COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
		}

		// Replace - Weather
		{
			DWORD dwMaxCount = 0;
			m_SFileMap >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwCount_Weather++;
			}
		
			//	Note : AABB Tree make
			COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
		}

		//	Note : Light 설정.
		m_LightMan.Load( m_SFileMap, VERSION, FALSE );

		// Note : 조각파일의 Tree를 Light Load 후 한다.
		DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_pNaviMesh = new NavigationMesh;
			m_pNaviMesh->LoadFile ( m_SFileMap, pd3dDevice );
		}

		DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_pStaticSoundMan = new CStaticSoundMan;
			m_pStaticSoundMan->LoadSet ( m_SFileMap );
		}	

		m_SFileMap >> bExist;
		if ( bExist )
		{
			bExist = TRUE;
			while ( bExist )
			{
				PLANDEFF pLandEff = new DXLANDEFF;
				pLandEff->Load ( m_SFileMap, pd3dDevice );
				AddLandEff ( pLandEff );
				
				m_SFileMap >> bExist;
			}
		
			BuildSingleEffTree ();
		}

		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_CameraAniMan.Load ( m_SFileMap );
		}

		//	Note : Land 에 삽입된 SkinObject
		//
		m_SFileMap >> bExist;
		if ( bExist )
		{
			bExist = TRUE;
			while ( bExist )
			{
				PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
				pLandSkinObj->Load ( m_SFileMap, pd3dDevice );
				AddLandSkinObj ( pLandSkinObj );
				
				m_SFileMap >> bExist;
			}
		
			BuildSkinObjTree ();
		}

		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_LandGateMan.Load ( m_SFileMap );
		}

		//	Note : m_WeatherMan
		//
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
		}

		//	Note : Bgm Sound
		//
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_BGMDATA.LoadSet ( m_SFileMap );
		}

		//	Note : Collision Map Skip ( By-Pass )
		//
		m_SFileMap >> bExist;
		DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
		//m_CollisionMap.LoadFile ( m_SFileMap );
		m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );
	
		//	DxFogMan
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_FOG_PROPERTY.LoadSet ( m_SFileMap );
		}
		else return TRUE;

		//	DxSkyMan
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SKY_PROPERTY.LoadSet ( m_SFileMap );
		}
		else return TRUE;

		//	StaticMesh
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_pStaticMesh = new DxStaticMesh;
			m_pStaticMesh->Load( pd3dDevice, szPathName );	// 자체 파일 로드
			m_pStaticMesh->StartThread( pd3dDevice );
		}
		else return TRUE;

		//	DxPieceManager	ver.112 에서 추가 됨
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_PieceManager.LoadWLD( pd3dDevice, m_SFileMap );
		}
		else return TRUE;
	}

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER112( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
		if ( !pPiece )		return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	}

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//	Note : Light 설정.
	m_LightMan.Load( m_SFileMap, VERSION, FALSE );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pNaviMesh = new NavigationMesh;
		m_pNaviMesh->LoadFile ( m_SFileMap, pd3dDevice );
	}

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap, pd3dDevice );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	//	Note : Land 에 삽입된 SkinObject
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
			pLandSkinObj->Load ( m_SFileMap, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			m_SFileMap >> bExist;
		}
	
		BuildSkinObjTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticMesh = new DxStaticMesh;
		m_pStaticMesh->Load( pd3dDevice, szPathName );	// 자체 파일 로드
		m_pStaticMesh->StartThread( pd3dDevice );
	}
	else return TRUE;

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_PieceManager.LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER110( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
		if ( !pPiece )		return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	}

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//	Note : Light 설정.
	m_LightMan.Load( m_SFileMap, VERSION, FALSE );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pNaviMesh = new NavigationMesh;
		m_pNaviMesh->LoadFile ( m_SFileMap, pd3dDevice );
	}

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap, pd3dDevice );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	//	Note : Land 에 삽입된 SkinObject
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
			pLandSkinObj->Load ( m_SFileMap, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			m_SFileMap >> bExist;
		}
	
		BuildSkinObjTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticMesh = new DxStaticMesh;
		m_pStaticMesh->Load( pd3dDevice, szPathName );	// 자체 파일 로드
		m_pStaticMesh->StartThread( pd3dDevice );
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER109 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
		if ( !pPiece )		return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER );
		if ( FAILED(hr) )
			MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
	}

	// Note : Weather 효과가 있는 것을 빼온다.
	SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
	COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

	//	Note : AABB Tree make
	COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );


	//	Note : Light 설정.
	m_LightMan.Load( m_SFileMap, VERSION, FALSE );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pNaviMesh = new NavigationMesh;
		m_pNaviMesh->LoadFile ( m_SFileMap, pd3dDevice );
	}

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap, pd3dDevice );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	//	Note : Land 에 삽입된 SkinObject
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
			pLandSkinObj->Load ( m_SFileMap, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			m_SFileMap >> bExist;
		}
	
		BuildSkinObjTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticMesh = new DxStaticMesh;
		m_pStaticMesh->Load( pd3dDevice, szPathName );	// 자체 파일 로드
		m_pStaticMesh->StartThread( pd3dDevice );
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER108 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
		if ( !pPiece )		return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER );
		if ( FAILED(hr) )
			MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
	}

	// Note : Weather 효과가 있는 것을 빼온다.
	SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
	COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

	//	Note : AABB Tree make
	COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );

	//	Note : Light 설정.
	m_LightMan.Load( m_SFileMap, VERSION, FALSE );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pNaviMesh = new NavigationMesh;
		m_pNaviMesh->LoadFile ( m_SFileMap, pd3dDevice );
	}

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap, pd3dDevice );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	//	Note : Land 에 삽입된 SkinObject
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
			pLandSkinObj->Load ( m_SFileMap, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			m_SFileMap >> bExist;
		}
	
		BuildSkinObjTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vPos )
{
	if( m_pStaticMesh )
	{
		m_pStaticMesh->BaseLoad( pd3dDevice, vPos );
		return TRUE;
	}

	float fDis = DxFogMan::GetInstance().GetFogRangeFar();
	D3DXVECTOR3 vMax = vPos + D3DXVECTOR3( fDis, fDis, fDis );
	D3DXVECTOR3 vMin = vPos - D3DXVECTOR3( fDis, fDis, fDis );
	return m_DxOctree.LoadBasicPos ( m_SFileMap, pd3dDevice, m_pEffectFrameList, vMax, vMin );
}

BOOL DxLandMan::LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	if( m_pStaticMesh )
	{
		m_pStaticMesh->BaseLoad( pd3dDevice, vMax, vMin );
		return TRUE;
	}
	return m_DxOctree.LoadBasicPos ( m_SFileMap, pd3dDevice, m_pEffectFrameList, vMax, vMin );
}

void DxLandMan::SwitchWeatherEffect( PDXEFFECTBASE& pSrc, DWORD& dwSrc, PDXEFFECTBASE& pDest, DWORD& dwDest )
{
	dwDest = 0;
	SAFE_DELETE( pDest );

	DxEffectBase*	pPrev = NULL;
	DxEffectBase*	pChange = NULL;
	DxEffectBase*	pCurSRC = pSrc;
	while( pCurSRC )
	{
		if( pCurSRC->GetTypeID()==DEF_EFFECT_RAINPOINT )
		{
			pChange = pCurSRC;				// 움직이기 위한 포인트 셋팅.
			pCurSRC = pCurSRC->pEffectNext;	// 다음것을 돌리기 위한 셋팅.
			--dwSrc;						// 소스에 것은 하나 빠진다.

			pChange->pEffectNext = pDest;	// pDest에 값을 삽입.
			pDest = pChange;				// pDest에 값을 삽입.
			++dwDest;						// 목적에 있는 것은 하나 더한다.

			if( pPrev )	pPrev->pEffectNext = pCurSRC;	// 빠진 것을 이어주기 위한 셋팅.
			else		pSrc = pCurSRC;					// Src 가 가르키는 값이 변하였다.

			// 뛰어 넘는다.
			continue;
		}

		pPrev = pCurSRC;
		pCurSRC = pCurSRC->pEffectNext;
	}
}
