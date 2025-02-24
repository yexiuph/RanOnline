#include "pch.h"

#include <algorithm>
#include "./SerialFile.h"
#include "./GLogic.h"
#include "./DxViewPort.h"
#include "./RENDERPARAM.h"

#include "DxSkinMesh9.h"
#include "DxSkinMeshMan.h"
#include "./StlFunctions.h"

#include "DxSkinEffResult.h"
#include "./DxEffChar.h"
#include "./EDITMESHS.h"
#include "./TextureManager.h"

#include "DxSkinPieceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern char szTRACE_NAME[MAX_TRACE][STRACE_NSIZE] = 
{
	STRACE_IMAGE01,
	STRACE_IMAGE02,
	STRACE_IMAGE11,
	STRACE_IMAGE12,
	STRACE_IMAGE21,
	STRACE_IMAGE22,

	STRACE_STRIKE01,
	STRACE_STRIKE02,
	STRACE_STRIKE11,
	STRACE_STRIKE12,

	STRACE_ARROW01,

	STRACE_RAIN01,
	STRACE_RAIN02,
	STRACE_RAIN03,

	STRACE_BLOOD01,

	STRACE_ENERGY01,
	STRACE_ENERGY02,
	STRACE_ENERGY03,
};

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

STRACOR::STRACOR () :
	m_vPos(0,0,0),
	m_vNormal(1,0,0)
{
	memset( m_szName, 0, sizeof(char)*STRACE_NSIZE );
}

STRACOR::STRACOR ( const char* _szName, const SVERTEXINFLU &_sVertexInflu, D3DXVECTOR3 _vPos ) :
	m_vPos(_vPos),
	m_vNormal(1,0,0)
{
	StringCchCopy ( m_szName, STRACE_NSIZE, _szName );

	m_sVertexInflu = _sVertexInflu;
}

STRACOR::STRACOR ( const STRACOR& _sTracor ) :
	m_vNormal(1,0,0)
{
	StringCchCopy ( m_szName, STRACE_NSIZE, _sTracor.m_szName );
	m_vPos = _sTracor.m_vPos;
	m_sVertexInflu = _sTracor.m_sVertexInflu;
}

bool STRACOR::operator== ( const STRACOR& lvalue )
{
	return !strcmp(m_szName,lvalue.m_szName);
}

HRESULT SMATERIAL_PIECE::LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = TextureManager::LoadTexture ( m_strTexture.c_str(), pd3dDevice, m_pTexture, NULL, 0 );

	if (FAILED(hr))
	{
		hr = S_OK;
		m_pTexture = NULL;
	}
	else
	{
		m_dwTexID = TextureManager::GetTextureID( m_strTexture.c_str() );;

		//	Note : 텍스쳐 읽기에 성공시에 이 메터리얼의 초기 값을 결정.
		TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( m_strTexture.c_str() );

		if( emType==TextureManager::EMTT_ALPHA_HARD )			m_dwFlags |= DXMATERIAL::FLAG_ALPHA_HARD;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT )		m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT01 )	m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT02 )	m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
	}

	return hr;
}

void SMATERIAL_PIECE::SaveFile ( CSerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		DWORD dwLength = static_cast<DWORD> ( m_strTexture.length() + 1 );
		SFile << dwLength;
		SFile.WriteBuffer ( m_strTexture.c_str(), dwLength );
		
		SFile << m_dwFlags;
		SFile << m_dwZBias;
	}
	SFile.EndBlock();
}

void SMATERIAL_PIECE::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVERSION, dwBLOCKSIZE;
	SFile >> dwVERSION;
	SFile >> dwBLOCKSIZE;
	if ( dwVERSION == VERSION )
	{
		DWORD dwLength;
		SFile >> dwLength;
		char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
		SFile.ReadBuffer ( szBuffer, dwLength );
		m_strTexture = szBuffer;

		SFile >> m_dwFlags;
		SFile >> m_dwZBias;

		m_fZBias = (float)m_dwZBias*0.00001f;
	}
	else
	{
		GASSERT("SMATERIAL_PIECE::LoadFile() 지원하지 않는 버전입니다.");
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	LoadTexture ( pd3dDevice );
}


DxSkinPieceContainer& DxSkinPieceContainer::GetInstance()
{
	static DxSkinPieceContainer Instance;
	return Instance;
}

