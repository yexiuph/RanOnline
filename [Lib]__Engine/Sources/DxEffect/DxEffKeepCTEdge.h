#pragma once

#include <vector>

#include "./DxEffKeep.h"

typedef std::vector<KEEP_COLORARGB>	VEC_COLORARGB;
typedef VEC_COLORARGB::iterator		VEC_COLORARGB_ITER;

#define	USE_EDGETOSHADECOLOR	0x001

struct EFFKEEP_PROPERTY_CTEDGE : public EFFKEEP_PROPERTY
{
	DWORD			m_dwFlag;			// SAVE // LOAD 
	float			m_fRoopTime;		// 1 단계 넘어가는 시간.SAVE // LOAD 
	VEC_COLORARGB	m_vecColor;			// Color List ..		SAVE // LOAD 

	EFFKEEP_PROPERTY_CTEDGE();
};

class DxEffKeepData_CTEdge : public DxEffKeepData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	void SetProperty( EFFKEEP_PROPERTY *pProperty )	{}
	EFFKEEP_PROPERTY* GetProperty()					{ return &m_sProp; }

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	EFFKEEP_PROPERTY_CTEDGE	m_sProp;

public:
	void InsertColor( DWORD dwColor );
	void DeleteColor( int nIndex );
	void ChageColor( int nIndex, DWORD dwColor );

public:
	virtual DxEffKeep* NEWOBJ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

public:
	DxEffKeepData_CTEdge();
	~DxEffKeepData_CTEdge();
};

class DxEffKeepCTEdge : public DxEffKeep
{
private:
	float	m_fIndexCoolTime;
	DWORD	m_dwIndexCUR;

public:
	EFFKEEP_PROPERTY_CTEDGE	m_sProp;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT FrameMove ( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffKeepCTEdge(void);
	~DxEffKeepCTEdge(void);
};

