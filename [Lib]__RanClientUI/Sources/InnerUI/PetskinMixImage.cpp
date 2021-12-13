#include "pch.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include ".\petskinmiximage.h"

CPetSkinMixImage::CPetSkinMixImage(void)
:	m_pAniBox(NULL)
,	m_ItemPosX(0)
,	m_ItemPosY(0)
,	m_sItemID(NATIVEID_NULL ())
{
	
}

CPetSkinMixImage::~CPetSkinMixImage(void)
{
}

void CPetSkinMixImage::CreateSubControl()
{

	CBasicAnimationBox* pAnibox = new CBasicAnimationBox;
	pAnibox->CreateSubEx(this,"PETSKIN_MIX_IMAGE_WINDOW",PETSKIN_MIX_IMAGE_WINDOW);
	pAnibox->CreateAni("PETMIX_ANIMATION_SHUFFLE1", CBasicAnimationBox::NORMAL_ANI);
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE2");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE3");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE4");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE5");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE6");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE7");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE8");
	pAnibox->SetAniTime(0.1f);
	pAnibox->SetEndTime(2.0f);
	pAnibox->SetLoop(FALSE);
	RegisterControl( pAnibox );
	m_pAniBox = pAnibox;

	CUIControl* pControl = CreateControl( "PETMIX_ANIMATION_IMAGE" );
//	pControl->SetAlignFlag( UI_FLAG_CENTER_X );



}


CBasicTextButton* CPetSkinMixImage::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

void CPetSkinMixImage::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_pAniBox->InitAnimateFrame();
		m_pAniBox->SetPause(FALSE);

	}
	else
	{	
		CInnerInterface::GetInstance().HideGroup( GetWndID () );

	}
}

void CPetSkinMixImage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_pAniBox->GetEndAnimate() )
	{
		CInnerInterface::GetInstance().HideGroup( GetWndID() );
//		GLGaeaClient::GetInstance().PetSkinPackApplyEffect();

		if( m_sItemID != NATIVEID_NULL() )
		{
			GLMSG::SNETPET_SKINPACKOPEN NetMsg;
			NetMsg.wPosX   = m_ItemPosX;
			NetMsg.wPosY   = m_ItemPosY;
			NetMsg.sItemId = m_sItemID;
			NETSENDTOFIELD ( &NetMsg );
		}
		
	}

}

void CPetSkinMixImage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if( m_pAniBox->GetPause() == FALSE ) return;

	/*if( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	{
		switch ( ControlID )
		{

		case ET_CONTROL_BUTTON:
			CInnerInterface::GetInstance().HideGroup( GetWndID() );
			break;
		}
	}*/

	//CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}