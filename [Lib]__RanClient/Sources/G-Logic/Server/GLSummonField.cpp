#include "pch.h"
#include "./GLGaeaserver.h"

#include ".\glsummonfield.h"

GLSummonField::GLSummonField(void) :
m_vDir(D3DXVECTOR3(0,0,-1)),
m_vDirOrig(D3DXVECTOR3(0,0,-1)),
m_vPos(D3DXVECTOR3(0,0,0)),
m_pLandMan(NULL),
m_pLandNode(NULL),

m_vTarPos(D3DXVECTOR3(0,0,0)),
m_pOwner(NULL),

m_Action(GLAT_IDLE),

m_pQuadNode(NULL),
m_pCellNode(NULL),

m_dwActionFlag(0),
m_fTIMER(0.0f),
m_bValid(false),
m_wAniSub(0),

m_fattTIMER(0.0f),

m_pAttackProp(NULL),

m_fIdleTime(0.0f),

m_dwOwnerCharID(0)
{

	m_ARoundSlot.SetSummon ( this );
	m_TargetID.RESET();
	m_sHITARRAY.clear();
}

GLSummonField::~GLSummonField(void)
{
}

HRESULT GLSummonField::Create ( GLLandMan* pLandMan, PGLCHAR pOwner, PGLSUMMON pSummonData )
{
	if ( !pSummonData )	return E_FAIL;
	if ( !pLandMan )	return E_FAIL;
	if ( !pOwner )		return E_FAIL;

	ASSIGN ( *pSummonData );

	SetPosionItem( pOwner->m_sSummonPosionID );


	m_pSummonCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	if( !m_pSummonCrowData )
	{
		DEBUGMSG_WRITE ( _T("GLSummonField::Create, Mob Create Failed. nativeid [%d/%d]"), m_sSummonID.wMainID, m_sSummonID.wSubID );
		return E_FAIL;
	}

	// ��ȯ �ϰ����� ���¸� �ִ밪����
	m_dwNowHP = GETMAXHP();
	m_wNowMP  = GETMAXMP();


	m_Action = GLAT_IDLE;

	m_pOwner   = pOwner;
	m_pLandMan = pLandMan;

	SetValid ();

	//	Note : �� ���� ������ �ʱ�ȭ.
	GLSUMMON::INIT_DATA ();

	// ���� �ֺ�
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos =  GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// ����
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	m_dwOwner = pOwner->m_dwGaeaID;
	m_sMapID  = pOwner->m_sMapID;

	m_dwOwnerCharID = pOwner->m_dwCharID;



	m_actorMove.Create ( m_pLandMan->GetNavi (), m_vPos, -1 );
	m_actorMove.Stop ();

	m_TargetID.RESET();
	m_sHITARRAY.clear();

	m_fattTIMER = 0.0f;

	m_fIdleTime = 0.0f;

	return S_OK;
}

HRESULT GLSummonField::DeleteDeviceObject ()
{
	CleanUp ();
	return S_OK;
}

void GLSummonField::CleanUp ()
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

	m_sHITARRAY.clear();
	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_fattTIMER = 0.0f;

	m_actorMove.Stop ();
	m_actorMove.Release ();

	m_pOwner	  = NULL;
	m_pAttackProp = NULL;

	m_vDir	     = D3DXVECTOR3 (0,0,-1);
	m_vPos	     = D3DXVECTOR3(0,0,0);
	m_vDirOrig   = D3DXVECTOR3(0,0,-1);

	m_bValid		   = false;

	ReSetAllSTATE ();
	SetSTATE ( EM_SUMMONACT_STOP );
	m_Action = GLAT_IDLE;

	m_TargetID.RESET();

	m_fattTIMER    = 0.0f;
	m_fIdleTime    = 0.0f;



	GLSUMMON::RESET_DATA();

	m_Action = GLAT_IDLE;

	m_ARoundSlot.Reset();

	RESET ();
}

