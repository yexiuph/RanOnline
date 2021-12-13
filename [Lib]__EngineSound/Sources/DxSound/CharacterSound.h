////////////////////////////////////////////////////////////////////////////////
//	이름	:	SChaSound
//	목적	:	어떤 사물에 종속적으로 움직이는 사운드 표현
//	설명	:	캐릭터나 물건, 효과등과 같이 고정된것이 아니라, 가변적인 부분에
//				대한 사운드를 처리하기 위한 구조체이다.
//				
//	작성자	:	성기엽
//
//	로그
//	$2.	2003-01-23T16:55	[실데이타와 작동 구조체 분리 제작]
//	$1.	2002-11-21T17:05	[최초작업]
////////////////////////////////////////////////////////////////////////////////
#pragma	once

#include "SuperSound.h"
#include <vector>

//	<--	마지막 한개는 나올수 없는 값.. 더미임	-->	//
#define	MAXPLAYFRAME	( 10 + 1 )
#define STRING_NUM_32	32

class	CSound;
class	CSerialFile;

////////////////////////////////////////////////////////////////////////
//	과거버전
#define	MAXPLAYFRAME_102	( 6 + 1 )
struct	SChaSoundData_102
{
	int		m_PlayFrameCount;					//	플레이해야하는 사운드 개수

	char	m_szFileName[MAXPLAYFRAME_102 - 1][STRING_NUM_32];
	WORD	m_BufferCount[MAXPLAYFRAME_102 - 1];	//	버퍼 카운트
	BOOL	m_bLoop[MAXPLAYFRAME_102 - 1];			//	루프

	WORD	m_PlayType[MAXPLAYFRAME_102 - 1];		//	사운드 재생 조건
	WORD	m_PlayFrame[MAXPLAYFRAME_102];			//	플레이가 시작될 프레임	
};
typedef	SChaSoundData_102	*PSCHASOUNDDATA_102;

#define	MAXPLAYFRAME_103	( 10 + 1 )
struct	SChaSoundData_103
{
	int		m_PlayFrameCount;					//	플레이해야하는 사운드 개수

	char	m_szFileName[MAXPLAYFRAME_103 - 1][32];
	WORD	m_BufferCount[MAXPLAYFRAME_103 - 1];	//	버퍼 카운트
	BOOL	m_bLoop[MAXPLAYFRAME_103 - 1];			//	루프

	WORD	m_PlayType[MAXPLAYFRAME_103 - 1];		//	사운드 재생 조건
	WORD	m_PlayFrame[MAXPLAYFRAME_103];			//	플레이가 시작될 프레임
};
typedef	SChaSoundData_103	*PSCHASOUNDDATA_103;
//
////////////////////////////////////////////////////////////////////////

//	데이타부
struct	SChaSoundData
{
public:
	int		m_PlayFrameCount;					//	플레이해야하는 사운드 개수

	char	m_szFileName[MAXPLAYFRAME - 1][32];
	BOOL	m_bLoop[MAXPLAYFRAME - 1];			//	루프
	WORD	m_PlayFrame[MAXPLAYFRAME];			//	플레이가 시작될 프레임	

public:
	SChaSoundData();
	~SChaSoundData();

	SChaSoundData&	operator= ( const SChaSoundData_102& rValue );
	SChaSoundData&	operator= ( const SChaSoundData_103& rValue );
};

//	동작부
struct	SChaSound
{
public:
static	const float fMINRANGE;
static	const float fMAXRANGE;

public:
	int			m_PlayFrameCount;				//	플레이해야하는 사운드 개수
	WORD		m_PlayType[MAXPLAYFRAME - 1];	//	사운드 재생 조건
	WORD		m_PlayFrame[MAXPLAYFRAME];		//	플레이가 시작될 프레임
	
	SSound		m_Sound[MAXPLAYFRAME - 1];		//	사운드 버퍼 객체 ( 2개 이상도 처리 )

	int			m_PlayingFrame;					//	진행해야할 사운드 프레임 포지션

public:
	SChaSound();
	~SChaSound();

public:
	SChaSound&	operator= ( const SChaSoundData& rValue );

public:
	void	PlayChaSound ( float fCurTime, D3DXVECTOR3 vPos );
	BOOL	PlayReset ( DWORD dwSTime );

protected:
	void	PlayChaSoundOnce ( SSound& rSoundObj, D3DXVECTOR3& vPos );	//	사운드 재생
	void	PlayChaSoundPosition ( D3DXVECTOR3& vPos );	//	사운드 플레이중 포지션 이동

protected:
	void	CtrlDS3DBuffer ( SSound& rSoundObj, D3DXVECTOR3& vPos );
};