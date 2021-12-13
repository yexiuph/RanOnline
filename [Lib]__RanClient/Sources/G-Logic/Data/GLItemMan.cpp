#include "pch.h"
#include "./GLItemMan.h"
#include "./GLGaeaServer.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/SUBPATH.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinPieceContainer.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinCharData.h"

//#include "./GLStringTable.h"
//#include "./GLAgentServer.h"
//#include "../NpcTalk/NpcDialogueSet.h"
//#include "../[Lib]__Engine/Sources/DxResponseMan.h"
//#include "../[Lib]__Engine/Sources/Common/SeqRandom.h"
//#include "../[Lib]__Engine/Sources/DxLand/DxLandMan.h"
//#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"
//#include "../[Lib]__Engine/Sources/DxMeshs/DxSimpleMeshMan.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLItemMan& GLItemMan::GetInstance()
{
	static GLItemMan Instance;
	return Instance;
}

const char* GLItemMan::_FILEHEAD = "GLITEM";

GLItemMan::GLItemMan () 
	: m_pd3dDevice(NULL)
	, m_ppItem(NULL)
	, m_bModify(false)
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
	
	m_strMoneyMesh[0] = "Money_1.X";
	m_strMoneyMesh[1] = "Money_2.X";
	m_strMoneyMesh[2] = "Money_3.X";

	SecureZeroMemory ( m_pMoneyMesh, sizeof(DxSimMesh*)*MONEYMESH_NUM );

	m_sQITEMID[0] = SNATIVEID((WORD) 9,(WORD)1);
	m_sQITEMID[1] = SNATIVEID((WORD) 9,(WORD)2);
	m_sQITEMID[2] = SNATIVEID((WORD) 9,(WORD)3);
	m_sQITEMID[3] = SNATIVEID((WORD) 9,(WORD)4);
	m_sQITEMID[4] = SNATIVEID((WORD) 9,(WORD)6);
	m_sQITEMID[5] = SNATIVEID((WORD) 9,(WORD)29);
	m_sQITEMID[6] = SNATIVEID((WORD) 9,(WORD)30);
	m_sQITEMID[7] = SNATIVEID((WORD) 9,(WORD)31);
}

GLItemMan::~GLItemMan ()
{
	CleanUp ();
}

HRESULT GLItemMan::LoadFile ( const char* szFile, BOOL bServer, bool bPastLoad )
{
	CleanUp ();

	OneTimeSceneInit ();

	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bGLOGIC_ZIPFILE, 
																	GLOGIC::strGLOGIC_ZIPFILE.c_str(),
																	szFullPath, 
																	szFile,
																	false,
																	GLOGIC::bGLOGIC_PACKFILE ) );

	if ( !pBStream.get() )
		return E_FAIL;

	basestream &SFile = *pBStream;


	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( (dwFILEVER>=ENCODE_VER) );

	DWORD dwNum=0;

	SFile >> dwNum;

	for ( DWORD i=0; i<dwNum; i++ )
	{
		CItemNode sItemNode;

		if( sItemNode.m_sItem.LoadFile ( SFile, bPastLoad  ) == E_FAIL )
			return E_FAIL;

		if ( sItemNode.m_sItem.GETAPPLYNUM() == 0 )
		{
			CDebugSet::ToLogFile ( "%d,%d, %s : 잘못된 apply num입니다.", sItemNode.m_sItem.sBasicOp.sNativeID.wMainID, 
				sItemNode.m_sItem.sBasicOp.sNativeID.wSubID, sItemNode.m_sItem.GetName() );
		}

		if ( bServer )
		{
			//	Note : 랜덤 옵션 로드. ( random option )
			sItemNode.m_pRANDOM_SET = LoadRandomGenData ( sItemNode.m_sItem.GetRandomOptFile() );
			if ( !sItemNode.m_pRANDOM_SET && strlen(sItemNode.m_sItem.sRandomOpt.szNAME)>0 )
			{
				CDebugSet::ToLogFile ( "item ran option setting file load fail : %s", sItemNode.m_sItem.sRandomOpt.szNAME );
			}
		}

		GASSERT ( m_ppItem[sItemNode.m_sItem.sBasicOp.sNativeID.wMainID][sItemNode.m_sItem.sBasicOp.sNativeID.wSubID]==NULL );
		InsertItem ( sItemNode.m_sItem.sBasicOp.sNativeID, &sItemNode, true );
	}

	ItemGenerateSet ();

	return S_OK;
}

