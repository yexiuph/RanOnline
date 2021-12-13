#include "pch.h"
#include "./GLPetField.h"

#include "./GLGaeaserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPetField::GLPetField(void) :
	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),
	m_pLandMan(NULL),
	m_pLandNode(NULL),

	m_vTarPos(D3DXVECTOR3(0,0,0)),

	m_pQuadNode(NULL),
	m_pCellNode(NULL),

	m_dwActionFlag(0),
	m_fTIMER(0.0f),
	m_bValid(false),
	m_wAniSub(0),

	m_dwOwnerCharID(0),
	m_hCheckStrDLL( NULL ),
	m_pCheckString( NULL )

{
}

GLPetField::~GLPetField(void)
{
}

HRESULT GLPetField::Create ( GLLandMan* pLandMan, PGLCHAR pOwner, PGLPET pPetData )
{
	if ( !pPetData )	return E_FAIL;
	if ( !pLandMan )	return E_FAIL;
	if ( !pOwner )		return E_FAIL;

	ASSIGN ( *pPetData );

	m_pOwner   = pOwner;
	m_pLandMan = pLandMan;

	SetValid ();

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos = GLPETDEFINE::GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	m_dwOwner = pOwner->m_dwGaeaID;
	m_sMapID  = pOwner->m_sMapID;

	m_dwOwnerCharID = pOwner->m_dwCharID;

	m_actorMove.Create ( m_pLandMan->GetNavi (), m_vPos, -1 );
	m_actorMove.Stop ();

#ifdef TH_PARAM
	HMODULE m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}
#endif

	return S_OK;
}

HRESULT GLPetField::DeleteDeviceObject ()
{
	CleanUp ();
	return S_OK;
}

void GLPetField::CleanUp ()
{
	m_vPos	   = D3DXVECTOR3(0,0,0);
	m_vDir	   = D3DXVECTOR3(0,0,-1);
	m_vTarPos  = D3DXVECTOR3(0,0,0);
	m_vDirOrig = D3DXVECTOR3(0,0,-1);

	m_pLandMan  = NULL;
	m_pLandNode = NULL;
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_wAniSub		= 0;
	m_fTIMER		= 0.0f;
	m_bValid		= false;
	m_dwActionFlag  = 0;
	m_dwOwnerCharID = 0;

	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

#ifdef TH_PARAM
    if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif

	RESET ();
}

HRESULT GLPetField::SetPosition ( GLLandMan* pLandMan )
{
	if ( !pLandMan )	return E_FAIL;

	m_pLandMan = pLandMan;

	SetValid ();

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos = GLPETDEFINE::GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	m_sMapID  = m_pOwner->m_sMapID;

	m_actorMove.Create ( m_pLandMan->GetNavi (), m_vPos, -1 );
	m_actorMove.Stop ();

	return S_OK;
}

HRESULT GLPetField::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);
	static float fTimer(0.0f);

	// PETSKINPACK 적용시
	if( IsUsePetSkinPack() )
	{
		CTime currentTime = CTime::GetCurrentTime();
		CTimeSpan timeSpan = currentTime - m_sPetSkinPackData.startTime;
		if( timeSpan.GetTotalSeconds() >= m_sPetSkinPackData.dwPetSkinTime )
		{
			m_sPetSkinPackData.Init();

			GLMSG::SNETPET_SKINPACKOPEN_FB NetMsgFB;
			NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_END;
			NetMsgFB.sPetSkinPackData.Init();
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsgFB );

			GLMSG::SNETPET_SKINPACKOPEN_BRD NetMsgBRD;
			NetMsgBRD.dwGUID		   = m_dwGUID;
			NetMsgBRD.sPetSkinPackData.Init();
			m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );
		}
	}

	// 움직임 상태 제어
	if ( IsSTATE ( EM_PETACT_MOVE ) )	
	{
		// ============== Update about Actor (Begin) ============== /

		hr = m_actorMove.Update ( fElapsedTime );
		if ( FAILED(hr) )	return E_FAIL;

		if ( !m_actorMove.PathIsActive() ) m_actorMove.Stop ();

		// 방향 업데이트
		D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
		if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
		{
			D3DXVECTOR3 vDirection = vMovement - m_vPos;
			if ( !DxIsMinVector(vDirection,0.2f) )
			{
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}

		// ============== Update about Actor (end)   ============== /
	}

	// 클라이언트 상태 업데이트
	UpdateClientState ( fElapsedTime );

	// 스킬딜레이 업데이트
	// UpdateSkillDelay ( fElapsedTime ); 기획팀 김병욱씨 요청으로 제거 [06.10.09]

	// 스킬 업데이트
	// UpdateSkillState ( fElapsedTime ); 기획팀 김병욱씨 요청으로 제거 [06.10.09]

	// 현재 위치 업데이트
	m_vPos = m_actorMove.Position();

	return S_OK;
}

