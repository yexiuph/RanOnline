#include "pch.h"
#include "./GLCrow.h"
#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BOX_R	(4)

GLCrow::GLCrow(void) :
	m_nSkillReActionError( 0 ),
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),
	m_vDir(0,0,-1),

	m_vMaxOrg(BOX_R,20,BOX_R),
	m_vMinOrg(-BOX_R,0,-BOX_R),

	m_vMax(BOX_R,20,BOX_R),
	m_vMin(-BOX_R,0,-BOX_R),

	m_wTARNUM(0),
	m_vTARPOS(0,0,0),

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
	m_vOrgTarPos(0,0,0),

	m_fScale( 0.0f ),

	m_vStartPos(0,0,0),

	m_dwAttPosSlot(GLARoundSlot::SLOT_SIZE),

	m_emReAction(EMREACT_NUNE),
	m_pAttackProp(0),

	m_fattTIMER(0.0f),

	m_sAssault(CROW_PC,GAEAID_NULL),

	m_fIdleTime(0.0f),
	m_fStayTimer(0.0f),

	m_fTargetTimer(0.0f),

	m_pCurAct(NULL),
	m_pMobSchedule(NULL),

	m_dwPC_AMOUNT(0),
	m_bNpcRecall(false),
	m_dwCallCharID(0),
	m_dwCallUserID(0),
	m_tGenTime(0),
	m_lnCommission(0)
{	
}

GLCrow::~GLCrow(void)
{
	SAFE_DELETE(m_pSkinChar);
}

void GLCrow::RESET_DATA ()
{
	int i=0;

	GLCROWLOGIC::RESET_DATA();

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
	m_vOrgTarPos = D3DXVECTOR3(0,0,0);

	m_fScale = 0.0f;

	m_vStartPos = D3DXVECTOR3(0,0,0);

	m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;
	m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);

	m_emReAction = EMREACT_NUNE;
	m_pAttackProp = NULL;
	m_fattTIMER = 0.0f;
	m_sHITARRAY.clear();

	m_wTARNUM = 0;
	
	for ( i=0; i<EMTARGET_NET; ++i )
		m_sTARIDS[i] = STARID(CROW_PC,GAEAID_NULL);
	m_vTARPOS = D3DXVECTOR3(0,0,0);

	m_sAssault = STARGETID(CROW_PC,GAEAID_NULL);

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	m_fIdleTime = 0.0f;
	m_fStayTimer = 0.0f;

	m_fTargetTimer = 0.0f;

	m_pCurAct = NULL;
	m_pMobSchedule = NULL;

	m_nSkillReActionError = 0;

	m_bNpcRecall = false;
	m_dwCallCharID = 0;
	m_dwCallUserID = 0;
	m_tGenTime = 0;
	m_lnCommission = 0;
}

