#ifndef DXCHARSHAM_H_
#define DXCHARSHAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxCustomTypes.h"
#include "DxSkinChar.h"
#include "GLogicEx.h"

#include "GLContrlMsg.h"
#include "GLogicEx.h"
#include "s_CSMsgList.h"

class DxCharSham : public GLCHARLOGIC
{
public:
	enum
	{
		RECEIVE_CHARDATA	= 0x0001,
	};

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*			m_pSkinChar;
	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	BOOL				m_bSelect;
	BOOL				m_bEffect;

	//	Note : 현제 상태.
	//
	float				m_fAge;
	EMACTIONTYPE		m_Action;
	float				m_fIdleTime;
	BOOL				m_bPeaceZone;
	BOOL				m_bIdleReserv;

	//	Note : AABB
	//
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 기초 대이터.
	//
protected:
	int					m_nID;
	BOOL				m_bNeedData;
	DWORD				m_dwRevData;
	SCHARINFO_LOBBY		m_CharInfo;

public:
	bool IsValidData ()				{ return (m_dwRevData&RECEIVE_CHARDATA)!=NULL; }

public:
	void GetAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	void SetSelect ( BOOL bSel, BOOL bReserv = FALSE );	

public:
	HRESULT Create( D3DXVECTOR3* pvPos, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread, BOOL bNeedData=FALSE, int nID=0 );
	HRESULT UpdateSuit ();

	HRESULT SetCharInfo ( const SCHARINFO_LOBBY &CharInfo, const BOOL bThread );
	void ResetCharInfo ()					{ m_dwRevData = NULL; }

	void SetDirection ( D3DXVECTOR3 vDir )	{ m_vDir = vDir; }

protected:
	void TurnAction ( EMACTIONTYPE toAction );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	DxCharSham(void);
	~DxCharSham(void);
};

#endif // DXCHARSHAM_H_