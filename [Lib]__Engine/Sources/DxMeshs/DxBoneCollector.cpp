#include "pch.h"

#include "./stlfunctions.h"
#include "./StringUtils.h"
#include "./StringFile.h"
#include <algorithm>

#include "./DxMethods.h"
#include "./editmeshs.h"
#include "./DxViewPort.h"
#include "./DxInputDevice.h"
#include "DxSkinDefine.h"

#include "DxBoneCollector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define __ROOT	("__ROOT")
DxBoneCollector& DxBoneCollector::GetInstance()
{
	static DxBoneCollector Instance;
	return Instance;
}

extern DWORD UPDATEBONECOUNT;

D3DXMATRIX	 g_matTemp;
float	DxSkeleton::g_fWeight = 1.f;


void DxBoneTrans::ResetBone ()
{
#ifdef USE_ANI_QUATERNION
	m_dwFlag = 0L;
	m_fScale = 1.f;
	D3DXQuaternionIdentity( &m_vQuatMIX );
	m_pQuatPosCUR = m_pQuatPosORIG;
	m_pQuatPosPREV = m_pQuatPosORIG;

#else
	matRot = matRotOrig;

#endif

	if ( pBoneTransFirstChild != NULL )		pBoneTransFirstChild->ResetBone ();
	if ( pBoneTransSibling != NULL )		pBoneTransSibling->ResetBone ();
}

