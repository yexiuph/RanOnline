#include "pch.h"
#include "./GLCrowClient.h"
#include "./GLFactEffect.h"
#include "./GLGaeaClient.h"
#include "./GLStrikeM.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/DamageDisplay.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"

#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"
#include "../[Lib]__Engine/Sources/DxTools/DxShadowMap.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffCharData.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLCrowClient::GLCrowClient(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(0,0,-1),
	m_vDirOrig(0,0,-1),
	m_vPos(0,0,0),
	m_fScale( 0.0f ),

	m_vServerPos(0,0,0),

	m_vMaxOrg(8,20,8),
	m_vMinOrg(-8,0,-8),

	m_vMax(8,20,8),
	m_vMin(-8,0,-8),
	m_fHeight(20.f),

	m_dwGlobID(0),
	m_dwCeID(0),
	m_pLandManClient(NULL),
	m_pGlobNode(NULL),
	m_pQuadNode(NULL),
	m_pCellNode(NULL),

	m_fAge(0.0f),
	m_Action(GLAT_IDLE),
	m_dwActState(0),

	m_wTARNUM(0),
	m_vTARPOS(0,0,0),

	m_pAttackProp(NULL),

	m_fattTIMER(0.0f),
	m_nattSTEP(0),

	m_fIdleTime(0.0f),
	m_fStayTimer(0.0f),
	m_fAttackTimer(0.0f),
	m_fTargetTimer(0.0f),

	m_fMoveDelay(0.0f)
{
	m_sTargetID.dwID = EMTARGET_NULL;
}

GLCrowClient::~GLCrowClient(void)
{
	FACTEFF::DeleteAllEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

	m_pd3dDevice = NULL;

	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_pLandManClient = NULL;
	m_pGlobNode = NULL;
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_sTargetID.RESET();
	
	SAFE_DELETE(m_pSkinChar);
}

