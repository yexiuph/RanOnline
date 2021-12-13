#pragma once

#include "UIWindowEx.h"

class CBasicTextButton;
class CBasicTextBox;

class CReceiveNoteWindow : public CUIWindowEx
{
	enum
	{
		RECEIVENOTE_FROM_TEXTBOX = ET_CONTROL_NEXT,
		RECEIVENOTE_FROM_NAME_TEXTBOX,

		RECEIVENOTE_SUBJECT_TEXTBOX,
		RECEIVENOTE_SUBJECT_NAME_TEXTBOX,

		RECEIVENOTE_TEXT_TEXTBOX,

		RECEIVENOTE_REPLY_BUTTON
	};

public:
	CReceiveNoteWindow(void);
	virtual ~CReceiveNoteWindow(void);

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );

protected:
	CBasicTextBox * m_pFromNameTextBox;
	CBasicTextBox * m_pSubjectNameTextBox;

	CBasicTextBox * m_pTextTextBox;

	CBasicTextButton * m_pReplyButton;
};