void DxSkeleton::UpdateBones ( DxBoneTrans *pBoneCur, const D3DXMATRIX &matCur )
{
	++UPDATEBONECOUNT;

#ifndef USE_ANI_QUATERNION

	//	Note : pBoneCur�� ��Ʈ������ ���. matCombined = matRot * matParent * matTrans
	D3DXMatrixMultiply( &pBoneCur->matCombined, &pBoneCur->matRot, &matCur );
	
	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	//
	DxBoneTrans *pBoneChild = pBoneCur->pBoneTransFirstChild;
	while( pBoneChild )
	{
		UpdateBones( pBoneChild, pBoneCur->matCombined );

		pBoneChild = pBoneChild->pBoneTransSibling;
	}

#else

	// Note : Base
	SQuatPos* pQuatPos = &pBoneCur->m_pQuatPosCUR;
	DWORD dwFlag = pBoneCur->m_dwFlag;

	// Note : Animation Blending
	if( dwFlag & USE_ANI_BLENDING )
	{
		SQuatPos* pQuatPosPREV = &pBoneCur->m_pQuatPosPREV;

		// Note : ���� �۾�.
		D3DXVec3Lerp( &pQuatPos->m_vPos, &pQuatPosPREV->m_vPos, &pQuatPos->m_vPos, g_fWeight );
		D3DXVec3Lerp( &pQuatPos->m_vScale, &pQuatPosPREV->m_vScale, &pQuatPos->m_vScale, g_fWeight );
		D3DXQuaternionSlerp( &pQuatPos->m_vQuat, &pQuatPosPREV->m_vQuat, &pQuatPos->m_vQuat, g_fWeight );
	}

	// Note : Ani �� ���ߴٸ�..~!!
	if( dwFlag & USE_ANI_ADD )
	{
		D3DXQuaternionNormalize( &pQuatPos->m_vQuat, &pQuatPos->m_vQuat );
		D3DXQuaternionNormalize( &pBoneCur->m_vQuatMIX , &pBoneCur->m_vQuatMIX  );
		//D3DXQuaternionMultiply( &pQuatPos->m_vQuat, &pQuatPos->m_vQuat, &pBoneCur->m_vQuatMIX );
		D3DXQuaternionMultiply( &pQuatPos->m_vQuat, &pBoneCur->m_vQuatMIX, &pQuatPos->m_vQuat );
	}

	D3DXMatrixRotationQuaternion( &g_matTemp, &pQuatPos->m_vQuat );

	g_matTemp._11 *= pQuatPos->m_vScale.x * pBoneCur->m_fScale;
	g_matTemp._12 *= pQuatPos->m_vScale.x * pBoneCur->m_fScale;
	g_matTemp._13 *= pQuatPos->m_vScale.x * pBoneCur->m_fScale;
	g_matTemp._21 *= pQuatPos->m_vScale.y * pBoneCur->m_fScale;
	g_matTemp._22 *= pQuatPos->m_vScale.y * pBoneCur->m_fScale;
	g_matTemp._23 *= pQuatPos->m_vScale.y * pBoneCur->m_fScale;
	g_matTemp._31 *= pQuatPos->m_vScale.z * pBoneCur->m_fScale;
	g_matTemp._32 *= pQuatPos->m_vScale.z * pBoneCur->m_fScale;
	g_matTemp._33 *= pQuatPos->m_vScale.z * pBoneCur->m_fScale;
	g_matTemp._41 = pQuatPos->m_vPos.x;
	g_matTemp._42 = pQuatPos->m_vPos.y;
	g_matTemp._43 = pQuatPos->m_vPos.z;

	//BYTE* pByte01 = (BYTE*)&pQuatPos->m_vScale;
	///BYTE* pByte02 = (BYTE*)&g_matTemp;

	//__asm
	//{
	//	mov		esi,	   pByte01	   	   // esi <- 
	//	mov		edi,	   pByte02		   	   	   // edi <- ���� �����Ͱ� ����Ǿ��� ������ ������ ����

	//	movss	xmm0,	  [esi]	  	   	   	   	   // xmm0 �� ���ؽ�x~���� ����
	//	movss	xmm1,	  [esi+0x4]	  	   	   	   // xmm1 �� ���ؽ�y~�� ����
	//	movss	xmm2,	  [esi+0x8]	  	   	   	   // xmm2 �� ���ؽ�z~�� ����
	//	shufps	xmm0,	  xmm0, 0x0	  	   	   	   // xmm0�� 4ĭ��, ���ؽ�x �� ä��
	//	shufps	xmm1,	  xmm1, 0x0	  	   	   	   // xmm1�� 4ĭ��, ���ؽ�y �� ä��
	//	shufps	xmm2,	  xmm2, 0x0	  	   	   	   // xmm2�� 4ĭ��, ���ؽ�z �� ä��

	//	movaps	xmm4,	  [edi]
	//	movaps	xmm5,	  [edi+0x10]	 	   // xmm4 �� ��Ʈ���� 21, 22, 23, 24 ����
	//	movaps	xmm6,	  [edi+0x20]	 	   // xmm4 �� ��Ʈ���� 31, 32, 33, 34 ����

	//	mulps	xmm4,	  xmm0	   	   	   	   // ���ؽ� X�� ��Ʈ���� 11, 12, 13, 14 ����
	//	mulps	xmm5,	  xmm1	   	   	   	   // ���ؽ� Y�� ��Ʈ���� 21, 22, 23, 34 ����
	//	mulps	xmm6,	  xmm2	   	   	   	   // ���ؽ� Z�� ��Ʈ���� 31, 32, 33, 34 ����

	//	movaps	[edi],		xmm4
	//	movaps	[edi+0x10],	xmm5
	//	movaps	[edi+0x20],	xmm6

	//	mov		esi,		pByte01
	//	mov		eax,		[esi]
	//	mov		[edi+0x30],	eax
	//	mov		eax,		[esi+0x4]
	//	mov		[edi+0x34],	eax
	//	mov		eax,		[esi+0x8]
	//	mov		[edi+0x38],	eax
	//}	

	//	Note : pBoneCur�� ��Ʈ������ ���. matCombined = matRot * matParent * matTrans
	D3DXMatrixMultiply( &pBoneCur->matCombined, &g_matTemp, &matCur );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	//
	DxBoneTrans *pBoneChild = pBoneCur->pBoneTransFirstChild;
	while( pBoneChild )
	{
		UpdateBones( pBoneChild, pBoneCur->matCombined );

		pBoneChild = pBoneChild->pBoneTransSibling;
	}

#endif
}

void DxSkeleton::CheckSphere( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	DxBoneTrans* pBoneTran = FindBone( szName );
	if( !pBoneTran )	return;

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	vPos.x = pBoneTran->matCombined._41;
	vPos.y = pBoneTran->matCombined._42;
	vPos.z = pBoneTran->matCombined._43;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, 0.006f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
}

void DxSkeleton::EditLineSphere( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EditLineSphere( pd3dDevice, pBoneRoot );
}