HRESULT GLCrowClient::CreateCrow ( GLLandManClient *pLandManClient, SDROP_CROW *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	GASSERT(pLandManClient);

	m_pLandManClient = pLandManClient;
	m_pd3dDevice = pd3dDevice;

	//	Note : 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCrowDrop->sNativeID );
	if( !m_pCrowData )
	{
		SNATIVEID sMobID = pCrowDrop->sNativeID;
		CDebugSet::ToLogFile ( "GLCrowClient::CreateCrow()-GLCrowDataMan::GetCrowData() [%d/%d] 가져오기 실패.", sMobID.wMainID, sMobID.wSubID );
		return E_FAIL;
	}

	GLCROWLOGIC::INIT_DATA ();

	//	Note : 이동 제어 초기화.
	m_vPos = pCrowDrop->vPos;
	m_vDir = pCrowDrop->vDir;
	m_fScale = pCrowDrop->fScale;
	
	m_actorMove.SetMaxSpeed ( m_pCrowData->m_sAction.m_fWalkVelo );
	m_actorMove.Create ( m_pLandManClient->GetNaviMesh(), m_vPos, -1 );

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice, TRUE );
	if ( !pCharData )
	{
		if( strlen( m_pCrowData->GetSkinObjFile() ) )
		{
			CDebugSet::ToLogFile ( "GLCrowClient::CreateCrow()-DxSkinCharDataContainer::LoadData() [%s] 가져오기 실패.", m_pCrowData->GetSkinObjFile() );
		}
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pCharData, pd3dDevice );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 현정보 적용.
	m_dwGlobID = pCrowDrop->dwGlobID;
	m_dwCeID = pCrowDrop->dwCeID;

	m_dwNowHP = pCrowDrop->dwNowHP;
	m_wNowMP = pCrowDrop->wNowMP;

	if ( pCrowDrop->dwActState & EM_REQ_VISIBLENONE )	SetSTATE ( EM_REQ_VISIBLENONE );
	if ( pCrowDrop->dwActState & EM_REQ_VISIBLEOFF )	SetSTATE ( EM_REQ_VISIBLEOFF );

	switch ( pCrowDrop->emAction )
	{
	case GLAT_MOVE:
		{
			//	Note : 이동 시키기 위해 내부 전달 메시지 발생.
			//
			/*GLMSG::SNETCROW_MOVETO NetMsgGoto;
			NetMsgGoto.dwGlobID = pCrowDrop->dwGlobID;
			NetMsgGoto.dwActState = pCrowDrop->dwActState;
			NetMsgGoto.vCurPos = pCrowDrop->vPos;
			NetMsgGoto.vTarPos = pCrowDrop->TargetID.vPos;

			MsgProcess ( (NET_MSG_GENERIC*) &NetMsgGoto );*/

			// 생성시 이동중인 위치에서 서버와 클라이언트가 서로 다른
			// 루트를 통해 이동하는 문제가 생기므로 수정함.
			if ( pCrowDrop->dwActState&EM_ACT_RUN )	SetSTATE ( EM_ACT_RUN );
			else									ReSetSTATE ( EM_ACT_RUN );

			m_sTargetID.vPos = pCrowDrop->TargetID.vPos;
			TurnAction ( GLAT_MOVE );

			// 시작위치를 지정해주고
			m_actorMove.SetPosition ( pCrowDrop->vStartPos, -1 );

			// 웨이포인트를 설정하고
			BOOL bSucceed = m_actorMove.GotoLocation
			(
				m_sTargetID.vPos+D3DXVECTOR3(0,+10,0),
				m_sTargetID.vPos+D3DXVECTOR3(0,-10,0)
			);

			// 현재의 위치를 지정해주고
			m_actorMove.SetPosition ( m_vPos, -1 );
			// 다음 웨이포인트를 설정해주고
			m_actorMove.SetNextWayPoint ( pCrowDrop->sNextWaypoint );

			// 방향을 업데이트 한다.
			if ( !m_pCrowData->IsDirHold() )
			{
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
			}
		}
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_FALLING );
		break;

	case GLAT_DIE:
		TurnAction ( GLAT_DIE );
		break;
	};

	//	Note : 현재 스킬 FACT 업데이트.
	//
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SDROP_SKILLFACT &sDropFact = pCrowDrop->sSKILLFACT[i];
		if ( sDropFact.sNATIVEID==NATIVEID_NULL() )			continue;

		RECEIVE_SKILLFACT ( sDropFact.sNATIVEID, sDropFact.wLEVEL, sDropFact.wSLOT, sDropFact.fAGE );
	}
	
	//	Note : 현재 상태이상 업데이트.
	//
	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		SDROP_STATEBLOW &sDropBlow = pCrowDrop->sSTATEBLOWS[i];
		if ( sDropBlow.emBLOW==EMBLOW_NONE )				continue;

		SSTATEBLOW *pSTATEBLOW = NULL;
		if ( sDropBlow.emBLOW <= EMBLOW_SINGLE )		pSTATEBLOW = &m_sSTATEBLOWS[0];
		else											pSTATEBLOW = &m_sSTATEBLOWS[sDropBlow.emBLOW-EMBLOW_SINGLE];

		sDropBlow.emBLOW = sDropBlow.emBLOW;
		sDropBlow.fAGE = sDropBlow.fAGE;
		sDropBlow.fSTATE_VAR1 = sDropBlow.fSTATE_VAR1;
		sDropBlow.fSTATE_VAR2 = sDropBlow.fSTATE_VAR2;
	}

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS, m_matTrans, m_vDir );

	//	Note : 생성 효과 발생
	//
	if ( pCrowDrop->dwFLAGS&SDROP_CROW::CROW_GEM )
	{
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, m_vPos.x, m_vPos.y, m_vPos.z );

		std::string strGEM_EFFECT = GLCONST_CHAR::strMOB_GEM_EFFECT.c_str();
		if ( !m_pCrowData->m_sAction.m_strBirthEffect.empty() )	strGEM_EFFECT = m_pCrowData->m_sAction.m_strBirthEffect;

		STARGETID sTargetID(m_pCrowData->m_emCrow,m_dwGlobID,m_vPos);
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			strGEM_EFFECT.c_str(),
			matEffect,
			&sTargetID
		);
	}

	if ( !m_pSkinChar->GETCURANIMNODE() )
	{
		CDebugSet::ToLogFile ( "ERORR : current animation node null point error [ %s ] [ M %d S %d ]", m_pSkinChar->GetFileName(), m_pSkinChar->GETCURMTYPE(), m_pSkinChar->GETCURSTYPE() );
		m_pSkinChar->DEFAULTANI ();
 	}

	return S_OK;
}

WORD GLCrowClient::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

void GLCrowClient::SetPosition ( D3DXVECTOR3 &vPos )
{
	GASSERT(m_pCrowData);

	m_vPos = vPos;
	m_actorMove.SetPosition ( m_vPos, -1 );
	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();
}

float GLCrowClient::GetMoveVelo ()
{
	float fVELO = IsSTATE(EM_ACT_RUN) ? m_pCrowData->m_sAction.m_fRunVelo : m_pCrowData->m_sAction.m_fWalkVelo;
	return fVELO * GLCROWLOGIC::GETMOVEVELO();
}

BOOL GLCrowClient::IsValidBody ()
{
	return ( m_Action < GLAT_FALLING );
}

BOOL GLCrowClient::IsDie ()
{
	if ( IsACTION(GLAT_DIE) )		return TRUE;

	return FALSE;
}

BOOL GLCrowClient::IsHaveVisibleBody ()
{
	return ( m_pCrowData->GetSkinObjFile() && (strlen(m_pCrowData->GetSkinObjFile())!=0) );
}

BOOL GLCrowClient::IsVisibleDetect ()
{
	if ( IsSTATE(EM_REQ_VISIBLENONE) || IsSTATE(EM_REQ_VISIBLEOFF) )	return FALSE;

	BOOL bRECVISIBLE = GLGaeaClient::GetInstance().GetCharacter()->m_bRECVISIBLE;
	if ( m_bINVISIBLE && !bRECVISIBLE )									return FALSE;

	return TRUE;
}

