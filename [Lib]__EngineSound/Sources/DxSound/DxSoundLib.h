#pragma	once

#include <string>
#include <map>

#include "SuperSound.h"

enum	ET_SOUNDTYPE
{
	SFX_SOUND,
	MAP_SOUND
};

struct	SSoundEx : public SSound
{
	ET_SOUNDTYPE	m_emVolType;
};

class	DxSoundLib
{
private:
	DxSoundLib ();
	virtual	~DxSoundLib ();

public:
	typedef std::map<std::string, SSoundEx*>	SOUNDLIB_MAP;
	typedef	SOUNDLIB_MAP::const_iterator		SOUNDLIB_MAP_CITER;
	typedef	SOUNDLIB_MAP::iterator				SOUNDLIB_MAP_ITER;
	typedef	SOUNDLIB_MAP::size_type				SOUNDLIB_MAP_SIZE;

public:
	bool	CreateSound ( std::string strKeyword, std::string strFileName, int nVolType, bool bLoop = false );
	bool	ReleaseSound( std::string strKeyword );

public:
	bool	PlaySound ( std::string strKeyword );
	bool	StopSound ( std::string strKeyword );
	bool	IsPlaying ( std::string strKeyword );

public:
	bool	Update ();

public:
	bool	ReleaseAllSound ();

private:
	SOUNDLIB_MAP	m_mapSoundLib;

public:
static	DxSoundLib*	GetInstance ();
static	void	ReleaseInstance ();	
};