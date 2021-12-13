#include "pch.h"

#include "DxMethods.h"

#include "DxPhysical.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float CPhysicalData::GRAVITY = 9.8f*10.f;		// 9.8m/s^2

CPhysicalData::CPhysicalData() :
	m_fTime(0.f),
	m_fElapsedTime(0.f),

	m_emObjectMove(EMOM_DONTMOVE),
	m_emBestHigh(EMBH_NONE),

	m_vPos(0,0,0),
	m_vDir(1.f,1.f,0),
	m_vDirAccel(0,0,0),
	m_fVelocity(0.f),      
	m_fAccel(0.f),

	m_fRadius(0.5f),
	m_fFriction(10.f),                         
	m_fAirResist(0.2f),
	m_fElastic(0.8f)
{
}

CPhysicalData::~CPhysicalData()
{
}

void CPhysicalData::InitData ( D3DXVECTOR3 vPos, D3DXVECTOR3 vVelocity, float fRadius, float fFriction, float fElastic )
{
	m_fTime			= 0.f;
	m_vPos			= vPos;
	m_vInitPos		= vPos;
	m_vInitVelocity = vVelocity;
	m_vVelocity		= vVelocity;
	m_fRadius		= fRadius;
	m_fFriction		= fFriction;
	m_fElastic		= fElastic;

	ComPuteHighData ();
}

void CPhysicalData::FrameMove ( float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;
	m_fTime += fElapsedTime;
}

void CPhysicalData::Render ()
{
	m_vPos_Prev = m_vPos;

	//	현재 시간의 위치값 계산
	m_vPos.x = m_vInitPos.x + (m_vInitVelocity.x*m_fTime);
	m_vPos.z = m_vInitPos.z + (m_vInitVelocity.z*m_fTime);
	m_vPos.y = m_vInitPos.y + (m_vInitVelocity.y*m_fTime) - (0.5f*GRAVITY*m_fTime*m_fTime);	// (vt) - (1/2gt^2)

	CheckObjectMove ();
}

void CPhysicalData::Render ( D3DXVECTOR3 vEndPos, float fAccel )
{
	m_vPos_Prev = m_vPos;

	D3DXVECTOR3 vDir = vEndPos - m_vPos;
	m_vVelocity = m_vVelocity + (vDir*fAccel*m_fElapsedTime);
	m_vVelocity.y = m_vVelocity.y - (GRAVITY*m_fElapsedTime);

	//	현재 시간의 위치값 계산
	m_vPos = m_vPos + (m_vVelocity*m_fElapsedTime);
}

D3DXVECTOR3 CPhysicalData::GetVec3Direct ()
{
	D3DXVECTOR3 vVelocity;
	ComPuteVelocity ( vVelocity, m_fTime );
	D3DXVec3Normalize ( &vVelocity, &vVelocity );	

	return vVelocity;
}

void CPhysicalData::CheckObjectMove ()
{
	if ( m_emBestHigh == EMBH_OK )			m_emBestHigh = EMBH_NAVER;

	if ( m_emObjectMove == EMOM_GROUND )	return;

	if ( m_fTime < m_fHighTime )			m_emObjectMove = EMOM_UP;

	if ( m_emObjectMove == EMOM_UP )
	{
		if ( m_fTime >= m_fHighTime )
		{
			m_emObjectMove = EMOM_DOWN;

			if ( m_emBestHigh==EMBH_NONE )	m_emBestHigh = EMBH_OK;			
		}
	}
}

void CPhysicalData::ComPuteVelocity ( D3DXVECTOR3& vVelocity, const float fTime )
{
	if ( (m_vInitVelocity.x==0.f) && (m_vInitVelocity.y==0.f) && (m_vInitVelocity.z==0.f) )
		vVelocity = D3DXVECTOR3 ( 0.f, -0.001f, 0.f );

	vVelocity.x = m_vInitVelocity.x;
	vVelocity.z = m_vInitVelocity.z;
	vVelocity.y = m_vInitVelocity.y - (GRAVITY*fTime);	// v - gt 
}

