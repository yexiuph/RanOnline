#include "pch.h"
#include "./GLItemMan.h"
#include "./GLPrivateMarket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPrivateMarket::GLPrivateMarket(void) :
	m_bOPEN(false)
{
	m_invenSALE.SetState ( EM_SALE_INVEN_X, EM_SALE_INVEN_Y );
}

GLPrivateMarket::~GLPrivateMarket(void)
{
}

bool GLPrivateMarket::DoMarketOpen ()
{
	m_bOPEN = true;
	return true;
}

void GLPrivateMarket::DoMarketClose()
{
	m_bOPEN = false;
	m_strTITLE.clear();

	m_mapSALE.clear();
	m_invenSALE.DeleteItemAll();

	m_mapSearchItem.clear();

}

void GLPrivateMarket::DoMarketInfoRelease ()
{
	m_mapSALE.clear();
	m_mapSearchItem.clear();
	m_invenSALE.DeleteItemAll();

}

void GLPrivateMarket::SetTITLE ( std::string strTITLE )
{
	m_strTITLE = strTITLE;
}

bool GLPrivateMarket::SetSaleState ( SNATIVEID sSALEPOS, DWORD dwNUM, bool bSOLD )
{
	MAPITEM_ITER pos = m_mapSALE.find ( sSALEPOS.dwID );
	if ( pos==m_mapSALE.end() )						return false;

	SSALEITEM &sSALE = (*pos).second;
	sSALE.dwNUMBER = dwNUM;
	sSALE.bSOLD = bSOLD;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sSALE.sITEMCUSTOM.sNativeID );
	if ( pITEM && pITEM->ISPILE() )
	{
		SINVENITEM *pINVENITEM = m_invenSALE.GetItem ( sSALEPOS.wMainID, sSALEPOS.wSubID );
		if (pINVENITEM )
		{
			pINVENITEM->sItemCustom.wTurnNum = (WORD) dwNUM;
		}
	}

	return true;
}

bool GLPrivateMarket::IsRegItem ( SNATIVEID nidITEM )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sITEMCUSTOM.sNativeID == nidITEM )	return true;
	}

	return false;
}

DWORD GLPrivateMarket::GetItemTurnNum ( SNATIVEID nidITEM )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sITEMCUSTOM.sNativeID == nidITEM )	return (*pos).second.dwNUMBER;
	}

	return 0;

}

bool GLPrivateMarket::IsRegInvenPos ( SNATIVEID sINVENPOS )
{
	MAPITEM_ITER pos = m_mapSALE.begin();
	MAPITEM_ITER end = m_mapSALE.end();
	for ( ; pos!=end; ++pos )
	{
		if ( (*pos).second.sINVENPOS == sINVENPOS )				return true;
	}

	return false;
}

bool GLPrivateMarket::FindInsertPos ( SNATIVEID nidITEM, SNATIVEID &sSALEPOS )
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )								return false;

	BOOL bOK = m_invenSALE.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, sSALEPOS.wMainID, sSALEPOS.wSubID );
	if ( !bOK )									return false;

	return true;
}

