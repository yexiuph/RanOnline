#include "pch.h"
#include "GLPetClient.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "DxGlobalStage.h"
#include "GLItemMan.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextcontrol.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextcontrol.h"
#include "DxEffGroupPlayer.h"
#include "DxShadowMap.h"
#include "DxEnvironment.h"
#include "GLStrikeM.h"

GLPetClient::GLPetClient(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_pOwner(NULL),

	m_bValid(false),
	m_bSkillProcessing(false),
	m_dwActionFlag(0),
	m_fIdleTimer(0.0f),
	m_fAttackTimer(0.0f),
	m_sPetCrowID(NATIVEID_NULL()),
	m_wColorTEMP(0),
	m_wStyleTEMP(0),
	m_bEnableSytle(FALSE),
	m_bEnableColor(FALSE),
	m_bCannotPickUpItem(false),
	m_bCannotPickUpPileItem(false),
	m_hCheckStrDLL( NULL ),
	m_pCheckString( NULL ),
	m_vRandPos(0.0f,0.0f,0.0f)

{
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );
	m_vecDroppedItems.reserve(50);
	m_sSkillTARGET.RESET ();
	m_SKILLDELAY.clear();

	m_sPetSkinPackState.Init();

#ifdef TH_PARAM
	HMODULE m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}
    
#endif
}

GLPetClient::~GLPetClient(void)
{
	SAFE_DELETE ( m_pSkinChar );
#ifdef TH_PARAM
    if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif
}

HRESULT GLPetClient::Create ( const PGLPET pPetData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pNavi || !pd3dDevice ) return E_FAIL;

	m_pd3dDevice = pd3dDevice;

	ASSIGN ( *pPetData );

	m_vPos = vPos;
	m_vDir = vDir;
	m_pOwner = GLGaeaClient::GetInstance().GetCharacter ();

	m_actorMove.Create ( pNavi, m_vPos, -1 );

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;

	UpdateSuit ( TRUE );

	// Note : 1.AABB Box�� �����´�. 2.���̸� ����� ���´�.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;
	
	m_bValid = true;

	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	return S_OK;
}

HRESULT GLPetClient::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sPetID == NATIVEID_NULL() )
	{
		GASSERT ( 0&&"m_sPetID==NATIVEID_NULL()" );
		return E_FAIL;
	}

	SNATIVEID loadPetID = m_sPetID;

	if( IsUsePetSkinPack() )
	{
		loadPetID = m_sPetSkinPackState.petSkinMobID;
	}
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( loadPetID );
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

	

	// ���� �� ���ϸ��̼��߿� �÷��̰� �Ұ����� �ִϸ��̼��� ã�´�.
	if( IsUsePetSkinPack() )
	{
		if( !m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE )  ) m_sPetSkinPackState.bNotFoundGuard = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_RUN, AN_SUB_NONE ) )		 m_sPetSkinPackState.bNotFoundRun = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_WALK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundWalk = TRUE;	
//		m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_NONE ) )
		if( !m_pSkinChar->GETANIAMOUNT( AN_ATTACK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundAttack = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_04 ) )	 m_sPetSkinPackState.bNotFoundSad	 = TRUE;
	}

	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

	return S_OK;
}

HRESULT GLPetClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT GLPetClient::UpdateSuit ( BOOL bChangeStyle )
{

	if( IsUsePetSkinPack() ) return S_OK;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice, 
		TRUE );

	if ( !pSkinChar )					  return E_FAIL;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// ��Ÿ�� ����
	if ( bChangeStyle )
	{
		if ( sSTYLE.wSTYLENum > m_wStyle )
		{
			std::string strSTYLE_CPS = sSTYLE.strSTYLE_CPS[m_wStyle];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	���� ���� ��Ų.

			if ( pCharPart && strcmp(strSTYLE_CPS.c_str(),pCharPart->m_szFileName) )
			{
				m_pSkinChar->SetPiece ( strSTYLE_CPS.c_str(), m_pd3dDevice );
			}
		}
	}

	// �÷� ���� (�÷�����ī�� ������̸� �������� �ʴ´�)
	if ( !m_bEnableColor ) m_pSkinChar->SetHairColor( m_wColor );

	// �Ǽ����� ��� ����
	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( m_PutOnItems[i].sNativeID );
		if ( pSlotItem ) m_pSkinChar->SetPiece ( pSlotItem->GetPetWearingFile (), m_pd3dDevice, FG_MUSTNEWLOAD );
		else
		{
			EMPIECECHAR emPiece = GetPieceFromSlot ( (ACCESSORYTYPE)i );
			if ( emPiece != PIECE_SIZE ) m_pSkinChar->ResetPiece ( emPiece );
		}
	}

	return S_OK;
}