void CPhysicalData::ComPuteHighData ()
{
	// 최고 높이 때 시간 구하기
	//	t = v/g
	if ( m_vInitVelocity.y <= 0.f )
	{
		m_emBestHigh	= EMBH_NAVER;
		m_fHighTime		= 0.f;
	}
	else
	{
		m_fHighTime = m_vInitVelocity.y/GRAVITY;
	}

	// 최고 높이 구하기
	//  h = z + v*t - ((1/2)*(g*t^2))
	//	h = z + (v^2/2g)
	m_fHighY = m_vInitPos.y + ((m_vInitVelocity.y*m_vInitVelocity.y)/(2.f*GRAVITY));
}

BOOL CPhysicalData::IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vNorDir, const D3DXVECTOR3& vVector, const int nColl )
{
	D3DXPLANE	sPlane;
	D3DXPlaneFromPointNormal ( &sPlane, &vTargetPOS, &vNorDir );

	float		fDis;
	D3DXVECTOR3 vNor;
	fDis = (sPlane.a*m_vPos_Prev.x) + (sPlane.b*m_vPos_Prev.y) + (sPlane.c*m_vPos_Prev.z) + sPlane.d;
	if ( fDis > 0.f )
	{
		// Note : 면과 충돌 되었는지 체크한다.
		fDis = (sPlane.a*m_vPos.x) + (sPlane.b*m_vPos.y) + (sPlane.c*m_vPos.z) + sPlane.d;
		//fDis = fabsf(fDis);		// 음수는 통과했다는 의미로 충돌이다.
		if ( fDis > m_fRadius )	return FALSE;		// 충돌 되지 않았다.

		vNor = vNorDir;
	}
	else
	{
		fDis = (sPlane.a*m_vPos.x) + (sPlane.b*m_vPos.y) + (sPlane.c*m_vPos.z) + sPlane.d;
		if ( fDis < -m_fRadius )	return FALSE;	// 충돌 되지 않았다.

		vNor = -vNorDir;
	}

	// Note : 충돌 되었다.
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vPos1 = vTargetPOS + (vNor*m_fRadius);
	D3DXVECTOR3 vPos2 = vPos1 + vVector;				// 머.. 픽킹이 이걸로 한거라서 크게 줄 필요는 없다.

	if ( D3DXPlaneIntersectLine ( &vPos, &sPlane, &vPos1, &vPos2 ) )
	{
		// 정확한 충돌 위치값을 셋팅해 준다.
		vPos = vPos1 - vPos;
		vPos = vTargetPOS + vPos;

		// 과도한 호출을 일으킬시에는 에러로 간주. 움직임을 멈춘다.
		if ( nColl > 10 )
		{
			// 아주 약간 올려준다. 붙어 있을 경우 바로 충돌이 일어나지 않도록 하기 위해서.
			m_vPos_Prev = vPos;
			m_vPos = vPos;

			m_emObjectMove = EMOM_DONTMOVE;
			m_emBestHigh = EMBH_NAVER;

			return FALSE;
		}

		// 반사 작업
		if ( IsReflect ( vPos, vNor ) )	return TRUE;
		else							return FALSE;
	}
	else
	{
		return FALSE;	// 평행하다.	무슨 작업을 해야 하지 ?
	}
}

