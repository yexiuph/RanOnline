//	[class DxLandMan]
//
//
//
//
#include "pch.h"
#include "./DxLandMan.h"
#include "./DxSetLandMan.h"

#include "./SerialFile.h"
#include "./DxEnvironment.h"
#include "./DxRenderStates.h"
#include "./DxEffectFrame.h"
#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/StaticSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include "./DxFrameMesh.h"
#include "./DxReplaceContainer.h"
#include "../NaviMesh/NavigationMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char DxLandMan::m_szPath[MAX_PATH];

namespace	ExportProgress
{
	int		EndPos;
	int		CurPos;
	BOOL	bValid = FALSE;
	char	szState[MAX_PATH] = "";

	void GetStateString ( char *szDesString )
	{
		if ( bValid )
		{
			size_t nStrLen = strlen(szState)+1;
			StringCchCopy( szDesString, nStrLen, szState );
			bValid = FALSE;
		}
		else
		{
			szDesString[0] = NULL;
		}
	}

	void SetStateString ( char *szString )
	{
		while ( bValid )
		{
			//	Note : ������ ��忡�� while()���� ��������
			//		������ ���ѷ����� ������ ����. ( ������ �� �ڵ忡�� �߻�. )
			Sleep( 1 );
		}

		StringCchCopy( szState, MAX_PATH, szString );
		bValid = TRUE;
	}
};

DxLandMan::DxLandMan () :
	m_pStaticMesh(NULL),
	m_pPieceObjTree(NULL),
	m_pPieceObjHead(NULL),
	m_pAniManHead(NULL),
	m_pAniManTree(NULL),
	m_dwEffectCount(0),
	m_pEffectList(NULL),
	m_pEffectTree(NULL),
	m_dwEffectCount_AFTER(0),
	m_pEffectList_AFTER(NULL),
	m_pEffectTree_AFTER(NULL),
	m_dwEffectCount_AFTER_1(0),
	m_pEffectList_AFTER_1(NULL),
	m_pEffectTree_AFTER_1(NULL),
	m_dwCount_Weather(0),
	m_pList_Weather(NULL),
	m_pTree_Weather(NULL),
	m_pNaviMesh(NULL),
	m_pEffectFrameList(NULL),
    m_bIntegrity(TRUE),
	m_pStaticSoundMan(NULL),
	m_dwNumLandEff(0),
	m_pLandEffList(NULL),
	m_pLandEffTree(NULL),
	m_dwNumLandSkinObj(NULL),
	m_pLandSkinObjList(NULL),
	m_pLandSkinObjTree(NULL)
{	
}

DxLandMan::~DxLandMan()
{
	CleanUp();
}

HRESULT DxLandMan::ActiveMap ()
{
	DxLightMan::SetInstance ( &m_LightMan );
	DxWeatherMan::SetInstance ( &m_WeatherMan );
	
	DxBgmSound::GetInstance().SetFile ( m_BGMDATA.strFileName );
	DxBgmSound::GetInstance().Play ();
	DxEffectMan::GetInstance().SetLandMan ( this );
	DxEffSingleMan::GetInstance().SetLandMan ( this );

	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );

	DxEnvironment::GetInstance().ChangeMapInitData();

	m_WeatherMan.SetMapID( m_MapID );

	return S_OK;
}

void DxLandMan::BuildSingleEffTree ()
{
	COLLISION::MakeAABBTree ( m_pLandEffTree, m_pLandEffList );
}

PLANDEFF DxLandMan::FindLandEff ( char* szName )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxLandMan::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void DxLandMan::DelLandEff ( PLANDEFF pLandEff )
{
	GASSERT(pLandEff);
	PLANDEFF pCur, pBack;

	if ( m_pLandEffList==pLandEff )
	{
		pCur = m_pLandEffList;
		m_pLandEffList = m_pLandEffList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandEff--;
		return;
	}

	pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( pCur==pLandEff )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandEff--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

void DxLandMan::AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	m_dwNumLandSkinObj++;
	pLandSkinObj->m_pNext = m_pLandSkinObjList;
	m_pLandSkinObjList = pLandSkinObj;
}