void GLPetClient::SetMoveState ( BOOL bRun = TRUE )
{
	if ( bRun ) 
	{
		m_actorMove.SetMaxSpeed ( m_fRunSpeed );
		SetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundRun )
			{
				m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );				
			}else{
				m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
			}
		}
	}
	else
	{
		m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
		ReSetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		}
	}

	// �̵����¸� �����ϴ� �޽��� �߼ۿ��
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE NetMsg;
	NetMsg.dwFlag = m_dwActionFlag;
	NETSENDTOFIELD ( &NetMsg );
}

HRESULT	GLPetClient::UpdatePetState ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vOwnerPos, vRandPos, vDist;
	float fDist;

	static float fTimer(0.0f);
	static WORD wSubAni(0);
	static float fIdleTimer(0.0f);

	// ���� ���ݸ�� ���̸�
	if ( IsSTATE ( EM_PETACT_ATTACK ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			// ���ݸ���� �ƹ��� ������ 2���̻� �������� �ʴ´�.
			m_fAttackTimer += fElapsedTime;
			if ( m_fAttackTimer > 2.0f )
			{
				ReqStop ( true );
			}
		}
	}

	// �����Ҷ�
	if ( IsSTATE ( EM_PETACT_SAD ) ) return S_OK;

	// ����������
	if ( IsSTATE ( EM_PETACT_STOP ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			m_vRandPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

			m_fIdleTimer += fElapsedTime;
			if ( m_fIdleTimer > 2.0f )
			{
				ReSetSTATE ( EM_PETACT_STOP );
				SetSTATE ( EM_PETACT_FUNNY );
				MakeAniSubKey ( fTime );

				// Funny Action
				m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

				GLMSG::SNETPET_REQ_FUNNY NetMsg;
				NetMsg.wFunny = m_wAniSub[wSubAni];
				NETSENDTOFIELD ( &NetMsg );

				m_fIdleTimer = 0.0f;
			}
		}

		return S_OK;
	}

	// FUNNY �׼����̸�
	if ( IsSTATE ( EM_PETACT_FUNNY ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
			wSubAni = 0;
		}
		else
		{
			PSANIMCONTAINER pANIMCON = m_pSkinChar->GETCURANIM ();

			// Funny �ִϸ��̼��� �������� Idle �ִϸ��̼� ����
			if ( pANIMCON->m_MainType == AN_GESTURE && pANIMCON->m_SubType == m_wAniSub[wSubAni] )
			{
				if ( m_pSkinChar->ISENDANIM () )
				{
					m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

					GLMSG::SNETPET_REQ_STOP NetMsg;
					NetMsg.dwGUID      = m_dwGUID;
					NetMsg.dwFlag      = EM_PETACT_STOP;
					NetMsg.vPos	       = m_vPos;
					NetMsg.bStopAttack = true; // ���ڸ��� �׳� ���ְ� �ϱ� ����
					NETSENDTOFIELD ( &NetMsg );

					if ( ++wSubAni > 2 ) 
					{
						// Funny �ִϸ��̼� ���� �缳��
						wSubAni = 0;
						MakeAniSubKey ( fTime );
					}

					fIdleTimer = 0.0f;
				}
			}
			// Idle �ִϸ��̼��� ������ Funny �ִϸ��̼� ����
			else if ( pANIMCON->m_MainType == AN_GUARD_N && pANIMCON->m_SubType == AN_SUB_NONE )
			{
				fIdleTimer += fElapsedTime;
				// �ִϸ��̼� ������ ������ �����Ǿ� �����Ƿ� ���� ������ �˼� �����Ƿ� �ð����� ������.
				if ( fIdleTimer > 3.0f )
				{
					// Funny Action
					m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

					GLMSG::SNETPET_REQ_FUNNY NetMsg;
					NetMsg.wFunny = m_wAniSub[wSubAni];
					NETSENDTOFIELD ( &NetMsg );

					fIdleTimer = 0.0f;
				}
			}
		}
	}

	// ������ ����/��ų ���϶�
	if ( m_pOwner->IsACTION ( GLAT_ATTACK ) || m_pOwner->IsACTION ( GLAT_SKILL ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPostision ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
		}
	}

	// �̵��� ������ ��ٸ���
	if ( IsSTATE ( EM_PETACT_WAIT ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();

		if ( m_pOwner->IsACTION ( GLAT_IDLE ) )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPostision ();
			vOwnerPos = m_pOwner->GetPosition ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
			return S_OK;
		}

		vDist = vOwnerPos - m_vPos;
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
			D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
			vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

			ReqGoto ( vOwnerPos, false );
		}

		m_fIdleTimer = 0.0f;
	}

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		// �������� �ֿ췯 �޸���
		if ( IsSTATE ( EM_PETACT_RUN_PICKUP_ITEM ) )
		{
			vDist = m_vPos - m_sSkillTARGET.vPos;
			float fDist = D3DXVec3Length(&vDist);
			if ( !m_actorMove.PathIsActive() )
			{
				// �������� �ݴ´�.
				m_pOwner->ReqFieldTo ( m_sSkillTARGET, true );
				ReqStop ();
				return S_OK;
			}
		}
		else
		{
			vOwnerPos = m_pOwner->GetPosition ();
			vDist = m_vPos - vOwnerPos;
			fDist = D3DXVec3Length(&vDist);

			STARGETID sTarget = m_pOwner->GetTargetID ();

			if ( !m_actorMove.PathIsActive() )
			{
				ReqStop ();
				return S_OK;
			}

			// 1�ʿ� �ѹ��� �̵��޽����� �����Ѵ�. (�̺κ��� �����غ��� �ҵ��ϴ�)
			fTimer += fElapsedTime;
			if ( fTimer > 1.0f && m_pOwner->IsACTION ( GLAT_MOVE ) )
			{
				// D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPostision ();
				vOwnerPos = m_pOwner->GetPosition ();

				D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
				D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
				vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

				ReqGoto ( vOwnerPos, false );
				fTimer = 0.0f;
			}
			
			// ������ ������ �Ÿ��� ���� �ٰų� �ȴ´�
			// [����] ���� �ڵ忡�� �̹� ���� ������ ���¸� ���������Ƿ� 
			//		   ������ �ȴ� ���¿� ���� ó���� ���ָ� �ȴ�.
			if ( !m_pOwner->IsSTATE ( EM_ACT_RUN ) )
			{
				vDist = m_vPos - vOwnerPos;
				float fDist = D3DXVec3Length(&vDist);

				// ���ΰ� �Ÿ��� �ʹ� �־����� �����Ÿ��� �����ϱ� ���� �޸���
				if ( fDist > GLCONST_PET::fRunArea && !IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					m_actorMove.SetMaxSpeed ( m_fRunSpeed );
					SetSTATE ( EM_PETACT_RUN_CLOSELY );
					SetMoveState ();
				}
				else if ( IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					if ( fDist <= GLCONST_PET::fWalkArea )
					{
						m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
						ReSetSTATE ( EM_PETACT_RUN_CLOSELY );
						SetMoveState ( FALSE );
					}
				}

				// ������ �����ִµ� �Ȱ� �ִٸ�
				if ( m_pOwner->IsACTION ( GLAT_IDLE ) )
				{
					SetMoveState ();
				}
			}
		}
		m_fIdleTimer = 0.0f;
	}

	return S_OK;
}

