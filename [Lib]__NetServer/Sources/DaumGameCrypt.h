#if _MSC_VER >= 1000
#pragma once
#endif

#ifndef _CDAUMGAMECRYPT_H_
#define _CDAUMGAMECRYPT_H_

#include <tchar.h>

#ifdef _WIN32

#if defined ( KR_PARAM ) || defined ( KRT_PARAM )
	#import "DaumCrypt.DLL" rename_namespace("DAUM"), named_guids
#endif

const IID IID_IEncrypt =	{0x40E692F5,0xCC5D,0x4609,{0x94,0x8D,0x09,0x88,0x12,0x5F,0xF8,0xB4}};
const IID IID_IDecrypt =	{0xBA5723E0,0x7F1C,0x4418,{0x94,0x91,0x21,0x19,0xEF,0x75,0xEB,0x83}};
const CLSID CLSID_Encrypt = {0x285659A3,0x6F2B,0x4036,{0x94,0xAC,0x53,0x4E,0x4D,0x23,0x88,0x86}};
const CLSID CLSID_Decrypt = {0x9AC5B738,0x6E08,0x4148,{0xB3,0x37,0x63,0xCE,0xC5,0x09,0x0C,0x49}};

MIDL_INTERFACE("40E692F5-CC5D-4609-948D-0988125FF8B4")
IEncrypt : public IDispatch
{
public:
    virtual /* [helpstring][id] */			HRESULT STDMETHODCALLTYPE Init(BSTR key) = 0;
    virtual /* [helpstring][id][propget] */	HRESULT STDMETHODCALLTYPE get_Input(/* [retval][out] */ BSTR *pVal) = 0;
    virtual /* [helpstring][id][propput] */	HRESULT STDMETHODCALLTYPE put_Input(/* [in] */ BSTR newVal) = 0;
    virtual /* [helpstring][id][propget] */	HRESULT STDMETHODCALLTYPE get_Output(/* [retval][out] */ BSTR *pVal) = 0;
};

MIDL_INTERFACE("BA5723E0-7F1C-4418-9491-2119EF75EB83")
IDecrypt : public IDispatch
{
public:
    virtual /* [helpstring][id] */			HRESULT STDMETHODCALLTYPE Init(BSTR key) = 0;
    virtual /* [helpstring][id][propget] */	HRESULT STDMETHODCALLTYPE get_Input(/* [retval][out] */ BSTR *pVal) = 0;
    virtual /* [helpstring][id][propput] */	HRESULT STDMETHODCALLTYPE put_Input(/* [in] */ BSTR newVal) = 0;
    virtual /* [helpstring][id][propget] */	HRESULT STDMETHODCALLTYPE get_Output(/* [retval][out] */ BSTR *pVal) = 0;
};

#else

#include <daumencrypt.h>

#endif

class CDaumGameCrypt
{
public:
	CDaumGameCrypt( void );
	~CDaumGameCrypt( void );

public:
	BOOL	Init( void );
	void	Destroy( void );

	BOOL	SetKey( LPCTSTR szKey );

	BOOL	Encrypt( IN LPCTSTR szText, OUT LPTSTR szEncryptText, int nMaxSize );
	BOOL	Decrypt( IN LPCTSTR szEncryptedText, OUT LPTSTR szText, int nMaxSize );

protected:
	BOOL				m_bInit;
#if defined ( KR_PARAM ) || defined ( KRT_PARAM )
	#ifdef _WIN32
		DAUM::IEncryptPtr	m_pEncrypt;
		DAUM::IDecryptPtr	m_pDecrypt;
	#else
		daum_encrypt_type*	m_pCryptor;
	#endif
#endif
};

#endif /* _CDAUMGAMECRYPT_H_ */