const DWORD	DxSkinPiece::VERSION = 0x00000107;
const char	DxSkinPiece::m_szPIECETYPE[PIECE_SIZE][64] =
{
	"머리",
	"상체",
	"하체",
	"손",
	"오른손도구",
	"왼손도구",
	"발",
	"헤어",
	"모자",
	"S.R.C",
	"REV03",
	"REV04",
	"REV05",
};

DxSkinPiece::DxSkinPiece () :
	m_szFileName(NULL),
	
	m_dwRef(0),
	
	m_szXFileName(NULL),
	m_pSkinMesh(NULL),

	m_szSkeleton(NULL),
	m_pSkeleton(NULL),

	m_emType(PIECE_HEAD),
	m_emWeaponWhereBack(EMPEACE_WEAPON_RSLOT),
	m_szMeshName(NULL),

	m_dwFlags(NULL),
	m_dwFlag(0L),
	m_pmcMesh(NULL),
	m_dwVertexNUM(0),

	m_dwMaterialNum(0),
	m_pMaterialPiece(NULL)
{
	m_dwFlag |= EMCF_SHADOW;
}

DxSkinPiece::~DxSkinPiece ()
{
	SAFE_DELETE_ARRAY(m_szFileName);
	SAFE_DELETE_ARRAY(m_szXFileName);
	SAFE_DELETE_ARRAY(m_szSkeleton);
	SAFE_DELETE_ARRAY(m_szMeshName);

	SAFE_DELETE_ARRAY(m_pMaterialPiece);

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT.erase ( m_vecEFFECT.begin(), m_vecEFFECT.end() );
	m_vecEFFECT.clear();

	// don't delete m_pmcMesh
}

void DxSkinPiece::SumEffFlag ()
{
	m_dwFlags = NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		m_dwFlags |= m_vecEFFECT[n]->GetFlag();
	}
}

void DxSkinPiece::AddEffList ( DxEffChar* pEffChar )
{
	GASSERT(pEffChar);

	m_vecEFFECT.push_back ( pEffChar );
	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	//	flag가 지정된 효과의 경우 flag를 합산.
	m_dwFlags |= pEffChar->GetFlag();
}

void DxSkinPiece::DelEffList ( DxEffChar* pEffChar )
{
	if ( !pEffChar )	return;

	DWORD dwFlag = pEffChar->GetFlag();
	DWORD dwOrder = pEffChar->GetStateOrder();

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), dwOrder, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > dwOrder )	break;	//	존제하지 않는다.
		if ( pEFFECT!=pEffChar )					continue;

		//	삭제.
		SAFE_DELETE(pEFFECT);
		m_vecEFFECT.erase(pos);
		break;
	}

	//	flag가 있었던 효과가 삭제될 경우 flag를 다시 합산.
	if ( dwFlag )		SumEffFlag ();
}

HRESULT DxSkinPiece::SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	GASSERT(szXFileName);
	GASSERT(szSkeleton);
	GASSERT(pd3dDevice);

	SAFE_DELETE_ARRAY(m_szSkeleton);
	int nStrLen = strlen(szSkeleton)+1;
	m_szSkeleton = new char[nStrLen];
	StringCchCopy( m_szSkeleton, nStrLen, szSkeleton );
	m_pSkeleton = DxBoneCollector::GetInstance().Load ( m_szSkeleton, pd3dDevice );
	if ( !m_pSkeleton )		return E_FAIL;

	SAFE_DELETE_ARRAY(m_szXFileName);
	nStrLen = strlen(szXFileName)+1;
	m_szXFileName = new char[nStrLen];
	StringCchCopy( m_szXFileName, nStrLen, szXFileName );
	m_pSkinMesh = DxSkinMeshMan::GetInstance().LoadSkinMesh( m_szXFileName, m_szSkeleton, pd3dDevice, bThread );
	if ( !m_pSkinMesh )		return E_FAIL;

	return S_OK;
}

void DxSkinPiece::SelectMesh ( PSMESHCONTAINER pmcMesh )
{
	GASSERT(pmcMesh);

	m_pmcMesh = (PSMESHCONTAINER) pmcMesh;

	if( !m_pmcMesh->Name )	return;

	SAFE_DELETE_ARRAY(m_szMeshName);
	int nStrLen = strlen(m_pmcMesh->Name)+1;
	m_szMeshName = new char[nStrLen];
	StringCchCopy( m_szMeshName, nStrLen, m_pmcMesh->Name );

	m_pmcMesh->GetNumMaterials();
	if ( m_pmcMesh->GetNumMaterials() == m_dwMaterialNum && m_pMaterialPiece )
	{
		return;
	}

	//	Note : 초기화.
	//
	m_dwMaterialNum = 0;
	SAFE_DELETE_ARRAY(m_pMaterialPiece);

	m_dwMaterialNum = m_pmcMesh->GetNumMaterials();
	if ( m_dwMaterialNum==0 )
	{
		return;
	}

	m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];

	return;
}

