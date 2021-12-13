#include "pch.h"

#include "UITextUtil.h"
#include "d3dfont.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon
#include "CHARSET.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_UITEXTUTIL
{
	//////////////////////////////////////////////////////////////////////////
	//	변수 선언
	STRINGVECTOR	InstanceCRLF;		//	개행문자 제거 스트링 벡터
	MULTILINETEXT	InstanceMultiLine;	//	텍스트박스 넓이를 고려한 멀티라인
	STRINGVECTOR	InstanceWORD;		//	문자열을 단어단위로

	//////////////////////////////////////////////////////////////////////////
	//	간단한 함수
	BOOL	IsDBCS ( const char c )
	{
		return CHARSET::IsLeadChar(c);
	}

	CString MAKE_MONEY_FORMAT ( const LONGLONG lnMoney, const int nUNIT, const CString& strUNIT_SYMBOL )
	{
		CString strMoney;
		strMoney.Format ( "%I64d", lnMoney );
		const int nLength = strMoney.GetLength ();

		for ( int i = nLength - nUNIT; 0 < i; i -= nUNIT )
		{
			strMoney.Insert ( i, strUNIT_SYMBOL );
		}

		return strMoney;
	}


    /////////////////////////////////////////////////////////////////////////
	//	멀티라인 메쏘드
	void	ClearMLText ()
	{
		InstanceMultiLine.clear ();
	}
	
	void SplitMLText ( CString& strOrigin, float fWidth, CD3DFontPar* pFont )
	{
		static const char cSPACE = ' ';

		if ( !pFont )
		{
			GASSERT ( pFont && "폰트가 지정되지 않았습니다." );
			return ;
		}

		SIZE Size;
		pFont->GetTextExtent ( strOrigin.GetString(), Size );

		CString strTemp;
		if ( fWidth < Size.cx )
		{
			int strLength = strOrigin.GetLength ();					
			for ( int i = 0; i < strLength; )
			{   				
				BOOL bDBCS = FALSE;				
				if ( (i + 1) < strLength )
				{
					//	한글인가?
					if ( IsDBCS ( strOrigin[i] ) )
					{
						bDBCS = TRUE;
					}
				}

				//	한글이면 하나 더 처리...
				if ( bDBCS )
				{				
					strTemp += strOrigin[i++];
					strTemp += strOrigin[i++];
				}

				bool bIsENGLISH = false;
				//	영문
				if( !bDBCS )
				{
					char c = strOrigin[i];
					if ( isalpha ( c ) ) bIsENGLISH = true;
					strTemp += strOrigin[i];
					i++;
				}

				//	문자열이 범위를 넘어서면 셋에 등록하고,
				//	그 다음부터 계속 진행한다.
				pFont->GetTextExtent ( strTemp.GetString(), Size );
                if ( fWidth < Size.cx )
				{
					//	오직 영문자인가만 확인한다.
					if ( bIsENGLISH )
					{
						int nIndex = strTemp.ReverseFind ( cSPACE );
						//	처음까지 가도 공백을 찾을 수 없는 경우
						//	또는 첫번째 문자가 공백인 경우
						if ( nIndex <= 0 )
						{
							if ( bDBCS )
							{
								GASSERT ( 0 && "들어온다.1" );
								i-=2;
								strTemp = strTemp.Left ( strTemp.GetLength() - 2 );
							}
							else
							{
								i--;
								strTemp = strTemp.Left ( strTemp.GetLength() - 1 );
							}
						}
						else
						{
							int nStrLength = strTemp.GetLength ();
							int nWordSize = nStrLength - nIndex;
							i-= nWordSize;
//							i++;	//	공백제거
							strTemp = strTemp.Left ( nIndex );
						}
					}
					else
					{
						//	영문자를 제외한 나머지.. ( 숫자, DBCS등 처리 )
						if ( bDBCS )
						{
							i-=2;
							strTemp = strTemp.Left ( strTemp.GetLength() - 2 );
						}
						else
						{
							i--;
							strTemp = strTemp.Left ( strTemp.GetLength() - 1 );
						}
					}

					if ( strTemp.GetLength () < 1 )
					{
						CDebugSet::ToLogFile ( "텍스트 출력창이 너무 작게 설정되어 있습니다.[%s]", strOrigin.GetString() );
						return ;
					}

					SSTRING	sSTRING;
					sSTRING.strLine = strTemp;
					sSTRING.bLASTLINE = false;
					InstanceMultiLine.push_back ( sSTRING );
					strTemp.Empty ();
				}
			}

			//	남아있는 문자들 처리
			SSTRING	sSTRING;
			sSTRING.strLine = strTemp;
			sSTRING.bLASTLINE = true;
			InstanceMultiLine.push_back ( sSTRING );
		}
		else
		{
			strTemp = strOrigin;
			SSTRING	sSTRING;
			sSTRING.strLine = strTemp;
			sSTRING.bLASTLINE = true;
			InstanceMultiLine.push_back ( sSTRING );
		}		
	}    

	MULTILINETEXT&	GetMLText ()
	{
		return InstanceMultiLine;
	}
	

	////////////////////////////////////////////////////////////////////////////
	//	개행문자 제거 메쏘드
	void	ClearCRLFText ()
	{
		InstanceCRLF.clear ();
	}

	void	RemoveCRLF ( const CString& strOrigin )
	{
		CString strTemp;
		int strLength = strOrigin.GetLength ();					
		for ( int i = 0; i < strLength; )
		{   				
			BOOL bDBCS = FALSE;
			if ( (i + 1) < strLength )
			{
				//	개행문자인가?
				if ( strOrigin[i] == '\r' && strOrigin[i+1] == '\n' )
				{
					InstanceCRLF.push_back ( strTemp );
					strTemp.Empty ();

					i += 2;

					continue;
				}

				//	한글인가?
				if ( IsDBCS ( strOrigin[i] ) )
				{
					bDBCS = TRUE;
				}
			}

			//	영문
			{
				strTemp += strOrigin[i];
				i++;
			}

			//	한글이면 하나 더 처리...
			if ( bDBCS )
			{				
				strTemp += strOrigin[i];
				i++;
			}
		}

		//	남아있는 문자들 처리
		InstanceCRLF.push_back ( strTemp );
	}

	void	MakeInstanceCRLF ( const CString& strOrigin )
	{
		CString strTemp;
		int strLength = strOrigin.GetLength ();					
		for ( int i = 0; i < strLength; )
		{   				
			BOOL bDBCS = FALSE;
			if ( (i + 1) < strLength )
			{
				//	개행문자인가?
				if ( strOrigin[i] == '\r' && strOrigin[i+1] == '\n' )
				{
					// MEMO : \r\n을 추가해본다.
					strTemp += strOrigin[i];
					strTemp += strOrigin[i+1];
					////////////////////////////

					InstanceCRLF.push_back ( strTemp );
					strTemp.Empty ();

					i += 2;

					continue;
				}

				//	한글인가?
				if ( IsDBCS ( strOrigin[i] ) )
				{
					bDBCS = TRUE;
				}
			}

			//	영문
			{
				strTemp += strOrigin[i];
				i++;
			}

			//	한글이면 하나 더 처리...
			if ( bDBCS )
			{				
				strTemp += strOrigin[i];
				i++;
			}
		}

		//	남아있는 문자들 처리
		InstanceCRLF.push_back ( strTemp );
	}

	STRINGVECTOR&	GetCFLFText ()
	{
		return InstanceCRLF;
	}


	///////////////////////////////////////////////////////////////////////////
	//	일반용
	MULTILINETEXT&	GetMLTextWithoutCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont )
	{
		//	개행문자 제거
		ClearCRLFText ();
		RemoveCRLF ( strOrigin );		
		
		//	멀티라인으로 바꿈
		ClearMLText ();
		if ( InstanceCRLF.size() )
		{
			for ( MULTILINETEXT::size_type i = 0; i < InstanceCRLF.size(); ++i )
			{				
				SplitMLText ( InstanceCRLF[i], fWidth, pFont );
			}
		}

		return GetMLText ();
	}

	MULTILINETEXT&	GetMLTextWithCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont )
	{
		GASSERT( pFont );

		//	개행문자 제거
		ClearCRLFText ();
		MakeInstanceCRLF ( strOrigin );		

		//	멀티라인으로 바꿈
		ClearMLText ();
		if ( InstanceCRLF.size() )
		{
			for ( MULTILINETEXT::size_type i = 0; i < InstanceCRLF.size(); ++i )
			{				
				SplitMLText ( InstanceCRLF[i], fWidth, pFont );
			}
		}

		return GetMLText ();
	}

	/////////////////////////////////////////////////////////////////////////////
	//	단어단위로 자르기
	void	ClearWordText ()
	{
		InstanceWORD.clear ();
	}

	STRINGVECTOR&	GetWordText ( const CString& strOrigin )
	{
		CString strTemp;
		int strLength = strOrigin.GetLength ();

		for ( int i = 0; i < strLength; )
		{   				
			BOOL bDBCS = FALSE;
			if ( (i + 1) < strLength )
			{				
				if ( strOrigin[i] == ' ' )
				{
					InstanceWORD.push_back ( strTemp );
					strTemp.Empty ();

					i++;

					continue;
				}

				//	한글인가?
				if ( IsDBCS ( strOrigin[i] ) )
				{
					bDBCS = TRUE;
				}
			}

			//	영문
			{
				strTemp += strOrigin[i];
				i++;
			}

			//	한글이면 하나 더 처리...
			if ( bDBCS )
			{				
				strTemp += strOrigin[i];
				i++;
			}
		}

		if ( strTemp.GetLength() && strTemp[strTemp.GetLength()-1] == ' ' )
		{
			strTemp = strTemp.Left ( strTemp.GetLength () - 1 );

			{
				InstanceWORD.push_back ( strTemp );
			}

			strTemp.Empty ();

			{
				InstanceWORD.push_back ( strTemp );
			}
		}
		else
		{
			//	남아있는 문자들 처리
			InstanceWORD.push_back ( strTemp );
		}

		return InstanceWORD;
	}
};