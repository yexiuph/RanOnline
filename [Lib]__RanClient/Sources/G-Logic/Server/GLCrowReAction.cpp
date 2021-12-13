#include "pch.h"
#include "./GLCrow.h"
#include "./GLGaeaServer.h"
#include "./stl_func.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

STARGETID GLCrow::FindEnemyChar ()
{
	STARGETID targetID;
	targetID.RESET();

	if ( !m_pLandMan )	return targetID;

	static ARRAY_CHAR vCharArray;
	vCharArray.clear();

	EMCROWACT_UP emACT_UP = GetActPattern();
	EMATT_RGTYPE emAttRgType = GetAttackRangeType();
	int nRange = (int) m_pCrowData->m_wViewRange;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = GLGaeaServer::GetInstance().GetChar ( dwGAEAID );
		if ( !pChar )											continue;

		if ( pChar->IsSafeTime() )								continue;
		if ( pChar->IsSafeZone() )								continue;
		if ( !pChar->IsValidBody() )							continue;
		if ( !pChar->IsVisibleDetect(m_bRECVISIBLE) )			continue;

		//	대련시에는 몹이 케릭터를 선공하지 않음.
		if ( pChar->GETCONFTING()->IsCONFRONTING() )			continue;

		BOOL bTarget = FALSE;
		float fTarLeng = FLT_MAX;
		if ( emAttRgType==EMATT_SHORT )
		{
			//	Note : 근거리 공격일 경우.
			if ( pChar->IsFreeARSlot() )
			{
				D3DXVECTOR3 vLengA = pChar->GetPosition() - m_vPos;
				fTarLeng = D3DXVec3Length(&vLengA);
				if ( fTarLeng <= nRange )		bTarget = TRUE;
					
			}
		}
		else
		{
			//	Note : 장거리 공격일 경우.
			D3DXVECTOR3 vLengA = pChar->GetPosition() - m_vPos;
			fTarLeng = D3DXVec3Length(&vLengA);
			if ( fTarLeng <= nRange )			bTarget = TRUE;
		}

		if ( !bTarget )											continue;

		switch ( emACT_UP )
		{
		default:	break;
		case EMCROWACT_UP_LOWLEVEL:
			if ( pChar->GETLEVEL() > GETLEVEL() )				bTarget = FALSE;
			break;
		case EMCROWACT_UP_LOWHP:
			if ( pChar->GETHP() > GETHP() )						bTarget = FALSE;
			break;

		case EMCROWACT_UP_BRIGHT:
			if ( pChar->GETBRIGHT() != BRIGHT_LIGHT )			bTarget = FALSE;
			break;

		case EMCROWACT_UP_DARK:
			if ( pChar->GETBRIGHT() != BRIGHT_DARK )			bTarget = FALSE;
			break;

		case EMCROWACT_UP_BLOW:
			if ( !pChar->ISSTATEBLOW() )						bTarget = FALSE;
			break;

		case EMCROWACT_UP_ARMER:
			{
				EMSLOT emRHand = pChar->GetCurRHand();
				if ( !pChar->VALID_SLOT_ITEM(emRHand) )			bTarget = FALSE;
			}
			break;

		case EMCROWACT_UP_RUNNER:
			if ( !pChar->IsRunning() )							bTarget = FALSE;
			break;
		};

		if ( !bTarget )		continue;


		vCharArray.push_back ( std_afunc::CHARDIST(fTarLeng,pChar) );
	}

	if ( !vCharArray.empty() )
	{
		std_afunc::CCompareCharDist Comp;
		std::sort ( vCharArray.begin(), vCharArray.end(), Comp );

		const std_afunc::CHARDIST &Target = *vCharArray.begin();

		if( Target.pChar )
		{
			PGLSUMMONFIELD pEnemySummon = GLGaeaServer::GetInstance().GetSummon ( Target.pChar->m_dwSummonGUID );
			if( pEnemySummon && pEnemySummon->IsValid () )
			{
				D3DXVECTOR3 vLeng = Target.pChar->GetPosition() - m_vPos;
				float fTarLeng1 = D3DXVec3Length(&vLeng);

				vLeng			  = pEnemySummon->GetPosition() - m_vPos;
				float fTarLeng2 = D3DXVec3Length(&vLeng);

				if( fTarLeng1 > fTarLeng2 )
				{
					targetID = STARGETID( CROW_SUMMON, pEnemySummon->m_dwGUID, pEnemySummon->GetPosition() );
				}else{
					targetID = STARGETID( CROW_PC, Target.pChar->m_dwGaeaID, Target.pChar->GetPosition() );
				}

			}else{
				targetID = STARGETID( CROW_PC, Target.pChar->m_dwGaeaID, Target.pChar->GetPosition() );
			}			 
		}
	}

	return targetID;
}


