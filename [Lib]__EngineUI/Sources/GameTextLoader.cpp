#include "pch.h"
#include "GameTextLoader.h"

#include <algorithm>
#include "StlFunctions.h"

#include "StringFile.h"
#include "STRINGUTILS.h"

#include "RanXMLParser.h"
#include "RANPARAM.h" //../[Lib]__MfcEx/Sources

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace NS_TEXT_LOADER
{
	BOOL bServer = FALSE;

//////////////////////////////////////////////////////////////////////////////////////
//	private
	BOOL Analyze_Sentense ( const CHAR * szFileName, CStringFile& StrFile, CString& strLine,
							CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		//	ī��Ʈ
		CString strLineCount = StrArray.GetAt ( 2 );
		const int LineCount = atoi ( strLineCount.GetString() );
		const CString strCRLF = "\r\n";
		CString strBuffer;
		int i = 0;
		for ( i = 0; i < LineCount && StrFile.GetNextLine(strLine); i++ )
		{
			strBuffer += strLine;

			//	���������� �ƴѰ��,
			//	�ڵ����� ���๮�ڸ� �ٿ��ش�.
			if ( (LineCount - 1) != i ) strBuffer += strCRLF;
		}

		if ( i != LineCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;
		pNewStrArray->Add ( strBuffer );

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags[std::string(StrMark.GetString())] = pNewStrArray;	

		return TRUE;
	}

	BOOL Analyze_Numeric ( const CHAR * szFileName, CStringFile& StrFile, CString& strLine,
							CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		StrFile.GetNextLine(strLine);

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( "," );
		STRUTIL::StringSeparate ( strLine, *pNewStrArray );

		//	ī��Ʈ
		CString strColumnCount = StrArray.GetAt ( 2 );
		const int ColumnCount = atoi ( strColumnCount.GetString() );	

		if ( pNewStrArray->GetCount() != ColumnCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			SAFE_DELETE ( pNewStrArray );
			return FALSE;
		}

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags[std::string(StrMark.GetString())] = pNewStrArray;	

		return TRUE;
	}
								
	BOOL Analyze_Word ( const CHAR * szFileName, CStringFile& StrFile, CString& strLine,
						CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		StrFile.GetNextLine(strLine);

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( "," );
		STRUTIL::StringSeparate ( strLine, *pNewStrArray );

		//	ī��Ʈ
		CString strColumnCount = StrArray.GetAt ( 2 );
		const int ColumnCount = atoi ( strColumnCount.GetString() );	

		if ( pNewStrArray->GetCount() != ColumnCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			SAFE_DELETE ( pNewStrArray );
			return FALSE;
		}

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags[std::string(StrMark.GetString())] = pNewStrArray;	

		return TRUE;
	}

	BOOL IsValid_Syntax ( const CHAR * szFileName, CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		if ( StrArray.GetSize() < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", StrArray.GetAt(0) );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		CString StrMark = StrArray.GetAt ( 1 );

		STRFLAG_IT found;			
		found = mapFlags.find ( std::string(StrMark.GetString()) );

		if ( found == mapFlags.end() )	return TRUE;		
		else
		{
			CString str;
			str.Format ( "Duplicated Keyword : [%s]", StrArray.GetAt(1) );
			MessageBox ( NULL, str, StrArray.GetAt(0), MB_OK );
			return FALSE;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////
//	public
	BOOL LoadText ( const char* szFileName, WORD progVersion, STRFLAG& mapFlags, BOOL bRefact, BOOL bServerExe )
	{
		GASSERT( szFileName );

		bServer = bServerExe;

		if( bRefact )
		{
			CRanXMLParser xmlParser( RANPARAM::strCountry );
			if( !xmlParser.Parse( mapFlags, szFileName ))
				return FALSE;
		}
		else
		{
			CStringFile StrFile;
			if ( !StrFile.Open ( szFileName ) )	return FALSE;		

			STRUTIL::ClearSeparator ();
			STRUTIL::RegisterSeparator ( "\t" );
			STRUTIL::RegisterSeparator ( "," );

			WORD fileVersion = 0x0000;
			CString strLine;
			while ( StrFile.GetNextLine(strLine) )
			{
				CStringArray *pStrArray = new CStringArray;

				STRUTIL::ClearSeparator ();
				STRUTIL::RegisterSeparator ( "\t" );
				STRUTIL::RegisterSeparator ( "," );
				STRUTIL::StringSeparate ( strLine, *pStrArray );

				//	���� ���� üũ
				int nArraySize = 0;
				nArraySize = (int)pStrArray->GetSize ();
				if ( !nArraySize )
				{
					SAFE_DELETE(pStrArray);
					continue;
				}

				//	�ּ� ó�� üũ
				//	���� �����Ѵٸ�, '//'���Ŀ� �����ϴ� �κ� ��� ����
				for ( int i = 0; i < nArraySize; i++ )
				{				
					CString strCheckSlash = pStrArray->GetAt(i);
					if ( strCheckSlash.GetLength() >= 2 && strCheckSlash.GetAt(0)=='/' && strCheckSlash.GetAt(1)=='/' )
					{
						for ( int j = nArraySize - 1; i <= j; j-- )
						{
							pStrArray->RemoveAt (i);
						}
						break;
					}
				}

				//	�ٽ� ���� üũ ( �ּ�ó���Ǿ� �������ٸ�, �޸� ������ ���� )
				nArraySize = (int)pStrArray->GetSize ();
				if ( !nArraySize )
				{
					SAFE_DELETE(pStrArray);
					continue;
				}


				CString StrMark = pStrArray->GetAt(0);
				if ( StrMark == "VERSION" )
				{
					CString strVersion = pStrArray->GetAt ( 1 );
					fileVersion = (WORD) atoi ( strVersion.GetString() );
					SAFE_DELETE(pStrArray);
				}
				else
				{
					if ( IsValid_Syntax ( szFileName, *pStrArray, mapFlags ) )
					{
						//	���� ó��, ������ �ܾ� ó��, �������� �� ó��
						if( StrMark=="NUMERIC" )		Analyze_Numeric( szFileName, StrFile, strLine, *pStrArray, mapFlags );
						else if( StrMark=="WORD" )		Analyze_Word( szFileName, StrFile, strLine, *pStrArray, mapFlags );
						else if( StrMark=="SENTENSE" )	Analyze_Sentense( szFileName, StrFile, strLine, *pStrArray, mapFlags );
					}
					SAFE_DELETE(pStrArray);
				}	
			}

			if ( progVersion < fileVersion )
			{
				//	Note : ��� CStringArray* �� �����ش�.
				ClearText ( mapFlags );			
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL ClearText ( STRFLAG& mapFlags )
	{
		std::for_each ( mapFlags.begin(), mapFlags.end(), std_afunc::DeleteMapObject() );
		return TRUE;
	}

	const CString & GetText ( std::string strKey, int Index, STRFLAG& mapFlags )
	{
		STRFLAG_IT found;
		found = mapFlags.find ( strKey );		
		if ( found == mapFlags.end() )
		{
			if( bServer )
			{
				CDebugSet::ToLogFile ( "[GAME TEXT ERROR]Not exist text keyword : %s", strKey.c_str() );
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Not Exist Text Keyword : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
			}

			static CString strEmpty; // MEMO : ������ ��ȯ�Ǵ� �� ��Ʈ��
			return strEmpty;
		}	

		CStringArray *pStrArray = (*found).second;

		if ( pStrArray->GetCount () <= Index )
		{
			if( bServer )
			{
				CDebugSet::ToLogFile ( "[GAME TEXT ERROR]Overflow index range : %s %d", strKey.c_str(), Index );
				//GASSERT ( 0 && "�ε��� ���� �ʰ�" );
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Overflow Index Range : %s %d", strKey.c_str(), Index );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
			}

			static CString strEmpty; // MEMO : ������ ��ȯ�Ǵ� �� ��Ʈ��
			return strEmpty;
		}
		const CString& str = pStrArray->GetAt ( Index );

//		return pStrArray->GetAt ( Index );
		return str;
	}

	int GetNumber ( std::string strKey, int Index, STRFLAG& mapFlags )
	{
		STRFLAG_IT found;
		found = mapFlags.find ( strKey );		
		if ( found == mapFlags.end() )
		{
			if( bServer )
			{
				CDebugSet::ToLogFile ( "[GAME TEXT ERROR]Not exist text keyword : %s", strKey.c_str() );
				//GASSERT ( 0 && "�������� �ʴ� �ؽ�Ʈ Ű����" );
			}
			else
			{	
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Not Exist Text Keyword : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
			}

			return 0; // MEMO : 0�� ��ȯ�Ǵ°��� ����ǥ�ð� �ɼ� �ִ��� ��.
		}	

		CStringArray *pStrArray = (*found).second;

		if ( pStrArray->GetCount () <= Index )
		{
			if( bServer )
			{
				CDebugSet::ToLogFile ( "[GAME TEXT ERROR]Overflow index range : %s", strKey.c_str() );
				//GASSERT ( 0 && "�ε��� ���� �ʰ�" );
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Overflow Index Range : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
			}

			return 0; // MEMO : 0�� ��ȯ�Ǵ°��� ����ǥ�ð� �ɼ� �ִ��� ��.
		}

		return atoi( pStrArray->GetAt( Index ).GetString() );
	}
};