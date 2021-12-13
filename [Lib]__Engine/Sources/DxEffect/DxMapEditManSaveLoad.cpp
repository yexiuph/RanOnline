#include "pch.h"
#include "./SerialFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "./StlFunctions.h"
#include "./SUBPATH.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <fcntl.h>

// ----------------------------------------------------------------------------------------------------------------------------------
//											M	a	p		E	d	i	t		M	a	n
// ----------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::SaveSet ( char *pFileName )
{
	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, pFileName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION;

	SFile << m_vMin;
	SFile << m_vMax;
	SFile << m_dwGrid;
	SFile << m_dwLength_EDIT;

	int nSize = 0;
	QuadBase* m_pCur = m_pQuadList;
	while ( m_pCur )
	{
		m_pCur = (QuadBase*)m_pCur->GetNext();
		++nSize;
	}

	SFile << nSize;

	m_pCur = m_pQuadList;
	while ( m_pCur )
	{
		m_pCur->SaveFile ( SFile );
		m_pCur = (QuadBase*)m_pCur->GetNext();
	}

	// Note : 이 부분은 Thread Loading 를 사용하므로 정적로드에는 안 쓰인다.
	//			다만 Save만 각 파일에 할 뿐이다.
	std::string strSrcName, strName, strNameGame;
	strSrcName = pFileName;
	nSize = (int)strSrcName.size();
	strSrcName.resize(nSize-4);

	StringCchCopy( m_szFullNameData, MAX_PATH, strSrcName.c_str() );		// Ted 파일이 뭔지 알려준다.

	NSQUADTREE::BASEMAP_ITER iter = NSQUADTREE::m_mapBaseEDIT.begin();
	for ( ; iter!=NSQUADTREE::m_mapBaseEDIT.end(); ++iter )
	{
		int n[8];
		n[0] = ((*iter).first&0xf0000000)>>28;
		n[1] = ((*iter).first&0xf000000)>>24;
		n[2] = ((*iter).first&0xf00000)>>20;
		n[3] = ((*iter).first&0xf0000)>>16;
		n[4] = ((*iter).first&0xf000)>>12;
		n[5] = ((*iter).first&0xf00)>>8;
		n[6] = ((*iter).first&0xf0)>>4;
		n[7] = ((*iter).first&0xf);

		for ( int i=0; i<8; ++i )
		{
			if ( n[i] >= 10 )	n[i] = n[i] + 87;	// 97
			else				n[i] = n[i] + 48;
		}
		
		strName = strSrcName;
		for ( int i=0; i<8; ++i )
		{
			strName += (char)n[i];
		}
		strNameGame = strName;
		strNameGame += m_szExtNameGame;
		strName += m_szExtNameEdit;

		QuadBase* pQuadBase = (QuadBase*)((*iter).second.pObject);
		if ( pQuadBase && pQuadBase->IsEditEnalble() )
		{
			pQuadBase->SaveFile ( strName.c_str(), FALSE );
		}

		pQuadBase = (QuadBase*)((*iter).second.pObject);
		if ( pQuadBase && pQuadBase->IsEditEnalble() )
		{
			pQuadBase->SaveFile ( strNameGame.c_str(), TRUE );
		}
	}

	NSQUADTREE::m_mapBaseEDIT.clear();

	// Piece 관리하는 파일 Save
	std::string szStaticPisName;
	szStaticPisName = strSrcName;
	szStaticPisName += ".apis";
	m_sPieceMan.Save ( szStaticPisName.c_str() );
}

void DxMapEditMan::LoadSet ( char *pFileName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	std::string strSrcName;
	strSrcName = pFileName;
	int	nSize = (int)strSrcName.size();
	strSrcName.resize(nSize-4);

	StringCchCopy( m_szFullNameData, MAX_PATH, strSrcName.c_str() );		// Ted 파일이 뭔지 알려준다.
	
	CSerialFile	SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_READ, pFileName );
	if ( !bOpened )	return;

	DWORD dwVer;
	SFile >> dwVer;
	if ( dwVer == VERSION )
	{

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_dwGrid;
		SFile >> m_dwLength_EDIT;

		m_dwGrid_Length = EM_GRID*m_dwLength_EDIT;

		SAFE_DELETE ( m_pQuadList );

		int nSize = 0;
		SFile >> nSize;
		for ( int i=0; i<nSize; ++ i )
		{
			QuadBase*	pQuad = new QuadBase;

			pQuad->LoadFile ( SFile );

			pQuad->pNext = m_pQuadList;
			m_pQuadList = pQuad;
		}

		NSQUADTREE::MakeQUADTree ( m_pQuadTree, m_pQuadList );

		// Piece 관리하는 파일 Load
		std::string szStaticPisName;
		szStaticPisName = strSrcName;
		szStaticPisName += ".apis";
		std::string szDirName;
		szDirName = "..\\..";
		szDirName += SUBPATH::OBJ_FILE_STATICMESH;
		m_sPieceMan.Load ( pd3dDevice, szStaticPisName.c_str(), szDirName.c_str() );
	}
	else
	{

	}
}

