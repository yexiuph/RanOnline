#include "pch.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "DxShadowMap.h"
#include "GLItemMan.h"
#include "GLCharDefine.h"
#include "GLFactEffect.h"
#include "GLStrikeM.h"
#include "DxEffGroupPlayer.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/DamageDisplay.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextcontrol.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextcontrol.h"

#include "../[Lib]__Engine/Sources/DxEffect/DxEffCharData.h"


#include ".\glanysummon.h"

//#include "GLSkill.h"

GLAnySummon::GLAnySummon(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),
	m_dwActionFlag(EM_SUMMONACT_STOP),

	m_vServerPos(D3DXVECTOR3(0,0,0)),

	m_Action(GLAT_IDLE),

	m_vTarPos(D3DXVECTOR3(0,0,0)),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_wAniSub(1)
{
	m_sTargetID.RESET();
}

GLAnySummon::~GLAnySummon(void)
{
	DeleteDeviceObjects ();
}

void GLAnySummon::ASSIGN ( PSDROPSUMMON pDropSummon )
{
	m_emTYPE			= pDropSummon->m_emTYPE;

	m_dwGUID			= pDropSummon->m_dwGUID;
	m_sSummonID			= pDropSummon->m_sSummonID;
	m_dwOwner			= pDropSummon->m_dwOwner;	
	m_vPos				= pDropSummon->m_vPos;
	m_vDir				= pDropSummon->m_vDir;
	m_vTarPos			= pDropSummon->m_vTarPos;

	m_sMapID			= pDropSummon->m_sMapID;
	m_dwCellID			= pDropSummon->m_dwCellID;
	
	m_dwActionFlag		= pDropSummon->m_dwActionFlag;
		
	m_wAniSub			= pDropSummon->m_wAniSub;

	m_dwSummonID		= pDropSummon->m_dwSummonID;

	m_dwOwner			= pDropSummon->m_dwOwner;

	m_dwNowHP			= pDropSummon->m_dwNowHP;
	m_wNowMP			= pDropSummon->m_wNowMP;	
}

HRESULT GLAnySummon::Create ( PSDROPSUMMON pDropSummon, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �⺻ ������ ����
	ASSIGN ( pDropSummon );

	m_pd3dDevice = pd3dDevice;

	// �ʱ� ��ġ�� ���� ����
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;


	PGLCHARCLIENT pOwnerClient = GLGaeaClient::GetInstance().GetChar( m_dwOwner );
	if( pOwnerClient ) pOwnerClient->m_dwSummonGUID = m_dwGUID;


	m_pSummonCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	if( !m_pSummonCrowData )
	{
		CDebugSet::ToLogFile ( _T("GLCrow::CreateMob, Mob Create Failed. nativeid [%d/%d]"), m_sSummonID.wMainID, m_sSummonID.wSubID );
		return E_FAIL;
	}

	if ( pNavi )
	{
		m_actorMove.Create ( pNavi, m_vPos, -1 );
	}

	m_Action = GLAT_IDLE;

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;


	// Note : 1.AABB Box�� �����´�. 2.���̸� ����� ���´�.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// ���� �����̰� �ִٸ�... ���θ޽��� �߻�
	if ( IsSTATE ( EM_SUMMONACT_MOVE ) || IsSTATE ( EM_SUMMONACT_TRACING ) )
	{
		GLMSG::SNET_SUMMON_GOTO_BRD NetMsgBrd;
		NetMsgBrd.dwGUID =  m_dwGUID;
		NetMsgBrd.vCurPos = m_vPos;
		NetMsgBrd.vTarPos = m_vTarPos;
		NetMsgBrd.dwFlag  = m_dwActionFlag;

		MsgGoto ( ( NET_MSG_GENERIC* ) &NetMsgBrd );
	}	

	return S_OK;
}

float GLAnySummon::GetDirection ()
{
	return DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
}