void GLCrowClient::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : 이전 액션 취소(쓰러질때와 죽을때는 제외)
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_FALLING:
		if ( toAction==GLAT_FALLING )	return;
		break;
		
	case GLAT_DIE:
		if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )	return;
		break;

	default:
		break;
	};

	//	Note : 액션 초기화.
	//
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		StartAttackProc ();
		break;

	case GLAT_SKILL:
		StartSkillProc ();
		break;

	case GLAT_FALLING:
		FACTEFF::DeleteEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );
		break;

	case GLAT_DIE:
		{
			FACTEFF::DeleteEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

			D3DXVECTOR3		vNORMAL;
			D3DXVECTOR3		vSTRIKE;

			bool bBLOOD_TRACE(false);
			for ( int i=0; i<PIECE_REV01; ++i )
			{
				PDXCHARPART pCharPart = m_pSkinChar->GetPiece(i);
				if ( !pCharPart )	continue;

				if ( pCharPart->GetTracePos(STRACE_BLOOD01) )
				{
					bBLOOD_TRACE = true;
					pCharPart->CalculateVertexInflu ( STRACE_BLOOD01, vSTRIKE, vNORMAL );
				}
			}

			if ( !bBLOOD_TRACE )		vSTRIKE = m_vPos;

			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, vSTRIKE.x, vSTRIKE.y, vSTRIKE.z );

			STARGETID sTargetID(m_pCrowData->m_emCrow,m_dwGlobID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strMOB_DIE_BLOOD.c_str(),
				matEffect,
				&sTargetID
			);

			DxEffcharDataMan::GetInstance().PutEffect
			(
				m_pSkinChar,
				GLCONST_CHAR::strERASE_EFFECT.c_str(), &m_vDir
			);
		}
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};

	if ( m_actorMove.PathIsActive() )
	{
		if ( !IsACTION(GLAT_MOVE) && !IsACTION(GLAT_PUSHPULL) )		m_actorMove.Stop();
	}
}

void GLCrowClient::StartAttackProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;
}

void GLCrowClient::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = GLGaeaClient::GetInstance().ValidCheckTarget ( m_sTargetID );
	if ( !bOk )	return;

	GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	타겟의 위치.
	D3DXVECTOR3 vTARPOS = GLGaeaClient::GetInstance().GetTargetPos ( m_sTargetID );

	D3DXVECTOR3 vDir = vTARPOS - m_vPos;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - m_vPos;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = m_vPos + vTarDir*fLength * 10.0f;

	//	타격 위치를 알아냄.
	STRIKE::SSTRIKE sStrike;
	STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
	bOk = Selector.SELECT ( sStrikeEff, sStrike );
	if ( !bOk )	return;

	//EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
	//if ( emSlot==SLOT_TSIZE )	return;

	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
	STARGETID sTargetID(m_sTargetID.emCrow,m_sTargetID.dwID,vTARPOS);

	EMELEMENT emELEMENT = STATE_TO_ELEMENT ( m_pAttackProp->emBLOW_TYPE );

	if ( m_pAttackProp->emAttRgType == EMATT_LONG )
	{
		if ( !m_pAttackProp->strTargetEffect.empty() )
		{
			DxEffSingleGroup* pEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup
			(
			m_pAttackProp->strTargetEffect.c_str(),
				matEffect,
				&sTargetID
			);
			
			if ( pEffSingleG )
			{
				pEffSingleG->AddEffAfter ( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT)  );

				if ( !m_pAttackProp->strTargBodyEffect.empty() )
					pEffSingleG->AddEffAfter ( EFFABODY, std::string(m_pAttackProp->strTargBodyEffect.c_str()) );

				pEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
			}
		}
	}
	else
	{
		DxEffSingleGroup* pEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
			matEffect,
			&sTargetID
		);

		DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
	}
}

BOOL GLCrowClient::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO();
	GASSERT(m_pSkinChar->GETCURANIM());
	if ( !m_pSkinChar->GETCURANIM() )	return FALSE;

	PSANIMCONTAINER pAnimCont = m_pSkinChar->GETCURANIM();

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( GLGaeaClient::GetInstance().IsVisibleCV(m_sTargetID) )
			{
				//	Note : 공격시 이팩트 발휘..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}

	return TRUE;
}