HRESULT GLPetField::UpdateClientState ( float fElapsedTime )
{
	m_fTIMER += fElapsedTime;
	// 팻의 포만도 갱신
	if ( m_fTIMER > 3600.0f/GLPeriod::REALTIME_TO_VBR )
	{
		m_fTIMER = 0;
		m_nFull -= GLCONST_PET::nFullDecrement[m_emTYPE];
		if ( m_nFull < 0 ) m_nFull = 0;

		// 클라이언트 포만도 갱신
		GLMSG::SNETPET_UPDATE_CLIENT_PETFULL NetMsg;
		NetMsg.nFull = m_nFull;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsg );
	}
	
	return S_OK;
}

NET_MSG_GENERIC* GLPetField::ReqNetMsg_Drop ()
{
	static GLMSG::SNETPET_DROP_PET NetMsg;
	NetMsg = GLMSG::SNETPET_DROP_PET();

	// 클라이언트에 뿌려줘야할 팻정보 설정
	
	NetMsg.Data.m_emTYPE			= m_emTYPE;
	NetMsg.Data.m_dwGUID			= m_dwGUID;
	NetMsg.Data.m_sPetID			= m_sPetID;
	NetMsg.Data.m_dwOwner			= m_dwOwner;
	NetMsg.Data.m_wStyle			= m_wStyle;
	NetMsg.Data.m_wColor			= m_wColor;
	NetMsg.Data.m_nFull				= m_nFull;
	NetMsg.Data.m_vPos				= m_vPos;
	NetMsg.Data.m_vDir				= m_vDir;
	NetMsg.Data.m_sMapID			= m_sMapID;
	NetMsg.Data.m_dwCellID			= m_dwCellID;
	NetMsg.Data.m_fRunSpeed			= m_fRunSpeed;
	NetMsg.Data.m_fWalkSpeed		= m_fWalkSpeed;
	NetMsg.Data.m_dwActionFlag		= m_dwActionFlag;
	NetMsg.Data.m_vTarPos			= m_vTarPos;
	NetMsg.Data.m_wAniSub			= m_wAniSub;
	NetMsg.Data.m_petSkinPackData	= m_sPetSkinPackData;

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		NetMsg.Data.m_PutOnItems[i] = m_PutOnItems[i];
	}

	StringCchCopy ( NetMsg.Data.m_szName, PETNAMESIZE+1, m_szName );

	return (NET_MSG_GENERIC*) &NetMsg;
}

bool GLPetField::CheckSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_CITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )
	{
		// 배우지 않은 스킬
		return false;
	}

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter!=m_SKILLDELAY.end() )
	{
		// 스킬딜레이
		return false;
	}
	return true;
}

void GLPetField::AccountSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( sSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return;
	PETSKILL sPetSkill = (*iter).second;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					 return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	// 메시지 딜레이만큼 더해준다.
	float fServerDelay = sSKILL_DATA.fDELAYTIME + NET_MSGDELAY; 
	m_SKILLDELAY.insert ( std::make_pair(sSkillID.dwID, fServerDelay) );
}

void GLPetField::UpdateSkillDelay ( float fElapsedTime )
{
	if ( m_SKILLDELAY.size() < 1 ) return;
	DELAY_MAP_ITER iter_del;

	PETDELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	PETDELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay <= 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

void GLPetField::UpdateSkillState ( float fElapsedTime )
{
	if ( m_sActiveSkillID == NATIVEID_NULL () ) return;
	
	if ( m_pOwner->m_sPETSKILLFACT.fAGE < 0.0f )
	{
		if ( m_pOwner->m_sPETSKILLFACT.emTYPE == SKILL::EMFOR_PET_PROTECTITEMS )
		{
			m_pOwner->m_bProtectPutOnItem = false;
		}

		m_pOwner->m_sPETSKILLFACT.RESET ();
		m_sActiveSkillID = NATIVEID_NULL ();
		GLMSG::SNETPET_REMOVE_SKILLFACT NetMsg;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsg );

		// 주변에 알림
		GLMSG::SNETPET_REQ_SKILLCHANGE_BRD NetMsgBRD;
		NetMsgBRD.dwGUID   = m_dwGUID;
		NetMsgBRD.dwTarID  = m_pOwner->m_dwGaeaID;
		NetMsgBRD.sSkillID = NATIVEID_NULL ();
		m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );
	}
}