//-----------------------------------------------------------------------
//
//		CLASS   DXInputString
//
//		Using IME -
//		Link : "imm32.lib"
//
//		coding : 2000/07/11 ~ 12
//		by jdh
//
//		- ���� : 2000/12/15 - ���ڿ� �Է½� Input ScanCode ��� ����.
//		- ���� : 2000/12/21 - TAB, ESCAPE, RETURN Ű �ν�.
//                          - TAB ���� ����.
//		- ���� : 2002/08/08 - �̱��� �������� ����.
//		- ���� : 2002/08/09 - OnChar() ����, ImmGetOpenStatus(m_hIMC)��
//							���� ���� ���� �Է� ���θ� �����ϰ� ����.
//
//
//-----------------------------------------------------------------------
#pragma once

#include <Imm.h>
#include <string>
#include "IMEEdit.h"

enum EMLANGFLAG
{
	EMLANGFLAG_DEFAULT	= 0,
	EMLANGFLAG_CHN_TRA	= 1,
	EMLANGFLAG_JP		= 2,
	EMLANGFLAG_KR		= 3,
	EMLANGFLAG_CHN_SIM	= 4,
	EMLANGFLAG_TH		= 5
};

class DXInputString
{
public:
	static BOOL	m_bCaratMove;

protected:
	BOOL		m_bOn;
	std::string	m_strString;

	CWnd*		m_pParentWnd;
	CIMEEdit	m_editIME;

public:
	DXInputString ();

	void Create ( CWnd *_pWnd, CRect &rtPosition );
	void Move ( CRect &rtPosition );

public:
	BOOL OnInput ();
	BOOL OffInput ();
	BOOL IsOn ();

public:
	bool CheckEnterKeyDown ()					{ return m_editIME.CheckEnterKeyDown(); }
	bool CheckComposition ()					{ return m_editIME.CheckComposition(); }
	bool CheckWideCaret ()						{ return m_editIME.CheckWideCaret(); }

public:
	int GetLanguage ()							{ return m_editIME.GetLanguage(); }

	void ConverseNativeToEng()					{ m_editIME.ConverseNativeToEng(); }
	void ConverseEngToNative()					{ m_editIME.ConverseEngToNative(); }
	void ConverseFullToHalf()					{ m_editIME.ConverseFullToHalf(); }

	BOOL IsNativeMode ()						{ return m_editIME.IsNativeMode(); }

public:
	const char* GetString ()					{ return m_editIME.GetString(); }
    void SetString ( const char* pStr = NULL )	{ m_editIME.SetString( pStr ); }

public:
	int GetInsertPos()							{ return m_editIME.GetInsertPos(); }
	int SetInsertPos( int xPos )				{ return m_editIME.SetInsertPos( xPos ); }

public:
	void DisableKeyInput()						{ m_editIME.DisableKeyInput(); }
	void UsableKeyInput()						{ m_editIME.UsableKeyInput(); }

public:
	static DXInputString& GetInstance();
};