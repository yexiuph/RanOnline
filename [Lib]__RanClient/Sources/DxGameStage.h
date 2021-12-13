#ifndef DXGAMESTAGE_H_
#define DXGAMESTAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxStage.h"
#include "GLCharacter.h"
#include "D3DApp.h"

struct DxCharJoinData
{
	bool				m_bCharJoin;
	DWORD				m_dwCharID;				//	받고 있는 케릭터의 ID.

	DWORD				m_dwClientID;			//	클라이언트 ID
	DWORD				m_dwGaeaID;				//	생성 메모리 인덱스용.
	SNATIVEID			m_sMapID;				//	맵 ID.
	D3DXVECTOR3			m_vPos;					//	맵 상의 위치.

	//	Note : 추가로 받아야 할 목록 개수.
	//
	DWORD				m_dwPutOnItems;
	DWORD				m_dwNumInvenItem;		//	인밴에 들어있는 아이템 갯수.
	DWORD				m_dwNumVNInvenItem;		//	베트남 인벤에 들어있는 아이템 갯수
	DWORD				m_dwNumSkill;			//	배운 스킬 목록.
	DWORD				m_dwNumQuestProc;		//	진행중인 퀘스트.
	DWORD				m_dwNumQuestEnd;		//	완료된 퀘스트.
	DWORD				m_dwChargeItem;			//	구입 아이템 목록.
	DWORD				m_dwClubMember;			//	클럽 구성원.
	DWORD				m_dwClubAlliance;		//	클럽 동맹.
	DWORD				m_dwClubBattle;			//	클럽 배틀.
	DWORD				m_dwItemCoolTime;		//	아이템 쿨타임

	//	Note : 수신한 목록 개수.
	DWORD				m_dwReceiveInvenItem;	//	인밴에 들어있는 아이템 갯수.
	DWORD				m_dwReceiveVNInvenItem;		//	베트남 인벤에 들어있는 아이템 갯수
	DWORD				m_dwReceiveSkill;		//	배운 스킬 목록.
	DWORD				m_dwReceiveQuestProc;	//	진행중인 퀘스트.
	DWORD				m_dwReceiveQuestEnd;	//	완료된 퀘스트.
	DWORD				m_dwReceiveChargeItem;	//	구입 아이템 목록.
	DWORD				m_dwReceiveClubMember;	//	클럽 맴버.
	DWORD				m_dwReceiveClubAlliance;//	클럽 동맹.
	DWORD				m_dwReceiveClubBattle;	//	클럽 배틀
	DWORD				m_dwReceiveItemCoolTime; //	아이템 쿨타임

	bool				m_bReceiveClubInfo;

	GLCLUB				m_cCLUB;
	SCHARDATA2			m_CharData2;			//	캐릭 기본 데이터.

	DxCharJoinData () :
		m_bCharJoin(false),
		m_dwCharID(0),
		m_dwClientID(0),
		m_dwGaeaID(0),
		m_sMapID(0,0),
		m_vPos(0,0,0),

		m_dwPutOnItems(0),
		m_dwNumInvenItem(0),
		m_dwNumVNInvenItem(0),
		m_dwNumSkill(0),
		m_dwNumQuestProc(0),
		m_dwNumQuestEnd(0),
		m_dwChargeItem(0),
		m_dwClubMember(0),
		m_dwClubAlliance(0),
		m_dwClubBattle(0),
		m_dwItemCoolTime(0),

		m_dwReceiveInvenItem(0),
		m_dwReceiveVNInvenItem(0),
		m_dwReceiveSkill(0),
		m_dwReceiveQuestProc(0),
		m_dwReceiveQuestEnd(0),
		m_dwReceiveChargeItem(0),
		m_dwReceiveClubMember(0),
		m_dwReceiveClubAlliance(0),
		m_dwReceiveClubBattle(0),
		m_dwReceiveItemCoolTime(0),

		m_bReceiveClubInfo(false)
	{
	}

	void ResetData ();

	bool CheckReceive ()
	{
		if ( !m_bCharJoin )									return false;
		if ( m_dwPutOnItems != SLOT_TSIZE )					return false;
		if ( m_dwNumInvenItem!=m_dwReceiveInvenItem )		return false;
		if ( m_dwNumSkill!=m_dwReceiveSkill )				return false;
		if ( m_dwNumQuestProc!=m_dwReceiveQuestProc )		return false;
		if ( m_dwNumQuestEnd!=m_dwReceiveQuestEnd )			return false;
		if ( m_dwChargeItem!=m_dwReceiveChargeItem )		return false;
		if ( m_dwItemCoolTime!=m_dwItemCoolTime )			return false;

		if ( !m_bReceiveClubInfo )							return false;
		if ( m_dwClubMember!=m_dwReceiveClubMember )		return false;
		if ( m_dwClubAlliance!=m_dwReceiveClubAlliance )	return false;
		if ( m_dwClubBattle!=m_dwReceiveClubBattle )		return false;

#if defined(VN_PARAM) //vietnamtest%%%
		if ( m_dwNumVNInvenItem!=m_dwReceiveVNInvenItem )	return false;
#endif

		return true;
	}
};

class DxGameStage : public DxStage
{
protected:
	D3DCAPSQ			m_d3dCaps;
	D3DSURFACE_DESC		m_d3dsdBackBuffer;

	DxCharJoinData		m_CharJoinData;

	BOOL				m_bUseInterface;

public:
	void OnInitLogin ();
	
	void ResetCharJoinData ();

	DWORD GetGaeaID ()			{	return m_CharJoinData.m_dwGaeaID; }
	DWORD GetClientID ()		{	return m_CharJoinData.m_dwClientID; }
	const DxCharJoinData& GetCharJoinData()		{	return m_CharJoinData; }

	void SetSelectChar ( SCHARINFO_LOBBY *pCharInfo );

public:
	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath );
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ();
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float m_fTime, float m_fElapsedTime );
	virtual HRESULT Render ();

	virtual void MsgProcess ( NET_MSG_GENERIC* nmg );
	virtual HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight );

public:
	DxGameStage(void);
	~DxGameStage(void);
};

#endif // DXGAMESTAGE_H_