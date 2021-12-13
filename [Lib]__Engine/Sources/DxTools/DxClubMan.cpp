#include "pch.h"
#include "./StlFunctions.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "SerialFile.h"
#include "./gltexfile.h"
#include "./TextureManager.h"

#include "DxClubMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <fcntl.h>

DxClubMan& DxClubMan::GetInstance()
{
	static DxClubMan Instance;
	return Instance;
}

const	DWORD	DxClubMan::VERSION		= 103;

const	DWORD	DxClubMan::CHAR_NUM		= 100;
const	DWORD	DxClubMan::FULL_WIDTH	= 256;
const	DWORD	DxClubMan::FULL_HEIGHT	= 256;

const	std::string	DxClubMan::m_strText = "Club_Tex.dat";
static const std::string strDEFAULTMARK = "Club_NoMark.bmp";
char	DxClubMan::m_szPath[MAX_PATH]	= "";

DxClubMan::DXTEXDATA::~DXTEXDATA ()
{
	TextureManager::ReleaseTexture( szName.c_str(), pTexture );
};

//-----------------------------------------------------------------------------------
//
DxClubMan::DxClubMan () :
	COL(0),
	ROW(0),
	UV_DIS(0.f,0.f)
{
	m_strBaseName = "Club_";
	m_strExtName = ".dds";			
//	CUR_TEXTURE = m_strBaseName + "01" + "_" + "00" + m_strExtName;		// 서버 번호, 텍스쳐 순서
}

DxClubMan::~DxClubMan ()
{
	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();
}

DxClubMan::ID_SERVER::ID_SERVER() :
	COUNT_NUM(0)
{
}

DxClubMan::ID_SERVER::~ID_SERVER()
{
	std::for_each ( mapData.begin(), mapData.end(), std_afunc::DeleteMapObject() );
	mapData.clear();

	std::for_each ( mapTex.begin(), mapTex.end(), std_afunc::DeleteMapObject() );
	mapTex.clear();
}


HRESULT DxClubMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD	dwWidth = WIDTH+1;
	DWORD	dwHeight = HEIGHT+1;

	COL = FULL_WIDTH/dwWidth;
	ROW = FULL_HEIGHT/dwHeight;

	UV_DIS = D3DXVECTOR2 ( (float)(WIDTH)/FULL_WIDTH, (float)(HEIGHT)/FULL_HEIGHT );



	LoadData ( pd3dDevice );	// 로딩

	//// 테스트													TEST
	//LPDWORD	pClubMark = NULL;//[WIDTH*HEIGHT];

	//for ( DWORD i=0; i<100; ++i )
	//{
	//	if ( SetClubMark ( "1.bmp", pClubMark ) )		// 성공
	//	{
	//		// 서버로 sClubMark 데이터를 보내겠지.
	//	}
	//	// sClubMark 서버에서 받는다.
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+0, 100, pClubMark );		// 이 문장은 처음 봐서 만드는 거야

	//	SetClubMark ( "2.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+1, 100, pClubMark );

	//	SetClubMark ( "3.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+2, 100, pClubMark );

	//	SetClubMark ( "4.bmp", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+3, 100, pClubMark );

	//	SetClubMark ( "4.tga", pClubMark );
	//	if ( pClubMark )	NewClubData ( pd3dDevice, (i*5)+4, 100, pClubMark );
	//}

	//SaveData();

	/*
	if ( IsValidData ( 1, 0, 0 ) )
	{
	}

	LPDWORD	pClubMark = NULL;//[WIDTH*HEIGHT];

	for ( DWORD i=0; i<1000; ++i )
	{
		GetClubData ( pd3dDevice, 1, i, 1 );
	}

	for ( DWORD i=0; i<1000; ++i )
	{
		GetClubData ( pd3dDevice, 2, i, 1 );
	}
	*/

	return S_OK;
}

HRESULT DxClubMan::DeleteDeviceObjects()
{
	return S_OK;
}