HRESULT GLCrow::CreateCrow ( GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, SNATIVEID sCROWID,
							LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXVECTOR3 pPos, FLOAT fScale )
{
	GASSERT(pLandMan);

	m_pLandMan = pLandMan;
	m_pd3dDevice = pd3dDevice;

	//	Note : ���� ���� ������ ������.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sCROWID );
	if( !m_pCrowData )
	{
		DEBUGMSG_WRITE ( _T("GLCrow::CreateMob, Mob Create Failed. nativeid [%d/%d]"), sCROWID.wMainID, sCROWID.wSubID );
		return E_FAIL;
	}
	
	//	Note : �� ���� ������ �ʱ�ȭ.
	GLCROWLOGIC::INIT_DATA ();


	//	Note : �����췯 �ʱ�ȭ.
	m_pMobSchedule = pMobSchedule;

	//	��ġ �ʱ�ȭ.
	if ( pPos )					m_vPos = *pPos;
	else if ( m_pMobSchedule )	m_vPos = m_pMobSchedule->m_pAffineParts->vTrans;
	
	m_vGenPos = m_vPos;
	m_vOrgTarPos = m_vPos;
	
	m_fScale = fScale;

	//	���� �ʱ�ȭ.
	D3DXMATRIX matY, matScale;
	D3DXMatrixIdentity ( &matY );
	D3DXMatrixIdentity ( &matScale );

	if ( m_pMobSchedule )
		D3DXMatrixRotationY ( &matY, m_pMobSchedule->m_pAffineParts->vRotate.x );

	D3DXVec3TransformCoord ( &m_vDir, &vDIRORIG, &matY );

	if ( m_pMobSchedule )
		m_pCurAct = m_pMobSchedule->m_sMobActList.m_pHead;

	//	Note : �̵� ���� �ʱ�ȭ.
	ToWalk ();	

	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();
	m_actorMove.Create ( pLandMan->GetNavi(), m_vPos, -1 );

	//	Note : �ֺ� �ɸ��� �ν�.
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

void GLCrow::SetPosition ( D3DXVECTOR3 &vPos )
{
	ToWalk ();
	m_actorMove.SetPosition ( vPos, -1 );
	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();

	return;
}

inline HRESULT GLCrow::MoveTo ( const D3DXVECTOR3 &vTarPos )
{
	if ( m_pCrowData->IsPosHold() )		return S_FALSE;

	m_TargetID.vPos = vTarPos;
	TurnAction ( GLAT_MOVE );

	return S_OK;
}

BOOL GLCrow::IsVisibleDetect ( const BOOL bRECVISIBLE )
{
	if ( IsSTATE(EM_REQ_VISIBLENONE) || IsSTATE(EM_REQ_VISIBLEOFF) )	return FALSE;

	if ( m_bINVISIBLE && !bRECVISIBLE )									return FALSE;

	return TRUE;
}

HRESULT GLCrow::ESCAPE ( STARGETID &sTARID, const DWORD dwDamage )
{
	if ( m_pCrowData->IsPosHold() )		return S_FALSE;

	EMCROWACT_UP emCROWACT = GetActPattern();
	BOOL bESCAPE = ( EMCROWACT_UP_ESCAPE == emCROWACT ) && ( dwDamage > DWORD ( m_pCrowData->m_dwHP * 0.10f ) );
    if ( !bESCAPE )		return S_FALSE;

	GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTARID );
	if ( pACTOR && pACTOR->IsValidBody() )
	{
		const D3DXVECTOR3 &vACTOR_POS = pACTOR->GetPosition();

		D3DXVECTOR3 vDir = ( GetPosition() - vACTOR_POS );		//	Attack -> Receive ������ ����.
		vDir.y = 0.0f;
		D3DXVec3Normalize (  &vDir, &vDir );

		enum { EMROTSIZE = 5, EMESCAPE_RANGE = 60 };
		static D3DXMATRIX _matROT000, _matROT045, _matROT315, _matROT090, _matROT270;
		static D3DXMATRIX matROT000 = *D3DXMatrixRotationY ( &_matROT000, 0 );
		static D3DXMATRIX matROT045 = *D3DXMatrixRotationY ( &_matROT045, D3DX_PI/4.0f );
		static D3DXMATRIX matROT315 = *D3DXMatrixRotationY ( &_matROT315, -D3DX_PI/4.0f );
		static D3DXMATRIX matROT090 = *D3DXMatrixRotationY ( &_matROT045, D3DX_PI/2.0f );
		static D3DXMATRIX matROT270 = *D3DXMatrixRotationY ( &_matROT045, -D3DX_PI/2.0f );
		static D3DXMATRIX *pmatROT[EMROTSIZE] = { &matROT000, &matROT045, &matROT315, &matROT090, &matROT270 };

		BOOL bCoB(FALSE);
		DWORD dwCoB(0);
		D3DXVECTOR3 vCoB(0,0,0);

		for ( int i=0; i<EMROTSIZE; ++i )
		{
			D3DXVECTOR3 vDirRot;
			D3DXVec3TransformCoord ( &vDirRot, &vDir, pmatROT[i] );

			D3DXVECTOR3 vPosB = GetPosition() + vDirRot * EMESCAPE_RANGE;
			NavigationMesh* pNavi = m_pLandMan->GetNavi();

			pNavi->IsCollision ( D3DXVECTOR3(vPosB.x,vPosB.y+20.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-30.0f,vPosB.z), vCoB, dwCoB, bCoB );
			if ( bCoB )
			{
				if ( m_pCrowData->m_sAction.m_bRun )	ToRun();
				MoveTo ( vCoB );
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

void GLCrow::ToRun ()
{
	GASSERT(m_pCrowData);

	SetSTATE(EM_ACT_RUN);
	m_actorMove.SetMaxSpeed ( m_pCrowData->m_sAction.m_fRunVelo );
}

void GLCrow::ToWalk ()
{
	GASSERT(m_pCrowData);

	ReSetSTATE(EM_ACT_RUN);
	m_actorMove.SetMaxSpeed ( m_pCrowData->m_sAction.m_fWalkVelo );
}

INT GLCrow::IsLowerHP ()							
{ 
	float fRate = 0.0f;
	for( int i = 0; i < (int)m_pCrowData->m_sAction.m_vecPatternList.size(); i++ )
	{
		fRate = m_pCrowData->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate;
		if( GETHP() >= (int)( (float)GETMAXHP() * fRate ) / 100.0f )
		{
			return i;
		}
	}
	return -1;
}

EMCROWACT_UP GLCrow::GetActPattern ()
{
	INT nIndex = IsLowerHP();

	EMCROWACT_UP emACT_UP;

	if( nIndex >= 0 )
	{
		emACT_UP = m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_emActPattern;
	}
	else if( nIndex < 0 )
	{
		emACT_UP = EMCROWACT_UP_IDLE;
	}

	return emACT_UP;
}


void GLCrow::TurnAction ( EMACTIONTYPE toAction, bool bMsg )
{
	//	Note : ���� �׼� ���.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_ATTACK:
		m_emReAction = EMREACT_NUNE;

		//	Note : ���ݼ����� ������ ������ ���.
		if ( m_pAttackProp )
		{
			m_fACTIONDELAY[m_dwAType] = m_pAttackProp->fDelay + 0.3f;
		}
		break;

	case GLAT_SKILL:
		{
			m_idACTIVESKILL = NATIVEID_NULL();
			m_wTARNUM = 0;
			m_emReAction = EMREACT_NUNE;

			if ( m_pAttackProp )
			{
				SNATIVEID sRunSkill = m_pAttackProp->skill_id;
				WORD wLevel = m_pAttackProp->skill_lev;

				//	Note : ��ų ���� ������.
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sRunSkill );
				if ( !pSkill )								break;
				if ( wLevel >= SKILL::MAX_LEVEL )			break;

				float fSkillDelay = pSkill->m_sAPPLY.sDATA_LVL[wLevel].fDELAYTIME;
				m_fACTIONDELAY[m_dwAType] = fSkillDelay + 0.1f;
			}
		}
		break;

	default:
		break;
	};

	//	Note : �׼� �ʱ�ȭ.
	//
	EMACTIONTYPE emOldAction = m_Action;
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;

			if ( !bMsg ) break;

			GLMSG::SNET_ACTION_BRD NetMsg;
			NetMsg.emCrow	= m_pCrowData->m_emCrow;
			NetMsg.dwID		= m_dwGlobID;
			NetMsg.emAction	= m_Action;

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
		}
		break;

	case GLAT_MOVE:
		{
			D3DXVECTOR3 vDist = m_vPos - m_TargetID.vPos;
			float fDist = D3DXVec3Length ( &vDist );
			if ( fDist < 1.0f )
			{
				TurnAction(GLAT_IDLE,(emOldAction!=GLAT_IDLE));
				break;
			}

			BOOL bSucceed = m_actorMove.GotoLocation
			(
				m_TargetID.vPos+D3DXVECTOR3(0,+10,0),
				m_TargetID.vPos+D3DXVECTOR3(0,-10,0)
			);


			if ( !bMsg ) break;

			if ( bSucceed )
			{
				m_vStartPos = m_vPos;

				//	Note : �̵� �޼��� �߻�.
				//
				GLMSG::SNETCROW_MOVETO NetMsg;
				NetMsg.dwGlobID		= m_dwGlobID;
				NetMsg.dwActState	= m_dwActState;
				//NetMsg.vCurPos	= m_vPos;					 m_actorMove.GotoLocation() ȣ���� 
				NetMsg.vCurPos		= m_actorMove.Position(); // m_vPos�� ����� �� �ֱ⶧���� ������.
				NetMsg.vTarPos		= m_TargetID.vPos;
				NetMsg.bTrace		= IsFLAG(EMTRACING);
				SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		break;

	case GLAT_ATTACK:
		{
			m_fattTIMER = 0.0f;
			PreStrikeProc ( FALSE, FALSE );

			if ( !bMsg ) break;

			//	Note : ���� �޼��� �߻�.
			//
			GLMSG::SNETCROW_ATTACK NetMsg;
			NetMsg.dwGlobID		= m_dwGlobID;
			NetMsg.dwTarID		= m_TargetID.dwID;
			NetMsg.emTarCrow	= m_TargetID.emCrow;
			NetMsg.dwAType		= m_dwAType;

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );

			STARGETID sTargetID = STARGETID ( CROW_MOB, m_dwGlobID, m_vPos );
			
			
			if( m_TargetID.emCrow == CROW_PC )
			{
				PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( m_TargetID.dwID );
				if ( pCHAR )
				{
					// PET Attack ó��
					PGLPETFIELD pEnemyPet = GLGaeaServer::GetInstance().GetPET ( pCHAR->m_dwPetGUID );
					if ( pEnemyPet && pEnemyPet->IsValid () && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
					{
						D3DXVECTOR3 vOwnerPos, vDist;
						float fDist;
						vOwnerPos = pCHAR->GetPosition ();
						vDist = pEnemyPet->m_vPos - vOwnerPos;
						fDist = D3DXVec3Length(&vDist);

						// �����Ÿ� �ȿ� ������ 
						if ( fDist <= GLCONST_PET::fWalkArea )
						{
							pEnemyPet->ReSetAllSTATE ();
							pEnemyPet->SetSTATE ( EM_PETACT_ATTACK );

							GLMSG::SNETPET_ATTACK NetMsg;
							NetMsg.sTarID = sTargetID;
							GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsg );
							
							GLMSG::SNETPET_ATTACK_BRD NetMsgBRD;
							NetMsgBRD.dwGUID = pEnemyPet->m_dwGUID;
							NetMsgBRD.sTarID = sTargetID;
							pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
						}
					}
					// Summon Attack ó��
					PGLSUMMONFIELD pEnemySummon = GLGaeaServer::GetInstance().GetSummon ( pCHAR->m_dwSummonGUID );
					if ( pEnemySummon && pEnemySummon->IsValid () && !pEnemySummon->IsSTATE ( EM_SUMMONACT_ATTACK ) && 
						pEnemySummon->GetAttackTarget( sTargetID ) )
					{
						/*D3DXVECTOR3 vOwnerPos, vDist;
						float fDist;
						vOwnerPos = pCHAR->GetPosition ();
						vDist = pEnemySummon->m_vPos - vOwnerPos;
						fDist = D3DXVec3Length(&vDist);

						if ( fDist <= pEnemySummon->m_fWalkArea )*/
						{
							pEnemySummon->SetAttackTarget( sTargetID );
						}
					}
				}
			}else if( m_TargetID.emCrow == CROW_SUMMON )
			{
				// Summon Attack ó��
				PGLSUMMONFIELD pEnemySummon = GLGaeaServer::GetInstance().GetSummon ( m_TargetID.dwID );
				if ( pEnemySummon && pEnemySummon->IsValid () && !pEnemySummon->IsSTATE ( EM_SUMMONACT_ATTACK ) && 
					pEnemySummon->GetAttackTarget( sTargetID ) )
				{
					//D3DXVECTOR3 vOwnerPos, vDist;
					//float fDist;
					//vOwnerPos = pCHAR->GetPosition ();
					//vDist = pEnemySummon->m_vPos - vOwnerPos;
					//fDist = D3DXVec3Length(&vDist);

					//// �����Ÿ� �ȿ� ������ 
					//if ( fDist <= GLCONST_SUMMON::fWalkArea )
					//{
					

					pEnemySummon->SetAttackTarget( sTargetID );
				}				
			}
		}
		break;

	case GLAT_SKILL:
		m_fattTIMER = 0.0f;
		PreStrikeProc ( TRUE, FALSE );

		StartSkillProc ();
		break;

	case GLAT_SHOCK:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_PUSHPULL:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_FALLING:
		{
			m_fIdleTime = 0.0f;
			if ( !bMsg ) break;

			//	Note : �ֺ� Char���� �������� �˸�.
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

	if ( m_actorMove.PathIsActive() )
	{
		if ( !IsACTION(GLAT_MOVE) && !IsACTION(GLAT_PUSHPULL) )		m_actorMove.Stop();
	}
}

inline DWORD GLCrow::ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const BOOL bShock )
{
	GLACTOR* pACTOR_TAR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pACTOR_TAR )	return 0;

	//	Note : Ÿ���� ���� ������ ��쿡 damage �ټ� ����.
	//
	if ( (CROW_PC==sTargetID.emCrow || CROW_SUMMON==sTargetID.emCrow) && !pACTOR_TAR->IsValidBody() )	return 0;

	return pACTOR_TAR->ReceiveDamage ( GETCROW(), m_dwGlobID, nDAMAGE, bShock );
}

void GLCrow::AvoidProc ( )
{
	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETCROW_AVOID NetMsg;
	NetMsg.dwGlobID		= m_dwGlobID;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.dwID;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );

	GLACTOR* pACTOR_TAR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
	if ( !pACTOR_TAR )	return;

	//	Note : Ÿ���� ���� ������ ��쿡 damage �ټ� ����.
	//
	if ( CROW_PC==m_TargetID.emCrow && pACTOR_TAR->IsValidBody() )
	{
		pACTOR_TAR->ReceiveDamage ( GETCROW(), m_dwGlobID, 0, false );
	}
}