SVERTEXINFLU* DxSkinPiece::GetTracePos ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(),STRACOR_OPER() );
	if ( found == m_vecTrace.end() )						return NULL;

	if ( !strcmp((*found).m_szName,strTrace.c_str()) )		return &(*found).m_sVertexInflu;
	return NULL;
}

void DxSkinPiece::SetTracePos ( std::string strTrace, SVERTEXINFLU &sVertInflu )
{
	m_vecTrace.push_back ( STRACOR(strTrace.c_str(),sVertInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
	std::sort  ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );
}

void DxSkinPiece::DelTracePos ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(), STRACOR_OPER() );
	if ( found==m_vecTrace.end() )	return;

	if ( !strcmp((*found).m_szName,strTrace.c_str()) )
	{
		m_vecTrace.erase ( found );
	}
}

HRESULT DxSkinPiece::LoadPiece( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	HRESULT hr;

	if( !szFile )	return E_FAIL;

	SAFE_DELETE_ARRAY(m_szFileName);
	int nStrLen = strlen(szFile)+1;
	m_szFileName = new char[nStrLen];
	StringCchCopy( m_szFileName, nStrLen, szFile );

	char szPathName[MAX_PATH] = {0};
	StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, m_szFileName );


	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<basestream> pBStream(	GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
																	GLOGIC::strSKINOBJ_ZIPFILE.c_str(), 
																	szPathName, 
																	szFile ) );

	if ( !pBStream.get() )
		return E_FAIL;

	basestream &SFile = *pBStream;

	DWORD dwVer;
	SFile >> dwVer;
	if ( dwVer == VERSION )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
		
		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SFile >> m_dwMaterialNum;
		if ( m_dwMaterialNum!=0 )
		{
			SAFE_DELETE_ARRAY(m_pMaterialPiece);

			m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
			for ( DWORD i=0; i<m_dwMaterialNum; ++i )
			{
				m_pMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
			}
		}

		std::string strBuffer;
		SVERTEXINFLU sVertexInflu;

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			SFile >> strBuffer;
			sVertexInflu.LoadFile ( SFile );

			m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		}

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;

			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_vecEFFECT.push_back ( pEffChar );
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		SFile >> m_dwFlag;
	}
	else if ( dwVer == 0x0106 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
		
		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SFile >> m_dwMaterialNum;
		if ( m_dwMaterialNum!=0 )
		{
			SAFE_DELETE_ARRAY(m_pMaterialPiece);

			m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
			for ( DWORD i=0; i<m_dwMaterialNum; ++i )
			{
				m_pMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
			}
		}

		std::string strBuffer;
		SVERTEXINFLU sVertexInflu;

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			SFile >> strBuffer;
			sVertexInflu.LoadFile ( SFile );

			m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		}

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;

			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_vecEFFECT.push_back ( pEffChar );
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		SFile >> m_dwFlag;
	}
	else if( dwVer == 0x0105 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SFile >> m_dwMaterialNum;
		if ( m_dwMaterialNum!=0 )
		{
			SAFE_DELETE_ARRAY(m_pMaterialPiece);

			m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
			for ( DWORD i=0; i<m_dwMaterialNum; ++i )
			{
				m_pMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
			}
		}

		std::string strBuffer;
		SVERTEXINFLU sVertexInflu;

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			SFile >> strBuffer;
			sVertexInflu.LoadFile ( SFile );

			m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		}

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;

			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_vecEFFECT.push_back ( pEffChar );
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		SFile >> m_dwFlag;

		// Note : Add for Version.106
		m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;
	}
	else if( dwVer == 0x0104 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SFile >> m_dwMaterialNum;
		if ( m_dwMaterialNum!=0 )
		{
			SAFE_DELETE_ARRAY(m_pMaterialPiece);

			m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
			for ( DWORD i=0; i<m_dwMaterialNum; ++i )
			{
				m_pMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
			}
		}

		std::string strBuffer;
		SVERTEXINFLU sVertexInflu;

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			SFile >> strBuffer;
			sVertexInflu.LoadFile ( SFile );

			m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		}

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;
			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_dwFlag |= pEffChar->GetFlag();
			m_vecEFFECT.push_back(pEffChar);
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		// Note : Add for Version.106
		m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;
	}
	else if ( dwVer == 0x0103 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		std::string strBuffer;
		SVERTEXINFLU sVertexInflu;

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			SFile >> strBuffer;
			sVertexInflu.LoadFile ( SFile );

			m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		}

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;
			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_dwFlag |= pEffChar->GetFlag();
			m_vecEFFECT.push_back(pEffChar);
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		// Note : Add for Version.106
		m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;
	}
	else if ( dwVer == 0x0102 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SVERTEXINFLU sTraceArray[10];
		for ( int i=0; i<10; i++ )
		{
			sTraceArray[i].LoadFile ( SFile );
		}

		if ( sTraceArray[0].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE01,sTraceArray[0],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		if ( sTraceArray[1].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE02,sTraceArray[1],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );

		if ( sTraceArray[2].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE11,sTraceArray[2],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );

		if ( sTraceArray[3].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE12,sTraceArray[3],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;
			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_dwFlag |= pEffChar->GetFlag();
			m_vecEFFECT.push_back(pEffChar);
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		// Note : Add for Version.106
		m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;
	}
	else if ( dwVer == 0x0101 )
	{
		SFile >> m_dwRef;

		DWORD dwSize;

		SFile >> dwSize;
		char *szXFileName = new char[dwSize];
		SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

		SFile >> dwSize;
		char *szSkeleton = new char[dwSize];
		SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

		//	Note : 스킨 메쉬 읽기.
		//
		hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bThread );
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE_ARRAY(szSkeleton);
		if ( FAILED(hr) )	return hr;

		SFile >> dwSize;
		m_emType = (EMPIECECHAR) dwSize;

		SFile >> dwSize;
		m_szMeshName = new char[dwSize];
		SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

		m_pmcMesh = m_pSkinMesh->FindMeshContainer ( m_szMeshName );
		if ( !m_pmcMesh )	return E_FAIL;

		SVERTEXINFLU sTraceArray[10];
		for ( int i=0; i<8; i++ )
		{
			sTraceArray[i].LoadFile ( SFile );
		}

		if ( sTraceArray[0].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE01,sTraceArray[0],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		if ( sTraceArray[1].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE02,sTraceArray[1],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );

		if ( sTraceArray[2].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE11,sTraceArray[2],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
		if ( sTraceArray[3].m_dwIndex != COL_ERR )
			m_vecTrace.push_back ( STRACOR(STRACE_IMAGE12,sTraceArray[3],D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );

		std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

		dwSize = 0;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; i++ )
		{
			DWORD TypeID;
			SFile >> TypeID;

			DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
			if ( !pEffChar )	return E_FAIL;
			pEffChar->SetLinkObj ( NULL, this );
			hr = pEffChar->LoadFile ( SFile, pd3dDevice );
			if (FAILED(hr))
			{
				SAFE_DELETE(pEffChar);
				continue;
			}

			m_dwFlag |= pEffChar->GetFlag();
			m_vecEFFECT.push_back(pEffChar);
		}

		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		// Note : Add for Version.106
		m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;
	}
	else
	{
		char szMsg[MAX_PATH] = "";
		StringCchCopy( szMsg, MAX_PATH, m_szFileName );
		StringCchCat( szMsg, MAX_PATH, _T(" / Can't Load Skin object!!!") );

		MessageBox ( NULL, szMsg, "ERROR", MB_OK );

		return E_FAIL;
	}

	if ( m_dwMaterialNum!=m_pmcMesh->GetNumMaterials() )
	{
		m_dwMaterialNum = m_pmcMesh->GetNumMaterials();

		SAFE_DELETE_ARRAY(m_pMaterialPiece);
		m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
	}

	// Note : 이제부터는 정점 갯수를 가지고 있을 것이다.
	m_dwVertexNUM = m_pmcMesh->m_dwVertexNUM;

	// Note : Cartoon Mesh를 생성한다.
	m_pSkinMesh->Load( m_szXFileName, pd3dDevice );

	return S_OK;
}

HRESULT DxSkinPiece::SavePiece ( const char* szFile )
{
//	GASSERT(szFile);
	
	SAFE_DELETE_ARRAY(m_szFileName);
	int nStrLen = strlen(szFile)+1;
	m_szFileName = new char[nStrLen];
	StringCchCopy( m_szFileName, nStrLen, szFile );

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, m_szFileName );

	CSerialFile SFile;
	if ( !SFile.OpenFile ( FOT_WRITE, (char*)szPathName ) )	return E_FAIL;

	SFile << VERSION;

	SFile << m_dwRef;

	DWORD dwSize;

	if ( !m_szXFileName )	return E_FAIL;

	dwSize = strlen(m_szXFileName)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szXFileName, sizeof(char)*dwSize );

	if ( !m_szSkeleton )	return E_FAIL;

	dwSize = strlen(m_szSkeleton)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile << (DWORD) m_emType;
	SFile << (DWORD)m_emWeaponWhereBack;

	if ( !m_szMeshName )	return E_FAIL;

	dwSize = strlen(m_szMeshName)+1;
	SFile << dwSize;
	SFile.WriteBuffer ( m_szMeshName, sizeof(char)*dwSize );

	SFile << m_dwMaterialNum;
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_pMaterialPiece[i].SaveFile ( SFile );
	}

	//	Note : trace point
	//
	{
		SFile << DWORD(m_vecTrace.size());
		for ( size_t n=0; n<m_vecTrace.size(); ++n )
		{
			SFile << std::string(m_vecTrace[n].m_szName);
			m_vecTrace[n].m_sVertexInflu.SaveFile ( SFile );
		}
	}

	//	Note : Effect skin Map
	//
	{
		SFile << (DWORD) m_vecEFFECT.size ();
		for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
		{
			DxEffChar* pEffChar = m_vecEFFECT[n];
			pEffChar->SaveFile ( SFile );
		}
	}

	SFile << m_dwFlag;

	// Note : Cartoon Mesh를 생성한다.	( 일반과 HLSL 용 2개를 생성한다. )
	m_pSkinMesh->SaveNORMAL( m_szXFileName );
	m_pSkinMesh->SaveHLSL( m_szXFileName );
	m_pSkinMesh->SaveNormalMesh( m_szXFileName );

