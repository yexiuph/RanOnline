#include "pch.h"
#include "SkillTimeDisplay.h"
#include "GLGaeaClient.h"
#include "SkillTimeUnit.h"
#include "GLSkill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSkillTimeDisplay::CSkillTimeDisplay () :
	m_NEWID ( SKILLTIME_UNIT )
{
}

CSkillTimeDisplay::~CSkillTimeDisplay ()
{
}

void CSkillTimeDisplay::CreateSubControl ()
{	
	const CString strKeyword = "SKILL_TIME_UNIT";
	CString strCombine;

	for ( int i = 0; i < SKILLFACT_SIZE; i++ )
	{
		strCombine.Format ( "%s%d", strKeyword, i );
		CUIControl* pDummyControl = new CUIControl;
		pDummyControl->CreateSub ( this, strCombine.GetString(), UI_FLAG_DEFAULT, SKILLTIME_DUMMY_BASE + i );
		RegisterControl ( pDummyControl );

		m_pSkillUnitDummy[i] = pDummyControl;
	}
}

UIGUID CSkillTimeDisplay::GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID )
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

void CSkillTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	if ( !pCharacter ) return;

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
		for ( int i = 0; i < SKILLFACT_SIZE; i++ )
		{
			const SSKILLFACT& sSKILLFACT = pCharacter->m_sSKILLFACT[i];
			const DWORD& dwID = sSKILLFACT.sNATIVEID.dwID;
			sSKILLFACT.wLEVEL;

			if ( dwID == NATIVEID_NULL().dwID ) continue;

			UIGUID cID = GET_EXIST_PLAY_SKILL_CONTROL ( dwID );
			if ( cID == NO_ID )	NEW_PLAY_SKILL ( dwID, sSKILLFACT.wLEVEL );
		}
	}

	if ( m_SkillPlayList.empty () ) return ;

	//	업데이트
	int nPLAY_SKILL = 0;
	SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
	SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();

	for ( ; iter != iter_end; ++iter )
	{
		const SKILLTIME_PAIR& skill_pair = (*iter);
		const DWORD& dwID = skill_pair.first;
		const UIGUID& cID = skill_pair.second;			

		CSkillTimeUnit* pControl = (CSkillTimeUnit*) FindControl ( cID );
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
		pControl->SetLeftTime ( fAGE );

		nPLAY_SKILL++;
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

bool CSkillTimeDisplay::GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT )
{
	GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	if ( !pCharacter ) return false;

	for ( int i = 0; i < SKILLFACT_SIZE; i++ )
	{
		const SSKILLFACT& sSkillFactIter = pCharacter->m_sSKILLFACT[i];
		if ( sSkillFactIter.sNATIVEID == dwSkillID )
		{
			sSKILLFACT = sSkillFactIter;
			return true;
		}
	}

	return false;
}

HRESULT CSkillTimeDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CSkillTimeDisplay::NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL )
{
	m_NEWID++;
	if ( SKILLTIME_UNIT_END <= m_NEWID ) m_NEWID = SKILLTIME_UNIT;

	float fLifeTime = 0.0f;
	if ( !GET_SKILL_LIFE_TIME ( dwSkillID, wLEVEL, fLifeTime ) ) return false;

	SNATIVEID sNativeID ( dwSkillID );
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	CSkillTimeUnit* pSkillTimeUnit = new CSkillTimeUnit;
	pSkillTimeUnit->CreateSub ( this, "SKILLTIME_UNIT", UI_FLAG_DEFAULT, m_NEWID );
	pSkillTimeUnit->CreateSubControl ();
	pSkillTimeUnit->InitDeviceObjects ( m_pd3dDevice );
	pSkillTimeUnit->RestoreDeviceObjects ( m_pd3dDevice );	
	pSkillTimeUnit->SetSkill ( dwSkillID, fLifeTime, pSkill->GetName() );
	RegisterControl ( pSkillTimeUnit );	

	SKILLTIME_PAIR pair (dwSkillID,m_NEWID);
	m_SkillPlayList.push_back ( pair );

	return true;
}

bool CSkillTimeDisplay::GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime )
{
	SNATIVEID sNativeID ( dwSkillID );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	//	최대 지연시간
	fLifeTime = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL].fLIFE;
	fLifeTime = (fLifeTime)?fLifeTime:1.0f;

	return true;
}