void GLCrow::DamageProc ( const int nDAMAGE, const DWORD dwDamageFlag )
{
	//	Note : ����ڿ��� ������� ����.
	bool bShock = ( dwDamageFlag & DAMAGE_TYPE_SHOCK );
	DWORD dwNowHP = ToDamage ( m_TargetID, nDAMAGE, bShock );

	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETCROW_DAMAGE NetMsg;
	NetMsg.dwGlobID		= m_dwGlobID;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.dwID;
	NetMsg.nDamage		= nDAMAGE;
	NetMsg.dwNowHP		= dwNowHP;
	NetMsg.dwDamageFlag	= dwDamageFlag;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );

	GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );

	//	Note : ���� �̻� ����.
	//
	if ( pACTOR && m_pAttackProp->emBLOW_TYPE!=EMBLOW_NONE )
	{
		//	Note : �߻� Ȯ�� ���.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(m_pAttackProp->emBLOW_TYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( m_pAttackProp->emBLOW_TYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( m_pAttackProp->emBLOW_TYPE, dwWeather, m_pLandMan->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(m_pAttackProp->emBLOW_TYPE)) )
		{
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( m_pAttackProp->fBLOW_RATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );
		}

		if ( bBLOW )
		{
			//	Note : �����̻� �߻�.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = m_pAttackProp->fBLOW_LIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = m_pAttackProp->emBLOW_TYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = m_pAttackProp->fBLOW_VAR1;
			sSTATEBLOW.fSTATE_VAR2 = m_pAttackProp->fBLOW_VAR2;

			pACTOR->STATEBLOW ( sSTATEBLOW );

			//	Note : �����̻� �߻� Msg.
			//
			GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
			NetMsgState.emCrow = m_TargetID.emCrow;
			NetMsgState.dwID = m_TargetID.dwID;
			NetMsgState.emBLOW = sSTATEBLOW.emBLOW;
			NetMsgState.fAGE = fLIFE;
			NetMsgState.fSTATE_VAR1 = sSTATEBLOW.fSTATE_VAR1;
			NetMsgState.fSTATE_VAR2 = sSTATEBLOW.fSTATE_VAR2;

			//	Note : '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
			//
			pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

			//	Note : ��ų ��󿡰� �޼��� ����.
			if ( m_TargetID.emCrow==CROW_PC )	GLGaeaServer::GetInstance().SENDTOCLIENT ( pACTOR->GETCLIENTID (), &NetMsgState );
		}
	}
}