HRESULT GLItemMan::SaveFile ( const char* szFile )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH];
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	CSerialFile SFile;
	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	DWORD dwNum=0;
	int i = 0, j = 0;
	for ( i = 0; i < MAX_MID; i++ )
	for ( j = 0; j < MAX_SID; j++ )
	{
		if ( m_ppItem[i][j] )	dwNum++;
	}

	SFile << dwNum;

	for ( i = 0; i < MAX_MID; i++ )
	{
		for ( j = 0; j < MAX_SID; j++ )
		{
			if ( m_ppItem[i][j] )
			{					
				m_ppItem[i][j]->m_sItem.SaveFile ( SFile );
			}
		}
	}

	m_bModify = false;
	return S_OK;
}


PITEMNODE GLItemMan::GetItemNode ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_ppItem);

	return m_ppItem[wMID][wSID];
}

SRANDOM_GEN* GLItemMan::GetItemRandomOpt ( SNATIVEID sNativeID )
{
	PITEMNODE pNODE = GetItemNode ( sNativeID.wMainID, sNativeID.wSubID );
	if ( !pNODE )						return NULL;

	return pNODE->m_pRANDOM_SET;
}

SITEM* GLItemMan::GetItem ( WORD wMID, WORD wSID )
{
	if ( !m_ppItem )		return NULL;

	if ( wMID==0xFFFF )		return NULL;
	if ( wSID==0xFFFF )		return NULL;
	if ( wMID>=MAX_MID )	return NULL;
	if ( wSID>=MAX_SID )	return NULL;

	if ( m_ppItem[wMID][wSID] )	return &(m_ppItem[wMID][wSID]->m_sItem);
	return NULL;
}

WORD GLItemMan::FindFreeMID ()
{
	GASSERT(m_ppItem);

	for ( WORD i=0; i<MAX_MID; i++ )
	{
		if ( !m_ppItem[i][0] )			return i;
	}

	return ITEMID_NOTFOUND;
}

WORD GLItemMan::FindFreeSID ( WORD wMID )
{
	GASSERT(m_ppItem);
	GASSERT(wMID<MAX_MID);
	
	for ( WORD i=0; i<MAX_SID; i++ )
	{
		if ( !m_ppItem[wMID][i] )		return i;
	}

	return ITEMID_NOTFOUND;
}

BOOL GLItemMan::InsertItem ( WORD wMID, WORD wSID, PITEMNODE pItem, bool binner )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);

	if ( !binner )		m_bModify = true;

	if ( m_ppItem[wMID][wSID] )
	{
		*m_ppItem[wMID][wSID] = *pItem;
		return FALSE;
	}

	m_ppItem[wMID][wSID] = new CItemNode;
	*m_ppItem[wMID][wSID] = *pItem;

	//	해당 아이템 분류군에 아이템을 등록함.
	DWORD dwSpecID = pItem->m_sItem.sGenerateOp.dwSpecID;
	if ( dwSpecID < SPECID_NUM )
	{
		m_sGenItems[dwSpecID].m_vecItems.push_back ( pItem->m_sItem.sBasicOp.sNativeID );
	}

	return TRUE;
}

BOOL GLItemMan::ItemGenerateSet ()
{
	for ( int i=0; i < SPECID_NUM; ++i )
	{
		SGENITEMS &sGenItems = m_sGenItems[i];

		WORD wSize = (WORD) sGenItems.m_vecItems.size();
		for ( int s=0; s<SHUFFLE_NUM; ++s )
		{
			for ( int ren=0; ren<wSize; ++ren )	sGenItems.m_sRendom.push_back(ren);
		}

		std::random_shuffle ( sGenItems.m_sRendom.begin(), sGenItems.m_sRendom.end() );
	}

	return TRUE;
}

SNATIVEID GLItemMan::RendomGenSpecID ( DWORD dwSpecID )
{
	GASSERT ( dwSpecID<SPECID_NUM );
	SGENITEMS &sGenItems = m_sGenItems[dwSpecID];
	
	if ( sGenItems.m_sRendom.empty() )	return NATIVEID_NULL();

	WORD wNow = sGenItems.m_sRendom[sGenItems.m_wNowCur];
	SNATIVEID sNativID = sGenItems.m_vecItems[wNow];
	
	++sGenItems.m_wNowCur;
	if ( sGenItems.m_wNowCur >= sGenItems.m_sRendom.size() )
		sGenItems.m_wNowCur = 0;

	return sNativID;
}