//	m_pSkinMesh->SaveLOD_SW( m_szXFileName );

	return S_OK;
}

HRESULT DxSkinPiece::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPiece::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPiece::InvalidateDeviceObjects ()
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPiece::DeleteDeviceObjects ()
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPiece::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : 이팩트 업데이트.
	//
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];

		pEffChar->FrameMove ( fTime, fElapsedTime );

		//	Note : 종료시점 도달시에 Effect 제거.
		//if ( pEffChar->IsEnd() )
		//{
		//	SAFE_DELETE(pEFFECT);
		//	m_vecEFFECT.erase ( m_vecEFFECT.begin()+n );
		//  break;// 이럼.. 다음것을 못하지. 쩝.
		//
		//	if ( dwFlag )	SumEffFlag();
		//}
	}

	SumEffFlag();

	return S_OK;
}

HRESULT DxSkinPiece::DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex )
{
	if ( !m_pmcMesh )		return E_FAIL;

	HRESULT hr = S_OK;

	if ( m_pSkinMesh )
	{
		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= m_pMaterialPiece;
		sCharSetting.pmtrlSpecular		= NULL;
		sCharSetting.bWorldIdentity		= FALSE;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->SetDrawState ( bOriginDraw, bAlpha, bShadow, bAlphaTex );
		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting );

		return S_OK;
	}

	return E_FAIL;
}