EM_FIELDITEM_STATE GLPetClient::IsInsertableInven ( STARGETID sTar )
{
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLAND ) return EM_FIELDITEM_STATE_GENERALFAIL;

	if ( m_sSkillTARGET.emCrow==CROW_ITEM )
	{
		PITEMCLIENTDROP pItemDrop = pLAND->GetItem ( m_sSkillTARGET.dwID );
		if ( !pItemDrop ) return EM_FIELDITEM_STATE_REMOVED;

		SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemClient.sNativeID);
		if ( !pItem )		return EM_FIELDITEM_STATE_GENERALFAIL;

		BOOL bOk = m_pOwner->IsInsertToInven ( pItemDrop );
		if ( !bOk ) return EM_FIELDITEM_STATE_INSERTFAIL;
	}
	else if ( m_sSkillTARGET.emCrow==CROW_MONEY )
	{
		PMONEYCLIENTDROP pMoney = pLAND->GetMoney ( m_sSkillTARGET.dwID );
		if ( !pMoney )	return EM_FIELDITEM_STATE_REMOVED;
	}

	return EM_FIELDITEM_STATE_INSERTOK;
}

void GLPetClient::StartSkillProc ()
{
	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID );
	if ( !pSkill )	return;

	// �� ��ų���� �Լ����� �޽����� �߼��ϰ� ȸ���� ������
	// ��ų �ߵ�...
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_PET_GETALLITEMS:		GetAllItems ();		break;
	case SKILL::EMFOR_PET_GETRAREITEMS:		GetRareItems ();	break;
	case SKILL::EMFOR_PET_GETPOTIONS:		GetPotions ();		break;
	case SKILL::EMFOR_PET_GETSTONE:			GetStone ();		break;
	case SKILL::EMFOR_PET_GETMONEY:			GetMoney ();		break;
	case SKILL::EMFOR_PET_HEAL:
	case SKILL::EMFOR_PET_SUPPROT:
	case SKILL::EMFOR_PET_BACKUP_ATK:
	case SKILL::EMFOR_PET_BACKUP_DEF:
		{
		}
		break;
	case SKILL::EMFOR_PET_AUTOMEDISON:
		break;
	case SKILL::EMFOR_PET_PROTECTITEMS:
		// ������ �������°��� �����ش�.
		break;

	default: break;
	};

	m_bSkillProcessing = true;

	//AccountSkill ( m_sActiveSkillID );
}

