#pragma once


struct STAXI_STATION
{
	DWORD		dwMAPID;
	WORD		wPosX;
	WORD		wPosY;
	DWORD		dwNPCID;
	DWORD		dwLINKID;
	DWORD		dwMapCharge;
	std::string	strMAP;
	std::string	strSTATION;

	STAXI_STATION ()
		: dwMAPID(0)
		, wPosX(0)
		, wPosY(0)
		, dwNPCID(0)
		, dwLINKID(0)
	{
	}

	STAXI_STATION& operator= ( const STAXI_STATION& value )
	{
		dwLINKID = value.dwLINKID;
        dwMAPID = value.dwMAPID;
		wPosX = value.wPosX;
		wPosY = value.wPosY;
		dwNPCID = value.dwNPCID;
		strMAP = value.strMAP;
		dwMapCharge = value.dwMapCharge;
		strSTATION = value.strSTATION;

		return *this;
	}
};

struct STAXI_MAP
{
	DWORD						dwMAPID;
	std::string					strMAP;
	std::vector<STAXI_STATION>	vecTaxiStation;

	STAXI_MAP () :
		dwMAPID(0)
	{
	}

	STAXI_MAP& operator= ( const STAXI_MAP& value )
	{
		dwMAPID = value.dwMAPID;
		strMAP = value.strMAP;
		vecTaxiStation = value.vecTaxiStation;
		return *this;
	}

	DWORD GetStationNum ()
	{
		return (DWORD) vecTaxiStation.size();
	}

	STAXI_STATION* GetStation ( DWORD dwIndex )
	{
		
		if ( vecTaxiStation.size() <= dwIndex )
			return NULL;

		return &vecTaxiStation[dwIndex];
	}
};

class GLTaxiStation 
{
public:
	virtual ~GLTaxiStation(void);

protected:
	std::string				m_strPATH;
	DWORD					m_dwBasicCharge;

public:
	void SetPath ( std::string strPATH )	{ m_strPATH = strPATH; }
	std::string GetPath ()					{ return m_strPATH.c_str(); }
	DWORD GetBasicCharge() { return m_dwBasicCharge; }


protected:
	
	std::vector<STAXI_MAP>	m_vecTaxiMap;

protected:
	GLTaxiStation(void);
	void insert( STAXI_STATION &sSTATION );

public:
	virtual bool LOAD( std::string strFILE );
	
public:
	DWORD GetTaxiMapNum();
	STAXI_MAP* GetTaxiMap ( int nIndex );

	virtual STAXI_STATION* GetStation ( int nMapIndex, int nStationIndex );

public:
	static GLTaxiStation& GetInstance ();
};