HRESULT GLSummonField::SetPosition ( GLLandMan* pLandMan )
{
	if ( !pLandMan )	return E_FAIL;

	m_pLandMan = pLandMan;

	SetValid ();

	// ���� �ֺ�
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos = GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// ����
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	m_sMapID  = m_pOwner->m_sMapID;

	m_actorMove.Create ( m_pLandMan->GetNavi (), m_vPos, -1 );
	m_actorMove.Stop ();

	return S_OK;
}




void GLSummonField::SetSTATE( DWORD dwStateFlag )
{ 

	m_dwActionFlag |= dwStateFlag; 
}

void GLSummonField::TurnAction ( EMACTIONTYPE toAction )
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
		m_fIdleTime = 0.0f;
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		m_fattTIMER = 0.0f;
		PreStrikeProc( FALSE, FALSE );
		break;

	case GLAT_FALLING:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_PUSHPULL:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_DIE:
		break;

	case GLAT_SHOCK:
		m_fIdleTime = 0.0f;
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





HRESULT GLSummonField::UpdateTurnAction( float fElapsedTime )
{
	HRESULT hr(S_OK);

	if( m_TargetID.dwID != EMTARGET_NULL ) 
	{
		GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTARGET )
		{
			m_TargetID.RESET();
			TurnAction( GLAT_IDLE );
			ReSetAllSTATE();
			SetSTATE( EM_SUMMONACT_STOP );
		}else{

			if( !pTARGET->IsValidBody() )
			{
				m_TargetID.RESET();
				TurnAction( GLAT_IDLE );
				ReSetAllSTATE();
				SetSTATE( EM_SUMMONACT_STOP );
			}else{
				D3DXVECTOR3 vDist = pTARGET->GetPosition () - m_vPos;
				float		fDist = D3DXVec3Length( &vDist );

				WORD wAttackRange = pTARGET->GetBodyRadius() + GETSUMMON_BODYRADIUS() + GETSUMMON_ATTACKRANGE() + 2;

				if( fDist <= wAttackRange && !IsACTION( GLAT_ATTACK ) && !IsSTATE(EM_SUMMONACT_TRACING) )
				{
					SetAttackTarget( m_TargetID );
				}	
			}
		}
	}

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
			hr = m_actorMove.Update ( fElapsedTime );
			if ( FAILED(hr) )	return E_FAIL;

			if ( !m_actorMove.PathIsActive() ) m_actorMove.Stop ();

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
			if ( !AttackProcess ( fElapsedTime ) )
			{
				SetSTATE( EM_SUMMONACT_STOP );
				TurnAction( GLAT_IDLE );
				//				m_TargetID.RESET();
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

		//case GLAT_SKILL:
		//	{
		//		if ( !SkillProcess ( fElapsedTime ) )
		//		{
		//			TurnAction ( GLAT_IDLE, false );
		//		}
		//	}
		//	break;

	case GLAT_SHOCK:
		if ( !ShockProcess ( fElapsedTime ) )	
		{
			TurnAction( GLAT_IDLE );
			SetSTATE( EM_SUMMONACT_STOP );
		}
		break;

	case GLAT_FALLING:
		m_fIdleTime += fElapsedTime;
		if ( m_fIdleTime > 4.0f )	TurnAction ( GLAT_DIE );
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

			//	Note : Mob�� ���� ��ġ ������Ʈ.
			//
			m_vPos = m_actorMove.Position();
		}
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	/*if( m_Action != GLAT_ATTACK && m_TargetID.dwID != EMTARGET_NULL ) 
	{
		GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTARGET )
		{
			m_TargetID.RESET();
		}else if( m_Action == GLAT_MOVE )
		{
			D3DXVECTOR3 vDist = pTARGET->GetPosition() - m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if( fDist >= m_fWalkArea )
			{
				m_TargetID.RESET();
			}
		}

	}*/

		return S_OK;
}


