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
#include <string>

#include "../[Lib]__Engine/Sources/Common/BaseStream.h"

class CNpcTalkControl;
struct SNpcTalkCondition;
class CSerialFile;

class CNpcDialogueCase
{
public:
	enum { VERSION = 0x0002, };

private:
	SNpcTalkCondition*	m_pCondition;		//	행동조건.
	std::string			m_strBasicTalk;		//	기본대화.
	CNpcTalkControl*	m_pTalkControl;		//	대화분기.

public:
	CNpcDialogueCase ();
	~CNpcDialogueCase ();

public:
	void Reset ();

public:	
	BOOL SetCondition ( const SNpcTalkCondition &sCondition );
	void ResetCondition ();

	void SetBasicTalk ( const std::string &strTalk );

	BOOL SetTalkControl ( const CNpcTalkControl &sTalkControl );
	void ResetTalkControl ();

public:	
	SNpcTalkCondition* GetCondition () const;
	const std::string& GetBasicTalk () const;
	CNpcTalkControl* GetTalkControl () const;

	DWORD GetTalkNum () const;

protected:
	BOOL LOAD_0001 ( basestream& SFile );
	BOOL LOAD_0002 ( basestream& SFile );

public:
	BOOL	SaveFile ( CSerialFile& SFile );
	BOOL	LoadFile ( basestream& SFile );

	void	SaveCSVFile( string &strLine );

public:
	CNpcDialogueCase&	operator= ( const CNpcDialogueCase& rNpcDialogCase );	
};