HRESULT DxClubMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )		// 테스트용 코드. ^^
{
	//struct	VERTEX_RHW
	//{
	//	D3DXVECTOR4	vPos;
	//	D3DXVECTOR2 vTex;
	//};

	//VERTEX_RHW	sVerties[4];

	//sVerties[0].vPos = D3DXVECTOR4 ( 0.f,	0.f, 1.f, 1.f );
	//sVerties[1].vPos = D3DXVECTOR4 ( 16.f,	0.f, 1.f, 1.f );
	//sVerties[2].vPos = D3DXVECTOR4 ( 0.f,	11.f, 1.f, 1.f );
	//sVerties[3].vPos = D3DXVECTOR4 ( 16.f,	11.f, 1.f, 1.f );

	//DXDATA sData = GetClubData ( pd3dDevice, 0, 0, 100 );

	//sVerties[0].vTex = sData.vTex_1_LU;
	//sVerties[1].vTex = sData.vTex_2_RU;
	//sVerties[2].vTex = sData.vTex_3_LD;
	//sVerties[3].vTex = sData.vTex_4_RD;

	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState ( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );

	//pd3dDevice->SetFVF ( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	//pd3dDevice->SetTexture ( 0, sData.pddsTexture );
	//pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sVerties, sizeof(VERTEX_RHW) );

	return S_OK;
}

//	---------------------------------------------------------------------------------------------------------------------
//											DATA			GET		&	SETTING
//	---------------------------------------------------------------------------------------------------------------------

BOOL DxClubMan::LoadClubMark ( const char* cName, LPDWORD& pColor )
{
	// 파일이 있는지 찾기
	std::string strPathname;

	if ( strDEFAULTMARK==cName )
	{
		strPathname = m_szPath;
		strPathname += cName;
	}
	else
	{
		strPathname = m_strREGISTPATH;
		strPathname += cName;
	}

	FILE* file = NULL;
	fopen_s(&file, strPathname.c_str(), "rb");
	if ( !file )	
	{
		return FALSE;						// 파일이 없다.
	}
	fclose(file);

	//	BMP, TGA 인지 찾기
	BOOL	bBMP = TRUE;
	std::string	strSrcName = cName;
	std::transform ( strSrcName.begin(), strSrcName.end(), strSrcName.begin(), tolower );
	std::string	strExtName;
	std::string::size_type idx = strSrcName.find('.');

	if ( idx == std::string::npos )		
	{
		return FALSE;		//	파일이름이 마침표를 가지고 있지 않다.
	}
	else
	{
		//	파일이름을 확장자를 가져온다.
		strExtName = strSrcName.substr ( idx+1 );
		idx = strExtName.find("bmp");
		if ( idx == std::string::npos )						// bmp 아니다.
		{
			bBMP = FALSE;
			idx = strExtName.find("tga");
			if ( idx == std::string::npos )
			{
				return FALSE;	// bmp 도 tga 도 아니다.
			}
		}
	}

	// 데이터 가져오기
	if ( bBMP )
	{
		if ( !LoadBMPFile ( strPathname.c_str(), WIDTH, HEIGHT, m_sClubMark ) )	
		{
			return FALSE;
		}
	}
	else
	{
		if ( !LoadTGAFile ( strPathname.c_str(), WIDTH, HEIGHT, m_sClubMark ) )	
		{
			return FALSE;
		}
	}

	pColor = m_sClubMark;	// !!!!

	return TRUE;
}

bool DxClubMan::IsValidData ( int nServer, int nID, DWORD dwVer )
{
	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);
	if ( iter_s == m_mapServer.end() )		return false;

	ID_DATA_MAP&		mapData = (*iter_s).second->mapData;
	ID_DATA_MAP_ITER	iter	= mapData.find(nID);
	if ( iter == mapData.end() )			return false;	// ID 가 없다.
	if ( (*iter).second->dwVer != dwVer )	return false;	// 버젼이 틀리다.

	return true;
}

bool DxClubMan::SetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer, DWORD *pClubMark )
{
	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);

	if ( iter_s==m_mapServer.end() )
	{
		NewServerData ( nServer );
		iter_s = m_mapServer.find(nServer);
	}
	if ( iter_s==m_mapServer.end() )	return false;

	ID_DATA_MAP& mapData	= (*iter_s).second->mapData;
	TEXDATA_MAP& mapTex		= (*iter_s).second->mapTex;
	ID_DATA_MAP_ITER iter	= mapData.find(nID);

	if ( iter != mapData.end() )		// 이러면 있는 거다.
	{
		if ( (*iter).second->dwVer != dwVer )			// 버젼 틀리다.
		{
			ModifyClubData ( pd3dDevice, iter_s, nID, dwVer, pClubMark );	// 버젼이 틀리다. 이 위치에다 그림을 넣는다.
			SaveData();														// 수정된 버젼 저장
		}
	}
	else	// 현재 ID 정보가 없다.
	{
		NewClubData ( pd3dDevice, iter_s, nID, dwVer, pClubMark );	// 새로운 값을 넣는다.
		SaveData();													// 새로 만들었으니 저장
	}

	return true;
}