VEC_SK_TAR GLCrow::DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind )
{
	VEC_SK_TAR vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = GLGaeaServer::GetInstance().GetChar ( dwGAEAID );
		if ( !pChar )				continue;
		if ( pChar->IsSafeTime() )	continue;
		if ( pChar->IsSafeZone() )	continue;
		if ( pChar->GETCONFTING()->IsCONFRONTING() ) continue;
		
		vPos = pChar->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetCtrlID(), pChar->GetPosition() ) );
		}

		// 소환수가 범위안에 있을경우에는 소환수도 스킬 타겟 리스트에 추가한다.
		PGLSUMMONFIELD pMySummon = GLGaeaServer::GetInstance().GetSummon( pChar->m_dwSummonGUID );
		if ( pMySummon && pMySummon->IsValid() )
		{
			vPos = pMySummon->GetPosition();
			vMax = vPos + D3DXVECTOR3(8,20,8);
			vMin = vPos + D3DXVECTOR3(-8,0,-8);

			if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
			{
				vecTargetID.push_back( STARGETID(pMySummon->GetCrow(),  pMySummon->GetCtrlID(), pMySummon->GetPosition() ) );
			}
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;

	std_afunc::CCompareTargetDist Comp(D3DXVECTOR3(0.0f,0.0f,0.0f));
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );
	if( vecTargetID.size() > dwMaxFind )
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );

	return vecTargetID;
}


VEC_SK_TAR GLCrow::DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
{
	VEC_SK_TAR vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = GLGaeaServer::GetInstance().GetChar ( dwGAEAID );
		if ( !pChar )				continue;
		if ( pChar->IsSafeTime() )	continue;
		if ( pChar->IsSafeZone() )	continue;
		if ( pChar->GETCONFTING()->IsCONFRONTING() ) continue;
		
		D3DXVECTOR3 vLengA = pChar->GetPosition() - vDetectPos;
		fTarLeng = D3DXVec3Length(&vLengA);
		if ( fTarLeng <= nRange )
		{
			vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetCtrlID(), pChar->GetPosition() ) );
		}

		// 소환수가 범위안에 있을경우에는 소환수도 스킬 타겟 리스트에 추가한다.
		PGLSUMMONFIELD pMySummon = GLGaeaServer::GetInstance().GetSummon( pChar->m_dwSummonGUID );
		if ( pMySummon && pMySummon->IsValid() )
		{
			D3DXVECTOR3 vLengA = pMySummon->GetPosition() - vDetectPos;
			fTarLeng = D3DXVec3Length(&vLengA);
			if ( fTarLeng <= nRange )
			{
				vecTargetID.push_back( STARGETID(pMySummon->GetCrow(),  pMySummon->GetCtrlID(), pMySummon->GetPosition() ) );
			}		
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;

	std_afunc::CCompareTargetDist Comp(D3DXVECTOR3(0.0f,0.0f,0.0f));
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );
	if( vecTargetID.size() > dwMaxFind )
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );

	return vecTargetID;
}