DxSkinPiece* GLItemMan::LoadSkinPiece ( WORD wMID, WORD wSID, EMCHARINDEX emIndex )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");

	PITEMNODE pItem = GetItemNode ( wMID, wSID );
	if ( !pItem )	return NULL;

	if ( !pItem->m_pSkinPiece )
	{
		pItem->m_pSkinPiece[emIndex] = DxSkinPieceContainer::GetInstance().LoadPiece( pItem->m_sItem.GetWearingFile(emIndex), m_pd3dDevice, TRUE );
	}

	return pItem->m_pSkinPiece[emIndex];
}

DxSimMesh* GLItemMan::GetFieldMesh ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");

	PITEMNODE pItem = GetItemNode ( wMID, wSID );
	if ( !pItem )	return NULL;

	if ( !pItem->m_pFieldMesh )
	{
		DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
		pItem->m_pFieldMesh = DxSimpleMeshMan::GetInstance().Load ( pItem->m_sItem.GetFieldFile(), m_pd3dDevice, dwFVF );
	}

	return pItem->m_pFieldMesh;
}

DxSimMesh* GLItemMan::GetInvenMesh ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");

	PITEMNODE pItem = GetItemNode ( wMID, wSID );
	if ( !pItem )	return NULL;

	if ( !pItem->m_pInvenMesh )
	{
		DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
		pItem->m_pInvenMesh = DxSimpleMeshMan::GetInstance().Load ( pItem->m_sItem.GetInventoryFile(), m_pd3dDevice, dwFVF );
	}

	return pItem->m_pInvenMesh;
}

HRESULT GLItemMan::OneTimeSceneInit ()
{
	//	Note : 아이탬 포인터 배열 생성.
	m_ppItem = new PITEMNODE*[MAX_MID];
	for ( WORD i=0; i<MAX_MID; i++ )
	{
		m_ppItem[i] = new PITEMNODE[MAX_SID];
		SecureZeroMemory ( m_ppItem[i], sizeof(PITEMNODE)*MAX_SID );
	}

	return S_OK;
}

HRESULT GLItemMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	for ( int i=0; i<MONEYMESH_NUM; i++ )
	{
		DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
		m_pMoneyMesh[i] = DxSimpleMeshMan::GetInstance().Load ( m_strMoneyMesh[i].c_str(), m_pd3dDevice, dwFVF );
	}

	return S_OK;
}

HRESULT GLItemMan::DeleteDeviceObjects ()
{
	if ( !m_ppItem )	return S_OK;

	for ( WORD wMID=0; wMID<MAX_MID; wMID++ )
	for ( WORD wSID=0; wSID<MAX_SID; wSID++ )
	{
		if ( m_ppItem[wMID][wSID] )
		{
			if ( m_ppItem[wMID][wSID]->m_pSkinPiece )
			{
				for ( int i=0; i<GLCI_NUM_NEWSEX; i++ )
				{
					DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetWearingFile((EMCHARINDEX)i) );
					m_ppItem[wMID][wSID]->m_pSkinPiece[i] = NULL;
				}
			}

			if ( m_ppItem[wMID][wSID]->m_pInvenMesh )
			{
				DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetInventoryFile() );
				m_ppItem[wMID][wSID]->m_pInvenMesh = NULL;
			}

			if ( m_ppItem[wMID][wSID]->m_pFieldMesh )
			{
				DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetFieldFile() );
				m_ppItem[wMID][wSID]->m_pFieldMesh = NULL;
			}
		}
	}

	for ( int i=0; i<MONEYMESH_NUM; i++ )
	{
		DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
		DxSimpleMeshMan::GetInstance().Release ( m_strMoneyMesh[i].c_str(), dwFVF );
	}

	return S_OK;
}

HRESULT GLItemMan::FinalCleanup ()
{
	if ( !m_ppItem )	return S_OK;

	for ( WORD i=0; i<MAX_MID; i++ )
	{
		for ( WORD j=0; j<MAX_SID; j++ )
		{
			SAFE_DELETE(m_ppItem[i][j]);
		}
		SAFE_DELETE_ARRAY(m_ppItem[i]);
	}
	SAFE_DELETE_ARRAY(m_ppItem);

	size_t nSIZE = m_sRandom.size();
	for ( size_t i=0; i<nSIZE; ++i )
	{
		SAFE_DELETE(m_sRandom[i].pSET);
	}
	m_sRandom.clear();

	return S_OK;
}

