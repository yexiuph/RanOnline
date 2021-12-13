////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	CStaticSoundMan
//	����	:	���� ���� �Ŵ���
//	����	:	���� Ư�� �κп� �ٴ� ����� ���� �������� �ʴ� ���带
//				�����Ѵ�.
//	�ۼ���	:	���⿱
//
//	�α�
//	$8.	2002-11-18T15:11	[SSound����ü�� 'SuperSound.h'�� ���� ���, �̰���
//							MovableSound�� ���� �۾���]
//	$7.	2002-11-13T09:31	[SaveSet/LoadSet �۾�]
//	$6.	2002-11-12T09:57	[SVecSound, �ּ� �ִ� �︲�ݰ����� ����
//							--> Rolloff, Doppler���� �������������
//								�ִ븸���δ� �������̾���]
//	$5.	2002-11-07T09:31	[����Ʈ ��, Final �����۾� -1,
//							Ŭ������ ����Ȯ�� �۾��� �� ������]	
//	$3.	2002-11-01T16:22	[���� ����]
//	$2.	2002-10-28T09:51	[�󼼱��� ����]
//	$1.	2002-10-18T11:23	[���� Ŭ���� ����]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__C_STATICSOUNDMAN__
#define	__C_STATICSOUNDMAN__

#include "SuperSound.h"
#include "Collision.h"



//	<--	Ŭ���� ����	-->	//
class	CSerialFile;




//	<--	����ü ����	-->	//
struct	SVecSound : public SSound, public OBJAABB
{	
	static	WORD	m_VecSoundVER;
	WORD		m_FramePos;
    
	char		m_szSoundName[256];
	//D3DXVECTOR3	m_Position;		//	������ ��ġ
	D3DXMATRIX	m_matWorld;
	float		m_MinRange;		//	������ �ּ� �︲�ݰ�
	float		m_MaxRange;		//	������ �ִ� �︲�ݰ�
	float		m_CurrentTime;	//	������ ���� ���� �ð�
	float		m_LoopTime;		//	���� �ֱ�
	
	float		m_FadeOutRange;	//	���̵� �ƿ� �ݰ�
    
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

//	<--	Ŭ���� ����	-->	//
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