void GLPetClient::ProceedSkill ()
{
	// ��ų�� ����������?
	if ( !m_bSkillProcessing ) return;

	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID );
	if ( !pSkill )	return;
	
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	// �������� �ݴ� ��ų�̸�
	case SKILL::EMFOR_PET_GETALLITEMS:
	case SKILL::EMFOR_PET_GETRAREITEMS:
	case SKILL::EMFOR_PET_GETPOTIONS:
	case SKILL::EMFOR_PET_GETSTONE:
	case SKILL::EMFOR_PET_GETMONEY:
		{
			// �ֺ��� �������� ������ �޷�����.
			if ( !IsSTATE(EM_PETACT_RUN_PICKUP_ITEM) )
			{
				if ( !m_vecDroppedItems.empty() )
				{
					// �ϳ� ������
					m_sSkillTARGET = m_vecDroppedItems.back();
					m_vecDroppedItems.pop_back ();

					// �������� �κ��� ���� �� �ִ��� �˻�
					EM_FIELDITEM_STATE emResult = IsInsertableInven ( m_sSkillTARGET );
					
					switch ( emResult )
					{
					// ��Ʈ��ũ �����̷� �������� �ʴ� �������� ��� �õ��Ұ��
					case EM_FIELDITEM_STATE_REMOVED:
						break;

					// �κ��丮 ���� ����
					case EM_FIELDITEM_STATE_INSERTFAIL:
						{
							m_sSkillTARGET.RESET ();
							ReqChangeActiveSkill ( NATIVEID_NULL() ); // ��ų�� ����ع�����.
							m_vecDroppedItems.clear();
							CInnerInterface::GetInstance().PrintConsoleTextDlg( ID2GAMEINTEXT("EMPET_NOTENOUGHINVEN") );
						}
						break;;
					// �κ��丮 ���� ����
					case EM_FIELDITEM_STATE_INSERTOK:
						{
							D3DXVECTOR3 vDist, vDir, vTar;
							vDist = m_sSkillTARGET.vPos - m_vPos;
							D3DXVec3Normalize ( &vDir, &vDist );
							float fDist = D3DXVec3Length ( &vDist ) - 10.0f; // �ݱ� ������ �Ÿ���ŭ �Ÿ� ����
							vTar = vDir * fDist;
							vTar = m_vPos + vTar;
							ReqGoto ( vTar, true );
						}
						break;
					// �Ϲ� ������ ���
					case EM_FIELDITEM_STATE_GENERALFAIL:
						{
						}
						break;
					}
				}
				// ���� ������ ������ ������ ��û�Ѵ�.
				else 
				{
					GetItembySkill ( m_sActiveSkillID ); 
				}
			}
		}
		break;

		// ������ ��ų���� ������ ���¸� �������ش�.
	case SKILL::EMFOR_PET_HEAL:
	case SKILL::EMFOR_PET_SUPPROT:
	case SKILL::EMFOR_PET_BACKUP_ATK:
	case SKILL::EMFOR_PET_BACKUP_DEF:
		{
		}
		break;

	// ţ���Կ� ������ �Ծ��ش�. (�������ӿ� �ѹ��� ���� �Ծ��ش�!)
	case SKILL::EMFOR_PET_AUTOMEDISON:
		{
			DWORD dwMaxOwnersHP = m_pOwner->GETMAXHP ();
			DWORD dwMaxOwnersMP = m_pOwner->GETMAXMP ();
			DWORD dwMaxOwnersSP = m_pOwner->GETMAXSP ();
            DWORD dwHPVar = dwMaxOwnersHP - m_pOwner->GETHP ();
			int   nMPVar  = dwMaxOwnersMP - m_pOwner->GETMP ();
			int   nSPVar  = dwMaxOwnersSP - m_pOwner->GETSP ();

			// Q, W, E, A, S, D
			for ( WORD wSLOT = 0; wSLOT < EMACTIONQUICK_SIZE; ++wSLOT )
			{
				const SACTION_SLOT &sACTION = m_pOwner->m_sACTIONQUICK[wSLOT];
				if ( !sACTION.VALID() )	continue;

				switch ( sACTION.wACT )
				{
				case EMACT_SLOT_DRUG:
					{
						SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindItem ( sACTION.sNID );
						if ( !pInvenItem )	continue;

						SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sACTION.sNID );
						if ( !pITEM ) continue;

						if ( pITEM->sBasicOp.emItemType!=ITEM_CURE ) continue;

						if ( m_pOwner->IsCoolTime( pInvenItem->sItemCustom.sNativeID ) ) continue;
						
						// HP �����̸�
						switch ( pITEM->sDrugOp.emDrug )
						{
						case ITEM_DRUG_HP:
						case ITEM_DRUG_HP_MP:
						case ITEM_DRUG_HP_MP_SP:
						case ITEM_DRUG_HP_CURE:
						case ITEM_DRUG_HP_MP_SP_CURE:
							{
								if ( pITEM->sDrugOp.bRatio )
								{
									if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersHP <= (float)dwHPVar )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}
								else
								{
									if ( pITEM->sDrugOp.wCureVolume <= dwHPVar )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}

								// HP�� 20% ������ �������� ������ �Դ´�.
								if ( (float)(dwMaxOwnersHP-dwHPVar)/dwMaxOwnersHP*100.0f < GLCONST_PET::fMaginotLineHP )
								{
									m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
									return;
								}
							}
						};

						// MP �����̸�
						switch ( pITEM->sDrugOp.emDrug )
						{
						case ITEM_DRUG_MP:
						case ITEM_DRUG_HP_MP:
						case ITEM_DRUG_MP_SP:
						case ITEM_DRUG_HP_MP_SP:
						case ITEM_DRUG_HP_MP_SP_CURE:
							{
								if ( pITEM->sDrugOp.bRatio )
								{
									if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersMP <= float(nMPVar) )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}
								else
								{
									if ( pITEM->sDrugOp.wCureVolume <= nMPVar )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}

								// MP�� 20% ������ ��������
								if ( (float)(dwMaxOwnersMP-nMPVar)/dwMaxOwnersMP*100.0f < GLCONST_PET::fMaginotLineMP )
								{
									m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
									return;
								}
							}
						};

						// SP �����̸�
						switch ( pITEM->sDrugOp.emDrug )
						{
						case ITEM_DRUG_SP:
						case ITEM_DRUG_MP_SP:
						case ITEM_DRUG_HP_MP_SP:
						case ITEM_DRUG_HP_MP_SP_CURE:
							{
								if ( pITEM->sDrugOp.bRatio )
								{
									if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersSP <= (float)nSPVar )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}
								else
								{
									if ( pITEM->sDrugOp.wCureVolume <= nSPVar )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
										return;
									}
								}

								// SP�� 20% ������ ��������
								if ( (float)(dwMaxOwnersSP-nSPVar)/dwMaxOwnersSP*100.0f < GLCONST_PET::fMaginotLineSP )
								{
									m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
									return;
								}
							}
						};
						
						// �̻�ġ��
						if ( pITEM->sDrugOp.emDrug == ITEM_DRUG_CURE )
						{
							if ( m_pOwner->ISSTATEBLOW() )
							{
								m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY );
								return;
							}
						}
					}
				};
			} // end of for
		}
		break;

	case SKILL::EMFOR_PET_PROTECTITEMS:
		break;

	default: break;
	};
}