BOOL GLItemMan::DeleteItem ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);

	SAFE_DELETE ( m_ppItem[wMID][wSID] );

	m_bModify = true;
	return TRUE;
}

BOOL GLItemMan::DeleteItem ( SNATIVEID sNativeID )
{
	DeleteItem ( sNativeID.wMainID, sNativeID.wSubID );

	return TRUE;
}

const char* GLItemMan::_LOGFILE = "_ItemData_Synce.txt";
const char* GLItemMan::_STRINGTABLE = "ItemStrTable.txt";

static CString	GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

static BOOL ISFILE_OBJ_EXIST ( CString strFile )
{
	CString strFullName;
	strFullName = DxSimpleMeshMan::GetInstance().GetPath() + strFile;

	BOOL bEXIST = FALSE;
	FILE* file = NULL;
	fopen_s(&file, strFullName.GetString(), "r");
	if ( file )
	{
		bEXIST = TRUE;
		fclose(file);
	}

	return bEXIST;
}

static BOOL ISFILE_CPS_EXIST ( CString strFile )
{
	CString strFullName;
	strFullName = DxSkinPieceContainer::GetInstance().GetPath() + strFile;

	BOOL bEXIST = FALSE;
 	FILE *file = NULL;
	fopen_s(&file, strFullName.GetString(), "r");
	if ( file )
	{
		bEXIST = TRUE;
		fclose(file);
	}

	return bEXIST;
}

bool GLItemMan::ValidData ()
{
	if ( !m_ppItem )	return false;

	for ( int i = 0; i < MAX_MID; i++ )
	for ( int j = 0; j < MAX_SID; j++ )
	{
		if ( m_ppItem[i][j] )		return true;
	}

	return false;
}

HRESULT GLItemMan::SyncUpdateData ()
{
	//	Note : 데이터 동기화 점검을 위한 초기화.
	//
	CString strAppPath;
	char szAppPath[MAX_PATH], szFullPath[MAX_PATH];
	strAppPath = GetAppPath ();
	StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

	// Note : SimpleMesh 기본 폴더 지정
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_OBJECT );
	DxSimpleMeshMan::GetInstance().SetPath ( szFullPath );

	//	Note : SkinObject 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );

	int nCHAR_FLAGS[GLCI_NUM_NEWSEX] =
	{
		GLCC_FIGHTER_M,
		GLCC_ARMS_M,
		GLCC_ARCHER_W,
		GLCC_SPIRIT_W,
		GLCC_EXTREME_M,
		GLCC_EXTREME_W,
		GLCC_FIGHTER_W,
		GLCC_ARMS_W,
		GLCC_ARCHER_M,
		GLCC_SPIRIT_M,
	};

	char strCHAR_FLAGS[GLCI_NUM_NEWSEX][256] =
	{
		"FighterM",	//격투부
		"KnightM",	//검도부
		"ArcherW",	//양궁부
		"MagicianW",	//기예부
		"ExtremeM",	//극강부 남
		"ExtremeW",	//극강부 여
		"FighterW",	//격투부
		"KnightW",	//검도부
		"ArcherM",	//양궁부
		"MagicianM",	//기예부

	};

	GASSERT ( m_ppItem );
	if ( !m_ppItem )	return E_FAIL;

	CDebugSet::ClearFile ( _LOGFILE );

	for ( int i = 0; i < MAX_MID; i++ )
	for ( int j = 0; j < MAX_SID; j++ )
	{
		if ( !m_ppItem[i][j] )		continue;

		CItemNode *pITEMNODE = m_ppItem[i][j];
		SITEM &sITEM = pITEMNODE->m_sItem;

		if ( sITEM.GetFieldFile() )
		{	//필드 메시가 설정되지 않음
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not setting field mesh", i, j, sITEM.GetName() );
		}
		else if ( !ISFILE_OBJ_EXIST(sITEM.GetFieldFile()) )
		{	//설정한 필드메시가 없음
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not search field mesh [%s]", i, j, sITEM.GetName(), sITEM.GetFieldFile() );
		}

		if ( sITEM.GetInventoryFile() )
		{	//'인벤메시'가 설정되지 않음
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not setting inven mesh", i, j, sITEM.GetName() );
		}
		else if ( !ISFILE_OBJ_EXIST(sITEM.GetInventoryFile()) )
		{	//설정한 '인벤메시' 없음
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not search inven mesh [%s]", i, j, sITEM.GetName(), sITEM.GetInventoryFile() );
		}

		if ( sITEM.sBasicOp.wInvenSizeX!=1 || sITEM.sBasicOp.wInvenSizeY!=1 )
		{	//인벤 크기가 1,1 이 아닙니다
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not inven size 1,1", i, j, sITEM.GetName() );

			sITEM.sBasicOp.wInvenSizeX = 1;
			sITEM.sBasicOp.wInvenSizeY = 1;
		}

		if ( sITEM.sSuitOp.emHand==HAND_LEFT )
		{	//왼손 도구
			CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - left hand equip", i, j, sITEM.GetName() );
		}

		if ( sITEM.sBasicOp.emItemType!=ITEM_SUIT )	continue;

		for ( int nClass=0; nClass<GLCI_NUM_NEWSEX; ++nClass )
		{
			if ( sITEM.sBasicOp.dwReqCharClass & nCHAR_FLAGS[nClass] )
			{
				if ( sITEM.GetWearingFile((EMCHARINDEX)nClass) )
				{	//'착용메시'가 설정되지 않음
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s][%s] - not setting wear mesh ", i, j, sITEM.GetName(), strCHAR_FLAGS[nClass] );
				}
				else if ( !ISFILE_CPS_EXIST(sITEM.GetWearingFile((EMCHARINDEX)nClass)) )
				{	//설정한 '착용메시' 없음
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s][%s] - not search wear mesh [%s]", i, j, sITEM.GetName(), strCHAR_FLAGS[nClass], sITEM.GetWearingFile((EMCHARINDEX)nClass) );
				}
			}
		}
	}

	CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );	

	return S_OK;
}

