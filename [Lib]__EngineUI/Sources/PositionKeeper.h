#pragma	once

#include "UIDataType.h"
#include <map>

class CUIControl;

class	CPositionKeeper
{
public:
	struct SPOSCONTROL
	{
		BOOL bSetFlag;
		CUIControl * pControl;
		UIRECT uiRect;

		SPOSCONTROL()
			: bSetFlag( FALSE )
			, pControl( NULL )
		{}
	};

public:		
	//typedef	std::pair<bool,UIRECT>			POSITION_PAIR; // MEMO
	typedef	std::map<UIGUID,SPOSCONTROL>	POSITION_MAP;
	typedef	POSITION_MAP::iterator			POSITION_MAP_ITER;
	typedef	POSITION_MAP::const_iterator	POSITION_MAP_CITER;	

public:
	CPositionKeeper ();
	virtual	~CPositionKeeper ();

public:
	BOOL RegisterControl( const UIGUID& cID, CUIControl * pControl);
	BOOL SetPosition( const UIGUID& cID, const UIRECT& rcPos );
	BOOL IsSetPosition( const UIGUID& cID );
	BOOL ResetPosition( const UIGUID& cID );
	BOOL GetPosition( const UIGUID& cID, UIRECT& rcPos );

public:
	void Clear()					{ m_mapPosition.clear (); }
	POSITION_MAP& GetPositionMap()	{ return m_mapPosition; }

private:
	POSITION_MAP m_mapPosition;
};