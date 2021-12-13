#include "pch.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "DxGlobalStage.h"
#include "GLItemMan.h"
#include "./GLFactEffect.h"
#include "./GLStrikeM.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/DamageDisplay.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextcontrol.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextcontrol.h"

#include "../[Lib]__Engine/Sources/DxEffect/DxEffCharData.h"
#include "DxEffGroupPlayer.h"
#include "DxShadowMap.h"
#include "DxEnvironment.h"
#include "GLStrikeM.h"

#include ".\GLSummonClient.h"

GLSummonClient::GLSummonClient(void) :
									m_pd3dDevice(NULL),
									m_pSkinChar(NULL),

									m_vDir(D3DXVECTOR3(0,0,-1)),
									m_vDirOrig(D3DXVECTOR3(0,0,-1)),
									m_vPos(D3DXVECTOR3(0,0,0)),

									m_vServerPos(0,0,0),

									m_vMax(6,20,6),
									m_vMin(-6,0,-6),
									m_fHeight(20.f),

									m_vMaxOrg(6,20,6),
									m_vMinOrg(-6,0,-6),

									m_fattTIMER(0.0f),
									m_nattSTEP(0),

									m_pOwner(NULL),
									m_pAttackProp(NULL),

									m_Action(GLAT_IDLE),


									m_bValid(false),
									m_bSkillProcessing(false),
									m_dwActionFlag(0),
									m_fIdleTimer(0.0f),
									m_fAttackTimer(0.0f),
									m_sSummonCrowID(NATIVEID_NULL())

{
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );
	m_sSkillTARGET.RESET ();
	m_sTargetID.RESET();

}

GLSummonClient::~GLSummonClient(void)
{
	SAFE_DELETE ( m_pSkinChar );

}


void GLSummonClient::TurnAction ( EMACTIONTYPE toAction )
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
		m_fIdleTimer = 0.0f;
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

void GLSummonClient::SetSTATE ( DWORD dwStateFlag )					
{
	m_dwActionFlag |= dwStateFlag; 
}

HRESULT GLSummonClient::Create ( const PGLSUMMON pSummonData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pNavi || !pd3dDevice ) return E_FAIL;

	m_pd3dDevice = pd3dDevice;

	ASSIGN ( *pSummonData );

	m_vPos = vPos;
	m_vDir = vDir;
	m_pOwner = GLGaeaClient::GetInstance().GetCharacter ();
	if( m_pOwner ) m_pOwner->m_dwSummonGUID = m_dwGUID;

	//	Note : ���� ���� ������ ������.
	m_pSummonCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pSummonCrowData->GetSkinObjFile(), m_pd3dDevice, TRUE );
	if ( !pCharData )
	{
		if( strlen( m_pSummonCrowData->GetSkinObjFile() ) )
		{
			CDebugSet::ToLogFile ( "GLSummonClient::CreateCrow()-DxSkinCharDataContainer::LoadData() [%s] �������� ����.", m_pSummonCrowData->GetSkinObjFile() );
		}
		return E_FAIL;
	}

	m_Action = GLAT_IDLE;

	m_actorMove.Create ( pNavi, m_vPos, -1 );

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;


	// Note : 1.AABB Box�� �����´�. 2.���̸� ����� ���´�.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	m_bValid = true;

	ReSetAllSTATE ();
	SetSTATE ( EM_SUMMONACT_STOP );
	TurnAction( GLAT_IDLE );

	return S_OK;
}

D3DXVECTOR3 GLSummonClient::GetRandomOwnerTarget ()
{
	D3DXVECTOR3 vRandPos = GetRandomPostision ();
	D3DXVECTOR3 vPetRandPos = GLGaeaClient::GetInstance().GetPetClient()->m_vRandPos;
	if( vPetRandPos != D3DXVECTOR3(0.0f,0.0f,0.0f) )
	{
		for( BYTE i = 0; i < 50; i++ )
		{
			if( vRandPos != vPetRandPos )break;
			vRandPos = GetRandomPostision ();			
		}
	}
	STARGETID sTargetID = m_pOwner->GetTargetID ();
	sTargetID.vPos += vRandPos;
	return sTargetID.vPos;
}


