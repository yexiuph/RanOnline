#include "pch.h"
#include "./GLLandManClient.h"
#include "./GLItemMan.h"
#include "./GLGaeaClient.h"
#include "./GLLandMark.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/NameDisplayMan.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/ItemShopIconMan.h"

#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSimpleMeshMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLLandManClient::GLLandManClient(void) 
	: m_fAge(0.0f)
	, m_pd3dDevice(NULL)
	
	, m_bPeaceZone(FALSE)
	
	, m_bClubBattle(false)
	, m_bClubBattleHall(false)
	, m_fCommission(0.0f)
	, m_bClubDeathMatch(false)
	, m_bClubDeathMatchHall(false)
	
	, m_dwGuidClub(0)
	, m_dwGuidClubMarkVer(0)
{
	SecureZeroMemory( m_ItemArray, sizeof(PITEMDROP)*MAXITEM );
	SecureZeroMemory( m_MoneyArray, sizeof(PMONEYDROP)*MAXMONEY );
	SecureZeroMemory( m_CROWArray, sizeof(PGLCROWCLIENT)*MAXCROW );
	SecureZeroMemory( m_MaterialArray, sizeof(PGLMATERIALCLIENT)*MAXCROW );

	memset( m_szGuidClubName, 0, sizeof(char) * (CHAR_SZNAME) );
	m_dwCOUNT = 0;
}

GLLandManClient::~GLLandManClient(void)
{
	CleanUp ();
}

GLLandManClient& GLLandManClient::GetInstance()
{
	static GLLandManClient Instance;
	return Instance;
}

BOOL GLLandManClient::LoadFile ( const char *szFile )
{
	GASSERT(szFile);
	GASSERT(m_pd3dDevice);
	HRESULT	hr = S_OK;

	//	Note : Map 데이터 리셋.
	CleanUp ();

	BOOL bOk(FALSE);

	bOk = GLLevelFile::LoadFile ( szFile, FALSE, m_pd3dDevice );
	if ( !bOk )			return FALSE;

	bOk = m_LandMan.LoadFile ( m_sLevelHead.m_strWldFile.c_str(), m_pd3dDevice, TRUE );
	if ( !bOk )			return FALSE;

	if( m_LandMan.GetMapID().dwID == 0 )
	{
		m_LandMan.SetMapID( m_sMapID );
	}

	//	Note : Map 대이터 초기화.
	m_vMax = D3DXVECTOR3(+10000,0,+10000);
	m_vMin = D3DXVECTOR3(-10000,0,-10000);

	NavigationMesh* pNaviMesh = m_LandMan.GetNaviMesh ();
	if ( pNaviMesh )	pNaviMesh->GetAABB ( m_vMax, m_vMin );

	hr = m_LandTree.SetState ( m_vMax.x, m_vMax.z, m_vMin.x, m_vMin.z, m_sLevelHead.m_eDivision );
	if ( FAILED(hr) )	return hr;

	hr = m_LandTree.MakeTree ();
	if ( FAILED(hr) )	return hr;

	return TRUE;
}

