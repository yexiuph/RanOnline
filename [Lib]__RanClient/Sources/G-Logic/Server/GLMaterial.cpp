#include "pch.h"
#include "./GLMaterial.h"
#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BOX_R	(4)


GLMaterial::GLMaterial(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),
	m_vDir(0,0,-1),

	m_vMaxOrg(BOX_R,20,BOX_R),
	m_vMinOrg(-BOX_R,0,-BOX_R),

	m_vMax(BOX_R,20,BOX_R),
	m_vMin(-BOX_R,0,-BOX_R),
	m_dwGlobID(0),

	m_dwCeID(0),
	m_pLandMan(NULL),

	m_pGlobNode(NULL),
	m_pQuadNode(NULL),
	m_pCellNode(NULL),

	m_fAge(0.0f),
	m_Action(GLAT_IDLE),
	m_dwActState(NULL),
	m_dwCFlag(NULL),
	m_vPos(0,0,0),
	m_vGenPos(0,0,0),
	m_vStartPos(0,0,0),

	m_fIdleTime(0.0f),
	m_pMobSchedule(NULL),
	m_dwGatherCharID(GAEAID_NULL),
	m_fGatherTime(0.0f)
{	
}

GLMaterial::~GLMaterial(void)
{
	SAFE_DELETE(m_pSkinChar);
}

void GLMaterial::RESET_DATA ()
{
	int i=0;

	m_pd3dDevice = NULL;
	
	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_pSkinChar = NULL;

	m_vDir = D3DXVECTOR3(0,0,0);
	
	D3DXMatrixIdentity ( &m_matTrans );

	m_vMaxOrg = D3DXVECTOR3(0,0,0);
	m_vMinOrg = D3DXVECTOR3(0,0,0);

	m_vMax = D3DXVECTOR3(0,0,0);
	m_vMin = D3DXVECTOR3(0,0,0);

	m_dwGlobID = 0;

	m_sMapID = SNATIVEID(0,0);
	m_dwCeID = 0;
	
	m_pLandMan = NULL;
	m_pGlobNode = NULL;
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_fAge = 0.0f;
	m_Action = GLAT_IDLE;
	m_dwActState = NULL;
	m_dwCFlag = NULL;
	m_vPos = D3DXVECTOR3(0,0,0);
	m_vGenPos = D3DXVECTOR3(0,0,0);
	m_vStartPos = D3DXVECTOR3(0,0,0);
	m_fIdleTime = 0.0f;

	m_dwGatherCharID = GAEAID_NULL;
	m_fGatherTime = 0.0f;
}

HRESULT GLMaterial::CreateMaterial ( GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, SNATIVEID sCROWID,
							LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXVECTOR3 pPos )
{
	GASSERT(pLandMan);

	m_pLandMan = pLandMan;
	m_pd3dDevice = pd3dDevice;

	//	Note : 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sCROWID );
	if( !m_pCrowData )
	{
		DEBUGMSG_WRITE ( _T("GLMaterial::CreateMaterial, Meterial Create Failed. nativeid [%d/%d]"), sCROWID.wMainID, sCROWID.wSubID );
		return E_FAIL;
	}

	m_sNativeID = m_pCrowData->sNativeID;

	//	Note : 스케쥴러 초기화.
	m_pMobSchedule = pMobSchedule;

	//	위치 초기화.
	if ( pPos )					m_vPos = *pPos;
	else if ( m_pMobSchedule )	m_vPos = m_pMobSchedule->m_pAffineParts->vTrans;
	
	m_vGenPos = m_vPos;

	//	방향 초기화.
	D3DXMATRIX matY;
	D3DXMatrixIdentity ( &matY );
	
	if ( m_pMobSchedule )
		D3DXMatrixRotationY ( &matY, m_pMobSchedule->m_pAffineParts->vRotate.x );

	D3DXVec3TransformCoord ( &m_vDir, &vDIRORIG, &matY );

	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();
	m_actorMove.Create ( pLandMan->GetNavi(), m_vPos, -1 );

	//	Note : 주변 케릭터 인식.
	//
	UpdateViewAround ();

	if ( m_pd3dDevice )
	{
		DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice, TRUE );
		if ( !pCharData )	return E_FAIL;

		SAFE_DELETE(m_pSkinChar);
		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData ( pCharData, pd3dDevice );
	}

	return S_OK;
}