void GLCrow::DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR )
{
	//	Note : ����ڿ��� ������� ����.
	DWORD dwNowHP = ToDamage ( sACTOR, nDAMAGE, FALSE );

	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETCROW_DAMAGE NetMsg;
	NetMsg.dwGlobID		= m_dwGlobID;
	NetMsg.emTarCrow	= sACTOR.emCrow;
	NetMsg.dwTarID		= sACTOR.dwID;
	NetMsg.nDamage		= nDAMAGE;
	NetMsg.dwNowHP		= dwNowHP;
	NetMsg.dwDamageFlag	= DAMAGE_TYPE_NONE;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
}

void GLCrow::PreAttackProc ()
{
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = CHECKHIT(m_TargetID,m_pLandMan) ? true : false;
		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

void GLCrow::PreStrikeProc ( BOOL bSkill, BOOL bLowSP )
{
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	m_sHITARRAY.clear ();

	STARGETID sTargetID;
	BOOL bCheckHit = FALSE;

	if ( !bSkill )
	{
		sTargetID = m_TargetID;
		bCheckHit = TRUE;
	}
	else
	{
		//	Note : ��ų ���� ������.
		//
		PGLSKILL pSkill = NULL;
		pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
		if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
		{
			//	Note : ��ų Ÿ���� �ϳ��� ��ȿ�ϰ� Ȯ���� �ϰ� ����.
			//	NEED : ���� Ÿ���� ��� �ٽ� Ȯ���ؾ���.
			sTargetID = STARGETID(static_cast<EMCROW>(m_sTARIDS[0].wCrow),static_cast<DWORD>(m_sTARIDS[0].wID));
			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.dwID = EMTARGET_NULL;
		}
	}

	//	ť �ʱ�ȭ
	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = true;
		if ( bCheckHit && (sTargetID.dwID!=EMTARGET_NULL) )
		{
			if ( !CHECKHIT(sTargetID,m_pLandMan) )		bhit = false;
		}

		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

BOOL GLCrow::AttackProcess ( float fElapsedTime )
{
	GASSERT(m_pAttackProp);
	m_fattTIMER += fElapsedTime * GETATTVELO();

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;
	int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int wThisKey = int(m_fattTIMER*UNITANIKEY_PERSEC);

	if ( GetAttackRangeType()==EMATT_SHORT )
	{
		GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTARGET )
		{
			TurnAction ( GLAT_IDLE, false );
			return FALSE;
		}

		D3DXVECTOR3 vDist = m_vPos - pTARGET->GetPosition();
		float fTarLength = D3DXVec3Length(&vDist);

		WORD wAttackRange = pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
		if ( wAttackRange*GLCONST_CHAR::fREACT_VALID_SCALE < (WORD)(fTarLength) )
		{
			TurnAction ( GLAT_IDLE, false );
			return FALSE;
		}
	}

	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( wThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
			{
				int nDamage = 0;
				DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
				dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGlobID, m_TargetID, m_pLandMan );
				DamageProc ( nDamage, dwDamageFlag );
			}
			else					AvoidProc ();

			//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	�̻����� ���� ����Ÿ�� ���� ó������ �������,
	//	�������� �Ѳ����� ó���ϰ�, FALSE�� �����Ѵ�.
	if ( wThisKey >= wTotalKeys )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();
				
				if ( sStrike.bHit )
				{
					int nDamage = 0;
					DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
					dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGlobID, m_TargetID, m_pLandMan );
					DamageProc ( nDamage, dwDamageFlag );
				}
				else					AvoidProc ();

				//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
				m_sHITARRAY.pop_front ();
			}
		}

		return FALSE;
	}

	return TRUE;
}