void GLCrow::RunNextAct ()
{
	if ( m_pCrowData->IsPosHold() )		return;

	if ( m_pCurAct )
	{
		const SMOBACTION &MobAction = m_pCurAct->Data;

		switch ( MobAction.emAction )
		{
		case EMACTION_MOVE:
			ToWalk();
			MoveTo ( MobAction.vPos );
			break;

		case EMACTION_ACTSTAY:
			{
				SetFLAG(EMSTAY);
				m_fStayTimer = MobAction.fLife;
			}
			break;
		};

		if ( IsFLAG(EMPREORDER) )
		{
			if ( !m_pCurAct->pPrev )	ReSetFLAG(EMPREORDER);

			if ( m_pCurAct->pPrev )		m_pCurAct = m_pCurAct->pPrev;
			else						m_pCurAct = m_pCurAct->pNext;
		}
		else
		{
			if ( !m_pCurAct->pNext )	SetFLAG(EMPREORDER);

			if ( m_pCurAct->pNext )		m_pCurAct = m_pCurAct->pNext;
			else						m_pCurAct = m_pCurAct->pPrev;
		}
	}
	else
	{
		//	Note : 임의의 위치에서 idle 상태에 있는 몹은 자기 위치로 다시 돌아간다.
		//		맵 입구에 몹들을 몰아 놓아서 다른 플래이어에게 피해를 주는 행위 방지용.
		//
		D3DXVECTOR3 vDist = m_vPos - m_vGenPos;
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_CHAR::fMOB_TRACING )
		{
			if ( m_pCrowData->m_sAction.m_bRun )		ToRun();
			MoveTo ( m_vGenPos );
		}
	}
}

BOOL GLCrow::CheckGroupAttack()
{
	// 그룹 리더일 경우 타겟이 죽기 전까진 타겟을 바꾸지 않는다.
	if( IsGroupLeader() )
	{
		// 현재 그룹 타겟이 존재하는지 체크 
		if( m_pLandMan->GET_GROUPTARGET( GetGroupName() ).dwID == EMTARGET_NULL )
		{
			if ( GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID ) ) return TRUE;
		}else{
			return FALSE;
		}
	}else{
		// 만약 그룹 리더가 타겟이 없을 경우
		PGLCROW pGLLeader = m_pLandMan->GET_GROUPLEADER(GetGroupName());
		if( pGLLeader && pGLLeader->GetTargetID().dwID == EMTARGET_NULL ) return TRUE;
	}

	return FALSE;

}

void GLCrow::GroupAttackProcess( const STARGETID sTargetID  )
{
	if( !IsGroupMember() ) return;

	// 그룹 리더일 경우 해당 타겟에 전부다 공격을 한다
	if( IsGroupLeader() )
	{

		// 현재 그룹 타겟이 존재하는지 체크 
		if( m_pLandMan->GET_GROUPTARGET( GetGroupName() ).dwID == EMTARGET_NULL )
		{
			// 그룹 타겟에 등록을 한다.
			m_pLandMan->SET_GROUPTARGET( GetGroupName(), sTargetID );

			SETGROUPMEM setGroupMember		= m_pLandMan->GET_GROUPMEMBER(GetGroupName());
			SETGROUPMEM_ITER setMember_iter = setGroupMember.begin();
			for( ; setMember_iter != setGroupMember.end(); ++setMember_iter )
			{
				// 멤버가 아니면 넘어간다.
				if( !m_pLandMan->IS_GROUPMEMBER(GetGroupName(), *setMember_iter ) ) continue;
				// 자신이면 넘어간다.
				if( m_dwGlobID == *setMember_iter ) continue;
				PGLCROW pGLCrow = m_pLandMan->GetCrow(*setMember_iter);
				if( !pGLCrow ) continue;
				
				pGLCrow->NewTarget( sTargetID );
			}
		}
	}else{
		PGLCROW pGLLeader = m_pLandMan->GET_GROUPLEADER(GetGroupName());
		if( pGLLeader && pGLLeader->GetTargetID().dwID == EMTARGET_NULL )
		{
			pGLLeader->NewTarget( sTargetID );
		}
	}
}

void GLCrow::NewTarget( const STARGETID &sTargetID, BOOL bCheckMember /* = FALSE */ )
{
	
	GLACTOR* pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pTarget )					return;

	m_vOrgTarPos = m_vPos;

	//	Note : 타겟으로 지정.
	m_TargetID.emCrow = sTargetID.emCrow;
	m_TargetID.dwID = pTarget->GetCtrlID();
	m_TargetID.vPos = pTarget->GetPosition();

	SetFLAG(EMTARGET);

	if(  bCheckMember ) GroupAttackProcess(sTargetID);
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

