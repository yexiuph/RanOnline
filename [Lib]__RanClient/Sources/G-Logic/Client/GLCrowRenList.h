#pragma once

//#include <map>
//#include <string>

#include "GLDefine.h"
#include "GLCharDefine.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "GLAnyPet.h"
#include "GLPetClient.h"
#include "GLAnySummon.h"
#include "GLSummonClient.h"
#include "GLMaterialClient.h"

namespace CROWCOLOR
{
	const DWORD MOB_COLOR_GRAY	= D3DCOLOR_ARGB(255,184,184,184);
	const DWORD MOB_COLOR_GREEN	= D3DCOLOR_ARGB(210,39,207,102);
	const DWORD MOB_COLOR_RED	= D3DCOLOR_ARGB(255,255,0,0);
};

enum DISP_PARTY
{
	//	번호 변경 불가 ( 텍스쳐 타입 )
	PARTY_MEMBER			= 0,
	PARTY_MASTER			= 1,
	PARTY_MEMBER_TARGET		= 2,
	PARTY_MASTER_TARGET		= 3,
	PARTY_MASTER_GENERAL	= 4,

    SOLO_TARGET				= 1001,
	OTHER_PLAYER			= 1002,
	NODATA					= USHRT_MAX
};

enum DISP_TYPE
{
	TYPE_NORMAL			= 0x00,	//	일반
	TYPE_TARGET			= 0x01,	//	타겟
	TYPE_OURS			= 0x02,	//	내가 속한 곳인가
	TYPE_PARTYMASTER	= 0x04,	//	파티 마스터
	TYPE_PARTYMEMBER	= 0x08,	//	파티 멤버
	TYPE_CLUBMASTER		= 0x10,	//	클럽 마스터
	TYPE_CLUBMEMBER		= 0x20,	//	클럽 멤버
	TYPE_CLUBCD			= 0x40,	//	CD 인증
	TYPE_CLUBBATTLE		= 0x80, //	클럽배틀

	TYPE_ERROR			= 0x100,	//	오류
};

class GLCharacter;
class GLCharClient;
class GLCrowClient;
class CItemClientDrop;
class CMoneyClientDrop;

struct CROWREN
{
	EMCROW		m_emCROW;
	DWORD		m_dwID;
	WORD		m_wSCHOOL;

	DWORD		m_dwCOLOR;
	DISP_PARTY	m_emPARTY;
	DWORD		m_dwTYPE;
	bool		m_bPMARKET;

	char		m_szNAME[CHAR_SZNAME];
	char		m_szPMARKET[CHAT_MSG_SIZE+1];
	char		m_szNICK[CHAR_SZNAME];

	DWORD		m_dwCLUB;
	DWORD		m_dwCLUB_MARK_VER;

	D3DXVECTOR3	m_vPOS;
	DWORD		m_dwCOUNT;

	CROWREN () 
		: m_emCROW(CROW_PC)
		, m_dwID(0)
		, m_wSCHOOL(NODATA)
		
		, m_dwCOLOR(NS_UITEXTCOLOR::DEFAULT)
		, m_emPARTY(NODATA)
		, m_dwTYPE(TYPE_NORMAL)
		, m_bPMARKET(false)
		
		, m_vPOS(0,0,0)
		, m_dwCLUB(CLUB_NULL)
		, m_dwCLUB_MARK_VER(0)
		, m_dwCOUNT(0)
	{
		memset(m_szNAME, 0, sizeof(char) * (CHAR_SZNAME));
		memset(m_szPMARKET, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
		memset(m_szNICK, 0, sizeof(char) * (CHAR_SZNAME));
	}

	bool DIFFERENT ( const CROWREN &sINFO );

	bool IsCLUB () const { return m_dwCLUB!=CLUB_NULL; }

	void INIT ( GLCharacter *pMYCHAR );
	void INIT ( GLCharClient *pCHAR );
	void INIT ( GLCrowClient *pCROW );
	void INIT ( CItemClientDrop *pITEM );
	void INIT ( CMoneyClientDrop *pMONEY );
	void INIT ( GLMaterialClient *pMaterial );

	// PET
	void INIT ( PGLPETCLIENT pMyPet );
	void INIT ( PGLANYPET pAnyPet );

	// SUMMON
	void INIT ( PGLSUMMONCLIENT pMySummon );
	void INIT ( PGLANYSUMMON pAnySummon );

	void SETTYPEFLAG ( GLCharacter *pMYCHAR );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, GLCharClient *pCHAR );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, GLCrowClient *pCROW );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, GLMaterialClient *pCROW );

	void SETTYPEFLAG ( GLCharacter *pMYCHAR, CItemClientDrop *pITEM );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, CMoneyClientDrop *pMONEY );

	// PET
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLPETCLIENT pMyPet );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLANYPET pAnyPet );

	// SUMMON
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLSUMMONCLIENT pMySummon );
	void SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLANYSUMMON pAnySummon );

	void UPDATE ( DWORD _COUNT, D3DXVECTOR3 _vPOS );
};

