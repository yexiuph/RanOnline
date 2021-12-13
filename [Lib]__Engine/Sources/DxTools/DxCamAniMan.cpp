#include "pch.h"
#include "./DxFrameMesh.h"
#include "./SerialFile.h"
#include "DxViewPort.h"

#include "DxCamAniMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------------------------[DxCameraAni]
const DWORD DxCameraAni::VERSION = 0x0101;

void DxCameraAni::ModifySetCamera ( DxCameraAni* pCameraAni )
{
	if( !pCameraAni->m_szName )	return;

	DWORD dwSize;

	dwSize = sizeof(char) * (strlen(pCameraAni->m_szName)+1);
	m_szName = new char[dwSize];
	StringCchCopy( m_szName, dwSize, pCameraAni->m_szName );
	
	m_emCamera = pCameraAni->m_emCamera;
	m_dwFlag = pCameraAni->m_dwFlag;
	
	m_STime = pCameraAni->m_STime;
	m_ETime = pCameraAni->m_ETime;

	GASSERT(pCameraAni->m_szFromCamera);
	dwSize = sizeof(char) * (strlen(pCameraAni->m_szFromCamera)+1);
	m_szFromCamera = new char[dwSize];
	StringCchCopy( m_szFromCamera, dwSize, pCameraAni->m_szFromCamera );

	m_pFromCamera = pCameraAni->m_pFromCamera;

	if ( pCameraAni->m_emCamera==CAMERA_TTARGET )
	{
		GASSERT(pCameraAni->m_szTargetCamera);
		dwSize = sizeof(char) * (strlen(pCameraAni->m_szTargetCamera)+1);
		m_szTargetCamera = new char[dwSize];
		StringCchCopy( m_szTargetCamera, dwSize, pCameraAni->m_szTargetCamera );

		m_pTargetCamera = pCameraAni->m_pTargetCamera;
	}
	else
	{
		m_szTargetCamera = NULL;
		m_pTargetCamera = NULL;
	}
}

void DxCameraAni::ModifyCamera ( DxCameraAni* pCameraAni )
{
	Clear ();

	size_t nStrLen = strlen(pCameraAni->m_szName)+1;
	m_szName = new char[nStrLen];
	StringCchCopy( m_szName, nStrLen, pCameraAni->m_szName );

	m_emCamera = pCameraAni->m_emCamera;
	m_dwFlag = pCameraAni->m_dwFlag & ~CAMERA_SET;

	m_STime = pCameraAni->m_STime;
	m_ETime = pCameraAni->m_ETime;

	if ( pCameraAni->m_szFromCamera )
	{
		nStrLen = strlen(pCameraAni->m_szFromCamera)+1;
		m_szFromCamera = new char[nStrLen];
		StringCchCopy( m_szFromCamera, nStrLen, pCameraAni->m_szFromCamera );
	}

	if ( pCameraAni->m_szTargetCamera )
	{
		nStrLen = strlen(pCameraAni->m_szTargetCamera)+1;
		m_szTargetCamera = new char[nStrLen];
		StringCchCopy( m_szTargetCamera, nStrLen, pCameraAni->m_szTargetCamera );
	}

	if ( pCameraAni->m_pFromCamera )
	{
		m_bLocalFromCamera = TRUE;
		m_pFromCamera = new DxAnimation;
		*m_pFromCamera = *pCameraAni->m_pFromCamera;
	}

	if ( pCameraAni->m_pTargetCamera )
	{
		m_bLocalTargetCamera = TRUE;
		m_pTargetCamera = new DxAnimation;
		*m_pTargetCamera = *pCameraAni->m_pTargetCamera;
	}
}

HRESULT DxCameraAni::SaveSet ( CSerialFile &SFile )
{
	int nNameSize;
	int nFromCaSize;
	int nTargetCaSize;

	if ( m_szName )			nNameSize = sizeof(char) * (strlen(m_szName)+1);
	else					nNameSize = 0;

	if ( m_szFromCamera )	nFromCaSize = sizeof(char) * (strlen(m_szFromCamera)+1);
	else					nFromCaSize = 0;

	if ( m_szTargetCamera )	nTargetCaSize = sizeof(char) * (strlen(m_szTargetCamera)+1);
	else					nTargetCaSize = 0;

	int nTotalSize = nNameSize
					+ sizeof(m_emCamera)
					+ sizeof(m_dwFlag)
					+ sizeof(m_STime)
					+ sizeof(m_ETime)
					+ nFromCaSize
					+ nTargetCaSize;

	SFile << VERSION;
	SFile << nTotalSize;
	
	SFile << nNameSize;
	if ( nNameSize )		SFile.WriteBuffer ( m_szName, nNameSize );
	
	SFile << (DWORD) m_emCamera;
	SFile << m_dwFlag;

	SFile << m_STime;
	SFile << m_ETime;

	SFile << nFromCaSize;
	if ( nFromCaSize )		SFile.WriteBuffer ( m_szFromCamera, nFromCaSize );

	SFile << nTargetCaSize;
	if ( nTargetCaSize )	SFile.WriteBuffer ( m_szTargetCamera, nTargetCaSize );

	return S_OK;
}

