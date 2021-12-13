#include "pch.h"
#include "./GLInventory.h"
#include "./GLItemMan.h"
#include "./stl_Func.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLInventory::GLInventory(void) :
	m_wCellSX(EM_INVENSIZE_X),
	m_wCellSY(EM_INVENSIZE_Y),
	m_wADDLINE(0),
	m_bLimit(false),
	m_wMaxItems(m_wCellSX*m_wCellSY),
	m_ppbBarrier(NULL)
{
	SetState ( m_wCellSX, m_wCellSY );
}

GLInventory::GLInventory(WORD _sizeX, WORD _sizeY) :
	m_wCellSX(_sizeX),
	m_wCellSY(_sizeY),
	m_wMaxItems(_sizeX*_sizeY),
	m_wADDLINE(0),
	m_bLimit(false),
	m_ppbBarrier(NULL)
{
	SetState ( m_wCellSX, m_wCellSY );
}

GLInventory::~GLInventory(void)
{
	CleanUp ();
}

void GLInventory::Assign ( const GLInventory &Inven )
{
	CleanUp ();
	SetAddLine ( Inven.GETAddLine(), Inven.IsLimit() );
	SetState ( Inven.m_wCellSX, Inven.m_wCellSY );

	//	기존 인벤과 크기가 같을 경우.
	if ( m_wCellSX==Inven.m_wCellSX && m_wCellSY==Inven.m_wCellSY )
	{
		CELL_MAP_CITER iter = Inven.m_ItemMap.begin();
		CELL_MAP_CITER iter_end = Inven.m_ItemMap.end();
		for ( ; iter!=iter_end; ++iter )
		{
			InsertItem ( (*iter).second->sItemCustom, (*iter).second->wPosX, (*iter).second->wPosY, true );
		}
	}
	//	크기가 틀려졌을 경우.
	else
	{
		SetState ( Inven.m_wCellSX, Inven.m_wCellSY );

		CELL_MAP_CITER iter = Inven.m_ItemMap.begin();
		CELL_MAP_CITER iter_end = Inven.m_ItemMap.end();
		for ( ; iter!=iter_end; ++iter )
		{
			InsertItem ( (*iter).second->sItemCustom );
		}
	}
}

void GLInventory::MakeBarrier ()
{
	m_ppbBarrier = new BYTE*[m_wCellSX];
	GASSERT(m_ppbBarrier);

	for ( int i=0; i<m_wCellSX; i++ )
	{
		m_ppbBarrier[i] = new BYTE[m_wCellSY];
		SecureZeroMemory ( m_ppbBarrier[i], sizeof(BYTE)*m_wCellSY );
		GASSERT(m_ppbBarrier[i]);
	}
}

void GLInventory::DeleteBarrier ()
{
	if ( m_ppbBarrier )
	{
		for ( int i=0; i<m_wCellSX; i++ )
		{
			SAFE_DELETE_ARRAY(m_ppbBarrier[i]);
		}
		SAFE_DELETE_ARRAY(m_ppbBarrier);

		m_ppbBarrier = NULL;
	}
}

void GLInventory::SetState ( WORD wCellSX, WORD wCellSY )
{
	GASSERT((m_wCellSX*m_wCellSY)<=255);

	GLInventory::CleanUp ();

	m_wCellSX = wCellSX;
	m_wCellSY = wCellSY;

	if ( m_wCellSX && m_wCellSY )	MakeBarrier ();
}

void GLInventory::SetAddLine ( WORD wLine, bool bLimit )
{
	m_bLimit = bLimit;
	m_wADDLINE = wLine;

	INT nADDLINE_MAXSIZE = EM_INVENSIZE_Y - EM_INVEN_DEF_SIZE_Y;

	if ( m_wADDLINE > nADDLINE_MAXSIZE )		m_wADDLINE = nADDLINE_MAXSIZE;
}

void GLInventory::SetItemGenTime ()
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;

		CTime tTIME = CTime::GetCurrentTime();
		pInvenItem->sItemCustom.tBORNTIME = tTIME.GetTime();
	}
}

//	Note : 인벤토리에서 사용한 메모리/장치들을 해제한다.
//		단 해제 순서상 CItemMan::Instance 의 해제보다 앞에 있어야만
//		메모리 누수가 발생하지 않는다.
//
void GLInventory::CleanUp ()
{
	m_wMaxItems = 0;
	DeleteBarrier ();

	if ( !m_ItemMap.empty() )
	{
		std::for_each ( m_ItemMap.begin(), m_ItemMap.end(), std_afunc::DeleteMapObject() );

		m_ItemMap.clear ();
	}
}

