#include "pch.h"

#include "./DxMethods.h"
#include "./EDITMESHS.h"
#include "../NaviMesh/NavigationMesh.h"
#include "./SerialFile.h"
#include "basestream.h"
#include "./StlFunctions.h"

#include <algorithm>
#include "DxLandGateMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL DxLandGateMan::m_bREND = TRUE;

void DxLandGate::SetGate ( DWORD dwFlags, DWORD GateID, SNATIVEID ToMapID, DWORD ToGateID, const D3DXVECTOR2 &vDiv )
{
	m_dwGeteFlags = dwFlags;
	m_GateID = GateID;
	m_ToMapID = ToMapID;
	m_ToGateID = ToGateID;
	m_vDiv = vDiv;
}

void DxLandGate::SetPos ( const D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vSize = m_vMax - m_vMin;

	m_vMax = vPos + vSize/2;
	m_vMin = vPos - vSize/2;
}

void DxLandGate::SetBox ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	GASSERT(vMax.x>=vMin.x);
	GASSERT(vMax.y>=vMin.y);
	GASSERT(vMax.z>=vMin.z);

	m_vMax = vMax;
	m_vMin = vMin;
}

D3DXVECTOR3 DxLandGate::GetGenPos ( DxLandGate::EMGENPOS_SEL emGenSel )
{
	if ( m_dwStartPNum==0 )		return D3DXVECTOR3(0,0,0);

	DWORD dwGenPos = 0;
	switch ( emGenSel )
	{
	case GEN_FIRST:
		{
			dwGenPos = 0;
		}
		break;

	case GEN_RENDUM:
		{
			if ( ++m_dwLastUsedPos < m_dwStartPNum )
			{
				dwGenPos = m_dwLastUsedPos;
			}
			else
			{
				dwGenPos = m_dwLastUsedPos = 0;
			}
		}
		break;
	};

	return m_pvStartPos[dwGenPos];
}

void DxLandGate::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	D3DXMatrixAABBTransform( m_vMax, m_vMin, matRotate );

	for ( DWORD i=0; i<m_dwStartPNum; i++ )
	{
		D3DXVec3TransformCoord( &m_pvStartPos[i], &m_pvStartPos[i], &matRotate );
	}
}

HRESULT DxLandGate::Load ( basestream	&SFile )
{
	DWORD dwVer, dwSize;
	SFile >> dwVer;
	SFile >> dwSize;

	if ( VERSION == dwVer )
	{
		SFile.ReadBuffer ( &m_Prop, sizeof(PROPERTY) );
		
		if ( m_Prop.m_dwStartPNum != 0 )
		{
			m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
			SFile.ReadBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
	}

	return S_OK;
}

HRESULT DxLandGate::Save ( CSerialFile	&SFile )
{
	DWORD dwSize = sizeof(PROPERTY) + sizeof(DWORD) + sizeof(D3DXVECTOR3)*m_dwStartPNum;

	SFile << VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Prop, sizeof(PROPERTY) );
		
	if ( m_Prop.m_dwStartPNum != 0 )
	{
		SFile.WriteBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
	}

	return S_OK;
}