DxClubMan::DXDATA DxClubMan::GetClubData ( LPDIRECT3DDEVICEQ pd3dDevice, int nServer, int nID, DWORD dwVer )
{
	// 클럽마크가 있다면 사용하고
	// 버젼이 틀리거나 없다면 기록을 한다.
	std::string sName;
	D3DXVECTOR2 vTex(0.f,0.f);

	TEXDATA_MAP mapTex;
	ID_SERVER_MAP_ITER iter_s = m_mapServer.find(nServer);

	if ( iter_s != m_mapServer.end() )
	{
		ID_DATA_MAP& mapData = (*iter_s).second->mapData;
		mapTex = (*iter_s).second->mapTex;

		ID_DATA_MAP_ITER iter = mapData.find(nID);

		if ( iter != mapData.end() )		// 이러면 있는 거다.
		{
			sName = (*iter).second->strTex;
			vTex = SetTexUV ( (*iter).second->dwCount );
		}
		else								// ID 가 없다.
		{
			LPDWORD	pClubMark = NULL;
			
			// 디폴트 이미지를 사용
			BOOL bOK = LoadClubMark ( strDEFAULTMARK.c_str(), pClubMark );
			if ( bOK )
			{
				NewClubData ( pd3dDevice, iter_s, nID, 0, pClubMark );	// 새로운 값을 넣는다.
				SaveData();												// 새로 만들었으니 저장

				iter = mapData.find(nID);
				if ( iter != mapData.end() )
				{
					sName = (*iter).second->strTex;
					vTex = SetTexUV ( (*iter).second->dwCount );
				}
			}
		}
	}
	else		// 서버정보 자체가 없다.
	{
		NewServerData ( nServer );

		iter_s = m_mapServer.find(nServer);
		if ( iter_s != m_mapServer.end() )
		{
			ID_DATA_MAP& mapData = (*iter_s).second->mapData;
			mapTex = (*iter_s).second->mapTex;

			LPDWORD	pClubMark = NULL;
			
			// 디폴트 이미지를 사용
			BOOL bOK = LoadClubMark ( strDEFAULTMARK.c_str(), pClubMark );
			if ( bOK )
			{
				NewClubData ( pd3dDevice, iter_s, nID, 0, pClubMark );	// 새로운 값을 넣는다.
				SaveData();												// 새로 만들었으니 저장

				ID_DATA_MAP_ITER iter = mapData.find(nID);
				if ( iter != mapData.end() )
				{
					sName = (*iter).second->strTex;
					vTex = SetTexUV ( (*iter).second->dwCount );
				}
			}
		}
	}

	//	마지막 데이터 !!!!!!! END 닷.
	TEXDATA_MAP_ITER _iter = mapTex.find ( sName.c_str() );
	if ( _iter != mapTex.end() )
	{
		(*_iter).second->szName = sName.c_str();
		m_sTextureData.pddsTexture = (*_iter).second->pTexture;
	}
	else
	{
		m_sTextureData.pddsTexture = NULL;
	}

	m_sTextureData.vTex_1_LU = vTex;
	m_sTextureData.vTex_2_RU = vTex;
	m_sTextureData.vTex_3_LD = vTex;
	m_sTextureData.vTex_4_RD = vTex;

	m_sTextureData.vTex_2_RU.x += UV_DIS.x;
	m_sTextureData.vTex_3_LD.y += UV_DIS.y;
	m_sTextureData.vTex_4_RD.x += UV_DIS.x;
	m_sTextureData.vTex_4_RD.y += UV_DIS.y;

	return m_sTextureData;
}

void DxClubMan::NewServerData ( int nServer )
{
	ID_SERVER_MAP_ITER iter = m_mapServer.find(nServer);
	if ( iter != m_mapServer.end() )	return;

	ID_SERVER* pServer = new ID_SERVER;
	m_mapServer.insert ( std::make_pair(nServer,pServer) );
}