void DxSkeleton::EditLineSphere( LPDIRECT3DDEVICEQ pd3dDevice, DxBoneTrans *pBoneCur )
{
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	vPos.x = pBoneCur->matCombined._41;
	vPos.y = pBoneCur->matCombined._42;
	vPos.z = pBoneCur->matCombined._43;

	EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, 0.003f*DxViewPort::GetInstance().GetDistance() );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxBoneTrans *pBoneChild = pBoneCur->pBoneTransFirstChild;
	while ( pBoneChild != NULL )
	{
		D3DXVECTOR3 vPosChild(0.f,0.f,0.f);
		vPosChild.x = pBoneChild->matCombined._41;
		vPosChild.y = pBoneChild->matCombined._42;
		vPosChild.z = pBoneChild->matCombined._43;

		EDITMESHS::RENDERLINE( pd3dDevice, vPos, vPosChild );

		EditLineSphere( pd3dDevice, pBoneChild );
		pBoneChild = pBoneChild->pBoneTransSibling;
	}
}

BOOL DxSkeleton::IsCollision( D3DXVECTOR3& vFrom, D3DXVECTOR3& vLook, std::string& strName, const float fLength )
{
	float fBestDis = FLT_MAX;
	CheckCollision( vFrom, vLook, strName, fLength, fBestDis, pBoneRoot );

	if( strName.size() > 0 )	return TRUE;
	else						return FALSE;
}

void DxSkeleton::CheckCollision( D3DXVECTOR3& vFrom, D3DXVECTOR3& vLook, std::string& strName, const float fLength, float fBestDis, DxBoneTrans *pBoneCur )
{
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	vPos.x = pBoneCur->matCombined._41;
	vPos.y = pBoneCur->matCombined._42;
	vPos.z = pBoneCur->matCombined._43;

	// Note : ���� ��ŷ �ߴ��� üũ
	if( COLLISION::IsCollisionLineToSphere( vFrom, vLook, vPos, fLength ) )
	{
		const D3DXVECTOR3 vDis = vFrom - vPos;
		const float fDis = D3DXVec3Length( &vDis );

		if( fDis < fBestDis )
		{
			strName = pBoneCur->szName;
			fBestDis = fDis;
		}
	}

	// Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxBoneTrans *pBoneChild = pBoneCur->pBoneTransFirstChild;
	while ( pBoneChild != NULL )
	{
		CheckCollision( vFrom, vLook, strName, fLength, fBestDis, pBoneChild );	
		pBoneChild = pBoneChild->pBoneTransSibling;
	}
}

HRESULT DxSkeleton::LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if (szFile == NULL)	return E_INVALIDARG;

	HRESULT hr = S_OK;
	LPDIRECTXFILE pxofapi = NULL;
	LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
	LPDIRECTXFILEDATA pxofobjCur = NULL;
	int cchFileName;

	SAFE_DELETE(pBoneRoot);

	cchFileName = strlen(szFile);
	if (cchFileName < 2)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	SAFE_DELETE_ARRAY(szXFileName);
	int nStrLen = cchFileName+1;
	szXFileName = new char[nStrLen];
	StringCchCopy( szXFileName, nStrLen, szFile );

	//	xFile ��ü�� ����.
	hr = DirectXFileCreate(&pxofapi);
	if (FAILED(hr))
		goto e_Exit;

    //	xFile ���ø�Ʈ ���.
	hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
		D3DRM_XTEMPLATE_BYTES);
	if (FAILED(hr))
		goto e_Exit;

	//	Note : Path xFile ���.
	//
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxBoneCollector::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szXFileName );

    //	���� xFile�� ����.
	hr = pxofapi->CreateEnumObject((LPVOID)szPathName,
		DXFILELOAD_FROMFILE,
		&pxofenum);
	if (FAILED(hr))
		goto e_Exit;

	//	Note : �ֻ��� ������Ʈ�� �о��.
	//
	pBoneRoot = new DxBoneTrans;
	nStrLen = strlen(__ROOT)+1;
	pBoneRoot->szName = new char[nStrLen];
	StringCchCopy( pBoneRoot->szName, nStrLen, __ROOT );
	while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
	{
		const GUID *type;

		hr = pxofobjCur->GetType(&type);
		if (FAILED(hr))	goto e_Exit;

		//	Note : ���� �����´�.
		//
		if ( *type == TID_D3DRMFrame )
		{
			hr = LoadBoneTrans ( pBoneRoot, pxofobjCur, EMBODY_DEFAULT );
			if (FAILED(hr))	goto e_Exit;
		}

		GXRELEASE(pxofobjCur);
	}

e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);

	return hr;
}