void GLMaterial::TurnAction ( EMACTIONTYPE toAction, bool bMsg )
{
	//	Note : 이전 액션 취소.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	default:
		break;
	};

	//	Note : 액션 초기화.
	//
	EMACTIONTYPE emOldAction = m_Action;
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;
		}
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_FALLING:
		{
			m_fIdleTime = 0.0f;
			if ( !bMsg ) break;

			//	Note : 주변 Char에게 쓰러짐을 알림.
			GLMSG::SNET_ACTION_BRD NetMsg;
			NetMsg.emCrow	= m_pCrowData->m_emCrow;
			NetMsg.dwID		= m_dwGlobID;
			NetMsg.emAction	= m_Action;

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
		}
		break;

	default:
		break;
	};
}

NET_MSG_GENERIC* GLMaterial::ReqNetMsg_Drop ()
{
	static GLMSG::SNETDROP_MATERIAL NetMsg;
	NetMsg = GLMSG::SNETDROP_MATERIAL();

	SDROP_MATERIAL &dropCrow	= NetMsg.Data;
	dropCrow.sNativeID			= m_sNativeID;

	dropCrow.sMapID				= m_sMapID;
	dropCrow.dwCeID				= m_dwCeID;
	dropCrow.dwGlobID			= m_dwGlobID;
	dropCrow.vPos				= m_vPos;
	dropCrow.vDir				= m_vDir;

	if ( m_fAge < 0.6f )
		dropCrow.dwFLAGS	|= SDROP_CROW::CROW_GEM;

	dropCrow.emAction		= m_Action;
	dropCrow.dwActState		= m_dwActState;

	return (NET_MSG_GENERIC*) &NetMsg;
}

inline void GLMaterial::INSERT_PC_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecPC_VIEW.size() < m_dwPC_AMOUNT+16 )
		m_vecPC_VIEW.resize(m_dwPC_AMOUNT+16);

	m_vecPC_VIEW[m_dwPC_AMOUNT++] = dwGAEAID;
}

HRESULT GLMaterial::UpdateViewAround ()
{
	int nX, nZ;

	m_dwPC_AMOUNT = 0;

	//	Note : 현재 시야에 들어오는 CELL 모두 탐색.
	//
	nX = int ( m_vPos.x );
	nZ = int ( m_vPos.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+MAX_VIEWRANGE,nZ+MAX_VIEWRANGE,nX-MAX_VIEWRANGE,nZ-MAX_VIEWRANGE);
	m_pLandMan->GetLandTree()->FindNodes ( bRect, m_pLandMan->GetLandTree()->GetRootNode(), &pQuadHead );

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();

	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode ) continue;
		if ( pQuadNode->pData->m_PCList.m_dwAmount==0 )						continue;
		
		//	Note : - Char - 
		//
		GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar *pPChar = pCharNode->Data;
			if( !pPChar ) continue;

			//	Note : 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
			if ( !pPChar )													
				continue;

			if ( !glGaeaServer.GetChar ( pPChar->m_dwGaeaID ) )	
				continue;

			if ( bRect.IsWithIn(nX,nZ) )
			{
				INSERT_PC_VIEW ( pPChar->m_dwGaeaID );
			}
		}
	}

	return S_OK;
}

HRESULT GLMaterial::SendMsgViewAround ( NET_MSG_GENERIC* nmg )
{
	DWORD dwGAEAID = 0;
	GLChar* pPChar = NULL;

	GLGaeaServer & glGaeaServer = GLGaeaServer::GetInstance();
	DWORD wSize = (DWORD)m_vecPC_VIEW.size();

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<wSize; ++i )
	{
		//	Memo :	m_vecPC_VIEW[i] is GaeaID
		pPChar = glGaeaServer.GetChar ( m_vecPC_VIEW[i] );
		if ( pPChar )
		{
			glGaeaServer.SENDTOCLIENT ( pPChar->m_dwClientID, nmg );
		}
	}

	return S_OK;
}