HRESULT GLSummonClient::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sSummonID == NATIVEID_NULL() )
	{
		GASSERT ( 0&&"m_sSUMMONID==NATIVEID_NULL()" );
		return E_FAIL;
	}



	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	if ( !pCrowData )
	{
		GASSERT ( pCrowData );
		return E_FAIL;
	}

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice, 
		TRUE );

	if ( !pSkinChar )
	{
		GASSERT ( pSkinChar );
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice );	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	m_pSkinChar->SetHairColor( 0 );

	return S_OK;
}

HRESULT GLSummonClient::UpdateAnimation ( float fTime, float fElapsedTime )
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
		if ( IsSTATE(EM_SUMMONACT_RUN) )		
			m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else									
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
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
//		DeleteDeviceObjects();
		return S_FALSE;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;
	};

	//	Note : Mob ��Ų ������Ʈ.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( m_Action )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GETMOVEVELO();
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

void GLSummonClient::SetMoveState ( BOOL bRun = TRUE )
{
	if ( bRun ) 
	{
		m_actorMove.SetMaxSpeed (  m_pSummonCrowData->m_sAction.m_fRunVelo );
		SetSTATE ( EM_SUMMONACT_RUN );
	}
	else
	{
		m_actorMove.SetMaxSpeed (  m_pSummonCrowData->m_sAction.m_fWalkVelo );
		ReSetSTATE ( EM_SUMMONACT_WALK );
	}

	TurnAction( GLAT_MOVE );

	// �̵����¸� �����ϴ� �޽��� �߼ۿ��
	GLMSG::SNET_SUMMON_REQ_UPDATE_MOVESTATE NetMsg;
	NetMsg.dwFlag = m_dwActionFlag;
	NETSENDTOFIELD ( &NetMsg );
}

bool GLSummonClient::UpdateSummonPos( bool bTargetMove )
{

	D3DXVECTOR3 vOwnerPos, vPetPos, vRandPos, vDist;
	float		fOwnerDist, fPetDist;

	vOwnerPos = m_pOwner->GetPosition ();
	vDist = m_vPos - vOwnerPos;
	fOwnerDist = D3DXVec3Length(&vDist);

	vPetPos = GLGaeaClient::GetInstance().GetPetClient()->GetPosition();
	vDist = m_vPos - vPetPos;
	fPetDist = D3DXVec3Length(&vDist);


	if ( fOwnerDist > m_fWalkArea || fPetDist < m_fPetDistance )
	{
		if( bTargetMove )
		{
			ReqGoto ( GetRandomOwnerTarget (), FALSE );
			return TRUE;
		}else{
			D3DXVECTOR3 vRandPos = GetRandomPostision ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, FALSE );
			return TRUE;
		}
	}

	return FALSE;
}