void GLCrowClient::StartSkillProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

	//	Note : 목표물에 따라 방향을 미리 회전한다.
	//
	m_vDir = UpdateSkillDirection ( m_vPos, m_vDir, m_idACTIVESKILL, m_vTARPOS, m_sTARIDS );

	SCROWATTACK &sATTACK = m_pCrowData->m_sCrowAttack[m_dwAType];
	ANIMCONTNODE* pAniContNode = m_pSkinChar->findanicontnode ( sATTACK.strAniFile.c_str() );
	if ( !pAniContNode )					return;
	
	EMPIECECHAR emStrikePiece = pAniContNode->pAnimCont->m_sStrikeEff[0].m_emPiece;
	STARGETID sTARG(GETCROW(),m_dwGlobID);

	EMELEMENT emELMT(EMELEMENT_SPIRIT);

	//	Note : 스킬 시작과 동시에 나타나는 이팩트. 발동.
	//
	if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_vDir );

	SANIMSTRIKE sStrike;
	sStrike.m_emPiece = emStrikePiece;
	sStrike.m_emEffect = EMSF_TARGET;
	sStrike.m_dwFrame = 0;

	if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			STARGETID sTARGET;
			sTARGET.emCrow = m_sTARIDS[i].GETCROW();
			sTARGET.dwID = m_sTARIDS[i].GETID();

			if ( !GLGaeaClient::GetInstance().IsVisibleCV(sTARGET) )	continue;
			sTARGET.vPos = GLGaeaClient::GetInstance().GetTargetPos(sTARGET);

			if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
		}
	}

}

void GLCrowClient::SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag )
{
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKEFF.sNATIVEID.wMainID, sSKEFF.sNATIVEID.wSubID );
		if ( !pSkill )								continue;

		if ( !pSkill->m_sEXT_DATA.strHOLDOUT.empty() )
		{

			if ( (pSkill->m_sAPPLY.emSPEC == EMSPECA_PSY_DAMAGE_REDUCE && 
				!( dwDamageFlag & DAMAGE_TYPE_PSY_REDUCE) ) )
			continue;
		
			if ( (pSkill->m_sAPPLY.emSPEC == EMSPECA_MAGIC_DAMAGE_REDUCE && 
				!( dwDamageFlag & DAMAGE_TYPE_MAGIC_REDUCE) ) )
				continue;

			if ( (pSkill->m_sAPPLY.emSPEC == EMSPECA_PSY_DAMAGE_REFLECTION && 
				!( dwDamageFlag & DAMAGE_TYPE_PSY_REFLECTION) ) )
				continue;

			if ( (pSkill->m_sAPPLY.emSPEC == EMSPECA_MAGIC_DAMAGE_REFLECTION && 
				!( dwDamageFlag & DAMAGE_TYPE_MAGIC_REFLECTION) ) )
				continue;


			const char* szZONE_EFF = pSkill->m_sEXT_DATA.strHOLDOUT.c_str();

			D3DXVECTOR3 vDIR = sTarget.vPos - m_vPos;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = m_vPos.x;
			matTrans._42 = m_vPos.y;
			matTrans._43 = m_vPos.z;

			//	Note : 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget );
		}
	}
}

void GLCrowClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS = m_vPos + m_vDir * 60.0f;

		//	타격 위치를 알아냄.
		STRIKE::SSTRIKE sStrike;
		STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrikeEff, sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	지면기준 목표점에 회전 속성 넣어서.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	Note : 자기 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget );
}

void GLCrowClient::SK_EFF_TARZONE ( const STARGETID &_sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = GLGaeaClient::GetInstance().GetSkinChar ( sTarget );

		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS ( m_vPos.x, m_vPos.y+15, m_vPos.z );;

		//	타격 위치를 알아냄.
		IMPACT_ARROW::SSTRIKE sStrike;
		IMPACT_ARROW::CSELECTOR Selector ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
			sTarget.vPos = vTARPOS;
		}
	}

	//	지면기준 목표점.
	if ( !bTRANS )
	{
		bTRANS = TRUE;
		D3DXMatrixTranslation ( &matTrans, sTarget.vPos.x, sTarget.vPos.y, sTarget.vPos.z );
	}

	//	Note : 목표 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget );
}

void GLCrowClient::SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &_sTarget )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSA == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vSTRPOS ( sTarget.vPos.x, sTarget.vPos.y+15, sTarget.vPos.z );;

		D3DXVECTOR3 vDir = vSTRPOS - m_vPos;
		float fLength = D3DXVec3Length ( &vDir );

		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vSTRPAR = m_vPos + vDir*fLength * 10.0f;

		//	스트라이크 위치를 알아냄.
		STRIKE::SSTRIKE sStrike;
		STRIKE::CSELECTOR Selector ( vSTRPAR, m_pSkinChar->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrikeEff, sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;
			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}
	
	//	지면기준 회전 속성 넣어서.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	목표 지점 선택
	BOOL bIMPACT(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSB == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = GLGaeaClient::GetInstance().GetSkinChar ( sTarget );
		if ( !pSkin )	return;

		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS ( m_vPos.x, m_vPos.y+15, m_vPos.z );;

		//	타격 위치를 알아냄.
		IMPACT_ARROW::SSTRIKE sImpact;
		IMPACT_ARROW::CSELECTOR Selmpact ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selmpact.SELECT ( sImpact );
		if ( bOk )
		{
			bIMPACT = TRUE;
			sTarget.vPos = sImpact.vPos;
		}
	}

	if ( !bIMPACT )
	{
		sTarget.vPos;
	}

	EMELEMENT emELMT(EMELEMENT_SPIRIT);
	DxEffSingleGroup* pEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARG(emELMT), matTrans, &sTarget );
	if ( !pEffSingleG )		return;

	//	Note : 목표 위치 이펙트.
	if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
		pEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

	if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
		pEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

	//	Note : 목표 몸 이펙트.
	if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
		pEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

	if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
		pEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
}

