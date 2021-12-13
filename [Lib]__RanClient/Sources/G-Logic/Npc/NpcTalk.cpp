#include "pch.h"
#include "NpcTalk.h"
#include "NpcTalkCondition.h"
#include "GLQUEST.h"
#include "GLQUESTMAN.h"
#include "GLogicEx.h"
#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string	SNpcTalk::strACTIONTYPE[EM_TALK_SIZE];
//{
//    "선택되지 않음",
//	"페이지 이동",
//	"기본 기능",
//	"퀘스트 시작",
//	"퀘스트 진행",
//};
//
std::string	SNpcTalk::szBASICDESC[EM_BASIC_SIZE];
//{
//    "창고 열기",
//	"상점 열기",
//	"치료",
//	"시작 위치 지정",
//	"stats, skill 초기화",
//	"아이템 교환",
//	"버스 정류장",
//
//	"클럽 생성",
//	"클럽 랭크 업",
//	
//	"시디 인증하기",
//	"상업 수수료",
//	"클럽 창고",
//
//	"아이템 개조"	// ITEMREBUILD_MARK
//};

SNpcTalk::SNpcTalk () :
	m_dwNID(UINT_MAX),
	m_dwGLOB_ID(0),

	m_pCondition(NULL),
	m_nACTION(EM_DO_NOTHING),
	m_dwACTION_NO(UINT_MAX),
	
	m_dwACTION_PARAM1(UINT_MAX),
	m_dwACTION_PARAM2(UINT_MAX),
	m_dwRandomTime ( 0 )
{
	int i;
	for( i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		m_dwNeedItem[i] = UINT_MAX;
	}

	for ( i = 0; i < MAX_QUEST_START; ++i )
	{
		m_dwQuestStartID[i] = UINT_MAX;
	}

	for ( i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		m_dwRandomPageID[i] = UINT_MAX;
	}
}

SNpcTalk::~SNpcTalk ()
{
	ResetCondition ();
}

void SNpcTalk::SetGlobID ( DWORD dwID )
{
	m_dwGLOB_ID = dwID;
}

BOOL SNpcTalk::SAVECSV ( std::fstream &SFile )
{	

//	SFile << (WORD)VERSION <<","; 

//	SFile << m_dwNID <<",";
	// 텍스트 출력
//	SFile << m_strTalk <<",";

//	SFile << m_nACTION <<",";
//	SFile << m_dwACTION_NO <<",";
	
//	SFile << m_dwACTION_PARAM1 <<",";
//	SFile << m_dwACTION_PARAM2 <<",";

//	SFile << m_strBusFile <<",";

//	SFile << BOOL(m_pCondition!=NULL);
//	if ( m_pCondition )		m_pCondition->SAVECSVFILE ( SFile );

	return TRUE;
}


// Need Add New Item Con.
BOOL SNpcTalk::SAVE ( CSerialFile& SFile )
{	
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD)VERSION;

	SFile << m_dwNID;
	SFile << m_strTalk;

	SFile << m_nACTION;
	SFile << m_dwACTION_NO;

	
	BYTE i;
	for( i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		SFile << m_dwNeedItem[i];
	}

	for ( i = 0; i < MAX_QUEST_START; ++i ) 
	{
		SFile << m_dwQuestStartID[i];
	}

	for ( i = 0; i < MAX_RANDOM_PAGE; ++i ) 
	{
		SFile << m_dwRandomPageID[i];
	}

	SFile << m_dwRandomTime;

	SFile << m_dwACTION_PARAM1;
	SFile << m_dwACTION_PARAM2;

	SFile << m_strBusFile;

	SFile << BOOL(m_pCondition!=NULL);
	if ( m_pCondition )		m_pCondition->SAVE ( SFile );

	return TRUE;
}


BOOL SNpcTalk::LOAD_0001 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	return TRUE;
}