DWORD GLCrow::FindOptAType ( const STARGETID &sTargetID, bool branvar )
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

	int nIndex = IsLowerHP();
	if( nIndex >= 0 )
	{
		if( m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_dwPatternAttackSet < SCROWDATA::EMMAXATTACK )
			return ( DWORD )m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_dwPatternAttackSet;
	}

	for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
	{
		const SCROWATTACK &sATTACK = m_pCrowData->m_sCrowAttack[i];
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



void GLCrow::ReAction ( const STARGETID &sTargetID, float fElapsedTime )
{
	DWORD dwFoundAType = FindOptAType ( sTargetID, false );
	if ( dwFoundAType >= SCROWDATA::EMMAXATTACK )	return;

	m_dwAType = dwFoundAType;
	m_pAttackProp = &(m_pCrowData->m_sCrowAttack[m_dwAType]);

	if ( m_pAttackProp->skill_id==SNATIVEID(false) )
	{
		NewTarget ( sTargetID );
		m_emReAction = EMREACT_ATTACK;		
	}
	else
	{
		NewTarget ( sTargetID );
		SETACTIVESKILL ( m_pAttackProp->skill_id, m_pAttackProp->skill_lev );
		m_emReAction = EMREACT_SKILL;
	}
}

void GLCrow::ResetAction ()
{
	m_emReAction = EMREACT_NUNE;

	ReSetFLAG(EMTARGET);
	ReSetFLAG(EMTRACING);
}

bool GLCrow::FindLongTracePos ( const D3DXVECTOR3 &vCur, const D3DXVECTOR3 &vTar, const float fMin, const float fMax, D3DXVECTOR3 &vTracePos )
{
	GASSERT(fMin<=fMax);

	D3DXVECTOR3 vDir = vCur - vTar;	// ( tar to cur direction )
	D3DXVECTOR3 vNDir;
	D3DXVec3Normalize(&vNDir, &vDir);
	float fLength = D3DXVec3Length(&vDir);

	float fTestTrace(fMax);
	if ( fLength < fMin )
	{
		fTestTrace = fMin * 1.2f;
		if ( fTestTrace >= fMax )	fTestTrace = fMin;
	}
	else
	{
		fTestTrace = fMax * 0.85f;
		if ( fTestTrace <= fMin )	fTestTrace = fMax;
	}

	D3DXMATRIX matRotY;
	const int nMaxCircular = 9;
	const float fCircular[nMaxCircular] = { 0, 5, -5, 10, -10, 15, -15, 20, -20 };
	const D3DXVECTOR3 vMarkPos = vNDir * fTestTrace;

	NavigationMesh* pNaviMesh = m_pLandMan->GetNavi ();

	D3DXVECTOR3 vTarCol(vTar.x,vTar.y+15.0f,vTar.z);
	for ( int i=0; i<nMaxCircular; ++i )
	{
		float fRadian = fCircular[i] / fTestTrace;
		D3DXMatrixRotationY ( &matRotY, fRadian );
		
		D3DXVECTOR3 vTestPos;
		D3DXVec3TransformCoord ( &vTestPos, &vMarkPos, &matRotY );
		vTestPos = vTar + vTestPos;

		//	유효한 위치인지 점검.
		BOOL bNaviCo(FALSE);
		DWORD dwCoID(0);
		D3DXVECTOR3 vNaviCo(0,0,0);
		D3DXVECTOR3 vPos1(vTestPos.x,vTestPos.y+5.0f,vTestPos.z);
		D3DXVECTOR3 vPos2(vTestPos.x,vTestPos.y-5.0f,vTestPos.z);

		pNaviMesh->IsCollision ( vPos1, vPos2, vNaviCo, dwCoID, bNaviCo );
		if ( !bNaviCo )	continue;

		D3DXVECTOR3 vSuitable(vTestPos.x,vTestPos.y+15.0f,vTestPos.z);
		BOOL bCollision = m_pLandMan->IsCollision( vSuitable, vTarCol, FALSE );
		if ( bCollision )	continue;

		vTracePos = vTestPos;
		return true;
	}

	vTracePos = vTar;

	return true;
}

bool GLCrow::DoShortTracing ( const WORD wTARRANGE, const float fElapsedTime )
{
	//	Note : 타겟의 위치 정보를 가져옴.
	//		(주의) 반드시 타겟이 존재한다는 가정하에 동작.
	//
	GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
	if ( !pTarget )		return false;

	GLARoundSlot* pARoundSlot = pTarget->GETSLOT ();
	GASSERT ( pARoundSlot && "glchar 만이 근접 위치 슬롯을 가지고 있습니다." );

	WORD wTarBodyRadius(0);
	D3DXVECTOR3 vTarPos(0,0,0);
	D3DXVECTOR3 vTarDist(0,0,0);
	float fDist(0.0f);

	vTarPos = pTarget->GetPosition();
	wTarBodyRadius = pTarget->GetBodyRadius();

	vTarDist = m_vPos-vTarPos;
	fDist = D3DXVec3Length ( &vTarDist );

	
	//	Note : 거리에 따른 타겟 리샛 검사.
	//
	if ( fDist < GLCONST_CHAR::fMOB_TRACING )
	{
		SetFLAG(EMTARGET);
	}
	else
	{
		SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
		if( pLevelEtcFunc && pLevelEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] )
		{
			SetFLAG(EMTARGET);
		}else{
			pARoundSlot->ResetTargetOptSlot ( m_dwGlobID, m_dwAttPosSlot );
			m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;

			ReSetFLAG(EMTARGET);
		}		
	}

	

	//	Note : 타겟을 노치는 경우.
	if ( !IsFLAG(EMTARGET) )
	{
		ReSetFLAG(EMTRACING);
		return false;
	}

	//	Note : 즉시 사용 가능 거리에 있는지 검사.
	//
	m_fStayTimer -= fElapsedTime;

	WORD wActionAble = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE + 3;
	WORD wActionAbleMin = static_cast<WORD> ( (wTarBodyRadius+GETBODYRADIUS())*0.4f );
	if ( fDist<wActionAble && fDist>wActionAbleMin )
	{
		SetFLAG(EMATTACKABLE);
	  	m_fStayTimer = 0.0f;
	}
	else
	{
		ReSetFLAG(EMATTACKABLE);
	}

	//	Note : 위치 고정형 몹일 경우.
	if ( m_pCrowData->IsPosHold() )
	{
		if ( fDist<wActionAble )
		{
			m_fStayTimer = 0.0f;
			SetFLAG(EMATTACKABLE);
			return true;
		}

		return false;
	}

	//	Note : '추적중' 상태를 리샛할지 검사.
	//
	if ( IsFLAG(EMTRACING) )
	{
		WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;
		WORD wTracingRange = WORD ( wActionRange * 1.2f );

		D3DXVECTOR3 vEndDist = m_TargetID.vPos-vTarPos;
		float fEndDist = D3DXVec3Length ( &vEndDist );

		//	이동이 끝낫거나 추적 위치랑 타갯위치가 너무 떨어졌을 경우 "추적중 상태"를 리샛함.
		//
		if ( !IsACTION(GLAT_MOVE) || fEndDist>wTracingRange )
		{
			ReSetFLAG(EMTRACING);	//	'추적중' 상태 리샛.
		}
		else
		{
			//	타겟이 있을 때에는 타겟 주변슬롯이 종전 설정 슬롯보다 최적 조건인 슬롯이 있는지 점검.
			//
			if ( !pTarget->IsAction(GLAT_MOVE) )
			{
				D3DXVECTOR3 vNewTracePos;
				DWORD dwAttPosSlot = pARoundSlot->TestOptimumSlot ( this, wActionRange, m_pLandMan->GetNavi(), vNewTracePos );
				if ( GLARoundSlot::SLOT_SIZE!=dwAttPosSlot )
				{
					BOOL bValidSlot = pARoundSlot->ValidOptimumSlot ( m_pLandMan->GetNavi(), vNewTracePos );
					if ( bValidSlot )
					{
						D3DXVECTOR3 vOldTraceDist = m_TargetID.vPos - m_vPos;
						float fOldTraceDist = D3DXVec3LengthSq ( &vOldTraceDist );

						D3DXVECTOR3 vNewTraceDist = vNewTracePos - m_vPos;
						float fNewTraceDist = D3DXVec3LengthSq ( &vNewTraceDist );

						//	다른 최적 슬롯이 있을때.
						//
						if ( fNewTraceDist < fOldTraceDist )
						{
							m_dwAttPosSlot = dwAttPosSlot;
							pARoundSlot->SetTargetOptSlot ( m_dwGlobID, dwAttPosSlot );

							//	Note : '추격' 시작.
							if ( m_pCrowData->m_sAction.m_bRun )	ToRun();
							MoveTo ( vNewTracePos );

							//	다음 '추격' 시간 유격 지정.
							m_fStayTimer = RANDOM_POS * 0.3f;
						}
					}
				}
			}
		}
	}

	//	Note : '추적'상태가 아닐 경우 추격 여부를 결정함.
	//
	if ( !IsFLAG(EMTRACING) )
	{
		//	Note : '추격' 여부 결정.
		BOOL bToTrace = FALSE;
		if ( !IsFLAG(EMATTACKABLE) && m_fStayTimer<=0.0f )
		{
			bToTrace = TRUE;
		}

		if( fDist < GetBodyRadius() && m_pCrowData->m_bOverlapAttack )
		{
			bToTrace = FALSE;
			SetFLAG(EMATTACKABLE);
			m_fStayTimer = 0.0f;
		}
		
		//	Note : '추격' 하기 위해서 이동.
		//
		if ( bToTrace )
		{
			WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;

			D3DXVECTOR3 vTracePos;
			pARoundSlot->ResetTargetOptSlot ( m_dwGlobID, m_dwAttPosSlot );

			m_dwAttPosSlot = pARoundSlot->ReqOptimumSlot ( this, wActionRange, m_pLandMan->GetNavi(), vTracePos );

			SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
			if ( GLARoundSlot::SLOT_SIZE!=m_dwAttPosSlot || ( pLevelEtcFunc && pLevelEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] ) )
			{
				//	Note : '추격' 시작.
				SetFLAG(EMTRACING);
				
				if ( m_pCrowData->m_sAction.m_bRun )	ToRun();
				MoveTo ( vTracePos );

				//	다음 '추격' 시간 유격 지정.
				m_fStayTimer = RANDOM_POS * 0.3f;
			}
			else
			{
				ReSetFLAG(EMTARGET);
				ReSetFLAG(EMTRACING);
			}
		}
	}

	return true;
}