HRESULT	GLSummonClient::UpdateSummonState ( float fTime, float fElapsedTime )
{


	static float fTimer(0.0f);
	static WORD wSubAni(0);
	static float fIdleTimer(0.0f);

	GLCOPY* pTarget = NULL;

	if( m_sTargetID.dwID != EMTARGET_NULL ) 
	{
		pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
		if ( !pTarget ) 
		{
			m_sTargetID.RESET();
			TurnAction( GLAT_IDLE );
			ReSetAllSTATE();
			SetSTATE( EM_SUMMONACT_STOP );
			return S_OK;
		}else
		{
			if( pTarget->IsACTION( GLAT_DIE ) || pTarget->IsACTION( GLAT_FALLING ) )
			{
				m_sTargetID.RESET();
				TurnAction( GLAT_IDLE );
				ReSetAllSTATE();
				SetSTATE( EM_SUMMONACT_STOP );
				UpdateSummonPos( TRUE );
			}else/* if( IsSTATE ( EM_SUMMONACT_ATTACK ) )*/
			{
				D3DXVECTOR3 vDist = pTarget->GetPosition () - m_vPos;
				float		fDist = D3DXVec3Length( &vDist );
				vDist			  = m_pOwner->GetPosition() - pTarget->GetPosition();
				float  fOnwerDist = D3DXVec3Length( &vDist );

				WORD wAttackRange = pTarget->GetBodyRadius() + GETSUMMON_BODYRADIUS() + GETSUMMON_ATTACKRANGE() + 2;

				// �Ÿ��� �ʹ� �ָ� ���� �ߴ�
				if( fOnwerDist > m_fRunArea )
				{	
					m_sTargetID.RESET();
					ReSetAllSTATE();
					ReqStop( FALSE );
					return S_OK;
				}else if( fDist > wAttackRange )
				{
					ReSetAllSTATE();
					SetSTATE( EM_SUMMONACT_TRACING );
					TurnAction(GLAT_MOVE);
				}			
			}
		}
	}

	// ������ ����/��ų ���϶�
	if ( m_pOwner->IsACTION ( GLAT_ATTACK ) || m_pOwner->IsACTION ( GLAT_SKILL ) )
	{
		UpdateSummonPos( FALSE );		
		return S_OK;
	}


	switch ( m_Action )
	{
	case GLAT_IDLE:
		// ����������
		if ( IsSTATE ( EM_SUMMONACT_STOP ) )
		{
			if( !pTarget) UpdateSummonPos( TRUE );	
		}else if ( IsSTATE ( EM_SUMMONACT_WAIT ) ) // �̵��� ������ ��ٸ���
		{
			D3DXVECTOR3 vOwnerPos, vRandPos, vDist;
			float fDist;

			vOwnerPos = m_pOwner->GetPosition ();

			if ( m_pOwner->IsACTION ( GLAT_IDLE ) )
			{
				D3DXVECTOR3 vRandPos = GetRandomPostision ();
				vOwnerPos = m_pOwner->GetPosition ();
				vOwnerPos += vRandPos;
				ReqGoto ( vOwnerPos, FALSE );
				return S_OK;
			}

			vDist = vOwnerPos - m_vPos;
			fDist = D3DXVec3Length(&vDist);
			if ( fDist > m_fWalkArea )
			{
				D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
				D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
				vOwnerPos += (vOwnerBack*m_fOwnerDistance);

				ReqGoto ( vOwnerPos, FALSE );
			}

			m_fIdleTimer = 0.0f;
		}
		break;

	case GLAT_MOVE:
		{
			if( IsSTATE ( EM_SUMMONACT_TRACING ) )
			{
				fTimer += fElapsedTime;
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - m_vPos;
				float		fDist = D3DXVec3Length( &vDirection );
				D3DXVec3Normalize ( &vDirection, &vDirection );		

				WORD wAttackRange = pTarget->GetBodyRadius() + GETSUMMON_BODYRADIUS() + GETSUMMON_ATTACKRANGE() + 2;



				if( fDist <= wAttackRange )
				{
					ReqStop( TRUE );
					TurnAction( GLAT_IDLE );
				}else{

					m_vDir = vDirection;

					D3DXVECTOR3 vTargetPos = pTarget->GetPosition();
					vDirection = vTargetPos - m_vPos;
					D3DXVec3Normalize ( &vDirection, &vDirection );
					D3DXVec3Scale( &vDirection, &vDirection, (fDist-wAttackRange)+1.0f );

					vDirection += m_vPos;

					ReqGoto( vDirection, TRUE );
				}
			}else if ( IsSTATE ( EM_SUMMONACT_MOVE ) )
				// �̵� ���� ��
			{
				{
					D3DXVECTOR3 vOwnerPos, vRandPos, vDist;
					float fDist;

					vOwnerPos = m_pOwner->GetPosition ();
					vDist = m_vPos - vOwnerPos;
					fDist = D3DXVec3Length(&vDist);

					STARGETID sTarget = m_pOwner->GetTargetID ();

					if ( !m_actorMove.PathIsActive() )
					{
						ReqStop ();
						return S_OK;
					}

					// ������ ������ �Ÿ��� ���� �ٰų� �ȴ´�
					// [����] ���� �ڵ忡�� �̹� ���� ������ ���¸� ���������Ƿ� 
					//		   ������ �ȴ� ���¿� ���� ó���� ���ָ� �ȴ�.
					if ( !m_pOwner->IsSTATE ( EM_ACT_RUN ) )
					{
						vDist = m_vPos - vOwnerPos;
						float fDist = D3DXVec3Length(&vDist);

						// ���ΰ� �Ÿ��� �ʹ� �־����� �����Ÿ��� �����ϱ� ���� �޸���
						if ( fDist > m_fRunArea && !IsSTATE ( EM_SUMMONACT_RUN_CLOSELY ) )
						{
							m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fRunVelo );
							SetSTATE ( EM_SUMMONACT_RUN_CLOSELY );
							SetMoveState ();
						}
						else if ( IsSTATE ( EM_SUMMONACT_RUN_CLOSELY ) )
						{
							if ( fDist <= m_fWalkArea )
							{
								m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fWalkVelo );
								ReSetSTATE ( EM_SUMMONACT_RUN_CLOSELY );
								SetMoveState ( FALSE );
							}
						}

						// ������ �����ִµ� �Ȱ� �ִٸ�
						if ( m_pOwner->IsACTION ( GLAT_IDLE ) )
						{
							SetMoveState ();
						}
					}

					// 1�ʿ� �ѹ��� �̵��޽����� �����Ѵ�. (�̺κ��� �����غ��� �ҵ��ϴ�)
					fTimer += fElapsedTime;
					if ( fTimer > 1.0f )
					{
						if( m_pOwner->IsACTION ( GLAT_MOVE ) )
						{
							// D3DXVECTOR3 vRandPos = GetRandomPostision ();
							vOwnerPos = m_pOwner->GetPosition ();

							D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
							D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
							vOwnerPos += (vOwnerBack*m_fOwnerDistance);

							ReqGoto ( vOwnerPos, FALSE );
						}						
						fTimer = 0.0f;
					}

				}
				m_fIdleTimer = 0.0f;
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
		// ���� ���ݸ�� ���̸�
		//		if ( IsSTATE ( EM_SUMMONACT_ATTACK ) )
		{
			AttackProc ( fElapsedTime );

			//	Note : ���� �������� ȸ��.
			//
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - m_vPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;		
			}


			//if( UpdateSummonPos( TRUE ) == FALSE )
			//{
			//	// ���ݸ���� �ƹ��� ������ 2���̻� �������� �ʴ´�.
			//	m_fAttackTimer += fElapsedTime;
			//	if ( m_fAttackTimer > 2.0f )
			//	{
			//		ReqStop ( true );
			//	}
			//}
		}

		break;

	case GLAT_SHOCK:
		break;

	case GLAT_FALLING:
		break;

	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction( GLAT_IDLE );
				SetSTATE( EM_SUMMONACT_STOP );
			}

		}
		break;

	case GLAT_DIE:
		break;
	};


	return S_OK;
}