void DxLandMan::DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	GASSERT(pLandSkinObj);
	PLANDSKINOBJ pCur, pBack;

	if ( m_pLandSkinObjList==pLandSkinObj )
	{
		pCur = m_pLandSkinObjList;
		m_pLandSkinObjList = m_pLandSkinObjList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandSkinObj--;
		return;
	}

	pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( pCur==pLandSkinObj )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandSkinObj--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PLANDSKINOBJ DxLandMan::FindLandSkinObj ( char* szName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

PLANDSKINOBJ DxLandMan::FindLandSkinObjByFileName ( const char* szFileName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szFileName,szFileName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxLandMan::BuildSkinObjTree ()
{
	COLLISION::MakeAABBTree ( m_pLandSkinObjTree, m_pLandSkinObjList );
}

HRESULT	DxLandMan::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->InitDeviceObjects ( pd3dDevice );

		pCur = pCur->m_pNext;
	}


	m_LightMan.InitDeviceObjects ( pd3dDevice );

	return 	m_DxOctree.InitDeviceObjects ( pd3dDevice );
}

HRESULT DxLandMan::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DxLandMan::InvalidateDeviceObjects()
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	m_DxOctree.InvalidateDeviceObjects ();

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects ();

			pCur = pCur->m_pNext;
		}
	}

	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects ();

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT	DxLandMan::DeleteDeviceObjects()
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ������ ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	m_DxOctree.DeleteDeviceObjects ();

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();
	
		pCur = pCur->m_pNext;
	}

	m_LightMan.DeleteDeviceObjects ();
	DxBgmSound::GetInstance().ForceStop ();

	CleanUp();

	return S_OK;
}

void DxLandMan::SetFogProperty ( FOG_PROPERTY& Property )
{
	m_FOG_PROPERTY = Property;
	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
}

void DxLandMan::SetSkyProperty ( SKY_PROPERTY& Property )
{
	m_SKY_PROPERTY = Property;
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );
}

void DxLandMan::AddEffectFrameList ( DxEffectBase* pEffect )
{
	pEffect->pEffectNext = m_pEffectFrameList;
	m_pEffectFrameList = pEffect;
}

HRESULT DxLandMan::EffectAdaptToFrame ( DxOctree &OcNode, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
							PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc )
{
	HRESULT hr S_OK;

	DxFrame	*pDxFrameResult = NULL;

	if(OcNode.IsSubDivision())
	{
		for ( int i=0; i<8; i++ )
		{
			DxOctree *pOctree = OcNode.GetOctreeNode(i);
			if ( pOctree )
			{
				hr = EffectAdaptToFrame ( *pOctree, TypeID, szFrame, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffectSrc );
				if ( FAILED(hr) )	return E_FAIL;
			}
		}
	}
	else
	{	
		DxFrame* pDxFrameHead = OcNode.GetDxFrameHead ();
		if ( !pDxFrameHead )		return S_OK;

		DxFrame *pDxFrame = pDxFrameHead->FindFrame ( szFrame );
		if ( !pDxFrame )			return S_OK;

		DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffect ) return NULL;

		//	Note : Property, Affine  ���� ����.
		//
		if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
		if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

		//	Note : �뷮�� ���۸� �����ϱ� ���� �޼ҵ�.
		//
		if ( pEffectSrc )	pEffect->CloneData ( pEffectSrc, pd3dDevice );

		hr = pEffect->Create ( pd3dDevice );
		if ( FAILED(hr) )
		{
			SAFE_DELETE(pEffect);
			return E_FAIL;
		}

		pEffect->AdaptToDxFrame ( pDxFrame, pd3dDevice );

		AddEffectFrameList ( pEffect );	// DxLandMan���� ����Ʈ ���������� ���.
	}

	return S_OK;
}