HRESULT GLItemMan::SyncStringTable()
{
	GASSERT ( m_ppItem );
	if ( !m_ppItem )	return E_FAIL;

	char	szFullPathFileName[MAX_PATH] = "";
	StringCchCopy ( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPathFileName, MAX_PATH, _STRINGTABLE );

	//DeleteFile ( szFullPathFileName );

	CString strBuffer1, strBuffer;
	std::string strTemp;
	std::string::size_type idx;

	strBuffer = "// File : Item String Table\r\n";
	strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";
	
	for ( int i = 0; i < MAX_MID; i++ )
	for ( int j = 0; j < MAX_SID; j++ )
	{
		if ( !m_ppItem[i][j] )		continue;

		CItemNode *pITEMNODE = m_ppItem[i][j];
		SITEM &sITEM = pITEMNODE->m_sItem;

		if( !sITEM.sBasicOp.strName.empty() )
		{
			strBuffer1.Format( _T("IN_%03d_%03d\t%s\r\n"), i, j, sITEM.sBasicOp.strName.c_str() );
			strBuffer += strBuffer1;
		}
		
		if( !sITEM.sBasicOp.strComment.empty() )
		{
			strTemp = sITEM.sBasicOp.strComment;
			idx = strTemp.find( "\r\n" );
			while ( idx != std::string::npos )
			{
				strTemp.replace( idx, 2, " " );
				idx = strTemp.find( "\r\n" );
			}
		
			strBuffer1.Format( _T("ID_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
			strBuffer += strBuffer1;
		}
	}
	//strBuffer.SetAt(strBuffer.GetLength()-1, NULL);
	//strBuffer.SetAt(strBuffer.GetLength()-2, NULL); // 마지막 \n은 제거한다.

	//DWORD dwLength = strBuffer.GetLength() + 1;
	//char * szEncode = new char[dwLength]; // 인코딩배열에 스트링 길이만큼 메모리 할당
	//StringCchCopy( szEncode, dwLength, strBuffer.GetString() ); // 인코딩배열에 스트링 복사
	//compbyte::encode ( (BYTE*)szEncode, dwLength ); // 인코딩

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite ); // 파일 열기
	file.Write( strBuffer.GetString(), strBuffer.GetLength()-2 ); // 파일 쓰기
	file.Close();

	//delete [] szEncode;
	return S_OK;
}

HRESULT GLItemMan::SaveCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return S_FALSE;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	// Csv Head
	SITEM::SaveCsvHead( streamFILE );

	for( int i=0; i<MAX_MID; ++i )
	{
		for( int j=0; j<MAX_SID; ++j )
		{
			if( !m_ppItem[i][j] )
				continue;

			// Csv Data
			CItemNode *pITEMNODE = m_ppItem[i][j];
			SITEM &sITEM = pITEMNODE->m_sItem;
			sITEM.SaveCsv( streamFILE );
		}
	}

	streamFILE.close();

	return S_OK;
}

