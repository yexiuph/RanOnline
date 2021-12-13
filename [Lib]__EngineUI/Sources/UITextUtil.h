//	BasicTextBox 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.11.19]
//			@ 최초제작
//			@ RanClientLib/UITextControl에서 engineLIb으로 옮김
//			engineLib에서도 문자열을 정리하는 펑션이 필요해서,
//			이곳이 더 적절하다고 판단.
//			기초 펑션은 그대로 RanClientLib에 위치시킴

#pragma	once

class	CD3DFontPar;

namespace	NS_UITEXTUTIL
{
	const DWORD LIGHTGREY	= D3DCOLOR_ARGB(0xFF,0xD3,0xD3,0xD3);
	const DWORD WHITESMOKE	= D3DCOLOR_ARGB(0xFF,0xF5,0xF5,0xF5);
	const DWORD WHITE		= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	const DWORD BLACK		= D3DCOLOR_ARGB(0xFF,0x00,0x00,0x00);

	const DWORD DISABLE		= D3DCOLOR_ARGB(0xFF,0xFF,0x00,0x00);
	const DWORD ENABLE		= LIGHTGREY;

	//	윈도우 타이틀 텍스트 칼라
	//const DWORD WINDOWTITLE = D3DCOLOR_ARGB(255,241,222,164);
	const DWORD WINDOWTITLE = WHITESMOKE;

	//	Function	
	BOOL	IsDBCS ( const char c );

	struct	SSTRING
	{
		CString	strLine;
		bool	bLASTLINE;
	};
	typedef std::vector<SSTRING>	MULTILINETEXT;
	typedef	std::vector<CString>	STRINGVECTOR;
	
	//	개행문자 제거후 문자열 잘라서 들고오기
	//
	void	ClearCRLFText ();
	MULTILINETEXT&	GetMLTextWithoutCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont );

	// MEMO : 멀티라인 에디터 때문에 추가한 함수
	MULTILINETEXT&	GetMLTextWithCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont );

	void	ClearWordText ();
	STRINGVECTOR&	GetWordText ( const CString& strOrigin );

	CString MAKE_MONEY_FORMAT ( const LONGLONG lnMoney, const int nUNIT, const CString& strUNIT_SYMBOL );
};