BOOL GLMaterial::IsValidBody () const
{
	return ( m_Action < GLAT_FALLING );
}

BOOL GLMaterial::IsDie () const
{
	if ( m_Action==GLAT_DIE )		
		return TRUE;

	return FALSE;
}

HRESULT GLMaterial::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	float fAnimation = fElapsedTime;

	EMANI_MAINTYPE MType = m_pSkinChar->GETCURMTYPE();
	EMANI_SUBTYPE SType = m_pSkinChar->GETCURSTYPE();
	float fLIFETIME = m_pSkinChar->GETCURANITIME();

	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		fAnimation = 0.0f;
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;
	};

	//	Note : Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( m_Action )
	{
	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		break;
	};

	m_pSkinChar->SetPosition ( m_vPos );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap );

	return S_OK;
}

HRESULT GLMaterial::UpdateGathering( float fElapsedTime )
{
	if ( m_dwGatherCharID == GAEAID_NULL )	return S_OK;

	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar( m_dwGatherCharID );
	if ( !pChar || !pChar->IsACTION( GLAT_GATHERING ) )
	{
		m_dwGatherCharID = GAEAID_NULL;
		return S_OK;
	}

	m_fGatherTime -= fElapsedTime;
			
	// 채집 완료 결과 전송
	if ( m_fGatherTime < 0.0f )
	{
		GLMSG::SNETPC_REQ_GATHERING_RESULT NetMsg;
		if ( RANDOM_GEN( m_pCrowData->m_fGatherRate ) ) 
		{
			bool bDrop = false;

			if ( !GenerateGathering ( pChar->m_dwGaeaID, bDrop ) ) 
			{
				NetMsg.emFB = EMREQ_GATHER_RESULT_ITEMFAIL;
			}
			else
			{
				if ( bDrop ) NetMsg.emFB = EMREQ_GATHER_RESULT_SUCCESS_EX;
				else NetMsg.emFB = EMREQ_GATHER_RESULT_SUCCESS;				
			}
		}
		else
		{
			NetMsg.emFB = EMREQ_GATHER_RESULT_FAIL;
		}
        		
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );				

		GLMSG::SNETPC_REQ_GATHERING_RESULT_BRD	NetMsgBrd;
		NetMsgBrd.dwGaeaID = pChar->m_dwGaeaID;
		NetMsgBrd.emFB = NetMsg.emFB;
		pChar->SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsgBrd );

		// 보상 지급			
		
		m_dwGatherCharID = GAEAID_NULL;
		// 죽음( 사라짐 )
		TurnAction ( GLAT_FALLING );
		pChar->ResetAction();
	}

	return S_OK;
}

HRESULT GLMaterial::FrameMove ( float fTime, float fElapsedTime )
{
	GASSERT(m_pCrowData);
	GASSERT(m_pLandMan);



	UpdateGathering( fElapsedTime );
	//	Note : 랜드에서 등록된 셀을 업데이트 하기 위해서 호출함.
	//
	UpdateViewAround ();

	//	현재 주위에 케릭터가 없고, skinchar가 무효일때(에뮬레이터,level edit)가 아닐때. & 보스몹이 아닐때
	if ( m_dwPC_AMOUNT==0 && !m_pSkinChar ) return S_OK;

	m_fAge += fElapsedTime;

	//	Note : AABB 계산.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	//	Note : 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//



	//	Note : 액션별 처리작업.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_FALLING:
		m_fIdleTime += fElapsedTime;
		if ( m_fIdleTime > 4.0f )	TurnAction ( GLAT_DIE );
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob의 현제 위치 업데이트.
	//
	m_vPos = m_actorMove.Position();

	//	Note : m_pd3dDevice 가 유효할 경우만 아랫 코드가 실행된다. ( Level Editor 에서 태스트용. )
	//
	if ( !m_pd3dDevice )	return S_OK;

	//	Note : 에니메이션, 스킨 업데이트.
	//
	UpateAnimation ( fTime, fElapsedTime );

	//	Note : 현제 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, vDIRORIG );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	return S_OK;
}