BOOL GLSummonClient::IsValidBody ()
{
	return ( m_Action < GLAT_FALLING );
}



HRESULT GLSummonClient::FrameMove( float fTime, float fElapsedTime )
{
	if ( !m_bValid ) return S_OK;
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


	UpdateSummonState ( fTime, fElapsedTime );

	if( UpdateAnimation( fTime, fElapsedTime ) == S_FALSE ) return S_OK;	


	// UpdateSkillDelay ( fElapsedTime ); ��ȹ�� �躴�� ��û���� ���� [06.10.09]

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
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;



	CDebugSet::ToView ( 1, "summon Pos : %4.1f, %4.1f, %4.1f", m_vPos.x, m_vPos.y, m_vPos.z );

	string strState;
	if( IsSTATE( EM_SUMMONACT_MOVE  ) )	// ������
	{
		strState += "�̵�";
	}
	if( IsSTATE( EM_SUMMONACT_WAIT  ) )	// ��ô��
	{
		strState += " ��ô��";
	}
	if( IsSTATE( EM_SUMMONACT_STOP  ) )	// ��������
	{
		strState += " ��������";
	}
	if( IsSTATE( EM_SUMMONACT_RUN   ) )		// �ٱ���
	{
		strState += " �ٱ���";
	}
	if( IsSTATE( EM_SUMMONACT_WALK	) )		// �ȱ���
	{
		strState += " �ȱ���";
	}
	if( IsSTATE( EM_SUMMONACT_RUN_CLOSELY )	)	// �����Ÿ����� �ٱ���
	{
		strState += " �����Ÿ�����";
	}
	if( IsSTATE( EM_SUMMONACT_ATTACK	  )	)	// Attack���
	{
		strState += " Attack";
	}
	if( IsSTATE( EM_SUMMONACT_TRACING	  )	)	// �������
	{
		strState += " �������";
	}


	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( !pTarget ) 
	{
		CDebugSet::ToView ( 2, "%s TargetID %d", strState.c_str(), m_sTargetID.dwID );
	}else{
		CDebugSet::ToView ( 2, "%s TargetID %d %.2fx %.2fy %.2fz", strState.c_str(), m_sTargetID.dwID, 
								pTarget->GetPosition().x, pTarget->GetPosition().y, pTarget->GetPosition().z );
	}
	CDebugSet::ToView ( 3, COMMENT::ATIONTYPE[m_Action].c_str() );


	return S_OK;
}