BOOL SNpcTalk::LOAD_0002 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	m_dwACTION_PARAM1 = UINT_MAX;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
		m_dwNeedItem[0] = m_dwACTION_PARAM1;

	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;


	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0003 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
		m_dwNeedItem[0] = m_dwACTION_PARAM1;

	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0004 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
		m_dwNeedItem[0] = m_dwACTION_PARAM1;

	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0005 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
	{
		BYTE i;
		for( i = 0; i < MAX_NEEDITEM_COUNT; ++i )
		{
			SFile >> m_dwNeedItem[i];
		}
	}
	else
	{
		SFile >> m_dwACTION_PARAM1;

		if ( m_nACTION == EM_QUEST_START )
		{
			m_dwQuestStartID[0] = m_dwACTION_PARAM1;
		}
	}

	SFile >> m_dwACTION_PARAM2;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0006 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	for ( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		SFile >> m_dwNeedItem[i];
	}

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}
	
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0007 ( basestream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	for ( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		SFile >> m_dwNeedItem[i];
	}

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		SFile >> m_dwRandomPageID[i];
	}

	SFile >> m_dwRandomTime;
	
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pCondition = new SNpcTalkCondition;
		m_pCondition->LOAD ( SFile );
	}

	return TRUE;
}


BOOL SNpcTalk::LOAD ( basestream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer = 0;
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001 ( SFile );	break;
	case 0x0002:	LOAD_0002 ( SFile );	break;
	case 0x0003:	LOAD_0003 ( SFile );	break;
	case 0x0004:	LOAD_0004 ( SFile );	break;
	case 0x0005:	LOAD_0005 ( SFile );	break;
	case 0x0006:	LOAD_0006 ( SFile );	break;
	case 0x0007:	LOAD_0007 ( SFile );	break;

	default:
		MessageBox ( NULL, "SNpcTalk::LOAD() unknown data version.", "ERROR", MB_OK );
		break;
	};

	return TRUE;
}

SNpcTalk&	SNpcTalk::operator= ( const SNpcTalk& rNpcTalk )
{
	m_dwNID = rNpcTalk.m_dwNID;
	m_strTalk = rNpcTalk.m_strTalk;

    m_nACTION = rNpcTalk.m_nACTION;
	m_dwACTION_NO = rNpcTalk.m_dwACTION_NO;
	
	for ( int i = 0 ; i < MAX_NEEDITEM_COUNT; ++i )
		m_dwNeedItem[i]  = rNpcTalk.m_dwNeedItem[i];

	for ( int i = 0 ; i < MAX_QUEST_START; ++i )
		m_dwQuestStartID[i]  = rNpcTalk.m_dwQuestStartID[i];

	for ( int i = 0 ; i < MAX_RANDOM_PAGE; ++i )
		m_dwRandomPageID[i]  = rNpcTalk.m_dwRandomPageID[i];

	m_dwRandomTime = rNpcTalk.m_dwRandomTime;

	m_dwACTION_PARAM1 = rNpcTalk.m_dwACTION_PARAM1;
	m_dwACTION_PARAM2 = rNpcTalk.m_dwACTION_PARAM2;

	m_strBusFile = rNpcTalk.m_strBusFile;

	ResetCondition ();
	if ( rNpcTalk.m_pCondition )
	{
		SetCondition ( *rNpcTalk.m_pCondition );
	}

	return *this;
}

BOOL SNpcTalk::SetCondition ( const SNpcTalkCondition &sCondition )
{
	ResetCondition ();

	m_pCondition = new SNpcTalkCondition;
	*m_pCondition = sCondition;

	return TRUE;
}

void SNpcTalk::ResetCondition ()
{
	SAFE_DELETE ( m_pCondition );
}
void SNpcTalk::SetTalk( const std::string &strTalk )
{
	m_strTalk = strTalk;
}

SNpcTalkCondition*	SNpcTalk::GetCondition () const
{	
	return m_pCondition;
}

