#include "pch.h"

#include "EDITMESHS.h"
#include "Collision.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace COLLISION
{
	HRESULT GetSizeNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		DWORD dwFace;
		D3DXVECTOR3 vDist;
		D3DXVECTOR3 *pvCur;
		WORD *pwIndex;
		
		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );

		for ( dwFace=0; dwFace < dwNumFaces; dwFace++ )
		{
			pwIndex = pIndices + pwFaces[dwFace]*3;
			pvCur = (D3DXVECTOR3*) ( pbPoints + fvfsize * (*(pwIndex)) );
			D3DXVec3TransformCoord ( &vDist, pvCur, &matComb );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;

			pvCur = (D3DXVECTOR3*) ( pbPoints + fvfsize * (*(pwIndex+1)) );
			D3DXVec3TransformCoord ( &vDist, pvCur, &matComb );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;

			pvCur = (D3DXVECTOR3*) ( pbPoints + fvfsize * (*(pwIndex+2)) );
			D3DXVec3TransformCoord ( &vDist, pvCur, &matComb );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;
		}

		//	Note : 충돌처리시 유격 필요.
		//
		vMax.x += 0.0001f;
		vMin.x -= 0.0001f;
		vMax.y += 0.0001f;
		vMin.y -= 0.0001f;
		vMax.z += 0.0001f;
		vMin.z -= 0.0001f;

		return S_OK;
	}

	BOOL IsWithInTriangle ( D3DXMATRIX &matComb,
							LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3,
							float fDivision, DWORD dwAxis )
	{
		D3DXVECTOR3 vDist;
		D3DXVECTOR3 vCenter;

		D3DXVec3TransformCoord ( &vDist, pvT1, &matComb );
		vCenter = vDist;
		D3DXVec3TransformCoord ( &vDist, pvT2, &matComb );
		vCenter += vDist;
		D3DXVec3TransformCoord ( &vDist, pvT3, &matComb );
		vCenter += vDist;

		vCenter /= 3;

		switch ( dwAxis )
		{
		case 0:
			if (vCenter.x <= fDivision )	return TRUE;
			break;
		
		case 1:
			if (vCenter.y <= fDivision )	return TRUE;
			break;
		
		case 2:
			if (vCenter.z <= fDivision )	return TRUE;
			break;
		};

		return FALSE;
	}

	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		PWORD pwIndex;
		DWORD dwFaceCur;
		D3DXVECTOR3 vDist;

		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );

		for ( dwFaceCur=0; dwFaceCur<dwNumFaces; dwFaceCur++ )
		{
			pwIndex = pIndices + pwFaces[dwFaceCur]*3;
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex)));
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+1)));
			LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+2)));

			D3DXVECTOR3 vCenter;

			D3DXVec3TransformCoord ( &vDist, pvT1, &matComb );
			vCenter = vDist;
			D3DXVec3TransformCoord ( &vDist, pvT2, &matComb );
			vCenter += vDist;
			D3DXVec3TransformCoord ( &vDist, pvT3, &matComb );
			vCenter += vDist;

			vCenter /= 3;

			if ( vDist.x > vMax.x )	vMax.x = vCenter.x;
			if ( vDist.y > vMax.y )	vMax.y = vCenter.y;
			if ( vDist.z > vMax.z )	vMax.z = vCenter.z;

			if ( vDist.x < vMin.x )	vMin.x = vCenter.x;
			if ( vDist.y < vMin.y )	vMin.y = vCenter.y;
			if ( vDist.z < vMin.z )	vMin.z = vCenter.z;
		}

		//	Note : 충돌 처리시 유격 필요.
		//
		vMax.x += 0.0001f;
		vMin.x -= 0.0001f;
		vMax.y += 0.0001f;
		vMin.y -= 0.0001f;
		vMax.z += 0.0001f;
		vMin.z -= 0.0001f;

		return S_OK;
	}

	void CalcAxisTable ( float SizeX, float SizeY, float SizeZ, PDWORD pdwAxis )
	{
		if ( SizeX >= SizeY && SizeX >= SizeZ )
		{
			pdwAxis[0] = 0;

			if ( SizeY >= SizeZ )
			{
				pdwAxis[1] = 1;
				pdwAxis[2] = 2;
			}
			else
			{
				pdwAxis[1] = 2;
				pdwAxis[2] = 1;
			}
		}
		else if ( SizeY >= SizeX && SizeY >= SizeZ )
		{
			pdwAxis[0] = 1;

			if ( SizeX >= SizeZ )
			{
				pdwAxis[1] = 0;
				pdwAxis[2] = 2;
			}
			else
			{
				pdwAxis[1] = 2;
				pdwAxis[2] = 0;
			}
		}
		else
		{
			pdwAxis[0] = 2;

			if ( SizeX >= SizeY )
			{
				pdwAxis[1] = 0;
				pdwAxis[2] = 1;
			}
			else
			{
				pdwAxis[1] = 1;
				pdwAxis[2] = 0;
			}
		}
	}

	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb,
										WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
										WORD *pwFaces, DWORD dwNumFaces,
										D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		HRESULT hr = S_OK;

		//	Note : 페이스가 하나이라면.
		//
		if ( dwNumFaces == 1 )
		{
			pNode->fMaxX = vMax.x;
			pNode->fMaxY = vMax.y;
			pNode->fMaxZ = vMax.z;
			pNode->fMinX = vMin.x;
			pNode->fMinY = vMin.y;
			pNode->fMinZ = vMin.z;
			
			pNode->dwFace = pwFaces[0];

			return S_OK;
		}

		//	Note : 두개의 부분 집합으로 나눌 공간 분할.
		//
		DWORD dwAxis[3];
		float fDivision;
		float SizeX = vMax.x - vMin.x;
		float SizeY = vMax.y - vMin.y;
		float SizeZ = vMax.z - vMin.z;

		COLLISION::CalcAxisTable ( SizeX, SizeY, SizeZ, dwAxis );

		//	Note : 분할된 공간에 값을 넣는다.
		//
		int i = 0;
		WORD *pwIndex=NULL;
		WORD *pBox1Faces=NULL, *pBox2Faces=NULL;
		WORD wFaceCur;
		DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );
		DWORD nNumBox1Face=0, nNumBox2Face=0;
		D3DXVECTOR3 vB1Max, vB1Min;
		D3DXVECTOR3 vB2Max, vB2Min;

		pBox1Faces = new WORD[dwNumFaces];
		pBox2Faces = new WORD[dwNumFaces];
		if ( pBox1Faces==NULL || pBox2Faces==NULL )	goto e_failed;

		for ( i=0; i<3; i++ )
		{
			nNumBox1Face = 0;
			nNumBox2Face = 0;

			switch ( dwAxis[i] )
			{
			case 0:
				fDivision = vMin.x+SizeX/2;
				break;

			case 1:
				fDivision = vMin.y+SizeY/2;
				break;

			case 2:
				fDivision = vMin.z+SizeZ/2;
				break;
			};

			for ( wFaceCur=0; wFaceCur<dwNumFaces; wFaceCur++ )
			{
				pwIndex = pIndices + pwFaces[wFaceCur]*3;
				LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex)));
				LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+1)));
				LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+2)));

				if ( COLLISION::IsWithInTriangle(matComb,pvT1,pvT2,pvT3,fDivision,dwAxis[i]) )
				{
					pBox1Faces[nNumBox1Face] = pwFaces[wFaceCur];
					nNumBox1Face++;
				}
				else
				{
					pBox2Faces[nNumBox2Face] = pwFaces[wFaceCur];
					nNumBox2Face++;
				}
			}
			
			if ( nNumBox1Face!=0 && nNumBox2Face!=0 )
				break;
		}

		//	Note : 분할된 AABB중 한곳이 비여버리면 삼각형의 중심차로 분할.
		//		삼각형의 중심이 한쪽 사면에 편중되었을 경우에...
		//
		if ( nNumBox1Face==0 || nNumBox2Face==0 )
		{
			D3DXVECTOR3 vCMax, vCMin;
			COLLISION::GetCenterDistNode ( matComb, pIndices, pbPoints, dwFVF, pwFaces, dwNumFaces, vCMax, vCMin );

			SizeX = vCMax.x - vCMin.x;
			SizeY = vCMax.y - vCMin.y;
			SizeZ = vCMax.z - vCMin.z;

			COLLISION::CalcAxisTable ( SizeX, SizeY, SizeZ, dwAxis );

			for ( i=0; i<3; i++ )
			{
				nNumBox1Face = 0;
				nNumBox2Face = 0;

				switch ( dwAxis[i] )
				{
				case 0:
					fDivision = vCMin.x+SizeX/2;
					break;

				case 1:
					fDivision = vCMin.y+SizeY/2;
					break;

				case 2:
					fDivision = vCMin.z+SizeZ/2;
					break;
				};

				for ( wFaceCur=0; wFaceCur<dwNumFaces; wFaceCur++ )
				{
					pwIndex = pIndices + pwFaces[wFaceCur]*3;
					LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex)));
					LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+1)));
					LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) (pbPoints+fvfsize*(*(pwIndex+2)));

					if ( COLLISION::IsWithInTriangle(matComb,pvT1,pvT2,pvT3,fDivision,dwAxis[i]) )
					{
						pBox1Faces[nNumBox1Face] = pwFaces[wFaceCur];
						nNumBox1Face++;
					}
					else
					{
						pBox2Faces[nNumBox2Face] = pwFaces[wFaceCur];
						nNumBox2Face++;
					}
				}
				
				if ( nNumBox1Face!=0 && nNumBox2Face!=0 )
					break;
			}
		}

		//	Note : 분할된 AABB중 한곳이 비여 버리면 임의로 분할.
		//		여기에 걸릴 가능성은 거의 없다. 최후의 보루.
		//
		if ( nNumBox1Face==0 || nNumBox2Face==0 )
		{
			nNumBox1Face = 0;
			nNumBox2Face = 0;

			for ( wFaceCur=0; wFaceCur<(dwNumFaces/2); wFaceCur++ )
			{
				pBox1Faces[nNumBox1Face] = pwFaces[wFaceCur];
				nNumBox1Face++;
			}
			
			for ( wFaceCur=WORD(dwNumFaces/2); wFaceCur<dwNumFaces; wFaceCur++ )
			{
				pBox2Faces[nNumBox2Face] = pwFaces[wFaceCur];
				nNumBox2Face++;
			}
		}

		//	Note : 분할 AABB 자식으로.
		//
		if ( nNumBox1Face )
		{
			hr = COLLISION::GetSizeNode ( matComb, pIndices, pbPoints, dwFVF, pBox1Faces, nNumBox1Face, vB1Max, vB1Min );
			if ( FAILED ( hr ) )
				goto e_failed;

			DxAABBNode *pNodeChild = new DxAABBNode;
			pNodeChild->fMaxX = vB1Max.x;
			pNodeChild->fMaxY = vB1Max.y;
			pNodeChild->fMaxZ = vB1Max.z;
			pNodeChild->fMinX = vB1Min.x;
			pNodeChild->fMinY = vB1Min.y;
			pNodeChild->fMinZ = vB1Min.z;

			pNode->pLeftChild = pNodeChild;

			hr = MakeAABBNode ( pNodeChild, matComb, pIndices, pbPoints, dwFVF, pBox1Faces, nNumBox1Face, vB1Max, vB1Min );
			if ( FAILED ( hr ) )
				goto e_failed;
		}

		if ( nNumBox2Face )
		{
			hr = COLLISION::GetSizeNode ( matComb, pIndices, pbPoints, dwFVF, pBox2Faces, nNumBox2Face, vB2Max, vB2Min );
			if ( FAILED ( hr ) )
				goto e_failed;

			DxAABBNode *pNodeChild = new DxAABBNode;
			pNodeChild->fMaxX = vB2Max.x;
			pNodeChild->fMaxY = vB2Max.y;
			pNodeChild->fMaxZ = vB2Max.z;
			pNodeChild->fMinX = vB2Min.x;
			pNodeChild->fMinY = vB2Min.y;
			pNodeChild->fMinZ = vB2Min.z;

			pNode->pRightChild = pNodeChild;

			hr = MakeAABBNode ( pNodeChild, matComb, pIndices, pbPoints, dwFVF, pBox2Faces, nNumBox2Face, vB2Max, vB2Min );
			if ( FAILED ( hr ) )
				goto e_failed;
		}

	e_failed:
		SAFE_DELETE_ARRAY(pBox1Faces);
		SAFE_DELETE_ARRAY(pBox2Faces);

		return hr;
	}


	void CollisionLineToTreePARENT ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor )
	{
		if ( !pTree )	return;

		if ( !IsCollisionLineToAABB ( vStart, vEnd, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->pObject )
		{
			pTree->pObject->IsCollisionLine(vStart,vEnd,vColl,vNor,pOBJ);// pOBJ = pTree->pObject;
			return;
		}

		OBJAABB*	pOBJ_1	= NULL;
		D3DXVECTOR3	vColl_1	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3	vNor_1	= D3DXVECTOR3 ( 0.f,1.f,0.f );
		OBJAABB*	pOBJ_2	= NULL;
		D3DXVECTOR3	vColl_2	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3	vNor_2	= D3DXVECTOR3 ( 0.f,1.f,0.f );

		if ( pTree->pLeftChild )	CollisionLineToTreePARENT ( pTree->pLeftChild, vStart, vEnd, pOBJ_1, vColl_1, vNor_1 );
		if ( pTree->pRightChild )	CollisionLineToTreePARENT ( pTree->pRightChild, vStart, vEnd, pOBJ_2, vColl_2, vNor_2 );

		// 데이터를 정리하여서 넘긴다.
		if ( pOBJ_1 && pOBJ_2 )
		{
			D3DXVECTOR3 vLength_1 = vColl_1-vStart;
			D3DXVECTOR3 vLength_2 = vColl_2-vStart;
			float		fLength_1 = D3DXVec3Length ( &vLength_1 );
			float		fLength_2 = D3DXVec3Length ( &vLength_2 );

			if ( fLength_1 <= fLength_2 )
			{
				pOBJ = pOBJ_1;
				vColl = vColl_1;
				vNor = vNor_1;
			}
			else
			{
				pOBJ = pOBJ_2;
				vColl = vColl_2;
				vNor = vNor_2;
			}
		}
		else if ( pOBJ_1 )
		{
			pOBJ = pOBJ_1;
			vColl = vColl_1;
			vNor = vNor_1;
		}
		else if ( pOBJ_2 )
		{
			pOBJ = pOBJ_2;
			vColl = vColl_2;
			vNor = vNor_2;
		}
		else
		{
			pOBJ = NULL;
			vColl = D3DXVECTOR3 ( 0.f,0.f,0.f );
			vNor = D3DXVECTOR3 ( 0.f,1.f,0.f );
		}
	}

	void CollisionLineToTree ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor )
	{
		if ( !pTree )	return;

		if ( !IsCollisionLineToAABB ( vStart, vEnd, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->pObject )
		{
			if ( pTree->pObject->IsCollisionLine(vStart,vEnd,vColl,vNor) ) pOBJ = pTree->pObject;
			return;
		}

		OBJAABB*	pOBJ_1	= NULL;
		D3DXVECTOR3	vColl_1	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3	vNor_1	= D3DXVECTOR3 ( 0.f,1.f,0.f );
		OBJAABB*	pOBJ_2	= NULL;
		D3DXVECTOR3	vColl_2	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3	vNor_2	= D3DXVECTOR3 ( 0.f,1.f,0.f );

		if ( pTree->pLeftChild )	CollisionLineToTree ( pTree->pLeftChild, vStart, vEnd, pOBJ_1, vColl_1, vNor_1 );
		if ( pTree->pRightChild )	CollisionLineToTree ( pTree->pRightChild, vStart, vEnd, pOBJ_2, vColl_2, vNor_2 );

		// 데이터를 정리하여서 넘긴다.
		if ( pOBJ_1 && pOBJ_2 )
		{
			D3DXVECTOR3 vLength_1 = vColl_1-vStart;
			D3DXVECTOR3 vLength_2 = vColl_2-vStart;
			float		fLength_1 = D3DXVec3Length ( &vLength_1 );
			float		fLength_2 = D3DXVec3Length ( &vLength_2 );

			if ( fLength_1 <= fLength_2 )
			{
				pOBJ = pOBJ_1;
				vColl = vColl_1;
				vNor = vNor_1;
			}
			else
			{
				pOBJ = pOBJ_2;
				vColl = vColl_2;
				vNor = vNor_2;
			}
		}
		else if ( pOBJ_1 )
		{
			pOBJ = pOBJ_1;
			vColl = vColl_1;
			vNor = vNor_1;
		}
		else if ( pOBJ_2 )
		{
			pOBJ = pOBJ_2;
			vColl = vColl_2;
			vNor = vNor_2;
		}
		else
		{
			pOBJ = NULL;
			vColl = D3DXVECTOR3 ( 0.f,0.f,0.f );
			vNor = D3DXVECTOR3 ( 0.f,1.f,0.f );
		}
	}

	HRESULT RenderAABBTree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, const char* szName )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( sCV, pNode->vMax, pNode->vMin ) )	return S_OK;

		if ( pNode->pObject )
		{
			hr = pNode->pObject->Render( pd3dDevice, sCV, szName );
			return hr;
		}

		hr = RenderAABBTree ( pd3dDevice, sCV, pNode->pLeftChild, szName );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderAABBTree ( pd3dDevice, sCV, pNode->pRightChild, szName );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	HRESULT RenderAABBTreeTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, 
								LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( sCV, pNode->vMax, pNode->vMin ) )	return S_OK;

		if ( pNode->pObject )
		{
			hr = pNode->pObject->RenderTHREAD( pd3dDevice, sCV, listLoadingData, CSLockLoading );
			return hr;
		}

		hr = RenderAABBTreeTHREAD ( pd3dDevice, sCV, pNode->pLeftChild, listLoadingData, CSLockLoading );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderAABBTreeTHREAD ( pd3dDevice, sCV, pNode->pRightChild, listLoadingData, CSLockLoading );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	HRESULT RenderAABBTreeDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, CSerialFile* const SFile )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( sCV, pNode->vMax, pNode->vMin ) )	return S_OK;

		if ( pNode->pObject )
		{
			hr = pNode->pObject->RenderDYNAMIC( pd3dDevice, sCV, SFile );
			return hr;
		}

		hr = RenderAABBTreeDYNAMIC ( pd3dDevice, sCV, pNode->pLeftChild, SFile );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderAABBTreeDYNAMIC ( pd3dDevice, sCV, pNode->pRightChild, SFile );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	HRESULT LoadAABBTreeBOX ( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, OBJAABBNode* pNode, CSerialFile* const SFile )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionAABBToAABB( vMax, vMin, pNode->vMax, pNode->vMin ) )	return S_OK;

		if ( pNode->pObject )
		{
			hr = pNode->pObject->FirstLoad( pd3dDevice, vMax, vMin, SFile );
			return hr;
		}

		hr = LoadAABBTreeBOX ( pd3dDevice, vMax, vMin, pNode->pLeftChild, SFile );
		if ( FAILED(hr) )	return E_FAIL;

		hr = LoadAABBTreeBOX ( pd3dDevice, vMax, vMin, pNode->pRightChild, SFile );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	void FrameMove( const float fElapsedTime, OBJAABBNode* pNode )
	{
		if ( !pNode )	return;

		if ( pNode->pObject )	pNode->pObject->FrameMove( fElapsedTime );

		FrameMove( fElapsedTime, pNode->pLeftChild );
		FrameMove( fElapsedTime, pNode->pRightChild );
	}

	void InsertColorList( OBJAABBNode* pNode, LOADINGDATALIST& listColorData )
	{
		if( !pNode )	return;

		if( pNode->pObject )	pNode->pObject->InsertColorList( listColorData );

		InsertColorList( pNode->pLeftChild, listColorData );
		InsertColorList( pNode->pRightChild, listColorData );
	}
};