BOOL CPhysicalData::IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vTargetNOR )
{
	float fTime;
	float fPowerXZ;
	//float fPowerY;
	D3DXVECTOR3	vDir;
	D3DXVECTOR3	vVelocity;

	if ( m_vInitVelocity.x )
	{
		fTime = (vTargetPOS.x-m_vInitPos.x)/m_vInitVelocity.x;	// 충돌 지점까지 가는 시간
	}
	else if ( m_vInitVelocity.z )
	{
		fTime = (vTargetPOS.z-m_vInitPos.z)/m_vInitVelocity.z;	// 충돌 지점까지 가는 시간
	}
	else if ( m_fHighY < vTargetPOS.y )
	{
		//m_vPos.y - m_vPos_Prev.y
		// 그 위치에 그냥 있어라.. 벽 박히는 것도 이상할꺼 같다.
		m_fTime			= 0.f;									// 초기값 셋팅
		m_vInitPos		= vTargetPOS + (vTargetNOR*m_fRadius);	// 초기값 셋팅 - 지면에서 약간 띄운다.
		m_vPos_Prev		= m_vInitPos;							// 충돌 체크를 가능하게 하기 위해서...
		m_vPos			= m_vInitPos;							// 초기값 셋팅
		m_vInitVelocity	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vVelocity		= m_vInitVelocity;						// 초기값 셋팅 - 탄성력이 여기서 들어감.
		ComPuteHighData ();										// 초기값 셋팅

		return FALSE;
	}
	else
	{
		// 최고위치는 발사 순간 계산이 되므로 올라갈 때 없다는 걱정은 안해도 된다.
		//	t = sqrtf ( (2*(최고위치-목표위치))/g );
		float fData;
		fData = (2.f*(m_fHighY-vTargetPOS.y))/GRAVITY;
		fTime = sqrtf( fData );

		if ( m_emBestHigh == EMBH_NONE )
		{
			fTime = m_fHighTime - fTime;
		}
		else	{}
		//
	}

	//if ( fTime <= 0.f )		// 이렇게 나오면 에러다. 어디 걸렸다.
	//{
	//	m_vPos_Prev = vTargetPOS + (vTargetNOR*0.01f);

	//	return FALSE;
	//}

	ComPuteVelocity ( vVelocity, fTime );					// 충돌 지점에서의 나온 벡터

	fPowerXZ = sqrtf( (vVelocity.x*vVelocity.x)+(vVelocity.y*vVelocity.y)+(vVelocity.z*vVelocity.z) );
	//fPowerY = fabsf ( vVelocity.y );
	D3DXVec3Normalize ( &vDir, &vVelocity );				// Normalize

	//	입사, 노멀 벡터는 단위 벡터여야 한다.
	//	반사		   입사	노멀
	//	R	= 2 * Dot3( -I * N ) * N + I 
	D3DXVECTOR3	_vDir = -vDir;
	float fDot3 = D3DXVec3Dot ( &_vDir, &vTargetNOR );

	//if ( fDot3 <= 0.f )		fDot3 = -fDot3;			// 반대 값을 주어서 이동하도록 한다.

	vDir = 2 * fDot3 * vTargetNOR + vDir;			// 반사 벡터 나옴45
	D3DXVec3Normalize ( &vDir, &vDir );				// 반사 벡터 나옴

	fTime = m_fTime - fTime;
//	if ( (fPowerY<1.0f) && (vDir.y>0.f) )	fPowerY = 0.f;	// 올라갈려고 할때만 올라가기 위한 최소 힘을 구한다.
	//if ( (fPowerXZ<0.1f) )					fPowerXZ = 0.f;	// 올라갈려고 할때만 올라가기 위한 최소 힘을 구한다.

	m_fTime				= 0.f;											// 초기값 셋팅
	m_vInitPos			= vTargetPOS;// + (vTargetNOR*m_fRadius);	// 초기값 셋팅 - 지면에서 약간 띄운다.
	m_vPos_Prev			= m_vInitPos;									// 충돌 체크를 가능하게 하기 위해서...
	m_vPos				= m_vInitPos;									// 초기값 셋팅
	m_vInitVelocity.x	= vDir.x*fPowerXZ;								// 초기값 셋팅 - 탄성력이 여기서 들어감.
	m_vInitVelocity.y	= vDir.y*fPowerXZ;						// 초기값 셋팅 - 탄성력이 여기서 들어감.
	m_vInitVelocity.z	= vDir.z*fPowerXZ;								// 초기값 셋팅 - 탄성력이 여기서 들어감.
	//m_vInitVelocity.x	= m_vInitVelocity.x - (m_vInitVelocity.x*m_fFriction*m_fElapsedTime);
	//m_vInitVelocity.z	= m_vInitVelocity.z - (m_vInitVelocity.z*m_fFriction*m_fElapsedTime);
	m_vVelocity			= m_vInitVelocity;								// 초기값 셋팅 - 탄성력이 여기서 들어감.
	ComPuteHighData ();													// 초기값 셋팅

	FrameMove ( fTime );
	//Render ();				// ERROR 수정해야함

	//// 이 때가 더 이상 올라기기 힘들 때 이다.
	//if ( fPowerY == 0.f ) 
	//{
	//	D3DXVECTOR3 vDir1 (0.f,0.f,0.f);
	//	D3DXVECTOR3 vDir2 (0.f,0.f,0.f);
	//	if ( (vTargetNOR.x==0.f) || (vTargetNOR.z==0.f) )
	//	{
	//		m_vPos = m_vInitPos + (m_vInitVelocity*m_fTime);
	//		CheckObjectMove ();
	//	}
	//	else
	//	{
	//		D3DXVECTOR3 vGravity ( 0.f, -GRAVITY, 0.f );
	//		D3DXVECTOR3 vDown ( 0.f, -1.f, 0.f );
	//		D3DXVECTOR3 vTemp;
	//		D3DXVec3Cross ( &vTemp, &vTargetNOR, &vDown );
	//		D3DXVec3Normalize ( &vTemp, &vTemp );
	//		D3DXVec3Cross ( &vDir1, &vTemp, &vTargetNOR );
	//		vDir1 = vDir1 * GRAVITY;
	//		vDir2 = vGravity - vDir1;

	//		m_vPos = m_vInitPos + (m_vInitVelocity*m_fTime) - (vDir1*m_fTime*m_fTime)  - (vDir2*m_fTime*m_fTime) ;	// (vt) - (1/2gt^2)
	//		CheckObjectMove ();
	//	}
	//}
	//else
	//{
		//	현재 시간의 위치값 계산
		m_vPos.x = m_vInitPos.x + (m_vInitVelocity.x*m_fTime);
		m_vPos.z = m_vInitPos.z + (m_vInitVelocity.z*m_fTime);
		m_vPos.y = m_vInitPos.y + (m_vInitVelocity.y*m_fTime) - (0.5f*GRAVITY*m_fTime*m_fTime);	// (vt) - (1/2gt^2)
		CheckObjectMove ();
	//}


	//if ( (m_vPos.y<vTargetPOS.y) && (vDir.y>=0.f) )		// 재계산 후에도 땅 아래에 있으면 더이상 올라갈 수 없는 것 이다.
	//{
	//	m_vPos.y		= vTargetPOS.y;
	//	m_vInitPos.y	= vTargetPOS.y;
	//	m_vPos_Prev		= m_vPos_Prev + (vTargetNOR*0.01f);	// 충돌 체크를 가능하게 하기 위해서...
	//	m_vDir.y		= 0.f;
	//	m_fHighTime		= 0.f;			// 높은것이 현재 값이다.
	//	m_fHighY		= m_vInitPos.y;	// 높은것이 현재 값이다.

	//	m_emObjectMove	= EMOM_GROUND;
	//	m_emBestHigh	= EMBH_NAVER;

	//	if ( (fPowerY==0.f) && (fPowerXZ==0.f) )	m_emObjectMove = EMOM_DONTMOVE;		// 움직일 수 없다.
	//}

	return TRUE;
}

