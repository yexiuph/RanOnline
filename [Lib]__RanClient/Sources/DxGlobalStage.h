#ifndef DXGLOBALSTAGE_H_
#define DXGLOBALSTAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetClient.h"
#include "DxMsgProcessor.h"

#include "DxLobyStage.h"
#include "DxGameStage.h"
#include "GLGaeaServer.h"

#include "s_CLock.h"

#define _PRELOAD_BONE	"preload_skeleton.lst"
#define _PRELOAD_ANI	"preload_ani.lst"
#define _PRELOAD_SKIN	"preload_skin.lst"

typedef struct _AUTH_DATA
{
	GG_AUTH_DATA	gg_Auth_Data;
	UINT			Auth_Msg;

	_AUTH_DATA() :
		Auth_Msg(0)
	{
	}
} AUTH_DATA, *P_AUTH_DATA;

class DxGlobalStage : public DxMsgProcessor
{
public:
	enum EMSTAGE
	{
		EM_STAGE_NULL	= 0,
		EM_STAGE_LOBY	= 1,
		EM_STAGE_GAME	= 2,
	};

	enum EMSTATE
	{
		EM_CHANGE				= 0x0001,
		EM_CLOSE				= 0x0002,

		EM_REQCONNECT_LOGINSVR	= 0x1000,
	};

public:
	typedef std::vector<PBYTE>			MSGBUFFERLIST;
	typedef MSGBUFFERLIST::iterator		MSGBUFFERLIST_ITER;

private:
	typedef std::queue<P_AUTH_DATA>		GG_AUTH_QUEUE;
	GG_AUTH_QUEUE	m_GGAuthBuffer;

public:
	UINT GetGGAuthData( PGG_AUTH_DATA pGG );

protected:
	HWND				m_hWnd;
	WORD				m_wWidth;
	WORD				m_wHeight;
	char				m_szFullPath[MAX_PATH];
	INT					m_nChannel; // 서버의 채널 번호

	BOOL				m_bEmulate;
	DWORD				m_dwState;
	EMSTAGE				m_emChangeStage;
	EMSTAGE				m_emThisStage;

protected:
	DxStage*			m_pActiveStage;
	DxLobyStage			m_LobyStage;
	DxGameStage			m_GameStage;

protected:
	CD3DApplication*	m_pD3DApp;
	D3DCAPSQ			m_d3dCaps;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:
	CNetClient				m_NetClient; // 통신객체

	MSGBUFFERLIST*			m_pMsgActive;
	MSGBUFFERLIST*			m_pMsgReceive;
	
	MSGBUFFERLIST			m_MsgBufferA;
	MSGBUFFERLIST			m_MsgBufferB;

	static CRITICAL_SECTION	m_CSMsgProcLock;

protected:
	DWORD				m_dwBLOCK_PROG_COUNT;
	DWORD				m_dwBLOCK_PROG_IMAGE;
	bool				m_bBLOCK_PROG_DETECTED;
	bool				m_bDETECTED_REPORT;
	float				m_fBLOCK_PROG_TIMER;

public:
	void SetProgDetected ( DWORD dwID );
	void SetDetectedReport ()			{ m_bDETECTED_REPORT = true; }
	bool GetDetectedReport ()			{ return m_bDETECTED_REPORT; }

protected:
	void MsgQueueFlip ();
	MSGBUFFERLIST& GetActiveMsgList ()			{ return *m_pMsgActive; }
	MSGBUFFERLIST& GetReceivedMsgList ()		{ return *m_pMsgReceive; }

public:
	void MsgProcessFrame ();
	void MsgProcessFrame ( NET_MSG_GENERIC *nmg );

public:
	bool GetBlockDetectState ()				{ return m_bDETECTED_REPORT; }
	CNetClient* GetNetClient ()				{ return &m_NetClient; }
	void NetSend ( NET_MSG_GENERIC *nmg );
	void NetSendToField ( NET_MSG_GENERIC *nmg );
	void NetSendEventLottery ( const char* szLotteryName );
	
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg );

public:
	void OnInitLogin ();
	BOOL IsEmulator ()					{ return m_bEmulate; }

	BOOL IsSTATE ( DWORD dwState )		{ return m_dwState&dwState; }
	void SetSTATE ( DWORD dwState )		{ m_dwState |= dwState; }
	void ReSetSTATE ( DWORD dwState )	{ m_dwState &= ~dwState; }

public:
	DxLobyStage* GetLobyStage ()	{ return &m_LobyStage; }
	DxGameStage* GetGameStage ()	{ return &m_GameStage; }
	DxStage* GetActiveStage ()		{ return m_pActiveStage; }

protected:
	HRESULT ChangeStage ( WORD wWidth, WORD wHeight );	//	스테이지 변경 구동.

public:
	HRESULT ToChangeStage ( EMSTAGE ChangeStage );	//	스테이지 변경 요청.
	void CloseGame ( LPCTSTR lpszMsg = NULL );

	HRESULT GameToLobbyStage ();

public:
	void SetD3DApp( CD3DApplication * pD3DApp );

	CD3DApplication* GetD3DApp ();
	LPDIRECT3DDEVICEQ GetD3dDevice ()			{ return m_pd3dDevice; }

	const char* GetAppPath()					{ return m_szFullPath; }

public:
	HRESULT OneTimeSceneInit ( const char* szAppPath, HWND hWnd, BOOL bEmulate=FALSE, WORD wWidth=0, WORD wHeight=0, const char* szMapList=NULL );
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup();

public:
	HRESULT FrameMove ( float m_fTime, float m_fElapsedTime );
	HRESULT Render ();

public:
	LRESULT MsgProc ( MSG* pMsg );
	LRESULT OnNetNotify(WPARAM wParam, LPARAM lParam);

	HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight );

public:
	HRESULT ChangeWndMode ();

public:
	HRESULT ToFullSize ();
	HRESULT ToRestoreSize ();

public:
	// 대만 서버의 채널 관련 함수
	void SetChannel( INT nChannel ) { m_nChannel = nChannel; }
	INT GetChannel() { return m_nChannel; }	// 채널은 0부터 시작, 초기값은 -1

	void SetGameJoin( BOOL bBOOL )	{ m_LobyStage.m_bGameJoin = bBOOL; }

protected:
	DxGlobalStage(void);

public:
	~DxGlobalStage(void);

public:
	static DxGlobalStage& GetInstance();
};

inline void NETSEND ( LPVOID nmg )
{
	DxGlobalStage::GetInstance().NetSend ( (NET_MSG_GENERIC*)nmg );
}

inline void NETSENDTOFIELD ( LPVOID nmg )
{
	DxGlobalStage::GetInstance().NetSendToField ( (NET_MSG_GENERIC*)nmg );
}

inline DWORD GETMYGAEAID ()
{
	return DxGlobalStage::GetInstance().GetGameStage()->GetGaeaID();
}

inline DWORD GETMYCLIENTID ()
{
	return DxGlobalStage::GetInstance().GetGameStage()->GetClientID();
}

#endif // DXGLOBALSTAGE_H_