HRESULT DxSkinPiece::MeshEditRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;
	if ( !m_pSkeleton || !m_pSkinMesh )		return S_FALSE;

	BOOL	bOriginDraw	= TRUE;
	BOOL	bAlphaTex	= TRUE;
	BOOL	bAlpha		= FALSE;
	BOOL bCrashRender(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// 렌더시 충돌이 일어난다.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlphaTex	= FALSE;
	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlpha		= TRUE;

	//	Note : 계층 메트릭스 업데이트.
	//
	if ( !pSkeleton )
	{
		m_pSkeleton->UpdateBones ( m_pSkeleton->pBoneRoot, matRot );
	}

	//	Note : 일반 메시일 경우 Bone이 틀릴 가능성을 고려함.
	//
	if ( !pSkeleton )	pSkeleton = m_pSkeleton;

	DWORD dwOldFill, dwOldLight;
	
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &dwOldFill );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwOldLight );

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );


	//	Note : 캐릭터 텍스쳐 파일에 스펙큘러 맵이 들어 있을 경우 1번만 뿌리기 위해서 기본 조각파일을 뿌리지 않는다.
	//
	if ( bOriginDraw )
	{
		//	Note : SkinMesh 선택부분 그리기.
		//
		if ( !m_pmcMesh )	return S_FALSE;

	
		
		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );
		pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

		LPD3DXBONECOMBINATION pBoneComb;
		LPD3DXBONECOMBINATION pBoneCur;
		UINT				  iMatrixIndex;
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pmcMesh->pBoneCombinationBuf->GetBufferPointer());


		for( DWORD iAttrib = 0; iAttrib < m_pmcMesh->NumAttributeGroups; iAttrib++ )
		{
			pBoneCur = &pBoneComb[iAttrib];

			for( DWORD i = 0; i < m_pmcMesh->NumInfl; i++ )
			{				
				iMatrixIndex = pBoneCur->BoneId[i];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &mat, &m_pmcMesh->pBoneOffsetMatrices[iMatrixIndex], m_pmcMesh->ppBoneMatrixPtrs[iMatrixIndex] );
					pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &mat );
				}
			}
			