void GLSummonField::AutoPosionCheck()
{
	if( m_sPosionID == NATIVEID_NULL() ) return;
	if( (float)m_dwNowHP / (float)GETMAXHP() > 0.3f ) return;

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindItem ( m_sPosionID );
	if ( !pInvenItem )	return;

	if ( m_pOwner->IsCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( m_sPosionID );
	if ( !pItem ) return;

	// ȸ�� ������ ���� üũ
	if ( pItem->sBasicOp.emItemType!=ITEM_CURE ) return;

	if ( pItem->sDrugOp.emDrug != ITEM_DRUG_HP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_CURE && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP_CURE ) return;


	HP_INCREASE( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );


	// �ڽſ��� �˸�
	GLMSG::SNET_SUMMON_UPDATE_HP NetMsg;
	NetMsg.dwHP	= m_dwNowHP;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsg );

	// �ֺ��� �˸�
	GLMSG::SNET_SUMMON_UPDATE_HP_BRD	NetMsgBrd;
	NetMsgBrd.dwGUID = m_dwGUID;
	NetMsgBrd.dwHP	 = m_dwNowHP;

	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBrd );

	m_pOwner->DoDrugInvenItem ( pInvenItem->wPosX, pInvenItem->wPosY );
}


HRESULT GLSummonField::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);
	static float fTimer(0.0f);

	GASSERT(m_dwSummon_AType<SCROWDATA::EMMAXATTACK);
	m_pAttackProp = &(m_pSummonCrowData->m_sCrowAttack[m_dwSummon_AType]);

	// �ڵ� ���� ��� üũ
	AutoPosionCheck();

	if ( IsValidBody () )
	{
		if ( m_dwNowHP!=0 )
		{
			m_ARoundSlot.Update ( m_pLandMan );

			// Summon �� ���� ���°����� Update �Ѵ�
			GLSUMMON::UPDATE_DATA ( fTime, fElapsedTime );
		}else{

			//	���� ��ȭ.

			TurnAction ( GLAT_FALLING );
		}
	}

	UpdateTurnAction( fElapsedTime );

	// ���� ��ġ ������Ʈ
	m_vPos = m_actorMove.Position();

	return S_OK;
}



BOOL GLSummonField::AttackProcess ( float fElapsedTime )
{
	GASSERT(m_pAttackProp);
	m_fattTIMER += fElapsedTime/* * GETATTVELO()*/;

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;
	int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int wThisKey = int(m_fattTIMER*UNITANIKEY_PERSEC);

	if ( GetAttackRangeType()==EMATT_SHORT )
	{
		GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTARGET )
		{
			TurnAction ( GLAT_IDLE );
			SetSTATE( EM_SUMMONACT_STOP );
			m_TargetID.RESET();
			return FALSE;
		}

		D3DXVECTOR3 vDist = m_vPos - pTARGET->GetPosition();
		float fTarLength = D3DXVec3Length(&vDist);

		WORD wAttackRange = pTARGET->GetBodyRadius() + GETSUMMON_BODYRADIUS() + GETSUMMON_ATTACKRANGE() + 2;
		if ( wAttackRange*GLCONST_CHAR::fREACT_VALID_SCALE < (WORD)(fTarLength) )
		{
			TurnAction ( GLAT_IDLE );
			SetSTATE( EM_SUMMONACT_STOP );
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
				dwDamageFlag = GLSUMMON::CALCDAMAGE ( nDamage, m_dwGUID, m_TargetID, m_pLandMan );
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
					dwDamageFlag = GLSUMMON::CALCDAMAGE ( nDamage, m_dwGUID, m_TargetID, m_pLandMan );
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



inline DWORD GLSummonField::ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const BOOL bShock )
{
	GLACTOR* pACTOR_TAR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pACTOR_TAR )	return 0;

	//	Note : Ÿ���� ���� ������ ��쿡 damage �ټ� ����.
	//
	if ( CROW_PC==sTargetID.emCrow && !pACTOR_TAR->IsValidBody() )	return 0;

	return pACTOR_TAR->ReceiveDamage ( GLSUMMON::GETCROW(), m_dwGUID, nDAMAGE, bShock );
}

void GLSummonField::AvoidProc ( )
{
	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	//	Note : �ڽſ���.
	GLMSG::SNET_SUMMON_ATTACK_AVOID NetMsg;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.dwID;

	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsg );
}