void GLCrowClient::SkillEffect ( const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	//	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
	//
	EMELEMENT emELMT(EMELEMENT_SPIRIT);
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			D3DXMatrixTranslation ( &matTrans, m_vTARPOS.x, m_vTARPOS.y, m_vTARPOS.z );
			DxEffSingleGroup* pEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans );
			if ( pEffSingleG )
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					pEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					pEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans );
	}
	//	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
	//		자기 위치에서 대상 위치까지. ( 화살 관통 )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( m_wTARNUM == 0 )	return;

		STARGETID sTarget(static_cast<EMCROW>(m_sTARIDS[m_wTARNUM-1].wCrow),static_cast<DWORD>(m_sTARIDS[m_wTARNUM-1].wID));
		if ( !GLGaeaClient::GetInstance().IsVisibleCV(sTarget) )		return;
		sTarget.vPos = GLGaeaClient::GetInstance().GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - m_vPos;
		vDir.y = 0.f;
		D3DXVec3Normalize ( &vDir, &vDir );

		//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
		//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
		if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
		{
			SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
		}
		//	단독으로 목표 지점에 발생.
		else
		{
			//	Note : 목표 위치 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			//	Note : 목표 몸 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
			if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		}
	}
	//	Note : 타겟 이펙트가 유닛을 향해 발사되는 경우.
	//
	else
	{
		//	Note : 목표 유닛이 있는 경우.
		//
		if ( m_wTARNUM )
		{
			for ( WORD i=0; i<m_wTARNUM; ++i )
			{
				STARGETID sTarget(static_cast<EMCROW>(m_sTARIDS[i].wCrow),static_cast<DWORD>(m_sTARIDS[i].wID));
				if ( !GLGaeaClient::GetInstance().IsVisibleCV(sTarget) )	continue;
				sTarget.vPos = GLGaeaClient::GetInstance().GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - m_vPos;
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
				//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
				if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
				{
					SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
				}
				//	단독으로 목표 지점에 발생.
				else
				{
					//	Note : 목표 위치 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

					if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

					//	Note : 목표 몸 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
					if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				}

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )	break;
			}
		}
	}


	STARGETID sTARG(GETCROW(),m_dwGlobID);

	if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_vDir );
}

BOOL GLCrowClient::SkillProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO();
	GASSERT(m_pSkinChar->GETCURANIMNODE());

	PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
	GASSERT(pAnicontNode);
	PSANIMCONTAINER pAnimCont = pAnicontNode->pAnimCont;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = pAnimCont->m_dwSTime + DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		if ( dwThisKey>=dwStrikeKey )
		{
			//	Note : 스킬 이펙트 발동.
			//
			SkillEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
		
			m_nattSTEP++;
		}
	}

	return TRUE;
}

HRESULT GLCrowClient::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bLowSP(false);

	BOOL bFreeze = FALSE;
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			BOOL bPANT(FALSE); // 헐떡거림.
			EMANI_MAINTYPE emMType;
			EMANI_SUBTYPE emSType;

			if ( m_pSkinChar->GETANI ( AN_GUARD_L, AN_SUB_NONE ) )
			{
				bPANT = bLowSP || m_bSTATE_PANT;
				for ( int i=0; i<EMBLOW_MULTI; ++i )
				{
					if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_STUN || m_sSTATEBLOWS[i].emBLOW == EMBLOW_MAD )		bPANT = TRUE;
				}
			}

			if ( bPANT )
			{
				emMType = AN_GUARD_L;
				emSType = AN_SUB_NONE;
			}
			else
			{
				emMType = AN_GUARD_N;
				emSType = AN_SUB_NONE;
			}

			m_pSkinChar->SELECTANI ( emMType, emSType );
		}
		break;

	case GLAT_TALK:
		m_pSkinChar->SELECTANI ( AN_TALK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsSTATE(EM_ACT_RUN) )		m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else							m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		m_pSkinChar->SELECTANI ( m_pAttackProp->strAniFile.c_str() );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SKILL:
		m_pSkinChar->SELECTANI ( m_pAttackProp->strAniFile.c_str() );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
		m_pSkinChar->SELECTANI ( AN_SHOCK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		m_pSkinChar->SELECTANI ( AN_SHOCK, AN_SUB_NONE );
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( m_Action )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLCROWLOGIC::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GETATTVELO();
		break;
	};

	m_pSkinChar->SetPosition ( m_vPos );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, TRUE, bFreeze );

	return S_OK;
}