HRESULT GLLandManClient::Create ( const char* szSetFileName, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vBasicPos, SNATIVEID* pMapID, BOOL bPeaceZone )
{
	GASSERT(szSetFileName);
	GASSERT(pd3dDevice);
	HRESULT	hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	//	이전 대이터 삭제.
	CleanUp ();

	m_strFileName = szSetFileName;

	//	Note : 파일에서 읽은 MAP ID 가 아니라 인수로 넘어온 ID를 사용.
	//
	if ( pMapID )	SetMapID ( *pMapID, bPeaceZone!=FALSE );

	//	Note : 멥을 읽어옴.
	//
	hr = LoadFile ( szSetFileName );
	if ( FAILED(hr) )	return hr;

	SMAPNODE* pMapNode = GLGaeaClient::GetInstance().FindMapNode ( m_sMapID );
	if ( pMapNode )
	{
		m_LandMan.SetBgmFile ( pMapNode->strBGM.c_str() );
		SetClubBattleZone( pMapNode->bClubBattleZone );
	}

	//	Note : 멥의 초기 위치 주변을 미리 읽어둔다.
	////
	//D3DXVECTOR3 vMax = vBasicPos + D3DXVECTOR3(1000,60,1000);
	//D3DXVECTOR3 vMin = vBasicPos + D3DXVECTOR3(-1000,-60,-1000);
	m_LandMan.LoadBasicPos ( m_pd3dDevice, vBasicPos );

	m_bPeaceZone = bPeaceZone;

	//	Note : Mob/Npc 의 형상 파일들을 미리 읽어 둡니다.
	//
	GLMobScheduleMan::MOBDESC& setMobDesc = m_MobSchMan.GetMobDesc();
	GLMobScheduleMan::MOBDESC_ITER iter = setMobDesc.begin();
	GLMobScheduleMan::MOBDESC_ITER iter_end = setMobDesc.end();
	SNATIVEID sMobID;
	for ( ; iter!=iter_end; ++iter )
	{
		sMobID.dwID = (*iter);

		//	Note : 몹의 설정 정보를 가져옴.
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
		if( !pCrowData )
		{
			CDebugSet::ToLogFile ( "GLLandManClient::Create()-GLCrowDataMan::GetCrowData() [%d/%d] 가져오기 실패.", sMobID.wMainID, sMobID.wSubID );
		}

		if ( pCrowData )
		{
			DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData ( pCrowData->GetSkinObjFile(), m_pd3dDevice, FALSE );
			if( !pCharData )
			{
				if( strlen( pCrowData->GetSkinObjFile() ) )
				{
					CDebugSet::ToLogFile ( "GLLandManClient::Create()-DxSkinCharDataContainer::LoadData() [%s] 가져오기 실패.", pCrowData->GetSkinObjFile() );
				}
			}
		}
	}

	hr = InitDeviceObjects ( m_pd3dDevice );
	if ( FAILED(hr) )	return hr;

	hr = RestoreDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

void GLLandManClient::CleanUp ()
{
	DWORD i=0;

	for ( i=0; i<MAXITEM; i++ )		DropOutItem (i);
	for ( i=0; i<MAXMONEY; i++ )	DropOutMoney (i);
	for ( i=0; i<MAXCROW; i++ )		DropOutCrow (i);
	for ( i=0; i<MAXCROW; ++i )		DropOutMaterial(i);

	
	if ( !m_PCArray.empty() )
	{
		CHARMAPID_ITER iter_end = (--m_PCArray.end());
		DWORD dwEndID = (*iter_end).first;
		for ( i=0; i<dwEndID+1; ++i )			DropOutChar (i);
	};

	if ( !m_PETArray.empty() )
	{
		PETMAPID_ITER iter_end = (--m_PETArray.end());
		DWORD dwEndID = (*iter_end).first;
		for ( i=0; i<dwEndID+1; ++i )			DropOutPet (i);
	};

	if ( !m_SummonArray.empty() )
	{
		SUMMONMAPID_ITER iter_end = (--m_SummonArray.end());
		DWORD dwEndID = (*iter_end).first;
		for ( i=0; i<dwEndID+1; ++i )			DropOutSummon (i);
	};

	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();

	m_LandMan.CleanUp ();
}

HRESULT GLLandManClient::ActiveMap ()
{
	return m_LandMan.ActiveMap();
}

void GLLandManClient::SETITEM_PICKDELAY ( DWORD dwGlobID )
{
	PITEMCLIENTDROP pItem = GetItem ( dwGlobID );
	if ( pItem )	pItem->fNextMsgDelay = GLCONST_CHAR::fFIELD_NEXTPICK;
}

void GLLandManClient::SETMONEY_PICKDELAY ( DWORD dwGlobID )
{
	PMONEYCLIENTDROP pMoney = GetMoney ( dwGlobID );
	if ( pMoney )	pMoney->fNextMsgDelay = GLCONST_CHAR::fFIELD_NEXTPICK;
}

BOOL GLLandManClient::ISITEM_PICKDELAY ( DWORD dwGlobID )
{
	//PITEMCLIENTDROP pItem = GetItem ( dwGlobID );
	//if ( !pItem || pItem->fNextMsgDelay > 0.0f )	return TRUE;
	return FALSE;
}

BOOL GLLandManClient::ISMONEY_PICKDELAY ( DWORD dwGlobID )
{
	//PMONEYCLIENTDROP pMoney = GetMoney ( dwGlobID );
	//if ( !pMoney || pMoney->fNextMsgDelay > 0.0f )	return TRUE;
	return FALSE;
}

HRESULT GLLandManClient::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	m_LandMan.InitDeviceObjects ( m_pd3dDevice );

	GLLevelFile::InitDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT GLLandManClient::RestoreDeviceObjects()
{
	m_LandMan.RestoreDeviceObjects ( m_pd3dDevice );
	GLLevelFile::RestoreDeviceObjects ( m_pd3dDevice );

	GLCHARCLIENTNODE *pCharCur = m_GlobPCList.m_pHead;
	for ( ; pCharCur; pCharCur = pCharCur->pNext )
	{
		PGLCHARCLIENT pChar = pCharCur->Data;
		if( !pChar ) continue;

		pChar->RestoreDeviceObjects ( m_pd3dDevice );
	}

	GLCROWCLIENTNODE *pCrowCur = m_GlobCROWList.m_pHead;
	for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
	{
		PGLCROWCLIENT pCrow = pCrowCur->Data;
		if( !pCrow ) continue;

		pCrow->RestoreDeviceObjects ( m_pd3dDevice );
	}

	GLMATERIALCLIENTNODE* pMaterialCur = m_GlobMaterialList.m_pHead;
	for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
	{
		PGLMATERIALCLIENT pMaterial = pMaterialCur->Data;
		if( !pMaterial ) continue;

		pMaterial->RestoreDeviceObjects ( m_pd3dDevice );
	}

	GLANYPETNODE *pPetCur = m_GlobAnyPetList.m_pHead;
	for ( ; pPetCur; pPetCur = pPetCur->pNext )
	{
		PGLANYPET pAnypet = pPetCur->Data;
		if( !pAnypet ) continue;

		pAnypet->RestoreDeviceObjects ( m_pd3dDevice );
	}

	GLANYSUMMONNODE *pSummonCur = m_GlobAnySummonList.m_pHead;
	for ( ; pSummonCur; pSummonCur = pSummonCur->pNext )
	{
		PGLANYSUMMON pAnySummon = pSummonCur->Data;
		if( !pAnySummon ) continue;

		pAnySummon->RestoreDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT GLLandManClient::InvalidateDeviceObjects()
{
	m_LandMan.InvalidateDeviceObjects ();
	GLLevelFile::InvalidateDeviceObjects ();

	GLCHARCLIENTNODE *pCharCur = m_GlobPCList.m_pHead;
	for ( ; pCharCur; pCharCur = pCharCur->pNext )
	{
		PGLCHARCLIENT pChar = pCharCur->Data;
		if( !pChar ) continue;

		pChar->InvalidateDeviceObjects ();
	}

	GLCROWCLIENTNODE *pCrowCur = m_GlobCROWList.m_pHead;
	for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
	{
		PGLCROWCLIENT pCrow = pCrowCur->Data;
		if( !pCrow ) continue;

		pCrow->InvalidateDeviceObjects ();
	}

	GLMATERIALCLIENTNODE *pMaterialCur = m_GlobMaterialList.m_pHead;
	for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
	{
		PGLMATERIALCLIENT pMaterial = pMaterialCur->Data;
		if( !pMaterial ) continue;

		pMaterial->InvalidateDeviceObjects ();
	}

	GLANYPETNODE *pPetCur = m_GlobAnyPetList.m_pHead;
	for ( ; pPetCur; pPetCur = pPetCur->pNext )
	{
		PGLANYPET pAnypet = pPetCur->Data;
		if( !pAnypet ) continue;

		pAnypet->InvalidateDeviceObjects ();
	}

	GLANYSUMMONNODE *pSummonCur = m_GlobAnySummonList.m_pHead;
	for ( ; pSummonCur; pSummonCur = pSummonCur->pNext )
	{
		PGLANYSUMMON pAnySummon = pSummonCur->Data;
		if( !pAnySummon ) continue;

		pAnySummon->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLLandManClient::DeleteDeviceObjects()
{
	LANDMARK::vecLANDMARK_DISP.clear();
	ClearDropObj ();
	m_LandMan.DeleteDeviceObjects ();
	GLLevelFile::DeleteDeviceObjects ();

	return S_OK;
}


HRESULT GLLandManClient::FrameMove ( float fTime, float fElapsedTime )
{
	m_fAge += fElapsedTime;

	PROFILE_BEGIN("DxLandMan::FrameMove");
	m_LandMan.FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxLandMan::FrameMove");

	GLLevelFile::FrameMove ( fTime, fElapsedTime );

	PROFILE_BEGIN("m_GlobPCList::FrameMove");
	GLCHARCLIENTNODE *pCharCur = m_GlobPCList.m_pHead;
	for ( ; pCharCur; pCharCur = pCharCur->pNext )
	{
		PGLCHARCLIENT pChar = pCharCur->Data;
		if( !pChar ) continue;

		pChar->FrameMove ( fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobPCList::FrameMove");

	// PET
	// 여기서 다른 팻들의 FrameMove 처리
	PROFILE_BEGIN("m_GlobAnyPetList::FrameMove");
	GLANYPETNODE *pPetCur = m_GlobAnyPetList.m_pHead;
	for ( ; pPetCur; pPetCur = pPetCur->pNext )
	{
		PGLANYPET pAnypet = pPetCur->Data;
		if( !pAnypet ) continue;

		pAnypet->FrameMove ( fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobAnyPetList::FrameMove");

	// SUMMON
	// 여기서 다른 팻들의 FrameMove 처리
	PROFILE_BEGIN("m_GlobAnySummonList::FrameMove");
	GLANYSUMMONNODE *pSummonCur = m_GlobAnySummonList.m_pHead;
	for ( ; pSummonCur; pSummonCur = pSummonCur->pNext )
	{
		PGLANYSUMMON pAnySummon = pSummonCur->Data;
		if( !pAnySummon ) continue;

		pAnySummon->FrameMove ( fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobAnySummonList::FrameMove");

	PROFILE_BEGIN("m_GlobCROWList::FrameMove");
	GLCROWCLIENTNODE *pCrowCur = m_GlobCROWList.m_pHead;
	for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
	{
		PGLCROWCLIENT pCrow = pCrowCur->Data;
		if( !pCrow ) continue;

		pCrow->FrameMove ( fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobCROWList::FrameMove");

	PROFILE_BEGIN("m_GlobMaterialList::FrameMove");
	GLMATERIALCLIENTNODE *pMaterialCur = m_GlobMaterialList.m_pHead;
	for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
	{
		PGLMATERIALCLIENT pMaterial = pMaterialCur->Data;
		if( !pMaterial ) continue;

		pMaterial->FrameMove ( fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobMaterialList::FrameMove");

	ITEMCLIENTDROPNODE* pCurItem = m_GlobItemList.m_pHead;
	for ( ; pCurItem; pCurItem = pCurItem->pNext )
	{
		PITEMCLIENTDROP pItemDrop = pCurItem->Data;
		if( !pItemDrop ) continue;

		CItemClientDrop sItemDrop = *pItemDrop;
		if ( sItemDrop.fNextMsgDelay > 0.0f )		sItemDrop.fNextMsgDelay -= fElapsedTime;
	}

	MONEYCLIENTDROPNODE* pCurMoney = m_GlobMoneyList.m_pHead;
	for ( ; pCurMoney; pCurMoney = pCurMoney->pNext )
	{
		PMONEYCLIENTDROP pMoneyDrop = pCurMoney->Data;
		if( !pMoneyDrop ) continue;

		CMoneyClientDrop &sMoneyDrop = *pMoneyDrop;
		if ( sMoneyDrop.fNextMsgDelay > 0.0f )		sMoneyDrop.fNextMsgDelay -= fElapsedTime;
	}

	return S_OK;
}

HRESULT GLLandManClient::Render ( CLIPVOLUME &CV )
{
	m_LandMan.Render ( m_pd3dDevice, CV );

	LANDMARK::vecLANDMARK_DISP.clear();
	GLLevelFile::Render ( NULL, CV );

	return S_OK;
}

HRESULT GLLandManClient::Render_MobItem ( CLIPVOLUME &CV )
{
	++m_dwCOUNT;

	BOOL bCOLL(FALSE);
	LPDXFRAME pDxFrame;
	D3DXVECTOR3 vCOLL, vPOS(0.f,0.f,0.f);

	GLCharacter &cMYCHAR = *GLGaeaClient::GetInstance().GetCharacter();
	D3DXVECTOR3 vCAMERPOS = DxViewPort::GetInstance().GetFromPt();
	CNameDisplayMan *pDISP_NAME_MAN = CInnerInterface::GetInstance().GetDispName();
	if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->SET_COUNT ( m_dwCOUNT );

	//	Note : 이름 목록에 자기 자신 추가.
	{
		CROWREN sDISP_NAME;
		sDISP_NAME.INIT ( &cMYCHAR );
		sDISP_NAME.SETTYPEFLAG ( &cMYCHAR );
		if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );

		if ( cMYCHAR.m_bItemShopOpen )
		{
			CItemShopIconMan* pShopIconMan = CInnerInterface::GetInstance().GetItemShopIconMan();
			if ( pShopIconMan ) pShopIconMan->SetRender( cMYCHAR.m_dwGaeaID );
		}
	}

	// 이름목록에 자신의 팻이름 추가
	GLPetClient* pMYPET = GLGaeaClient::GetInstance().GetPetClient ();
	if ( pMYPET->IsVALID () )
	{
		if ( COLLISION::IsCollisionVolume( CV,pMYPET->m_vMax,pMYPET->m_vMin ) )
		{
			vPOS = pMYPET->GetPosition();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_MYPETNAME;
				sDISP_MYPETNAME.INIT ( pMYPET );
				sDISP_MYPETNAME.SETTYPEFLAG ( &cMYCHAR, pMYPET );
				if ( pDISP_NAME_MAN ) pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_MYPETNAME, cMYCHAR.GetName() );
			}
		}
	}

	// 이름목록에 자신의 소환수 이름 추가
	GLSummonClient* pMYSummon = GLGaeaClient::GetInstance().GetSummonClient ();
	if ( pMYSummon->IsVALID () )
	{
		if ( COLLISION::IsCollisionVolume( CV,pMYSummon->m_vMax,pMYSummon->m_vMin ) )
		{
			vPOS = pMYSummon->GetPosition();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_MYSummonNAME;
				sDISP_MYSummonNAME.INIT ( pMYSummon );
				sDISP_MYSummonNAME.SETTYPEFLAG ( &cMYCHAR, pMYSummon );
				if ( pDISP_NAME_MAN ) pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_MYSummonNAME, cMYCHAR.GetName() );
			}
		}
	}

	GLCHARCLIENTNODE *pCharCur = m_GlobPCList.m_pHead;
	for ( ; pCharCur; pCharCur = pCharCur->pNext )
	{
		PGLCHARCLIENT pChar = pCharCur->Data;
		if( !pChar ) continue;

		if( !GLCONST_CHAR::bBATTLEROYAL || !pChar->IsDie() )
		{
			if ( pChar->IsVisibleDetect() && COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) )
			{
				vPOS = pChar->GetPosition();
				vPOS.y += 8.0f;

				m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
				if ( !bCOLL )
				{
					CROWREN sDISP_NAME;
					sDISP_NAME.INIT ( pChar );
					sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pChar );
					if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );

					if ( pChar->m_bItemShopOpen )
					{
						CItemShopIconMan* pShopIconMan = CInnerInterface::GetInstance().GetItemShopIconMan();
						if ( pShopIconMan ) pShopIconMan->SetRender( pChar->m_dwGaeaID );
					}
				}
			}
		}

		pChar->Render ( m_pd3dDevice, CV, FALSE );
		pChar->RenderShadow( m_pd3dDevice, CV );
	}

	// PET
	// 여기서 다른 팻들의 Render 처리
	GLANYPETNODE *pPetCur = m_GlobAnyPetList.m_pHead;
	for ( ; pPetCur; pPetCur = pPetCur->pNext )
	{
		PGLANYPET pPet = pPetCur->Data;

		// 팻이름 표시
		if ( COLLISION::IsCollisionVolume( CV,pPet->m_vMax,pPet->m_vMin ) )
		{
			vPOS = pPet->GetPosition ();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pPet );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pPet );
				if ( pDISP_NAME_MAN )
				{
					PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar( pPet->m_dwOwner );
					if( pCHAR )
					{
						pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME, pCHAR->GetName() );
					}
				}
			}
		}

		pPet->Render ( m_pd3dDevice );
		pPet->RenderShadow ( m_pd3dDevice, CV );
	}

	// SUMMON
	// 여기서 다른 팻들의 Render 처리
	GLANYSUMMONNODE *pSummonCur = m_GlobAnySummonList.m_pHead;
	for ( ; pSummonCur; pSummonCur = pSummonCur->pNext )
	{
		PGLANYSUMMON pSummon = pSummonCur->Data;

		// 소환수이름 표시
		if ( COLLISION::IsCollisionVolume( CV,pSummon->m_vMax,pSummon->m_vMin ) )
		{
			vPOS = pSummon->GetPosition ();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pSummon );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pSummon );
				if ( pDISP_NAME_MAN )
				{
					PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar( pSummon->m_dwOwner );
					if( pCHAR )
					{
						pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME, pCHAR->GetName() );
					}
				}
			}
		}

		pSummon->Render ( m_pd3dDevice );
		pSummon->RenderShadow ( m_pd3dDevice, CV );
	}

	GLCROWCLIENTNODE *pCrowCur = m_GlobCROWList.m_pHead;
	for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
	{
		PGLCROWCLIENT pCrow = pCrowCur->Data;
		if( !pCrow ) continue;

		if ( !pCrow->IsDie() && pCrow->IsVisibleDetect() && COLLISION::IsCollisionVolume(CV,pCrow->m_vMax,pCrow->m_vMin) )
		{
			vPOS = pCrow->GetPosition();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pCrow );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pCrow );
				if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );
			}
		}

		pCrow->Render ( m_pd3dDevice, CV, FALSE );
		pCrow->RenderShadow( m_pd3dDevice, CV );
	}

	GLMATERIALCLIENTNODE *pMaterialCur = m_GlobMaterialList.m_pHead;
	for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
	{
		PGLMATERIALCLIENT pMaterial = pMaterialCur->Data;
		if( !pMaterial ) continue;

		if ( !pMaterial->IsDie() && COLLISION::IsCollisionVolume(CV,pMaterial->m_vMax,pMaterial->m_vMin) )
		{
			vPOS = pMaterial->GetPosition();
			vPOS.y += 8.0f;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pMaterial );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pMaterial );
				if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );
			}
		}

		pMaterial->Render ( m_pd3dDevice, CV, FALSE );
		pMaterial->RenderShadow( m_pd3dDevice, CV );
	}

	DWORD dwOldLighting;
	m_pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &dwOldLighting );
	m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	DWORD dwOldColorOp;
	m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &dwOldColorOp );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	ITEMCLIENTDROPNODE* pCurItem = m_GlobItemList.m_pHead;
	for ( ; pCurItem; pCurItem = pCurItem->pNext )
	{
		PITEMCLIENTDROP pItemDrop = pCurItem->Data;
		if( !pItemDrop ) continue;

		//CItemClientDrop &sItemDrop = *pCurItem->Data;

		bool bCV = pItemDrop->IsCollision ( CV );
		if ( bCV )
		{
			vPOS = pItemDrop->vPos;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pItemDrop );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pItemDrop );
				if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );
			}

			pItemDrop->RenderItem ( m_pd3dDevice, pItemDrop->matWld, NULL );
		}
	}

	MONEYCLIENTDROPNODE* pCurMoney = m_GlobMoneyList.m_pHead;
	for ( ; pCurMoney; pCurMoney = pCurMoney->pNext )
	{
		PMONEYCLIENTDROP pMoneyDrop = pCurMoney->Data;
		if( !pMoneyDrop ) continue;

		//CMoneyClientDrop &sMoneyDrop = *pMoneyDrop;

		bool bCV = pMoneyDrop->IsCollision ( CV );
		if ( bCV )
		{
			vPOS = pMoneyDrop->vPos;
			m_LandMan.IsCollision ( vCAMERPOS, vPOS, vCOLL, bCOLL, pDxFrame, FALSE );
			if ( !bCOLL )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pMoneyDrop );
				sDISP_NAME.SETTYPEFLAG ( &cMYCHAR, pMoneyDrop );
				if ( pDISP_NAME_MAN )	pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );
			}

			pMoneyDrop->RenderItem ( m_pd3dDevice, pMoneyDrop->matWld, NULL );
		}
	}

	m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, dwOldLighting );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );

	//	Note : 타겟은 채력 표시용으로 Display 되므로 이름 출력 리스트에서 삭제.
	//	
	//SACTIONTAR& sACTAR = GLGaeaClient::GetInstance().GetCharacter()->GetActTarget();
	//EMCROW emCROW = sACTAR.sTARID.emCrow;
	//if ( (emCROW==CROW_PC&&sACTAR.emACTAR==EMACTAR_PC_PVP) || sACTAR.emACTAR==EMACTAR_MOB )
	//{
	//	
	//}

	return S_OK;
}