SINVENITEM* GLInventory::FindPosItem ( const WORD wPosX, const WORD wPosY )
{
	if ( wPosX >= m_wCellSX )	return NULL;
	if ( wPosY >= m_wCellSY )	return NULL;
	if ( !m_ppbBarrier[wPosX][wPosY] )	return NULL;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		pInvenItem->sItemCustom.sNativeID;
		if ( ( ( pInvenItem->wPosX <= wPosX ) && ( wPosX < (pInvenItem->wPosX+pItem->sBasicOp.wInvenSizeX) ) ) &&
			( ( pInvenItem->wPosY <= wPosY ) && ( wPosY < (pInvenItem->wPosY+pItem->sBasicOp.wInvenSizeY) ) ) )
		{
			return pInvenItem;
		}
	}

	return NULL;
}

BOOL GLInventory::FindInsrtable ( const WORD wSX, const WORD wSY, WORD &wPosX, WORD &wPosY, bool bAllLine )
{
	GASSERT(wSX!=0&&wSY!=0);
	GASSERT(wSX<=m_wCellSX&&wSY<=m_wCellSY);

	int nCellSX = m_wCellSX - wSX;
	int nCellSY = GetValidCellY() - wSY;
	if ( bAllLine ) nCellSY = m_wCellSY - wSY;
	
	for ( int cx=0; cx<=nCellSX; cx++ )
	for ( int cy=0; cy<=nCellSY; cy++ )
	{
		BOOL bOccupied = FALSE;
		for ( WORD i=0; i<wSX; i++ )
		for ( WORD j=0; j<wSY; j++ )
		{
			if ( m_ppbBarrier[cx+i][cy+j] )
			{
				bOccupied = TRUE;
			}
		}

		if ( !bOccupied )
		{
			wPosX = cx;
			wPosY = cy;
			return TRUE;
		}
	}

	wPosX = 0;
	wPosY = 0;
	return FALSE;
}

BOOL GLInventory::ValidCheckInsrt ( const WORD _wInsertNum, const WORD wSX, const WORD wSY, bool bAllLine )
{
	GASSERT(wSX!=0&&wSY!=0);
	GASSERT(wSX<=m_wCellSX&&wSY<=m_wCellSY);

	//	체크 메모리 생성.
	BYTE** ppbCheckBarr = (NULL);
	{
		ppbCheckBarr = new BYTE*[m_wCellSX];
		GASSERT(m_ppbBarrier);

		for ( int i=0; i<m_wCellSX; i++ )
		{
			ppbCheckBarr[i] = new BYTE[m_wCellSY];
			SecureZeroMemory ( ppbCheckBarr[i], sizeof(BYTE)*m_wCellSY );
			GASSERT(ppbCheckBarr[i]);
		}
	}

	WORD i(0), j(0);
	for ( i=0; i<m_wCellSX; i++ )
	for ( j=0; j<m_wCellSY; j++ )
	{
		ppbCheckBarr[i][j] = m_ppbBarrier[i][j];
	}

	//	인벤에 넣을수 있는 아이템 수.
	WORD wValidNum(0);
	for ( WORD wNum=0; wNum<_wInsertNum; ++wNum )
	{
		int nCellSX = m_wCellSX - wSX;
		int nCellSY = GetValidCellY() - wSY;
		if ( bAllLine ) nCellSY = m_wCellSY - wSY;
		for ( int cx=0; cx<=nCellSX; cx++ )
		for ( int cy=0; cy<=nCellSY; cy++ )
		{
			//	Note : 일정 크기의 아이템이 들어갈 공간이 있는지 점검.
			BOOL bOccupied = FALSE;
			for ( i=0; i<wSX; i++ )
			for ( j=0; j<wSY; j++ )
			{
				if ( ppbCheckBarr[cx+i][cy+j] )
				{
					bOccupied = TRUE;
				}
			}
			if ( bOccupied )	continue;

			//	Note : 아이템이 들어갈 공간이 충분하면 종료.
			//	
			wValidNum += 1;
			if ( wValidNum==_wInsertNum )		goto _END;

			//	Note : 아이템이 들어갈 공간을 점거함.

			for ( i=0; i<wSX; i++ )
			for ( j=0; j<wSY; j++ )
			{
				ppbCheckBarr[cx+i][cy+j] = TRUE;
			}
		}
	}

_END:

	//	체크 메모리 제거.
	if ( ppbCheckBarr )
	{
		for ( int i=0; i<m_wCellSX; i++ )
		{
			SAFE_DELETE_ARRAY(ppbCheckBarr[i]);
		}
		SAFE_DELETE_ARRAY(ppbCheckBarr);

		ppbCheckBarr = NULL;
	}

	return (wValidNum==_wInsertNum);
}