void DxLandGate::operator= ( DxLandGate &LandGate )
{
	m_Prop = LandGate.m_Prop;

	SAFE_DELETE_ARRAY(m_pvStartPos);
	m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
	memcpy ( m_pvStartPos, LandGate.m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
}

BOOL DxLandGate::MakeStartPos ( NavigationMesh* pNaviMesh )
{
	GASSERT(pNaviMesh);
	SAFE_DELETE_ARRAY(m_pvStartPos);

	D3DXVECTOR3 vTestUp;
	D3DXVECTOR3 vTestDown;

	D3DXVECTOR3 vCollision;
	DWORD dwCollisionID;
	BOOL bCollision;
	D3DXVECTOR3 vCenter = (m_vMax+m_vMin)/2;

	m_dwStartPNum = 0;
	std::vector<D3DXVECTOR3> PosArray;
	for ( float fX=(m_vMin.x+m_vDiv.x); fX<(m_vMax.x-m_vDiv.x); fX+=m_vDiv.x )
	for ( float fZ=(m_vMin.z+m_vDiv.y); fZ<(m_vMax.z-m_vDiv.y); fZ+=m_vDiv.y )
	{
		vTestUp = D3DXVECTOR3(fX,m_vMax.y,fZ);
		vTestDown = D3DXVECTOR3(fX,-FLT_MAX,fZ);

		pNaviMesh->IsCollision ( vTestUp, vTestDown, vCollision, dwCollisionID, bCollision );
		if ( bCollision )
		{
			PosArray.push_back ( vCollision );
			m_dwStartPNum++;
		}
	}
	
	if ( m_dwStartPNum == 0 )
	{
		vTestUp = D3DXVECTOR3(vCenter.x, m_vMax.y, vCenter.z );
		vTestDown = D3DXVECTOR3(vCenter.x, -FLT_MAX, vCenter.z );

		pNaviMesh->IsCollision ( vTestUp, vTestDown, vCollision, dwCollisionID, bCollision );
		if ( bCollision )
		{
			m_pvStartPos = new D3DXVECTOR3[1];
			m_pvStartPos[0] = vCollision;
			m_dwStartPNum = 1;
		}
		else	return FALSE;
	}
	else
	{
		//	Note : PosArray 에 있는 위치를 정렬후 배열에 복사 작업.
		//
		m_pvStartPos = new D3DXVECTOR3[m_dwStartPNum];
		if ( m_dwStartPNum > 1 )
		{
			std_afunc::CCompare Comp(vCenter);
			std::sort ( PosArray.begin(), PosArray.end(), Comp );

			for ( DWORD j=0; j<m_dwStartPNum; j++ )
				m_pvStartPos[j] = *(PosArray.begin()+j);
		}
	}

	return TRUE;
}

HRESULT DxLandGate::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );

	for ( DWORD i=0; i<m_dwStartPNum; i++ )
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_pvStartPos[i], 2.0f );
	}

	return S_OK;
}

DxLandGateMan::DxLandGateMan(void) :
	m_dwNumLandGate(0),
	m_pLandGateList(NULL),
	m_pLandGateTree(NULL)
{
}

DxLandGateMan::~DxLandGateMan(void)
{
	CleanUp ();
}

void DxLandGateMan::AddLandGate ( PDXLANDGATE pLandGate )
{
	m_dwNumLandGate++;
	pLandGate->m_pNext = m_pLandGateList;
	m_pLandGateList = pLandGate;
}

void DxLandGateMan::DelLandGate ( char* szName )
{
	GASSERT(szName);
	PDXLANDGATE pCur, pBack;

	if ( !strcmp(m_pLandGateList->GetName(),szName) )
	{
		pCur = m_pLandGateList;
		m_pLandGateList = m_pLandGateList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandGate--;
		return;
	}

	pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( !strcmp(pCur->GetName(),szName) )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandGate--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PDXLANDGATE DxLandGateMan::FindLandGate ( const char* szName )
{
	GASSERT(szName);

	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( !strcmp(szName,pCur->GetName()) )	return pCur;
		pCur = pCur->m_pNext;
	}

	return NULL;
}

PDXLANDGATE DxLandGateMan::FindLandGate ( DWORD dwGateID )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( dwGateID == pCur->GetGateID() )	return pCur;
		pCur = pCur->m_pNext;
	}

	return NULL;
}

void DxLandGateMan::BuildTree ()
{
	COLLISION::MakeAABBTree ( m_pLandGateTree, m_pLandGateList );
}

void DxLandGateMan::SetObjRotate90()
{
	PDXLANDGATE pCur = m_pLandGateList;
	while( pCur )
	{
		pCur->SetObjRotate90();
		pCur = pCur->m_pNext;
	}
	COLLISION::MakeAABBTree( m_pLandGateTree, m_pLandGateList );
}