HRESULT DxSkeleton::LoadBoneTrans ( PDXBONETRANS &pBone, LPDIRECTXFILEDATA pxofobjCur, EMBODY emBody )
{
	HRESULT hr = S_OK;
	LPDIRECTXFILEDATA pxofobjChild = NULL;
	LPDIRECTXFILEOBJECT pxofChild = NULL;
	const GUID *type;
	DWORD cbSize;
	D3DXMATRIX *pmatNew;
	PDXBONETRANS pBoneTransCur;
	DWORD cchName;
    
	//	Note : ������Ʈ�� Ÿ��.
	//
	hr = pxofobjCur->GetType(&type);
	if (FAILED(hr))	goto e_Exit;
    
    //	Note : Ʈ������.
	//
	if (*type == TID_D3DRMFrameTransformMatrix)
	{
		hr = pxofobjCur->GetData ( NULL, &cbSize, (PVOID*)&pmatNew );
		if (FAILED(hr))	goto e_Exit;
        
		//// Normal ���� �������ͷ� ������Ű�� ���� �۾�
		//D3DXMATRIX	sM = *pmatNew;
		//float fLength = sM._11*(sM._22*sM._33-sM._23*sM._32) - sM._12*(sM._21*sM._33-sM._23*sM._31) + sM._13*(sM._21*sM._32-sM._22*sM._31);
		//fLength = 1.f/fLength;
		//*pmatNew *= fLength;

		//	update the parents matrix with the new one
		pBone->matRot = pBone->matRotOrig = *pmatNew;
		D3DXMatrixToSQT( pBone->m_pQuatPosCUR.m_vScale, pBone->m_pQuatPosCUR.m_vQuat, pBone->m_pQuatPosCUR.m_vPos, pBone->matRot );
		pBone->m_pQuatPosPREV = pBone->m_pQuatPosORIG = pBone->m_pQuatPosCUR;
	}
	//	Note : ���� ������.
	//
	else if (*type == TID_D3DRMFrame)
	{
		char* szFrameName = NULL;

		//	Note : �������� �̸��� �ִ��� �˻�.
		//
		hr = pxofobjCur->GetName(NULL, &cchName);
		if ( SUCCEEDED(hr) && cchName!=0 )
		{
			szFrameName = (char*) _alloca(sizeof(char)*(cchName+1));
			hr = pxofobjCur->GetName ( szFrameName, &cchName );
			if (FAILED(hr))	goto e_Exit;
		}

		// Note : �̸��� ���ٸ� �۾� �� ����.
		if( !szFrameName )	goto e_Exit;
        
		//	Note : �� ������ ����.
		//
		pBoneTransCur = new DxBoneTrans();
		if (pBoneTransCur == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		pBoneTransCur->SetName ( szFrameName );

		// ��ü���� ��ü���� �����Ѵ�. ( �θ� ��ü�� �ڽĵ� ��ü�̴�. )
		switch( emBody )
		{
		case EMBODY_DEFAULT:
			if( strcmp(szFrameName,"Bip01_Spine1")==0 )
			{
				pBoneTransCur->m_emBody = EMBODY_UPBODY;
			}
			else
			{
				pBoneTransCur->m_emBody = EMBODY_DEFAULT;
			}
			break;

		case EMBODY_UPBODY:
			pBoneTransCur->m_emBody = EMBODY_UPBODY;
			break;
		};

		BoneMap.insert ( std::make_pair(std::string(pBoneTransCur->szName),pBoneTransCur) );
		pBone->AddBoneTrans(pBoneTransCur);

		//	Note : ���������� Ž���ϸ鼭 ���ϸ��̼��� �о�´�.
		//		QueryInterface()�� ����Ͽ� ������Ʈ�� Ÿ���� Ȯ����.
		//
		while ( SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)) )
		{
			hr = pxofChild->QueryInterface(IID_IDirectXFileData,
					(LPVOID *)&pxofobjChild);

			if (SUCCEEDED(hr))
			{
				hr = LoadBoneTrans ( pBoneTransCur, pxofobjChild, pBoneTransCur->m_emBody );
				if (FAILED(hr))		goto e_Exit;

				GXRELEASE(pxofobjChild);
            }

			GXRELEASE(pxofChild);
		}

	}

e_Exit:
	GXRELEASE(pxofobjChild);
	GXRELEASE(pxofChild);

	return S_OK;
}

