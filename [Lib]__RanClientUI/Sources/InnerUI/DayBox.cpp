#include "pch.h"
#include "DayBox.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/G-logic/GLdefine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CDayBox::CDayBox()
	: m_nDay(0)
	, m_nDayOfWeek(0)
	, m_dwColor(0)
	, m_bAttendance(false)
	, m_bAttendReward(false)
	, m_pNumberImage ( NULL )
	, m_pNumberImageTen ( NULL )
	, m_pAttendanceMark ( NULL ) 
	, m_pAttendRewardMark ( NULL ) 
{
}

CDayBox::~CDayBox()
{
}

CUIControl* CDayBox::CreateControl( const char * szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void CDayBox::CreateSubControl()
{
	m_pNumberImage = CreateControl( "NUMBER_IMAGE" );
	m_pNumberImageTen = CreateControl( "NUMBER_IMAGE_TEN" );
	m_pAttendanceMark = CreateControl( "ATTENDANCE_MARK" );
	m_pAttendRewardMark = CreateControl( "ATTEND_REWARD_MARK" );
}

void CDayBox::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CDayBox::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		if ( m_nDay < 10 )	m_pNumberImageTen->SetVisibleSingle( FALSE );
		else m_pNumberImageTen->SetVisibleSingle( TRUE );
	}
}

void CDayBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CDayBox::CalcTexturePos()
{
	UIRECT rcTexPos, rcPos;
	
	rcPos = m_pNumberImage->GetGlobalPos();
	rcTexPos = m_pNumberImage->GetTexturePos();
	rcTexPos.left = ( rcPos.sizeX / 512.0f ) * ( m_nDay % 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pNumberImage->SetTexturePos( rcTexPos );

	rcPos = m_pNumberImageTen->GetGlobalPos();
	rcTexPos = m_pNumberImageTen->GetTexturePos();
	rcTexPos.left = ( rcPos.sizeX / 512.0f ) * ( m_nDay / 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pNumberImageTen->SetTexturePos( rcTexPos );
}

void CDayBox::SetDay ( int nDay, int nDayOfWeek )
{
	m_nDay = nDay;
	m_nDayOfWeek = nDayOfWeek;
	
	if ( m_nDayOfWeek == 0 )	m_dwColor = NS_UITEXTCOLOR::RED;
	else if ( m_nDayOfWeek == 6 ) m_dwColor = NS_UITEXTCOLOR::LIGHTSKYBLUE;
	else m_dwColor = NS_UITEXTCOLOR::BLACK;

	CalcTexturePos();	
}

void CDayBox::SetAttendance( bool bAttendance )
{
	m_bAttendance = bAttendance;
	m_pAttendanceMark->SetVisibleSingle( bAttendance );
}

void CDayBox::SetAttendReward( bool bAttendReward )
{
	m_bAttendReward = bAttendReward;
	m_pAttendRewardMark->SetVisibleSingle( bAttendReward );
}

void CDayBox::SetMarkPos ()
{
	
	const int nRandomPos = 5;

	float fLeft, fTop;

	const UIRECT& rcLocalPosOldParent = GetLocalPos();
	const UIRECT& rcGlobalPosOldParent = GetGlobalPos();

	// AttendMark
	{
		const UIRECT& rcLocalPosOldChild = m_pAttendanceMark->GetLocalPos();
		const UIRECT& rcGlobalPosOldChild = m_pAttendanceMark->GetGlobalPos();

		// Áß¾Ó Á¤·Ä
		fLeft = rcGlobalPosOldParent.left + ( ( rcGlobalPosOldParent.sizeX - rcGlobalPosOldChild.sizeX ) / 2 );
		fTop = rcGlobalPosOldParent.top + ( ( rcGlobalPosOldParent.sizeY - rcGlobalPosOldChild.sizeY ) / 2 );
			
		// ·£´ý Àû¿ë
		fLeft += RANDOM_NUM * nRandomPos;
		fTop += RANDOM_NUM * nRandomPos;

		UIRECT rcGlobalPosNew = UIRECT( fLeft, fTop,
								rcGlobalPosOldChild.sizeX, rcGlobalPosOldChild.sizeY );

		m_pAttendanceMark->AlignSubControl ( rcGlobalPosOldChild, rcGlobalPosNew );

		m_pAttendanceMark->SetLocalPos ( D3DXVECTOR2 ( rcLocalPosOldChild.left, rcLocalPosOldChild.top ) );
		m_pAttendanceMark->SetGlobalPos ( rcGlobalPosNew );
	}


	// Reward ¸¶Å©
	{
		const UIRECT& rcLocalPosOldChild = m_pAttendRewardMark->GetLocalPos();
		const UIRECT& rcGlobalPosOldChild = m_pAttendRewardMark->GetGlobalPos();

		// Áß¾Ó Á¤·Ä
		fLeft = rcGlobalPosOldParent.left + ( ( rcGlobalPosOldParent.sizeX - rcGlobalPosOldChild.sizeX ) / 2 );
		fTop = rcGlobalPosOldParent.top + ( ( rcGlobalPosOldParent.sizeY - rcGlobalPosOldChild.sizeY ) / 2 );
			
		// ·£´ý Àû¿ë
		fLeft += RANDOM_NUM * nRandomPos;
		fTop += RANDOM_NUM * nRandomPos;

		UIRECT rcGlobalPosNew = UIRECT( fLeft, fTop,
								rcGlobalPosOldChild.sizeX, rcGlobalPosOldChild.sizeY );

		m_pAttendRewardMark->AlignSubControl ( rcGlobalPosOldChild, rcGlobalPosNew );

		m_pAttendRewardMark->SetLocalPos ( D3DXVECTOR2 ( rcLocalPosOldChild.left, rcLocalPosOldChild.top ) );
		m_pAttendRewardMark->SetGlobalPos ( rcGlobalPosNew );
	}

}

void CDayBox::ResetData()
{
	SetAttendance ( false );
	SetAttendReward ( false );
}

HRESULT CDayBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_nDay == 28 )
	{
		int a =0; 
	}

	return CUIGroup::Render( pd3dDevice );
}