// ----------------------------------------------------------------------------------------------------------------------------------
//													Q	u	a	d		B	a	s	e
// ----------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::QuadBase::SaveFile ( CSerialFile &SFile )
{
	SFile << nID_X;
	SFile << nID_Z;
	SFile << vMax;
	SFile << vMin;
}

void DxMapEditMan::QuadBase::LoadFile ( CSerialFile &SFile )
{
	SFile >> nID_X;
	SFile >> nID_Z;
	SFile >> vMax;
	SFile >> vMin;
}

void DxMapEditMan::QuadBase::NewFile ( const char* pName )
{
	int fh;

	_sopen_s(&fh, pName, _O_CREAT | _O_TRUNC, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if( fh == -1 )
		perror( "Couldn't create data file" );
	else
	{
		printf( "Created data file.\n" );
		_close( fh );
	}
}

void DxMapEditMan::QuadBase::SaveFile ( const char* pName, BOOL bThread )
{
	CSerialFile SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, pName );
	if ( !bOpened )
	{
		NewFile ( pName );

		bOpened = SFile.OpenFile ( FOT_WRITE, pName );
		if ( !bOpened )		return;		
	}

	pBase->SaveFile ( SFile, bThread );
}

DxMapEditMan::BASE* DxMapEditMan::QuadBase::LoadFile ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bThread )	// Thread 안에서 함수 호출이 됨
{
	int n[8];
	n[0] = (nID_X&0xf000)>>12;
	n[1] = (nID_X&0xf00)>>8;
	n[2] = (nID_X&0xf0)>>4;
	n[3] = (nID_X&0xf);
	n[4] = (nID_Z&0xf000)>>12;
	n[5] = (nID_Z&0xf00)>>8;
	n[6] = (nID_Z&0xf0)>>4;
	n[7] = (nID_Z&0xf);

	for ( int i=0; i<8; ++i )
	{
		if ( n[i] >= 10 )	n[i] = n[i] + 87;	// 97
		else				n[i] = n[i] + 48;
	}

	std::string	strName;
	strName = m_szFullNameData;
	for ( int i=0; i<8; ++i )
	{
		strName += (char)n[i];
	}

	if ( bThread )	strName += m_szExtNameGame;
	else			strName += m_szExtNameEdit;

	CSerialFile SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_READ, strName.c_str() );
	if ( !bOpened )
	{
		SAFE_DELETE ( pBase );
		pBase = new BASE;
		CreateBaseData ( pd3dDevice );	// if File not. Create. Default Method.	// - -; 아 영어.
		return pBase;
	}

	SAFE_DELETE ( pBase );
	pBase = new BASE;
	pBase->LoadFile ( pd3dDevice, SFile, bThread );

	return pBase;
}

// ----------------------------------------------------------------------------------------------------------------------------------
//															B	A	S	E
// ----------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::BASE::SaveFile ( CSerialFile &SFile, BOOL bThread )
{
	if ( bThread )
	{
		VERTEX*	pVertices;
		pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
		SFile.WriteBuffer ( pVertices, sizeof(VERTEX)*4*EM_GRID*EM_GRID );
		pVB->Unlock ();

		WORD*	pIndices;
		pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		SFile.WriteBuffer ( pIndices, sizeof(WORD)*3*EM_FACE );
		pIB->Unlock ();

		sBlend.SaveFile_Game ( SFile );
		sDxColor.SaveFile_Game ( SFile );
	}
	else
	{
		if ( pVertex )
		{
			SFile.WriteBuffer ( pVertex, sizeof(VERTEX)*EM_VERTEX_PART*EM_VERTEX_PART );
		}

		if ( pTile )
		{
			SFile.WriteBuffer ( pTile, sizeof(TILE)*EM_GRID*EM_GRID );
		}

		VERTEX*	pVertices;
		pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
		SFile.WriteBuffer ( pVertices, sizeof(VERTEX)*4*EM_GRID*EM_GRID );
		pVB->Unlock ();

		WORD*	pIndices;
		pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		SFile.WriteBuffer ( pIndices, sizeof(WORD)*3*EM_FACE );
		pIB->Unlock ();

		sBlend.SaveFile_Edit ( SFile );
		sDxColor.SaveFile_Edit ( SFile );
	}
}