bool GLPrivateMarket::RegItem(const SINVENITEM &sInvenItem, LONGLONG llPRICE, DWORD dwNUM, SNATIVEID sSALEPOS/* =SNATIVEID */, bool bSearchMarket /*= FALSE*/ )
{
	if ( m_mapSALE.size() >= EMMAX_SALE_NUM )		return false;

	SNATIVEID sINVENPOS(sInvenItem.wPosX,sInvenItem.wPosY);

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
	if ( !pITEM )									return false;

	SITEMCUSTOM sITEMCUSTOM = sInvenItem.sItemCustom;

	//	Note : 겹침 가능일 경우 동일한 종류의 아이템이 이미 등록되어 있는지 검사.
	if ( pITEM->ISPILE() )
	{
		bool bREG = IsRegItem ( sInvenItem.sItemCustom.sNativeID );
		if ( bREG )									return false;

		sITEMCUSTOM.wTurnNum = (WORD) dwNUM;
	}
	
	//	Note : 겹침이 아닐 경우는 판매 수량 1개로 제한.
	if ( !pITEM->ISPILE() )
	{
		dwNUM = 1;
	}

	//	Note : 들어갈 슬롯이 지정되어 있지 않을 경우.
	if ( sSALEPOS==SNATIVEID(false) )
	{
		BOOL bOK = m_invenSALE.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, sSALEPOS.wMainID, sSALEPOS.wSubID );
		if ( !bOK )									return false;
	}

	//	Note : 인벤토리에 등록.
	BOOL bOK = m_invenSALE.InsertItem ( sITEMCUSTOM, sSALEPOS.wMainID, sSALEPOS.wSubID );
	if ( !bOK )										return false;

	//	Note : 판매 정보 등록.
	SSALEITEM sSALE;
	sSALE.sSALEPOS = sSALEPOS;
	sSALE.sITEMCUSTOM = sInvenItem.sItemCustom;
	sSALE.sINVENPOS = sINVENPOS;
	sSALE.llPRICE = llPRICE;
	sSALE.dwNUMBER = dwNUM;

	m_mapSALE.insert ( std::make_pair(sSALEPOS.dwID,sSALE) );


	// Add Search Item Data
	SSEARCHITEMDATA searchItemData;
	StringCbCopy( searchItemData.szItemName, MAP_NAME_LENGTH, pITEM->GetName() );
	searchItemData.dwReqCharClass = pITEM->sBasicOp.dwReqCharClass;
	searchItemData.dwSuitType	  = (DWORD)pITEM->sSuitOp.emAttack;
	searchItemData.wReqLevel	  = pITEM->sBasicOp.wReqLevelDW;
	searchItemData.sReqStats	  = pITEM->sBasicOp.sReqStats;	
	searchItemData.llPRICE		  = llPRICE;

	searchItemData.RandOptTYPE1	  = (BYTE)sITEMCUSTOM.GETOptTYPE1();	
	searchItemData.RandOptTYPE2	  = (BYTE)sITEMCUSTOM.GETOptTYPE2()	;	
	searchItemData.RandOptTYPE3	  = (BYTE)sITEMCUSTOM.GETOptTYPE3();	
	searchItemData.RandOptTYPE4	  = (BYTE)sITEMCUSTOM.GETOptTYPE4()	;	

	searchItemData.fRandOptValue1 = sITEMCUSTOM.GETOptVALUE1();	
	searchItemData.fRandOptValue2 = sITEMCUSTOM.GETOptVALUE2();	
	searchItemData.fRandOptValue3 = sITEMCUSTOM.GETOptVALUE3();	
	searchItemData.fRandOptValue4 = sITEMCUSTOM.GETOptVALUE4();	

	searchItemData.cDAMAGE		  = sITEMCUSTOM.cDAMAGE;		
	searchItemData.cDEFENSE		  = sITEMCUSTOM.cDEFENSE;		

	searchItemData.cRESIST_FIRE	  = sITEMCUSTOM.cRESIST_FIRE;	
	searchItemData.cRESIST_ICE	  = sITEMCUSTOM.cRESIST_ICE;		
	searchItemData.cRESIST_ELEC   = sITEMCUSTOM.cRESIST_ELEC;		
	searchItemData.cRESIST_POISON = sITEMCUSTOM.cRESIST_POISON;		
	searchItemData.cRESIST_SPIRIT = sITEMCUSTOM.cRESIST_SPIRIT;		


	if( bSearchMarket )
		m_mapSearchItem.insert( std::make_pair( pITEM->sBasicOp.sNativeID.dwID, searchItemData ) );

	return true;
}