HRESULT GLItemMan::LoadCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return S_FALSE;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return S_FALSE;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	// Csv Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();	

	int iLine = 1;
	char szError[ 256 ];

	while( StrFile.GetNextLine( strLine ) )
	{
		++iLine;

		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return S_FALSE;
		}

		int iMID = atoi( StrArray[0] );
		int iSID = atoi( StrArray[1] );

		if( iMID >= MAX_MID || iSID >= MAX_SID )
			continue;

		if( !m_ppItem[ iMID ][ iSID ] )
		{
			CItemNode pItemNode;
			if( !InsertItem( iMID, iSID, &pItemNode ) )
				continue;
		}

		// Csv Data
		CItemNode *pITEMNODE = m_ppItem[ iMID ][ iSID ];
		SITEM &sITEM = pITEMNODE->m_sItem;
		sITEM.LoadCsv( StrArray );
	}

	return S_OK;
}

#define USAGE_RANDOM_OPT_SE
#ifdef USAGE_RANDOM_OPT_SE
//#ifdef DAUMTESTPARAM	// 한국 테섭에서만 2차 랜덤 옵션 파일 사용 ( 준혁 )
	SRANDOM_GEN* GLItemMan::LoadRandomGenData ( const char* szName )
	{
		if( !szName )
			return FALSE;
		if( strlen( szName ) == 0 )
			return FALSE;

		VEC_RANDOM::iterator found = std::lower_bound( m_sRandom.begin(), m_sRandom.end(), szName, SRANDOM_DATA_OPER() );
		if( found != m_sRandom.end() && !strcmp( (*found).szNAME,szName ) )
			return (*found).pSET;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szName;

		SRANDOM_DATA sDATA;
		StringCchCopy( sDATA.szNAME, SRANDOM_DATA::NAME_LEN, szName );
		sDATA.pSET = new SRANDOM_GEN;
		SRANDOM_GEN &sGEN = *sDATA.pSET;

		gltexfile cFILE;
		cFILE.reg_sep( '\t' );
		cFILE.reg_sep( ' ' );
		cFILE.reg_sep( ',' );

		if( GLOGIC::bGLOGIC_ZIPFILE )
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

		if( !cFILE.open( strPath, true ) )
		{
			CDebugSet::ToLogFile( "ERROR : GLItemMan::LoadRandomGenData(), %s", szName );
			SAFE_DELETE( sDATA.pSET );
			return NULL;
		}

		cFILE.getflag( "fRATE", 1, 1, sGEN.fRATE );
		cFILE.getflag( "dwSET_NUM", 1, 1, sGEN.dwSET_NUM );
		cFILE.getflag( "D_point", 1, 1, sGEN.fD_point );
		cFILE.getflag( "S_value", 1, 1, sGEN.fS_value );

		TCHAR strObj[2][4] = { "NEW", "REB" };
		TCHAR szOPTION[64] = {0};
		TCHAR szOPT_SE[64] = {0};

		for( INT i=0; i<2; ++i )
		{
			for( DWORD j=0; j<sGEN.dwSET_NUM && j<SRANDOM_GEN::MAX_SET; ++j )
			{
				SRANDOM_OPT_SET* pSET = &sGEN.sSET[j];
				if( i == 1 )
					pSET = &sGEN.sSETR[j];

				//std::strstream strOption;
				//strOption << strObj[i] << "_OPTION_" << j+1 << std::ends;

				_snprintf_s( szOPTION, 64, "%s_OPTION_%u", strObj[i], j+1 );

				cFILE.getflag( szOPTION, 1, 3, pSET->fRATE );
				cFILE.getflag( szOPTION, 2, 3, pSET->wTYPE );
				cFILE.getflag( szOPTION, 3, 3, pSET->wSECT );

				//strOption.freeze( false );

				if( pSET->wSECT <= 0 )
				{
					CDebugSet::ToLogFile( "ERROR : GLItemMan::LoadRandomGenData(), %s", szName );
					SAFE_DELETE( sDATA.pSET );
					return NULL;
				}

				pSET->pSECT = new SRANDOM_OPT_SECTION[ pSET->wSECT ];

				for( WORD k=0; k<pSET->wSECT; ++k )
				{
					_snprintf_s( szOPT_SE, 64, "%s_OPT_SE_%u_%u", strObj[i], j+1, k+1 );

					//std::strstream strSection;
					//strSection << strObj[i] << "_OPT_SE_" << j+1 << "_" << k+1 << std::ends;

					cFILE.getflag( szOPT_SE, 1, 3, pSET->pSECT[k].fRATE );
					cFILE.getflag( szOPT_SE, 2, 3, pSET->pSECT[k].fHIGH );
					cFILE.getflag( szOPT_SE, 3, 3, pSET->pSECT[k].fLOW );

					//strSection.freeze( false );
				}
			}
		}

		m_sRandom.push_back( sDATA );
		std::sort( m_sRandom.begin(), m_sRandom.end(), SRANDOM_DATA_OPER() );

		return sDATA.pSET;
	}
	#undef USAGE_RANDOM_OPT_SE