void GLAnySummon::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : ���� �׼� ���(���������� �������� ����)
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

	//	Note : �׼� �ʱ�ȭ.
	//
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		StartAttackProc ();
		break;

		/*case GLAT_SKILL:
		StartSkillProc ();
		break;*/

	case GLAT_FALLING:
		FACTEFF::DeleteEffect ( STARGETID(CROW_MOB,m_dwGUID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );
		break;

	case GLAT_DIE:
		{
			FACTEFF::DeleteEffect ( STARGETID(CROW_MOB,m_dwGUID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

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

			STARGETID sTargetID(m_pSummonCrowData->m_emCrow,m_dwGUID,m_vPos);
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
		GASSERT("GLCharacter::TurnAction() �غ���� ���� ACTION �� ���Խ��ϴ�.");
		break;
	};

	if ( m_actorMove.PathIsActive() )
	{
		if ( !IsACTION(GLAT_MOVE) && !IsACTION(GLAT_PUSHPULL) )		m_actorMove.Stop();
	}
}


void GLAnySummon::SetSTATE ( DWORD dwStateFlag )					
{
	m_dwActionFlag |= dwStateFlag; 
}

HRESULT GLAnySummon::UpdateAnimation ( float fTime, float fElapsedTime )
{

	HRESULT hr=S_OK;

	BOOL bLowSP(false);

	BOOL bFreeze = FALSE;
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			BOOL bPANT(FALSE); // �涱�Ÿ�.
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

			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
		break;

	case GLAT_TALK:
		m_pSkinChar->SELECTANI ( AN_TALK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsSTATE(EM_SUMMONACT_RUN) )		m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else									m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
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
		//		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		m_pSkinChar->TOENDTIME();
		//DeleteDeviceObjects();
		return S_FALSE;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		break;
	};

	//	Note : Mob ��Ų ������Ʈ.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( m_Action )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLSUMMON::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GLSUMMON::GETATTVELO();
		break;
	};


	m_pSkinChar->SetPosition ( m_vPos );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, TRUE, bFreeze );


	return S_OK;
}

HRESULT GLAnySummon::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice, 
		TRUE );

	if ( !pSkinChar )	return E_FAIL;

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice );
	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, (EMANI_SUBTYPE)0 );

	m_pSkinChar->SetHairColor( 0 );

	return S_OK;
}



HRESULT	GLAnySummon::UpdateTurnAction ( float fTime, float fElapsedTime )
{

	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;
	case GLAT_MOVE:
		{
			if ( !m_actorMove.PathIsActive() )
			{
				//			ReSetSTATE ( EM_SUMMONACT_MOVE );
				SetSTATE ( EM_SUMMONACT_STOP );
				TurnAction ( GLAT_IDLE );
				m_actorMove.Stop ();
				//			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			}

			// ���� ������Ʈ
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

		break;
	case GLAT_ATTACK:
		{
			AttackProc ( fElapsedTime );

			//	Note : ���� �������� ȸ��.
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
	case GLAT_PUSHPULL:
		m_actorMove.Update ( fElapsedTime );
		if ( !m_actorMove.PathIsActive() )
		{
			m_actorMove.Stop ();
			TurnAction ( GLAT_IDLE );
		}
		break;
	}

//	if(  m_Action == GLAT_PUSHPULL )
//	{
//
//	}
//
//	if ( IsSTATE ( EM_SUMMONACT_STOP ) )
//	{
//		// �̵� �޽����� ��ٸ���.
//	}
//
//	if ( IsSTATE ( EM_SUMMONACT_MOVE ) || IsSTATE ( EM_SUMMONACT_TRACING ) )
//	{
//		if ( !m_actorMove.PathIsActive() )
//		{
////			ReSetSTATE ( EM_SUMMONACT_MOVE );
//			SetSTATE ( EM_SUMMONACT_STOP );
//			TurnAction ( GLAT_IDLE );
//			m_actorMove.Stop ();
////			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
//		}
//	}
//
//	if ( IsSTATE ( EM_SUMMONACT_WAIT ) )
//	{
//		// ���� �޽����� ��ٸ���.
//	}
//
//	// ���� ���ݸ�� ���̸�
//	if ( IsSTATE ( EM_SUMMONACT_ATTACK ) )
//	{
//		AttackProc ( fElapsedTime );
//
//		//	Note : ���� �������� ȸ��.
//		//
//		GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
//		if ( pTarget )
//		{
//			D3DXVECTOR3 vDirection = pTarget->GetPosition() - m_vPos;
//			D3DXVec3Normalize ( &vDirection, &vDirection );
//			m_vDir = vDirection;
//		}	
//	}
	
	return S_OK;
}

HRESULT GLAnySummon::FrameMove( float fTime, float fElapsedTime )
{
	if ( !m_pSummonCrowData ) return S_OK;

	HRESULT hr(S_OK);

	m_pAttackProp = &(m_pSummonCrowData->m_sCrowAttack[m_dwAType]);

	if ( IsValidBody () )
	{
		if ( m_dwNowHP!=0 )
		{
			// Summon �� ���� ���°����� Update �Ѵ�
			UPDATE_DATA ( fTime, fElapsedTime );
		}else{
			//	���� ��ȭ.
			TurnAction ( GLAT_FALLING );
		}
	}

	UpdateTurnAction ( fTime, fElapsedTime );

	UpdateAnimation( fTime, fElapsedTime );
	
	// ============== Update about Actor (Begin) ============== /

	hr = m_actorMove.Update ( fElapsedTime );
	if ( FAILED(hr) )	return E_FAIL;


	// ���� ��ġ ������Ʈ
	m_vPos = m_actorMove.Position();	

	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;


	//	Note : AABB ���.
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	return S_OK;
}

HRESULT GLAnySummon::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar ) m_pSkinChar->Render( pd3dDevice, m_matTrans );

#ifdef _SYNC_TEST
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vServerPos, 2.0f );
	}
