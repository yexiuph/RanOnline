#ifndef GLVEHICLE_H_
#define GLCEHICLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLItem.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"

class GLChar;

struct SVEHICLEITEMINFO
{
	VEHICLE_TYPE	m_emTYPE;
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCE_TYPE_SIZE];

	SVEHICLEITEMINFO () :
		m_emTYPE(VEHICLE_TYPE_NONE),
		m_nFull(0)
	{
	}

	SVEHICLEITEMINFO ( const SVEHICLEITEMINFO& value ) { operator=( value ); }
	SVEHICLEITEMINFO& operator= ( const SVEHICLEITEMINFO& rvalue );

};

typedef std::map<DWORD,SVEHICLEITEMINFO>			VEHICLEITEMINFO_MAP;
typedef VEHICLEITEMINFO_MAP::iterator				VEHICLEITEMINFO_MAP_ITER;

struct CLIENT_VEHICLE
{
	VEHICLE_TYPE	m_emTYPE;
	DWORD			m_dwGUID;						// Ż���� GUID
	SNATIVEID		m_sVehicleID;					// ��ȯ�� ��û�� Ż�� �������� Mid/Sid
	SITEMCLIENT		m_PutOnItems[ACCE_TYPE_SIZE];
	
	CLIENT_VEHICLE() 
		: m_emTYPE ( VEHICLE_TYPE_NONE )
		, m_dwGUID ( 0 )
		, m_sVehicleID ( NATIVEID_NULL() )
	{
	}
};


struct GLVEHICLE
{
	VEHICLE_TYPE	m_emTYPE;
	DWORD			m_dwGUID;						// Ż���� GUID
	DWORD			m_dwOwner;						// ������ charID
	SNATIVEID		m_sVehicleID;					// ��ȯ�� ��û�� Ż�� �������� Mid/Sid
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCE_TYPE_SIZE];
	float			m_fSpeedRate;					// Ż���� �ӵ� ����( ���� �κ��� ��ģ�� )
	float			m_fSpeedVol;					// Ż���� �ӵ� ���밪( ���� �κ��� ��ģ�� )
	SNATIVEID		m_sSkinID;						// Ż���� ��Ų ������ Item ID
	bool			m_bActiveValue;					// ���� Ż���� �������� Ȯ��

	// constructor & operator
	GLVEHICLE () :
		m_emTYPE(VEHICLE_TYPE_NONE),
		m_dwGUID(UINT_MAX),
		m_dwOwner(0),
		m_nFull(0),
		m_sVehicleID(NATIVEID_NULL()),
		m_fSpeedRate( 0.0f ),
		m_fSpeedVol( 0.0f ),
		m_sSkinID ( NATIVEID_NULL() ),
		m_bActiveValue ( false )
	{
	}

	GLVEHICLE ( const GLVEHICLE& value ) { operator=( value ); }
	GLVEHICLE& operator= ( const GLVEHICLE& rvalue );

	BOOL GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const;

	void ASSIGN ( const GLVEHICLE& sVehicleData );
	void RESET ();
	bool IncreaseFull ( const WORD volume, const BOOL bRatio = TRUE );

	BOOL IsNotEnoughFull () { return m_nFull < 10; }

	BOOL IsActiveValue() { return m_bActiveValue; }
	VOID SetActiveValue( bool bActiveValue ) { m_bActiveValue = bActiveValue; }
	SNATIVEID GetSkinID()	{ return m_sSkinID; }


	SITEMCUSTOM GetSlotitembySuittype ( EMSUIT emSuit );
	void		SetSlotItem ( EMSUIT emType, SITEMCUSTOM sCustomItem);
	void		ReSetSlotItem ( EMSUIT emType);
	BOOL		CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT );
//    EMPIECECHAR GetPieceFromSlot ( VEHICLE_ACCESSORYTYPE emType );

	// access
	BOOL		LoadFile ( const char* szFileName, VEHICLE_TYPE emType );

	void		UpdateTimeLmtItem ( GLChar* pChar );
	void		ITEM_UPDATE();

	float		GetSpeedRate()	{ return m_fSpeedRate; }
	float		GetSpeedVol()	{ return m_fSpeedVol; }
};

typedef GLVEHICLE* PGLVEHICLE;

namespace COMMENT
{
	extern std::string VEHICLE_TYPE[VEHICLE_TYPE_SIZE];
};

#endif // GLVEHICLE_H_