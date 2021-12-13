#include "pch.h"
#include "./GLCrow.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	typedef std::vector<SDAMAGELOG>	VECDAMAGE;
	typedef VECDAMAGE::iterator		VECDAMAGE_ITER;

	bool damage_less ( const SDAMAGELOG &lhs, const SDAMAGELOG &rhs )
	{
		return lhs.dwDamage < rhs.dwDamage;
	}
};

namespace
{
	typedef std::vector<SDAMAGELOGPARTY> VECDAMAGEPARTY;
	typedef VECDAMAGEPARTY::iterator VECDAMAGEPARTY_ITER;

	bool party_damage_less ( const SDAMAGELOGPARTY &lhs, const SDAMAGELOGPARTY &rhs )
	{
		return lhs.m_dwDamage < rhs.m_dwDamage;
	}
};

void GLCrow::GenerateReward()
{
	GenerateReward_20060417();
	// GenerateReward_20060402();
	// GenerateReward_200502();
}

//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-17 Jgkim ���ʿ��� vector ����
void GLCrow::GenerateReward_20060417()
{
	STARGETID mytarid(GETCROW(),m_dwGlobID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.dwID != GAEAID_NULL))
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( m_sAssault.dwID );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			dwOWNER = pChar->m_dwGaeaID;

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			if( pChar->m_bTracingUser )
			{
				NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = pChar->GetUserID();
				StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, pChar->m_szUID );

				CString strTemp;
				strTemp.Format( "Moster Kill!!, [%s][%s], Monster ID:mid[%d]sid[%d]", 
						 pChar->m_szUID, pChar->m_szName, m_sNativeID.wMainID, m_sNativeID.wSubID );

				StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

				GLGaeaServer::GetInstance().SENDTOAGENT( pChar->m_dwClientID, &TracingMsg );
			}
#endif
		}
	}


	if ((m_sAssault.emCrow == CROW_SUMMON) &&
		(m_sAssault.dwID != GAEAID_NULL))
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( m_sAssault.dwID );
		if( pSummon ) 
		{
			if ( pSummon->m_pOwner )
			{
				pSummon->m_pOwner->ReceiveKillExp ( mytarid, false, true );
				dwOWNER = pSummon->m_pOwner->m_dwGaeaID;

			}
		}
	}
	
	DWORD dwPartyMasterGaeaID = GAEAID_NULL;
	DWORD dwPartyDamage = 0;
	// ��Ƽ������ ���	
	if (!m_cDamageLogParty.empty())
	{
		DAMAGELOGPARTY_ITER iterP     = m_cDamageLogParty.begin();
		DAMAGELOGPARTY_ITER iterP_end = m_cDamageLogParty.end();

		GLPARTY_FIELD* pParty = NULL;
		for ( ; iterP != iterP_end; ++iterP )
		{
			SDAMAGELOGPARTY &sDamageLogParty = (*iterP).second;
			pParty = GLGaeaServer::GetInstance().GetParty(sDamageLogParty.m_dwPartyID);
			if (pParty == NULL) 
			{
				continue;
			}
			else
			{				
				if (sDamageLogParty.m_dwDamage > dwPartyDamage)
				{
					// ��Ƽ�߿��� ���� ���� �������� �� ��Ƽ�� �����͸� ã�´�.
					dwPartyMasterGaeaID = pParty->m_dwMASTER;
					dwPartyDamage       = sDamageLogParty.m_dwDamage;
				}
			}
		}
	}
	
	// ĳ���Ͱ� �� ������ ���
	DWORD dwCharGaeaID = GAEAID_NULL;
	DWORD dwCharDamage = 0;
	if (!m_cDamageLog.empty())
	{
		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for (; iter!=iter_end; ++iter)
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = GLGaeaServer::GetInstance().GetChar ( sDamageLog.dwGaeaID );
			
			if (!pChar) continue;
			if (sDamageLog.dwUserID != pChar->GetUserID()) continue;

			pChar->ReceiveAttackExp(mytarid, sDamageLog.dwDamage, FALSE);

			if (sDamageLog.dwDamage > dwCharDamage)
			{
				// ĳ�����߿� ���� ���� �������� �� ĳ���͸� ã�´�.
                dwCharGaeaID = pChar->m_dwGaeaID;
				dwCharDamage = sDamageLog.dwDamage;
			}
		}
	}

	// ��ȯ���� �� ������ ���
