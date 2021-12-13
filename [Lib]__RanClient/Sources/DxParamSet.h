#ifndef DXPARAMSET_H_
#define DXPARAMSET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3dapp.h"

namespace DXPARAMSET
{
	struct STESTSET
	{
		BOOL			m_bDIS_CONFT;		//	'�ڵ�' ��� �ź�.
		BOOL			m_bDIS_TRADE;		//	'�ڵ�' �ŷ� �ź�.
		BOOL			m_bDIS_PARTY;		//	'�ڵ�' ��Ƽ �ź�.
		BOOL			m_bSHOW_SIMPLEHP;	//	������ ��ǥ��
//		BOOL			m_bDIS_FRIEND;		//	'�ڵ�' ģ�� ��û �ź�.

		DWORD			m_dwScrWidth;
		DWORD			m_dwScrHeight;
		EMSCREEN_FORMAT	m_emScrFormat;

		UINT			m_uScrRefreshHz;
		BOOL			m_bScrWindowed;

		DWORD			m_dwVideoLevel;
		DWORD			m_dwShadowChar;
		BOOL			m_bBuff;
		BOOL			m_bShadowLand;
		BOOL			m_bRealReflect;
		DWORD			m_dwSkinDetail;
		BOOL			m_bGlow;
		BOOL			m_bPost;
		BOOL			m_bRefract;
		BOOL			m_bFrameLimit;

		DWORD			m_dwFogRange;

		STESTSET () :
			m_bDIS_CONFT(FALSE),
			m_bDIS_TRADE(FALSE),
			m_bDIS_PARTY(FALSE),
			m_bSHOW_SIMPLEHP(FALSE),

			m_dwScrWidth(800),
			m_dwScrHeight(600),
			m_emScrFormat(EMSCREEN_F16),
			m_uScrRefreshHz(60),
			m_bScrWindowed(FALSE),

			m_dwShadowChar(4),
			m_bBuff(TRUE),
			m_bShadowLand(TRUE),
			m_bRealReflect(TRUE),
			m_dwSkinDetail(2),
			m_bGlow(TRUE),
			m_bPost(TRUE),
			m_bRefract(TRUE),
			m_bFrameLimit(TRUE),
			m_dwFogRange(0)
		{
		};

		void VideoOK( CD3DApplication *pD3dApp );
		void GameOK();
		void AudioOK();
		void CANCEL( CD3DApplication *pD3dApp );

	private:
		void APPLY ( CD3DApplication *pD3dApp );
	};

	extern STESTSET& GetInstance();

	void	INIT ( );
	void	LOADDATA ();
};

#endif // DXPARAMSET_H_