bool GLCrow::DoLongTracing ( const WORD wTARRANGE, const float fElapsedTime )
{
	//	Note : 타겟의 위치 정보를 가져옴.
	//		(주의) 반드시 타겟이 존재한다는 가정하에 동작.
	//
	GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
	if ( !pTarget )		return false;

	WORD wTarBodyRadius(0);
	D3DXVECTOR3 vTarPos(0,0,0);
	D3DXVECTOR3 vTarDist(0,0,0);
	float fDist(0.0f);

	vTarPos = pTarget->GetPosition();
	wTarBodyRadius = pTarget->GetBodyRadius();

	vTarDist = m_vPos-vTarPos;
	fDist = D3DXVec3Length ( &vTarDist );

	//	Note : 거리에 따른 타겟 리샛 검사.
	//

	
	if ( fDist < GLCONST_CHAR::fMOB_TRACING )
	{
		SetFLAG(EMTARGET);
	}
	else
	{
		SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
		if( pLevelEtcFunc && pLevelEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] )
		{
			SetFLAG(EMTARGET);
		}else{
			ReSetFLAG(EMTARGET);
		}
	}

	//	Note : 타겟을 놓치는 경우.
	if ( !IsFLAG(EMTARGET) )
	{
		ReSetFLAG(EMTRACING);
		return false;
	}

	//	Note : 즉시 사용 가능 거리에 있는지 검사.
	//
	m_fStayTimer -= fElapsedTime;

	WORD wActionAble = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE + 3;
	WORD wActionAbleMin = static_cast<WORD> ( (wTarBodyRadius+GETBODYRADIUS()) ); //*0.4f
	if ( fDist<wActionAble && fDist>wActionAbleMin )
	{
		SetFLAG(EMATTACKABLE);
	  	m_fStayTimer = 0.0f;
	}
	else
	{
		ReSetFLAG(EMATTACKABLE);
	}

	//	Note : 위치 고정형 몹일 경우.
	if ( m_pCrowData->IsPosHold() )
	{
		if ( fDist<wActionAble )
		{
			m_fStayTimer = 0.0f;
			SetFLAG(EMATTACKABLE);
			return true;
		}

		return false;
	}

	//	Note : '추적중' 상태를 리샛할지 검사.
	//
	if ( IsFLAG(EMTRACING) )
	{
		WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;
		WORD wTracingRange = WORD ( wActionRange * 1.2f );

		D3DXVECTOR3 vEndDist = m_TargetID.vPos-vTarPos;
		float fEndDist = D3DXVec3Length ( &vEndDist );

		//	이동이 끝낫거나 추적 위치랑 타갯위치가 너무 떨어졌을 경우 "추적중 상태"를 리샛함.
		//
		if ( !IsACTION(GLAT_MOVE) || fEndDist>wTracingRange || fEndDist<wActionAbleMin )
		{
			ReSetFLAG(EMTRACING);	//	'추적중' 상태 리샛.
		}
	}

	//	Note : '추적'상태가 아닐 경우 추격 여부를 결정함.
	//
	if ( !IsFLAG(EMTRACING) )
	{
		//	Note : '추적' 여부 결정.
		BOOL bToTrace = FALSE;
		if ( !IsFLAG(EMATTACKABLE) && m_fStayTimer<=0.0f )
		{
			bToTrace = TRUE;
		}

		if( fDist < GetBodyRadius() && m_pCrowData->m_bOverlapAttack )
		{
			bToTrace = FALSE;
			SetFLAG(EMATTACKABLE);
			m_fStayTimer = 0.0f;
		}
		
		//	Note : '추격' 하기 위해서 이동.
		//
		if ( bToTrace )
		{
			WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;

			float fMin = (float) ( wTarBodyRadius + GETBODYRADIUS() );
			float fMax = (float) ( wTarBodyRadius + GETBODYRADIUS() + wTARRANGE );

			D3DXVECTOR3 vTracePos;
			bool btacepos = FindLongTracePos ( m_vPos, vTarPos, fMin, fMax, vTracePos );
			if ( !btacepos )
			{
				ReSetFLAG(EMTARGET);
				ReSetFLAG(EMTRACING);
				return false;
			}

			//	Note : '추격' 시작.
			SetFLAG(EMTRACING);
			
			if ( m_pCrowData->m_sAction.m_bRun )	ToRun();
			MoveTo ( vTracePos );

			//	다음 '추격' 시간 유격 지정.
			m_fStayTimer = RANDOM_POS * 0.3f;
		}
	}

	return true;
}

