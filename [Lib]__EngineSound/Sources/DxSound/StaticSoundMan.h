////////////////////////////////////////////////////////////////////////////////
//	이름	:	CStaticSoundMan
//	목적	:	고정 사운드 매니저
//	설명	:	맵의 특정 부분에 붙는 사운드와 같이 움직이지 않는 사운드를
//				관리한다.
//	작성자	:	성기엽
//
//	로그
//	$8.	2002-11-18T15:11	[SSound구조체를 'SuperSound.h'로 따로 떼어냄, 이것은
//							MovableSound를 위한 작업임]
//	$7.	2002-11-13T09:31	[SaveSet/LoadSet 작업]
//	$6.	2002-11-12T09:57	[SVecSound, 최소 최대 울림반경으로 변경
//							--> Rolloff, Doppler등의 계수설정때문에
//								최대만으로는 역부족이었음]
//	$5.	2002-11-07T09:31	[프린트 후, Final 수정작업 -1,
//							클래스간 관계확인 작업이 주 목적임]	
//	$3.	2002-11-01T16:22	[구조 변경]
//	$2.	2002-10-28T09:51	[상세구현 시작]
//	$1.	2002-10-18T11:23	[최초 클래스 구현]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__C_STATICSOUNDMAN__
#define	__C_STATICSOUNDMAN__

#include "SuperSound.h"
#include "Collision.h"



//	<--	클래스 선언	-->	//
class	CSerialFile;




//	<--	구조체 정의	-->	//
struct	SVecSound : public SSound, public OBJAABB
{	
	static	WORD	m_VecSoundVER;
	WORD		m_FramePos;
    
	char		m_szSoundName[256];
	//D3DXVECTOR3	m_Position;		//	사운드의 위치
	D3DXMATRIX	m_matWorld;
	float		m_MinRange;		//	사운드의 최소 울림반경
	float		m_MaxRange;		//	사운드의 최대 울림반경
	float		m_CurrentTime;	//	사운드의 현재 진행 시간
	float		m_LoopTime;		//	사운드 주기
	
	float		m_FadeOutRange;	//	페이드 아웃 반경
    
	SVecSound	*m_pNext;

public:
	SVecSound();
	SVecSound(const SVecSound& rValue);
	virtual	~SVecSound();

	SVecSound&	operator= ( const SVecSound& rValue );	

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual OBJAABB* GetNext ();

	virtual	bool Play ();

public:
	BOOL	SaveSet ( CSerialFile &SFile );
	BOOL	LoadSet ( CSerialFile &SFile, WORD VecSoundVer, WORD SoundVer );
};
typedef	SVecSound	*PSVECSOUND;

//	<--	클래스 정의	-->	//
class	CStaticSoundMan
{
private:
	typedef	CStaticSoundMan	*LPCSTATICSOUNDMAN;

private:
	static	WORD	m_StaticSoundManVER;

protected:
	SVecSound*	m_pVecSoundHead;	
	OBJAABBNode*m_pVecSoundTree;
	WORD		m_VecSoundCount;

	SVecSound*	m_pVecSoundNew[256];
	WORD		m_VecSoundNewCount;
	SVecSound*	m_pVecSoundOld[256];
	WORD		m_VecSoundOldCount;

	WORD		m_FramePos;	

protected:
	BOOL		m_bRenderRange;
	BOOL		m_bEnableSound;	

private:
	float	m_fFadeRatio;
	BOOL	m_bDSPName;

public:
	CStaticSoundMan ();
	virtual	~CStaticSoundMan();

public:
	HRESULT OneTimeSceneInit();
	HRESULT FinalCleanup();

	void	FrameMove ( D3DXVECTOR3 ListenerPos, float fElapsedTime );	
	void	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL	Import ( LPCSTATICSOUNDMAN& rpStaticSoundMan );

protected:
	void	FindSoundToPlay ( POBJAABBNODE pTreeRoot, D3DXVECTOR3 &ListenerPos, float &fElapsedTime );
	void	PlaySound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos, float& fElapsedTime );	
	void	PlayLoopSound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos );
	void	PlayTimeSound ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos, float& fElapsedTime );	
	void	CtrlDSBuffer ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos );
	void	CtrlDSFade ( SVecSound* pVecSound, D3DXVECTOR3& ListenerPos );
	
	void	FindSoundToStop ();

public:	
	void	EnableRenderRange ( BOOL bRenderRange ) { m_bRenderRange = bRenderRange; }
	void	EnableSound ( BOOL bEnableSound );
	void	Clone_MouseShift( LPD3DXMATRIX& pMatrix );
	void	SetObjRotate90();

public:	
    BOOL		AddVecSound ( SVecSound *pVecSound );
	BOOL		DelVecSound ( char *szSoundName );
	SVecSound*	GetVecSound ( const char *szSoundName );

	POBJAABBNODE &GetOBJAABBNode()	{ return m_pVecSoundTree; }
    SVecSound	*GetVecSoundHead()	{ return m_pVecSoundHead; }
	WORD		GetCount()			{ return m_VecSoundCount; }

	void SetDSPName( BOOL bUse )	{ m_bDSPName = bUse; }

public:
	BOOL	SaveSet ( CSerialFile &SFile );
	BOOL	LoadSet ( CSerialFile &SFile );
};

#endif	//	__C_STATICSOUNDMAN__