//	dwCharGaeaID = GAEAID_NULL;
//	dwCharDamage = 0;
	if (!m_cSummonDamageLog.empty())
	{
		PGLSUMMONFIELD pSummon = NULL;
		DAMAGELOG_ITER iter = m_cSummonDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cSummonDamageLog.end ();
		for (; iter!=iter_end; ++iter)
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pSummon = GLGaeaServer::GetInstance().GetSummon ( sDamageLog.dwGaeaID );

			if (!pSummon || !pSummon->IsValid() || pSummon->m_pOwner == NULL) continue;
			if (sDamageLog.dwGaeaID !=  pSummon->GetCtrlID()) continue;

			pSummon->m_pOwner->ReceiveAttackExp(mytarid, sDamageLog.dwDamage, FALSE, FALSE, TRUE);

			if (sDamageLog.dwDamage > dwCharDamage)
			{
				// ĳ�����߿� ���� ���� �������� �� ĳ���͸� ã�´�.
				dwCharGaeaID = pSummon->m_pOwner->m_dwGaeaID;
				dwCharDamage = sDamageLog.dwDamage;
			}
		}
	}

	// ��Ƽ�� ĳ���� �� ���� �������� �� �ʿ�
	// ������ �������� �ο��Ѵ�.
	if (dwCharDamage > dwPartyDamage)
	{
        dwOWNER = dwCharGaeaID;
	}
	else
	{
		dwOWNER = dwPartyMasterGaeaID;
	}

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();	
	m_cSummonDamageLog.clear();

	GenerateRewardItem(dwOWNER);
}



