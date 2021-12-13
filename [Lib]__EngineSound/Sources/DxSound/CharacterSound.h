////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	SChaSound
//	����	:	� �繰�� ���������� �����̴� ���� ǥ��
//	����	:	ĳ���ͳ� ����, ȿ����� ���� �����Ȱ��� �ƴ϶�, �������� �κп�
//				���� ���带 ó���ϱ� ���� ����ü�̴�.
//				
//	�ۼ���	:	���⿱
//
//	�α�
//	$2.	2003-01-23T16:55	[�ǵ���Ÿ�� �۵� ����ü �и� ����]
//	$1.	2002-11-21T17:05	[�����۾�]
////////////////////////////////////////////////////////////////////////////////
#pragma	once

#include "SuperSound.h"
#include <vector>

//	<--	������ �Ѱ��� ���ü� ���� ��.. ������	-->	//
#define	MAXPLAYFRAME	( 10 + 1 )
#define STRING_NUM_32	32

class	CSound;
class	CSerialFile;

////////////////////////////////////////////////////////////////////////
//	���Ź���
#define	MAXPLAYFRAME_102	( 6 + 1 )
struct	SChaSoundData_102
{
	int		m_PlayFrameCount;					//	�÷����ؾ��ϴ� ���� ����

	char	m_szFileName[MAXPLAYFRAME_102 - 1][STRING_NUM_32];
	WORD	m_BufferCount[MAXPLAYFRAME_102 - 1];	//	���� ī��Ʈ
	BOOL	m_bLoop[MAXPLAYFRAME_102 - 1];			//	����

	WORD	m_PlayType[MAXPLAYFRAME_102 - 1];		//	���� ��� ����
	WORD	m_PlayFrame[MAXPLAYFRAME_102];			//	�÷��̰� ���۵� ������	
};
typedef	SChaSoundData_102	*PSCHASOUNDDATA_102;

#define	MAXPLAYFRAME_103	( 10 + 1 )
struct	SChaSoundData_103
{
	int		m_PlayFrameCount;					//	�÷����ؾ��ϴ� ���� ����

	char	m_szFileName[MAXPLAYFRAME_103 - 1][32];
	WORD	m_BufferCount[MAXPLAYFRAME_103 - 1];	//	���� ī��Ʈ
	BOOL	m_bLoop[MAXPLAYFRAME_103 - 1];			//	����

	WORD	m_PlayType[MAXPLAYFRAME_103 - 1];		//	���� ��� ����
	WORD	m_PlayFrame[MAXPLAYFRAME_103];			//	�÷��̰� ���۵� ������
};
typedef	SChaSoundData_103	*PSCHASOUNDDATA_103;
//
////////////////////////////////////////////////////////////////////////

//	����Ÿ��
struct	SChaSoundData
{
public:
	int		m_PlayFrameCount;					//	�÷����ؾ��ϴ� ���� ����

	char	m_szFileName[MAXPLAYFRAME - 1][32];
	BOOL	m_bLoop[MAXPLAYFRAME - 1];			//	����
	WORD	m_PlayFrame[MAXPLAYFRAME];			//	�÷��̰� ���۵� ������	

public:
	SChaSoundData();
	~SChaSoundData();

	SChaSoundData&	operator= ( const SChaSoundData_102& rValue );
	SChaSoundData&	operator= ( const SChaSoundData_103& rValue );
};

//	���ۺ�
struct	SChaSound
{
public:
static	const float fMINRANGE;
static	const float fMAXRANGE;

public:
	int			m_PlayFrameCount;				//	�÷����ؾ��ϴ� ���� ����
	WORD		m_PlayType[MAXPLAYFRAME - 1];	//	���� ��� ����
	WORD		m_PlayFrame[MAXPLAYFRAME];		//	�÷��̰� ���۵� ������
	
	SSound		m_Sound[MAXPLAYFRAME - 1];		//	���� ���� ��ü ( 2�� �̻� ó�� )

	int			m_PlayingFrame;					//	�����ؾ��� ���� ������ ������

public:
	SChaSound();
	~SChaSound();

public:
	SChaSound&	operator= ( const SChaSoundData& rValue );

public:
	void	PlayChaSound ( float fCurTime, D3DXVECTOR3 vPos );
	BOOL	PlayReset ( DWORD dwSTime );

protected:
	void	PlayChaSoundOnce ( SSound& rSoundObj, D3DXVECTOR3& vPos );	//	���� ���
	void	PlayChaSoundPosition ( D3DXVECTOR3& vPos );	//	���� �÷����� ������ �̵�

protected:
	void	CtrlDS3DBuffer ( SSound& rSoundObj, D3DXVECTOR3& vPos );
};