void SNpcTalk::Init ()
{
	m_dwNID = UINT_MAX;
	m_strTalk = "";

    m_nACTION = EM_DO_NOTHING;
	m_dwACTION_NO = UINT_MAX;
	m_dwACTION_PARAM1 = UINT_MAX;
	m_dwACTION_PARAM2 = UINT_MAX;

	m_strBusFile = "";

	for ( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
		m_dwNeedItem[i] = UINT_MAX;

	for ( int i = 0; i < MAX_QUEST_START; ++i )
		m_dwQuestStartID[i] = UINT_MAX;

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
		m_dwRandomPageID[i] = UINT_MAX;

	m_dwRandomTime = 0;

}

BOOL SNpcTalk::DoTEST ( GLCHARLOGIC *pCHARLOGIC, DWORD dwIndex )
{
	if ( !pCHARLOGIC )		return FALSE;

	if ( m_pCondition )
	{
		BOOL bCONDITION = m_pCondition->DoTEST ( pCHARLOGIC );
		if ( !bCONDITION )	return FALSE;
	}

	BOOL bOK = FALSE;

	switch ( m_nACTION )
	{
	case EM_QUEST_START:
		{
			if ( dwIndex == UINT_MAX ) 
			{
				for ( int i = 0; i < MAX_QUEST_START; ++i )
				{
					if ( m_dwQuestStartID[i] == UINT_MAX ) continue;

					//	이미 진행중인 퀘스트는 새로이 시작할 수 없다.
					GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwQuestStartID[i]);
					if ( pQUEST_PROG )	continue;

					GLQUESTPROG *pQUEST_END = pCHARLOGIC->m_cQuestPlay.FindEnd(m_dwQuestStartID[i]);
					if ( pQUEST_END )
					{
						GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( m_dwQuestStartID[i] );
						if ( !pQUEST )	continue;

						if ( pQUEST_END->m_bCOMPLETE )
						{
							//	이미 완료한 퀘스트의 경우에는 반복 수행이 가능할때만 가능.
							if ( !pQUEST->IsREPEAT() )	continue;
						}
						else
						{
							//	이미 포기한 퀘스트의 경우에는제시도 수행이 가능할때만 가능.
							if ( !pQUEST->IsAGAIN() )	continue;
						}
					}

					bOK = TRUE;
				}
			}
			else
			{
				if ( m_dwQuestStartID[dwIndex] == UINT_MAX ) return FALSE;

				//	이미 진행중인 퀘스트는 새로이 시작할 수 없다.
				GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwQuestStartID[dwIndex]);
				if ( pQUEST_PROG )	return FALSE;

				GLQUESTPROG *pQUEST_END = pCHARLOGIC->m_cQuestPlay.FindEnd(m_dwQuestStartID[dwIndex]);
				if ( pQUEST_END )
				{
					GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( m_dwQuestStartID[dwIndex] );
					if ( !pQUEST )	return FALSE;

					if ( pQUEST_END->m_bCOMPLETE )
					{
						//	이미 완료한 퀘스트의 경우에는 반복 수행이 가능할때만 가능.
						if ( !pQUEST->IsREPEAT() )	return FALSE;
					}
					else
					{
						//	이미 포기한 퀘스트의 경우에는제시도 수행이 가능할때만 가능.
						if ( !pQUEST->IsAGAIN() )	return FALSE;
					}
				}

				bOK = TRUE;

			}
		}
		break;

	case EM_QUEST_STEP:
		{
			//	진행중인 퀘스트일때만.
			GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwACTION_PARAM1);
			if ( !pQUEST_PROG )	return FALSE;
			
			if ( pQUEST_PROG->m_dwSTEP!=m_dwACTION_PARAM2 )		return FALSE;

			bOK = TRUE;
		}
		break;
	default:
		{
			bOK = TRUE;
		}
		break;
	};
	
	return bOK;
}