//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-17 Jgkim GenerateReward �Լ����� �и�.
void GLCrow::GenerateRewardItem(DWORD dwOWNER /* GAEAID_NULL */)
{
	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	bool bParty = FALSE;

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = GLGaeaServer::GetInstance().IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGEN_RATE;

			GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty(pChar->m_dwPartyID);
			if ( pParty )
			{
				bParty = TRUE;

				switch ( pParty->m_sOPTION.emGET_ITEM )
				{
				default:
				case EMPTYITEMOPT_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPTYITEMOPT_FREE:
				case EMPTYITEMOPT_LEADER:
				case EMPTYITEMOPT_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_dwPartyID;
					break;
				};

				switch ( pParty->m_sOPTION.emGET_MONEY )
				{
				default:
				case EMPTYMONEYOPT_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPTYMONEYOPT_FREE:
				case EMPTYMONEYOPT_LEADER:
				case EMPTYMONEYOPT_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_dwPartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwOWNER );

	//	Note : ������ �߻�. ( �ܼ� )
	//
	const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
	if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
	{
		float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::fITEM_DROP_SCALE * fGEN_RATE;

		if( !bParty )
		{
			fGEN_RATE_R = CalculateItemGenRate( fGEN_RATE_R, pChar, bParty );
		}
		if ( RANDOM_GEN(fGEN_RATE_R) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
				bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID );
				if ( bvalid )
				{
					//	������ �߻�.
					m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}
	}

	//	Note : ���� ������ �߻�.
	//
	float fGenRate = 1.0f; 

	if( !bParty )
	{
		fGenRate = CalculateItemGenRate( fGenRate, pChar, bParty );
	}

	SGENITEM &sGenItem = m_pCrowData->m_sGenItem;



	if ( sGenItem.ISGEN(fGenRate) )
	{
		WORD wGenNum = sGenItem.SELECTNUM();
		
		for ( WORD i=0; i<wGenNum; ++i )
		{
			bool bSEPCID = false;
			DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
			SNATIVEID sNID;
			
			if ( dwSpecID==SGENITEM::EMSPEC_NULL )	continue;
			
			if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
			else	sNID = dwSpecID;
			
			if ( sNID==NATIVEID_NULL() )	continue;
		
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( !bCollision )				continue;

			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : Question ������ �߻� by ���
	//
	SQTGENITEM & sQtGenItem = m_pCrowData->m_sQtGenItem;
	SNATIVEID nidQITEM = sQtGenItem.SELECTITEM();
	if ( nidQITEM != SNATIVEID(false) )
	{
		D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
		
		BOOL bCollision;
		D3DXVECTOR3 vCollision;
		bCollision = m_pLandMan->IsCollisionNavi
		(
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
			vCollision
		);

		if ( bCollision )
		{
			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( nidQITEM, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : �� �߻�.
	//
	if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
	{
		float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::fMONEY_DROP_SCALE;
		if ( RANDOM_GEN(fGEN_RATE) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);
			
			//*/*/*/*
			if ( bCollision )
			{
				//	�� �߻�.
				DWORD dwGemMoney = DWORD ( GLCONST_CHAR::fMONEY_DROP_SCALE * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*RANDOM_NUM ) );
				m_pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
				
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if ( m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_REPULSE"),
			m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}
}

// �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ������ ������� ���Ѵ�.
float GLCrow::CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty )
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


//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-02 Jgkim ��Ƽ������ ����ϵ��� ����
void GLCrow::GenerateReward_20060402 ()
{
	STARGETID mytarid(GETCROW(),m_dwGlobID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.dwID != GAEAID_NULL))
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( m_sAssault.dwID );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			dwOWNER = pChar->m_dwGaeaID;
		}
	}

	// ��Ƽ������ ���
	VECDAMAGEPARTY vecDAMAGEPARTY;
	if ( !m_cDamageLogParty.empty() )
	{		
		vecDAMAGEPARTY.reserve ( m_cDamageLogParty.size() );

		DAMAGELOGPARTY_ITER iterP     = m_cDamageLogParty.begin();
		DAMAGELOGPARTY_ITER iterP_end = m_cDamageLogParty.end();

		GLPARTY_FIELD* pParty = NULL;
		for ( ; iterP != iterP_end; ++iterP )
		{
			SDAMAGELOGPARTY &sDamageLogParty = (*iterP).second;
			pParty = GLGaeaServer::GetInstance().GetParty(sDamageLogParty.m_dwPartyID);
			if (pParty == NULL) 
			{
				continue;
			}
			else
			{
				vecDAMAGEPARTY.push_back(sDamageLogParty);
			}
		}
	}
	
	//	Note : �Ϲ� ������ ���
	VECDAMAGE vecDAMAGE;
	if ( !m_cDamageLog.empty() )
	{		
		vecDAMAGE.reserve ( m_cDamageLog.size() );

		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = GLGaeaServer::GetInstance().GetChar ( sDamageLog.dwGaeaID );
			
			if ( !pChar ) continue;

			if ( sDamageLog.dwUserID != pChar->GetUserID() )		continue;

			pChar->ReceiveAttackExp ( mytarid, sDamageLog.dwDamage, FALSE );

			vecDAMAGE.push_back ( sDamageLog );
		}
	}	

	// ��Ƽ�߿��� ���� ���� �������� �� ��Ƽ�� �����͸� ���Ѵ�.
	DWORD dwPartyMasterGaeaID = GAEAID_NULL;
	DWORD dwPartyDamage = 0;
	if ( !vecDAMAGEPARTY.empty() )
	{
		VECDAMAGEPARTY_ITER posP = std::max_element ( vecDAMAGEPARTY.begin(), 
			                                         vecDAMAGEPARTY.end(), 
													 party_damage_less );
		if ( posP != vecDAMAGEPARTY.end() )
		{
			const SDAMAGELOGPARTY &sDAMAGEPARTY = (*posP);
			GLPARTY_FIELD* pParty2 = GLGaeaServer::GetInstance().GetParty(sDAMAGEPARTY.m_dwPartyID);
			if (pParty2 != NULL)
			{				
				PGLCHAR pCharX = GLGaeaServer::GetInstance().GetChar(pParty2->m_dwMASTER);
				if (pCharX != NULL)
				{
					dwPartyDamage = sDAMAGEPARTY.m_dwDamage; // ��Ƽ�� �� ������
					dwPartyMasterGaeaID = pParty2->m_dwMASTER; // ��Ƽ ������
					dwOWNER = dwPartyMasterGaeaID;
				}
			}
		}
	}

	//	Note : ������ ���� ���� ��� �ѻ������ ������ ȹ�� ���� �ο�.
	//		���� ���� ����� �������� ��� ȹ�� ������ ����.
	DWORD dwCharGaeaID = GAEAID_NULL;
	DWORD dwCharDamage = 0;
	if ( !vecDAMAGE.empty() )
	{
		VECDAMAGE_ITER pos = std::max_element ( vecDAMAGE.begin(), vecDAMAGE.end(), damage_less );
		if ( pos!=vecDAMAGE.end() )
		{
			const SDAMAGELOG &sDAMAGE = (*pos);

			PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sDAMAGE.dwGaeaID );
			if ( pChar && sDAMAGE.dwUserID==pChar->GetUserID() )
			{
				dwCharGaeaID = sDAMAGE.dwGaeaID;
				dwCharDamage = sDAMAGE.dwDamage;
				if (dwCharDamage > dwPartyDamage)
				{
					dwOWNER = dwCharGaeaID;
				}
			}
		}
	}	

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = GLGaeaServer::GetInstance().IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGEN_RATE;

			GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty(pChar->m_dwPartyID);
			if ( pParty )
			{
				switch ( pParty->m_sOPTION.emGET_ITEM )
				{
				default:
				case EMPTYITEMOPT_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPTYITEMOPT_FREE:
				case EMPTYITEMOPT_LEADER:
				case EMPTYITEMOPT_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_dwPartyID;
					break;
				};

				switch ( pParty->m_sOPTION.emGET_MONEY )
				{
				default:
				case EMPTYMONEYOPT_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPTYMONEYOPT_FREE:
				case EMPTYMONEYOPT_LEADER:
				case EMPTYMONEYOPT_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_dwPartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	//	Note : ������ �߻�. ( �ܼ� )
	//
	const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
	if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
	{
		float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::fITEM_DROP_SCALE * fGEN_RATE;

		if ( RANDOM_GEN(fGEN_RATE_R) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
				bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID );
				if ( bvalid )
				{
					//	������ �߻�.
					m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}
	}

	//	Note : ���� ������ �߻�.
	//
	SGENITEM &sGenItem = m_pCrowData->m_sGenItem;
	if ( sGenItem.ISGEN() )
	{
		WORD wGenNum = sGenItem.SELECTNUM();
		
		for ( WORD i=0; i<wGenNum; ++i )
		{
			bool bSEPCID = false;
			DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
			SNATIVEID sNID;
			
			if ( dwSpecID==SGENITEM::EMSPEC_NULL )	continue;
			
			if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
			else	sNID = dwSpecID;
			
			if ( sNID==NATIVEID_NULL() )	continue;
		
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( !bCollision )				continue;

			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : Question ������ �߻� by ���
	//
	SQTGENITEM & sQtGenItem = m_pCrowData->m_sQtGenItem;
	SNATIVEID nidQITEM = sQtGenItem.SELECTITEM();
	if ( nidQITEM != SNATIVEID(false) )
	{
		D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
		
		BOOL bCollision;
		D3DXVECTOR3 vCollision;
		bCollision = m_pLandMan->IsCollisionNavi
		(
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
			vCollision
		);

		if ( bCollision )
		{
			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( nidQITEM, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : �� �߻�.
	//
	if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
	{
		float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::fMONEY_DROP_SCALE;
		if ( RANDOM_GEN(fGEN_RATE) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);
			
			if ( bCollision )
			{
				//	�� �߻�.
				DWORD dwGemMoney = DWORD ( GLCONST_CHAR::fMONEY_DROP_SCALE * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*RANDOM_NUM ) );

				m_pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if ( m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_REPULSE"),
			m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}
}

void GLCrow::GenerateReward_200502 ()
{
	STARGETID mytarid(GETCROW(),m_dwGlobID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.dwID != GAEAID_NULL))
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( m_sAssault.dwID );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			dwOWNER = pChar->m_dwGaeaID;
		}
	}

	//	Note : �Ϲ� ������.
	//
	if ( !m_cDamageLog.empty() )
	{
		VECDAMAGE vecDAMAGE;
		vecDAMAGE.reserve ( m_cDamageLog.size() );

		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = GLGaeaServer::GetInstance().GetChar ( sDamageLog.dwGaeaID );
			
			if ( !pChar ) continue;

			if ( sDamageLog.dwUserID != pChar->GetUserID() )		continue;

			pChar->ReceiveAttackExp ( mytarid, sDamageLog.dwDamage, FALSE );

			vecDAMAGE.push_back ( sDamageLog );
		}

		//	Note : ������ ���� ���� ��� �ѻ������ ������ ȹ�� ���� �ο�.
		//		���� ���� ����� �������� ��� ȹ�� ������ ����.
		if ( !vecDAMAGE.empty() )
		{
			VECDAMAGE_ITER pos = std::max_element ( vecDAMAGE.begin(), vecDAMAGE.end(), damage_less );
			if ( pos!=vecDAMAGE.end() )
			{
				const SDAMAGELOG &sDAMAGE = (*pos);

				PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( sDAMAGE.dwGaeaID );
				if ( pChar && sDAMAGE.dwUserID==pChar->GetUserID() )
				{
					dwOWNER = sDAMAGE.dwGaeaID;
				}
			}
		}
	}

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = GLGaeaServer::GetInstance().IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGEN_RATE;

			GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty(pChar->m_dwPartyID);
			if ( pParty )
			{
				switch ( pParty->m_sOPTION.emGET_ITEM )
				{
				default:
				case EMPTYITEMOPT_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPTYITEMOPT_FREE:
				case EMPTYITEMOPT_LEADER:
				case EMPTYITEMOPT_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_dwPartyID;
					break;
				};

				switch ( pParty->m_sOPTION.emGET_MONEY )
				{
				default:
				case EMPTYMONEYOPT_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPTYMONEYOPT_FREE:
				case EMPTYMONEYOPT_LEADER:
				case EMPTYMONEYOPT_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_dwPartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	//	Note : ������ �߻�. ( �ܼ� )
	//
	const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
	if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
	{
		float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::fITEM_DROP_SCALE * fGEN_RATE;

		if ( RANDOM_GEN(fGEN_RATE_R) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
				bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID );
				if ( bvalid )
				{
					//	������ �߻�.
					m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}
	}

	//	Note : ���� ������ �߻�.
	//
	SGENITEM &sGenItem = m_pCrowData->m_sGenItem;
	if ( sGenItem.ISGEN() )
	{
		WORD wGenNum = sGenItem.SELECTNUM();
		
		for ( WORD i=0; i<wGenNum; ++i )
		{
			bool bSEPCID = false;
			DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
			SNATIVEID sNID;
			
			if ( dwSpecID==SGENITEM::EMSPEC_NULL )	continue;
			
			if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
			else	sNID = dwSpecID;
			
			if ( sNID==NATIVEID_NULL() )	continue;
		
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( !bCollision )				continue;

			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( sNID, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : Question ������ �߻� by ���
	//
	SQTGENITEM & sQtGenItem = m_pCrowData->m_sQtGenItem;
	SNATIVEID nidQITEM = sQtGenItem.SELECTITEM();
	if ( nidQITEM != SNATIVEID(false) )
	{
		D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
		
		BOOL bCollision;
		D3DXVECTOR3 vCollision;
		bCollision = m_pLandMan->IsCollisionNavi
		(
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
			vCollision
		);

		if ( bCollision )
		{
			//	�߻� ������ �ִ��� �˻�.
			bool bvalid = GLITEMLMT::GetInstance().DoCheckItemGen ( nidQITEM, EMGEN_MOB );
			if ( bvalid )
			{
				//	������ �߻�.
				m_pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
			}
		}
	}

	//	Note : �� �߻�.
	//
	if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
	{
		float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::fMONEY_DROP_SCALE;
		if ( RANDOM_GEN(fGEN_RATE) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);
			
			if ( bCollision )
			{
				//	�� �߻�.
				DWORD dwGemMoney = DWORD ( GLCONST_CHAR::fMONEY_DROP_SCALE * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*RANDOM_NUM ) );

				m_pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if ( m_pCrowData->m_sAction.m_dwActFlag&EMCROWACT_KNOCK )
	{
		const char *szMAP_NAME = GLGaeaServer::GetInstance().GetMapName ( m_sMapID );
		CString strTEXT;
		strTEXT.Format ( ID2SERVERTEXT("EMCROWACT_REPULSE"),
			m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		NetMsg.SETTEXT ( strTEXT.GetString() );

		GLGaeaServer::GetInstance().SENDTOAGENT ( &NetMsg );
	}
}