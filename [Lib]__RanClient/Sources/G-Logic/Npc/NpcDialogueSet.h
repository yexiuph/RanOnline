///////////////////////////////////////////////////////////////////////
//	CNpcDialogue
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include <vector>
#include <map>

#include "basestream.h"

//	CNpcDialogueSet <- CNpcDialogue <- CNpcDialogueCase <- CNpcTalkControl <- SNpcTalk
//
//
class CNpcDialogue;
class CSerialFile;
struct SNpcTalk;

class	CNpcDialogueSet
{
public:
	CNpcDialogueSet ();
	virtual	~CNpcDialogueSet ();

public:
	enum { VERSION = 0x0013 };	// ���� ��ȭ��
	typedef std::map<DWORD,CNpcDialogue*>	DIALOGMAP;
	typedef DIALOGMAP::iterator				DIALOGMAP_IT;

	typedef std::vector<SNpcTalk*>			VECTALK;

	CString m_strFileName;

protected:
	void RegistTalk ( SNpcTalk* pNpcTalk );

	void ReportAction ();
	void AssignTalkGlobID ();

	BOOL LOAD_0001 ( basestream &SFile );
	BOOL LOAD_0002 ( basestream &SFile );
	BOOL LOAD_0003 ( basestream &SFile );
	BOOL LOAD_0004 ( basestream &SFile );
	BOOL LOAD_0005 ( basestream &SFile );
	BOOL LOAD_0006 ( basestream &SFile );
	BOOL LOAD_0007 ( basestream &SFile );	// ITEMREBUILD_MARK
	BOOL LOAD_0008 ( basestream &SFile );	// Ȧ¦����
	BOOL LOAD_0009 ( basestream &SFile );	// ����ġ ȸ��
	BOOL LOAD_0010 ( basestream &SFile );	// ���Ǵ�ȭ
	BOOL LOAD_0011 ( basestream &SFile );	// ������ �˻� �߰�
	BOOL LOAD_0012 ( basestream &SFile );	// �⼮�� ��� �߰�
	BOOL LOAD_0013 ( basestream &SFile );	// ������ ���� �߰�

	static CString m_strPath;

private:
	bool		m_bModify;
	DIALOGMAP	m_mapDialogue;

	bool		m_bStorage; // ���� ��Ŀ
	bool		m_bMarket; // ���� �����Ǹ�
	bool		m_bCure; // ġ��
	bool		m_bStartPoint; // �������� ����
	bool		m_bCharReset; // ���� ��ų �ʱ�ȭ
	bool		m_bBusStation; // ���� �ܸ���
	bool		m_bClubNew; // Ŭ������
	bool		m_bClubRankUp; // Ŭ�� ��ũ ��
	bool		m_bCDCertify; // CD ����
	bool		m_bCommission; // �������� ������ ����
	bool		m_bClubStorage; // Ŭ�� â��
	bool		m_bItemRebuild;	// ������ ����	// ITEMREBUILD_MARK
	bool		m_bOddEven; // Ȧ¦����
	bool		m_bRecoveryExp;	// ����ġ ��Ȱ
	bool		m_bRandomPage;
	bool		m_bItemSearch; // ������ �˻�
	bool		m_bAttendanceBook;	// �⼮��
	bool		m_bItemMix;		// ������ ����

	VECTALK		m_vecTALK;

public:
	DWORD MakeDlgNID ();

	BOOL AddDialogue ( CNpcDialogue* pDialogue );
	void DelDialogue ( DWORD NativeID );

	CNpcDialogue* GetDialogue ( DWORD NativeID );
	CNpcDialogue* GetDialogue2 ( int nDialogue );
	void CNpcDialogueSet::GetDlgText( DWORD dwIndex, std::string& strName );

	void RemoveAllDialogue ();

	SNpcTalk* GetTalk ( DWORD dwGlobID );

	DIALOGMAP* GetDialogueMap ();

	BOOL Save ( const char* szFileName );
	BOOL Load ( const char* szFileName );

	BOOL SimpleEdit ( CWnd* pWnd );

	void DoModified ()		{ m_bModify = true; }
	void ResetModified ()	{ m_bModify = false; }
	bool IsModify ()		{ return m_bModify; }

	bool IsSTORAGE ()		{ return m_bStorage; }
	bool IsMARKET ()		{ return m_bMarket; }
	bool IsCURE ()			{ return m_bCure; }
	bool IsSTARTPOINT ()	{ return m_bStartPoint; }
	bool IsCHARRESET () 	{ return m_bCharReset; }
	bool IsBUSSTATION ()	{ return m_bBusStation; }
	bool IsCLUBNEW ()		{ return m_bClubNew; }
	bool IsCLUBRANKUP ()	{ return m_bClubRankUp; }
	bool IsCDCERTIFY ()		{ return m_bCDCertify; }
	bool IsCOMMISSION ()	{ return m_bCommission; }
	bool IsCLUBSTORAGE ()	{ return m_bClubStorage; }
	bool IsITEMREBUILD()	{ return m_bItemRebuild; }	// ITEMREBUILD_MARK
	bool IsODDEVEN()		{ return m_bOddEven; }
	bool IsRecoveryExp()	{ return m_bRecoveryExp; }
	bool IsRandomPage()		{ return m_bRandomPage; }
	bool IsItemSearch()		{ return m_bItemSearch; }
	bool IsAttendanceBook() { return m_bAttendanceBook; }
	bool IsItemMix()		{ return m_bItemMix; }

	static const char* GetPath ()			{ return m_strPath.GetString(); }
	static void SetPath ( char* szPath )	{ m_strPath = szPath; }

	CNpcDialogueSet& operator= ( CNpcDialogueSet &rvalue );
};