HRESULT DxLandMan::EffectAdaptToList ( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
							PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc )
{
	HRESULT hr;

	DxFrame *pDxFrame = pFrameRoot->FindFrame ( szFrame );

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return NULL;

	hr = pEffect->AdaptToEffList ( pDxFrame, pd3dDevice );

	if ( hr == S_FALSE )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "�ܵ� ȿ��(%s)�� Adapt �޼ҵ尡 �������� �ʽ��ϴ�.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	if ( FAILED(hr) )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "�ܵ� ȿ��(%s)�� Adapt ���� ������ �߻��Ͽ����ϴ�.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	//	Note : Property, Affine  ���� ����.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )		pEffect->SetAffineValue ( pAffineParts );

	//	Note : �뷮�� ���۸� �����ϱ� ���� �޼ҵ�.
	//
	if ( pEffectSrc )	pEffect->CloneData ( pEffectSrc, pd3dDevice );

	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

	if( pEffect->GetFlag()&(_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0) )
	{
		m_dwEffectCount_AFTER++;
		pEffect->pEffectNext = m_pEffectList_AFTER;
		m_pEffectList_AFTER = pEffect;
	}
	else if( pEffect->GetFlag()&_EFF_SINGLE_AFTER_1 )
	{
		m_dwEffectCount_AFTER_1++;
		pEffect->pEffectNext = m_pEffectList_AFTER_1;
		m_pEffectList_AFTER_1 = pEffect;
	}
	else if( pEffect->GetFlag()&_EFF_SINGLE_AFTER_2 )
	{
		m_dwCount_Weather++;
		pEffect->pEffectNext = m_pList_Weather;
		m_pList_Weather = pEffect;
	}
	else
	{
		m_dwEffectCount++;
		if( pEffect->GetTypeID() == DEF_EFFECT_SHADOW )
		{
			if ( !m_pEffectList )					// ����ִٸ� �ʱ�ȭ��. .
			{
				m_pEffectList = pEffect;
				return S_OK;
			}

			DxEffectBase* pCur = m_pEffectList;
			while ( pCur )							// ����
			{
				if ( !(pCur->pEffectNext) )
				{
					pCur->pEffectNext = pEffect;	// �׸��ڴ� ������ �������� �д�.
					return S_OK;
				}
				pCur = pCur->pEffectNext;
			}
		}
		else
		{
			pEffect->pEffectNext = m_pEffectList;
			m_pEffectList = pEffect;
		}
	}

	return S_OK;
}

