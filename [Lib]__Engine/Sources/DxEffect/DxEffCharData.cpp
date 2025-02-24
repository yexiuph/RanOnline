#include "pch.h"
#include <algorithm>
#include "./dxeffchardata.h"
#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffcharDataMan& DxEffcharDataMan::GetInstance()
{
	static DxEffcharDataMan Instance;
	return Instance;
}

DxEffCharData::DxEffCharData(void) :
	m_emPieceType(PIECE_HEAD)
{
}

DxEffCharData::~DxEffCharData(void)
{
	CleanUp ();
}

BOOL DxEffCharData::Import ( EMPIECECHAR _emType, GLEFFCHAR_VEC &vecEFFECT )
{
	m_emPieceType = _emType;

	size_t nSIZE = vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = vecEFFECT[n];

		SEFFDATA* pEFFDATA = new SEFFDATA;
		pEFFECT->SaveFile ( pEFFDATA->m_byteStream );

		m_listEffData.push_back ( pEFFDATA );
	}

	return TRUE;
}

void DxEffCharData::ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return;
		pNewEffChar->LoadFile( pEffData->m_byteStream, pd3dDevice );

		pNewEffChar->ConvertTracePoint(); 

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();
		pNewEffChar->SaveFile( pEffData->m_byteStream );

		SAFE_DELETE(pNewEffChar);
	}
}

void DxEffCharData::CleanUp ()
{
	std::for_each ( m_listEffData.begin(), m_listEffData.end(), std_afunc::DeleteObject() );
	m_listEffData.clear ();
}

BOOL DxEffCharData::SetPart ( DxCharPart *pCharPart, LPDIRECT3DDEVICEQ	pd3dDevice, D3DXVECTOR3 *pDir, float fTarDir )
{
	HRESULT hr;
	if ( pCharPart->m_emType != m_emPieceType )		return FALSE;

	GASSERT(pDir&&"DxEffCharData::SetPart() 방향백터가 null입니다." );
	D3DXVECTOR3 vDIR(0,0,1);
	if ( pDir==NULL )	pDir = &vDIR;

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return FALSE;

		pNewEffChar->SetFileName ( m_strFileName );
		pNewEffChar->SetLinkObj ( pCharPart, NULL );
		pNewEffChar->SetDirection ( pDir, fTarDir );
		hr = pNewEffChar->LoadFile ( pEffData->m_byteStream, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pNewEffChar);
			continue;
		}

		pCharPart->AddEffList ( pNewEffChar );
	}

	return TRUE;
}

BOOL DxEffCharData::SetPiece ( DxSkinPiece *pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 *pDir, float fTarDir )
{
	HRESULT hr;
	if ( pSkinPiece->m_emType != m_emPieceType )		return FALSE;

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return FALSE;

		pNewEffChar->SetFileName ( m_strFileName );
		pNewEffChar->SetLinkObj ( NULL, pSkinPiece );
		if ( pDir )	pNewEffChar->SetDirection ( pDir, fTarDir );
		hr = pNewEffChar->LoadFile ( pEffData->m_byteStream, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pNewEffChar);
			continue;
		}

		pSkinPiece->AddEffList ( pNewEffChar );
	}

	return TRUE;
}

BOOL DxEffCharData::LoadFile ( basestream &SFile )
{
	DWORD dwType;
	DWORD dwSize;
	SFile >> dwType;
	m_emPieceType = EMPIECECHAR(dwType);

	SFile >> dwSize;

	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD dwBuffSize;
		SFile >> dwBuffSize;
		LPBYTE pBuffer = new BYTE[dwBuffSize];
		
		SFile.ReadBuffer ( pBuffer, dwBuffSize );

		SEFFDATA* pEFFDATA = new SEFFDATA;
		pEFFDATA->m_byteStream.WriteBuffer ( pBuffer, dwBuffSize );
		m_listEffData.push_back ( pEFFDATA );

		delete[] pBuffer;
	}

	return TRUE;
}