void GLSummonField::DamageProc ( const int nDAMAGE, DWORD dwDamageFlag )
{
	//	Note : ����ڿ��� ������� ����.
	bool bShock = ( dwDamageFlag & DAMAGE_TYPE_SHOCK );
	DWORD dwNowHP = ToDamage ( m_TargetID, nDAMAGE, bShock );

	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	//	Note : �ڽſ���.
	GLMSG::SNET_SUMMON_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= m_TargetID.emCrow;
	NetMsg.dwTarID			= m_TargetID.dwID;
	NetMsg.nDamage			= nDAMAGE;
	NetMsg.dwDamageFlag		= dwDamageFlag;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsg );

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
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( m_pAttackProp->fBLOW_RATE*fPOWER, GLSUMMON::GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );
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



BOOL GLSummonField::IsValidBody () const
{
	return ( m_Action < GLAT_FALLING );
}

BOOL GLSummonField::IsDie () const
{
	if ( m_Action==GLAT_DIE )		
		return TRUE;

	return FALSE;
}


void GLSummonField::PreAttackProc ()
{
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = GLSUMMON::CHECKHIT(m_TargetID,m_pLandMan) ? true : false;
		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

void GLSummonField::PreStrikeProc ( BOOL bSkill, BOOL bLowSP )
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
	//else
	//{
	//	//	Note : ��ų ���� ������.
	//	//
	//	PGLSKILL pSkill = NULL;
	//	pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	//	if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
	//	{
	//		//	Note : ��ų Ÿ���� �ϳ��� ��ȿ�ϰ� Ȯ���� �ϰ� ����.
	//		//	NEED : ���� Ÿ���� ��� �ٽ� Ȯ���ؾ���.
	//		sTargetID = STARGETID(static_cast<EMCROW>(m_sTARIDS[0].wCrow),static_cast<DWORD>(m_sTARIDS[0].wID));
	//		bCheckHit = TRUE;
	//	}
	//	else
	//	{
	//		sTargetID.dwID = EMTARGET_NULL;
	//	}
	//}

	//	ť �ʱ�ȭ
	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = true;
		if ( bCheckHit && (sTargetID.dwID!=EMTARGET_NULL) )
		{
			if ( !GLSUMMON::CHECKHIT(sTargetID,m_pLandMan) )		bhit = false;
		}

		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

bool GLSummonField::GetAttackTarget( STARGETID sTargetID ) 
{ 
	if( m_TargetID.dwID != EMTARGET_NULL ) 
	{
		if( GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID ) == NULL )
		{
			m_TargetID.RESET();
			return TRUE;
		}

		if( m_TargetID == sTargetID ) return TRUE;
		else						  return FALSE;
	}

	return TRUE; 
}

DWORD GLSummonField::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock )
{
	if ( !IsValid() )		return	   m_dwNowHP;
	if ( m_dwNowHP == 0 )	return m_dwNowHP;

	//	Note : ���� ������ ���� ����?
	//		( emACrow, dwAID )
	WORD wDxHP = (WORD)GLSUMMON::RECEIVE_DAMAGE ( (WORD) dwDamage );

	//	Note : ��� ����.
	//
	//TurnAction ( GLAT_SHOCK );

	//	Note : ��� ����.
	if( !m_pSummonCrowData->IsIgnoreShock() )
	{
		if ( bShock )	
		{
			//			SetSTATE ( EM_SUMMONACT_SHOCK );
			TurnAction( GLAT_SHOCK );
		}
	}

	m_sAssault.emCrow = emACrow;
	m_sAssault.dwID = dwAID;


	STARGETID sTargetID = STARGETID ( emACrow, dwAID );
	if( !IsSTATE ( EM_SUMMONACT_ATTACK ) && GetAttackTarget( sTargetID ) )
	{
		SetAttackTarget( sTargetID );
	}

	/*if ( emACrow == CROW_PC )
	{
		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwAID );
		AddDamageLog      ( TurnAction, dwAID, pChar->GetUserID(), wDxHP );
		AddDamageLogParty ( m_cDamageLogParty, pChar->m_dwPartyID, wDxHP );
	}*/



		return m_dwNowHP;
}

