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
	SNpcTalkCondition*	m_pCondition;		//	�ൿ����.
	std::string			m_strBasicTalk;		//	�⺻��ȭ.
	CNpcTalkControl*	m_pTalkControl;		//	��ȭ�б�.

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