bool GLPrivateMarket::DisItem( SNATIVEID sSALEPOS, bool bSearchMarket )
{
	// Add Search Item Data
	if( bSearchMarket )
	{
		SSALEITEM *pSaleItem = GetItem ( sSALEPOS );
		if( pSaleItem )
		{
			MAPSEARCH_ITER pos = m_mapSearchItem.find( pSaleItem->sITEMCUSTOM.sNativeID.dwID );
			if( pos != m_mapSearchItem.end() )
			{
				m_mapSearchItem.erase(pos);
			}
		}
	}

	{
		MAPITEM_ITER pos = m_mapSALE.find ( sSALEPOS.dwID );
		if ( pos==m_mapSALE.end() )						return false;

		m_invenSALE.DeleteItem ( sSALEPOS.wMainID, sSALEPOS.wSubID );
		m_mapSALE.erase ( pos );
	}

	return true;
}

bool GLPrivateMarket::DoSale ( SNATIVEID sSALEPOS, DWORD dwNUM, bool bSearchMarket )
{
	MAPITEM_ITER pos = m_mapSALE.find ( sSALEPOS.dwID );
	if ( pos==m_mapSALE.end() )						return false;
	
	SSALEITEM &sSALE = (*pos).second;

	if ( sSALE.bSOLD )								return false;
	if ( sSALE.dwNUMBER < dwNUM )					return false;

	sSALE.dwNUMBER -= dwNUM;

	if ( 0==sSALE.dwNUMBER )
	{
		sSALE.bSOLD = true;
		if( bSearchMarket )
		{
			SSALEITEM *pSaleItem = GetItem ( sSALEPOS );
			if( pSaleItem )
			{
				MAPSEARCH_ITER pos = m_mapSearchItem.find( pSaleItem->sITEMCUSTOM.sNativeID.dwID );
				if( pos != m_mapSearchItem.end() )
				{
					m_mapSearchItem.erase(pos);
				}
			}
		}

	}

	return true;
}

SSALEITEM* GLPrivateMarket::GetItem ( SNATIVEID sSALEPOS )
{
	MAPITEM_ITER pos = m_mapSALE.find ( sSALEPOS.dwID );
	if ( pos==m_mapSALE.end() )						return NULL;

	return &(*pos).second;
}


std::vector<SFINDRESULT> GLPrivateMarket::FindItem( SSEARCHITEMDATA sSearchData )
{
	bool bFindUseClass = FALSE;
	bool bFindUseType  = FALSE;
	bool bFindUseLevel = FALSE;
	bool bFindUseStats = FALSE;

	if( sSearchData.dwReqCharClass		 != 0 ) bFindUseClass = TRUE;
	if( sSearchData.dwSuitType			 != 0 ) bFindUseType  = TRUE;
	if( sSearchData.wReqLevel			 != 0 ) bFindUseLevel = TRUE;
	if( !sSearchData.sReqStats.IsZERO()		  ) bFindUseStats = TRUE;

	std::vector<SFINDRESULT> vecFindResult;
	SFINDRESULT				 findResult;
	vecFindResult.clear();

	if( m_mapSearchItem.size() == 0 ) return vecFindResult;



	MAPSEARCH_ITER pos = m_mapSearchItem.begin();
	for( ; pos != m_mapSearchItem.end(); ++pos )
	{
		SSEARCHITEMDATA itemData = pos->second;

		CString str = itemData.szItemName;
		if( str.Find( sSearchData.szItemName ) == -1 ) 	continue;

		if( bFindUseClass )
		{
			if( !itemData.dwReqCharClass & sSearchData.dwReqCharClass ) continue;
		}

		if( bFindUseType )
		{
			if( itemData.dwSuitType != sSearchData.dwSuitType ) continue;
		}

		if( bFindUseLevel )
		{
			if( itemData.wReqLevel < sSearchData.wReqLevel ) continue;
		}

		if( bFindUseStats )
		{
			if( sSearchData.sReqStats.wPow != 0 && itemData.sReqStats.wPow == 0 )	continue;
			if( sSearchData.sReqStats.wDex != 0 && itemData.sReqStats.wDex == 0 )	continue;
			if( sSearchData.sReqStats.wSpi != 0 && itemData.sReqStats.wSpi == 0 )	continue;
		}
		findResult.sSaleItemID = pos->first;
		findResult.llPRICE	   = itemData.llPRICE;		
		vecFindResult.push_back( findResult );

	}
	return vecFindResult;


}