HRESULT GLMaterial::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	HRESULT hr;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	if ( bRendAABB ) EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	테스트용.
	
	hr = m_pSkinChar->Render ( pd3dDevice, m_matTrans );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}


DWORD GLMaterial::GenerateGathering( DWORD dwGaeaID, bool& bDrop )
{
	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	//	Note : 획득 권한 옵션 설정.
	//

	if ( dwGaeaID == GAEAID_NULL ) return FALSE;

	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar ) return FALSE;

	float fGEN_RATE = 1.0f;
	bool bgenitemhold = GLGaeaServer::GetInstance().IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
	}
	else 
	{
		emItemHoldGrp = EMGROUP_ONE;
		dwItemHoldID = dwGaeaID;			
	}

	float fGenRate = 1.0f; 

	fGenRate = CalculateItemGenRate( fGenRate, pChar, false );
	
	SGENITEM &sGenItem = m_pCrowData->m_sGenItem;

	if ( sGenItem.ISGEN(fGenRate) )
	{
		WORD wGenNum = sGenItem.SELECTNUM();
		
		for ( WORD i=0; i<wGenNum; ++i )
		{
			bool bSEPCID = false;
			DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
			SNATIVEID sNID;
			
			if ( dwSpecID==SGENITEM::EMSPEC_NULL )	return FALSE;
			
			if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
			else	sNID = dwSpecID;
			
			if ( sNID==NATIVEID_NULL() )	return FALSE;
		
			SITEM* pItemGather = GLItemMan::GetInstance().GetItem( sNID );
			if ( !pItemGather ) return FALSE;

			SITEMCUSTOM sITEM_NEW;
			sITEM_NEW.sNativeID = sNID;
			sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
			sITEM_NEW.cGenType = EMGEN_GATHERING;
			sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
			sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
			sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW;
			if ( pChar->IsInsertToInven ( &cDropItem ) )
			{
				pChar->InsertToInven ( &cDropItem );
				
				GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_MOB, m_pCrowData->sNativeID.dwID, ID_CHAR, pChar->m_dwCharID, 
					EMITEM_ROUTE_GATHERING, sITEM_NEW.wTurnNum );
			}
			else
			{
				if ( m_pLandMan )
				{
					bDrop = true;
					m_pLandMan->DropItem ( m_vPos, 
											&(cDropItem.sItemCustom), 
											emItemHoldGrp, 
											dwItemHoldID );
				}
			}

		}
	}
	
	return TRUE;	
}


// 이벤트나 탐닉 방지 시스템에 의해 변경된 아이템 드랍율을 구한다.
float GLMaterial::CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty )
{
	float fEVENT_RATE = 1.0f; 
	if( pChar ) fEVENT_RATE = GLGaeaServer::GetInstance().GetItemGainRate(pChar->m_CHARINDEX,pChar->m_wLevel);

	float fRate = fGenRate * fEVENT_RATE;

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( pChar )
	{
		if( pChar->m_ChinaGainType == GAINTYPE_HALF )
		{
			fRate /= 2.0f;
		}else if( pChar->m_ChinaGainType == GAINTYPE_EMPTY )
		{
			fRate = 0.0f;
		}
	}
#endif
#ifndef CH_PARAM_USEGAIN //** Add EventTime
	if( pChar )
	{
		if( pChar->m_bEventStart && pChar->m_bEventApply)
		{
			SEventState sEventState = GLGaeaServer::GetInstance().m_sEventState;
			if( sEventState.bEventStart )
				fRate *= sEventState.fItemGainRate;
		}
	}
#endif

	return fRate;
}