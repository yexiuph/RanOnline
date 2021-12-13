////////////////////////////////////////////////////////////////////////////////
//	이름	:	SSound
//	목적	:	
//	설명	:	
//				
//	작성자	:	성기엽
//
//	로그
//	$1.	2002-11-18T15:10	[StaticSoundMan으로부터 떼어냄]
////////////////////////////////////////////////////////////////////////////////
#pragma	once

//#define	_DEBUG_SOUND

#include <string>

#ifdef	_DEBUG_SOUND
	#include <map>
#endif	//	_DEBUG_SOUND

//	<--	클래스 선언	-->	//
class	basestream;
class	CSound;
class	CSerialFile;

//	<--	구조체 정의	-->	//
struct	SSound
{
public:
static	const	WORD	m_SoundVER;

public:
	std::string	m_strFileName;
	BOOL		m_bLoop;	//	루프

private:
	DWORD	m_BufferID;		//	잡고 있는 버퍼 ID
	CSound*	m_pSound;
	bool	m_bStereo;

public:
	SSound();
	SSound(const SSound& rValue );
	virtual	~SSound();

	SSound& operator= ( const SSound& rValue );

public:	
	bool IsLoad ();
	bool Load ( std::string strFileName, bool bLoop, bool bStereo = false );
	bool UnLoad ();

	virtual	bool Play ();
	bool Stop ();
	bool IsPlaying ();

	bool IsLoop ();

	bool SetVolume ( const long lVolume );

	bool SetPan ( const long lPan );
	bool SetPosition ( const D3DXVECTOR3& vPos, const DWORD dwApply );
	bool GetPosition ( D3DXVECTOR3* pvPos );
	bool SetMinDistance ( const float fDist, const DWORD dwApply );
	bool SetMaxDistance ( const float fDist, const DWORD dwApply );

public:
	BOOL	SaveSet ( CSerialFile &SFile );
	BOOL	LoadSet ( basestream &SFile, WORD SoundVer );

private:
	BOOL	LoadSet100 ( basestream &SFile, WORD SoundVer );

#ifdef	_DEBUG_SOUND
public:
typedef	std::map<std::string,int>	SOUNDMAP;
typedef SOUNDMAP::iterator			SOUNDMAP_ITER;
typedef SOUNDMAP::const_iterator	SOUNDMAP_CITER;

static	SOUNDMAP mapSound;
static	void	PRINT_TO_VIEW ();
#endif	//	_DEBUG_SOUND
};
typedef	SSound	*PSSOUND;