HRESULT DxCameraAni::LoadSet ( CSerialFile &SFile, DxFrameMesh *pFrameMesh )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	
	dwSize = SFile.ReadBlockSize ();
	if ( dwVer==VERSION || dwVer==0x0100 )
	{
		DWORD dwLength;

		SFile >> dwLength;
		if ( dwLength )
		{
			m_szName = new char[dwLength];
			SFile.ReadBuffer ( m_szName, dwLength );
		}
		
		DWORD dwData;
		SFile >> dwData;
		m_emCamera = (CAMERA_TYPE) dwData;
		SFile >> m_dwFlag;

		SFile >> m_STime;
		SFile >> m_ETime;

		SFile >> dwLength;
		if ( dwLength )
		{
			m_szFromCamera = new char[dwLength];
			SFile.ReadBuffer ( m_szFromCamera, dwLength );

			m_pFromCamera = pFrameMesh->FindAnimation ( m_szFromCamera );
		}

		SFile >> dwLength;
		if ( dwLength )
		{
			m_szTargetCamera = new char[dwLength];
			SFile.ReadBuffer ( m_szTargetCamera, dwLength );

			m_pTargetCamera = pFrameMesh->FindAnimation ( m_szTargetCamera );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
		return S_FALSE;
	}

	return S_OK;
}

HRESULT DxCameraAni::Save ( CSerialFile &SFile )
{
	int nTotalSize = 0;
	int nStringSize;

	SFile << VERSION;

	SFile.BeginBlock ();

	if ( m_szName )			nStringSize = strlen(m_szName)+1;
	else					nStringSize = 0;

	SFile << nStringSize;
	if ( nStringSize )		SFile.WriteBuffer ( m_szName, nStringSize );
	
	SFile << (DWORD) m_emCamera;
	SFile << m_dwFlag;

	SFile << m_STime;
	SFile << m_ETime;

	if ( m_szFromCamera )	nStringSize = strlen(m_szFromCamera)+1;
	else					nStringSize = 0;

	SFile << nStringSize;
	if ( nStringSize )		SFile.WriteBuffer ( m_szFromCamera, nStringSize );

	if ( m_szTargetCamera )	nStringSize = strlen(m_szTargetCamera)+1;
	else					nStringSize = 0;

	SFile << nStringSize;
	if ( nStringSize )		SFile.WriteBuffer ( m_szTargetCamera, nStringSize );

	//	Note : 카메라 에니메이션 저장.
	//
	if ( m_pFromCamera )	SFile << BOOL(TRUE);
	else					SFile << BOOL(FALSE);

	if ( m_pFromCamera )	m_pFromCamera->SaveFile ( SFile );

	if ( m_pTargetCamera )	SFile << BOOL(TRUE);
	else					SFile << BOOL(FALSE);

	if ( m_pTargetCamera )	m_pTargetCamera->SaveFile ( SFile );

	//	Note : 저장된 크기 마킹.
	//
	SFile.EndBlock ();

	return S_OK;
}