BOOL GLCrowClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

HRESULT GLCrowClient::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;
	GASSERT(m_pCrowData);
	GASSERT(m_pLandManClient);
	m_pAttackProp = &(m_pCrowData->m_sCrowAttack[m_dwAType]);

	//	Note : 케릭 현제 위치 업대이트.
	//
	m_vPos = m_actorMove.Position ();

	//	Note : 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( !pTarget )
	{
		m_sTargetID.dwID = EMTARGET_NULL;
		if ( IsACTION(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );
	}

	if ( IsValidBody() )
	{
		GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime, TRUE );
	}

	//	Note : 에니메이션, 스킨 업데이트.
	//
	UpateAnimation( fTime, fElapsedTime );

	switch ( m_Action )
	{
	case GLAT_MOVE:
		{
			//	Note : 케릭의 이동 업데이트.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo () );
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE );
			}

			//	Note : 케릭의 현제 위치 업데이트.
			//
			m_vPos = m_actorMove.Position();

			//	Note : Mob의 현재 방향 업데이트.
			//
			if ( !m_pCrowData->IsDirHold() )
			{
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
			}
		}
		break;

	case GLAT_ATTACK:
		{
			AttackProc ( fElapsedTime );

			//	Note : 공격 방향으로 회전.
			//
			GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - m_vPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			SkillProc ( fElapsedTime );

			//	Note : 공격 방향으로 회전.
			//
			D3DXVECTOR3 vDirection = UpdateSkillDirection ( m_vPos, m_vDir, m_idACTIVESKILL, m_vTARPOS, m_sTARIDS );
		}
		break;

	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE );
			}
		}
		break;
	};

	//	Note : 스킬 이펙트 업데이트.
	//
	FACTEFF::UpdateSkillEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

	//	Note : Mob의 현재 위치 업데이트.
	//
	m_vPos = m_actorMove.Position();

	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	//	Note : AABB 계산.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	return S_OK;
}

float GLCrowClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
}

HRESULT GLCrowClient::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )			return S_FALSE;
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;

	//	Note : m_bINVISIBLE 가 참일 경우 반투명하게 표현해야함.
	//
	if ( m_bINVISIBLE )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
	}
	else
	{
		DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
	}


	if( m_fScale > 0.0f )
	{
		D3DXMATRIX matScale;
		D3DXMatrixIdentity( &matScale );


		D3DXMatrixScaling( 
			&matScale, 
			m_fScale, 
			m_fScale, 
			m_fScale );

		m_matTrans = matScale * m_matTrans;
	}

	hr = m_pSkinChar->Render ( pd3dDevice, m_matTrans );
	if ( FAILED(hr) )	return hr;

	#ifdef _SYNC_TEST
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vServerPos, 2.0f );
	}
	#endif

	return S_OK;
}

HRESULT GLCrowClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )			return S_FALSE;
	if ( IsACTION(GLAT_DIE) )			return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	
	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLCrowClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	return S_OK;
}

HRESULT GLCrowClient::InvalidateDeviceObjects ()
{
	m_pSkinChar->InvalidateDeviceObjects ();
	return S_OK;
}

void GLCrowClient::ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR )
{
	if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	TurnAction ( GLAT_SHOCK );
	
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		CInnerInterface::GetInstance().SetDamage( vPos, wDamage, dwDamageFlag, UI_ATTACK );
		
		if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
		{
			// 강한타격 이펙트
			D3DXVECTOR3 vDIR = sACTOR.vPos - m_vPos;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = m_vPos.x;
			matTrans._42 = m_vPos.y + 10.0f;
			matTrans._43 = m_vPos.z;

			//	Note : 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR );
		}
	}

	RECEIVE_DAMAGE(wDamage);

	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}

void GLCrowClient::ReceiveAVoid ()
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		CInnerInterface::GetInstance().SetDamage( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK );
	}
}

void GLCrowClient::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
}

void GLCrowClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_CROW_MOVETO:
		{
			GLMSG::SNETCROW_MOVETO *pNetMsg = (GLMSG::SNETCROW_MOVETO *) nmg;
			
			if ( pNetMsg->dwActState&EM_ACT_RUN )	SetSTATE ( EM_ACT_RUN );
			else									ReSetSTATE ( EM_ACT_RUN );

			m_sTargetID.vPos = pNetMsg->vTarPos;
			TurnAction ( GLAT_MOVE );

			// 서버/클라이언트간의 셀이 서로 다르면 정정해주어야 정확한 웨이포인트로
			// 이동하게 된다. 하지만 추격중이면 지정해주지 않는다.
			if ( m_actorMove.CurrentCellID() != pNetMsg->dwCellID && !pNetMsg->bTrace )
			{
				m_actorMove.SetPosition ( pNetMsg->vCurPos, -1 );
			}

			BOOL bSucceed = m_actorMove.GotoLocation
			(
				m_sTargetID.vPos+D3DXVECTOR3(0,+10,0),
				m_sTargetID.vPos+D3DXVECTOR3(0,-10,0)
			);

			//CDebugSet::ToListView ( "NET_MSG_GCTRL_CROW_MOVETO" );
		}
		break;

	case NET_MSG_GCTRL_CROW_ATTACK:
		{
			GLMSG::SNETCROW_ATTACK *pNetMsg = (GLMSG::SNETCROW_ATTACK *) nmg;

			m_sTargetID.emCrow = pNetMsg->emTarCrow;
			m_sTargetID.dwID = pNetMsg->dwTarID;
			m_dwAType = pNetMsg->dwAType;
			TurnAction ( GLAT_ATTACK );

			//CDebugSet::ToListView ( "NET_MSG_GCTRL_CROW_ATTACK" );
		}
		break;

	case NET_MSG_GCTRL_CROW_SKILL:
		{
			GLMSG::SNETCROW_SKILL *pNetMsg = (GLMSG::SNETCROW_SKILL *)nmg;

			m_dwAType = pNetMsg->dwAType;
			m_idACTIVESKILL = pNetMsg->skill_id;
			m_wACTIVESKILL_LVL = pNetMsg->skill_lev;
			m_vTARPOS = pNetMsg->vTARPOS;

			m_wTARNUM = pNetMsg->wTARNUM;
			if( m_wTARNUM > EMTARGET_NET )	
			{
				break;
			}
			else
			{
				memcpy ( m_sTARIDS, pNetMsg->sTARIDS, sizeof(STARID)*m_wTARNUM );
				TurnAction ( GLAT_SKILL );
			}
		}
		break;

	case NET_MSG_GCTRL_CROW_AVOID:
		{
			GLMSG::SNETCROW_AVOID *pNetMsg = (GLMSG::SNETCROW_AVOID *) nmg;
			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )	pActor->ReceiveAVoid ();
		}
		break;

	case NET_MSG_GCTRL_CROW_DAMAGE:
		{
			GLMSG::SNETCROW_DAMAGE *pNetMsg = (GLMSG::SNETCROW_DAMAGE *) nmg;

			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )
			{
				STARGETID sACTOR(GetCrow(),GetCtrlID(),GetPosition());
				pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->dwDamageFlag, sACTOR );
			}
		}
		break;

	case NET_MSG_GCTRL_ACTION_BRD:
		{
			GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)nmg;
			TurnAction ( pNetMsg->emAction );
		}
		break;

	case NET_MSG_GCTRL_SKILLFACT_BRD:
		{
			GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)nmg;

			GLOGICEX::VARIATION ( m_dwNowHP, m_pCrowData->m_dwHP, pNetMsg->nVAR_HP );
			GLOGICEX::VARIATION ( m_wNowMP, m_pCrowData->m_wMP, pNetMsg->nVAR_MP );
			GLOGICEX::VARIATION ( m_wNowSP, m_pCrowData->m_wSP, pNetMsg->nVAR_SP );

			//	데미지 메시지.
			if ( pNetMsg->nVAR_HP < 0 )
			{
				if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_SHOCK )	TurnAction ( GLAT_SHOCK );
				else					ReceiveSwing ();

				D3DXVECTOR3 vPos = GetPosBodyHeight();
				CInnerInterface::GetInstance().SetDamage( vPos, static_cast<WORD>(-pNetMsg->nVAR_HP), pNetMsg->dwDamageFlag, UI_ATTACK );

				//	Note : 방어스킬의 이펙트가 있을때 발동시킴.
				STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
				sACTOR.vPos = GLGaeaClient::GetInstance().GetTargetPos ( sACTOR );

				SKT_EFF_HOLDOUT ( sACTOR, pNetMsg->dwDamageFlag );

				if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
				{
					// 강한타격 이펙트

					D3DXVECTOR3 vDIR = sACTOR.vPos - m_vPos;

					D3DXVECTOR3 vDIR_ORG(1,0,0);
					float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

					D3DXMATRIX matTrans;
					D3DXMatrixRotationY ( &matTrans, fdir_y );
					matTrans._41 = m_vPos.x;
					matTrans._42 = m_vPos.y + 10.0f;
					matTrans._43 = m_vPos.z;

					//	Note : 자기 위치 이펙트 발생시킴.
					DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR );
				}


			}

			//	힐링 메시지.
			//if ( pNetMsg->nVAR_HP > 0 )
			//{
			//	CPlayInterface::GetInstance().InsertText ( GetPosition(), static_cast<WORD>(pNetMsg->nVAR_HP), pNetMsg->bCRITICAL, 1 );
			//}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)nmg;

			//	지속형 스킬의 경우 스킬 팩터 추가됨.
			if ( pNetMsg->skill_id != SNATIVEID(false) )
			{
				RECEIVE_SKILLFACT ( pNetMsg->skill_id, pNetMsg->wLEVEL, pNetMsg->wSELSLOT );
				FACTEFF::NewSkillFactEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, pNetMsg->skill_id, m_matTrans, m_vDir );
			}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_RS_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD *)nmg;

			//	Note : 스킬 fact 들을 종료.
			//		바로 리샛 하지 않고 여기서 시간 조종하여 정상 종료되게 함. ( 이팩트 종료 때문. )
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				if ( pNetMsg->bRESET[i] )
				{
					FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

					DISABLESKEFF(i);
				}
			}
		}
		break;

	case NET_MSG_GCTRL_STATEBLOW_BRD:
		{
			GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)nmg;

			//	Note : "단독상태이상"일 경우 단독 효과 모두 끄기.
			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
				FACTEFF::DeleteBlowSingleEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, m_sSTATEBLOWS );

			//	Note : 상태 이상 추가.
			SSTATEBLOW *pSTATEBLOW = NULL;
			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )		pSTATEBLOW = &m_sSTATEBLOWS[0];
			else										pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];
			pSTATEBLOW->emBLOW = pNetMsg->emBLOW;
			pSTATEBLOW->fAGE = pNetMsg->fAGE;
			pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
			pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;

			//	Note : 효과 생성.
			FACTEFF::NewBlowEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, pSTATEBLOW->emBLOW, m_matTrans, m_vDir );
		}
		break;

	case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
		{
			GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)nmg;

			for ( int i=0; i<EMBLOW_MULTI; ++i )
			{
				EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;
				if ( emBLOW==EMBLOW_NONE )		continue;

				EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
				if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

				DISABLEBLOW(i);
				FACTEFF::DeleteBlowEffect ( STARGETID(CROW_MOB,m_dwGlobID,m_vPos), m_pSkinChar, emBLOW );
			}
		}
		break;

	case NET_MSG_GCTRL_PUSHPULL_BRD:
		{
			GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)nmg;
			const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

			//	Note : 밀려날 위치로 이동 시도.
			//
			BOOL bSucceed = m_actorMove.GotoLocation
			(
				D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
				D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
			);

			if ( bSucceed )
			{
				//	Note : 밀려나는 엑션 시작.
				//
				m_sTargetID.vPos = vMovePos;
				TurnAction ( GLAT_PUSHPULL );

				//	Note : 밀리는 속도 설정.
				//
				m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			}
		}
		break;

	case NET_MSG_GCTRL_POSITIONCHK_BRD:
		{
			GLMSG::SNET_POSITIONCHK_BRD *pNetNsg = (GLMSG::SNET_POSITIONCHK_BRD *)nmg;
			m_vServerPos = pNetNsg->vPOS;
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCrowClient::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d/%d]", nmg->nType, nmg->nType-NET_MSG_GCTRL );
		break;
	};
}