BOOL DxEffCharData::SaveFile ( CSerialFile &SFile )
{
	SFile << DWORD(m_emPieceType);
	SFile << DWORD(m_listEffData.size());

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		SEFFDATA *pEffData = (*iter);

		LPBYTE pBuff;
		DWORD dwSize;
		pEffData->m_byteStream.GetBuffer( pBuff, dwSize );

		SFile << dwSize;
		SFile.WriteBuffer ( pBuff, dwSize );
	}

	return TRUE;
}

BOOL DxEffCharData::LoadFile ( const char* szFile )
{
	m_strFileName = szFile;

	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
																	GLOGIC::strEFFECT_CHAR_ZIPFILE.c_str(),
																	strPathName.c_str(), 
																	szFile ) );

	if ( !pBStream.get() )
		return FALSE;

	basestream &SFile = *pBStream;

	return LoadFile ( SFile );
}

BOOL DxEffCharData::SaveFile ( const char* szFile )
{
	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	CSerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_WRITE, strPathName.c_str() );
	if ( !bOk )		return FALSE;

	return SaveFile ( SFile );
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

BOOL DxEffCharDataArray::SetSkinPart ( PDXCHARPART pCharPart, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pCharPart )	return FALSE;

	if ( m_pEffCharDataArray[pCharPart->m_emType] )
	{
		m_pEffCharDataArray[pCharPart->m_emType]->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SetSkinChar ( DxSkinChar *pSkinChar, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 *pDir, float fTarDir )
{
	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
		if ( pCharPart && pCharPart->m_pmcMesh && m_pEffCharDataArray[i] )
		{
			m_pEffCharDataArray[i]->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
		}
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SetSkinCharData ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		PDXSKINPIECE pSkinPiece = pSkinCharData->GetPiece(i);
		if ( pSkinPiece && m_pEffCharDataArray[i] )
		{
			m_pEffCharDataArray[i]->SetPiece ( pSkinPiece, pd3dDevice );
		}
	}

	return TRUE;
}

BOOL DxEffCharDataArray::Import ( DxSkinCharData *pSkinCharData )
{
	CleanUp ();

	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		PDXSKINPIECE pSkinPiece = pSkinCharData->GetPiece(i);
		if ( pSkinPiece )
		{
			DxEffCharData *pEffCharData = new DxEffCharData;
			pEffCharData->Import ( pSkinPiece->m_emType, pSkinPiece->m_vecEFFECT );

			m_pEffCharDataArray[i] = pEffCharData;
		}
	}

	return TRUE;
}

void DxEffCharDataArray::ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		if ( !m_pEffCharDataArray[i] )	continue;

		m_pEffCharDataArray[i]->ConvertTPoint( pd3dDevice );
	}
}

void DxEffCharDataArray::CleanUp ()
{
	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		SAFE_DELETE(m_pEffCharDataArray[i]);
	}
}


BOOL DxEffCharDataArray::LoadFile ( const char* szFile )
{
	m_strFileName = szFile;

	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
																	GLOGIC::strEFFECT_CHAR_ZIPFILE.c_str(),
																	strPathName.c_str(), 
																	szFile ) );

	if ( !pBStream.get() )
		return FALSE;

	basestream &SFile = *pBStream;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		BOOL bExit;
		SFile >> bExit;
		if ( !bExit )	continue;

		DxEffCharData *pEffCharData = new DxEffCharData;
		pEffCharData->LoadFile ( SFile );
		pEffCharData->SetFileName ( m_strFileName );

		EMPIECECHAR emPieceType = pEffCharData->GetPieceType();

		GASSERT(!m_pEffCharDataArray[emPieceType]);
		m_pEffCharDataArray[emPieceType] = pEffCharData;
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SaveFile ( const char* szFile )
{
	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	CSerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_WRITE, strPathName.c_str() );
	if ( !bOk )		return FALSE;

	SFile << DWORD(PIECE_SIZE);

	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		if ( m_pEffCharDataArray[i] )	SFile << BOOL(TRUE);
		else							SFile << BOOL(FALSE);

		if ( !m_pEffCharDataArray[i] )	continue;

		m_pEffCharDataArray[i]->SaveFile ( SFile );
	}

	return TRUE;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

