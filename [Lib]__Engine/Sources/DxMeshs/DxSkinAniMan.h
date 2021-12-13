//	[Skin Animation Manager], JDH
//	
//	(2002.12.02),JDH, DxSkinAniControl::ResetBone () 메소드 추가. - 에니메이션 키중에 빠진
//			키가 있을 경우 Render 마다 키가 리셋되지 않고 앳날 대이터가 남아있어서
//			문제 발생. 이를 해결하기 위한 것.

#pragma once

#include <GASSERT.h>

#include "DxSkinAniControl.h"

#include <map>
#include <list>
#include <string>

#define	E_DIFFERENT_BONEFILE	MAKE_HRESULT(1,27,100)

class DxSkinAniMan
{
private:
	typedef std::map<std::string,SAnimContainer*>			ANIMCONTMAP;
	typedef std::map<std::string,SAnimContainer*>::iterator	ANIMCONTMAP_ITER;

	typedef std::list<SAnimContainer*>						ANIMCONTLIST;
	typedef std::list<SAnimContainer*>::iterator			ANIMCONTLIST_ITER;

protected:
	char				m_szPath[MAX_PATH];
	ANIMCONTMAP			m_mapAnimCont;
	ANIMCONTMAP			m_mapPreLoadAnimCont;

protected:
	static CRITICAL_SECTION	m_criticalCONT;

public:
	SAnimContainer* FindAnim ( const char *szFile );
	char*			GetPath () { return m_szPath; }

public:
	SAnimContainer* LoadAnimContainer ( const char *szFileName, const char *szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	BOOL SaveAnimConInfo ( char *szFileName );

public:
	bool SetBinAnim ( const char *szFileName, SAnimContainer::SANILIST &listANI, SAnimContainer::SANILIST &listANIUPBODY );

public:
	BOOL PreLoad ( char* szAniContList, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void OneTimeSceneInit ( char* szPath );
	void CleanUp ();
	HRESULT DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxSkinAniMan(void);

public:
	~DxSkinAniMan(void);

public:
	static DxSkinAniMan& GetInstance();

protected:
	struct SBINFILELOAD
	{
		BOOL	m_bProgressInit;
		BOOL	m_bFORCE_TERMINATE;
		BOOL	m_bEND;

		SBINFILELOAD () :
			m_bProgressInit(FALSE),
			m_bFORCE_TERMINATE(FALSE),
			m_bEND(TRUE)
		{
		}
	};

	struct SBIN
	{
		std::string		m_strFILE;
		std::string		m_strFILEPATH;
		DxSkeleton*		m_pSkeleton;

		SBIN () :
			m_strFILE(""),
			m_strFILEPATH(""),
			m_pSkeleton(NULL)
		{
		}

		SBIN& operator= ( const SBIN &value )
		{
			m_strFILE = value.m_strFILE;
			m_strFILEPATH = value.m_strFILEPATH;
			m_pSkeleton = value.m_pSkeleton;

			return *this;
		}
	};

	typedef std::list<SBIN>			LISTBINLOAD;
	typedef LISTBINLOAD::iterator	LISTBINLOAD_ITER;

	static SBINFILELOAD		m_sBINFILELOAD_THREAD;
	static LISTBINLOAD		m_listBIN_LOAD;
	static CRITICAL_SECTION	m_criticalLOAD;

protected:
	DWORD	m_dwBinFileThreadID;			//	스레드 ID
	HANDLE	m_hBinFileThread;				//	스레드 핸들

public:
	BOOL	StartBinFileLoadThread ();
	void	EndBinFileLoadThread ();

	void	ClearBinLoadingData ();

public:
	void	AddBinFIleLoad ( std::string strFILE, DxSkeleton *pSkeleton );

public:
	static unsigned int WINAPI BinFileLoadThread( LPVOID pData );
};

namespace COMMENT
{
	extern std::string ANI_MAINTYPE[AN_TYPE_SIZE];
	extern std::string ANI_SUBTYPE[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_00[AN_SUB_00_SIZE];
	extern std::string ANI_SUBTYPE_01[AN_SUB_00_SIZE];

	extern std::string STRIKE_EFF[EMSF_SIZE];
};