BOOL GLCrow::ShockProcess ( float fElapsedTime )
{
	VECANIATTACK &vecShock = m_pCrowData->GetAnimation(AN_SHOCK,AN_SUB_NONE);
	if ( vecShock.empty() )	return FALSE;

	m_fIdleTime += fElapsedTime * GETATTVELO();
			
	SANIATTACK &sAniShock = vecShock[0];

	int wTotalKeys = int(sAniShock.m_dwETime) - int(sAniShock.m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);
	
	if ( wThisKey >= wTotalKeys )	return FALSE;

	return TRUE;
}

NET_MSG_GENERIC* GLCrow::ReqNetMsg_Drop ()
{
	static GLMSG::SNETDROP_CROW NetMsg;
	NetMsg = GLMSG::SNETDROP_CROW();

	SDROP_CROW &dropCrow	= NetMsg.Data;
	dropCrow.sNativeID		= m_sNativeID;

	dropCrow.sMapID			= m_sMapID;
	dropCrow.dwCeID			= m_dwCeID;
	dropCrow.dwGlobID		= m_dwGlobID;
	dropCrow.vPos			= m_vPos;
	dropCrow.vDir			= m_vDir;

	dropCrow.dwNowHP		= m_dwNowHP;
	dropCrow.wNowMP			= m_wNowMP;

	dropCrow.fScale			= m_fScale;

	if ( m_fAge < 0.6f )
		dropCrow.dwFLAGS	|= SDROP_CROW::CROW_GEM;

	dropCrow.emAction		= m_Action;
	dropCrow.dwActState		= m_dwActState;
	dropCrow.TargetID		= m_TargetID;
	
	if ( dropCrow.emAction==GLAT_MOVE )
	{
		if ( m_actorMove.PathIsActive() )
		{
			dropCrow.TargetID.vPos = m_actorMove.GetTargetPosition();

			// ���� Ŭ���̾�Ʈ�� ���� ��ġ ����ȭ�� ����
			dropCrow.vStartPos		= m_vStartPos;
			dropCrow.sNextWaypoint  = m_actorMove.GetNextWayPoint ();
		}
		else
		{
			dropCrow.emAction = GLAT_IDLE;
		}
	}

	int i;
	for ( i=0; i<SKILLFACT_SIZE; ++i )	dropCrow.sSKILLFACT[i].Assign ( m_sSKILLFACT[i], i );
	for ( i=0; i<EMBLOW_MULTI; ++i )	dropCrow.sSTATEBLOWS[i] = m_sSTATEBLOWS[i];

	return (NET_MSG_GENERIC*) &NetMsg;
}

inline void GLCrow::INSERT_PC_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecPC_VIEW.size() < m_dwPC_AMOUNT+16 )
		m_vecPC_VIEW.resize(m_dwPC_AMOUNT+16);

	m_vecPC_VIEW[m_dwPC_AMOUNT++] = dwGAEAID;
}