HRESULT GLLandManClient::Render_EFF ( CLIPVOLUME &CV )
{
	m_LandMan.Render_EFF ( m_pd3dDevice, CV );

	GLLevelFile::Render_EFF ( m_pd3dDevice, CV );

	return S_OK;
}

void GLLandManClient::RenderPickAlpha()
{
	m_LandMan.RenderPickAlpha( m_pd3dDevice );
}


BOOL GLLandManClient::DropItem ( SDROP_CLIENT_ITEM *pItemDrop )
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.FindNode ( (int)pItemDrop->vPos.x, (int)pItemDrop->vPos.z );
	if ( !pLandNode )	return	FALSE;

	SITEM* pITEM = GLItemMan::GetInstance().GetItem( pItemDrop->sItemClient.sNativeID );
	if ( !pITEM )		return FALSE;

	//	Note : 메모리를 할당.
	//
	CItemClientDrop *pNewItem = new CItemClientDrop;


	//	Note : 지형의 높이랑 맞추어서 떨어지는 지점을 선택한다.
	//		( 내비게이션이랑 오차가 약간 있는걸 감안. )
	D3DXVECTOR3 vPoint1 = pItemDrop->vPos + D3DXVECTOR3(0,8,0);
	D3DXVECTOR3 vPoint2 = pItemDrop->vPos + D3DXVECTOR3(0,-5,0);
	
	D3DXVECTOR3 vCollision(FLT_MAX,FLT_MAX,FLT_MAX);
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame = NULL;
	m_LandMan.IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, FALSE );

	D3DXVECTOR3 vDropPos = pItemDrop->vPos;
	if ( bCollision )	vDropPos = vCollision;
	vDropPos.y += 1.0f;

	//	Note : 아이템 정보 ( 종류, 위치 ).
	//
	pNewItem->sItemClient = pItemDrop->sItemClient;
	pNewItem->vPos = vDropPos;
	D3DXMatrixTranslation ( &pNewItem->matWld, pNewItem->vPos.x, pNewItem->vPos.y, pNewItem->vPos.z );

	//	Note : 관리 ID 부여.
	//
	pNewItem->sMapID = m_sMapID;
	pNewItem->dwGlobID = pItemDrop->dwGlobID;
	pNewItem->dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_ItemArray[pNewItem->dwGlobID] = pNewItem;										// - 전역.
	pNewItem->pGlobList = m_GlobItemList.ADDHEAD ( pNewItem );						// - 전역.

	pNewItem->pQuadNode = pLandNode;												// - 트리 노드.
	pNewItem->pCellList = pLandNode->pData->m_ItemList.ADDHEAD ( pNewItem );		// - 트리 노드.

	std::string strFieldFile = pITEM->sBasicOp.strFieldFile;
	std::transform ( strFieldFile.begin(), strFieldFile.end(), strFieldFile.begin(), tolower );

	bool bSIMMESH = false;
	bool bEFFECT = false;
	std::string::size_type idx;
	idx = strFieldFile.find ( ".x" );
	if ( idx!=string::npos )	bSIMMESH = true;

	idx = strFieldFile.find ( ".egp" );
	if ( idx!=string::npos )	bEFFECT = true;

	pNewItem->vMax = D3DXVECTOR3(10,10,10);
	pNewItem->vMin = D3DXVECTOR3(-10,-10,-10);

	if ( bSIMMESH )
	{
		DxSimMesh* pSimMesh = GLItemMan::GetInstance().GetFieldMesh ( pNewItem->sItemClient.sNativeID );
		if ( !pSimMesh )
		{
			CDebugSet::ToLogFile ( "ERROR : item field mesh null %s", pITEM->GetName() );
		}
		else
		{
			pSimMesh->GetBoundBox ( pNewItem->vMax, pNewItem->vMin );
		}
		
		pNewItem->pSimMesh = pSimMesh;
	}
	else if ( bEFFECT )
	{
		EFF_PROPGROUP* pPropGroup = DxEffSinglePropGMan::GetInstance().FindEffGProp ( strFieldFile.c_str() );
		if ( pPropGroup )
		{
			pNewItem->vMax = pPropGroup->m_vMax;
			pNewItem->vMin = pPropGroup->m_vMin;
		}

		STARGETID sTARID ( CROW_ITEM, pNewItem->dwGlobID, pNewItem->vPos );
		DxEffGroupPlayer::GetInstance().PassiveEffect ( strFieldFile.c_str(), pNewItem->matWld, sTARID );
	}

	COLLISION::MINDETECTAABB ( pNewItem->vMax, pNewItem->vMin, 1.1f, 4.0f );
	COLLISION::TransformAABB ( pNewItem->vMax, pNewItem->vMin, pNewItem->matWld );


	if ( pItemDrop->fAge < 0.01f )
	{
		std::string strSOUND = "ITEMDROP_COIN";
		switch ( pITEM->sBasicOp.emItemType )
		{
		case ITEM_ANTI_DISAPPEAR:
		case ITEM_REVIVE:
		case ITEM_SUIT:
		case ITEM_VEHICLE:
			{
				switch ( pITEM->sSuitOp.emSuit )
				{
				case SUIT_HEADGEAR:
				case SUIT_UPPER:
				case SUIT_LOWER:
				case SUIT_HAND:
				case SUIT_VEHICLE:
					strSOUND = "ITEMDROP_SUIT";
					break;

				case SUIT_FOOT:
					strSOUND = "ITEMDROP_SHOES";
					break;

				case SUIT_HANDHELD:
					strSOUND = "ITEMDROP_WAPON";
					break;

				case SUIT_NECK:
				case SUIT_WRIST:
				case SUIT_FINGER:
					strSOUND = "ITEMDROP_RING";
					break;
				};
			}
			break;

		case ITEM_SKILL:
			strSOUND = "ITEMDROP_SCROLL";
			break;

		case ITEM_CURE:
			strSOUND = "ITEMDROP_DRUGS";
			break;

		case ITEM_ARROW:
		case ITEM_RECALL:
		case ITEM_KEY:
		case ITEM_GRINDING:
		case ITEM_CHARM:
		case ITEM_TICKET:
		case ITEM_SKP_RESET:
		case ITEM_STAT_RESET:
		case ITEM_SKP_STAT_RESET:
		case ITEM_BOX:
		case ITEM_CLEANSER:
		case ITEM_LOUDSPEAKER:
		case ITEM_FIRECRACKER:
		case ITEM_CHARACTER_CARD:
		case ITEM_INVEN_CARD:
		case ITEM_STORAGE_CARD:
		case ITEM_STORAGE_CONNECT:
		case ITEM_PREMIUMSET:
		case ITEM_PRIVATEMARKET:
		case ITEM_RANDOMITEM:
		case ITEM_DISJUNCTION:
		case ITEM_REMODEL:
		case ITEM_GARBAGE_CARD:
		case ITEM_TELEPORT_CARD:
			strSOUND = "ITEMDROP_SUIT";
			break;

		case ITEM_QITEM:
			strSOUND = "ITEMDROP_?BOX";
			break;
		};

		DxSoundLib::GetInstance()->PlaySound ( strSOUND );
	}

	return TRUE;
}