void DxClubMan::NewClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor )
{
	ID_DATA_MAP& mapData = (*_iter).second->mapData;
	TEXDATA_MAP& mapTex = (*_iter).second->mapTex;

	ID_DATA_MAP_ITER iter = mapData.find(nID);	// TEST 용
	if ( iter != mapData.end() )	return;		// TEST 용

	DWORD		COUNT_NUM	= (*_iter).second->COUNT_NUM;
	std::string CUR_TEXTURE = (*_iter).second->CUR_TEXTURE;

	if ( (COUNT_NUM>=(COL*ROW)) || !mapTex.size() )	// 텍스쳐 수용 범위를 넘어섰다. || 텍스쳐가 하나도 없다.
	{
		COUNT_NUM = 0;

		int nSize = (*_iter).first;
		int n10 = nSize/10;
		int n1 = nSize-(n10*10);
		char c10 = (char)(n10+48);
		char c1 = (char)(n1+48);

		CUR_TEXTURE = m_strBaseName + c10 + c1 + "_";			// 서버..

		nSize = mapTex.size();
		n10 = nSize/10;
		n1 = nSize-(n10*10);
		c10 = (char)(n10+48);
		c1 = (char)(n1+48);

		CUR_TEXTURE = CUR_TEXTURE + c10 + c1 + m_strExtName;	// 텍스쳐 갯수 . ?

		// 기본 텍스쳐를 만든다.
		LPDIRECT3DTEXTUREQ pTexture;
		D3DXCreateTexture ( pd3dDevice, FULL_WIDTH, FULL_HEIGHT, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture );

		std::string strPathname = m_szPath;
		strPathname += CUR_TEXTURE.c_str();
		D3DXSaveTextureToFile ( strPathname.c_str(), D3DXIFF_DDS, pTexture, NULL );
		TextureManager::ReScanTexture ();

		SAFE_RELEASE ( pTexture );

		//	이제 새로운 텍스쳐를 등록한다.
		DXTEXDATA* pData = new DXTEXDATA;
		mapTex.insert( std::make_pair(CUR_TEXTURE.c_str(),pData) );

		if ( !LoadTex ( pd3dDevice ) )	return;		// 텍스쳐를 다시 re로드 한다.
	}

	ID_DATA* pData = new ID_DATA;

	pData->dwVer = dwVer;
	pData->dwCount = COUNT_NUM;
	pData->strTex = CUR_TEXTURE;

	mapData.insert ( std::make_pair(nID,pData) );

	// 그림을 지정된 위치에 넣어준다.
	DrawClubTex ( pd3dDevice, mapTex, pData->strTex.c_str(), pData->dwCount, pColor );

	++COUNT_NUM;

	(*_iter).second->COUNT_NUM		= COUNT_NUM;	// 값 넣기
	(*_iter).second->CUR_TEXTURE	= CUR_TEXTURE;	// 값 넣기
}

void DxClubMan::ModifyClubData ( LPDIRECT3DDEVICEQ pd3dDevice, ID_SERVER_MAP_ITER& _iter, int nID, DWORD dwVer, DWORD* pColor )
{
	ID_DATA_MAP& mapData	= (*_iter).second->mapData;
	TEXDATA_MAP& mapTex		= (*_iter).second->mapTex;

	ID_DATA_MAP_ITER iter = mapData.find ( nID );

	if ( iter != mapData.end() )
	{
		(*iter).second->dwVer = dwVer;
		DrawClubTex ( pd3dDevice, mapTex, (*iter).second->strTex.c_str(), (*iter).second->dwCount, pColor );	// 그림을 지정된 위치에 넣어준다.
	}
}