HRESULT GLCrow::UpdateViewAround ()
{
	int nX, nZ;

	m_dwPC_AMOUNT = 0;

	//	Note : ���� �þ߿� ������ CELL ��� Ž��.
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

			//	Note : ĳ������ ���� ������ UpdateViewAround ()���� ����ġ�� ĳ���Ͱ� �������� ���� �� ����.
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

HRESULT GLCrow::SendMsgViewAround ( NET_MSG_GENERIC* nmg )
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

BOOL GLCrow::IsValidBody () const
{
	return ( m_Action < GLAT_FALLING );
}

BOOL GLCrow::IsDie () const
{
	if ( m_Action==GLAT_DIE )		
		return TRUE;

	return FALSE;
}


HRESULT GLCrow::UpateAnimation ( float fTime, float fElapsedTime )
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
		m_pSkinChar->SELECTANI ( AN_TALK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsSTATE(EM_ACT_RUN) )				m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else									m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
		if ( !ShockProcess ( fElapsedTime ) )	TurnAction ( GLAT_IDLE );
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

	//	Note : Mob ��Ų ������Ʈ.
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
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap );

	return S_OK;
}

float GLCrow::GetMoveVelo ()
{
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? m_pCrowData->m_sAction.m_fRunVelo : m_pCrowData->m_sAction.m_fWalkVelo;
	return fDefaultVelo * GLCROWLOGIC::GETMOVEVELO();
}

