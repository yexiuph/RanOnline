#ifndef GLINVENTORY_H_
#define GLINVENTORY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLItem.h"
#include "../[Lib]__Engine/Sources/Common/ByteStream.h"

enum
{
	EM_INVENSIZE_X			= 6,
	EM_INVENSIZE_Y			= 10,
	EM_INVEN_DEF_SIZE_Y		= 4,
	EM_INVEN_PREMIUM_SIZE	= 1,
};

class GLInventory
{
public:
	typedef std::pair<WORD,WORD>				CELL_KEY;
	typedef std::map<CELL_KEY,SINVENITEM*>		CELL_MAP;
	typedef CELL_MAP::iterator					CELL_MAP_ITER;
	typedef CELL_MAP::const_iterator			CELL_MAP_CITER;

protected:
	WORD					m_wCellSX;
	WORD					m_wCellSY;
	WORD					m_wADDLINE;
	bool					m_bLimit;

	BYTE**					m_ppbBarrier;

	WORD					m_wMaxItems;

	CELL_MAP				m_ItemMap;

protected:
	void MakeBarrier ();
	void DeleteBarrier ();

protected:
	void SetMark ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY );
	void ReSetMark ( WORD wSX, WORD wSY, WORD wPosX, WORD wPosY );

public:
	WORD GetSizeX()			{ return m_wCellSX; }
	WORD GetSizeY()			{ return m_wCellSY; }

public:
	BOOL SETITEM_BYBUFFER ( CByteStream &ByteStream );
	BOOL GETITEM_BYBUFFER ( CByteStream &ByteStream ) const;
	BOOL GETITEM_BYBUFFER_FORSTORAGE ( CByteStream &ByteStream ) const;

public:
	BOOL IsInsertable ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY, bool bLOAD=false );

public:
	SINVENITEM* FindPosItem ( const WORD wPosX, const WORD wPosY );
	SINVENITEM* FindItemByGenNumber ( LONGLONG llGenNum, SNATIVEID sID, BYTE cGenType );
	BOOL FindInsrtable ( const WORD wSX, const WORD wSY, WORD &wPosX, WORD &wPosY, bool bAllLine=false );
	BOOL HaveEventItem ();

	BOOL ValidPileInsrt ( const WORD wINSRTNUM, const SNATIVEID &sNID, const WORD wPILENUM, const WORD wSX, const WORD wSY, bool bAllLine=false );
	BOOL ValidCheckInsrt ( const WORD wNum, const WORD wSX, const WORD wSY, bool bAllLine );

	DWORD CountItem ( const SNATIVEID sNID );
	DWORD CountTurnItem ( const SNATIVEID sNID );
	DWORD CountPileItem ( const SNATIVEID sNID );

	SINVENITEM* FindItem ( const SNATIVEID sNID );
	SINVENITEM* FindItem ( const EMITEM_TYPE emTYPE );
	SINVENITEM* FindItem ( const EMITEM_TYPE emTYPE, const SNATIVEID sNID );
	SINVENITEM* FindDrugItem ( EMITEM_DRUG emDrug );

	SINVENITEM* FindItemBack ( const WORD wBackX, const WORD wBackY );

public:
	BOOL InsertItem ( const SITEMCUSTOM &ItemCustom );
	BOOL InsertItem ( const SITEMCUSTOM &ItemCustom, const WORD wPosX, const WORD wPosY, bool bLOAD=false );	//	로드(Assign) 시에는 가용 라인과 별개로 넣어짐.
	BOOL InsertItem ( const SITEMCUSTOM &ItemCustom, const WORD wPosX, const WORD wPosY, const WORD wBackX, const WORD wBackY );

	SINVENITEM* GetItem ( WORD wPosX, WORD wPosY );

public:
	BOOL DeleteItem ( WORD wPosX, WORD wPosY );
	void DeleteItemAll ();

public:
	GLInventory::CELL_MAP* GetItemList ()	{ return &m_ItemMap; }
	DWORD GetNumItems ()					{ return static_cast<DWORD>(m_ItemMap.size()); }
	DWORD GetAmountDrugItem ( EMITEM_DRUG emDrug );
	bool GetCharResetItem ( WORD &wPosX, WORD &wPosY );
	bool GetPileItem ( const SNATIVEID _sNID, WORD &wPosX, WORD &wPosY );	//	겹침 가능 갯수 많큼 겹쳐 있는 것을 반환.
	WORD GetValidCellY () const;
	WORD GETAddLine () const;
	bool IsLimit () const					{ return m_bLimit; }

public:
	void SetState ( WORD wCellSX, WORD wCellSY );
	void SetAddLine ( WORD wLine, bool bLimit );
	void SetItemGenTime ();

	void CleanUp ();

public:
	GLInventory(void);
	GLInventory(WORD _sizeX, WORD _sizeY);
	~GLInventory(void);

private:
	GLInventory(const GLInventory &Inven )			{ GASSERT(0&&"묵시적 복사 불허!"); }
	GLInventory& operator= ( GLInventory &Inven )	{ GASSERT(0&&"묵시적 복사 불허!"); return *this; }

public:
	void Assign ( const GLInventory &Inven );
};

#endif // GLINVENTORY_H_