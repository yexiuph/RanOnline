//	BasicTextBox Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ :
//	�α�
//		[2003.11.19]
//			@ ��������
//			@ RanClientLib/UITextControl���� engineLIb���� �ű�
//			engineLib������ ���ڿ��� �����ϴ� ����� �ʿ��ؼ�,
//			�̰��� �� �����ϴٰ� �Ǵ�.
//			���� ����� �״�� RanClientLib�� ��ġ��Ŵ

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

	//	������ Ÿ��Ʋ �ؽ�Ʈ Į��
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
	
	//	���๮�� ������ ���ڿ� �߶� ������
	//
	void	ClearCRLFText ();
	MULTILINETEXT&	GetMLTextWithoutCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont );

	// MEMO : ��Ƽ���� ������ ������ �߰��� �Լ�
	MULTILINETEXT&	GetMLTextWithCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont );

	void	ClearWordText ();
	STRINGVECTOR&	GetWordText ( const CString& strOrigin );

	CString MAKE_MONEY_FORMAT ( const LONGLONG lnMoney, const int nUNIT, const CString& strUNIT_SYMBOL );
};