HRESULT DxEffcharDataMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return TRUE;
}

BOOL DxEffcharDataMan::PutEffect ( DxCharPart *pCharPart, const char* szEffFile,
								  D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pCharPart )	return FALSE;

	DxEffCharData *pEffCharData = NULL;

	EFFDATA_MAP_ITER iter = m_mapEffData.find ( std::string(szEffFile) );
	if ( iter == m_mapEffData.end() )
	{
		pEffCharData = new DxEffCharData;
		BOOL bOk = pEffCharData->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharData;
			return FALSE;
		}

		m_mapEffData[std::string(szEffFile)] = pEffCharData;
	}
	else
	{
		pEffCharData = iter->second;
	}

	pEffCharData->SetPart ( pCharPart, m_pd3dDevice, pDir, fTarDir );

	return TRUE;
}

BOOL DxEffcharDataMan::PutEffect ( DxSkinChar *pSkinChar, const char* szEffFile,
								  D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pSkinChar )	return FALSE;

	DxEffCharDataArray *pEffCharDataArray = NULL;

	EFFDATA_ARRAYMAP_ITER iter = m_mapEffDataArray.find ( std::string(szEffFile) );
	if ( iter == m_mapEffDataArray.end() )
	{
		pEffCharDataArray = new DxEffCharDataArray;
		BOOL bOk = pEffCharDataArray->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharDataArray;
			return FALSE;
		}

		m_mapEffDataArray[std::string(szEffFile)] = pEffCharDataArray;
	}
	else
	{
		pEffCharDataArray = iter->second;
	}

	pEffCharDataArray->SetSkinChar ( pSkinChar, m_pd3dDevice, pDir, fTarDir );

	return TRUE;
}

BOOL DxEffcharDataMan::PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pSkinChar )	return FALSE;
	if ( !szEffFile || szEffFile[0]==NULL )	return E_FAIL;

	DxEffCharDataArray *pEffCharDataArray = NULL;

	EFFDATA_ARRAYMAP_ITER iter = m_mapEffDataArray.find ( std::string(szEffFile) );
	if ( iter == m_mapEffDataArray.end() )
	{
		pEffCharDataArray = new DxEffCharDataArray;
		BOOL bOk = pEffCharDataArray->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharDataArray;
			return FALSE;
		}

		m_mapEffDataArray[std::string(szEffFile)] = pEffCharDataArray;
	}
	else
	{
		pEffCharDataArray = iter->second;
	}

	//	Note : 지속 이팩트의 경우 파트가 변경되어 이팩트가 삭제될 경우를 대비해서 개별 확인후 삽입 해준다.
	//
	for ( int i=0; i<PIECE_SIZE; i++ )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
		BOOL bFind = pCharPart->FindEffList ( szEffFile );
		if ( !bFind )
		{
			pEffCharDataArray->SetSkinPart ( pCharPart, m_pd3dDevice, pDir, fTarDir );
		}
	}

	return TRUE;
}

void DxEffcharDataMan::OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile )
{
	if ( !pSkinChar )	return;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
		pCharPart->DelEffList ( strEffFile.c_str() );
	}
}

void DxEffcharDataMan::CleanUp ()
{
	std::for_each ( m_mapEffData.begin(), m_mapEffData.end(), std_afunc::DeleteMapObject() );
	m_mapEffData.clear ();

	std::for_each ( m_mapEffDataArray.begin(), m_mapEffDataArray.end(), std_afunc::DeleteMapObject() );
	m_mapEffDataArray.clear ();
}