HRESULT DxCameraAni::Load ( CSerialFile &SFile )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwVer==VERSION || dwVer==0x0100 )
	{
		DWORD dwLength;
		SFile >> dwLength;
		if ( dwLength )
		{
			m_szName = new char[dwLength];
			SFile.ReadBuffer ( m_szName, dwLength );
		}
		
		DWORD dwData;
		SFile >> dwData;
		m_emCamera = (CAMERA_TYPE) dwData;
		SFile >> m_dwFlag;

		SFile >> m_STime;
		SFile >> m_ETime;

		SFile >> dwLength;
		if ( dwLength )
		{
			m_szFromCamera = new char[dwLength];
			SFile.ReadBuffer ( m_szFromCamera, dwLength );
		}

		SFile >> dwLength;
		if ( dwLength )
		{
			m_szTargetCamera = new char[dwLength];
			SFile.ReadBuffer ( m_szTargetCamera, dwLength );
		}

		//	Note : 카메라 에니메이션 저장.
		//
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_bLocalFromCamera = TRUE;
			m_pFromCamera = new DxAnimation;
			m_pFromCamera->LoadFile ( SFile );
		}

		SFile >> bExist;
		if ( bExist )
		{
			m_bLocalTargetCamera = TRUE;
			m_pTargetCamera = new DxAnimation;
			m_pTargetCamera->LoadFile ( SFile );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
		return S_FALSE;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------[DxCamAniMan]
DxCamAniMan::DxCamAniMan(void) :
	m_fAgeCamera(0.0f),
	m_pCurrentAni(NULL),
	m_pAniList(NULL)
{
}

DxCamAniMan::~DxCamAniMan(void)
{
	Cleanup ();
}

HRESULT DxCamAniMan::Cleanup ()
{
	SAFE_DELETE(m_pAniList);
	m_pCurrentAni = NULL;

	return S_OK;
}

HRESULT DxCamAniMan::AddCameraAni ( DxCameraAni* pCameraAni )
{
	GASSERT(pCameraAni);

	//	Note : 이미 등록된 이름이라면 이전 이름을 삭제한다.
	//
	DelCameraAni ( pCameraAni->m_szName );

	//	Note : 새로이 생성하고 값을 대입.
	//
	DxCameraAni* pNewCameraAni = new DxCameraAni;
	pNewCameraAni->ModifySetCamera ( pCameraAni );

	//	Note : 리스트에 삽입.
	//
	pNewCameraAni->m_pNext = m_pAniList;
	m_pAniList = pNewCameraAni;

	return S_OK;
}

DxCameraAni* DxCamAniMan::FindCameraAni ( char* szName )
{
	DxCameraAni *pCur = m_pAniList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;
		pCur = pCur->m_pNext;
	}

	return NULL;
}

HRESULT DxCamAniMan::DelCameraAni ( char* szName )
{
	GASSERT(szName);
	if ( !m_pAniList )	return S_FALSE;

	//	Note : 만약 현제 액티브된 에니메이션이 지워질 경우 액티브 헤제.
	//
	if ( m_pCurrentAni && !strcmp(m_pCurrentAni->m_szName,szName) )
	{
		m_fAgeCamera = 0.0f;
		m_pCurrentAni = NULL;
	}

	DxCameraAni *pCur = m_pAniList, *pCurBack = NULL;
	if ( !strcmp(pCur->m_szName,szName) )
	{
		m_pAniList = m_pAniList->m_pNext;
		pCur->m_pNext = NULL;
		SAFE_DELETE(pCur);

		return S_OK;
	}

	pCurBack = m_pAniList;
	pCur = m_pAniList->m_pNext;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )
		{
			pCurBack->m_pNext = pCur->m_pNext;
			pCur->m_pNext = NULL;
			SAFE_DELETE(pCur);

			return S_OK;
		}

		pCurBack = pCur;
		pCur = pCur->m_pNext;
	}

	return E_FAIL;
}

HRESULT DxCamAniMan::ActiveCameraAni ( char* szName )
{
	DxCameraAni *pCurCamera = FindCameraAni ( szName );
	if ( !pCurCamera )	return E_FAIL;

	m_fAgeCamera = 0.0f;
	m_pCurrentAni = pCurCamera;
	DxViewPort::GetInstance().SetCameraType ( m_pCurrentAni->m_emCamera );

	return S_FALSE;
}

HRESULT DxCamAniMan::Import ( DxCamAniMan* pCameraAniMan )
{
	Cleanup ();

	DxCameraAni *pCameraAni = pCameraAniMan->GetCameraList();
	while ( pCameraAni )
	{
		DxCameraAni* pNewCameraAni = new DxCameraAni;
		pNewCameraAni->ModifyCamera ( pCameraAni );

		//	Note : 리스트에 삽입.
		//
		pNewCameraAni->m_pNext = m_pAniList;
		m_pAniList = pNewCameraAni;

		pCameraAni = pCameraAni->m_pNext;
	}

	return S_OK;
}

HRESULT DxCamAniMan::TestCameraAni ( DxCameraAni* pCameraAni )
{
	if ( pCameraAni )
	{
		m_fAgeCamera = 0.0f;
		m_pCurrentAni = pCameraAni;
		DxViewPort::GetInstance().SetCameraType ( m_pCurrentAni->m_emCamera );
	}
	return S_OK;
}