BOOL GLSummonField::ShockProcess ( float fElapsedTime )
{
	VECANIATTACK &vecShock = m_pSummonCrowData->GetAnimation(AN_SHOCK,AN_SUB_NONE);
	if ( vecShock.empty() )	return FALSE;

	m_fIdleTime += fElapsedTime * GETATTVELO();

	SANIATTACK &sAniShock = vecShock[0];

	int wTotalKeys = int(sAniShock.m_dwETime) - int(sAniShock.m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);

	if ( wThisKey >= wTotalKeys )	return FALSE;

	return TRUE;
}

BOOL GLSummonField::IsOwnerShortDistance( D3DXVECTOR3 vCheckPos )
{

	D3DXVECTOR3 vLeng = m_pOwner->GetPosition() - vCheckPos;
	float fTarLeng1 = D3DXVec3Length(&vLeng);

	vLeng			  = m_vPos - vCheckPos;
	float fTarLeng2 = D3DXVec3Length(&vLeng);

	if( fTarLeng1 > fTarLeng2 )
	{
		return TRUE;
	}

	return FALSE;
}

void GLSummonField::ReceivePushPull ( const D3DXVECTOR3 &vMovePos )
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
		NetMsgBRD.dwID = m_dwGUID;
		NetMsgBRD.vMovePos = vMovePos;

		//	Note : [�ڽ��� �ֺ� Char] ���� Msg�� ����.
		//
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsgBRD );
		SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );
	}
}

void GLSummonField::BUFF_REMOVE( DWORD dwBuffFlag )
{
	GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;

	if ( dwBuffFlag == 0 ) return;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		if ( dwBuffFlag == EMSPECA_BUFF_REMOVE )
		{
			if ( pSkill->m_sBASIC.emIMPACT_SIDE == SIDE_ENERMY )	continue;
		}
		else if ( dwBuffFlag == EMSPECA_DEBUFF_REMOVE )
		{
			if ( pSkill->m_sBASIC.emIMPACT_SIDE == SIDE_OUR )	continue;
		}

		DISABLESKEFF( i );

		NetMsgSkillBrd.bRESET[i] = true;
	}

	NetMsgSkillBrd.dwID = m_dwGUID;
	NetMsgSkillBrd.emCrow = CROW_SUMMON;

	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgSkillBrd );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsgSkillBrd );
}

void GLSummonField::DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR )
{
	//	Note : ����ڿ��� ������� ����.
	DWORD dwNowHP = ToDamage ( sACTOR, nDAMAGE, FALSE );

	//	Note : �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETCROW_DAMAGE NetMsg;
	NetMsg.dwGlobID		= m_dwGUID;
	NetMsg.emTarCrow	= sACTOR.emCrow;
	NetMsg.dwTarID		= sACTOR.dwID;
	NetMsg.nDamage		= nDAMAGE;
	NetMsg.dwNowHP		= dwNowHP;
	NetMsg.dwDamageFlag	= DAMAGE_TYPE_NONE;

	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsg );
}


namespace
{
	typedef std::pair<int,int> PRIOR_PAIR;
	class prior_sort
	{
	public:
		bool operator () ( const PRIOR_PAIR &lvalue, const PRIOR_PAIR &rvalue )
		{
			return lvalue.first > rvalue.first;
		}
	};

	// float g_fMINNEAR(15.0f);
	float g_fMINNEAR(21.0f);
	float g_fMAXNEAR(35.0f);
};