BOOL GLLandManClient::DropMoney ( LONGLONG lnAmount, D3DXVECTOR3 vPos, DWORD dwGlobID, float fAge )
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	FALSE;

	//	Note : 메모리를 할당.
	//
	CMoneyClientDrop *pNewMoney = new CMoneyClientDrop;

	//	Note : 지형의 높이랑 맞추어서 떨어지는 지점을 선택한다.
	//		( 내비게이션이랑 오차가 약간 있는걸 감안. )
	D3DXVECTOR3 vPoint1 = vPos + D3DXVECTOR3(0,8,0);
	D3DXVECTOR3 vPoint2 = vPos + D3DXVECTOR3(0,-5,0);
	
	D3DXVECTOR3 vCollision(FLT_MAX,FLT_MAX,FLT_MAX);
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame = NULL;
	m_LandMan.IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, FALSE );

	D3DXVECTOR3 vDropPos = vPos;
	if ( bCollision )	vDropPos = vCollision;
	vDropPos.y += 1.0f;

	//	Note : 아이템 정보 ( 종류, 위치 ).
	//
	pNewMoney->fAge = fAge;
	pNewMoney->lnAmount = lnAmount;
	pNewMoney->vPos = vDropPos;
	D3DXMatrixTranslation ( &pNewMoney->matWld, pNewMoney->vPos.x, pNewMoney->vPos.y, pNewMoney->vPos.z );


	//	Note : 관리 ID 부여.
	//
	pNewMoney->sMapID = m_sMapID;
	pNewMoney->dwGlobID = dwGlobID;
	pNewMoney->dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_MoneyArray[dwGlobID] = pNewMoney;												// - 전역.
	pNewMoney->pGlobList = m_GlobMoneyList.ADDHEAD ( pNewMoney );					// - 전역.

	pNewMoney->pQuadNode = pLandNode;												// - 트리 노드.
	pNewMoney->pCellList = pLandNode->pData->m_MoneyList.ADDHEAD ( pNewMoney );		// - 트리 노드.

	DxSimMesh* pMoneyMesh = NULL;
	if ( lnAmount>200 )			pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if ( lnAmount>60 )		pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else						pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	
	if ( !pMoneyMesh )
	{
		CDebugSet::ToLogFile ( "ERROR : money field mesh null ( amount %I64d )", lnAmount );
	}

	pNewMoney->pMoneyMesh = pMoneyMesh;

	if ( fAge < 0.01f )
	{
		DxSoundLib::GetInstance()->PlaySound ( "ITEMDROP_COIN" );
	}

	return TRUE;
}

