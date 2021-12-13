///////////////////////////////////////////////////////////////////////
//	CNpcDialogue
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once
#include <vector>

#include "./NpcDialogueCase.h"
#include "./NpcTalkCondition.h"

#include "../[Lib]__Engine/Sources/Common/BaseStream.h"

class CNpcDialogue
{
public:
	enum { VERSION=0x0002, };
	typedef std::vector<CNpcDialogueCase*>	DLGCASE;
	typedef DLGCASE::iterator				DLGCASE_ITER;
	typedef DLGCASE::const_iterator			DLGCASE_CITER;

private:
	DWORD		m_dwNID;
	DLGCASE		m_vecCase;

public:
	CNpcDialogue ();
	virtual	~CNpcDialogue ();

public:
	void Reset ();

public:
	DWORD FindConditionSatisfied ( GLCHARLOGIC *pCHAR );
	DWORD FindNonCondition ();

public:
	DWORD AddCase ( const CNpcDialogueCase &sCase );
	BOOL DelCase ( const CNpcDialogueCase *pCase );
	BOOL DelCase ( DWORD dwIndex );

public:
	DWORD ToUp ( DWORD dwNID );
	DWORD ToDown ( DWORD dwNID );

public:
	const DLGCASE& GetDlgCaseVec () const;
	CNpcDialogueCase* GetDlgCase ( DWORD dwIndex );

public:
	void SetDlgNID ( const DWORD NativeID );
	DWORD GetDlgNID ();

protected:
	BOOL LOAD_0001 ( basestream& SFile );
	BOOL LOAD_0002 ( basestream& SFile );

public:
	BOOL SaveFile ( CSerialFile& SFile );
	BOOL LoadFile ( basestream& SFile );

	BOOL SaveCSVFile ( std::fstream &SFile );

public:
	CNpcDialogue& operator= ( const CNpcDialogue& rNpcDialog );	
};