void DxClubMan::DrawClubTex ( LPDIRECT3DDEVICEQ pd3dDevice, TEXDATA_MAP& mapTex, const char* cName, int nCount, DWORD* pColor )
{
	if ( !pColor )		return;

	TEXDATA_MAP_ITER iter = mapTex.find ( cName );
	if ( iter!=mapTex.end() )
	{
		LPDIRECT3DTEXTUREQ	pTexture;
		pTexture = (*iter).second->pTexture;

		DWORD dwHeight	= nCount/COL;
		DWORD dwWidth	= (nCount-(dwHeight*COL))*(WIDTH+1);
		dwHeight	= (nCount/COL)*(HEIGHT+1);

		DWORD			dwBPP = 32;
		D3DLOCKED_RECT	lrDst;
		RECT			pRect;
		pRect.left		= 0 + dwWidth;
		pRect.top		= 0 + dwHeight; 
		pRect.right		= 15 + dwWidth;
		pRect.bottom	= 10 + dwHeight;
		pTexture->LockRect ( 0, &lrDst, &pRect, 0 );
		DWORD* pDst       = (DWORD*)lrDst.pBits;

		for( DWORD y=0; y<HEIGHT; y++ )
		{
			for( DWORD x=0; x<WIDTH; x++ )
			{
				pDst[x] = pColor[(y*WIDTH)+x];
			}
			pDst = pDst+(lrDst.Pitch/(dwBPP/8));
		}

		pTexture->UnlockRect(0);


		std::string strPathname = m_szPath;
		strPathname += cName;
		D3DXSaveTextureToFile( strPathname.c_str(), D3DXIFF_DDS, pTexture, NULL );

		// Note : Texture 다시 로드
		TextureManager::ReleaseTexture( (*iter).second->szName.c_str(), pTexture );
		(*iter).second->szName = cName;
		HRESULT hr = TextureManager::LoadTexture ( cName, pd3dDevice, (*iter).second->pTexture, 0, 1 );
		if( FAILED( hr ) )		// 텍스쳐가 없다는 소리. 데이터는 이제 소용없다.
		{
			std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
			m_mapServer.clear();
			SaveData();
			return;
		}
	}
}

D3DXVECTOR2	DxClubMan::SetTexUV ( int nCount )
{
	DWORD dwRow = nCount/COL;			// 열
	DWORD dwCol = nCount-(dwRow*COL);	// 행

	float fMicroWidth = 0.25f/FULL_WIDTH;
	float fMicroHeight = 0.25f/FULL_HEIGHT;

	D3DXVECTOR2 vTex ( ((float)(dwCol*(WIDTH+1))/FULL_WIDTH) + fMicroWidth, 
						((float)(dwRow*(HEIGHT+1))/FULL_HEIGHT) + fMicroHeight );

	return vTex;
}

//	---------------------------------------------------------------------------------------------------------------------
//													SAVE	&	LOAD
//	---------------------------------------------------------------------------------------------------------------------

void DxClubMan::ID_SERVER::Save ( CSerialFile &SFile )
{
	DWORD dwSize = mapTex.size();
	SFile << dwSize;

	TEXDATA_MAP_ITER iter = mapTex.begin();
	for ( ; iter!=mapTex.end(); ++iter )
	{
		SFile << (*iter).first;
	}

	dwSize = mapData.size();
	SFile << dwSize;

	{
		ID_DATA_MAP_ITER iter = mapData.begin();

		for ( ; iter!=mapData.end(); ++iter )
		{
			SFile << (*iter).first;

			SFile << (*iter).second->dwVer;
			SFile << (*iter).second->dwCount;
			SFile << (*iter).second->strTex;
		}
	}

	SFile << COUNT_NUM;
	SFile << CUR_TEXTURE;
}

void DxClubMan::ID_SERVER::Load ( CSerialFile &SFile )
{
	std::string strName;

	DWORD dwCount=0;
	DWORD dwCount_Cur=1;

	DXTEXDATA* pTexData = NULL;
	SFile >> dwCount;
	for ( DWORD i=0; i<dwCount; ++i )
	{
		SFile >> strName;

		pTexData = new DXTEXDATA;
		mapTex.insert ( std::make_pair(strName,pTexData) );
	}

	dwCount = 0;
	SFile >> dwCount;

	ID_DATA* pData = NULL;
	DWORD dwID = 0;
	for ( DWORD i=0; i<dwCount; ++i )
	{		
		pData = new ID_DATA;
		SFile >> dwID;
		SFile >> pData->dwVer;
		SFile >> pData->dwCount;
		SFile >> pData->strTex;
		
		mapData.insert ( std::make_pair(dwID,pData) );
	}

	SFile >> COUNT_NUM;
	SFile >> CUR_TEXTURE;
}