HRESULT	DxLandMan::Import( int MaxTriangles, int MaxSubdivisions, DxSetLandMan *pSetLandMan, LPDIRECT3DDEVICEQ pd3dDevice, char *szDebugFullDir )
{	
	HRESULT hr;

	DxFrameMesh *pFrameMesh = pSetLandMan->GetFrameMesh();
	float fUNITTIME = pFrameMesh->GetUnitTime();
	DxFrame *pDxFrameRoot = pFrameMesh->GetFrameRoot();
	if (!pDxFrameRoot) return E_FAIL;


	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;
	
	//	Note : �� ����Ÿ ��� ����.
	//
    ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "���� ����Ÿ ����..." );
	CleanUp ();

	//	Note : �� ��� ����.
	//
	m_MapID = pSetLandMan->GetMapID();
	memcpy ( m_szMapName, pSetLandMan->GetMapName(), sizeof(char)*MAXLANDNAME );

	////	Note : ���� Obj ��Ʈ�� �������� ���� ����, EndPos�� DxOctree Import �߰��κп��� ����
	////
 //   ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "������ �޽� ����Ÿ ������..." );	
	//
	//m_CollisionMap.SetDxFrameMesh ( pFrameMesh );
	//m_CollisionMap.Import ();	

	////	Note : ���� Obj ��Ʈ�� �������� ���� ����, EndPos�� DxOctree Import �߰��κп��� ����
	////
 //   ExportProgress::CurPos = 0;	
	//ExportProgress::SetStateString ( "���� Obj ��Ʈ�� �������� ������..." );
	//m_DxOctree.SetLoadState ( TRUE );
	//m_DxOctree.Import ( MaxTriangles, MaxSubdivisions, &m_CollisionMap );

	////	Note : AABBƮ���� ����
	////
	//ExportProgress::CurPos = 0;		
	//ExportProgress::SetStateString ( "AABB Ʈ�� ������..." );	
	//m_DxOctree.MakeAABBTree ();

	//	Note : ���Ἲ�� ���� ����� ���� �Լ������� �����Ȳ�� üũ��
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 6;	//	������ ȣ��� �Լ��� ����	
	
	//	Note : ���ϸ��̼� ������ ��������.
	//
	ExportProgress::SetStateString ( "���ϸ��̼� ������ ����������..." );	
	DxAnimationMan::m_dwAniManCount = 0;
	hr = pDxFrameRoot->GetAniFrameMesh ( m_pAniManHead, fUNITTIME, pd3dDevice );
	if ( FAILED(hr) )
	{
		OctreeDebugInfo::EndNodeCount = 0;
		OctreeDebugInfo::AmountDxFrame = 0;

		return E_FAIL;
	}
	ExportProgress::CurPos++;
	

	//	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	//
	ExportProgress::SetStateString ( "���ϸ��̼� �޴���, ���� ������..." );	

	DWORD dwNodeCount = 0;
	POBJAABB pNodeCur = m_pAniManHead;
	while ( pNodeCur )
	{
		dwNodeCount++;
		pNodeCur = pNodeCur->GetNext();
	}

	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = dwNodeCount;

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );
	ExportProgress::CurPos++;

	//	Note : ġȯ Obj ���� ����.
	//
	ExportProgress::SetStateString ( "ġȯ Obj ���� ������..." );
	DXPIECEOBJ* pPieceCur = pFrameMesh->GetPieceList ();
	while ( pPieceCur )
	{
		//	Note : ġȯ ���� ��������.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( pPieceCur->szFileName );
		if ( !pPiece )		return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = pPieceCur->matComb;

		if ( pPiece->bRendAni )
		{
			srand(GetTickCount());
			pNewPieceObj->fCurTime = (rand()%1000) * 0.01f;
		}

		AddPieceObj ( pNewPieceObj );

		pPieceCur = pPieceCur->pNext;
	}
	ExportProgress::CurPos++;


	//	Note : ġȯ Obj AABB-Tree ����.
	//
	ExportProgress::SetStateString ( "ġȯ Obj AABB-Tree ������..." );	
	DxReplaceContainer::MakeAABBTree ( m_pPieceObjTree, m_pPieceObjHead );
	ExportProgress::CurPos++;


	//	Note : Effect ȿ�� �����ϱ�.
	//	
	ExportProgress::SetStateString ( "Effect ȿ�� ������..." );	
	m_dwEffectCount = 0;
	m_dwEffectCount_AFTER = 0;
	DxEffectBase* pEffCur = pFrameMesh->GetEffectList ();
	while ( pEffCur )
	{
		DWORD TypeID = pEffCur->GetTypeID();
		char* szFrameName = pEffCur->GetAdaptFrameName();

		PBYTE pProperty;
		DWORD dwSize;
		DWORD dwVer;
		pEffCur->GetProperty ( pProperty, dwSize, dwVer );
		LPDXAFFINEPARTS pAffineParts = pEffCur->m_pAffineParts;

		if ( pEffCur->GetFlag()&(_EFF_REPLACE|_EFF_SINGLE|_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0|_EFF_SINGLE_AFTER_1|_EFF_SINGLE_AFTER_2) )
		{
			EffectAdaptToList ( pDxFrameRoot, TypeID, szFrameName, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffCur );
		}
		else
		{
			EffectAdaptToFrame ( m_DxOctree, TypeID, szFrameName, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffCur );
		}

		pEffCur = pEffCur->pEffectNext;
	}
	ExportProgress::CurPos++;


	//	Note : Optimum Octree..
	//
	SAFE_DELETE ( m_pStaticMesh );
	m_pStaticMesh = new DxStaticMesh;
	m_pStaticMesh->StartThread( pd3dDevice );
	m_pStaticMesh->MakeAABBOCTree( pd3dDevice, pFrameMesh, TRUE, FALSE );	// Modify

	// Note : ���� ���� �Ŵ���
	m_PieceManager.ClonePSFtoWLD( pd3dDevice, pSetLandMan->GetPieceManager() );

	//	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	//
	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������..." );
	COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER" );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER_1" );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...Weather" );
	COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	ExportProgress::CurPos++;


	//	Note : �׺���̼� �޽� �����ϱ�
	//	
	DxFrame	*pNaviFrame = NULL;
	pSetLandMan->GetNaviFrame ( &pNaviFrame );

	if ( pNaviFrame && pNaviFrame->pmsMeshs )
	{		
		ULONG	NumOfFaces = pNaviFrame->pmsMeshs->m_pSysMemMesh->GetNumFaces();

		ExportProgress::CurPos = 0;
		ExportProgress::EndPos = NumOfFaces;
		ExportProgress::SetStateString ( "�׺���̼� �޽� ������..." );	

		m_pNaviMesh = new NavigationMesh;
	    m_pNaviMesh->Clear();

		hr = m_pNaviMesh->CreateNaviMesh ( pNaviFrame, pd3dDevice );
		if ( FAILED ( hr ) )
		{
			ExportProgress::SetStateString ( "����!! �׺���̼� �޽� ������ �����Ͽ����ϴ�." );
			m_pNaviMesh->Clear();
			SAFE_DELETE ( m_pNaviMesh );
		}
		else
		{
			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = NumOfFaces;
			ExportProgress::SetStateString ( "�׺���̼� �޽� AABB Ʈ�� ������..." );
			m_pNaviMesh->MakeAABBTree ();

			//	<--	NavigationMesh ���� ����			
			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = NumOfFaces;
			ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ��..." );	
			m_pNaviMesh->LinkCells ();
			//	-->	NavigationMesh ���� ����

			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = 1;
			ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ ���Ἲ �˻���..." );
			m_bIntegrity = m_pNaviMesh->CheckIntegrity ( szDebugFullDir );
		}
	}
	else
	{
		ExportProgress::EndPos = 1;
		ExportProgress::SetStateString ( "���!! �׺���̼� �޽��� �������� �ʾҽ��ϴ�." );			
		ExportProgress::CurPos++;
	}

	//	Note : ���� ��ü �����ϱ� ( DxSetLandMan --> DxLandMan )
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "���� ��ü ������..." );

	m_pStaticSoundMan = new CStaticSoundMan;
	CStaticSoundMan* pStaticSoundMan = pSetLandMan->GetStaticSoundMan();
	pStaticSoundMan->Import ( m_pStaticSoundMan );	
	ExportProgress::CurPos++;

	//	Note : Single Effect �����ϱ� ( DxSetLandMan --> DxLandMan )
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = pSetLandMan->GetNumLandEff ();
	ExportProgress::SetStateString ( "Single Effect ȿ�� ������..." );

	PLANDEFF pCurLandEff = pSetLandMan->GetLandEffList ();
	while ( pCurLandEff )
	{
		PLANDEFF pNewLandEff = new DXLANDEFF;
		pNewLandEff->SetEffect( pCurLandEff->m_szName, pCurLandEff->m_matWorld, pCurLandEff->m_szFileName, pd3dDevice );

		ExportProgress::CurPos++;
		AddLandEff ( pNewLandEff );

		pCurLandEff = pCurLandEff->m_pNext;
	}
	
	BuildSingleEffTree ();

	//	Note : Light �Ӽ� ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Light �Ӽ� ����..." );
	DxLightMan* pLightMan = pSetLandMan->GetLightMan ();
	pLightMan->CloneInstance ( &m_LightMan );

	//	Note : pWeatherMan �Ӽ� ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Weather �Ӽ� ����..." );
	DxWeatherMan* pWeatherMan = pSetLandMan->GetWeatherMan ();
	pWeatherMan->CloneInstance ( &m_WeatherMan, pd3dDevice );

	//	Note : Camera Animation ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Camera Animation �Ӽ� ����..." );
	m_CameraAniMan.Import ( pSetLandMan->GetCameraAniMan() );

	//	Note : Land Skin_Object ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Land Skin_Object ����..." );

	PLANDSKINOBJ pCurSkinObj = pSetLandMan->GetLandSkinObjList ();
	while ( pCurSkinObj )
	{
		PLANDSKINOBJ pNewSkinObj = new DXLANDSKINOBJ;
		pNewSkinObj->SetSkinChar ( pCurSkinObj->m_szName, pCurSkinObj->m_matWorld, pCurSkinObj->m_szFileName, pd3dDevice );

		AddLandSkinObj ( pNewSkinObj );

		pCurSkinObj = pCurSkinObj->m_pNext;
	}

	BuildSkinObjTree ();
	
	//	Note : Land Gate.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Land Gate ����..." );

	m_LandGateMan = *pSetLandMan->GetLandGateMan();

	//	Note : fog.
	m_FOG_PROPERTY = pSetLandMan->GetFogProperty();

	//	Note : sky.
	m_SKY_PROPERTY = pSetLandMan->GetSkyProperty();

	m_BGMDATA = *pSetLandMan->GetBGMDATA ();

	ExportProgress::SetStateString ( "----------------------------------------------------" );

	return S_OK;
}