HRESULT GLCrow::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr;
	GASSERT(m_pCrowData);
	GASSERT(m_pLandMan);

	//	Note : ���忡�� ��ϵ� ���� ������Ʈ �ϱ� ���ؼ� ȣ����.
	//
	m_pLandMan->MoveCrow ( m_dwGlobID, m_vPos );
	UpdateViewAround ();

	//	���� ������ �ɸ��Ͱ� ����, skinchar�� ��ȿ�϶�(���ķ�����,level edit)�� �ƴҶ�. & �������� �ƴҶ�
	if ( m_dwPC_AMOUNT==0 && !m_pSkinChar && (m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_BOSS)==NULL) return S_OK;

	GASSERT(m_dwAType<SCROWDATA::EMMAXATTACK);
	m_pAttackProp = &(m_pCrowData->m_sCrowAttack[m_dwAType]);

	m_fAge += fElapsedTime;
	for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
	{
		if ( m_fACTIONDELAY[i] > 0.0f )
			m_fACTIONDELAY[i] -= fElapsedTime;
	}

	//	Note : AABB ���.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	//	Note : ���� ��ǥ�� ��ȿ�� �˻�.
	//		(����) Ÿ������ ����� �ݵ�� ȣ���Ͽ� ��ȿ�� �˻� �ʿ�.
	//
	bool bVALID_TAR(true);

	BOOL bCHECK_TAR = GLGaeaServer::GetInstance().ValidCheckTarget(m_pLandMan,m_TargetID);
	if ( !bCHECK_TAR )	bCHECK_TAR = false;

	if ( bCHECK_TAR && m_TargetID.emCrow==CROW_PC )
	{
		PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( m_TargetID.dwID );
		if ( pCHAR )
		{
			// ������ΰ�?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )	bVALID_TAR = false;

            // ĳ���Ͱ� ��������ΰ�? GM �� visible off �Ǵ� none �� �����ϴ�.
			if ( !pCHAR->IsVisibleDetect(m_bRECVISIBLE) )	bVALID_TAR = false;

			// SafeZone �ϰ�� ���� �Ұ�
			if ( pCHAR->IsSafeZone() ) bVALID_TAR = false;

			// ���Ͱ� ������ ������ �ϴ� ��� �����̸� �������� �ʴ´�.
			SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
			if( !(pLevelEtcFunc && pLevelEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE]) )
			{
				//	Note : ������ ������ ����.
				//
				// �ٱⰡ���� ���ΰ�?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// ���� ĳ���� �ֺ��� �����ִ� ���� ����
					DWORD dwAmount = pCHAR->GetARoundSlot().GetAmount();
					// ���� ĳ���� �ֺ��� �����ִ� ���� ���ڰ� ������ �������� ũ�ٸ�...
					if ( dwAmount > GLCONST_CHAR::wMAXMOB_TRACKING )
					{
						// ���� Ÿ�ٰ��� �Ÿ��� ����Ѵ�.
						D3DXVECTOR3 vDist = m_vPos - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);
						
						// ���� �Ÿ��� �� Ʈ��ŷ �Ÿ����� ũ�ٸ�...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// �ۼ� �ִ� ���̶��?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// �پ��
								ToRun();
							}
							
							// ������ġ�� ���ư���.
							MoveTo ( m_vGenPos );
						}
					}
				}
			}
		}
	}

	if ( bCHECK_TAR && m_TargetID.emCrow==CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon( m_TargetID.dwID );		
		PGLCHAR		   pCHAR   = GLGaeaServer::GetInstance().GetChar  ( pSummon->m_dwOwner );
		if ( pCHAR )
		{
			// ������ΰ�?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )	bVALID_TAR = false;

			// ĳ���Ͱ� ��������ΰ�? GM �� visible off �Ǵ� none �� �����ϴ�.
			if ( !pCHAR->IsVisibleDetect(m_bRECVISIBLE) )	bVALID_TAR = false;

			// ���Ͱ� ������ ������ �ϴ� ��� �����̸� �������� �ʴ´�.
			SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
			if( !(pLevelEtcFunc && pLevelEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE]) )
			{
				//	Note : ������ ������ ����.
				//
				// �ٱⰡ���� ���ΰ�?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// ���� ��ȯ�� �ֺ��� �����ִ� ���� ����
					DWORD dwAmount = pSummon->GetARoundSlot().GetAmount();
					// ���� ��ȯ�� �ֺ��� �����ִ� ���� ���ڰ� ������ �������� ũ�ٸ�...
					if ( dwAmount > GLCONST_CHAR::wMAXMOB_TRACKING )
					{
						// ���� Ÿ�ٰ��� �Ÿ��� ����Ѵ�.
						D3DXVECTOR3 vDist = m_vPos - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);

						// ���� �Ÿ��� �� Ʈ��ŷ �Ÿ����� ũ�ٸ�...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// �ۼ� �ִ� ���̶��?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// �پ��
								ToRun();
							}

							// ������ġ�� ���ư���.
							MoveTo ( m_vGenPos );
						}
					}
				}
			}
		}
	}
	
	// Ÿ���� �����󿡼� ��ȿ���� �ʰų� ���̻� ������ �ʿ䰡 ���� PC �� ���
	if ( !bCHECK_TAR || !bVALID_TAR )
	{
		m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;
		
		// Ÿ������ ��ȿȭ�Ѵ�.
		ReSetFLAG(EMTARGET);
		m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);

		// �׷� Ÿ���� ���� �����ϸ� �׷� Ÿ���� �����Ѵ�.
		if( IsGroupMember() )
		{
			if( m_pLandMan->GET_GROUPTARGET( GetGroupName() ).dwID != EMTARGET_NULL )
			{
				NewTarget( m_pLandMan->GET_GROUPTARGET( GetGroupName() ) );
			}
		}
	}


	// ���� �ʾ�����
	if ( IsValidBody() )
	{
		if ( m_dwNowHP==0 )
		{
			//	Note : �������� ���� �߻�. ( ����ġ + ������ + �ݾ� )
			GenerateReward ();

			//	���� ��ȭ.
			TurnAction ( GLAT_FALLING );
		}
		// Ÿ���� ��ȿȭ �Ǿ��µ� ���ݾ׼� ���̸�
		else if ( !IsACTION(GLAT_SHOCK) && !IsACTION(GLAT_PUSHPULL) )
		{
			if ( !IsFLAG(EMTARGET) && IsACTION(GLAT_ATTACK) )
			{
				// ���޻��·� ����
				TurnAction ( GLAT_IDLE, false );
			}

			// Crow �� ���� ���°����� Update �Ѵ�
			GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime );

			if ( !m_bSTATE_STUN )
			{
				//	Note : ������ ����.
				RunSchedule ( fTime, fElapsedTime);
			}
		}
	}

	//	Note : �׼Ǻ� ó���۾�.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		{
			//	Note : �ɸ��� �̵� ������Ʈ.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo() );
			hr = m_actorMove.Update ( fElapsedTime );
			if ( FAILED(hr) )	return E_FAIL;
		
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE, false );
			}

			//	Note : Mob�� ���� ��ġ ������Ʈ.
			//
			m_vPos = m_actorMove.Position();

			//	Note : Mob�� ���� ���� ������Ʈ.
			//
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
			if ( !AttackProcess ( fElapsedTime ) )
			{
				TurnAction ( GLAT_IDLE, false );
			}

			//	Note : ���� �������� ȸ��.
			//
			GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
			if ( pACTOR )
			{
				D3DXVECTOR3 vDirection = pACTOR->GetPosition() - m_vPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			if ( !SkillProcess ( fElapsedTime ) )
			{
				TurnAction ( GLAT_IDLE, false );
			}
		}
		break;

	case GLAT_SHOCK:
		if ( !ShockProcess ( fElapsedTime ) )		TurnAction ( GLAT_IDLE, false );
		break;

	case GLAT_FALLING:
		{
			m_fIdleTime += fElapsedTime;

			if( m_pCrowData->m_sAction.m_bMobLink )
			{
				if ( m_fIdleTime > m_pCrowData->m_sAction.m_fMobLinkDelay )	
				{
					TurnAction ( GLAT_DIE );
					m_pLandMan->DropCrow( 
						m_pMobSchedule, 
						m_pCrowData->m_sAction.m_sMobLinkID, 
						m_vPos.x, 
						m_vPos.z,
						m_pCrowData->m_sAction.m_fMobLinkScale );
				}

			}
			else
			{
				if ( m_fIdleTime > 4.0f )	
				{
					TurnAction ( GLAT_DIE );
				}
			}
		}
		break;

	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE, false );
			}

			//	Note : Mob�� ���� ��ġ ������Ʈ.
			//
			m_vPos = m_actorMove.Position();
		}
		break;

	case GLAT_DIE:
		{
		}
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob�� ���� ��ġ ������Ʈ.
	//
	m_vPos = m_actorMove.Position();

	//	Note : m_pd3dDevice �� ��ȿ�� ��츸 �Ʒ� �ڵ尡 ����ȴ�. ( Level Editor ���� �½�Ʈ��. )
	//
	if ( !m_pd3dDevice )	return S_OK;

	//	Note : ���ϸ��̼�, ��Ų ������Ʈ.
	//
		UpateAnimation( fTime, fElapsedTime );

	//	Note : ���� ��ġ�� �������� Transform ��Ʈ���� ���.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, vDIRORIG );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	return S_OK;
}

HRESULT GLCrow::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	HRESULT hr;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	if ( bRendAABB ) EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	�׽�Ʈ��.
	
	hr = m_pSkinChar->Render ( pd3dDevice, m_matTrans );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

void GLCrow::ReceivePushPull ( const D3DXVECTOR3 &vMovePos )
{
	if ( !IsValidBody () )		return;

	//	Note : �з��� ��ġ�� �̵� �õ�.
	//
	BOOL bSucceed = m_actorMove.GotoLocation
	(
		D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
		D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
	);

	if ( bSucceed )
	{
		//	Note : �з����� ���� ����.
		//
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		//	Note : �и��� �ӵ� ����.
		//
		m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );

		//	Note : �޽��� ���� ����.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow = GETCROW();
		NetMsgBRD.dwID = m_dwGlobID;
		NetMsgBRD.vMovePos = vMovePos;

		//	Note : [�ڽ��� �ֺ� Char] ���� Msg�� ����.
		//
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );
	}
}