void DxClubMan::SaveData()
{
	std::string strPathname;
	strPathname = m_szPath;
	strPathname += m_strText.c_str();

	CSerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, strPathname.c_str() );
	if ( !bOPEN )	return;

	SFile << (DWORD)VERSION;

	DWORD dwSize = m_mapServer.size();
	SFile << dwSize;

	ID_SERVER_MAP_ITER iter_s = m_mapServer.begin();
	for ( ; iter_s!=m_mapServer.end(); ++iter_s )
	{
		SFile << (*iter_s).first;
		(*iter_s).second->Save ( SFile );
	}
}

void DxClubMan::LoadData( LPDIRECT3DDEVICEQ pd3dDevice )
{
	std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
	m_mapServer.clear();

	std::string strPathname;
	strPathname = m_szPath;
	strPathname += m_strText.c_str();

	CSerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_READ, strPathname.c_str() );
	if ( !bOPEN )
	{
		NewData();
		
		SaveData();
		goto _RETURN;
	}

	DWORD dwVer = 0;
	SFile >> dwVer;

	if ( dwVer==VERSION )
	{
		DWORD dwCount = 0;
		SFile >> dwCount;

		DWORD dwServer = 0;
		ID_SERVER* pServer = NULL;
		for ( DWORD i=0; i<dwCount; ++i )
		{			
			SFile >> dwServer;
			pServer = new ID_SERVER;
			pServer->Load ( SFile );

			m_mapServer.insert ( std::make_pair(dwServer,pServer) );
		}
	}
	else
	{
		SFile.CloseFile();

		NewData();

		SaveData();
		goto _RETURN;
	}

_RETURN:
	LoadTex ( pd3dDevice );	// 텍스쳐 로드
}

void DxClubMan::NewData()
{
	std::string strPathname;
	strPathname = m_szPath;
	strPathname += m_strText.c_str();

	int fh;

	_sopen_s(&fh, strPathname.c_str(), _O_CREAT | _O_TRUNC, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if( fh == -1 )
		perror( "Couldn't create data file" );
	else
	{
		printf( "Created data file.\n" );
		_close( fh );
	}

	// 이 이미지를 사용한다고 알린다.
	FILE* file = NULL;
	fopen_s(&file, strPathname.c_str(), "wb");
	if ( !file )	return;

	fwrite ( &VERSION, sizeof(DWORD), 1, file );					// Save

	DWORD dwCount = 0;
	fwrite ( &dwCount, sizeof(DWORD), 1, file );						// Save

	fclose(file);
}

BOOL DxClubMan::LoadTex ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	ID_SERVER_MAP_ITER iter_s = m_mapServer.begin();
	for ( ; iter_s!=m_mapServer.end(); ++iter_s )
	{
		TEXDATA_MAP			mapTex	= (*iter_s).second->mapTex;
		TEXDATA_MAP_ITER	iter	= mapTex.begin();

		for ( ; iter!=mapTex.end(); ++iter )
		{
			TextureManager::ReleaseTexture( (*iter).second->szName.c_str(), (*iter).second->pTexture );

			HRESULT hr = TextureManager::LoadTexture ( (*iter).first.c_str(), pd3dDevice, (*iter).second->pTexture, 0, 1 );
			if( FAILED( hr ) )		// 텍스쳐가 없다는 소리. 데이터는 이제 소용없다.
			{
				std::for_each ( m_mapServer.begin(), m_mapServer.end(), std_afunc::DeleteMapObject() );
				m_mapServer.clear();
				SaveData();
				return FALSE;
			}
		}
	}
	return TRUE;
}

//	---------------------------------------------------------------------------------------------------------------------
//													IMAGE	LOAD
//	---------------------------------------------------------------------------------------------------------------------