#endif

	return S_OK;
}

HRESULT GLAnySummon::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;

	//	Note : �׸��� ������.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}


void GLAnySummon::StartAttackProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;
}

void GLAnySummon::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = GLGaeaClient::GetInstance().ValidCheckTarget ( m_sTargetID );
	if ( !bOk )	return;

	GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	Ÿ���� ��ġ.
	D3DXVECTOR3 vTARPOS = GLGaeaClient::GetInstance().GetTargetPos ( m_sTargetID );

	D3DXVECTOR3 vDir = vTARPOS - m_vPos;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - m_vPos;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = m_vPos + vTarDir*fLength * 10.0f;

	//	Ÿ�� ��ġ�� �˾Ƴ�.
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

BOOL GLAnySummon::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime;/* * GETATTVELO();*/
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
				//	Note : ���ݽ� ����Ʈ ����..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}

	return TRUE;
}



HRESULT GLAnySummon::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}


HRESULT GLAnySummon::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLAnySummon::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLAnySummon::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLAnySummon::DeleteDeviceObjects ()
{
	SAFE_DELETE(m_pSkinChar);
	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_sTargetID.RESET();

	PGLCHARCLIENT pOwnerClient = GLGaeaClient::GetInstance().GetChar( m_dwOwner );
	if( pOwnerClient ) pOwnerClient->m_dwSummonGUID = GAEAID_NULL;

	RESET ();

	return S_OK;
}






void GLAnySummon::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	��Ʈ����ũ ��������.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vTARPOS = m_vPos + m_vDir * 60.0f;

		//	Ÿ�� ��ġ�� �˾Ƴ�.
		STRIKE::SSTRIKE sStrike;
		STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrikeEff, sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	������� ��ǥ���� ȸ�� �Ӽ� �־.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget );
}



WORD GLAnySummon::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

void GLAnySummon::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
}

BOOL GLAnySummon::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

void GLAnySummon::ReceiveDamage( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR )
{
	if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	
	{
		//		m_pSkinChar->SELECTANI ( AN_SHOCK, AN_SUB_NONE );
		//		if ( m_pSkinChar->ISENDANIM() )			SetSTATE ( EM_SUMMONACT_STOP );
		//		SetSTATE ( EM_SUMMONACT_SHOCK );
		TurnAction ( GLAT_SHOCK );
	}


	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		CInnerInterface::GetInstance().SetDamage( vPos, wDamage, dwDamageFlag, UI_ATTACK );

		if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
		{
			// ����Ÿ�� ����Ʈ
			D3DXVECTOR3 vDIR = sACTOR.vPos - m_vPos;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = m_vPos.x;
			matTrans._42 = m_vPos.y + 10.0f;
			matTrans._43 = m_vPos.z;

			//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR );
		}
	}

	RECEIVE_DAMAGE(wDamage);

	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}

void GLAnySummon::ReceiveAVoid ()
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		CInnerInterface::GetInstance().SetDamage( vPos, 0, FALSE, UI_ATTACK );
	}
}

BOOL GLAnySummon::IsValidBody ()
{
	return ( m_Action < GLAT_FALLING );
}


void GLAnySummon::SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag )
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

			//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget );
		}
	}
}

BOOL GLAnySummon::RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge )
{
	//	��ų ���� ������.
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

	//	Note : ������ ��ų�� ��� ���� ������ ã�Ƽ� ��ų ȿ���� �־���.
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

	GASSERT ( 0 && "���� ��ų������ ��ȿ���� ����." );
	return FALSE;
}




