////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	SMovSound
//	����	:	� �繰�� ���������� �����̴� ���� ǥ��
//	����	:	ĳ���ͳ� ����, ȿ����� ���� �����Ȱ��� �ƴ϶�, �������� �κп�
//				���� ���带 ó���ϱ� ���� ����ü�̴�.
//				
//	�ۼ���	:	���⿱
//
//	�α�
//	$1.	2002-11-18T15:10	[�����۾�]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__S_MOVSOUND__
#define	__S_MOVSOUND__

#include "SuperSound.h"



//	<--	Ŭ���� ����	-->	//
class	basestream;
class	CSound;
class	CSerialFile;



//	<--	SMovSound�� �⺻ ����	-->	//
//	<**	SVecSound�� SMovSound�� ���� �и��� ������ ���� �߿��ϴ�.
//		������ ������Ʈ���� ���γ����� ��� ������ �˼� �����Ƿ�,
//		������ ����, ���� ����� �����ߴ�. ���, ������ SVecSound��
//		SMovSound�� �ߺ��Ǵ� �κ��� ���� �ʴ�.
//		��������(m_Volume)�κ��� �ְ� ������, �ð������ ������ �����Ҽ�
//		������ ����.
//	**>

//	<--	����ü ����	-->	//
struct	SMovSound : public SSound
{	
	static	WORD	m_MovSoundVER;    

	float	m_MinRange;		//	������ �ּ� �︲�ݰ�
	float	m_MaxRange;		//	������ �ִ� �︲�ݰ�

	float	m_fGAgeBACK;
	bool	m_bSTARTED;
	bool	m_bFORCE_STOP;

public:
	SMovSound();
	SMovSound(const SMovSound& rValue);
	virtual	~SMovSound();

	SMovSound&	operator= ( const SMovSound& rValue );

	virtual	bool Play ();

	void Update ( const D3DXVECTOR3& vGNowPos, const float& fGBeginTime, const float& fGAge, const float& fGLifeTime );

public:
	BOOL	SaveSet ( CSerialFile &SFile );
	BOOL	LoadSet ( basestream &SFile, WORD MovSoundVer, WORD SoundVer );
};
typedef	SMovSound	*PSMOVSOUND;

#endif	//	__S_MOVSOUND__