void GLPetClient::GetItembySkill ( SNATIVEID sSkillID )
{
	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )	return;
	
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	// �������� �ݴ� ��ų�̸�
	case SKILL::EMFOR_PET_GETALLITEMS:  GetAllItems ();
		break;
	case SKILL::EMFOR_PET_GETRAREITEMS: GetRareItems ();
		break;
	case SKILL::EMFOR_PET_GETPOTIONS:   GetPotions ();
		break;
	case SKILL::EMFOR_PET_GETSTONE:		GetStone ();
		break;
	case SKILL::EMFOR_PET_GETMONEY:		GetMoney ();
		break;
	}
}

void GLPetClient::GetAllItems ()
{
	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_ITEM | CROW_EX_MONEY );
	DETECTMAP* pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, m_vPos, MAX_VIEWRANGE/6.0f );
	if ( m_vecDroppedItems.empty() && pDetectMap )
	{
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		bool bDetectItem = pairRange.first != pairRange.second;
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
		bool bDetectMoney = pairRange.first != pairRange.second;

		// �����۰� ���� ���� ������ ������
		if ( bDetectItem || bDetectMoney )
		{
			m_vecDroppedItems.clear();
			GLMSG::SNETPET_REQ_GETRIGHTOFITEM NetMsg;
			NetMsg.dwGUID = m_dwGUID;
			NetMsg.emSkill = EMPETSKILL_GETALL;
			NETSENDTOFIELD ( &NetMsg );
		}
	}
}