BOOL DxLandMan::AddPieceObj ( DXPIECEOBJ *pNewPieceObj )
{
	//	Note : ����Ʈ�� ���.
	//
	pNewPieceObj->pNext = m_pPieceObjHead;
	m_pPieceObjHead = pNewPieceObj;

	return TRUE;
}

HRESULT DxLandMan::FrameMove ( float fTime, float fElapsedTime )
{
	DxAnimationMan *pAnimManCur = m_pAniManHead;
	while(pAnimManCur)
	{
		pAnimManCur->FrameMove ( fTime, fElapsedTime );
		pAnimManCur = pAnimManCur->m_pNext;
	}

	DxEffectBase *pEffectCur;

	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );

		pEffectCur = pEffectCur->pEffectNext;
	}

	pEffectCur = m_pEffectList;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}

	if ( m_pStaticSoundMan )
	{
		//	Note : ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�
		//		   ���� ���� Ŭ����
		//
		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt();
		D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
		m_pStaticSoundMan->FrameMove ( vLookatPt, fElapsedTime );
	}

	DXLANDEFF::FrameMove ( fTime, fElapsedTime );
	DXLANDSKINOBJ::FrameMove ( fTime, fElapsedTime );

	m_CameraAniMan.FrameMove ( fTime, fElapsedTime );

	// Note : ���� ���� �Ŵ���
	m_PieceManager.FrameMove( fTime, fElapsedTime );


	// Note : m_pStaticMesh
	if ( m_pStaticMesh )	m_pStaticMesh->FrameMove( fElapsedTime );

	return S_OK;
}

