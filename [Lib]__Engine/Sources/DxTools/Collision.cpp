// Collision.cpp: implementation of the CCollision class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./EDITMESHS.h"
#include "./Collision.h"
#include "./SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace COLLISION
{
	D3DXVECTOR3 vColTestStart;
	D3DXVECTOR3 vColTestEnd;

	BOOL IsPoint_Inside_Triangle ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
								D3DXVECTOR3 *pPoint,
								BOOL bX, BOOL bY, BOOL bZ )
	{
		D3DXVECTOR2 v1, v2, v3, vP;

		float deltaX=0.0f;
		float m1=0.0f, m2=0.0f, m3=0.0f;
		float b1=0.0f, b2=0.0f, b3=0.0f;
		BOOL b12Vect=FALSE, b23Vect=FALSE, b31Vect=FALSE;

		//	Note : 단 하나만 TRUE 되어 있어야 정상 동작한다.
		//
		if ( ((bX==FALSE)&&(bY==FALSE)) ||
			((bX==FALSE)&&(bZ==FALSE)) ||
			((bY==FALSE)&&(bZ==FALSE)) ||
			((bX==FALSE)&&(bY==FALSE)&&(bZ==FALSE)) )
		{
			return FALSE;
		}

		//	Note : x 축 투영.
		if ( bX==FALSE )
		{
			v1.x = pTri0->y;
			v1.y = pTri0->z;
			v2.x = pTri1->y;
			v2.y = pTri1->z;
			v3.x = pTri2->y;
			v3.y = pTri2->z;
			
			vP.x = pPoint->y;
			vP.y = pPoint->z;
		}
		//	Note : y축 투영.
		else if ( bY==FALSE )
		{
			v1.x = pTri0->x;
			v1.y = pTri0->z;
			v2.x = pTri1->x;
			v2.y = pTri1->z;
			v3.x = pTri2->x;
			v3.y = pTri2->z;
			
			vP.x = pPoint->x;
			vP.y = pPoint->z;
		}
		//	Note : z축 투영.
		else if ( bZ==FALSE )
		{
			v1.x = pTri0->x;
			v1.y = pTri0->y;
			v2.x = pTri1->x;
			v2.y = pTri1->y;
			v3.x = pTri2->x;
			v3.y = pTri2->y;
			
			vP.x = pPoint->x;
			vP.y = pPoint->y;
		}


		//	Note : y=mx+b 세가지 선분 방정식 계산.
		//
		deltaX = (v2.x-v1.x);
		if ( deltaX!=0 )
		{
			m1 = (v2.y-v1.y) / deltaX;	//	v1 -> v2
			b1 = v1.y - (m1*v1.x);		//	b=y-mx
		}
		else //	deltaX==0
		{
			b12Vect = TRUE;
		}

		deltaX = (v3.x-v2.x);
		if ( deltaX!=0 )
		{
			m2 = (v3.y-v2.y) / deltaX;	//	v2 -> v3
			b2 = v2.y - (m2*v2.x);		//	b=y-mx
		}
		else //	deltaX==0
		{
			b23Vect = TRUE;
		}

		deltaX = (v1.x-v3.x);
		if ( deltaX!=0 )
		{
			m3 = (v1.y-v3.y) / deltaX;	//	v3 -> v1
			b3 = v3.y - (m3*v3.x);		//	b=y-mx
		}
		else //	deltaX==0
		{
			b31Vect = TRUE;
		}
		
		//	Note : 삼각형이 아니라 직선에 가까워서 확인 작업 중단.
		if ( IsfEqual(m1,m2) && IsfEqual(m1,m3) )	return FALSE;
		////  Modify : 가까운 2개의 점이 있을 경우 문제가 발생했다.
		////			밑 부분에서 선같은 경우는 체크를 못하기 때문에 여기서 막았다.
		//if ( IsfEqual(m1,m2) || IsfEqual(m1,m3) || IsfEqual(m2,m3) )	return FALSE;

		//	Note : 중점 찾기.
		//		- 트라이엥글 안에 있는 점 구하기.
		//
		D3DXVECTOR2 vCenter;
		vCenter.x = (v1.x+v2.x+v3.x) / 3;
		vCenter.y = (v1.y+v2.y+v3.y) / 3;

		//	Note : 선분을 기준으로 중점이 어디에 위치하는지 구하기.
		//
		BOOL bUP1, bUP2, bUP3;

		//	Note : v1 -> v2
		if ( (m1*vCenter.x+b1) >= vCenter.y )
			bUP1 = TRUE;
		else
			bUP1 = FALSE;

		//	Note : v2 -> v3
		if ( (m2*vCenter.x+b2) >= vCenter.y )
			bUP2 = TRUE;
		else
			bUP2 = FALSE;

		//	Note : v3 -> v1
		if ( (m3*vCenter.x+b3) >= vCenter.y )
			bUP3 = TRUE;
		else
			bUP3 = FALSE;


		//	Note : 삼각형 내부에 있는지 판단.
		//
		int nInside = 0;

		//	Note : v1 -> v2
		if ( b12Vect==TRUE )
		{
			if ( (v1.x<vP.x) && (v1.x<vCenter.x) )
				nInside++;
			else if ( (v1.x>vP.x) && (v1.x>vCenter.x) )
				nInside++;
		}
		else
		{
			if ( bUP1 )
			{
				if ( vP.y <= (m1*vP.x)+b1 )
					nInside++;
			}
			else
			{
				if ( vP.y >= (m1*vP.x)+b1 )
					nInside++;
			}
		}

		//	Note : v2 -> v3
		if ( b23Vect==TRUE )
		{
			if ( (v2.x<vP.x) && (v2.x<vCenter.x) )
				nInside++;
			else if ( (v2.x>vP.x) && (v2.x>vCenter.x) )
				nInside++;
		}
		else
		{
			if ( bUP2 )
			{
				if ( vP.y <= (m2*vP.x)+b2 )
					nInside++;
			}
			else
			{
				if ( vP.y >= (m2*vP.x)+b2 )
					nInside++;
			}
		}

		//	Note : v3 -> v1
		if ( b31Vect==TRUE )
		{
			if ( (v3.x<vP.x) && (v3.x<vCenter.x) )
				nInside++;
			else if ( (v3.x>vP.x) && (v3.x>vCenter.x) )
				nInside++;
		}
		else
		{
			if ( bUP3 )
			{
				if ( vP.y <= (m3*vP.x)+b3 )
					nInside++;
			}
			else
			{
				if ( vP.y >= (m3*vP.x)+b3 )
					nInside++;
			}
		}

		if ( nInside == 3 )		return TRUE;

		return FALSE;
	}

	BOOL IsLineTriangleCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
								D3DXVECTOR3 *pPoint1, D3DXVECTOR3 *pPoint2,
								D3DXVECTOR3 *pCollision, D3DXVECTOR3 *pNormal, const BOOL bFrontColl )
	{
		//	Note : 평면이 아닌 상황 걸러내기.
		//
		DWORD dwEqual = 0;
		bool bEQX(false), bEQY(false), bEQZ(false);
		if ( IsfEqual(pTri0->x,pTri1->x) && IsfEqual(pTri0->x,pTri2->x) )
		{
			bEQX = true;
			dwEqual++;
		}

		if ( IsfEqual(pTri0->y,pTri1->y) && IsfEqual(pTri0->y,pTri2->y) )
		{
			bEQY = true;
			dwEqual++;
		}

		if ( IsfEqual(pTri0->z,pTri1->z) && IsfEqual(pTri0->z,pTri2->z) )
		{
			bEQZ = true;
			dwEqual++;
		}

		if ( dwEqual > 1 )	return FALSE;

		//	Note : 노말 값 구하기.
		//
		D3DXVECTOR3 v1, v2, vNormal;
		v1 = *pTri1 - *pTri0;
		v2 = *pTri2 - *pTri0;
		D3DXVec3Cross ( &vNormal, &v1, &v2 );
		D3DXVec3Normalize ( &vNormal, &vNormal );

		// Note : 면이 나를 바라보고 있지 않을 경우 픽킹을 하지 않는다.
		if( bFrontColl )
		{
			D3DXVECTOR3 vDir = *pPoint2 - *pPoint1;
			float fDot = D3DXVec3Dot( &vDir, &vNormal );
			if( fDot > 0.f )	return FALSE;
		}

		D3DXVECTOR3	vPoint = *pPoint1;

		//	Note : 평면내의 교점 찾기.
		//
		float Distance, Ratio;
		D3DXVECTOR3 vDxP1Tri0, vDxP1P2, vCollision;

		vDxP1Tri0 = (vPoint-*pTri0);
		Distance = D3DXVec3Dot ( &vDxP1Tri0, &vNormal );

		vDxP1P2 = (vPoint-*pPoint2);
		Ratio = Distance / D3DXVec3Dot ( &vDxP1P2, &vNormal );

		vCollision = vPoint + Ratio * ( *pPoint2 - vPoint );


		//	Note : 교점이 두 꼭지점 사이에 있는지 판단.
		//
		//	Note : 변이 평면에 수직인지 검사.
		if ( ( vPoint.x == pPoint2->x ) && ( vPoint.z == pPoint2->z ) )
		{
			if ( !( ( (vPoint.y<=vCollision.y) && (vCollision.y<=pPoint2->y) ) ||
				( (pPoint2->y<=vCollision.y) && (vCollision.y<=vPoint.y) ) ) )
			{
				return FALSE;	//	충돌 없음.
			}
		}

		//	Note : 평면 방정식.
		//
		D3DXPLANE Plane;
		Plane.a = vNormal.x;
		Plane.b = vNormal.y;
		Plane.c = vNormal.z;
		Plane.d = -(vNormal.x * pTri0->x + vNormal.y * pTri0->y + vNormal.z * pTri0->z);	// 기존 방식

		//	Note : 두꼭지점 사이에 있는지 검사.
		float t=0, i=0;
		i = Plane.a*pPoint2->x + Plane.b*pPoint2->y + Plane.c*pPoint2->z + Plane.d;
		t = - ( Plane.a*pPoint2->x + Plane.b*pPoint2->y + Plane.c*pPoint2->z );
		t += Plane.a*vPoint.x + Plane.b*vPoint.y + Plane.c*vPoint.z;

		//	Note : 0 나누기 발생 우려.
		t = (-i) / t;
		if ( ! ( (t>=0) && (t<=1) ) )
		{
			return FALSE;	//	충돌 없음.
		}

		//	Note : 삼각형 영역 내에 있는지 검사.
		//
		BOOL bInside = FALSE;
		
		//	Note : 특정축으로 투영되어 있을 때에는 그것을 기준으로 검사.
		//		아닐 경우, 축 방정식으로 분류.
		if ( bEQX || bEQY || bEQZ )
		{
			if ( bEQX )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, FALSE, TRUE, TRUE );
			else if ( bEQY )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, TRUE, FALSE, TRUE );
			else if ( bEQZ )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, TRUE, TRUE, FALSE );
		}
		else
		{
			//	Note : X가 최대인 경우.
			if ( (fabs(Plane.a)>=fabs(Plane.b)) && (fabs(Plane.a)>=fabs(Plane.c)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, FALSE, TRUE, TRUE );

			//	Note : Y가 최대인 경우.
			if ( (fabs(Plane.b)>=fabs(Plane.a)) && (fabs(Plane.b)>=fabs(Plane.c)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, TRUE, FALSE, TRUE );
			
			//	Note : Z가 최대인 경우.
			if ( (fabs(Plane.c)>=fabs(Plane.a)) && (fabs(Plane.c)>=fabs(Plane.b)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vCollision, TRUE, TRUE, FALSE );
		}

		if ( !bInside )		return FALSE;

		//	Note : 충돌 값을 반환.
		//
		*pCollision = vCollision;
		if ( pNormal )	*pNormal = vNormal;

		return TRUE;
	}

	BOOL IsCollisionVolume ( const CLIPVOLUME& cv, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
	{
		D3DXVECTOR3 vNearMin(	cv.pNear.a * vMin.x,	cv.pNear.b * vMin.y,	cv.pNear.c * vMin.z );
		D3DXVECTOR3 vNearMax(	cv.pNear.a * vMax.x,	cv.pNear.b * vMax.y,	cv.pNear.c * vMax.z );
		D3DXVECTOR3 vFarMin(	cv.pFar.a * vMin.x,		cv.pFar.b * vMin.y,		cv.pFar.c * vMin.z );
		D3DXVECTOR3 vFarMax(	cv.pFar.a * vMax.x,		cv.pFar.b * vMax.y,		cv.pFar.c * vMax.z );

		if( ((vNearMin.x + vNearMax.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vNearMin.x + vNearMax.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vNearMax.x + vNearMax.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vNearMax.x + vNearMax.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vNearMin.x + vNearMin.y + vNearMin.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vNearMin.x + vNearMin.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vNearMax.x + vNearMin.y + vNearMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vNearMax.x + vNearMin.y + vNearMin.z + cv.pNear.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vFarMin.x + vFarMax.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vFarMin.x + vFarMax.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vFarMax.x + vFarMax.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vFarMax.x + vFarMax.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vFarMin.x + vFarMin.y + vFarMin.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vFarMin.x + vFarMin.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vFarMax.x + vFarMin.y + vFarMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vFarMax.x + vFarMin.y + vFarMin.z + cv.pFar.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		D3DXVECTOR3 vLeftMin(	cv.pLeft.a * vMin.x,	cv.pLeft.b * vMin.y,	cv.pLeft.c * vMin.z );
		D3DXVECTOR3 vLeftMax(	cv.pLeft.a * vMax.x,	cv.pLeft.b * vMax.y,	cv.pLeft.c * vMax.z );
		D3DXVECTOR3 vRightMin(	cv.pRight.a * vMin.x,	cv.pRight.b * vMin.y,	cv.pRight.c * vMin.z );
		D3DXVECTOR3 vRightMax(	cv.pRight.a * vMax.x,	cv.pRight.b * vMax.y,	cv.pRight.c * vMax.z );

		if( ((vLeftMin.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vLeftMin.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vLeftMax.x + vLeftMax.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vLeftMin.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vLeftMin.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vLeftMax.x + vLeftMin.y + vLeftMin.z + cv.pLeft.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vRightMin.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vRightMin.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vRightMax.x + vRightMax.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vRightMax.x + vRightMax.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vRightMin.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vRightMin.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vRightMax.x + vRightMin.y + vRightMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vRightMax.x + vRightMin.y + vRightMin.z + cv.pRight.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		D3DXVECTOR3 vTopMin(	cv.pTop.a * vMin.x,		cv.pTop.b * vMin.y,		cv.pTop.c * vMin.z );
		D3DXVECTOR3 vTopMax(	cv.pTop.a * vMax.x,		cv.pTop.b * vMax.y,		cv.pTop.c * vMax.z );
		D3DXVECTOR3 vBottomMin(	cv.pBottom.a * vMin.x,	cv.pBottom.b * vMin.y,	cv.pBottom.c * vMin.z );
		D3DXVECTOR3 vBottomMax(	cv.pBottom.a * vMax.x,	cv.pBottom.b * vMax.y,	cv.pBottom.c * vMax.z );

		if( ((vTopMin.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
			((vTopMin.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
			((vTopMax.x + vTopMax.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
			((vTopMax.x + vTopMax.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

			((vTopMin.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
			((vTopMin.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
			((vTopMax.x + vTopMin.y + vTopMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
			((vTopMax.x + vTopMin.y + vTopMin.z + cv.pTop.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		else if( ((vBottomMin.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
				 ((vBottomMin.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
				 ((vBottomMax.x + vBottomMax.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
				 ((vBottomMax.x + vBottomMax.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

				 ((vBottomMin.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
				 ((vBottomMin.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
				 ((vBottomMax.x + vBottomMin.y + vBottomMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
				 ((vBottomMax.x + vBottomMin.y + vBottomMin.z + cv.pBottom.d) > -0.01f) )	//BOTTOM_RIGHT_FRONT
			return FALSE;

		return TRUE;
	}

	BOOL IsCollisionAABBToAABB ( const D3DXVECTOR3 &vAMax, const D3DXVECTOR3 &vAMin,
		const D3DXVECTOR3 &vBMax, const D3DXVECTOR3 &vBMin )
	{
		if ( vAMax.x<vBMin.x && vAMin.x<vBMin.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.x>vBMax.x && vAMin.x>vBMax.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.y<vBMin.y && vAMin.y<vBMin.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.y>vBMax.y && vAMin.y>vBMax.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.z<vBMin.z && vAMin.z<vBMin.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vAMax.z>vBMax.z && vAMin.z>vBMax.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		return TRUE;
	}

	BOOL IsCollisionLineToAABB ( const D3DXVECTOR3 vStart, const D3DXVECTOR3 vEnd,
		const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
	{
		D3DXVECTOR3 vP1 = vStart;
		D3DXVECTOR3 vP2 = vEnd;
		float fRatio;
		D3DXVECTOR3 vCullPos;

		//	Note : X,min
		//		N(-1,0,0) : -X + X,min = 0
		//		X < X,min is front
		//
		if ( vP1.x<vMin.x && vP2.x<vMin.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.x<vMin.x || vP2.x<vMin.x )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.x + vMin.x ) / ( -vP1.x + vP2.x );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.x < vMin.x )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : X,max
		//		N(1,0,0) : X - X,max = 0
		//		X > X,max is front
		//
		if ( vP1.x>vMax.x && vP2.x>vMax.x )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.x>vMax.x || vP2.x>vMax.x )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( vP1.x - vMax.x ) / ( vP1.x - vP2.x );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.x > vMax.x )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Y,min
		//		N(0,-1,0) : -Y + Y,min = 0
		//		Y < Y,min is front
		//
		if ( vP1.y<vMin.y && vP2.y<vMin.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.y<vMin.y || vP2.y<vMin.y )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.y + vMin.y ) / ( -vP1.y + vP2.y );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.y < vMin.y )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Y,max
		//		N(0,1,0) : Y - Y,max = 0
		//		Y > Y,max is front
		//
		if ( vP1.y>vMax.y && vP2.y>vMax.y )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.y>vMax.y || vP2.y>vMax.y )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( vP1.y - vMax.y ) / ( vP1.y - vP2.y );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.y > vMax.y )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Z,min
		//		N(0,0,-1) : -Z + Z,min = 0
		//		Z < Z,min is front
		//
		if ( vP1.z<vMin.z && vP2.z<vMin.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		if ( vP1.z<vMin.z || vP2.z<vMin.z )
		{
			//	Note : 충돌점 구하기.
			//
			fRatio = ( -vP1.z + vMin.z ) / ( -vP1.z + vP2.z );
			vCullPos = vP1 + fRatio * ( vP2 - vP1 );

			//	Note : 선분을 이등분하여 front 부분 버리기.
			//
			if ( vP1.z < vMin.z )	vP1 = vCullPos;
			else					vP2 = vCullPos;
		}

		//	Note : Z,max
		//		N(0,0,-1) : Z - Z,max = 0
		//		Z > Z,max is front
		//
		if ( vP1.z > vMax.z && vP2.z > vMax.z )
			return FALSE;	// back 부분이 없으므로 충돌하지 않음.

		//	Note : 충돌 되었음.
		return TRUE;
	}

	BOOL IsCollisionLineToSphere( const D3DXVECTOR3 vStart, const D3DXVECTOR3 vEnd, const D3DXVECTOR3 &vCenter, const float fLength )
	{
		// Note : 삼각형을 만들기 위한 3번째의 점을 찾는다.
		D3DXVECTOR3 vThree;
		const D3DXVECTOR3 vLine1 = vStart - vEnd;
		const D3DXVECTOR3 vLine2 = vStart - vCenter;

		if( ((vLine1.x + 0.01f) >= vLine2.x) && (vLine1.x <= (vLine2.x + 0.01f)) && 
			((vLine1.y + 0.01f) >= vLine2.y) && (vLine1.y <= (vLine2.y + 0.01f)) && 
			((vLine1.z + 0.01f) >= vLine2.z) && (vLine1.z <= (vLine2.z + 0.01f)) )		return TRUE;

		D3DXVec3Cross( &vThree, &vLine1, &vLine2 );
		vThree += vEnd;

		// Note : 삼각형을 만든다.
		D3DXPLANE sPlane;
		D3DXPlaneFromPoints( &sPlane, &vStart, &vEnd, &vThree );

		// Note : 원점과의 거리를 구한다.
		float fTempDis = (sPlane.a*vCenter.x) + (sPlane.b*vCenter.y) + (sPlane.c*vCenter.z) + sPlane.d;
		fTempDis = fabsf( fTempDis );

		// Note : 체크를 한다.
		if( fTempDis > fLength )	return FALSE;
		else						return TRUE;
	}

	D3DXVECTOR3 ClosestPointOnLine ( D3DXVECTOR3 *pvA, D3DXVECTOR3 *pvB, D3DXVECTOR3 vPoint )
	{
		D3DXVECTOR3 vVecter1 = vPoint - *pvA;
		D3DXVECTOR3 vVecter2 = *pvB - *pvA;
		float fDistant = D3DXVec3Length ( &vVecter2 );
		D3DXVec3Normalize ( &vVecter2, &vVecter2 );

		float fDot = D3DXVec3Dot ( &vVecter2, &vVecter1 );

		if ( fDot <= 0 )			return *pvA;
		if ( fDot >= fDistant )		return *pvB;

		D3DXVECTOR3 vVecter3 = vVecter2 * fDot;

		return *pvA + vVecter3;
	}

	BOOL IsSpherePointCollision ( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCenter, const float fRadius )
	{
		return D3DXVec3Length ( &D3DXVECTOR3(vPos-vCenter) ) < fRadius;
	}

	BOOL IsSpherePlolygonCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
								const D3DXVECTOR3 &vCenter, const float fRadius )
	{
		//	Note : 노말 값 구하기.
		//
		D3DXVECTOR3 v1, v2, vNormal;
		v1 = *pTri1 - *pTri0;
		v2 = *pTri2 - *pTri0;
		D3DXVec3Cross ( &vNormal, &v1, &v2 );
		D3DXVec3Normalize ( &vNormal, &vNormal );

		float fDistance = 0.0f;


		//	Note : 평면 방정식.
		//
		D3DXPLANE Plane;
		Plane.a = vNormal.x;
		Plane.b = vNormal.y;
		Plane.c = vNormal.z;
		Plane.d = -(vNormal.x * pTri0->x + vNormal.y * pTri0->y + vNormal.z * pTri0->z);

		//	Note : 평면과 구 사이의 거리 계산.
		//		Ax + Bx + Cz + d = 0  :  x,y,z (pTri0)
		//
		fDistance = Plane.a * vCenter.x + Plane.b * vCenter.y + Plane.c * vCenter.z + Plane.d;

		//	Note : 충돌.
		//
		if ( fDistance < fRadius && fDistance > -fRadius )
		{
			//	Note : vCenter와 평면간에 가장 가까운 지점을 찾음.
			//
			D3DXVECTOR3 vPosition;

			vPosition = vCenter - vNormal*fDistance;
			
			//	Note : 삼각형 영역 내에 있는지 검사.
			//
			BOOL bInside = FALSE;
			
			//	Note : X가 최대인 경우.
			if ( (fabs(Plane.a)>=fabs(Plane.b)) && (fabs(Plane.a)>=fabs(Plane.c)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, FALSE, TRUE, TRUE );

			//	Note : Y가 최대인 경우.
			if ( (fabs(Plane.b)>=fabs(Plane.a)) && (fabs(Plane.b)>=fabs(Plane.c)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, TRUE, FALSE, TRUE );
			
			//	Note : Z가 최대인 경우.
			if ( (fabs(Plane.c)>=fabs(Plane.a)) && (fabs(Plane.c)>=fabs(Plane.b)) )
				bInside = IsPoint_Inside_Triangle ( pTri0, pTri1, pTri2, &vPosition, TRUE, TRUE, FALSE );

			return bInside;
		}
		//	Note : Back 에 있음.
		//
		else if ( fDistance < fRadius )
		{
			D3DXVECTOR3 vClosestP = ClosestPointOnLine ( pTri0, pTri1, vCenter );
			vClosestP = vClosestP-vCenter;

			float fDist = D3DXVec3Length ( &vClosestP );

			if ( fDist < fRadius )	return TRUE;
		}

		return FALSE;
	}

	HRESULT CalculateBoundingBox ( const D3DXMATRIX *const pmatComb, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin,
								PBYTE pbPoints, UINT cVertices, DWORD dwFVF )
	{
		GASSERT(pbPoints);

		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		PBYTE pbCur;
		D3DXVECTOR3 vDist;
		D3DXVECTOR3 *pvCur;
		DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );

		UINT iPoint;
		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
		{
			pvCur = (D3DXVECTOR3*)pbCur;
			D3DXVec3TransformCoord ( &vDist, pvCur, pmatComb );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;
		}

		return S_OK;
	}

	bool CalculatePlane2Line ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, D3DXVECTOR3 &vCol )
	{
		D3DXVECTOR3 vNormal(plane.a,plane.b,plane.c);
		D3DXVECTOR3 vDist = vLine2 - vLine1;

		float fNormal4Line1 = D3DXVec3Dot ( &vNormal, &vLine1 );
		float fNormal4Dist = D3DXVec3Dot ( &vNormal, &vDist );

		float t = ( - plane.d - fNormal4Line1 ) / fNormal4Dist;

		//	t의 값은 ( 0 <= t <= 1 ) 으로 제한된다.
		//
		if ( 0 > t )	return false;
		if ( t > 1 )	return false;

		vCol = vLine1 + t * vDist;

		return true;
	}

	void TransformAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXMATRIX &matTrans )
	{
		D3DXVECTOR3 VertBox[8];

		VertBox[0] = D3DXVECTOR3(vMin.x,vMax.y,vMin.z);
		VertBox[1] = D3DXVECTOR3(vMin.x,vMax.y,vMax.z);
		VertBox[2] = D3DXVECTOR3(vMax.x,vMax.y,vMax.z);
		VertBox[3] = D3DXVECTOR3(vMax.x,vMax.y,vMin.z);

		VertBox[4] = D3DXVECTOR3(vMin.x,vMin.y,vMin.z);
		VertBox[5] = D3DXVECTOR3(vMin.x,vMin.y,vMax.z);
		VertBox[6] = D3DXVECTOR3(vMax.x,vMin.y,vMax.z);
		VertBox[7] = D3DXVECTOR3(vMax.x,vMin.y,vMin.z);

		CalculateBoundingBox ( &matTrans, vMax, vMin, (PBYTE)VertBox, 8, D3DFVF_XYZ );
	}

	DWORD dwLeafCount = 0;
	DWORD dwNodeCount = 0;
	void MakeAABBTree ( const POBJAABB *const ppArray, const DWORD dwArraySize,
		const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin, POBJAABBNODE &pTreeNode, BOOL bQUAD )
	{
		if ( dwArraySize == 1 )
		{
			D3DXVECTOR3 vObjMax, vObjMin;
			ppArray[0]->GetAABBSize ( vObjMax, vObjMin );

			pTreeNode->vMax = vObjMax;
			pTreeNode->vMin = vObjMin;

			dwLeafCount++;
			pTreeNode->pObject = ppArray[0];
			return;
		}

		float SizeX = vMax.x - vMin.x;
		float SizeY = vMax.y - vMin.y;
		float SizeZ = vMax.z - vMin.z;

		D3DXVECTOR3 vDivMax = vMax;
		D3DXVECTOR3 vDivMin = vMin;

		if ( bQUAD )
		{
			if ( SizeX >= SizeZ )		vDivMin.x = vMin.x + SizeX/2;
			else						vDivMin.z = vMin.z + SizeZ/2;
		}
		else
		{
			if ( SizeX > SizeY && SizeX > SizeZ )	vDivMin.x = vMin.x + SizeX/2;
			else if ( SizeY > SizeZ )				vDivMin.y = vMin.y + SizeY/2;
			else									vDivMin.z = vMin.z + SizeZ/2;
		}

		DWORD dwLeftCount = 0, dwRightCount = 0;
		POBJAABB* pLeftArray = new POBJAABB[dwArraySize];
		POBJAABB* pRightArray = new POBJAABB[dwArraySize];

		//	Note : 분할된 공간으로 요소 분리.
		DWORD i;
		for ( i=0; i<dwArraySize; i++ )
		{
			if ( ppArray[i]->IsDetectDivision (vDivMax,vDivMin) )
				pLeftArray[dwLeftCount++] = ppArray[i];
			else
				pRightArray[dwRightCount++] = ppArray[i];
		}

		//	Note : 무작위로 요소 분리.
		if ( dwLeftCount == 0 || dwRightCount == 0 )
		{
			DWORD dwDivCount = dwArraySize / 2;
		
			dwLeftCount = 0;
			dwRightCount = 0;

			for ( i=0; i<dwDivCount; i++ )
				pLeftArray[dwLeftCount++] = ppArray[i];

			for ( i=dwDivCount; i<dwArraySize; i++ )
				pRightArray[dwRightCount++] = ppArray[i];
		}

		D3DXVECTOR3 vNodeMax, vNodeMin;

		//	Note : 왼쪽 자식 노드 생성.
		//
		D3DXVECTOR3 vCMax, vCMin;
		vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		for ( i=0; i<dwLeftCount; i++ )
		{
			pLeftArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
		}

		pTreeNode->pLeftChild = new OBJAABBNode;
		pTreeNode->pLeftChild->vMax = vCMax;
		pTreeNode->pLeftChild->vMin = vCMin;
		MakeAABBTree ( pLeftArray, dwLeftCount, vCMax, vCMin, pTreeNode->pLeftChild, bQUAD );

		//	Note : 오른쪽 자식 노드 생성.
		//
		vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( i=0; i<dwRightCount; i++ )
		{
			pRightArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
		}

		pTreeNode->pRightChild = new OBJAABBNode;
		pTreeNode->pRightChild->vMax = vCMax;
		pTreeNode->pRightChild->vMin = vCMin;
		MakeAABBTree ( pRightArray, dwRightCount, vCMax, vCMin, pTreeNode->pRightChild, bQUAD );

		SAFE_DELETE_ARRAY(pRightArray);
		SAFE_DELETE_ARRAY(pLeftArray);
	}

	void MakeAABBTree ( POBJAABBNODE &pTreeHead, POBJAABB pListHead, BOOL bQUAD )
	{
		SAFE_DELETE(pTreeHead);

		dwLeafCount = 0;
		dwNodeCount = 0;
		POBJAABB pNodeCur;

		//	Note : 갯수 파악.
		//
		pNodeCur = pListHead;
		while ( pNodeCur )
		{
			dwNodeCount++;
			pNodeCur = pNodeCur->GetNext();
		}

		if ( dwNodeCount == 0 )	return;

		//	Note : 포인터 배열에 저장후 정렬수행.
		//
		POBJAABB *ppArray = new POBJAABB[dwNodeCount];

		dwNodeCount = 0;
		pNodeCur = pListHead;
		while ( pNodeCur )
		{
			ppArray[dwNodeCount++] = pNodeCur;
			pNodeCur = pNodeCur->GetNext();
		}

		D3DXVECTOR3 vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		D3DXVECTOR3 vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		D3DXVECTOR3 vNodeMax, vNodeMin;
		for ( DWORD i=0; i<dwNodeCount; i++ )
		{
			ppArray[i]->GetAABBSize ( vNodeMax, vNodeMin );

			if ( vMax.x < vNodeMax.x )	vMax.x = vNodeMax.x;
			if ( vMax.y < vNodeMax.y )	vMax.y = vNodeMax.y;
			if ( vMax.z < vNodeMax.z )	vMax.z = vNodeMax.z;

			if ( vMin.x > vNodeMin.x )	vMin.x = vNodeMin.x;
			if ( vMin.y > vNodeMin.y )	vMin.y = vNodeMin.y;
			if ( vMin.z > vNodeMin.z )	vMin.z = vNodeMin.z;
		}

		pTreeHead = new OBJAABBNode;
		pTreeHead->vMax = vMax;
		pTreeHead->vMin = vMin;
		MakeAABBTree ( ppArray, dwNodeCount, vMax, vMin, pTreeHead, bQUAD );

		SAFE_DELETE_ARRAY(ppArray);
	}

	DWORD dwRendAABB = 0;
	HRESULT RenderAABBTree ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )	return S_OK;
		if ( bRendAABB )		EDITMESHS::RENDERAABB ( pd3dDevice, pNode->vMax, pNode->vMin );	//	테스트용.

		if ( pNode->pObject )
		{
			dwRendAABB++;
			hr = pNode->pObject->Render ( pd3dDevice, pCV );
			return hr;
		}

		hr = RenderAABBTree ( pd3dDevice, pCV, pNode->pLeftChild, bRendAABB );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderAABBTree ( pd3dDevice, pCV, pNode->pRightChild, bRendAABB );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	HRESULT RenderAABBTreeFrame ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB, DxLandMan* pLandMan )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )	return S_OK;
		if ( bRendAABB )		EDITMESHS::RENDERAABB ( pd3dDevice, pNode->vMax, pNode->vMin );	//	테스트용.

		if ( pNode->pObject )
		{
			dwRendAABB++;
			return pNode->pObject->Render ( NULL, pd3dDevice, NULL, pLandMan );
		}

		hr = RenderAABBTreeFrame ( pd3dDevice, pCV, pNode->pLeftChild, bRendAABB, pLandMan );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderAABBTreeFrame ( pd3dDevice, pCV, pNode->pRightChild, bRendAABB, pLandMan );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	POBJAABB IsCollisonPointToOBJAABBTree ( POBJAABBNODE &pTreeNode, const D3DXVECTOR3 &vPos )
	{
		if ( IsWithInPoint ( pTreeNode->vMax, pTreeNode->vMin, vPos ) )
		{
			if ( pTreeNode->pObject )	return pTreeNode->pObject;

			if ( pTreeNode->pLeftChild )
			{
				POBJAABB pObjAABB = IsCollisonPointToOBJAABBTree ( pTreeNode->pLeftChild, vPos );
				if ( pObjAABB )			return pObjAABB;
			}
			
			if ( pTreeNode->pRightChild )
			{
				POBJAABB pObjAABB = IsCollisonPointToOBJAABBTree ( pTreeNode->pRightChild, vPos );
				if ( pObjAABB )			return pObjAABB;
			}
		}

		return NULL;
	}

	void MINDETECTAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const float fRate, const float fMinLeng )
	{
		float fLength(0);
		fLength = vMax.x - vMin.x;
		vMax.x += ( fLength*fRate - fLength ) / 2.0f;
		vMin.x -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.x - vMin.x;
		if ( fLength < fMinLeng )
		{
			vMax.x += fMinLeng/2.0f;
			vMin.x -= fMinLeng/2.0f;
		}

		fLength = vMax.y - vMin.y;
		vMax.y += ( fLength*fRate - fLength ) / 2.0f;
		vMin.y -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.y - vMin.y;
		if ( fLength < fMinLeng )
		{
			vMax.y += fMinLeng/2.0f;
			vMin.y -= fMinLeng/2.0f;
		}

		fLength = vMax.z - vMin.z;
		vMax.z += ( fLength*fRate - fLength ) / 2.0f;
		vMin.z -= ( fLength*fRate - fLength ) / 2.0f;

		fLength = vMax.z - vMin.z;
		if ( fLength < fMinLeng )
		{
			vMax.z += fMinLeng/2.0f;
			vMin.z -= fMinLeng/2.0f;
		}
	}
};

BOOL DxAABBNode::IsCollision ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 )
{
	float fRatio;
	D3DXVECTOR3 vCollision;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<fMinX && vP2.x<fMinX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<fMinX || vP2.x<fMinX )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + fMinX ) / ( -vP1.x + vP2.x );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < fMinX )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>fMaxX && vP2.x>fMaxX )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>fMaxX || vP2.x>fMaxX )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - fMaxX ) / ( vP1.x - vP2.x );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > fMaxX )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<fMinY && vP2.y<fMinY )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<fMinY || vP2.y<fMinY )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + fMinY ) / ( -vP1.y + vP2.y );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < fMinY )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>fMaxY && vP2.y>fMaxY )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>fMaxY || vP2.y>fMaxY )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - fMaxY ) / ( vP1.y - vP2.y );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > fMaxY )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<fMinZ && vP2.z<fMinZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<fMinZ || vP2.z<fMinZ )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + fMinZ ) / ( -vP1.z + vP2.z );
		vCollision = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < fMinZ )	vP1 = vCollision;
		else					vP2 = vCollision;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > fMaxZ && vP2.z > fMaxZ )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	//	Note : 충돌 발생.
	return TRUE;
}

BOOL DxAABBNode::IsCollisionVolume ( CLIPVOLUME & cv )
{
	if (((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	return TRUE;
}

BOOL DxAABBNode::SaveFile ( CSerialFile &SFile )
{	
	SFile << fMaxX;
	SFile << fMaxY;
	SFile << fMaxZ;

	SFile << fMinX;
	SFile << fMinY;
	SFile << fMinZ;

	SFile << dwFace;

	if ( pLeftChild )
	{
		SFile << BOOL ( TRUE );
		pLeftChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pRightChild )
	{
		SFile << BOOL ( TRUE );
		pRightChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
	return TRUE;
}

BOOL DxAABBNode::LoadFile ( CSerialFile &SFile )
{
	BOOL	bExist = FALSE;

	SFile >> fMaxX;
	SFile >> fMaxY;
	SFile >> fMaxZ;

	SFile >> fMinX;
	SFile >> fMinY;
	SFile >> fMinZ;

	SFile >> dwFace;

	SFile >> bExist;
	if ( bExist )
	{
		pLeftChild = new DxAABBNode();
		pLeftChild->LoadFile ( SFile );
	}	

	SFile >> bExist;
	if ( bExist )
	{
		pRightChild = new DxAABBNode();
		pRightChild->LoadFile ( SFile );
	}

	return TRUE;
}
