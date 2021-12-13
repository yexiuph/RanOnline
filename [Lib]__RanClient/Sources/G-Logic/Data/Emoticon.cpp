#include "pch.h"
#include "./Emoticon.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEmoticon::CEmoticon(void)
{
}

CEmoticon::~CEmoticon(void)
{
}

CEmoticon& CEmoticon::GetInstance ()
{
	static CEmoticon Instance;
	return Instance;
}

bool CEmoticon::LOAD ()
{
	std::string strPATH( GLOGIC::GetPath() );
	strPATH += "emoticon.ini";

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	if( !cFILE.open( strPATH, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile( "ERROR : CEmoticon::LOAD()" );
		return false;
	}

	UINT nNumber(0);
	cFILE.getflag( "FILE_INFO", "DataNumber", 0, 1, nNumber );

	CString strSection;
	for( UINT i=0; i<nNumber; ++i )
	{
		strSection.Format( "EMOTICON_%04d", i+1 );

		bool bUsed(false);
		cFILE.getflag( strSection.GetString(), "Used", 0, 1, bUsed );

		if( !bUsed ) continue;

		SEMOTICON sEmoticon;

		cFILE.getflag( strSection.GetString(), "Id", 0, 1, sEmoticon.nID );
		cFILE.getflag( strSection.GetString(), "Motion", 0, 1, sEmoticon.strMotion );

		UINT nSize = cFILE.GetKeyDataSize( strSection.GetString(), "TermList" );

		std::string strTerm;
		for( UINT j=0; j<nSize; ++j )
		{
			cFILE.getflag( strSection.GetString(), "TermList", j, nSize, strTerm );
			sEmoticon.listTerm.push_back( strTerm );
		}

		m_listEmoticon.push_back( sEmoticon );
	}

	return true;
}

int	CEmoticon::GetEmoticonOfMotion(const std::string & strSrc)
{
	std::string strTemp( strSrc.substr( 1 ) );

	LISTEMOTICON_ITER iter = m_listEmoticon.begin();
	LISTEMOTICON_ITER iter_end = m_listEmoticon.end();

	for( ;iter != iter_end ;++iter )
	{
		if( iter->strMotion == strTemp ) return iter->nID;
	}
	
	return UINT_MAX;
}

int	CEmoticon::GetEmoticonOfTerm(const std::string & strSrc)
{
	LISTEMOTICON_ITER iter = m_listEmoticon.begin();
	LISTEMOTICON_ITER iter_end = m_listEmoticon.end();

	for( ; iter != iter_end; ++iter )
	{
		LISTSTRING_ITER iter_in = iter->listTerm.begin();
		LISTSTRING_ITER iter_end_in = iter->listTerm.end();

		for( ; iter_in != iter_end_in; ++iter_in )
		{
			std::string strChat = CString(strSrc.c_str()).MakeLower().GetString();

			std::string::size_type idx = strChat.find( *iter_in );
			if ( idx != std::string::npos )
			{
				return iter->nID;
			}
		}
	}

	return UINT_MAX;
}