HRESULT DxLandMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxLandMan::Render");

	OctreeDebugInfo::TotalNodesDrawn = 0;
	OctreeDebugInfo::FaceViewCount = 0;
	OctreeDebugInfo::VertexViewCount = 0;
	OctreeDebugInfo::TodalDxFrameDrawn = 0;

	//	Note : ���� Ʈ������ �ʱ�ȭ.
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	
	//	Note : �⺻ ���� ������.
	//
	PROFILE_BEGIN("DxOctree::Render");
	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.Render ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList, this );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}
	PROFILE_END("DxOctree::Render");

	//	Note : Optimum Octree
	//
	PROFILE_BEGIN("DxOptimumOctree::Render");
	if ( m_pStaticMesh )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial ( &sMaterial );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		m_pStaticMesh->Render_THREAD_( pd3dDevice, cv );
		m_pStaticMesh->Render_THREAD_Alpha ( pd3dDevice, cv );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	}
	PROFILE_END("DxOptimumOctree::Render");

	//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
	PROFILE_BEGIN("DxLandMan::Render-m_pEffectTree");
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree, FALSE, this );	
	PROFILE_END("DxLandMan::Render-m_pEffectTree");

	//	Note : ġȯ ������Ʈ ������.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjTree, &cv, emReflect, fDistance );

	//	Note : ��Ų ������Ʈ ������.
	//
	PROFILE_BEGIN("DxLandMan::Render-m_pLandSkinObjTree");
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandSkinObjTree, FALSE );
	PROFILE_END("DxLandMan::Render-m_pLandSkinObjTree");

	//CDebugSet::ToView ( 3, "���� Octree %d, DxFrame %d", OctreeDebugInfo::EndNodeCount, OctreeDebugInfo::AmountDxFrame );
	//CDebugSet::ToView ( 4, "������ Octree %d, DxFrame %d", OctreeDebugInfo::TotalNodesDrawn, OctreeDebugInfo::TodalDxFrameDrawn );
	//CDebugSet::ToView ( 5, "������ ���̽� %d, ���ؽ� %d", OctreeDebugInfo::FaceViewCount, OctreeDebugInfo::VertexViewCount );
	//CDebugSet::ToView ( 6, "������ ���� %d", COLLISION::dwRendAABB );
	//CDebugSet::ToView ( 7, "Effect Num %d", m_dwEffectCount );

	// Note : ���� ���� �Ŵ���
	m_PieceManager.Render( pd3dDevice, cv );


	//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
	//
	PROFILE_BEGIN("DxLandMan::Render_EFF-m_pEffectTree_1");
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree_AFTER_1, FALSE, this );	
	PROFILE_END("DxLandMan::Render_EFF-m_pEffectTree_1");

	PROFILE_END("DxLandMan::Render");
	return S_OK;
}

HRESULT DxLandMan::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxLandMan::Render_EFF");

	//	Note : ���ϸ��̼� �ִ� ������Ʈ ������.
	//
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pAniManTree, FALSE );

	//	Note : ���� �� �Ӽ��� Frame ������.
	//
	DrawAlphaMap ( pd3dDevice, &cv );

	//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
	//
	PROFILE_BEGIN("DxLandMan::Render_EFF-m_pEffectTree");
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree_AFTER, FALSE, this );	
	PROFILE_END("DxLandMan::Render_EFF-m_pEffectTree");

	//	Note : ������ ���� ����Ʈ �߿��� ���� ��� �Ѹ��� Render �ϴ� ����Ʈ.
	//
	PROFILE_BEGIN("m_AfterRender.Render");
	m_AfterRender.Render ( pd3dDevice );
	m_AfterRender.Clear ();
	PROFILE_END("m_AfterRender.Render");

	//	Note : ġȯ ������Ʈ ������.
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	DxReplaceContainer::DrawRePlacePiece_ALPHA( pd3dDevice, m_pPieceObjTree, &cv, emReflect );

	// Note : ���� ���� �Ŵ���
	m_PieceManager.RenderAlpha( pd3dDevice, m_pStaticMesh, NULL );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	//
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	//	Note : Weather..
	PROFILE_BEGIN("Weather");
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame( pd3dDevice, &cv, m_pTree_Weather, FALSE, this );	
	PROFILE_END("Weather");

	PROFILE_END("DxLandMan::Render_EFF");

	return S_OK;
}