void GLPetClient::GetRareItems ()
{
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLAND ) return;

	DETECTMAP* pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_ITEM, m_vPos, MAX_VIEWRANGE/6.0f );
	if ( m_vecDroppedItems.empty() && pDetectMap )
	{
		DETECTMAP_RANGE pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			DETECTMAP_ITER iter = pairRange.first;
			for ( ; iter != pairRange.second; ++iter )
			{
				const STARGETID &sTarID = (*iter);
				PITEMCLIENTDROP pDropItem = pLAND->GetItem ( sTarID.dwID );
				if ( !pDropItem ) continue;
				SITEM *pItem = GLItemMan::GetInstance().GetItem(pDropItem->sItemClient.sNativeID);
				if ( !pItem ) continue;

				// ����������� �ϳ��� ���� ������ ������
				if ( pItem->sBasicOp.emLevel == LEVEL_RARE )
				{
					m_vecDroppedItems.clear();
					GLMSG::SNETPET_REQ_GETRIGHTOFITEM NetMsg;
					NetMsg.dwGUID = m_dwGUID;
					NetMsg.emSkill = EMPETSKILL_GETRARE;
					NETSENDTOFIELD ( &NetMsg );
					break;
				}
			}
		}
	}
}

void GLPetClient::GetPotions ()
{
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLAND ) return;

	DETECTMAP* pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_ITEM, m_vPos, MAX_VIEWRANGE/6.0f );
	if ( m_vecDroppedItems.empty() && pDetectMap )
	{
		DETECTMAP_RANGE pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			DETECTMAP_ITER iter = pairRange.first;
			for ( ; iter != pairRange.second; ++iter )
			{
				const STARGETID &sTarID = (*iter);
				PITEMCLIENTDROP pDropItem = pLAND->GetItem ( sTarID.dwID );
				if ( !pDropItem ) continue;
				SITEM *pItem = GLItemMan::GetInstance().GetItem(pDropItem->sItemClient.sNativeID);
				if ( !pItem ) continue;

				// ������ �ϳ��� ���� ������ ������
				if ( pItem->sBasicOp.emItemType == ITEM_CURE )
				{
					m_vecDroppedItems.clear();
					GLMSG::SNETPET_REQ_GETRIGHTOFITEM NetMsg;
					NetMsg.dwGUID = m_dwGUID;
					NetMsg.emSkill = EMPETSKILL_GETPOTIONS;
					NETSENDTOFIELD ( &NetMsg );
					break;
				}
			}
		}
	}
}