namespace DXPHYSICAL
{
	// 맥스에서 작업한 1.75M 케릭은 게임으로 와선 17.5가 된다. 
	// 그러므로 중력은 10배로 되어야 데이터가 맞다.
	const float GRAVITY = 9.8f*10.f;		// 9.8m/s^2
	const float RISE_SOMEPOWER = 0.5f;		// 게임에서 2 현세계에서 5cm, 0.05M 이네.

	BOOL CHECK_SKY_GROUND ( float fElapsedTime, float& fPos, float& fTar, float& fDir, float& fVelocity )
	{
		float _fDir = fabsf(fDir);
		_fDir = _fDir*fVelocity;

		float fDistance = fTar - fPos;
		fDistance = fabsf(fDistance);

		if ( (_fDir<1.f) && (fDistance<1.f) )
		{
			fDir = 0.f;
			return FALSE;
		}

		return TRUE;
	}

	//void COMPUTE_FREEFALLING ( float fElapsedTime, CPhysicalData& sData )
	//{
	//	float fAirResistRate	= 1.f - (sData.m_fAirResist * fElapsedTime);
 //
	//	// vDir #1
	//	sData.m_vDir	= sData.m_vDir * sData.m_fVelocity;
	//	sData.m_vDir.y	= sData.m_vDir.y + (-GRAVITY * fElapsedTime);	// 중력