void DxLandMan::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxReplaceContainer::DrawRePlacePiecePickAlpha( pd3dDevice );		// Note : Prev PieceManager
	m_PieceManager.RenderPickAlpha( pd3dDevice, m_pStaticMesh, NULL );	// Note : Recent PieceManager
}


HRESULT	DxLandMan::Render_ReflectOLD( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	//	Note : �⺻ ���� ������.
	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.Render_NOEFF ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}

	//	Note : Optimum Octree
	//
	if ( m_pStaticMesh )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial ( &sMaterial );

		m_pStaticMesh->Render_THREAD_ ( pd3dDevice, cv );
	}

	//	Note : ���� �� �Ӽ��� Frame ������.
	//
	DrawAlphaMap ( pd3dDevice, &cv );

	//	Note : ġȯ ������Ʈ ������.
	//			�ݻ������� FALSE�� �ذ��� �ݻ� ����� Ʋ���� �׷���.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emRelflect = DxReplaceContainer::EM_REFLECT_OLD;
	DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, &cv, emRelflect, fDistance );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	//
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	return hr;
}

HRESULT	DxLandMan::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	// �� ������������ ���ؼ� ������ ����.
	DWORD	dwCullMode;
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

	D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

	// Think : �⺻ ������ Matrix�� Identity �̰�, �� ���̰� vCenter.y �̰�, 
	//		������ ������ �� ���̺��� -vCenter.y�� �־�����,
	//		������ �Ŀ��� �� ���̺��� +vCenter.y�� �־�� �Ѵ�.
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	matWorld._42 = -(vCenter.y*2.f);	
	D3DXMatrixMultiply_MIRROR( matWorld );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	//	Note : �⺻ ���� ������.
	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.Render_NOEFF ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}

	//	Note : Optimum Octree
	if ( m_pStaticMesh )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial( &sMaterial );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// m_pStaticMesh�� Diffuse Color�� ����ϹǷ� Light�� ����.

		m_pStaticMesh->Render_THREAD_( pd3dDevice, cv );
	}

	//	Note : ���� �� �Ӽ��� Frame ������.
	DrawAlphaMap( pd3dDevice, &cv, TRUE );

	//	Note : ġȯ ������Ʈ ������.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_REFLECT;
	DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, &cv, emReflect, fDistance );

	// Note : ���� ���� �Ŵ���
	m_PieceManager.Render_Reflect( pd3dDevice );

	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	return hr;
}

HRESULT DxLandMan::Render_NOSKIN ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxLandMan::Render");

	OctreeDebugInfo::TotalNodesDrawn = 0;
	OctreeDebugInfo::FaceViewCount = 0;
	OctreeDebugInfo::VertexViewCount = 0;
	OctreeDebugInfo::TodalDxFrameDrawn = 0;

	//	Note : ���� Ʈ������ �ʱ�ȭ.
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	
	//	Note : �⺻ ���� ������.
	//
	PROFILE_BEGIN("DxOctree::Render");
	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.Render ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList, this );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}
	PROFILE_END("DxOctree::Render");

	//	Note : Optimum Octree
	//
	if ( m_pStaticMesh )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial ( &sMaterial );

		m_pStaticMesh->Render_THREAD_ ( pd3dDevice, cv );
	}

	//	Note : ������ ���� ����Ʈ �߿��� ���� ��� �Ѹ��� Render �ϴ� ����Ʈ.
	//
	PROFILE_BEGIN("m_AfterRender.Render");
	m_AfterRender.Render ( pd3dDevice );
	m_AfterRender.Clear ();
	PROFILE_END("m_AfterRender.Render");

	PROFILE_END("DxLandMan::Render");
	return hr;
}

HRESULT DxLandMan::DrawAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bReflect )
{
	PROFILE_BEGIN("DxLandMan::DrawAlphaMap");

	//	Note : Ʈ������ ����.
	//
	if ( !bReflect )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	}

	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.RenderAlpha( *pCV );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}

	//	Note : Optimum Octree
	if ( m_pStaticMesh )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial ( &sMaterial );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// m_pStaticMesh�� Diffuse Color�� ����ϹǷ� Light�� ����.

		m_pStaticMesh->Render_THREAD_SoftAlpha ( pd3dDevice, *pCV );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	}

	PROFILE_END("DxLandMan::DrawAlphaMap");

	return S_OK;
}