BOOL GLInventory::ValidPileInsrt ( const WORD wINSRTNUM, const SNATIVEID &sNID, const WORD wPILENUM, const WORD wSX, const WORD wSY, bool bAllLine )
{
	//	넣기 요청된 아이템수. ( 잔여량. )
	WORD wREQINSRTNUM(wINSRTNUM);

	//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	//		아이템 점검 숫자를 감소시킴.
	GLInventory::CELL_MAP_ITER iter = m_ItemMap.begin();
	for ( ; iter!=m_ItemMap.end(); ++iter )
	{
		SITEMCUSTOM &sITEMCUSTOM = (*iter).second->sItemCustom;
		if ( sITEMCUSTOM.sNativeID != sNID )		continue;
		if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;
	
		WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

		if ( wREQINSRTNUM > wSURPLUSNUM )
		{
			wREQINSRTNUM -= wSURPLUSNUM;
		}
		else
		{
			//	기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
			return TRUE;
		}
	}

	//	새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

	//	최대겹침아이템 수량.
	WORD wITEMNUM = wREQINSRTNUM / wPILENUM;

	//	여분겹침아이템의 겹침수.
	WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
	if ( wSPLITNUM > 0 )	wITEMNUM += 1;

	BOOL bOK = ValidCheckInsrt ( wITEMNUM, wSX, wSY, bAllLine );

	return (bOK);
}

DWORD GLInventory::CountItem ( const SNATIVEID sNID )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );

		if ( pItem && pItem->sBasicOp.sNativeID == sNID )	++dwNum;
	}

	return dwNum;
}

DWORD GLInventory::CountTurnItem ( const SNATIVEID sNID )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		if ( pInvenItem->sItemCustom.sNativeID == sNID )
		{
			dwNum += pInvenItem->sItemCustom.wTurnNum;
		}
	}

	return dwNum;
}

DWORD GLInventory::CountPileItem ( const SNATIVEID sNID )
{
	DWORD dwNum = 0;

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pInvenItem->sItemCustom.sNativeID != sNID )	continue;
		
		if ( pItem->sDrugOp.wPileNum == 1 )										dwNum++;
		else if ( pItem->sDrugOp.wPileNum == pInvenItem->sItemCustom.wTurnNum )	dwNum++;
	}

	return dwNum;
}

SINVENITEM* GLInventory::FindItem ( const SNATIVEID sNID )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		if ( pInvenItem->sItemCustom.sNativeID == sNID )	return pInvenItem;
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItem ( const EMITEM_TYPE emTYPE )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem && pItem->sBasicOp.emItemType == emTYPE )	return pInvenItem;
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItem ( const EMITEM_TYPE emTYPE, const SNATIVEID sNID )
{
	if ( emTYPE==ITEM_NSIZE )	return FALSE;

	if ( sNID != NATIVEID_NULL() )
	{
		CELL_MAP_ITER iter = m_ItemMap.begin();
		CELL_MAP_ITER iter_end = m_ItemMap.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
			GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

			if ( pItem->sBasicOp.sNativeID == sNID )	return pInvenItem;
		}		
	}

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem && pItem->sBasicOp.emItemType == emTYPE )	return pInvenItem;
	}

	return NULL;
}

BOOL GLInventory::HaveEventItem ()
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem && pItem->sBasicOp.IsEVENT() )	return TRUE;
	}

	return FALSE;
}

SINVENITEM* GLInventory::FindDrugItem ( EMITEM_DRUG emDrug )
{
	GASSERT(emDrug!=ITEM_DRUG_NUNE);

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem && pItem->sDrugOp.emDrug==emDrug )	return pInvenItem;
	}

	return NULL;
}

SINVENITEM* GLInventory::FindItemBack ( const WORD wBackX, const WORD wBackY )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;

		if ( pInvenItem->wBackX==wBackX && pInvenItem->wBackY==wBackY )		return pInvenItem;
	}

	return NULL;
}

