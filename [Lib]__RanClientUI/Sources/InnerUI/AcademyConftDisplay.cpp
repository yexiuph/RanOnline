#include "pch.h"
#include "AcademyConftDisplay.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CAcademyConftDisplay::fALPHA_FULL_TIME = 1.0f;

CAcademyConftDisplay::CAcademyConftDisplay () :
	m_pNumberLEFT( NULL ),
	m_pMarkLEFT( NULL ),
	m_pNumberRIGHT( NULL ),
	m_pMarkRIGHT( NULL ),
	m_pd3dDevice ( NULL ),
	m_NEWID ( BASE_CONTROL_ID ),
	m_fUpdateNumberTime ( 0.0f ),
	m_bUSE_UPDATE_ALPHA ( false ),
	m_nNumberLEFT ( 0 ),	//	나올수 없는 값임, 초기화용
	m_nNumberRIGHT ( 0 ),
	m_nMarkLEFT ( INIT_SCHOOL ),	
	m_nMarkRIGHT ( INIT_SCHOOL )
{
}

CAcademyConftDisplay::~CAcademyConftDisplay ()
{
}

HRESULT CAcademyConftDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

CUIControl*	CAcademyConftDisplay::REGISTER_NUMBER ( const int& nNUMBER )
{
	if ( nNUMBER < 1 || 8 < nNUMBER )
	{
		GASSERT ( 0 && "학원 대련 '숫자'입력이 잘못되었습니다" );
		return NULL;
	}

	static const CString strNUMBER = "ACADEMY_PARTY_CONFT_NUM";

	CString strCombine;
	strCombine.Format ( "%s%d", strNUMBER, nNUMBER );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, strCombine.GetString (), UI_FLAG_DEFAULT, MAKE_NEWID () );
	pControl->InitDeviceObjects ( m_pd3dDevice );
	pControl->RestoreDeviceObjects ( m_pd3dDevice );
	RegisterControl ( pControl );
	return pControl;
}

CUIControl*	CAcademyConftDisplay::REGISTER_MARK ( const int& nMARK )
{
	if ( nMARK < 0 || 2 < nMARK )
	{
		GASSERT ( 0 && "학원 대련 '학원'입력이 잘못되었습니다" );
		return NULL;
	}

	static const CString strMARK = "ACADEMY_PARTY_CONFT_MARK";

	CString strCombine;
	strCombine.Format ( "%s%d", strMARK, nMARK );

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, strCombine.GetString (), UI_FLAG_DEFAULT, MAKE_NEWID () );
	pControl->InitDeviceObjects ( m_pd3dDevice );
	pControl->RestoreDeviceObjects ( m_pd3dDevice );
	RegisterControl ( pControl );
	return pControl;
}

bool CAcademyConftDisplay::INIT_CONTROL ( P_CUICONTROL& pControl )
{
	if ( !pControl ) return true;
    
	const UIGUID& cID = pControl->GetWndID ();

	BOOL bResult = FALSE;
	bResult = DeleteControl ( cID, 0 );
	if ( !bResult ) return false;

	pControl = NULL;

	return true;
}

bool CAcademyConftDisplay::SET_MARK_L ( const int& nMARK )
{
	if ( nMARK != m_nMarkLEFT )
	{
		m_nMarkLEFT = nMARK;
		if ( !INIT_CONTROL ( m_pMarkLEFT ) ) return false;
		CUIControl* pMark = REGISTER_MARK ( nMARK );
		if ( !pMark ) return false;
		m_pMarkLEFT = pMark;			

		UPDATE_MARK_POS_L ();
	}
	return true;
}

bool CAcademyConftDisplay::SET_MARK_R ( const int& nMARK )
{
	if ( nMARK != m_nMarkRIGHT )
	{
		m_nMarkRIGHT = nMARK;
		if ( !INIT_CONTROL ( m_pMarkRIGHT ) ) return false;
		CUIControl* pMark = REGISTER_MARK ( nMARK );
		if ( !pMark ) return false;
		m_pMarkRIGHT = pMark;

		UPDATE_MARK_POS_R ();
	}
	return true;
}

bool CAcademyConftDisplay::SET_NUMBER_L ( const int& nNUMBER )
{
	if ( nNUMBER < 0 || 8 < nNUMBER ) return false;

	if ( nNUMBER != m_nNumberLEFT )
	{
		m_nNumberLEFT = nNUMBER;
		if ( !INIT_CONTROL ( m_pNumberLEFT ) ) return false;
		CUIControl* pNumber = REGISTER_NUMBER ( nNUMBER );
		if ( !pNumber ) return false;
		m_pNumberLEFT = pNumber;

		UPDATE_NUMBER_POS_L ();
	}
	return true;
}

bool CAcademyConftDisplay::SET_NUMBER_R ( const int& nNUMBER )
{
	if ( nNUMBER < 0 || 8 < nNUMBER ) return false;

	if ( nNUMBER != m_nNumberRIGHT )
	{
		m_nNumberRIGHT = nNUMBER;
		if ( !INIT_CONTROL ( m_pNumberRIGHT ) ) return false;
		CUIControl* pNumber = REGISTER_NUMBER ( nNUMBER );
		if ( !pNumber ) return false;
		m_pNumberRIGHT = pNumber;

		UPDATE_NUMBER_POS_R ();
	}
	return true;
}