BOOL GLLandManClient::CreateAnyPet ( PSDROPPET pDropPet )
{
	// 중복되면 이전것을 지우다.
	if  ( GetPet ( pDropPet->m_dwGUID ) ) DropOutPet ( pDropPet->m_dwGUID );

	if ( pDropPet->m_sMapID != m_sMapID )	return FALSE;
	
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropPet->m_dwCellID );
	if ( !pLandNode )					return FALSE;

	PGLANYPET pNewPet = new GLAnyPet;
	HRESULT hr = pNewPet->Create ( pDropPet, m_LandMan.GetNaviMesh(), m_pd3dDevice );
	if ( FAILED ( hr ) ) return FALSE;
	
	m_PETArray.insert ( std::make_pair(pDropPet->m_dwGUID,pNewPet) );
	pNewPet->m_pGlobNode = m_GlobAnyPetList.ADDHEAD ( pNewPet );

	pNewPet->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewPet->m_pCellNode = pLandNode->pData->m_PETList.ADDHEAD ( pNewPet );		// - 트리 노드.

	// 생성 효과
	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, pDropPet->m_vPos.x, pDropPet->m_vPos.y, pDropPet->m_vPos.z );

	std::string strGEN_EFFECT = GLCONST_CHAR::strPET_GEN_EFFECT.c_str();
	STARGETID sTargetID(CROW_PET,pNewPet->m_dwGUID,pNewPet->GetPosition());
	DxEffGroupPlayer::GetInstance().NewEffGroup
	(
		strGEN_EFFECT.c_str(),
		matEffect,
		&sTargetID
	);

	return TRUE;
}