#else
	SRANDOM_GEN* GLItemMan::LoadRandomGenData ( const char* szName )
	{
		if( !szName )
			return FALSE;
		if( strlen(szName) == 0 )
			return FALSE;

		VEC_RANDOM::iterator found = std::lower_bound( m_sRandom.begin(), m_sRandom.end(), szName, SRANDOM_DATA_OPER() );
		if( found != m_sRandom.end() && !strcmp( (*found).szNAME,szName ) )
			return (*found).pSET;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szName;

		SRANDOM_DATA sDATA;
		StringCchCopy( sDATA.szNAME, SRANDOM_DATA::NAME_LEN, szName );
		sDATA.pSET = new SRANDOM_GEN;
		SRANDOM_GEN &sGEN = *sDATA.pSET;

		gltexfile cFILE;
		cFILE.reg_sep( '\t' );
		cFILE.reg_sep( ' ' );
		cFILE.reg_sep( ',' );

		if( GLOGIC::bGLOGIC_ZIPFILE )
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

		if( !cFILE.open( strPath, true ) )
		{
			CDebugSet::ToLogFile( "ERROR : GLItemMan::LoadRandomGenData(), %s", szName );
			SAFE_DELETE( sDATA.pSET );
			return NULL;
		}

		cFILE.getflag( "fRATE", 1, 1, sGEN.fRATE );
		cFILE.getflag( "dwSET_NUM", 1, 1, sGEN.dwSET_NUM );

		TCHAR szTemp[64] = {0};

		for( DWORD i=0; i<sGEN.dwSET_NUM && i<SRANDOM_GEN::MAX_SET; ++i )
		{
			SRANDOM_OPT_SET &sSET = sGEN.sSET[i];

			//std::strstream strSTREAM;
			//strSTREAM << "OPTION_" << i+1 << std::ends;

			_snprintf( szTemp, 64, "OPTION_%u", i+1 );

			cFILE.getflag( szTemp, 1, 4, sSET.fRATE );
			cFILE.getflag( szTemp, 2, 4, sSET.wTYPE );
			cFILE.getflag( szTemp, 3, 4, sSET.fHIGH );
			cFILE.getflag( szTemp, 4, 4, sSET.fLOW );

			//strSTREAM.freeze( false );
		}

		m_sRandom.push_back( sDATA );
		std::sort( m_sRandom.begin(), m_sRandom.end(), SRANDOM_DATA_OPER() );

		return sDATA.pSET;
	}
#endif

SNATIVEID GLItemMan::GetRandomQItemID ( SNATIVEID sNativeID )
{
	int nMax(0);
	if ( GLGaeaServer::GetInstance().m_nServiceProvider != SP_KOREA ) nMax = 5;
	else															 nMax = MAX_NUM;

	// 막아놓은 번호와 일치하면 
	for ( int idx = 0; idx < nMax; idx++ )
	{
		if ( sNativeID == m_sQITEMID[idx] )
		{
			srand(unsigned int(time(NULL)));
			WORD idx = (WORD)(rand() % MAX_NUM);
			return m_sQITEMID[idx];
		}
	}

	return NATIVEID_NULL();
}