bool GLSummonField::SetAttackTarget( STARGETID sAttackTargetID )
{
	if( !IsValidBody() ) return FALSE;

	GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sAttackTargetID );
	if( !pTARGET ) return FALSE;	

	D3DXVECTOR3 vDist = pTARGET->GetPosition () - m_vPos;
	float		fDist = D3DXVec3Length( &vDist );
	vDist			  = m_pOwner->GetPosition() - pTARGET->GetPosition();
	float  fOnwerDist = D3DXVec3Length( &vDist );

	// ���ΰ� ���� �Ÿ��� �ʹ� �ָ� ���� �ߴ�
	if( fOnwerDist > m_fRunArea ) return FALSE;


	DWORD dwFoundAType = FindOptAType ( sAttackTargetID, false );
	if ( dwFoundAType >= SCROWDATA::EMMAXATTACK )	return FALSE;

	m_TargetID = sAttackTargetID;

	m_dwAType = dwFoundAType;
	m_pAttackProp = &(m_pSummonCrowData->m_sCrowAttack[m_dwAType]);

	ReSetAllSTATE ();
	SetSTATE ( EM_SUMMONACT_ATTACK );
	TurnAction( GLAT_ATTACK );

	GLMSG::SNET_SUMMON_ATTACK NetMsg;
	NetMsg.sTarID  = m_TargetID;
	NetMsg.dwAType = m_dwAType;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsg );

	GLMSG::SNET_SUMMON_ATTACK_BRD NetMsgBRD;
	NetMsgBRD.dwGUID  = m_dwGUID;
	NetMsgBRD.dwAType = m_dwAType;
	NetMsgBRD.sTarID  = m_TargetID;
	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

	STARGETID sTargetID = STARGETID ( CROW_SUMMON, m_dwGUID, m_vPos );

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
	}


	return TRUE;
}


DWORD GLSummonField::FindOptAType ( const STARGETID &sTargetID, bool branvar )
{
	GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pTarget )		return SCROWDATA::EMMAXATTACK;

	bool bTarStateRun = (pTarget->IsRunning()==TRUE);

	D3DXVECTOR3 vDX = m_vPos - pTarget->GetPosition();
	float fLength = D3DXVec3Length ( &vDX );

	//	Note : ������ �켱���� ����.
	bool bprior_near(false);
	if ( g_fMAXNEAR>fLength && !bTarStateRun )	bprior_near = true;

	static std::vector<PRIOR_PAIR> vecPRIOR;
	vecPRIOR.clear ();

	for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
	{
		const SCROWATTACK &sATTACK = m_pSummonCrowData->m_sCrowAttack[i];
		if ( !sATTACK.bUsed )							continue;

		if ( m_fACTIONDELAY[i] > 0.0f )					continue;

		bool bskill(false);
		bool bnear(false);
		if ( sATTACK.skill_id!=SNATIVEID(false) )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sATTACK.skill_id );
			if ( !pSkill )								continue;
			if ( sATTACK.skill_tar == CROWSKTAR_OUR )	continue;

			//	������ �ð�, ���Ǽ�ġ�� Ȯ���Ͽ� ��� �������� ����.
			EMSKILLCHECK emcheck = CHECHSKILL ( DWORD(i) );
			if ( emcheck!=EMSKILL_OK && emcheck!=EMSKILL_NOTSP )	continue;

			bskill = true;

			if ( pSkill->m_sBASIC.wTARRANGE < g_fMAXNEAR )		bnear = true;
			else												bnear = false;
		}
		else
		{
			if ( sATTACK.emAttRgType == EMATT_SHORT )	bnear = true;
			else										bnear = false;
		}

		//	�켱���� ���.
		int nprior(0);

		//	�켱������ ������ ��ġ�� �ణ �ο�. ( ���� ���� )
		if ( branvar )					nprior += int ( (RANDOM_POS) * SCROWDATA::EMMAXATTACK );

		//	��ų�� ��� ������.
		if ( bskill )					nprior += 11;

		//	���� ������ �����Ѱ� ��Ÿ��� �����Ѱ��� ���� �켱 ���� ����.
		if ( bprior_near )
		{
			//	���� �켱.
			if ( bnear )				nprior += 10;
			else						nprior += 5;
		}
		else
		{
			//	��Ÿ� �켱.
			if ( bnear )				nprior += 5;
			else						nprior += 10;
		}

		vecPRIOR.push_back ( std::make_pair(nprior,i) );
	}

	if ( vecPRIOR.empty() )		return SCROWDATA::EMMAXATTACK;

	std::sort ( vecPRIOR.begin(), vecPRIOR.end(), prior_sort() );

	const PRIOR_PAIR &sPRIOR = *vecPRIOR.begin();

	GASSERT ( sPRIOR.second < SCROWDATA::EMMAXATTACK );
	if ( sPRIOR.second >= SCROWDATA::EMMAXATTACK )	return SCROWDATA::EMMAXATTACK;

	return sPRIOR.second;
}