//	Note : bFrontColl �� �ǹ̴� TRUE - �ո鸸 Coll �Ѵ�.
//								FALSE - �ո�, �޸� �� �Ѵ�.
//			bBackColl �� �߾���� �ߴ�.. ~!!
//
void DxLandMan::IsCollision ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, const BOOL bFrontColl )
{
	bCollision = FALSE;
	vCollision = vEnd;

	COLLISION::vColTestStart = vStart;
	COLLISION::vColTestEnd = vEnd;

	const char* szName = NULL;
	D3DXVECTOR3 vNewNormal(0.f,1.f,0.f);
	if ( m_pStaticMesh )
	{
		if( m_pStaticMesh->IsCollisionLine( vStart, vEnd, vCollision, vNewNormal, szName, bFrontColl ) )	bCollision = TRUE;
		else																								bCollision = FALSE;
	}
	else
	{
		m_DxOctree.IsCollision( vStart, vEnd, vCollision, vNewNormal, bCollision, pDxFrame, szName, bFrontColl );
	}

	CDebugSet::ToView( 4, "%s", szName );
}

void DxLandMan::IsCollisionEX ( D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal )
{
	bCollision = FALSE;
	vCollision = vEnd;

	COLLISION::vColTestStart = vStart;
	COLLISION::vColTestEnd = vEnd;

	D3DXVECTOR3	vNewEnd		= vEnd;

	D3DXVECTOR3 vNewNormal (0.f, 1.f, 0.f);
	if ( vNormal )	vNewNormal = *vNormal;

	if ( m_pStaticMesh )
	{
		if( m_pStaticMesh->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, TRUE ) )	bCollision = TRUE;
		else																						bCollision = FALSE;
	}
	else
	{
		m_DxOctree.IsCollision( vStart, vNewEnd, vCollision, vNewNormal, bCollision, pDxFrame, szName, TRUE );
	}

	CDebugSet::ToView( 10, "%s", szName );

	if ( vNormal )	*vNormal = vNewNormal;
}

void DxLandMan::IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						const BOOL bBackColl, const BOOL bPiece )
{
	if( !m_pStaticMesh )	return;

	bCollision = FALSE;
	vCollision = vEnd;

	COLLISION::vColTestStart = vStart;
	COLLISION::vColTestEnd = vEnd;

	D3DXVECTOR3	vNewEnd		= vEnd;

	// Note : �Ϲ� �޽� �浹 ó��
	const char* szName = NULL;
	D3DXVECTOR3 vNewNormal(0.f,1.f,0.f);
	if( m_pStaticMesh->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, !bBackColl ) )	bCollision = TRUE;
	else																								bCollision = FALSE;

	// Note : ���� ���� �ʿ����� �浹ó��
	if( m_PieceManager.IsCollisionLine( vStart, vNewEnd, vCollision ) )					bCollision = TRUE;


	CDebugSet::ToView( 4, "%s", szName );
}

void DxLandMan::CleanUp ()
{
	m_WeatherMan.ReSetSound ();

	if( m_pStaticMesh )
	{
		m_pStaticMesh->EndThread();
		SAFE_DELETE( m_pStaticMesh );
	}

	//	Note : �� ����Ÿ ��� ����.
	//
	m_SFileMap.CloseFile ();

	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	m_DxOctree.CleanUp ();
	SAFE_DELETE(m_pAniManHead);
	SAFE_DELETE(m_pAniManTree);
	SAFE_DELETE(m_pPieceObjHead);
	SAFE_DELETE(m_pPieceObjTree);
	
	SAFE_DELETE(m_pEffectFrameList);
	SAFE_DELETE(m_pEffectList);
	SAFE_DELETE(m_pEffectTree);
	SAFE_DELETE(m_pEffectList_AFTER);
	SAFE_DELETE(m_pEffectTree_AFTER);
	SAFE_DELETE(m_pEffectList_AFTER_1);
	SAFE_DELETE(m_pEffectTree_AFTER_1);
	SAFE_DELETE(m_pList_Weather);
	SAFE_DELETE(m_pTree_Weather);
	SAFE_DELETE(m_pNaviMesh);
	m_bIntegrity = TRUE;

	SAFE_DELETE(m_pStaticSoundMan);

	m_dwNumLandEff = 0;
	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(m_pLandEffTree);

	m_dwNumLandSkinObj = 0;
	SAFE_DELETE(m_pLandSkinObjList);
	SAFE_DELETE(m_pLandSkinObjTree);

	m_LightMan.CleanUp ();
	m_CameraAniMan.Cleanup ();
	m_LandGateMan.CleanUp ();

	m_CollisionMap.CleanUp ();

	m_PieceManager.CleanUp();
}
