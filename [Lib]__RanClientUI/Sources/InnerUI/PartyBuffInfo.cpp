#include "pch.h"
#include "PartyBuffInfo.h"
#include "GLGaeaClient.h"
#include "PartyBuffInfoUnit.h"
#include "GLSkill.h"
#include "GLPartyClient.h"
#include "BasicLineBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPartyBuffInfo::CPartyBuffInfo () :
	m_NEWID ( SKILLTIME_UNIT )
{
}

CPartyBuffInfo::~CPartyBuffInfo ()
{
}

void CPartyBuffInfo::CreateSubControl ()
{	
	{
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxWaitServer ( "PARTYBUFF_BACK" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
		m_pLineBox = pLineBox;

		m_fBUFF_BACK_SIZE_X = m_pLineBox->GetLocalPos().sizeX;
	}


	const CString strKeyword = "PARTYBUFF_UNIT";
	CString strCombine;

	for ( int i = 0; i < SKILLFACT_SIZE; i++ )
	{
		strCombine.Format ( "%s%d", strKeyword, i );
		CUIControl* pDummyControl = new CUIControl;
		pDummyControl->CreateSub ( this, strCombine.GetString() );
		RegisterControl ( pDummyControl );

		m_pSkillUnitDummy[i] = pDummyControl;
		m_fBUFF_SIZE_X = pDummyControl->GetLocalPos().sizeX;
	}
}

UIGUID CPartyBuffInfo::GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID )
{
	SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
	SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();
	for ( ; iter != iter_end; ++iter )
	{
		const SKILLTIME_PAIR& pair = (*iter);
		const DWORD& dwID = pair.first;
		const UIGUID& cID = pair.second;
		if ( dwID == dwSkillID ) return cID;
	}

	return NO_ID;
}

void CPartyBuffInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	//	삭제
	if ( !m_SkillPlayList.empty () )
	{
		SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
		SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();

		for ( ; iter != iter_end; )
		{
			const SKILLTIME_PAIR& skill_pair = (*iter);
			const DWORD& dwID = skill_pair.first;
			const UIGUID& cID = skill_pair.second;

			//	찾기			
			SSKILLFACT sSKILLFACT;
			if ( !GET_PLAY_SKILLFACT ( dwID, sSKILLFACT ) )
			{				
				DeleteControl ( cID, 0 );
				iter = m_SkillPlayList.erase ( iter );
			}
			else ++iter;
		}
	}

	//	등록
	{
		//	NOTE
		//		PARTY SLOT에 따라 SKILLFACT를 들고 온다.
		GLPARTY_CLIENT *pMember = NULL;
		if ( m_nPartyID == 0 )	//	마스터인가?
			pMember = GLPartyClient::GetInstance().GetMaster ();
		else
			pMember = GLPartyClient::GetInstance().GetMember ( m_nPartyID-1 );

		if ( pMember )	
		{
			bool bSELF = false;
			if ( pMember == FindSelfClient () ) bSELF = true;

			PGLCHARCLIENT pCHAR = NULL;
			if ( !bSELF )
			{
				pCHAR = GLGaeaClient::GetInstance().GetChar ( pMember->m_dwGaeaID );
				if ( pCHAR )
				{
					for ( int i = 0; i < SKILLFACT_SIZE; i++ )
					{
						//	NOTE
						//		PARTY SLOT에 따라 SKILLFACT를 들고 온다.
						const SSKILLFACT* pSKILLFACT = pCHAR->GET_SKILLFACT ( i );
						if ( !pSKILLFACT ) continue;
						const DWORD& dwID = pSKILLFACT->sNATIVEID.dwID;		

						if ( dwID == NATIVEID_NULL().dwID ) continue;

						UIGUID cID = GET_EXIST_PLAY_SKILL_CONTROL ( dwID );
						if ( cID == NO_ID )	NEW_PLAY_SKILL ( dwID, pSKILLFACT->wLEVEL );
					}
				}
			}
			else
			{
				for ( int i = 0; i < SKILLFACT_SIZE; i++ )
				{
					//	NOTE
					//		PARTY SLOT에 따라 SKILLFACT를 들고 온다.
					const SSKILLFACT* pSKILLFACT = &(GLGaeaClient::GetInstance().GetCharacter()->m_sSKILLFACT[i]);
					if ( !pSKILLFACT ) continue;
					const DWORD& dwID = pSKILLFACT->sNATIVEID.dwID;		

					if ( dwID == NATIVEID_NULL().dwID ) continue;

					UIGUID cID = GET_EXIST_PLAY_SKILL_CONTROL ( dwID );
					if ( cID == NO_ID )	NEW_PLAY_SKILL ( dwID, pSKILLFACT->wLEVEL );
				}
			}
		}
	}

	if ( m_SkillPlayList.empty () )
	{
		SetVisibleSingle ( FALSE );
		return ;
	}

	//	업데이트
	int nPLAY_SKILL = 0;
	if ( !m_SkillPlayList.empty () )
	{		
		SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
		SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();

		for ( ; iter != iter_end; ++iter )
		{
			const SKILLTIME_PAIR& skill_pair = (*iter);
			const DWORD& dwID = skill_pair.first;
			const UIGUID& cID = skill_pair.second;			

			CPartyBuffInfoUnit* pControl = (CPartyBuffInfoUnit*) FindControl ( cID );
			if ( !pControl )
			{
				GASSERT ( 0 && "컨트롤 찾기 실패" );
				continue;
			}

			//	찾기			
			SSKILLFACT sSKILLFACT;
			if ( !GET_PLAY_SKILLFACT ( dwID, sSKILLFACT ) )
			{
				GASSERT ( 0 && "스킬 데이타 찾기 실패" );
				continue;
			}

			const float& fAGE = sSKILLFACT.fAGE;

			pControl->SetGlobalPos ( m_pSkillUnitDummy[nPLAY_SKILL]->GetGlobalPos () );
			pControl->SetLocalPos ( m_pSkillUnitDummy[nPLAY_SKILL]->GetLocalPos () );
			pControl->SetLeftTime ( fAGE );

			nPLAY_SKILL++;
		}
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		float fGAP = m_fBUFF_BACK_SIZE_X - m_fBUFF_SIZE_X;

		const UIRECT& rcGlboalPosOld = GetLocalPos ();
		UIRECT rcGlobalPosNew ( 0.0f, 0.0f, (m_fBUFF_SIZE_X+1.0f) * nPLAY_SKILL + fGAP, rcGlboalPosOld.sizeY );

		SetAlignFlag ( UI_FLAG_XSIZE );
		m_pLineBox->SetAlignFlag ( UI_FLAG_XSIZE );
		{
			AlignSubControl ( rcGlboalPosOld, rcGlobalPosNew );
			SetLocalPos ( rcGlobalPosNew );
			SetGlobalPos ( rcGlobalPosNew );
		}
		m_pLineBox->ResetAlignFlag ();
		ResetAlignFlag ();

		SetGlobalPos ( D3DXVECTOR2 ( (float)x + 25, (float)y + 25 ) );
	}
}