BOOL DxClubMan::LoadBMPFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor )
{
	FILE* file = NULL;
	fopen_s(&file, strPathname, "rb");
	if( !file )        return FALSE;

	BITMAPFILEHEADER	bfHeader;
	fread(&bfHeader,sizeof(BITMAPFILEHEADER),1,file);

	DWORD dwBufferSize = bfHeader.bfSize-bfHeader.bfOffBits;
	if ( bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER) > sizeof(BITMAPINFO)+(dwWidth*dwHeight)*sizeof(RGBQUAD) )	return FALSE;
	
	BITMAPINFO* pbmi=(BITMAPINFO*)new BYTE[bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)];
	fread(pbmi,bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER),1,file);

	DWORD	dwStage	= 1;
	DWORD	dwBPP	= pbmi->bmiHeader.biBitCount;
	if ( dwBPP != 24 )							goto _RETURN;
	if ( dwWidth != pbmi->bmiHeader.biWidth )	goto _RETURN;
	if ( dwHeight != pbmi->bmiHeader.biHeight )	goto _RETURN;
	
	DWORD dwOffset;
	DWORD b;
	DWORD g;
	DWORD r;
	DWORD a;

	for ( DWORD y=0;y<dwHeight;++y )
	{
		dwOffset = (dwHeight-y-1)*dwWidth;
		for ( DWORD x=0;x<dwWidth;++x )
		{
			b=getc(file);
			g=getc(file);
			r=getc(file);
			a=0xff;
			pColor[dwOffset+x]=(a<<24)+(r<<16)+(g<<8)+b;
		}
	}

	SAFE_DELETE_ARRAY ( pbmi );
	fclose(file);
	return TRUE;

_RETURN:
	SAFE_DELETE_ARRAY ( pbmi );
	fclose(file);
	return FALSE;
}

BOOL DxClubMan::LoadTGAFile ( const char* strPathname, DWORD dwWidth, DWORD dwHeight, DWORD* pColor )
{
	FILE* file = NULL;
	fopen_s(&file, strPathname, "rb");
	if( !file )			return FALSE;

	struct TargaHeader
	{
		BYTE	idLength;		
		BYTE	ColorMapType;	
		BYTE	ImageType;		
		WORD	ColorMapFirst;	
		BYTE	ColorMapBits;	
		WORD	FirstX;			
		WORD	FirstY;			
		WORD	Width;			
		WORD	Height;			
		BYTE	Bits;			
		BYTE	Descriptor;	
	}tga;

	fread( &tga, sizeof(TargaHeader), 1, file );

	DWORD dwBPP = tga.Bits;
	if ( dwBPP!=32 && dwBPP!=24 )	goto _RETURN;
	if ( dwWidth != tga.Width )		goto _RETURN;
	if ( dwHeight != tga.Height )	goto _RETURN;

	DWORD dwOffset;
	DWORD PacketInfo;
	DWORD PacketType;
	DWORD PixelCount;
	DWORD dwB;
	DWORD dwG;
	DWORD dwR;
	DWORD dwA;
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;

	for( DWORD y=0; y<dwHeight; y++ )
	{
		dwOffset = y*dwWidth;					//Origin in lower left-hand corner

		if( 0 == ( tga.Descriptor & 0x0010 ) )	//Origin in upper left-hand corner
			dwOffset = (dwHeight-y-1)*dwWidth;

		for( DWORD x=0; x<dwWidth; x )
		{
			//이미지 타입이 10인 경우
			if( tga.ImageType == 10 )
			{
				PacketInfo = getc( file );
				PacketType = 0x80 & PacketInfo;
				PixelCount = ( 0x007f & PacketInfo ) + 1;

				if( PacketType )				//for the run length packet
				{
					dwB = getc( file );
					dwG = getc( file );
					dwR = getc( file );
					dwA = 0xff;
					if( dwBPP == 32 )
						dwA = getc( file );

					while( PixelCount-- )		//반복된 칼라 정보
					{
						pColor[dwOffset+x] = (dwA<<24)+(dwR<<16)+(dwG<<8)+dwB;
						x++;
					}
				}
				else							//for the raw packet
				{
					while( PixelCount-- )		
					{
						b = getc( file );
						g = getc( file );
						r = getc( file );
						a = 0xff;
						if( dwBPP == 32 )
							a = getc( file );

						pColor[dwOffset+x] = (a<<24)+(r<<16)+(g<<8)+b;
						x++;
					}
				}
			}
			//이미지 타입이 2인 경우
			else
			{
				b = getc( file );
				g = getc( file );
				r = getc( file );
				a = 0xff;
				if( dwBPP == 32 )
					a = getc( file );

				pColor[dwOffset+x] = (a<<24)+(r<<16)+(g<<8)+b;
				x++;
			}
		}
	}

	fclose( file );
	return TRUE;

_RETURN:
	fclose( file );
	return FALSE;
}
