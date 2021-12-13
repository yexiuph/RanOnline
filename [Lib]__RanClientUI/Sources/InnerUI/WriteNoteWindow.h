#pragma once

#include "UIWindowEx.h"

class CBasicTextButton;
class CBasicTextBox;
class CUIMultiEditBox;
class CUIEditBox;

class CWriteNoteWindow : public CUIWindowEx
{
	static const int nLIMIT_CHAR;

	enum
	{
		WRITENOTE_TO_TEXTBOX = ET_CONTROL_NEXT,
		WRITENOTE_TO_NAME_TEXTBOX,

		WRITENOTE_SUBJECT_TEXTBOX,
		WRITENOTE_SUBJECT_EDITBOX,

		WRITENOTE_EDITBOX_L01,
		WRITENOTE_EDITBOX_L02,
		WRITENOTE_EDITBOX_L03,
		WRITENOTE_EDITBOX_L04,
		WRITENOTE_EDITBOX_L05,
		WRITENOTE_EDITBOX_L06,
		WRITENOTE_EDITBOX_L07,
		WRITENOTE_EDITBOX_L08,
		WRITENOTE_EDITBOX_L09,
		WRITENOTE_EDITBOX_L10,

		WRITENOTE_REWRITE_BUTTON,
		WRITENOTE_SEND_BUTTON
	};

public:
	CWriteNoteWindow(void);
	virtual ~CWriteNoteWindow(void);

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void EDIT_END();
	void EDIT_BEGIN( const CString& strName = "" );

protected:
	CBasicTextBox * m_pNameTextBox;
	CUIEditBox * m_pSubjectEditBox;

	CUIMultiEditBox * m_pMultiEditBox;

	CBasicTextButton * m_pRewriteButton;
	CBasicTextButton * m_pSendButton;
};