static int nSkillReActionError = 0;

bool GLCrow::SkillReAction ( float fTime, float fElapsedTime )
{
	if ( !VALIDSKILL() )				return false;

	DWORD dwFoundAType = FindOptAType ( m_TargetID, false );
	if ( dwFoundAType != m_dwAType )
	{
		ReAction ( m_TargetID );
		return false;
	}

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )				return false;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[m_wACTIVESKILL_LVL];
	EMIMPACT_TAR emIMPACT_TAR = pSkill->m_sBASIC.emIMPACT_TAR;
	WORD wTARRANGE = pSkill->m_sBASIC.wTARRANGE;

	if ( emIMPACT_TAR!=TAR_SPEC && emIMPACT_TAR!=TAR_SELF_TOSPEC )
	{
		if ( nSkillReActionError < 10 )
		{
		CDebugSet::ToLogFile(
			_T("GLCrow::SkillReAction %d %d"),
			m_idACTIVESKILL.wMainID,
			m_idACTIVESKILL.wSubID );
			nSkillReActionError++;
		}
		// GASSERT(0&&"Can't Act Skill");
	}

	//	Note : tracing
	//
	bool bShortTrace(false);
	if ( wTARRANGE < g_fMINNEAR )	bShortTrace = true;

	bool btrace(false);
	if ( bShortTrace )
	{
		btrace = DoShortTracing ( wTARRANGE, fElapsedTime );
	}
	else
	{
		btrace = DoLongTracing ( wTARRANGE, fElapsedTime );
	}

	if ( !btrace )
	{
		ResetAction ();
		return false;
	}

	//	Note : 스킬 사용 여부 결정.
	//
	BOOL bactionable(FALSE);
	if ( bShortTrace )		bactionable = IsFLAG(EMATTACKABLE) && !IsFLAG(EMTRACING);
	else					bactionable = IsFLAG(EMATTACKABLE);

	if ( bactionable )
	{
		//	Note : 시야가 확보되는가?
		//
		GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTarget )
		{
			ResetAction ();
			return false;
		}
		
		D3DXVECTOR3 vFinderPos(m_vPos.x,m_vPos.y+15.0f,m_vPos.z);
		D3DXVECTOR3 vTargetPos(pTarget->GetPosition());
		vTargetPos.y += 15.0f;
		
		BOOL bCollision = m_pLandMan->IsCollision( vFinderPos, vTargetPos, FALSE );
		if ( bCollision )
		{
			ResetAction ();
			return false;
		}

		PreSkillProc ();

		m_emReAction = EMREACT_NUNE;
	}

	return true;
}