HRESULT DxCamAniMan::ResetTestCameraAni ()
{
	m_fAgeCamera = 0.0f;
	m_pCurrentAni = NULL;

	return S_OK;
}

HRESULT DxCamAniMan::SaveSet ( CSerialFile &SFile )
{
	DxCameraAni* pCur;
	DWORD dwNumCameraAni = 0;

	pCur = m_pAniList;
	while ( pCur )
	{
		dwNumCameraAni++;
		pCur = pCur->m_pNext;
	}

	SFile << dwNumCameraAni;

	pCur = m_pAniList;
	while ( pCur )
	{
		pCur->SaveSet ( SFile );
		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxCamAniMan::LoadSet ( CSerialFile &SFile, DxFrameMesh *pFrameMesh )
{
	SAFE_DELETE(m_pAniList);

	DxCameraAni* pCur = NULL;
	DWORD dwNumCameraAni = 0;

	SFile >> dwNumCameraAni;
	for ( DWORD i=0; i<dwNumCameraAni; ++i )
	{
		pCur = new DxCameraAni;
		
		HRESULT hr = pCur->LoadSet ( SFile, pFrameMesh );
		if ( FAILED(hr) )
		{
			SAFE_DELETE(pCur);
			return E_FAIL;
		}

		if ( hr==S_FALSE )
		{
			SAFE_DELETE(pCur);
		}
		else
		{
			//	Note : 리스트에 삽입.
			//
			pCur->m_pNext = m_pAniList;
			m_pAniList = pCur;
		}
	}

	return S_OK;
}

HRESULT DxCamAniMan::Save ( CSerialFile &SFile )
{
	DxCameraAni* pCur;
	DWORD dwNumCameraAni = 0;

	pCur = m_pAniList;
	while ( pCur )
	{
		dwNumCameraAni++;
		pCur = pCur->m_pNext;
	}

	SFile << dwNumCameraAni;

	pCur = m_pAniList;
	while ( pCur )
	{
		pCur->Save ( SFile );
		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT DxCamAniMan::Load ( CSerialFile &SFile )
{
	SAFE_DELETE(m_pAniList);

	DxCameraAni* pCur;
	DWORD dwNumCameraAni = 0;

	SFile >> dwNumCameraAni;
	for ( DWORD i=0; i<dwNumCameraAni; ++i )
	{
		pCur = new DxCameraAni;
		
		HRESULT hr = pCur->Load ( SFile );
		if ( FAILED(hr) )
		{
			SAFE_DELETE(pCur);
			return E_FAIL;
		}

		if ( hr==S_FALSE )
		{
			SAFE_DELETE(pCur);
		}
		else
		{
			//	Note : 리스트에 삽입.
			//
			pCur->m_pNext = m_pAniList;
			m_pAniList = pCur;
		}
	}

	return S_OK;
}

HRESULT DxCamAniMan::ActiveCameraPos ( char* szName )
{
	DxCameraAni *pCurCamera = FindCameraAni ( szName );
	if ( !pCurCamera )	return E_FAIL;

	m_fAgeCamera = 0.0f;
	m_pCurrentAni = NULL;

	DxViewPort::GetInstance().SetCameraType ( pCurCamera->m_emCamera );
	CAMERA_TYPE	 emCamera = DxViewPort::GetInstance().GetCameraType();
	switch ( emCamera )
	{
	case CAMERA_TFREE:
		{
			DxAnimation* pAnimation = pCurCamera->m_pFromCamera;

			LPD3DXMATRIX pmatCur = pAnimation->SetTime ( pCurCamera->m_STime, 0x0 );

			D3DXVECTOR3 vFromPt ( pmatCur->_41, pmatCur->_42, pmatCur->_43 );
			D3DXVECTOR3 vLookatPt ( 0,-40,0 );

			D3DXVec3TransformCoord ( &vLookatPt, &vLookatPt, pmatCur );
			D3DXVECTOR3 vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
		}
		break;

	case CAMERA_TTARGET:
		{
			DxAnimation* pAnimation;
			LPD3DXMATRIX pmatCur;

			D3DXVECTOR3 vFromPt;
			D3DXVECTOR3 vLookatPt;

			pAnimation = pCurCamera->m_pFromCamera;
			pmatCur = pAnimation->SetTime ( 0.0f );

			vFromPt.x = pmatCur->_41;
			vFromPt.y = pmatCur->_42;
			vFromPt.z = pmatCur->_43;

			pAnimation = pCurCamera->m_pTargetCamera;
			pmatCur = pAnimation->SetTime ( 0.0f );

			vLookatPt.x = pmatCur->_41;
			vLookatPt.y = pmatCur->_42;
			vLookatPt.z = pmatCur->_43;

			D3DXVECTOR3 vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
		}
		break;

	case CAMERA_UICONTR:
		break;

	case CAMERA_NULL:
		break;
	};

	return S_OK;
}

HRESULT DxCamAniMan::FrameMove ( float fTime, float fElapsedTime )
{
	CAMERA_TYPE	 emCamera = DxViewPort::GetInstance().GetCameraType();
	if ( !m_pCurrentAni )	return E_FAIL;

	switch ( emCamera )
	{
	case CAMERA_TFREE:
		{
			DxAnimation* pAnimation = m_pCurrentAni->m_pFromCamera;

			//	Note : 에니메이션 시간을 업데이트하고 기간 만료를 점검.
			//
			if ( m_pCurrentAni->m_STime > m_fAgeCamera )	m_fAgeCamera = m_pCurrentAni->m_STime;

			m_fAgeCamera += fElapsedTime*UNITANIKEY_PERSEC;
			if ( m_fAgeCamera > m_pCurrentAni->m_ETime )
			{
				if ( !(CAMERA_ANILOOP&m_pCurrentAni->m_dwFlag) )
				{
					m_fAgeCamera = m_pCurrentAni->m_ETime;

					if ( m_pCurrentAni->m_dwFlag&CAMERA_FREESET)	emCamera = CAMERA_UICONTR;
					else											emCamera = CAMERA_NULL;
					m_pCurrentAni = NULL;
				}
				else
				{
					m_fAgeCamera = m_pCurrentAni->m_STime;
				}
			}

			LPD3DXMATRIX pmatCur = pAnimation->SetTime ( m_fAgeCamera, 0x0 );

			D3DXVECTOR3 vFromPt ( pmatCur->_41, pmatCur->_42, pmatCur->_43 );
			D3DXVECTOR3 vLookatPt ( 0,-40,0 );

			D3DXVec3TransformCoord ( &vLookatPt, &vLookatPt, pmatCur );
			D3DXVECTOR3 vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
		}
		break;

	case CAMERA_TTARGET:
		{
			//	Note : 에니메이션 시간을 업데이트하고 기간 만료를 점검.
			//
			if ( m_pCurrentAni->m_STime > m_fAgeCamera )	m_fAgeCamera = m_pCurrentAni->m_STime;

			m_fAgeCamera += fElapsedTime*UNITANIKEY_PERSEC;
			if ( m_fAgeCamera > m_pCurrentAni->m_ETime )
			{
				if ( !(CAMERA_ANILOOP&m_pCurrentAni->m_dwFlag) )
				{
					m_fAgeCamera = m_pCurrentAni->m_ETime;

					if ( m_pCurrentAni->m_dwFlag&CAMERA_FREESET)	emCamera = CAMERA_UICONTR;
					else											emCamera = CAMERA_NULL;
					m_pCurrentAni = NULL;
				}
				else
				{
					m_fAgeCamera = m_pCurrentAni->m_STime;
				}
			}

			m_fAgeCamera += fElapsedTime;

			DxAnimation* pAnimation;
			LPD3DXMATRIX pmatCur;

			D3DXVECTOR3 vFromPt;
			D3DXVECTOR3 vLookatPt;

			pAnimation = m_pCurrentAni->m_pFromCamera;
			pmatCur = pAnimation->SetTime ( m_fAgeCamera );

			vFromPt.x = pmatCur->_41;
			vFromPt.y = pmatCur->_42;
			vFromPt.z = pmatCur->_43;

			pAnimation = m_pCurrentAni->m_pTargetCamera;
			pmatCur = pAnimation->SetTime ( m_fAgeCamera );

			vLookatPt.x = pmatCur->_41;
			vLookatPt.y = pmatCur->_42;
			vLookatPt.z = pmatCur->_43;

			D3DXVECTOR3 vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
		}
		break;

	case CAMERA_UICONTR:
		break;

	case CAMERA_NULL:
		break;
	};

	if ( emCamera != DxViewPort::GetInstance().GetCameraType() )
		DxViewPort::GetInstance().SetCameraType ( emCamera );

	return S_OK;
}