//			D3DXMatrixMultiply( &mat, &m_pmcMesh->pBoneOffsetMatrices[i], m_pmcMesh->ppBoneMatrixPtrs[i] );
			m_pmcMesh->MeshData.pMesh->DrawSubset(iAttrib);
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, dwOldFill );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, dwOldLight );

	return S_OK;
}

HRESULT DxSkinPiece::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;
	if ( !m_pSkeleton || !m_pSkinMesh )		return S_FALSE;

	BOOL	bOriginDraw	= TRUE;
	BOOL	bAlphaTex	= TRUE;
	BOOL	bAlpha		= FALSE;
	BOOL bCrashRender(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// 렌더시 충돌이 일어난다.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlphaTex	= FALSE;
	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlpha		= TRUE;

	//	Note : 계층 메트릭스 업데이트.
	//
	if ( !pSkeleton )
	{
		m_pSkeleton->UpdateBones ( m_pSkeleton->pBoneRoot, matRot );
	}

	//	Note : 일반 메시일 경우 Bone이 틀릴 가능성을 고려함.
	//
	if ( !pSkeleton )	pSkeleton = m_pSkeleton;

	if ( !m_pSkinMesh )
	{
		//if ( m_pmcMesh->m_numBoneComb == 3 )
		//{
		//	//	Note : 영향 받는 본의 matrix 포인터를 다시 찾는다.
		//	//

		//	//	m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//	//	m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//	//	m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.

		//	DxBoneTrans* pBoneTrans = pSkeleton->FindBone ( m_pmcMesh->m_szBoneNames[2].c_str() );
		//	if ( pBoneTrans )	m_pmcMesh->m_pBoneMatrix[2] = &pBoneTrans->matCombined;
		//	else				m_pmcMesh->m_pBoneMatrix[2] = NULL;
		//}
	}

	DxEffChar*	pEffAmbient = NULL;
	DxEffChar*	pEffToon = NULL;

	//	Note : Setting State.
	//
	{
		SKINEFFDATA sSkinEffData;

		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];
			if ( pEFFECT->GetStateOrder() > EMEFFSO_RENDERSTATE )	break;

			pEFFECT->SettingState( pd3dDevice, sSkinEffData );
		}
	}

	//	Note : 캐릭터 텍스쳐 파일에 스펙큘러 맵이 들어 있을 경우 1번만 뿌리기 위해서 기본 조각파일을 뿌리지 않는다.
	//
	if ( bOriginDraw )
	{
		//	Note : SkinMesh 선택부분 그리기.
		//
		if ( !m_pmcMesh )	return S_FALSE;

		m_pSkinMesh->SetDrawState( bOriginDraw, bAlpha, FALSE, bAlphaTex );

		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= m_pMaterialPiece;
		sCharSetting.pmtrlSpecular		= NULL;
		sCharSetting.bWorldIdentity		= TRUE;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );
	}

	//	Note : render effect skin
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_NORMAL, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_NORMAL )	break;	//	EMEFFSO_NORMAL이 아니면 중단.

			switch( RENDERPARAM::emCharRenderTYPE )
			{
			case EMCRT_SOFTWARE:
			case EMCRT_NORMAL:
			case EMCRT_VERTEX:
				if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
				break;
			case EMCRT_PIXEL:
				//if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
				break;
			}

			if( bCrashRender )
			{
				switch( RENDERPARAM::emCharRenderTYPE )
				{
				case EMCRT_SOFTWARE:
				case EMCRT_NORMAL:
				case EMCRT_VERTEX:
					if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
					break;
				case EMCRT_PIXEL:
					if( pEFFECT->GetFlag() & EMECF_NONHLSL )	continue;	// 충돌이 일어나는 것을 뿌리지 않는다.
					break;
				}
			}

			pEFFECT->Render( pd3dDevice, TRUE );
		}
	}

	//	Note : Setting State.
	//
	{
		GLEFFCHAR_VEC_RITER pos = std::lower_bound ( m_vecEFFECT.rbegin(), m_vecEFFECT.rend(), EMEFFSO_RENDERSTATE, DXEFFCHAR_DISOPER() );
		for ( ; pos!=m_vecEFFECT.rend(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);

			pEFFECT->RestoreState ( pd3dDevice );
		}
	}


	//	Note : Glow
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_GLOW )	break;	//	EMEFFSO_GLOW이 아니면 중단.

			pEFFECT->Render( pd3dDevice, TRUE );
		}
	}

	//	Note : Single Eff
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_GLOW이 아니면 중단.

			pEFFECT->SetSkeleton( pSkeleton );
			pEFFECT->RenderEff( pd3dDevice, NULL, 1.f );
		}
	}

	return S_OK;
}