DWORD GLInventory::GetAmountDrugItem ( EMITEM_DRUG emDrug )
{
	GASSERT(emDrug!=ITEM_DRUG_NUNE);

	DWORD dwAmount = 0;
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem && pItem->sDrugOp.emDrug==emDrug )
		{
			dwAmount += ( pInvenItem->sItemCustom.wTurnNum );
		}
	}

	return dwAmount;
}

bool GLInventory::GetCharResetItem ( WORD &wPosX, WORD &wPosY )
{
	DWORD dwAmount(0);
	SNATIVEID sNID_TAR(false);

	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;

		const SNATIVEID &sNID = pInvenItem->sItemCustom.sNativeID;

		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem->sBasicOp.emItemType!=ITEM_SKP_STAT_RESET )	continue;

		if ( pItem->sDrugOp.wPileNum==pInvenItem->sItemCustom.wTurnNum )
		{
			wPosX = pInvenItem->wPosX;
			wPosY = pInvenItem->wPosY;
			return true;
		}
	}

	return false;
}

bool GLInventory::GetPileItem ( const SNATIVEID _sNID, WORD &wPosX, WORD &wPosY )	//	겹침 가능 갯수 많큼 겹쳐 있는 것을 반환.
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;

		const SNATIVEID &sNID = pInvenItem->sItemCustom.sNativeID;
		if ( _sNID!=sNID )										continue;

		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");

		if ( pItem->sDrugOp.wPileNum==pInvenItem->sItemCustom.wTurnNum )
		{
			wPosX = pInvenItem->wPosX;
			wPosY = pInvenItem->wPosY;
			return true;
		}
	}

	return false;
}

WORD GLInventory::GetValidCellY () const
{
	if ( !m_bLimit )	return m_wCellSY;

	WORD wValidY = EM_INVEN_DEF_SIZE_Y + m_wADDLINE;
	if ( wValidY>m_wCellSY )	wValidY = m_wCellSY;

	return wValidY;
}

WORD GLInventory::GETAddLine () const
{
	return m_wADDLINE;
}

BOOL GLInventory::IsInsertable ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY, bool bLOAD )
{
	GASSERT(wSX!=0&&wSY!=0);
	GASSERT(wSX<=m_wCellSX&&wSY<=m_wCellSY);

	// 해킹하려는 흔적 발견
	if ( wSX==0||wSY==0 )               return FALSE;
	if ( wSX>m_wCellSX||wSY>m_wCellSY ) return FALSE;

	WORD wVALIDY = GetValidCellY();
	if ( bLOAD )	wVALIDY = m_wCellSY;

	if ( (m_wCellSX-wSX)<wPosX )		return FALSE;
	if ( (wVALIDY-wSY)<wPosY )			return FALSE;

	BOOL bInsrtable = TRUE;
	for ( WORD i=0; i<wSX; i++ )
	for ( WORD j=0; j<wSY; j++ )
	{
		if ( m_ppbBarrier[wPosX+i][wPosY+j] )
			return FALSE;
	}

	return TRUE;
}

void GLInventory::SetMark ( const WORD wSX, const WORD wSY, const WORD wPosX, const WORD wPosY )
{
	GASSERT(wSX!=0&&wSY!=0);
	GASSERT(wSX<=m_wCellSX&&wSY<=m_wCellSY);

	GASSERT((m_wCellSX-wSX)>=wPosX);
	GASSERT((m_wCellSY-wSY)>=wPosY);

	for ( WORD i=0; i<wSX; i++ )
	for ( WORD j=0; j<wSY; j++ )
	{
		GASSERT ( m_ppbBarrier[wPosX+i][wPosY+j]==FALSE && "인벤 barrier가 이미 점거 되어 있습니다." );

		GASSERT ( wPosX+i < m_wCellSX );
		GASSERT ( wPosY+j < m_wCellSY );
		m_ppbBarrier[wPosX+i][wPosY+j] = TRUE;
	}
}

void GLInventory::ReSetMark ( WORD wSX, WORD wSY, WORD wPosX, WORD wPosY )
{
	GASSERT(wSX!=0&&wSY!=0);
	GASSERT(wSX<=m_wCellSX&&wSY<=m_wCellSY);

	GASSERT((m_wCellSX-wSX)>=wPosX);
	GASSERT((m_wCellSY-wSY)>=wPosY);

	for ( WORD i=0; i<wSX; i++ )
	for ( WORD j=0; j<wSY; j++ )
	{
		GASSERT ( m_ppbBarrier[wPosX+i][wPosY+j]==TRUE && "인벤 barrier에 마크되지 않은 공간을 리샛하려고 했습니다." );

		GASSERT ( wPosX+i < m_wCellSX );
		GASSERT ( wPosY+j < m_wCellSY );
		m_ppbBarrier[wPosX+i][wPosY+j] = FALSE;
	}
}