BOOL GLLandManClient::DropPet ( PSDROPPET pDropPet )
{
	// 중복되면 이전것을 지우다.
	if  ( GetPet ( pDropPet->m_dwGUID ) ) DropOutPet ( pDropPet->m_dwGUID );

	if ( pDropPet->m_sMapID != m_sMapID )	return FALSE;
	
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropPet->m_dwCellID );
	if ( !pLandNode )					return FALSE;

	PGLANYPET pNewPet = new GLAnyPet;
	HRESULT hr = pNewPet->Create ( pDropPet, m_LandMan.GetNaviMesh(), m_pd3dDevice );
	if ( FAILED ( hr ) ) return FALSE;
	
	m_PETArray.insert ( std::make_pair(pDropPet->m_dwGUID,pNewPet) );
	pNewPet->m_pGlobNode = m_GlobAnyPetList.ADDHEAD ( pNewPet );

	pNewPet->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewPet->m_pCellNode = pLandNode->pData->m_PETList.ADDHEAD ( pNewPet );		// - 트리 노드.

	return TRUE;
}

BOOL GLLandManClient::DropOutPet ( DWORD dwGUID )
{
	if ( !GetPet ( dwGUID ) )	return FALSE;

	PGLANYPET pPet = GetPet ( dwGUID );

	PETMAPID_ITER iter = m_PETArray.find(dwGUID);
	if ( m_PETArray.end() != iter ) m_PETArray.erase( iter );

	//	Note : Global-List 해지.
	//
	if ( pPet->m_pGlobNode )
		m_GlobAnyPetList.DELNODE ( pPet->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pPet->m_pQuadNode;

	if ( pPet->m_pCellNode )
		pLandNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pPet);

	return TRUE;
}

BOOL GLLandManClient::CreateAnySummon ( PSDROPSUMMON pDropSummon )
{
	// 중복되면 이전것을 지우다.
	if  ( GetSummon ( pDropSummon->m_dwGUID ) ) DropOutSummon ( pDropSummon->m_dwGUID );

	if ( pDropSummon->m_sMapID != m_sMapID )	return FALSE;

	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropSummon->m_dwCellID );
	if ( !pLandNode )					return FALSE;

	PGLANYSUMMON pNewSummon = new GLAnySummon;
	HRESULT hr = pNewSummon->Create ( pDropSummon, m_LandMan.GetNaviMesh(), m_pd3dDevice );
	if ( FAILED ( hr ) ) return FALSE;

	m_SummonArray.insert ( std::make_pair(pDropSummon->m_dwGUID,pNewSummon) );
	pNewSummon->m_pGlobNode = m_GlobAnySummonList.ADDHEAD ( pNewSummon );

	pNewSummon->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewSummon->m_pCellNode = pLandNode->pData->m_SummonList.ADDHEAD ( pNewSummon );		// - 트리 노드.

	// 생성 효과
	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, pDropSummon->m_vPos.x, pDropSummon->m_vPos.y, pDropSummon->m_vPos.z );

	std::string strGEN_EFFECT = GLCONST_CHAR::strPET_GEN_EFFECT.c_str();
	STARGETID sTargetID(CROW_SUMMON,pNewSummon->m_dwGUID,pNewSummon->GetPosition());
	DxEffGroupPlayer::GetInstance().NewEffGroup
		(
		strGEN_EFFECT.c_str(),
		matEffect,
		&sTargetID
		);

	return TRUE;
}

BOOL GLLandManClient::DropSummon ( PSDROPSUMMON pDropSummon )
{
	// 중복되면 이전것을 지우다.
	if  ( GetSummon ( pDropSummon->m_dwGUID ) ) DropOutSummon ( pDropSummon->m_dwGUID );

	if ( pDropSummon->m_sMapID != m_sMapID )	return FALSE;

	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropSummon->m_dwCellID );
	if ( !pLandNode )					return FALSE;

	PGLANYSUMMON pNewSummon = new GLAnySummon;
	HRESULT hr = pNewSummon->Create ( pDropSummon, m_LandMan.GetNaviMesh(), m_pd3dDevice );
	if ( FAILED ( hr ) ) return FALSE;

	m_SummonArray.insert ( std::make_pair(pDropSummon->m_dwGUID,pNewSummon) );
	pNewSummon->m_pGlobNode = m_GlobAnySummonList.ADDHEAD ( pNewSummon );

	pNewSummon->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewSummon->m_pCellNode = pLandNode->pData->m_SummonList.ADDHEAD ( pNewSummon );		// - 트리 노드.

	return TRUE;
}