DxBoneCollector::DxBoneCollector(void)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxBoneCollector::~DxBoneCollector(void)
{
	CleanUp ();
}

void DxBoneCollector::OneTimeSceneInit ( char* szPath )
{
	StringCchCopy( m_szPath, MAX_PATH, szPath );
}

void DxBoneCollector::CleanUp ()
{
	std::for_each ( m_MapBone.begin(), m_MapBone.end(), std_afunc::DeleteMapObject() );
	m_MapBone.clear ();
}

DxSkeleton* DxBoneCollector::Find ( const char* szFile )
{
	BONEMAP_ITER iter = m_MapBone.find ( std::string(szFile) );
	if ( iter!=m_MapBone.end() )	return iter->second;

	return NULL;
}

DxSkeleton* DxBoneCollector::Load ( const char* _szFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxSkeleton* pSkeleton;

	CString strFile(_szFile);
	strFile.MakeLower();

	//	Note : �̹� �ε�� ���ٱ����� �˻��� ó��.
	//
	pSkeleton = Find ( strFile.GetString() );
	if ( pSkeleton )
	{
		pSkeleton->dwRefCount++;
		return pSkeleton;
	}

	//CDebugSet::ToListView ( "DxSkeleton '%s' Load", strFile.GetString() );

	//	Note : ���ο� ���ٱ� ����.
	//
	pSkeleton = new DxSkeleton;
	hr = pSkeleton->LoadFile ( strFile.GetString(), pd3dDevice );
	if ( FAILED(hr) )	return NULL;
	pSkeleton->dwRefCount++;
	
	//	Note : ����Ʈ�� ����.
	//
	m_MapBone[std::string(pSkeleton->szXFileName)] = pSkeleton;

	return pSkeleton;
}

BOOL DxBoneCollector::PreLoad ( const char* szBoneList, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CString strPath;
	strPath = GetPath();
	strPath += szBoneList;	

	CStringFile StrFile;
	if ( !StrFile.Open ( strPath.GetString() ) )	return FALSE;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );

	CString strLine;
	while ( StrFile.GetNextLine(strLine) )
	{
		if ( strLine.GetLength() == 0 )		continue;

		CStringArray *pStrArray = new CStringArray;
		STRUTIL::StringSeparate ( strLine, *pStrArray );

		if ( pStrArray->GetSize() != 1 )
		{
			SAFE_DELETE(pStrArray);
			continue;
		}

		CString strBone = pStrArray->GetAt(0);
		strBone.MakeLower();

		DxSkeleton* pSkeleton = Load ( strBone.GetString(), pd3dDevice );
		if ( !pSkeleton )
		{
			CString strMessage;
			strMessage.Format ( "DxBoneCollector::PreLoad() [%s] Load�� �����Ͽ����ϴ�.", strBone );
			MessageBox ( NULL, strMessage, "ERROR", MB_OK );

			SAFE_DELETE(pStrArray);
			continue;
		}

		m_PreLoadBoneMap.insert ( std::make_pair(std::string(strBone.GetString()),pSkeleton) );

		SAFE_DELETE(pStrArray);
	};

	return TRUE;
}

HRESULT DxBoneCollector::DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BONELIST listInterimClean;
	
	//	Note : ������ ����Ʈ �ۼ�.
	//
	{
		BONEMAP_ITER iter = m_MapBone.begin();
		BONEMAP_ITER iter_end = m_MapBone.end();
		for ( ; iter!=iter_end; ++iter )
		{
			//	Note : ���� �ε� ����Ʈ�� �ִ� ���� ����.
			//
			BONEMAP_ITER found = m_PreLoadBoneMap.find ( (*iter).first );
			if ( found!=m_PreLoadBoneMap.end() )	continue;

			//	���� ����Ʈ�� ���.
			listInterimClean.push_back ( (*iter).second );
		}
	}

	//	Note : ������ ����.
	//
	{
		BONELIST_ITER iter = listInterimClean.begin();
		BONELIST_ITER iter_end = listInterimClean.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DxSkeleton* pBone = (*iter);
			std::string strname = pBone->szXFileName;

			//	������ ����.
			SAFE_DELETE(pBone);

			//	�ε� ���� ����Ʈ���� ����.
			BONEMAP_ITER iter_del = m_MapBone.find ( strname );
			if ( iter_del!=m_MapBone.end() )		m_MapBone.erase ( iter_del );
		}
	}

	return S_OK;
}