bool CPartyBuffInfo::GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT )
{
	//	NOTE
	//		PARTY SLOT에 따라 SKILLFACT를 들고 온다.
	GLPARTY_CLIENT *pMember = NULL;
	if ( m_nPartyID == 0 )	//	마스터인가?
		pMember = GLPartyClient::GetInstance().GetMaster ();
	else
		pMember = GLPartyClient::GetInstance().GetMember ( m_nPartyID-1 );
	if ( !pMember ) return false;

	bool bSELF = false;
	if ( pMember == FindSelfClient () ) bSELF = true;

	PGLCHARCLIENT pCHAR = NULL;
	if ( !bSELF )
	{
		pCHAR = GLGaeaClient::GetInstance().GetChar ( pMember->m_dwGaeaID );
		if ( !pCHAR ) return false;
	}

	for ( int i = 0; i < SKILLFACT_SIZE; i++ )
	{
		SSKILLFACT* pSkillFactIter = NULL;
		if ( bSELF )
			pSkillFactIter = &(GLGaeaClient::GetInstance().GetCharacter()->m_sSKILLFACT[i]);
		else
			pSkillFactIter = pCHAR->GET_SKILLFACT ( i );

		if ( !pSkillFactIter ) continue;

		if ( pSkillFactIter->sNATIVEID == dwSkillID )
		{
			sSKILLFACT = *pSkillFactIter;
			return true;
		}
	}

	return false;
}

void CPartyBuffInfo::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );
}

HRESULT CPartyBuffInfo::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CPartyBuffInfo::NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL )
{
	m_NEWID++;
	if ( SKILLTIME_UNIT_END <= m_NEWID ) m_NEWID = SKILLTIME_UNIT;

	float fLifeTime = 0.0f;
	if ( !GET_SKILL_LIFE_TIME ( dwSkillID, wLEVEL, fLifeTime ) ) return false;

	SNATIVEID sNativeID ( dwSkillID );
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	CPartyBuffInfoUnit* pSkillTimeUnit = new CPartyBuffInfoUnit;
	pSkillTimeUnit->CreateSub ( this, "PARTYBUFF_UNIT", UI_FLAG_DEFAULT, m_NEWID );
	pSkillTimeUnit->CreateSubControl ();
	pSkillTimeUnit->InitDeviceObjects ( m_pd3dDevice );
	pSkillTimeUnit->RestoreDeviceObjects ( m_pd3dDevice );	
	pSkillTimeUnit->SetSkill ( dwSkillID, fLifeTime, pSkill->GetName() );
	RegisterControl ( pSkillTimeUnit );	

	SKILLTIME_PAIR pair (dwSkillID,m_NEWID);
	m_SkillPlayList.push_back ( pair );

	return true;
}

bool CPartyBuffInfo::GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime )
{
	SNATIVEID sNativeID ( dwSkillID );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	//	최대 지연시간
	fLifeTime = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL].fLIFE;
	fLifeTime = (fLifeTime)?fLifeTime:1.0f;

	return true;
}

void CPartyBuffInfo::SetPartyID ( const int nPartyID )
{
	m_nPartyID = nPartyID;
}


GLPARTY_CLIENT*	CPartyBuffInfo::FindSelfClient ()
{
	//	마스터가 아니면, 자식들을 찾아본다.
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	마스터
	if ( pMaster && pMaster->ISONESELF() )
	{
		return pMaster;
	}
	else
	{
		//	자기찾기
		for ( int i = 0; i < 7; i++ )
		{
			GLPARTY_CLIENT *pMember = GLPartyClient::GetInstance().GetMember( i );
			if ( pMember && pMember->ISONESELF () )
			{
				return pMember;				
			}
		}
	}

//	GASSERT ( 0 && "자신을 찾을 수 없습니다." );
	return NULL;
}
