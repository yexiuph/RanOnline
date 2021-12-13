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

	// 소환 하고나서의 상태를 최대값으로
	m_dwNowHP = GETMAXHP();
	m_wNowMP  = GETMAXMP();


	m_Action = GLAT_IDLE;

	m_pOwner   = pOwner;
	m_pLandMan = pLandMan;

	SetValid ();

	//	Note : 몹 로직 데이터 초기화.
	GLSUMMON::INIT_DATA ();

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos =  GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
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

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = m_pOwner->GetPosition ();
	vRandPos = GetRandomPostision ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
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
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
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
			//	Note : 케릭의 이동 업데이트.
			//
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

			//	Note : 공격 방향으로 회전.
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

			//	Note : Mob의 현제 위치 업데이트.
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

	// 회복 아이템 여부 체크
	if ( pItem->sBasicOp.emItemType!=ITEM_CURE ) return;

	if ( pItem->sDrugOp.emDrug != ITEM_DRUG_HP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_CURE && 
		pItem->sDrugOp.emDrug != ITEM_DRUG_HP_MP_SP_CURE ) return;


	HP_INCREASE( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );


	// 자신에게 알림
	GLMSG::SNET_SUMMON_UPDATE_HP NetMsg;
	NetMsg.dwHP	= m_dwNowHP;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsg );

	// 주변에 알림
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

	// 자동 물약 사용 체크
	AutoPosionCheck();

	if ( IsValidBody () )
	{
		if ( m_dwNowHP!=0 )
		{
			m_ARoundSlot.Update ( m_pLandMan );

			// Summon 의 현재 상태값들을 Update 한다
			GLSUMMON::UPDATE_DATA ( fTime, fElapsedTime );
		}else{

			//	행위 변화.

			TurnAction ( GLAT_FALLING );
		}
	}

	UpdateTurnAction( fElapsedTime );

	// 현재 위치 업데이트
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

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
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

				//	큐 데이타가 처리된 경우 삭제한다.
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

	//	Note : 타겟이 정상 상태일 경우에 damage 줄수 있음.
	//
	if ( CROW_PC==sTargetID.emCrow && !pACTOR_TAR->IsValidBody() )	return 0;

	return pACTOR_TAR->ReceiveDamage ( GLSUMMON::GETCROW(), m_dwGUID, nDAMAGE, bShock );
}

void GLSummonField::AvoidProc ( )
{
	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	//	Note : 자신에게.
	GLMSG::SNET_SUMMON_ATTACK_AVOID NetMsg;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.dwID;

	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsg );
}

void GLSummonField::DamageProc ( const int nDAMAGE, DWORD dwDamageFlag )
{
	//	Note : 대상자에게 대미지값 적용.
	bool bShock = ( dwDamageFlag & DAMAGE_TYPE_SHOCK );
	DWORD dwNowHP = ToDamage ( m_TargetID, nDAMAGE, bShock );

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	//	Note : 자신에게.
	GLMSG::SNET_SUMMON_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= m_TargetID.emCrow;
	NetMsg.dwTarID			= m_TargetID.dwID;
	NetMsg.nDamage			= nDAMAGE;
	NetMsg.dwDamageFlag		= dwDamageFlag;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsg );

	GLACTOR* pACTOR = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );

	//	Note : 상태 이상 유발.
	//
	if ( pACTOR && m_pAttackProp->emBLOW_TYPE!=EMBLOW_NONE )
	{
		//	Note : 발생 확율 계산.
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
			//	Note : 상태이상 발생.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = m_pAttackProp->fBLOW_LIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = m_pAttackProp->emBLOW_TYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = m_pAttackProp->fBLOW_VAR1;
			sSTATEBLOW.fSTATE_VAR2 = m_pAttackProp->fBLOW_VAR2;

			pACTOR->STATEBLOW ( sSTATEBLOW );

			//	Note : 상태이상 발생 Msg.
			//
			GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
			NetMsgState.emCrow = m_TargetID.emCrow;
			NetMsgState.dwID = m_TargetID.dwID;
			NetMsgState.emBLOW = sSTATEBLOW.emBLOW;
			NetMsgState.fAGE = fLIFE;
			NetMsgState.fSTATE_VAR1 = sSTATEBLOW.fSTATE_VAR1;
			NetMsgState.fSTATE_VAR2 = sSTATEBLOW.fSTATE_VAR2;

			//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

			//	Note : 스킬 대상에게 메세지 전송.
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
	//	//	Note : 스킬 정보 가져옴.
	//	//
	//	PGLSKILL pSkill = NULL;
	//	pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	//	if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
	//	{
	//		//	Note : 스킬 타겟을 하나만 유효하게 확인을 하고 있음.
	//		//	NEED : 다중 타겟일 경우 다시 확인해야함.
	//		sTargetID = STARGETID(static_cast<EMCROW>(m_sTARIDS[0].wCrow),static_cast<DWORD>(m_sTARIDS[0].wID));
	//		bCheckHit = TRUE;
	//	}
	//	else
	//	{
	//		sTargetID.dwID = EMTARGET_NULL;
	//	}
	//}

	//	큐 초기화
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

	//	Note : 공격 받을때 반응 감안?
	//		( emACrow, dwAID )
	WORD wDxHP = (WORD)GLSUMMON::RECEIVE_DAMAGE ( (WORD) dwDamage );

	//	Note : 충격 받음.
	//
	//TurnAction ( GLAT_SHOCK );

	//	Note : 충격 받음.
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
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		//	Note : 밀리는 속도 설정.
		//
		m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );

		//	Note : 메시지 내용 설정.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow = GETCROW();
		NetMsgBRD.dwID = m_dwGUID;
		NetMsgBRD.vMovePos = vMovePos;

		//	Note : [자신의 주변 Char] 에게 Msg를 전달.
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
	//	Note : 대상자에게 대미지값 적용.
	DWORD dwNowHP = ToDamage ( sACTOR, nDAMAGE, FALSE );

	//	Note : 주변 클라이언트들에게 메세지 전송.
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

	// 주인과 몹의 거리가 너무 멀면 공격 중단
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
			// PET Attack 처리
			PGLPETFIELD pEnemyPet = GLGaeaServer::GetInstance().GetPET ( pCHAR->m_dwPetGUID );
			if ( pEnemyPet && pEnemyPet->IsValid () && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
			{
				D3DXVECTOR3 vOwnerPos, vDist;
				float fDist;
				vOwnerPos = pCHAR->GetPosition ();
				vDist = pEnemyPet->m_vPos - vOwnerPos;
				fDist = D3DXVec3Length(&vDist);

				// 일정거리 안에 있으면 
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
			// Summon Attack 처리
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

	//	Note : 근접이 우선인지 점검.
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

			//	딜래이 시간, 조건수치를 확인하여 사용 가능한지 점검.
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

		//	우선순위 계산.
		int nprior(0);

		//	우선순위에 랜덤성 수치를 약간 부여. ( 랜덤 선택 )
		if ( branvar )					nprior += int ( (RANDOM_POS) * SCROWDATA::EMMAXATTACK );

		//	스킬의 경우 가산점.
		if ( bskill )					nprior += 11;

		//	현제 근접이 유리한가 장거리가 유리한가에 따라 우선 순위 변경.
		if ( bprior_near )
		{
			//	근접 우선.
			if ( bnear )				nprior += 10;
			else						nprior += 5;
		}
		else
		{
			//	장거리 우선.
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