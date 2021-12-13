#pragma once

#include <vector>
#include <GASSERT.h>

#include "./basestream.h"

class CByteStream : public basestream
{
public:
	typedef std::vector<BYTE>			BVECT;
	typedef std::vector<BYTE>::iterator	BVECT_IT;

protected:
	BVECT		m_Buffer;
	DWORD		m_dwIter;

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
	virtual BOOL operator << ( const std::string &str );

	virtual BOOL operator << ( bool Value );

	virtual BOOL operator << ( __int64 Value );

	template<typename TYPE>
	BOOL operator << ( const std::vector<TYPE> &vecVALUE );

	virtual BOOL WriteBuffer ( const void* pBuffer, DWORD Size );

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

	virtual BOOL operator >> ( __int64 &Value );

	template<typename TYPE>
	BOOL operator >> ( std::vector<TYPE> &vecVALUE );

	virtual BOOL ReadBuffer ( void* pBuffer, DWORD Size );

public:
	virtual BOOL SetOffSet ( long _OffSet )
	{
		int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
		if ( _OffSet>sizeBuff )	return FALSE;

		m_dwIter = _OffSet;

		return TRUE;
	};
	
	virtual long GetfTell ()
	{
		return m_dwIter;
	}

public:
	BOOL IsEmpty ()		{ return m_Buffer.empty(); }
	void GetBuffer ( LPBYTE &pBuff, DWORD &dwSize );

	void ResetIter ()	{ m_dwIter=0; }
	void ClearBuffer ()
	{
		m_Buffer.clear(); 
		m_dwIter=0;
	}

public:
	CByteStream ( const LPBYTE pBuff, const DWORD dwSize );

public:
	CByteStream(void);
	virtual ~CByteStream(void);
};


inline CByteStream::CByteStream(void) :
	m_dwIter(0)
{
}

inline CByteStream::CByteStream ( const LPBYTE pBuff, const DWORD dwSize ) :
	m_dwIter(0)
{
	WriteBuffer ( pBuff, dwSize );
}

inline CByteStream::~CByteStream(void)
{
}

inline BOOL CByteStream::operator << ( short Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( int Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( WORD Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( DWORD Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( UINT Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( float Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( double Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( D3DVECTOR Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( D3DCOLORVALUE &Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( D3DBLEND &Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( BYTE Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( char Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( bool Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( __int64 Value )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value) );
	return TRUE;
}

inline BOOL CByteStream::operator << ( const std::string &str )
{
	*this << DWORD(str.length()+1);
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(str.c_str()), LPBYTE(str.c_str()) + sizeof(char)*(str.length()+1) );
	return TRUE;
}

inline BOOL CByteStream::WriteBuffer ( const void* pBuffer, DWORD Size )
{
	m_Buffer.insert ( m_Buffer.end(), LPBYTE(pBuffer), LPBYTE(pBuffer)+Size );
	return TRUE;
}

inline BOOL CByteStream::operator >> ( short &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( int &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( WORD &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( DWORD &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( UINT &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( float &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( double &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( D3DVECTOR &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( D3DCOLORVALUE &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( D3DBLEND &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( BYTE &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( char &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( bool &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( __int64 &Value )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(Value)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );

	m_dwIter += sizeof(Value);

	return TRUE;
}

inline BOOL CByteStream::operator >> ( std::string &str )
{
	DWORD dwSize;
	*this >> dwSize;

	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
	GASSERT ( sizeof(char)*dwSize<=DWORD(sizeBuff) );

	char *szBuffer = new char[dwSize];
	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(char)*dwSize], LPBYTE(szBuffer) );
	str = szBuffer;
	delete[] szBuffer;

	m_dwIter += sizeof(char)*dwSize;

	return TRUE;
}

template<typename TYPE>
BOOL CByteStream::operator << ( const std::vector<TYPE> &vecVALUE )
{
	BOOL bOK(FALSE);
	bOK = operator << ( (DWORD)vecVALUE.size() );
	if ( !bOK )					return FALSE;

	if ( vecVALUE.empty() )		return TRUE;
	return WriteBuffer ( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
}

template<typename TYPE>
BOOL CByteStream::operator >> ( std::vector<TYPE> &vecVALUE )
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

inline BOOL CByteStream::ReadBuffer ( void* pBuffer, DWORD Size )
{
	int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));

	if ( (int)Size > sizeBuff )	return FALSE;	//ERROR
	GASSERT ( int(Size)<=sizeBuff );

	std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+Size], (LPBYTE)pBuffer );

	m_dwIter += Size;

	return TRUE;
}

inline void CByteStream::GetBuffer ( LPBYTE &pBuff, DWORD &dwSize )
{
	pBuff = &m_Buffer[0];
	dwSize = (DWORD) m_Buffer.size();
}