BOOL GLLandManClient::DropOutSummon ( DWORD dwGUID )
{
	if ( !GetSummon ( dwGUID ) )	return FALSE;

	PGLANYSUMMON pSummon = GetSummon ( dwGUID );

	SUMMONMAPID_ITER iter = m_SummonArray.find(dwGUID);
	if ( m_SummonArray.end() != iter ) m_SummonArray.erase( iter );

	//	Note : Global-List 해지.
	//
	if ( pSummon->m_pGlobNode )
		m_GlobAnySummonList.DELNODE ( pSummon->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pSummon->m_pQuadNode;

	if ( pSummon->m_pCellNode )
		pLandNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pSummon);

	return TRUE;
}

BOOL GLLandManClient::DropCrow ( SDROP_CROW *pCrowDrop )
{
	HRESULT hr=S_OK;

	if( !pCrowDrop ) return FALSE;

	if ( pCrowDrop->sMapID != m_sMapID )	return E_FAIL;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pCrowDrop->dwCeID );
	if ( !pLandNode )	return	FALSE;

	//	Note : 메모리를 할당.
	//
	GLCrowClient *pNewCrow = new GLCrowClient;
	hr = pNewCrow->CreateCrow ( this, pCrowDrop, m_pd3dDevice );
	if ( FAILED(hr) )	return FALSE;

	//	Note : 관리 ID 부여.
	//
	pNewCrow->m_dwGlobID = pCrowDrop->dwGlobID;
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_CROWArray[pCrowDrop->dwGlobID] = pNewCrow;								// - 전역.
	pNewCrow->m_pGlobNode = m_GlobCROWList.ADDHEAD ( pNewCrow );				// - 전역.

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandManClient::DropMaterial ( SDROP_MATERIAL *pCrowDrop )
{
	HRESULT hr=S_OK;

	if( !pCrowDrop ) return FALSE;

	if ( pCrowDrop->sMapID != m_sMapID )	return E_FAIL;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pCrowDrop->dwCeID );
	if ( !pLandNode )	return	FALSE;

	//	Note : 메모리를 할당.
	//
	GLMaterialClient *pNewMaterial = new GLMaterialClient;
	hr = pNewMaterial->CreateMaterial ( this, pCrowDrop, m_pd3dDevice );
	if ( FAILED(hr) )	return FALSE;

	//	Note : 관리 ID 부여.
	//
	pNewMaterial->m_dwGlobID = pCrowDrop->dwGlobID;
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_MaterialArray[pCrowDrop->dwGlobID] = pNewMaterial;								// - 전역.
	pNewMaterial->m_pGlobNode = m_GlobMaterialList.ADDHEAD ( pNewMaterial );				// - 전역.

	pNewMaterial->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterailList.ADDHEAD ( pNewMaterial );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandManClient::DropChar ( SDROP_CHAR *pCharDrop )
{
	HRESULT hr=S_OK;

	if( !pCharDrop ) return FALSE;

	if ( pCharDrop->sMapID != m_sMapID )	return E_FAIL;

	//	Note : 같은 ID가 이미 존재하는지 검사.
	//
	if ( FindChar(pCharDrop->dwGaeaID) )
	{
		//	Note : 이전에 존재하는 것을 제거함.
		//
		DropOutChar ( pCharDrop->dwGaeaID );
	}

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pCharDrop->dwCeID );
	if ( !pLandNode )	return	FALSE;

	//	Note : 메모리를 할당.
	//
	GLCharClient *pNewChar = new GLCharClient;
	hr = pNewChar->Create ( this, pCharDrop, m_pd3dDevice );
	if ( FAILED(hr) )	return FALSE;

	//	Note : 관리 ID 부여.
	//
	pNewChar->m_dwGaeaID = pCharDrop->dwGaeaID;
	pNewChar->m_dwCeID = pLandNode->dwCID;

	//	Note : 관리 노드 포인터.
	//
	m_PCArray.insert ( std::make_pair(pNewChar->m_dwGaeaID,pNewChar) );			// - 전역.

	//	Note : 케릭터 이름 맵 등록하기.
	//
	m_mapPC.insert ( std::make_pair(pNewChar->GetCharData().szName,pNewChar) );

	pNewChar->m_pGlobNode = m_GlobPCList.ADDHEAD ( pNewChar );					// - 전역.

	pNewChar->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewChar->m_pCellNode = pLandNode->pData->m_PCList.ADDHEAD ( pNewChar );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandManClient::DropOutItem ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXITEM );
	if ( m_ItemArray[dwGlobID] == NULL )	return FALSE;

	PITEMCLIENTDROP pItemDrop = m_ItemArray[dwGlobID];
	m_ItemArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pItemDrop->pGlobList )
		m_GlobItemList.DELNODE ( pItemDrop->pGlobList );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pItemDrop->pQuadNode;
	if ( pItemDrop->pCellList )
		pLandNode->pData->m_ItemList.DELNODE ( pItemDrop->pCellList );


	STARGETID sTARID ( CROW_ITEM, pItemDrop->dwGlobID, pItemDrop->vPos );
	DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sTARID );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pItemDrop);

	return TRUE;
}

BOOL GLLandManClient::DropOutMoney ( DWORD dwGlobID )
{
	//GASSERT ( dwGlobID<MAXMONEY );
	if ( dwGlobID >= MAXMONEY )
	{
		CDebugSet::ToLogFile( "GLLandManClient::DropOutMoney() -- dwGlobID >= MAXMONEY" );
		return FALSE;
	}
	if ( m_MoneyArray[dwGlobID] == NULL )	return FALSE;

	PMONEYCLIENTDROP pMoneyDrop = m_MoneyArray[dwGlobID];
	m_MoneyArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pMoneyDrop->pGlobList )
		m_GlobMoneyList.DELNODE ( pMoneyDrop->pGlobList );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pMoneyDrop->pQuadNode;
	if ( pMoneyDrop->pCellList )
		pLandNode->pData->m_MoneyList.DELNODE ( pMoneyDrop->pCellList );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pMoneyDrop);

	return TRUE;
}

BOOL GLLandManClient::DropOutCrow ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXCROW );
	if ( m_CROWArray[dwGlobID] == NULL )	return FALSE;

	PGLCROWCLIENT pCrow = m_CROWArray[dwGlobID];
	m_CROWArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pCrow->m_pGlobNode )
		m_GlobCROWList.DELNODE ( pCrow->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pCrow->m_pQuadNode;

	if ( pCrow->m_pCellNode )
		pLandNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pCrow);

	return TRUE;
}

