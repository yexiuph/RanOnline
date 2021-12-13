#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class CHeadChatDisplay : public CUIGroup
{
public:
	CHeadChatDisplay ();
	virtual	~CHeadChatDisplay ();

public:
	void CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void SetChat( CString strName, const D3DCOLOR& dwIDColor, CString strChat, const D3DCOLOR& dwChatColor );
	
	void SetLifeTime( float fTime )		{ m_fLifeTime = fTime; }
	float GetLifeTime()					{ return m_fLifeTime; }

	const CString & GetName() const		{ return m_strName; }

	bool IsMemPool()					{ return m_bUsedMemPool; }
	void SetMemPool()					{ m_bUsedMemPool = true; }

private:
	float m_fLifeTime;
	CString m_strName;
	bool m_bUsedMemPool; // MEMO : 메모리 풀을 사용하기 위한 플래그

	CBasicTextBox* m_pChatBox;
	CUIControl* m_pSelfDummy;
};