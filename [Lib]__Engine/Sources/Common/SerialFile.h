// [SerialFile.h]	class CSerialFile
//
//	[�Լ� �߰�] - JDH, 2003.01.16
//
//		ex) GLMobSchedule, DxCameraAni �� ���� ����,�ε�ο� ����.
//
//		BOOL BeginBlock ();			( ���� ������ ���� ���� ũ�� ���Ͽ� ��ŷ. )
//		BOOL EndBlock ();			( ������ �� ���� �������� ��������� ���� ũ�⸦ ��ŷ�ο� ����. )
//		DWORD ReadBlockSize ();		( ����� �� ũ�⸦ ������. )
//
#ifndef SERIALFILE_H_
#define SERIALFILE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "./basestream.h"

class CSerialFile : public basestream
{
protected:
	FILE*		m_FileStream;
	
protected:
	virtual void ReadFileType ();
	virtual void WriteFileType ();

public:
	virtual void SetFileType ( const char *szFileType, DWORD FileID );
	virtual void GetFileType ( char *szFileType, int nStrLen, DWORD &FileID );

	virtual void SetEncode ( bool bencode )		{ m_bencode = bencode; }

	BOOL OpenFile ( const FOPENTYPE _Type, 
					const char *_szFileName, 
					bool bencode=false );
	virtual BOOL IsOpen () { return m_IsFileOpened; }
	virtual void CloseFile ();

	virtual const char* GetFileName() { return m_szFileName; }

public:
	virtual DWORD GetDefaltOffSet () { return m_DefaultOffSet; }	//	�⺻ ������ġ �ɼ��� ������.

public:
	virtual BOOL SetOffSet ( long _OffSet );				//	���� ���� ��ġ�� ����.
	virtual long GetfTell ();								//	���� ������ġ�� ������.

protected:
	BOOL NewFile ( const char* pName );

public:
	virtual BOOL BeginBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 );
	virtual BOOL EndBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 );

	virtual DWORD ReadBlockSize ();

protected:
	virtual size_t read ( void *pbuffer, DWORD dwSize );
	virtual size_t write ( const void* pbuffer, DWORD dwSize );

// ------- �����ϱ�.
public:
	virtual BOOL operator << ( short Value );
	virtual BOOL operator << ( int Value );
	virtual BOOL operator << ( WORD Value );
	virtual BOOL operator << ( DWORD Value );
	virtual BOOL operator << ( UINT Value );

	virtual BOOL operator << ( float Value );
	virtual BOOL operator << ( double Value );

	virtual BOOL operator << ( D3DVECTOR Value );
	virtual BOOL operator << ( D3DCOLORVALUE &Value );
	virtual BOOL operator << ( D3DBLEND &Value );

	virtual BOOL operator << ( BYTE Value );
	virtual BOOL operator << ( char Value );

	virtual BOOL operator << ( bool Value );

	virtual BOOL operator << ( const std::string &str );

	template<typename TYPE>
	BOOL operator << ( const std::vector<TYPE> &vecVALUE );

	virtual BOOL WriteBuffer ( const void *pBuffer, DWORD Size );

// ------- �ҷ�����.
public:
	virtual BOOL operator >> ( short &Value );
	virtual BOOL operator >> ( int &Value );
	virtual BOOL operator >> ( WORD &Value );
	virtual BOOL operator >> ( DWORD &Value );
	virtual BOOL operator >> ( UINT &Value );

	virtual BOOL operator >> ( float &Value );
	virtual BOOL operator >> ( double &Value );

	virtual BOOL operator >> ( D3DVECTOR &Value );
	virtual BOOL operator >> ( D3DCOLORVALUE &Value );
	virtual BOOL operator >> ( D3DBLEND &Value );

	virtual BOOL operator >> ( BYTE &Value );
	virtual BOOL operator >> ( char &Value );
	virtual BOOL operator >> ( std::string &str );

	virtual BOOL operator >> ( bool &Value );

	template<typename TYPE>
	BOOL operator >> ( std::vector<TYPE> &vecVALUE );

	virtual BOOL ReadBuffer ( void* pBuffer, DWORD Size );

public:
	CSerialFile ();
	CSerialFile ( FOPENTYPE _Type, const char *_szFileName, bool bencode=false );
	virtual ~CSerialFile ();
};

template<typename TYPE>
BOOL CSerialFile::operator << ( const std::vector<TYPE> &vecVALUE )
{
	BOOL bOK(FALSE);
	DWORD dwVecSize = (DWORD)vecVALUE.size();
	bOK = operator << (dwVecSize);
	if ( !bOK )					return FALSE;

	if ( vecVALUE.empty() )		return TRUE;
	return WriteBuffer ( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
}

template<typename TYPE>
BOOL CSerialFile::operator >> ( std::vector<TYPE> &vecVALUE )
{
	BOOL bOK(FALSE);
	DWORD dwSize(0);

	vecVALUE.clear();

	bOK = operator >> ( dwSize );
	if ( !bOK )					return FALSE;
	
	if ( dwSize==0 )			return TRUE;

	vecVALUE.reserve(dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		TYPE tVALUE;
		bOK = ReadBuffer ( &tVALUE, DWORD(sizeof(TYPE)) );
		if ( !bOK )				return FALSE;

		vecVALUE.push_back ( tVALUE );
	}

	return TRUE;
}

#endif // SERIALFILE_H_