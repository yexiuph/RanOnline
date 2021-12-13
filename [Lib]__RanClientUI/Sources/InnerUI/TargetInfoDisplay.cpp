#include "pch.h"
#include "./TargetInfoDisplay.h"

#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicLineBoxEx.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTargetInfoDisplay::CTargetInfoDisplay () :
	m_pNameBoxDummy ( NULL )
{
	//memset ( m_pSchoolMark, 0, sizeof ( CUIControl* ) * nSCHOOLMARK );
	//memset ( m_pPartyMark, 0, sizeof ( CUIControl* ) * nPARTYMARK );
}

CTargetInfoDisplay::~CTargetInfoDisplay ()
{
}

void CTargetInfoDisplay::CreateSubControl ()
{
	m_pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_TARGETINFO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxTargetInfo ( "BASIC_TARGETINFO_LINE_BOX" );
	RegisterControl ( pLineBox );

	CBasicProgressBar* pHP = new CBasicProgressBar;
	pHP->CreateSub ( this, "TARGETINFO_HP" );
	pHP->CreateOverImage ( "TARGETINFO_HP_OVERIMAGE" );
	pHP->SetControlNameEx ( "HP 프로그래스바" );
	pHP->SetAlignFlag ( UI_FLAG_XSIZE );
	RegisterControl ( pHP );
	m_pHP = pHP;

	CBasicTextBox* pNameBox = new CBasicTextBox;
	pNameBox->CreateSub ( this, "TARGETINFO_NAMEBOX" );
    pNameBox->SetFont ( m_pFont8 );
	pNameBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pNameBox->SetControlNameEx ( "HP 텍스트박스" );	
	pNameBox->SetAlignFlag ( UI_FLAG_XSIZE );
	RegisterControl ( pNameBox );
	m_pNameBox = pNameBox;

	m_pNameBoxDummy = CreateControl ( "TARGETINFO_NAMEBOX", UI_FLAG_DEFAULT );

	//{
	//	CString strSchoolMark[] = 
	//	{
	//		"TARGET_DISPLAY_SCHOOL1",
	//		"TARGET_DISPLAY_SCHOOL2",
	//		"TARGET_DISPLAY_SCHOOL3"
	//	};

	//	for ( int i = 0; i < nSCHOOLMARK; i++ )
	//	{
	//		m_pSchoolMark[i] = CreateControl ( strSchoolMark[i].GetString(), UI_FLAG_LEFT );
	//		m_pSchoolMark[i]->SetVisibleSingle ( FALSE );
	//	}

	//	//m_pSchoolMark[0]->SetVisible ( TRUE );
	//}

	//{
	//	CString strPartyMark[] = 
	//	{
	//		"TARGET_DISPLAY_PARTY_MEMBER",
	//		"TARGET_DISPLAY_PARTY_MASTER",
	//		"TARGET_DISPLAY_PARTY_MEMBER_TARGET",
	//		"TARGET_DISPLAY_PARTY_MASTER_TARGET",
	//		"TARGET_DISPLAY_PARTY_MASTER_GENERAL"
	//	};

	//	for ( int i = 0; i < nPARTYMARK; i++ )
	//	{
	//		m_pPartyMark[i] = CreateControl ( strPartyMark[i].GetString(), UI_FLAG_RIGHT );
	//		m_pPartyMark[i]->SetVisibleSingle ( FALSE );
	//	}

	//	//m_pPartyMark[0]->SetVisible ( TRUE );
	//}
}

CUIControl*	CTargetInfoDisplay::CreateControl ( const char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl, wAlignFlag );
	RegisterControl ( pControl );
	return pControl;
}

void CTargetInfoDisplay::SetTargetInfo ( float fPercent, CString strName, D3DCOLOR dwColor ) /*, int nSchoolMark, int nPartyMark )*/
{
	m_pHP->SetPercent ( fPercent );

	m_pNameBox->ClearText ();

	SIZE Size;
	m_pFont8->GetTextExtent ( strName.GetString (), Size );
	
	const UIRECT& rcNameBoxDummy = m_pNameBoxDummy->GetLocalPos();
	float fNameWidth = 0.0f;
	fNameWidth = (float)Size.cx;

	//	NOTE
	//		실제로는 이미지를 정확하게 잘라서, 스케일에 따라 사이즈를 조절해야한다.		
	//		크게 표가 나지 않는것을 감안, 우선은 무시한다. ㅡㅡ;;	
	{
		//{
		//	//	NOTE
		//	//		학교 파티 마크 붙이기
		//	const float fGapMarkName = 1.0f;
		//	const float fSchoolMark = m_pSchoolMark[0]->GetLocalPos ().sizeX + fGapMarkName;
		//	const float fPartyMark = m_pPartyMark[0]->GetLocalPos ().sizeX + fGapMarkName;

		//	long lLineSize = 0l;
		//	lLineSize = (long)fNameWidth;

		//	for ( int i = 0; i < nSCHOOLMARK; i++ ) 
		//	{
		//		m_pSchoolMark[i]->SetVisibleSingle ( FALSE );
		//	}

		//	if ( NODATA < nSchoolMark && nSchoolMark < nSCHOOLMARK )
		//	{				
		//		m_pSchoolMark[nSchoolMark]->SetVisibleSingle ( TRUE );

		//		lLineSize += long ( fSchoolMark );
		//		m_pNameBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
		//	}

		//	for ( int i = 0; i < nPARTYMARK; i++ )
		//	{
		//		m_pPartyMark[i]->SetVisibleSingle ( FALSE );
		//	}

		//	if ( NODATA < nPartyMark && nPartyMark < nPARTYMARK )
		//	{				
		//		m_pPartyMark[nPartyMark]->SetVisibleSingle ( TRUE );

		//		lLineSize += long ( fPartyMark );
		//		m_pNameBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		//	}

		//	fNameWidth = (float)lLineSize;
		//}

		SetAlignFlag ( UI_FLAG_XSIZE );

		if ( fNameWidth < rcNameBoxDummy.sizeX )
		{
			fNameWidth = rcNameBoxDummy.sizeX;
		}

		//	텍스트 박스 크게 리사이징
		const UIRECT& rcNameLocalPos = m_pNameBox->GetLocalPos ();
		const UIRECT& rcNameBoxNew = UIRECT ( rcNameLocalPos.left, rcNameLocalPos.top,
			fNameWidth, rcNameLocalPos.sizeY );
		AlignSubControl ( rcNameLocalPos, rcNameBoxNew );

		const UIRECT& rcLocalPosOld = GetLocalPos ();
		UIRECT rcLocalPosNew = UIRECT ( rcLocalPosOld.left, rcLocalPosOld.top,
			rcLocalPosOld.sizeX + (fNameWidth - rcNameLocalPos.sizeX), rcLocalPosOld.sizeY );

		SetLocalPos ( rcLocalPosNew );
		SetGlobalPos ( rcLocalPosNew );

		//	텍스트 넣기 ( 알아서 잘림 )				
		m_pNameBox->AddTextNoSplit ( strName, dwColor );	
		m_strName = strName;
	}
}