BOOL GLCrowClient::RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge )
{
	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )				return FALSE;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];

	BOOL bHOLD = FALSE;
	SSKILLFACT sSKILLEF;
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
		bHOLD = TRUE;
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	if ( pSkill->m_sAPPLY.emADDON != EMIMPACTA_NONE )
	{
		bHOLD = TRUE;
		sSKILLEF.emADDON = pSkill->m_sAPPLY.emADDON;
		sSKILLEF.fADDON_VAR = pSkill->m_sAPPLY.fADDON_VAR[wlevel];
	}

	switch ( pSkill->m_sAPPLY.emSPEC )
	{
	case EMSPECA_REFDAMAGE:
	case EMSPECA_NONBLOW:
	case EMSPECA_PIERCE:
	case EMSPECA_TARRANGE:
	case EMSPECA_MOVEVELO:
	case EMSPECA_CRUSHING_BLOW:
	case EMSPECA_PSY_DAMAGE_REDUCE:
	case EMSPECA_MAGIC_DAMAGE_REDUCE:
	case EMSPECA_PSY_DAMAGE_REFLECTION:
	case EMSPECA_MAGIC_DAMAGE_REFLECTION:
	case EMSPECA_BUFF_REMOVE:
	case EMSPECA_DEBUFF_REMOVE:
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		bHOLD = TRUE;
		sSKILLEF.emSPEC = pSkill->m_sAPPLY.emSPEC;
		sSKILLEF.fSPECVAR1 = pSkill->m_sAPPLY.sSPEC[wlevel].fVAR1;
		sSKILLEF.fSPECVAR2 = pSkill->m_sAPPLY.sSPEC[wlevel].fVAR2;
		sSKILLEF.dwSPECFLAG = pSkill->m_sAPPLY.sSPEC[wlevel].dwFLAG;
		sSKILLEF.dwNativeID = pSkill->m_sAPPLY.sSPEC[wlevel].dwNativeID;
		break;
	};

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//
	if ( bHOLD )
	{
		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.fAGE		= sSKILL_DATA.fLIFE;

		if ( dwSELECT < SKILLFACT_SIZE )
		{
			m_sSKILLFACT[dwSELECT] = sSKILLEF;
		}

		return TRUE;
	}

	GASSERT ( 0 && "지속 스킬정보가 유효하지 않음." );
	return FALSE;
}