HRESULT DxSkinPiece::RenderTEST( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton )
{
	//	Note : Single Eff
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_GLOW이 아니면 중단.

			pEFFECT->RenderEDIT( pd3dDevice, NULL, 1.f );
		}
	}

	return S_OK;
}

HRESULT DxSkinPiece::DrawTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pmcMesh )	return S_FALSE;
	
	
	VECTRACE_ITER iter = m_vecTrace.begin ();
	VECTRACE_ITER iter_end = m_vecTrace.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		SVERTEXINFLU* pVertInflu = &((*iter).m_sVertexInflu);

		D3DXVECTOR3 vVecOutput;
		D3DXVECTOR3 vNorOutput;
		m_pmcMesh->CalculateVertexInfluences( pVertInflu, vVecOutput, vNorOutput, NULL, TRUE );

		pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
		EDITMESHS::RENDERSPHERE ( pd3dDevice, vVecOutput, 0.2f );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );
	}

	return S_OK;
}

HRESULT DxSkinPiece::DrawSelTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szSelVert )
{
	if ( !m_pmcMesh )	return S_FALSE;

	SVERTEXINFLU* pVertInflu = GetTracePos(szSelVert);
	if ( !pVertInflu )	return S_FALSE;

	D3DXVECTOR3 vVecOutput;
	D3DXVECTOR3 vNorOutput;

	m_pmcMesh->CalculateVertexInfluences ( pVertInflu, vVecOutput, vNorOutput, NULL, TRUE );

	pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
	EDITMESHS::RENDERSPHERE ( pd3dDevice, vVecOutput, 0.35f );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

	return S_OK;
}

void DxSkinPiece::ClearAll ()
{
	SAFE_DELETE_ARRAY(m_szFileName);
	SAFE_DELETE_ARRAY(m_szXFileName);
	SAFE_DELETE_ARRAY(m_szSkeleton);
	SAFE_DELETE_ARRAY(m_szMeshName);

	SAFE_DELETE_ARRAY(m_pMaterialPiece);

	m_pSkinMesh = NULL;
	m_pSkeleton = NULL;
	
	m_emType = PIECE_HEAD;
	m_pmcMesh = NULL;

	m_vecTrace.clear ();

	m_dwFlags = NULL;

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT.erase ( m_vecEFFECT.begin(), m_vecEFFECT.end() );
	m_vecEFFECT.clear();
	m_dwMaterialNum = 0;
}

HRESULT DxSkinPiece::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if ( !m_pSkinMesh || !m_pmcMesh )		return E_FAIL;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	//	Note : 계층 메트릭스 업데이트.
	//
	if ( m_pSkeleton )
	{
		m_pSkeleton->UpdateBones ( m_pSkeleton->pBoneRoot, matIdentity );
	}

	HRESULT hr = m_pSkinMesh->CalculateBoundingBox ( pd3dDevice, m_pmcMesh, vMax, vMin );

	return S_OK;
}