	//	// fVelocity
	//	sData.m_fVelocity = D3DXVec3Length ( &sData.m_vDir );
	//	sData.m_fVelocity = sData.m_fVelocity*fAirResistRate;			// 공기 저항
	//	
	//	// vDir #2
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );				// 방향의 정규화

	//	// vPos
	//	sData.m_vPos = sData.m_vPos + (sData.m_vDir*sData.m_fVelocity*fElapsedTime);	// 위치 = 전위치 + (방향*속도*시간)
	//}

	//void COMPUTE_GROUNDMOVE ( float fElapsedTime, CPhysicalData& sData, const float& _fHeight, const D3DXVECTOR3& vNor )
	//{
	//	float fAirResistRate	= 1.f - (sData.m_fAirResist * fElapsedTime);

	//	// 가중계수를 구한다.
	//	D3DXVECTOR3 vMoveDir = D3DXVECTOR3(0.f,-1.f,0.f);
	//	float fTemp = D3DXVec3Dot ( &vNor, &vMoveDir );
	//	if ( fTemp >= 0.f )		return;				// 이건 자유낙하 운동 쪽이다.

	//	fTemp = 1.f - fTemp;

	//	// 움직일 방향의 이동 벡터
	//	float fWidth;
	//	float fHeight;
	//	if ( (vNor.x==0.f) && (vNor.z==0.f) )
	//	{
	//		fWidth = 0.f;
	//		fHeight = 1.f;
	//	}
	//	else									
	//	{
	//		fWidth = sqrtf( (vNor.x*vNor.x) + (vNor.z*vNor.z) );
	//		fHeight = fabsf(vNor.y);

	//		fHeight = fHeight/fWidth;
	//	}

	//	vMoveDir.x = vNor.x * fHeight;
	//	vMoveDir.z = vNor.z * fHeight;
	//	vMoveDir.y = (vNor.y>=0.f) ? -fWidth : fWidth;

	//	// vDir #1
	//	sData.m_vDir = sData.m_vDir * sData.m_fVelocity;
	//	sData.m_vDir = sData.m_vDir + (vMoveDir * (GRAVITY * fTemp * fElapsedTime)); // 중력 * 가중계수 * 시간

	//	// fVelocity
	//	sData.m_fVelocity = D3DXVec3Length ( &sData.m_vDir );
	//	sData.m_fVelocity = sData.m_fVelocity*fAirResistRate;						// 공기 저항

	//	// vDir #2
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );							// 방향의 정규화

	//	// vPos
	//	sData.m_vPos	= sData.m_vPos + (sData.m_vDir * sData.m_fVelocity * fElapsedTime);		// 위치 = 전위치 + (방향*속도*시간)
	//	sData.m_vPos.y	= (sData.m_vPos.y > _fHeight) ? sData.m_vPos.y : _fHeight+0.01f;		// 약간 올려줘야 한다.
	//}

	D3DXVECTOR3 COMPUTE_DIR_V ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const float& fVelocity )
	{
		//						
		// 각도					거리 중력	초기속력
		//	a = 1/2 * (sin-1)( ( r  * g ) /  s^2 )

		D3DXVECTOR3 vDir = vEnd - vStart;
		float fLength = sqrtf((vDir.x*vDir.x) + (vDir.z*vDir.z));

		float fTemp = (fLength * GRAVITY) / (fVelocity * fVelocity);

		if ( fTemp>1.f )	fTemp = 1.f;

		fTemp = 0.5f * asinf(fTemp);			// 각도 나옴
		fTemp = (D3DX_PI*0.5f)-fTemp;

		float fWidth = cosf(fTemp) * fVelocity;	// 너비값

		vDir.y = sinf(fTemp) * fVelocity;		// 높이값

		vDir.x = (fWidth/fLength)*vDir.x;		// 비율로 나눔
		vDir.z = (fWidth/fLength)*vDir.z;		// 비율로 나눔

		D3DXVec3Normalize ( &vDir, &vDir );

		return vDir;
	}

	D3DXVECTOR3 COMPUTE_DIR_H ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fHeight )
	{
		D3DXVECTOR3 vDir = vEnd - vStart;
		D3DXVec3Normalize ( &vDir, &vDir );
		vDir.y += fHeight;
		D3DXVec3Normalize ( &vDir, &vDir );

		return vDir;
	}

	float COMPUTE_POWER ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const D3DXVECTOR3& _vDir )
	{
		//						
		//	초기속력	거리 중력	각도
		//	s = sqrt ( ( r  * g ) / sin(2a)  )

		float fLength2 = sqrtf((_vDir.x*_vDir.x) + (_vDir.z*_vDir.z));	// 각도 구하기
		float fAngel = atanf ( _vDir.y/fLength2 );						// 각도 구하기

		D3DXVECTOR3 vDir	= vEnd - vStart;
		float		fLength = sqrtf((vDir.x*vDir.x) + (vDir.z*vDir.z));
		float		fTemp	= (fLength * GRAVITY) / sinf ( 2*fAngel );	

		fTemp = sqrtf( fTemp );								// 초기속력 나옴

		return fTemp;
	}

	BOOL CHECK_PASS ( const D3DXVECTOR3& vInitDir, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTarget )
	{
		BOOL bUse = FALSE;

		D3DXVECTOR3 vDir = vTarget - vPos;

		float fDot3 = D3DXVec3Dot ( &vInitDir, &vDir );

		if ( fDot3 >= 0.f )	bUse = TRUE;

		return bUse;
	}

	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vNorDir )
	//{
	//	//	입사, 노멀 벡터는 단위 벡터여야 한다.
	//	//	반사		   입사	노멀
	//	//	R	= 2 * Dot3( -I * N ) * N + I 

	//	D3DXVECTOR3	_vDir = -sData.m_vDir;
	//	float fDot3 = D3DXVec3Dot ( &_vDir, &vNorDir );

	//	if ( fDot3 <= 0.f )		return FALSE;				// 방향을 생각한다.

	//	sData.m_vDir = 2 * fDot3 * vNorDir + sData.m_vDir;					// 반사 벡터 나옴
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );					// 반사 벡터 나옴

	//	sData.m_fVelocity = (sData.m_fVelocity * sData.m_fElastic);			// 속도 변함
	//	sData.m_fVelocity = (sData.m_fVelocity>0.f) ? sData.m_fVelocity : 0.f ;

	//	return TRUE;
	//}

	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vCrashPos, const D3DXVECTOR3& vPosPrev, const D3DXVECTOR3& vNorDir )
	//{
	//	//	입사, 노멀 벡터는 단위 벡터여야 한다.
	//	//	반사		   입사	노멀
	//	//	R	= 2 * Dot3( -I * N ) * N + I 

	//	D3DXVECTOR3	_vDir = -sData.m_vDir;
	//	float fDot3 = D3DXVec3Dot ( &_vDir, &vNorDir );

	//	if ( fDot3 <= 0.f )		return FALSE;					// 방향을 생각한다.

	//	sData.m_vDir = 2 * fDot3 * vNorDir + sData.m_vDir;						// 반사 벡터 나옴
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );						// 반사 벡터 나옴

	//	sData.m_fVelocity = (sData.m_fVelocity * sData.m_fElastic) - GRAVITY;				// 속도 변함
	//	sData.m_fVelocity = (sData.m_fVelocity>0.f) ? sData.m_fVelocity : 0.f ;			// 속도 변함

	//	//_vDir = vCrashPos - vPosPrev;
	//	//float fLength1 = D3DXVec3Length ( &_vDir );
	//	//_vDir = vCrashPos - vPos;
	//	//float fLength2 = D3DXVec3Length ( &_vDir );
	//	//fLength1 = ( fLength2 <= fLength1 ) ? fLength2 : fLength1;
	//	//if ( fLength1 == 0.f )	fLength1 = 0.01f;

	//	sData.m_vPos = vCrashPos + (sData.m_vDir * 0.0001f);				// 위치값 변해서 나옴

	//	return TRUE;
	//}
}


