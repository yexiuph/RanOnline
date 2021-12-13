#pragma	once

#include "BasicVarTextBox.h"

class	CPrivateMarketShow : public CBasicVarTextBox
{
public:
    CPrivateMarketShow ();
	virtual	~CPrivateMarketShow ();

public:
	void SetGaeaID( const DWORD& dwGaeaID ) { m_dwGaeaID = dwGaeaID; }
	DWORD GetGaeaID()						{ return m_dwGaeaID; }

	bool IsMemPool()		{ return m_bUsedMemPool; }
	void SetMemPool()		{ m_bUsedMemPool = true; }

	void SetNameString( CString& strName ){ m_strName = strName; }
	CString& GetNameString(){ return m_strName; }

	void SetMarketString( CString& strMarket ){ m_strMarket = strMarket; }
	CString& GetMarketString(){ return m_strMarket; }

private:
	DWORD m_dwGaeaID;
	bool m_bUsedMemPool; // MEMO : 메모리 풀을 사용하기 위한 플래그

	CString m_strName;
	CString m_strMarket;
};