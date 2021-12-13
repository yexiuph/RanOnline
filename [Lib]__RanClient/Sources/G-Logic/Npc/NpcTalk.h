///////////////////////////////////////////////////////////////////////
//	SNpcTalk
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include <string>
#include "basestream.h"

#define MAX_NEEDITEM_COUNT 5
#define MAX_QUEST_START	5
#define MAX_RANDOM_PAGE	5

class CSerialFile;
struct SNpcTalkCondition;
struct GLCHARLOGIC;

struct SNpcTalk
{
public:
	enum { VERSION = 0x0007, };

	enum EM_TALK
	{
		EM_DO_NOTHING = 0,
		EM_PAGE_MOVE,		//	��ȭ ����.
		EM_BASIC,			//	�⺻ ���. ( â��, ����, ġ��, ������ġ, stats����, �����۱�ȯ )
		EM_QUEST_START,		//	����Ʈ ����.
		EM_QUEST_STEP,		//	����Ʈ ����.

		EM_TALK_SIZE
	};

	enum EM_BASIC
	{
		EM_STORAGE = 0,		//	â��.
		EM_MARKET,			//	����.
		EM_CURE,			//	ġ��.
		EM_STARTPOINT,		//	������ġ ����.
		EM_CHAR_RESET,		//	stats, skill ����.
		EM_ITEM_TRADE,		//	item ��ȯ.
		EM_BUSSTATION,		//	���� ������.

		EM_CLUB_NEW,		//	Ŭ�� ����.
		EM_CLUB_UP,			//	Ŭ�� ��ũ ��.

		EM_CD_CERTIFY,		//	�õ� �����ϱ�.
		EM_COMMISSION,		//	��� ������ ����.
		EM_CLUB_STORAGE,	//	Ŭ�� â��.

		EM_ITEM_REBUILD,	//	������ ����.	// ITEMREBUILD_MARK
		EM_ODDEVEN,			//	Ȧ¦����
		EM_RECOVERY_EXP,	//	����ġ ȸ��
		EM_RANDOM_PAGE,		//  ���Ǵ�ȭ
		EM_ITEMSEARCH_PAGE,	//  ������ �˻�
		EM_ATTENDANCE_BOOK,	//	�⼮��
		EM_ITEM_MIX,

		EM_BASIC_SIZE
	};

public:
	SNpcTalk ();
	~SNpcTalk ();

public:
	static	std::string	strACTIONTYPE[EM_TALK_SIZE];
	static	std::string	szBASICDESC[EM_BASIC_SIZE];

	DWORD				m_dwNID;			//	��ȭ ID
	DWORD				m_dwGLOB_ID;		//	��ü ��Ͽ��� ID.

	SNpcTalkCondition*	m_pCondition;		//	�ൿ����.
	std::string			m_strTalk;			//	�⺻ ��ȭ

	int					m_nACTION;			//	�׼�
	DWORD				m_dwACTION_NO;		//	�׼� ������ ����. ( ���� ��ȭ, �⺻ ��� ���� )

    DWORD				m_dwNeedItem[MAX_NEEDITEM_COUNT];
	
	// ������ ����Ʈ ID
	DWORD				m_dwQuestStartID[MAX_QUEST_START];

	DWORD				m_dwRandomPageID[MAX_RANDOM_PAGE];

	DWORD				m_dwRandomTime;

	//	�Ķ��Ÿ 1.	������ ��ȯ A, ������ ����ƮID  , 
	DWORD				m_dwACTION_PARAM1;
	//	�Ķ��Ÿ 2.	������ ��ȯ B,                , ������ ����ƮSTEP, 
	DWORD				m_dwACTION_PARAM2;

	std::string			m_strBusFile;		// ���������̼� ����

public:	
	BOOL SetCondition ( const SNpcTalkCondition &sCondition );
	void ResetCondition ();

	void SetGlobID ( DWORD dwID );

	BOOL DoTEST ( GLCHARLOGIC *pCHARLOGIC, DWORD dwIndex = UINT_MAX );

	const char* GetTalk ()	{ return m_strTalk.c_str(); }
	SNpcTalkCondition* GetCondition () const;

	void SetTalk ( const std::string &strTalk );

	void Init ();

	BOOL SAVE ( CSerialFile& SFile );
	BOOL LOAD ( basestream& SFile );

	BOOL SAVECSV ( std::fstream &SFile );

    SNpcTalk& operator= ( const SNpcTalk& rNpcTalk );

protected:
	BOOL LOAD_0001 ( basestream& SFile );
	BOOL LOAD_0002 ( basestream& SFile );
	BOOL LOAD_0003 ( basestream& SFile );
	BOOL LOAD_0004 ( basestream& SFile );
	BOOL LOAD_0005 ( basestream& SFile );
	BOOL LOAD_0006 ( basestream& SFile );
	BOOL LOAD_0007 ( basestream& SFile );
};