//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
//
HRESULT DxSkinPiece::CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice )
{
	SVERTEXINFLU* pVertInflu = GetTracePos(strTrace);
	if ( !pVertInflu )	return E_FAIL;
	if ( !m_pmcMesh )	return E_FAIL;

	////	TODO::
	////	Note : 일반 메시일 경우 Bone에 링크되어 있는걸 가정.
	////		연결 설정된 본과 다른 곳에 사용된다는걸 가정하여 본을 다시 연결	시켜줌.
	////
	//if ( !m_pmcMesh->pSkinInfo )
	//{
	//	//	Note : 영향 받는 본의 matrix 포인터를 다시 찾는다.
	//	//
	//	//	m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.

	//	DxBoneTrans* pHandHeld = m_pSkeleton->FindBone ( m_pmcMesh->szBoneName );

	//	if ( pHandHeld )	m_pmcMesh->ppBoneMatrixPtrs[2] = &pHandHeld->matCombined;
	//	else				m_pmcMesh->ppBoneMatrixPtrs[2] = NULL;
	//}

	//return m_pmcMesh->CalculateVertexInfluences ( pVertInflu, vVert, vNormal, pBoneMatrice );

	if ( !m_pmcMesh->pSkinInfo )
	{
		vVert = pVertInflu->m_vVector;
		vNormal = pVertInflu->m_vNormal;

		return S_OK;
	}
	else
	{
		return m_pmcMesh->CalculateVertexInfluences ( pVertInflu, vVert, vNormal, pBoneMatrice, FALSE );
	}
}

void DxSkinPiece::CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD )
{
	if( !m_pmcMesh )	return;
	m_pmcMesh->IsCartoon_CreateMesh( pd3dDevice, fThickness, fSizeADD );
}

void DxSkinPiece::CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pmcMesh )	return;
	m_pmcMesh->CreateNormalMapMESH( pd3dDevice );
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxSkinPieceContainer::DxSkinPieceContainer() :
	m_pd3dDevice(NULL)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxSkinPieceContainer::~DxSkinPieceContainer()
{
}

DxSkinPiece* DxSkinPieceContainer::LoadPiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
	HRESULT hr;
	GASSERT(szFile);

	DxSkinPiece* pSkinPiece = NULL;

	pSkinPiece = FindPiece ( szFile );
	if ( pSkinPiece )	return pSkinPiece;

	//	Note : SkinPiece Load 수행.
	//
	pSkinPiece = new DxSkinPiece;

	hr = pSkinPiece->LoadPiece( szFile, pd3dDevice, bThread );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pSkinPiece);
		return NULL;
	}

	if( pSkinPiece->m_szFileName )
	{
		int nStrLen = strlen(pSkinPiece->m_szFileName)+1;
		StringCchCopy( pSkinPiece->m_szFileName, nStrLen, szFile );

		//	Note : 리스트에 삽입.
		m_mapSkinPiece[std::string(pSkinPiece->m_szFileName)] = pSkinPiece;
	}
	else	
	{
		return NULL;
	}

	return pSkinPiece;
}

HRESULT DxSkinPieceContainer::ReleasePiece ( const char* szFile )
{
	GASSERT(szFile);

	//	Note : 만약 전역 객채 파괴 순서상에서 후순위일때 에러 유발 가능성을 막음.
	if ( m_mapSkinPiece.empty() )		return E_FAIL;

	PIECEMAP_ITER iter = m_mapSkinPiece.find ( std::string(szFile) );
	if ( iter==m_mapSkinPiece.end() )	return E_FAIL;

	iter->second->m_dwRef--;
	if ( iter->second->m_dwRef == 0 )
	{
		delete iter->second;
		m_mapSkinPiece.erase ( iter );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeletePiece ( const char* szFile )
{
	if ( !szFile )				return S_FALSE;

	PIECEMAP_ITER iter = m_mapSkinPiece.find ( std::string(szFile) );
	if ( iter==m_mapSkinPiece.end() )	return E_FAIL;

	delete iter->second;
	m_mapSkinPiece.erase ( iter );

	return S_OK;
}

DxSkinPiece* DxSkinPieceContainer::FindPiece ( const char* szFile )
{
	GASSERT(szFile);

	PIECEMAP_ITER iter = m_mapSkinPiece.find ( std::string(szFile) );
	if ( iter!=m_mapSkinPiece.end() )	return iter->second;

	return NULL;
}

HRESULT DxSkinPieceContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::InvalidateDeviceObjects ()
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeleteDeviceObjects()
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::FinalCleanup ()
{
	std::for_each ( m_mapSkinPiece.begin(), m_mapSkinPiece.end(), std_afunc::DeleteMapObject() );
	m_mapSkinPiece.clear ();

	return S_OK;
}

HRESULT DxSkinPieceContainer::CleanUp ()
{
	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();
	FinalCleanup ();

	return S_OK;
}