HRESULT GLSummonClient::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )   return S_OK;
	if ( !pd3dDevice ) return E_FAIL;

	// ���� �ǽ� ������ �Ųٷ� �׸���.
	if ( m_pSkinChar ) 
	{
		m_pSkinChar->Render( pd3dDevice, m_matTrans, FALSE, TRUE, TRUE );
	}


#ifdef _SYNC_TEST
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vServerPos, 2.0f );
	}
#endif

	return S_OK;
}

HRESULT GLSummonClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )					return S_FALSE;
	if ( !pd3dDevice )					return S_FALSE;
	//	if ( IsACTION(GLAT_DIE) )			return S_FALSE;

	//	Note : �׸��� ������.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLSummonClient::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )   return S_OK;
	if ( !pd3dDevice )					return S_FALSE;
	//	if ( IsACTION(GLAT_DIE) )			return S_FALSE;

	//	Note : �ݻ� ������.
	//
	if ( m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLSummonClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLSummonClient::InvalidateDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLSummonClient::DeleteDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	//	if( !IsValidBody() && m_Action != GLAT_DIE )
	//		return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
		SAFE_DELETE( m_pSkinChar );
	}

	m_actorMove.Stop ();
	m_actorMove.Release ();

	if( m_pOwner ) 
		m_pOwner->m_dwSummonGUID = GAEAID_NULL;

	m_pd3dDevice  = NULL;
	m_pOwner	  = NULL;
	m_pAttackProp = NULL;

	m_vDir	     = D3DXVECTOR3 (0,0,-1);
	m_vPos	     = D3DXVECTOR3(0,0,0);
	m_vDirOrig   = D3DXVECTOR3(0,0,-1);
	m_vMax	     = D3DXVECTOR3(6,20,6);
	m_vMin	     = D3DXVECTOR3(-6,0,-6);
	m_vMaxOrg    = D3DXVECTOR3(6,20,6);
	m_vMinOrg    = D3DXVECTOR3(-6,0,-6);


	m_bValid		   = false;
	m_bSkillProcessing = false;

	ReSetAllSTATE ();
	SetSTATE ( EM_SUMMONACT_STOP );
	m_Action = GLAT_IDLE;

	m_sSkillTARGET.RESET ();
	m_sTargetID.RESET();

	m_fattTIMER    = 0.0f;
	m_fIdleTimer   = 0.0f;
	m_fAttackTimer = 0.0f;

	m_nattSTEP	   = 0;


	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );

	m_sSummonCrowID = NATIVEID_NULL();


	CInnerInterface::GetInstance().HideGroup( SUMMON_HP );	

	RESET_DATA();

	RESET ();

	return S_OK;
}

void GLSummonClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
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

void GLSummonClient::SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag )
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

void GLSummonClient::MakeAniSubKey ( float fTime )
{
	static bool bPlus(true);

	srand ( unsigned int(fTime) );
	m_wAniSub[0] = rand()%3+1;
	m_wAniSub[1] = rand()%3+1;
	if ( m_wAniSub[0] == m_wAniSub[1] )
	{
		if ( bPlus )
		{
			m_wAniSub[1] += 1;
			if ( m_wAniSub[1] > 3 ) m_wAniSub[1] = 1;
		}
		else
		{
			m_wAniSub[1] -= 1;
			if ( m_wAniSub[1] < 1 ) m_wAniSub[1] = 3;
		}
		bPlus = !bPlus;
	}
	for ( int i = 1; i < 4; ++i )
	{
		if ( m_wAniSub[0] != i && m_wAniSub[1] != i )
		{
			m_wAniSub[2] = i;
			break;
		}
	}
}


void GLSummonClient::StartAttackProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;
}

void GLSummonClient::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
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

BOOL GLSummonClient::AttackProc ( float fElapsedTime )
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

BOOL GLSummonClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

WORD GLSummonClient::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

void GLSummonClient::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
}

void GLSummonClient::ReceiveDamage( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR )
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

void GLSummonClient::ReceiveAVoid ()
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		CInnerInterface::GetInstance().SetDamage( vPos, 0, FALSE, UI_ATTACK );
	}
}

float GLSummonClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
}


BOOL GLSummonClient::RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, const DWORD dwSELECT, float fAge )
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