BOOL GLInventory::InsertItem ( const SITEMCUSTOM &ItemCustom )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( ItemCustom.sNativeID );
	if ( !pItem )	return FALSE;

	WORD wPosX, wPosY;
	BOOL bOk = FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( bOk )
	{
		SINVENITEM* pInvenItem = new SINVENITEM;
		GASSERT ( pInvenItem&&"인벤토리 아이템 메모리 할당을 하지 못했습니다!" );

		//	Note : 인벤 공간을 마크함.
		//
		SetMark ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );

		pInvenItem->wPosX = wPosX;
		pInvenItem->wPosY = wPosY;
		pInvenItem->sItemCustom = ItemCustom;

		m_ItemMap.insert ( std::make_pair(CELL_KEY(wPosX,wPosY),pInvenItem) );

		return TRUE;
	}

	return FALSE;
}

//	Note : 초기 로드시에 ( Assign ) bLOAD 를 사용하면 ( 사용 허가된 line 이 아니더라도 넣어질수 있다. )
//
BOOL GLInventory::InsertItem ( const SITEMCUSTOM &ItemCustom, const WORD wPosX, const WORD wPosY, bool bLOAD )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( ItemCustom.sNativeID );
	if ( !pItem )	return FALSE;

	BOOL bOk = IsInsertable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY, bLOAD );
	if ( bOk )
	{
		SINVENITEM* pInvenItem = new SINVENITEM;
		GASSERT ( pInvenItem&&"인벤토리 아이템 메모리 할당을 하지 못했습니다!" );

		//	Note : 인벤 공간을 마크함.
		//
		SetMark ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );

		pInvenItem->wPosX = wPosX;
		pInvenItem->wPosY = wPosY;
		pInvenItem->sItemCustom = ItemCustom;

		m_ItemMap.insert ( std::make_pair(CELL_KEY(wPosX,wPosY),pInvenItem) );
		
		return TRUE;
	}

	return FALSE;
}

BOOL GLInventory::InsertItem ( const SITEMCUSTOM &ItemCustom, const WORD wPosX, const WORD wPosY, const WORD wBackX, const WORD wBackY )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( ItemCustom.sNativeID );
	if ( !pItem )	return FALSE;

	BOOL bOk = IsInsertable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( bOk )
	{
		SINVENITEM* pInvenItem = new SINVENITEM;
		GASSERT ( pInvenItem&&"인벤토리 아이템 메모리 할당을 하지 못했습니다!" );

		//	Note : 인벤 공간을 마크함.
		//
		SetMark ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );

		pInvenItem->wPosX = wPosX;
		pInvenItem->wPosY = wPosY;

		pInvenItem->wBackX = wBackX;
		pInvenItem->wBackY = wBackY;

		pInvenItem->sItemCustom = ItemCustom;

		m_ItemMap.insert ( std::make_pair(CELL_KEY(wPosX,wPosY),pInvenItem) );
		
		return TRUE;
	}

	return FALSE;
}

BOOL GLInventory::DeleteItem ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = GetItem ( wPosX, wPosY );
	if ( !pInvenItem )	return FALSE;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");
	SITEM &Item = *pItem;

	//	Note : 인벤 공간 마크를 풀어줌.
	//
	ReSetMark ( Item.sBasicOp.wInvenSizeX, Item.sBasicOp.wInvenSizeY, wPosX, wPosY );

	CELL_MAP_ITER iter_erase = m_ItemMap.find ( CELL_KEY(wPosX,wPosY) );
	if ( iter_erase!=m_ItemMap.end() )
	{
		m_ItemMap.erase ( iter_erase );
		SAFE_DELETE(pInvenItem);
	}

	return TRUE;
}

void GLInventory::DeleteItemAll ()
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
	
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");
		SITEM &Item = *pItem;

		//	Note : 인벤 공간 마크를 풀어줌.
		//
		ReSetMark ( Item.sBasicOp.wInvenSizeX, Item.sBasicOp.wInvenSizeY, pInvenItem->wPosX, pInvenItem->wPosY );
		SAFE_DELETE(pInvenItem);
	}

	m_ItemMap.clear ();
}