void GLPetClient::GetMoney ()
{
	DETECTMAP* pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_MONEY, m_vPos, MAX_VIEWRANGE/6.0f );
	if ( m_vecDroppedItems.empty() && pDetectMap )
	{
		DETECTMAP_RANGE pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			m_vecDroppedItems.clear();
			GLMSG::SNETPET_REQ_GETRIGHTOFITEM NetMsg;
			NetMsg.dwGUID = m_dwGUID;
			NetMsg.emSkill = EMPETSKILL_GETMONEY;
			NETSENDTOFIELD ( &NetMsg );
		}
	}
}

void GLPetClient::GetStone ()
{
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLAND ) return;

	DETECTMAP* pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_ITEM, m_vPos, MAX_VIEWRANGE/6.0f );
	if ( m_vecDroppedItems.empty() && pDetectMap )
	{
		DETECTMAP_RANGE pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			DETECTMAP_ITER iter = pairRange.first;
			for ( ; iter != pairRange.second; ++iter )
			{
				const STARGETID &sTarID = (*iter);
				PITEMCLIENTDROP pDropItem = pLAND->GetItem ( sTarID.dwID );
				if ( !pDropItem ) continue;
				SITEM *pItem = GLItemMan::GetInstance().GetItem(pDropItem->sItemClient.sNativeID);
				if ( !pItem ) continue;

				// �������� �ϳ��� ���� ������ ������
				if ( pItem->sBasicOp.emItemType == ITEM_GRINDING
					 || pItem->sBasicOp.emItemType == ITEM_MATERIALS )
				{
					m_vecDroppedItems.clear();
					GLMSG::SNETPET_REQ_GETRIGHTOFITEM NetMsg;
					NetMsg.dwGUID = m_dwGUID;
					NetMsg.emSkill = EMPETSKILL_GETSTONE;
					NETSENDTOFIELD ( &NetMsg );
					break;
				}
			}
		}
	}
}

bool GLPetClient::CheckSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_CITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )
	{
		// ����� ���� ��ų
		return false;
	}

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter!=m_SKILLDELAY.end() )
	{
		// ��ų������
		return false;
	}
	return true;
}

float GLPetClient::GetSkillDelayPercent ( SNATIVEID sSkillID )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					     return 0.0f;

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter==m_SKILLDELAY.end() ) return 0.0f;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[0];
	float fSkillDelay = sSKILL_DATA.fDELAYTIME;
	float fCurDelay = (*delayiter).second;

	if ( fSkillDelay == 0.0f ) fSkillDelay = 1.0f;

	return fCurDelay / fSkillDelay;
}

void GLPetClient::AccountSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( sSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return;
	PETSKILL sPetSkill = (*iter).second;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					 return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	m_SKILLDELAY.insert ( std::make_pair(sSkillID.dwID, sSKILL_DATA.fDELAYTIME) );
}

void GLPetClient::UpdateSkillDelay ( float fElapsedTime )
{
	DELAY_MAP_ITER iter_del;

	PETDELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	PETDELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay < 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

HRESULT GLPetClient::FrameMove( float fTime, float fElapsedTime )
{
	if ( !m_bValid ) return S_OK;

	HRESULT hr(S_OK);
	
	UpdatePetState ( fTime, fElapsedTime );
	ProceedSkill ();
	// UpdateSkillDelay ( fElapsedTime ); ��ȹ�� �躴�� ��û���� ���� [06.10.09]

	// ============== Update about Actor (Begin) ============== /

	hr = m_actorMove.Update ( fElapsedTime );
	if ( FAILED(hr) )	return E_FAIL;

	// ============== Update about Actor (end)   ============== /

	// ============== Update about Skin (Begin) ============== /

	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;
	
	m_pSkinChar->SetPosition ( m_vPos );

	if( IsUsePetSkinPack() && IsSTATE ( EM_PETACT_MOVE ) )
	{
		float fSpeedGap1, fSpeedGap2, fTempTime, fTempElapsedTime;

		fSpeedGap1 = 1.0f / m_sPetSkinPackState.fPetScale;
		if( IsSTATE( EM_PETACT_RUN ) && m_sPetSkinPackState.bNotFoundRun ) fSpeedGap2 = m_fRunSpeed / m_fWalkSpeed;
		else															  fSpeedGap2 = 1.0f;

		fTempTime		 = fTime * fSpeedGap1 * fSpeedGap2;
		fTempElapsedTime = fElapsedTime * fSpeedGap1 * fSpeedGap2;

		m_pSkinChar->FrameMove ( fTempTime, fTempElapsedTime );
   	}else{
		m_pSkinChar->FrameMove ( fTime, fElapsedTime );
	}

	// ============== Update about Skin (End)   ============== /

	// ���� ��ġ ������Ʈ
	m_vPos = m_actorMove.Position();

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

	//	Note : AABB ���.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	return S_OK;
}

HRESULT GLPetClient::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )   return S_OK;
	if ( !pd3dDevice ) return E_FAIL;

	// ���� �ǽ� ������ �Ųٷ� �׸���.
	if ( m_pSkinChar ) 
	{
		if( IsUsePetSkinPack() )
		{
			D3DXMATRIX matScale, matRender;
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;
			m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE );
		}else{
			m_pSkinChar->Render( pd3dDevice, m_matTrans, FALSE, TRUE, TRUE );
		}
	}
	
	return S_OK;
}