bool GLCrow::AttackReAction ( float fTime, float fElapsedTime )
{
	if ( !VALIDATTACK() )	return false;

	DWORD dwFoundAType = FindOptAType ( m_TargetID, false );
	if ( dwFoundAType != m_dwAType )
	{
		ReAction ( m_TargetID );
		return false;
	}

	WORD wTARRANGE = GETATTACKRANGE();

	//	Note : tracing
	//
	bool bShortTrace(false);
	if ( m_pAttackProp->emAttRgType == EMATT_SHORT )	bShortTrace = true;

	bool btrace(false);
	if ( bShortTrace )		btrace = DoShortTracing ( wTARRANGE, fElapsedTime );
	else					btrace = DoLongTracing ( wTARRANGE, fElapsedTime );

	if ( !btrace )
	{
		ResetAction ();
		return false;
	}

	//	Note : 공격 여부 결정.
	//
	BOOL bactionable(FALSE);
	if ( bShortTrace )		bactionable = IsFLAG(EMATTACKABLE) && !IsFLAG(EMTRACING);
	else					bactionable = IsFLAG(EMATTACKABLE);

	if ( bactionable )
	{
		//	Note : 시야가 확보되는가?
		//
		GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTarget )
		{
			ResetAction ();
			return false;
		}
		
		D3DXVECTOR3 vFinderPos(m_vPos.x,m_vPos.y+15.0f,m_vPos.z);
		D3DXVECTOR3 vTargetPos(pTarget->GetPosition());
		vTargetPos.y += 15.0f;
		
		BOOL bCollision = m_pLandMan->IsCollision( vFinderPos, vTargetPos, FALSE );
		if ( bCollision )
		{
			ResetAction ();
			return false;
		}

		TurnAction ( GLAT_ATTACK );

		m_emReAction = EMREACT_NUNE;
	}

	return true;
}