SINVENITEM* GLInventory::GetItem ( WORD wPosX, WORD wPosY )
{
	CELL_MAP_ITER iter_get = m_ItemMap.find ( CELL_KEY(wPosX,wPosY) );
	if ( iter_get != m_ItemMap.end() )	return (*iter_get).second;

	return NULL;
}

BOOL GLInventory::SETITEM_BYBUFFER ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETITEM_BYBUFFER() CByteStream::IsEmpty()" );
		return TRUE;
	}

	DeleteItemAll();

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + SINVENITEM_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;
	
	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;
	
	SINVENITEM sInvenItem;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==0x0100 )
		{
			SINVENITEM_SAVE_100 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0101 )
		{
			SINVENITEM_SAVE_101 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0102 )
		{
			SINVENITEM_SAVE_102 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0103 )
		{
			SINVENITEM_SAVE_103 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0104 )
		{
			SINVENITEM_SAVE_104 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0105 )
		{
			SINVENITEM_SAVE_105 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0106 )
		{
			SINVENITEM_SAVE_106 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0107 )
		{
			SINVENITEM_SAVE_107 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0108 )
		{
			SINVENITEM_SAVE_108 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0109 )
		{
			SINVENITEM_SAVE_109 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0110 )
		{
			SINVENITEM_SAVE_110 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else if ( dwVersion==0x0111)
		{
			SINVENITEM_SAVE_111 sOld;
			GASSERT(dwSize==sizeof(sOld));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sOld, sizeof(sOld) );
			if ( !bOk )
			{
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			sInvenItem.sSaveData.Assign ( sOld );
			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );

		}
		else if ( dwVersion==SINVENITEM_SAVE::VERSION )
		{
			GASSERT(dwSize==sizeof(SINVENITEM_SAVE));
			BOOL bOk = ByteStream.ReadBuffer ( (LPBYTE)&sInvenItem.sSaveData, sizeof(SINVENITEM_SAVE) );
			if ( !bOk )
			{
				GASSERT ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." && 0 );
				CDebugSet::ToLogFile ( "inventory save byte stream 처리 도중 오류가 발생하였습니다." );
				return FALSE;	//ERROR
			}

			InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY, true );
		}
		else
		{
			//	Note : 알수 없는 버전의 inventory save structure 입니다.
			//
			GASSERT ( "알수 없는 버전의 inventory save structure 입니다." && 0 );
			CDebugSet::ToLogFile ( "알수 없는 버전의 inventory save structure 입니다." );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GLInventory::GETITEM_BYBUFFER ( CByteStream &ByteStream ) const
{
	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + SINVENITEM_ARRAY[SIZE]
	//
	ByteStream << SINVENITEM_SAVE::VERSION;
	ByteStream << (DWORD)sizeof(SINVENITEM_SAVE);
	
	DWORD dwSize = (DWORD) m_ItemMap.size();
	ByteStream << dwSize;

	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM *pInvenItem = (*iter).second;

		ByteStream.WriteBuffer ( (LPBYTE)&pInvenItem->sSaveData, (DWORD)sizeof(SINVENITEM_SAVE) );
	}
	
	return TRUE;
}

BOOL GLInventory::GETITEM_BYBUFFER_FORSTORAGE ( CByteStream &ByteStream ) const
{
	ByteStream << SINVENITEM_SAVE::VERSION;
	ByteStream << (DWORD)sizeof(SINVENITEM_SAVE);
	
	DWORD dwSize = (DWORD) m_ItemMap.size();
	ByteStream << dwSize;

	CELL_MAP_CITER iter = m_ItemMap.begin();
	CELL_MAP_CITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM *pInvenItem = (*iter).second;
		ByteStream.WriteBuffer ( (LPBYTE)&pInvenItem->sSaveData, (DWORD)sizeof(SINVENITEM_SAVE) );
	}

	return TRUE;
}

SINVENITEM* GLInventory::FindItemByGenNumber ( LONGLONG llGenNum, SNATIVEID sID, BYTE cGenType )
{
	CELL_MAP_ITER iter = m_ItemMap.begin();
	CELL_MAP_ITER iter_end = m_ItemMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
		GASSERT(pItem&&"잘못된 고유 ITEM ID입니다.");
		
		if ( pInvenItem->sItemCustom.lnGenNum == llGenNum
			&& pInvenItem->sItemCustom.sNativeID == sID 
			&& pInvenItem->sItemCustom.cGenType == cGenType )
		{
			return pInvenItem;
		}
	}

	return NULL;
}