void CAcademyConftDisplay::UPDATE_MARK_POS_L ()
{
	if ( m_pMarkLEFT )
	{
		const UIRECT& rcPos = m_pMarkLEFTDummy->GetGlobalPos();
		const D3DXVECTOR2 vPos ( rcPos.left, rcPos.top );
		m_pMarkLEFT->SetGlobalPos ( vPos );
	}
}

void CAcademyConftDisplay::UPDATE_MARK_POS_R ()
{
	if ( m_pMarkRIGHT )
	{
		const UIRECT& rcPos = m_pMarkRIGHTDummy->GetGlobalPos();
		const D3DXVECTOR2 vPos ( rcPos.left, rcPos.top );
		m_pMarkRIGHT->SetGlobalPos ( vPos );
	}
}

void CAcademyConftDisplay::UPDATE_NUMBER_POS_L ()
{
	if ( m_pNumberLEFT )
	{
		const UIRECT& rcPos = m_pNumberLEFTDummy->GetGlobalPos();
		const D3DXVECTOR2 vPos ( rcPos.left, rcPos.top );
		m_pNumberLEFT->SetGlobalPos ( vPos );
	}
}

void CAcademyConftDisplay::UPDATE_NUMBER_POS_R ()
{
	if ( m_pNumberRIGHT )
	{
		const UIRECT& rcPos = m_pNumberRIGHTDummy->GetGlobalPos();
		const D3DXVECTOR2 vPos ( rcPos.left, rcPos.top );
		m_pNumberRIGHT->SetGlobalPos ( vPos );
	}
}

void CAcademyConftDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	UPDATE_MARK_POS_L ();
	UPDATE_MARK_POS_R ();
	UPDATE_NUMBER_POS_L ();
	UPDATE_NUMBER_POS_R ();

	if ( m_bUSE_UPDATE_ALPHA )
	{
		m_fUpdateNumberTime -= fElapsedTime;

		float fPercent = (m_fUpdateNumberTime/fALPHA_FULL_TIME);
		if ( fPercent < 0.4f )
		{		
			m_bUSE_UPDATE_ALPHA = false;
			fPercent = 0.4f;
		}
		WORD wALPHA = (WORD)ceil(UCHAR_MAX * fPercent);
		UPDATE_ALPHA ( wALPHA );
	}
}

void CAcademyConftDisplay::CreateSubControl ()
{	
	m_pNumberLEFTDummy = CreateControl ( "ACADEMY_PARTY_CONFT_LEFT_NUMBER" );
	m_pNumberLEFTDummy->SetVisibleSingle ( FALSE );

	m_pMarkLEFTDummy = CreateControl ( "ACADEMY_PARTY_CONFT_LEFT_MARK" );
	m_pMarkLEFTDummy->SetVisibleSingle ( FALSE );

	m_pNumberRIGHTDummy = CreateControl ( "ACADEMY_PARTY_CONFT_RIGHT_NUMBER" );
	m_pNumberRIGHTDummy->SetVisibleSingle ( FALSE );

	m_pMarkRIGHTDummy = CreateControl ( "ACADEMY_PARTY_CONFT_RIGHT_MARK" );	
	m_pMarkRIGHTDummy->SetVisibleSingle ( FALSE );
}

CUIControl*	CAcademyConftDisplay::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

UIGUID	CAcademyConftDisplay::MAKE_NEWID ()
{
	m_NEWID++;
	if ( BASE_CONTROL_ID_END <= m_NEWID ) m_NEWID = BASE_CONTROL_ID;
	return m_NEWID;
}

void CAcademyConftDisplay::UPDATED_NUMBER ()
{
	UPDATE_ALPHA ( USHRT_MAX );

	m_fUpdateNumberTime = fALPHA_FULL_TIME;
	m_bUSE_UPDATE_ALPHA = true;
}

void CAcademyConftDisplay::UPDATE_ALPHA ( const WORD wAlpha )
{
	if ( m_pMarkLEFT )		m_pMarkLEFT->SetDiffuseAlpha ( wAlpha );
	if ( m_pMarkRIGHT )		m_pMarkRIGHT->SetDiffuseAlpha ( wAlpha );
	if ( m_pNumberLEFT )	m_pNumberLEFT->SetDiffuseAlpha ( wAlpha );
	if ( m_pNumberRIGHT )	m_pNumberRIGHT->SetDiffuseAlpha ( wAlpha );
}

int	CAcademyConftDisplay::GET_MARK_L ()
{
	return m_nMarkLEFT;
}

int	CAcademyConftDisplay::GET_MARK_R ()
{
	return m_nMarkRIGHT;
}

int	CAcademyConftDisplay::GET_NUMBER_L ()
{
	return m_nNumberLEFT;
}

int	CAcademyConftDisplay::GET_NUMBER_R ()
{
	return m_nNumberRIGHT;
}