PDXLANDGATE DxLandGateMan::DetectGate ( const D3DXVECTOR3 &vPos )
{
	if ( !m_pLandGateTree )		return NULL;

	PDXLANDGATE pLandGate = (PDXLANDGATE) COLLISION::IsCollisonPointToOBJAABBTree ( m_pLandGateTree, vPos );

	if ( pLandGate )	return pLandGate;
	return NULL;
}

void DxLandGateMan::CleanUp ()
{
	m_dwNumLandGate = 0;
	SAFE_DELETE(m_pLandGateList);
	SAFE_DELETE(m_pLandGateTree);
}

HRESULT DxLandGateMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		pCur->Render ( pd3dDevice );
		pCur = pCur->m_pNext;
	}

	return S_OK;
}

VOID DxLandGateMan::SaveCsvHead( std::fstream &SFile )
{
	SFile << "Gate Name" << ",";
	SFile << "Gate ID" << ",";
	SFile << "Type" << ",";
	SFile << "ToMapMID" << ",";
	SFile << "ToMapSID" << ",";
	SFile << "ToGate" << ",";
	SFile << "SizeA" << ",";
	SFile << "SizeB" << ",";
}

VOID DxLandGateMan::SaveCsv( std::fstream &SFile )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		SFile << pCur->GetName() << ",";
		SFile << pCur->GetGateID() << ",";

		SFile << pCur->GetFlags() << ",";

		SFile << pCur->GetToMapID().wMainID << ",";
		SFile << pCur->GetToMapID().wSubID << ",";

		SFile << pCur->GetToGateID() << ",";

		SFile << pCur->GetDiv().x << ",";
		SFile << pCur->GetDiv().y << ",";
		SFile << std::endl;

		pCur = pCur->m_pNext;
	};
}

VOID DxLandGateMan::LoadCsv( CStringArray &StrArray )
{
	PDXLANDGATE pCur = new DxLandGate;

	int			iCsvCur = 0;
	DWORD	    dwFlags;
	DWORD	    dwGateID;
	SNATIVEID   sToMapID;
	DWORD	    dwToGateID;
	D3DXVECTOR2 vDiv;

	pCur->SetName( StrArray[ iCsvCur++ ] );

	dwGateID = (DWORD)atoi( StrArray[ iCsvCur++ ] );
	dwFlags  = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	sToMapID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	sToMapID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );

	dwToGateID = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	vDiv.x = (float)atof( StrArray[ iCsvCur++ ] );
	vDiv.y = (float)atof( StrArray[ iCsvCur++ ] );

	pCur->SetGate( dwFlags, dwGateID, sToMapID, dwToGateID, vDiv );


	AddLandGate ( pCur );

}


HRESULT DxLandGateMan::Load ( basestream &SFile )
{
	BOOL bExist = FALSE;
	CleanUp ();

	SFile >> bExist;
	while ( bExist )
	{
		PDXLANDGATE pCur = new DxLandGate;
		pCur->Load ( SFile );
	
		AddLandGate ( pCur );

		SFile >> bExist;
	}

	BuildTree ();

	return S_OK;
}

HRESULT DxLandGateMan::Save ( CSerialFile	&SFile )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		SFile << TRUE;
		pCur->Save ( SFile );

		pCur = pCur->m_pNext;
	};
	
	SFile << FALSE;

	return S_OK;
}

DxLandGateMan& DxLandGateMan::operator= ( DxLandGateMan &LandGateMan )
{
	CleanUp ();

	PDXLANDGATE pCur = LandGateMan.m_pLandGateList;
	while ( pCur )
	{
		DxLandGate *pNew = new DxLandGate;
		*pNew = *pCur;
		AddLandGate ( pNew );

		pCur = pCur->m_pNext;
	}

	BuildTree();

	return *this;
}

