#pragma once

class CSerialFile;

#define DEF_TEXEFF_DIFFUSE		0x0001
#define DEF_TEXEFF_FLOWUV		0x0002
#define DEF_TEXEFF_ROTATE		0x0004
#define DEF_TEXEFF_SPECULAR		0x0008

#define DEF_TEXEFF_ALL			0x00ff

struct DxTexEffType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxTexEffType *pNextType;

	DxTexEffType() :
		TYPEID(0xffffffff),
		pNextType(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxTexEffType ()
	{
		SAFE_DELETE(pNextType);
	}
};

class DxTexEffBase
{
public:
	virtual DWORD GetTypeID()				{ return 0xffffffff; }
	virtual const char* GetName()			{ return "[0]_정의되지않은_[UnDefined]"; }

	virtual void FrameMove( const float fElapsedTime )	{}	

	virtual void GetProperty( PBYTE &pProp ) { pProp=NULL; }
	virtual void SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp ) {}

	virtual void SavePSF( CSerialFile& SFile ) {}
	virtual void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile ) {}

public:
	DxTexEffBase* m_pNext;

public:
	DxTexEffBase();
	virtual ~DxTexEffBase();
};