void DxMapEditMan::BASE::LoadFile ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, BOOL bThread )
{
	if ( bThread )
	{
		VERTEX*	pVertices;
		SAFE_RELEASE ( pVB );
		pd3dDevice->CreateVertexBuffer ( 4*EM_GRID*EM_GRID*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &pVB, NULL );

		pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
		SFile.ReadBuffer ( pVertices, sizeof(VERTEX)*4*EM_GRID*EM_GRID );
		pVB->Unlock ();

		Sleep( 1 );

		WORD*	pIndices;
		SAFE_RELEASE ( pIB );
		pd3dDevice->CreateIndexBuffer ( 3*EM_FACE*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL );

		pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		SFile.ReadBuffer ( pIndices, sizeof(WORD)*3*EM_FACE );
		pIB->Unlock ();

		Sleep( 1 );

		sBlend.LoadFile_Game ( pd3dDevice, SFile );

		Sleep( 1 );

		sDxColor.LoadFile_Game ( pd3dDevice, SFile );

		Sleep( 1 );
	}
	else
	{
		SAFE_DELETE_ARRAY ( pVertex );
		pVertex = new VERTEX[EM_VERTEX_PART*EM_VERTEX_PART];
		if ( pVertex )
		{
			SFile.ReadBuffer ( pVertex, sizeof(VERTEX)*EM_VERTEX_PART*EM_VERTEX_PART );
		}

		SAFE_DELETE_ARRAY ( pTile );
		pTile = new TILE[EM_GRID*EM_GRID];
		if ( pTile )
		{
			SFile.ReadBuffer ( pTile, sizeof(TILE)*EM_GRID*EM_GRID );
		}

		VERTEX*	pVertices;
		SAFE_RELEASE ( pVB );
		pd3dDevice->CreateVertexBuffer ( 4*EM_GRID*EM_GRID*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &pVB, NULL );

		pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
		SFile.ReadBuffer ( pVertices, sizeof(VERTEX)*4*EM_GRID*EM_GRID );
		pVB->Unlock ();
		WORD*	pIndices;
		SAFE_RELEASE ( pIB );
		pd3dDevice->CreateIndexBuffer ( 3*EM_FACE*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL );

		pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		SFile.ReadBuffer ( pIndices, sizeof(WORD)*3*EM_FACE );
		pIB->Unlock ();

		sBlend.LoadFile_Edit ( pd3dDevice, SFile );
		sDxColor.LoadFile_Edit ( pd3dDevice, SFile );
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------
//											T	O	O	L	에서 사용되는 것
// ----------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::SaveTileData ( const char* pName )
{
	CSerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, pName );
	if ( !bOPEN )	return;

	SFile << (DWORD)VERSION_DATA;

	DWORD dwSize = (DWORD)m_mapTiles.size();
	SFile << dwSize;

	TILING_EDIT_MAP_ITER iter = m_mapTiles.begin();
	for ( ; iter!=m_mapTiles.end(); ++iter )
	{
		SFile << (*iter).first;

		SFile << (*iter).second.cName;
		SFile << (*iter).second.fScale;
	}

	for ( int i=0; i<14; ++i )
	{
		SFile << m_cColor_EDIT[i].r;
		SFile << m_cColor_EDIT[i].g;
		SFile << m_cColor_EDIT[i].b;
		SFile << m_cColor_EDIT[i].a;
	}
}

void DxMapEditMan::LoadTileData ( const char* pName )
{
	m_mapTiles.clear();

	std::string	sName;
	TILING_EDIT	sTileData;

	CSerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_READ, pName );
	if ( !bOPEN )	return;

	DWORD dwVer;
	SFile >> dwVer;

	if ( dwVer == VERSION_DATA )
	{
		DWORD dwSize;
		SFile >> dwSize;

		for ( DWORD k=0; k<dwSize; ++k )
		{
			SFile >> sName;

			SFile >> sTileData.cName;
			SFile >> sTileData.fScale;

			m_mapTiles.insert ( std::make_pair ( sName, sTileData ) );
		}

		for ( int i=0; i<14; ++i )
		{
			SFile >> m_cColor_EDIT[i].r;
			SFile >> m_cColor_EDIT[i].g;
			SFile >> m_cColor_EDIT[i].b;
			SFile >> m_cColor_EDIT[i].a;
		}
	}
}

void DxMapEditMan::NewTileData ()
{
	std::string strPathname;
	strPathname = m_szPathData;
	strPathname += m_szDefaultFileName.c_str();

	CSerialFile SFile;
	BOOL bOPEN = SFile.OpenFile ( FOT_READ, strPathname.c_str() );
	if ( bOPEN )
	{
		LoadTileData ( strPathname.c_str() );
	}
	else
	{
		int fh;

		_sopen_s(&fh, strPathname.c_str(), _O_CREAT | _O_TRUNC, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		if( fh == -1 )
			perror( "Couldn't create data file" );
		else
		{
			printf( "Created data file.\n" );
			_close( fh );
		}

		m_mapTiles.clear();
		SaveTileData ( strPathname.c_str() );
	}
}