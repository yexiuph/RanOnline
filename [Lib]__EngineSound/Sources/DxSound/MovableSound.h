////////////////////////////////////////////////////////////////////////////////
//	이름	:	SMovSound
//	목적	:	어떤 사물에 종속적으로 움직이는 사운드 표현
//	설명	:	캐릭터나 물건, 효과등과 같이 고정된것이 아니라, 가변적인 부분에
//				대한 사운드를 처리하기 위한 구조체이다.
//				
//	작성자	:	성기엽
//
//	로그
//	$1.	2002-11-18T15:10	[최초작업]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__S_MOVSOUND__
#define	__S_MOVSOUND__

#include "SuperSound.h"



//	<--	클래스 선언	-->	//
class	basestream;
class	CSound;
class	CSerialFile;



//	<--	SMovSound의 기본 개념	-->	//
//	<**	SVecSound와 SMovSound를 따로 분리한 이유가 가장 중요하다.
//		추후의 업데이트에서 세부내용이 어떻게 변할지 알수 없으므로,
//		여유를 위해, 따로 떼어내어 제작했다. 사실, 지금은 SVecSound와
//		SMovSound는 중복되는 부분이 없지 않다.
//		볼륨조절(m_Volume)부분을 넣고 싶지만, 시간관계상 아직은 구현할수
//		없을것 같다.
//	**>

//	<--	구조체 정의	-->	//
struct	SMovSound : public SSound
{	
	static	WORD	m_MovSoundVER;    

	float	m_MinRange;		//	사운드의 최소 울림반경
	float	m_MaxRange;		//	사운드의 최대 울림반경

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