BOOL GLLandManClient::DropOutMaterial ( DWORD dwGlobID )
{
	GASSERT ( dwGlobID<MAXCROW );
	if ( m_MaterialArray[dwGlobID] == NULL )	return FALSE;

	PGLMATERIALCLIENT pMaterial = m_MaterialArray[dwGlobID];
	m_MaterialArray[dwGlobID] = NULL;

	//	Note : Global-List 해지.
	//
	if ( pMaterial->m_pGlobNode )
		m_GlobMaterialList.DELNODE ( pMaterial->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	CLIENTQUADNODE* pLandNode = pMaterial->m_pQuadNode;

	if ( pMaterial->m_pCellNode )
		pLandNode->pData->m_MaterailList.DELNODE ( pMaterial->m_pCellNode );

	//	Note : 메모리 해지.
	//
	SAFE_DELETE(pMaterial);

	return TRUE;
}

BOOL GLLandManClient::DropOutChar ( DWORD dwGlobID )
{
	if ( !FindChar(dwGlobID) )	return FALSE;

	GLCharClient* pChar = FindChar(dwGlobID);
	
	//	Note : 케릭터 ID 맵 지워주기.
	//
	{
		CHARMAPID_ITER iter = m_PCArray.find(dwGlobID);
		if ( m_PCArray.end()!=iter )
		{
			m_PCArray.erase( iter );
		}
		else
		{
			//std::strstream strText;
			//strText << std::endl;
			//strText << "GLLandManClient::DropOutChar m_PCArray.end()!=iter" << std::ends;
			CDebugSet::ToLogFile( "\nGLLandManClient::DropOutChar m_PCArray.end()!=iter" );
			//strText.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.			
		}
	}

	//	Note : 케릭터 이름 맵 지워주기.
	//
	{
		CHARMAP_ITER iter = m_mapPC.find ( pChar->GetCharData().szName );
		if ( m_mapPC.end()!=iter )
		{
			m_mapPC.erase( iter );
		}
		else
		{
			//std::strstream strText;
			//strText << std::endl;
			//strText << "GLLandManClient::DropOutChar m_mapPC.end()!=iter" << std::ends;
			CDebugSet::ToLogFile( "\nGLLandManClient::DropOutChar m_mapPC.end()!=iter" );
			//strText.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		}		
	}

	//	Note : Gaea-List 해지.
	//
	if ( pChar->m_pGlobNode )
		m_GlobPCList.DELNODE ( pChar->m_pGlobNode );

	//	Note : QuadNode-List 해지.
	//
	if ( pChar->m_pQuadNode && pChar->m_pCellNode )
		pChar->m_pQuadNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode );

	//	Note : CHAR 메모리 해제.
	//
	SAFE_DELETE(pChar);

	return TRUE;
}

PGLCHARCLIENT GLLandManClient::FindChar ( std::string strName )
{
	CHARMAP_ITER iter = m_mapPC.find ( strName );
	if ( m_mapPC.end()!=iter )	return (*iter).second;

	return NULL;
}

PGLCHARCLIENT GLLandManClient::FindChar ( DWORD dwID )
{
	CHARMAPID_ITER iter = m_PCArray.find ( dwID );
	if ( m_PCArray.end()!=iter )	return (*iter).second;

	return NULL;
}

WORD GLLandManClient::GetTargetBodyRadius ( const STARGETID &sTargetID )
{
	//	Note : 타겟의 위치 정보를 가져옴.
	if ( sTargetID.emCrow == CROW_PC )
	{
		PGLCHARCLIENT pChar = GetChar ( sTargetID.dwID );
		if ( pChar ) return pChar->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB )
	{
		PGLCROWCLIENT pCrow = GetCrow ( sTargetID.dwID );
		if ( pCrow ) return pCrow->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIALCLIENT pMaterial = GetMaterial ( sTargetID.dwID );
		if ( pMaterial ) return pMaterial->GetBodyRadius();
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}
	else if( sTargetID.emCrow == CROW_SUMMON )	// SummonData
	{
		PGLANYSUMMON pSummon = GetSummon ( sTargetID.dwID );
		if ( pSummon ) return pSummon->GETBODYRADIUS();
	}
	else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}

	return WORD(0xFFFF);
}

HRESULT GLLandManClient::ClearDropObj ()
{
	DWORD i=0;
	for ( i=0; i<MAXITEM; ++i )		DropOutItem(i);
	for ( i=0; i<MAXMONEY; ++i )	DropOutMoney(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutCrow(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutMaterial(i);
	
	CHARMAPID_ITER iter_del;
	CHARMAPID_ITER iter = m_PCArray.begin();
	CHARMAPID_ITER iter_end = m_PCArray.end();
	for ( ; iter!=iter_end; )
	{
		iter_del = iter++;
		DropOutChar ( (*iter_del).first );
	}

	PETMAPID_ITER iterPet_del;
	PETMAPID_ITER iterPet = m_PETArray.begin();
	PETMAPID_ITER iterPet_end = m_PETArray.end();
	for ( ; iterPet!=iterPet_end; )
	{
		iterPet_del = iterPet++;
		DropOutPet ( (*iterPet_del).first );
	}

	SUMMONMAPID_ITER iterSummon_del;
	SUMMONMAPID_ITER iterSummon = m_SummonArray.begin();
	SUMMONMAPID_ITER iterSummon_end = m_SummonArray.end();
	for ( ; iterSummon!=iterSummon_end; )
	{
		iterSummon_del = iterSummon++;
		DropOutSummon ( (*iterSummon_del).first );
	}

	return S_OK;
}

void GLLandManClient::DisplayAllUser ()
{
	CHARMAPID_ITER iter = m_PCArray.begin();
	CHARMAPID_ITER iter_end = m_PCArray.end();

	for ( ; iter!=iter_end; )
	{
		PGLCHARCLIENT pChar = iter->second;
		CInnerInterface::GetInstance().PrintConsoleText ( "%s	: charid = %d",
						pChar->m_strName.c_str(), pChar->GetCharData().dwCharID );
		++iter;
	}
}

BOOL GLLandManClient::ChangeCharMap ( char* szOldName, char* szNewName )
{
	CHARMAP_ITER iter = m_mapPC.find ( szOldName );

	if ( iter != m_mapPC.end() )
	{
		PGLCHARCLIENT pCHAR = (*iter).second;
		m_mapPC.insert ( std::make_pair ( szNewName, pCHAR ) );
		m_mapPC.erase ( iter );
		pCHAR->m_strName = szNewName;
	}

	return TRUE;
}

BOOL GLLandManClient::ResetClientObjects ()
{
	DWORD i=0;
	for ( i=0; i<MAXITEM; ++i )		DropOutItem(i);
	for ( i=0; i<MAXMONEY; ++i )	DropOutMoney(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutCrow(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutMaterial(i);

	m_GlobItemList.DELALL ();
	m_GlobMoneyList.DELALL ();
	m_GlobCROWList.DELALL ();
	m_GlobMaterialList.DELALL ();
	m_GlobAnyPetList.DELALL ();
	m_GlobAnySummonList.DELALL ();
	return TRUE;
}