// 2006-04-03 Jgkim
DWORD GLCrow::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock )
{
	//	Note : Damage ���� ������. 0 �϶��� miss �߻� ( Ÿ�� ������ ���� ȣ���. )
	if ( m_dwNowHP > 0 )
	{
		//	Note : Damage ����.
		DWORD dwDxHp = GLCROWLOGIC::RECEIVE_DAMAGE ( (WORD)dwDamage );

		//	Damage ���.
		if ( emACrow == CROW_PC )
		{
			PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwAID );
			AddDamageLog      ( m_cDamageLog, dwAID, pChar->GetUserID(), dwDxHp );
			AddDamageLogParty ( m_cDamageLogParty, pChar->m_dwPartyID, dwDxHp );
		}

		//  ��ȯ�� ������ ��� SummonDamageLog�� ����Ѵ�.
		if ( emACrow == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( dwAID );
			if( pSummon )
			{
				AddDamageLog      ( m_cSummonDamageLog, dwAID, pSummon->GetCtrlID(), dwDxHp );
				AddDamageLogParty ( m_cDamageLogParty, pSummon->m_pOwner->m_dwPartyID, dwDxHp );
			}
		}

		//	Note : ��� ����.
		if( !m_pCrowData->IsIgnoreShock() )
		{
			if ( bShock )	
			{
				TurnAction ( GLAT_SHOCK );
			}
		}

		m_sAssault.emCrow = emACrow;
		m_sAssault.dwID = dwAID;
	}

	//	Note :Ÿ���� �������� ������ �����ڰ� ������ Ÿ������ ��� �õ�.
	//		���� Ÿ���� ���� ���� �ۿ� ���� ���� Ÿ�� �ٲ�.
	//
	HRESULT hrESCAPE = S_OK;
	EMCROWACT_UP emCROWACT = GetActPattern();

	if ( EMCROWACT_UP_ESCAPE == emCROWACT )
		hrESCAPE = ESCAPE ( STARGETID(emACrow,dwAID), dwDamage );
	
	if ( EMCROWACT_UP_ESCAPE != emCROWACT || hrESCAPE!=S_OK )
	{
		BOOL bChangeTar = FALSE;
		if ( !IsFLAG(EMTARGET) )											bChangeTar = TRUE;
		else if ( m_pCrowData->IsTarShort() && (!IsFLAG(EMATTACKABLE)) )	bChangeTar = TRUE;

		if ( bChangeTar || CheckGroupAttack() )
		{
			PGLCHAR pTarget = NULL;
			if ( emACrow==CROW_PC )		pTarget = GLGaeaServer::GetInstance().GetChar(dwAID);
			if ( pTarget )
			{
				STARGETID sTARID(CROW_PC,pTarget->GetCtrlID(),pTarget->GetPosition());
				NewTarget ( sTARID, TRUE );
			}
		}
	}

	return m_dwNowHP;
}


// Original Code
/*
DWORD GLCrow::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock )
{
	//	Note : ����� ���� ������. 0 �϶��� miss �߻� ( Ÿ�� ������ ���� ȣ���. )
	//
	if ( m_dwNowHP > 0 )
	{
		//	Note : ����� ����.
		DWORD dwDxHp = GLCROWLOGIC::RECEIVE_DAMAGE ( (WORD)dwDamage );

		//	����� ���.
		if ( emACrow == CROW_PC )
		{
			PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwAID );
			AddDamageLog ( m_cDamageLog, dwAID, pChar->m_dwUserID, dwDxHp );
		}

		//	Note : ��� ����.
		if( !m_pCrowData->IsIgnoreShock() )
		{
			if ( bShock )	
			{
				TurnAction ( GLAT_SHOCK );
			}
		}

		m_sAssault.emCrow = emACrow;
		m_sAssault.dwID = dwAID;
	}

	//	Note :Ÿ���� �������� ������ �����ڰ� ������ Ÿ������ ��� �õ�.
	//		���� Ÿ���� ���� ���� �ۿ� ���� ���� Ÿ�� �ٲ�.
	//
	HRESULT hrESCAPE = S_OK;
	EMCROWACT_UP emCROWACT = GetActPattern();

	if ( EMCROWACT_UP_ESCAPE == emCROWACT )
		hrESCAPE = ESCAPE ( STARGETID(emACrow,dwAID), dwDamage );
	
	if ( EMCROWACT_UP_ESCAPE != emCROWACT || hrESCAPE!=S_OK )
	{
		BOOL bChangeTar = FALSE;
		if ( !IsFLAG(EMTARGET) )											bChangeTar = TRUE;
		else if ( m_pCrowData->IsTarShort() && (!IsFLAG(EMATTACKABLE)) )	bChangeTar = TRUE;

		if ( bChangeTar )
		{
			PGLCHAR pTarget = NULL;
			if ( emACrow==CROW_PC )		pTarget = GLGaeaServer::GetInstance().GetChar(dwAID);
			if ( pTarget )
			{
				STARGETID sTARID(CROW_PC,pTarget->GetCtrlID(),pTarget->GetPosition());
				NewTarget ( sTARID );
			}
		}
	}

	return m_dwNowHP;
}
*/