void GLCrow::RunSchedule ( float fTime, float fElapsedTime )
{
	GASSERT(m_pCrowData);

	if ( IsACTION(GLAT_IDLE) )
	{
		if ( IsFLAG(EMSTAY) )
		{
			m_fStayTimer -= fElapsedTime;
			if ( m_fStayTimer<0.0f )	ReSetFLAG(EMSTAY);
		}
		else if ( !IsFLAG(EMTARGET) )	RunNextAct ();		// 새로운 액션.
	}

	if ( IsFLAG(EMTARGET) )
	{
		if ( !IsACTION(GLAT_ATTACK) && !IsACTION(GLAT_SKILL) )
		{
			switch ( m_emReAction )
			{
			case EMREACT_NUNE:
				ReAction ( m_TargetID, fElapsedTime );
				break;

			case EMREACT_ATTACK:
				AttackReAction ( fTime, fElapsedTime );
				break;

			case EMREACT_SKILL:
				SkillReAction ( fTime, fElapsedTime );
				break;
			};
		}
	}
	else if ( m_pCrowData->m_emCrow == CROW_MOB && VALIDACTION() )
	{
		EMCROWACT_UP emACT_UP = GetActPattern();
		if ( (emACT_UP!=EMCROWACT_UP_IDLE) && (emACT_UP!=EMCROWACT_UP_ESCAPE) )
		{
			m_fTargetTimer -= fElapsedTime;
			if ( m_fTargetTimer<0.0f )
			{
				m_fTargetTimer = 0.1f;	// 다음 검색 유격.
				
				//	Note : 검색 시작.
				STARGETID targetID = FindEnemyChar ();
				if ( targetID.dwID != EMTARGET_NULL )
				{
//					STARGETID sTARID(CROW_PC,pTarget->m_dwGaeaID,pTarget->GetPosition());
					ReAction(targetID);
				}
			}
		}
	}
}