HRESULT GLPetClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )   return S_OK;
	if ( !pd3dDevice ) return E_FAIL;

	//	Note : �׸��� ������.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )   return S_OK;
	if ( !pd3dDevice ) return E_FAIL;

	//	Note : �ݻ� ������.
	//
	if ( m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLPetClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}
	
	return S_OK;
}

HRESULT GLPetClient::InvalidateDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}
	
	return S_OK;
}

HRESULT GLPetClient::DeleteDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
		SAFE_DELETE( m_pSkinChar );
	}

	m_actorMove.Stop ();
	m_actorMove.Release ();

	m_pd3dDevice = NULL;
	m_pOwner	 = NULL;
	m_vDir	     = D3DXVECTOR3 (0,0,-1);
	m_vPos	     = D3DXVECTOR3(0,0,0);
	m_vDirOrig   = D3DXVECTOR3(0,0,-1);
	m_bValid	 = false;
	m_bSkillProcessing = false;
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	m_sSkillTARGET.RESET ();
	m_fIdleTimer = 0.0f;
	m_fAttackTimer = 0.0f;
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );

	m_sPetCrowID = NATIVEID_NULL();

	m_SKILLDELAY.clear();

	m_bCannotPickUpPileItem = false;
	m_bCannotPickUpItem = false;

	m_vecDroppedItems.clear();

	RESET ();

	return S_OK;
}

void GLPetClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
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

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
void GLPetClient::StyleInitData()
{
	m_bEnableSytle = TRUE;
	
	m_wStyleTEMP = m_wStyle;

	// �÷� ��ȭ ���� ��� �ǵ��� �ʴ´�.
	if( !m_bEnableColor )
	{
		m_wColorTEMP = m_wColor;
	}
}

void GLPetClient::ColorInitData()
{
	m_bEnableColor = TRUE;

	m_wColorTEMP = m_wColor;

	// ��Ÿ�� ��ȭ ���� ��� �ǵ��� �ʴ´�.
	if( !m_bEnableSytle )
	{
		m_wStyleTEMP = m_wStyle;
	}
}

void GLPetClient::StyleChange( WORD wStyle )
{
	m_wStyleTEMP = wStyle; 

	if ( m_wColor == GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyle] && !m_bEnableColor )
	{
		m_wColorTEMP = GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyleTEMP];
	}

	StyleUpdate();
	ColorUpdate();
}

void GLPetClient::ColorChange( WORD wColor )
{ 
	m_wColorTEMP = wColor; 

	ColorUpdate();
}

void GLPetClient::StyleUpdate()
{
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice, 
		TRUE );

	if ( !pSkinChar )					  return;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// ��Ÿ��.
	if ( sSTYLE.wSTYLENum > m_wStyleTEMP )
	{
		std::string strHAIR_CPS = sSTYLE.strSTYLE_CPS[m_wStyleTEMP];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	���� ���� ��Ų.

		if( pCharPart && strcmp(strHAIR_CPS.c_str(),pCharPart->m_szFileName) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL );
		}
	}

	// Note : �̰��� ȣ���ϸ鼭.. �Ӹ����� �����·� �ٲ�� ������.
	//		�׷��� FALSE ȣ��� �Ӹ������ �ȹٲ�� �Ѵ�.
	//UpdateSuit( FALSE );
}

void GLPetClient::ColorUpdate()
{
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice, 
		TRUE );

	if ( !pSkinChar )					  return;

	m_pSkinChar->SetHairColor( m_wColorTEMP );
}

void GLPetClient::MakeAniSubKey ( float fTime )
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
