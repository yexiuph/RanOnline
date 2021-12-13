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

	//	���� �ð��� ��ġ�� ���
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

	//	���� �ð��� ��ġ�� ���
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
	// �ְ� ���� �� �ð� ���ϱ�
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

	// �ְ� ���� ���ϱ�
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
		// Note : ��� �浹 �Ǿ����� üũ�Ѵ�.
		fDis = (sPlane.a*m_vPos.x) + (sPlane.b*m_vPos.y) + (sPlane.c*m_vPos.z) + sPlane.d;
		//fDis = fabsf(fDis);		// ������ ����ߴٴ� �ǹ̷� �浹�̴�.
		if ( fDis > m_fRadius )	return FALSE;		// �浹 ���� �ʾҴ�.

		vNor = vNorDir;
	}
	else
	{
		fDis = (sPlane.a*m_vPos.x) + (sPlane.b*m_vPos.y) + (sPlane.c*m_vPos.z) + sPlane.d;
		if ( fDis < -m_fRadius )	return FALSE;	// �浹 ���� �ʾҴ�.

		vNor = -vNorDir;
	}

	// Note : �浹 �Ǿ���.
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vPos1 = vTargetPOS + (vNor*m_fRadius);
	D3DXVECTOR3 vPos2 = vPos1 + vVector;				// ��.. ��ŷ�� �̰ɷ� �ѰŶ� ũ�� �� �ʿ�� ����.

	if ( D3DXPlaneIntersectLine ( &vPos, &sPlane, &vPos1, &vPos2 ) )
	{
		// ��Ȯ�� �浹 ��ġ���� ������ �ش�.
		vPos = vPos1 - vPos;
		vPos = vTargetPOS + vPos;

		// ������ ȣ���� ����ų�ÿ��� ������ ����. �������� �����.
		if ( nColl > 10 )
		{
			// ���� �ణ �÷��ش�. �پ� ���� ��� �ٷ� �浹�� �Ͼ�� �ʵ��� �ϱ� ���ؼ�.
			m_vPos_Prev = vPos;
			m_vPos = vPos;

			m_emObjectMove = EMOM_DONTMOVE;
			m_emBestHigh = EMBH_NAVER;

			return FALSE;
		}

		// �ݻ� �۾�
		if ( IsReflect ( vPos, vNor ) )	return TRUE;
		else							return FALSE;
	}
	else
	{
		return FALSE;	// �����ϴ�.	���� �۾��� �ؾ� ���� ?
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
		fTime = (vTargetPOS.x-m_vInitPos.x)/m_vInitVelocity.x;	// �浹 �������� ���� �ð�
	}
	else if ( m_vInitVelocity.z )
	{
		fTime = (vTargetPOS.z-m_vInitPos.z)/m_vInitVelocity.z;	// �浹 �������� ���� �ð�
	}
	else if ( m_fHighY < vTargetPOS.y )
	{
		//m_vPos.y - m_vPos_Prev.y
		// �� ��ġ�� �׳� �־��.. �� ������ �͵� �̻��Ҳ� ����.
		m_fTime			= 0.f;									// �ʱⰪ ����
		m_vInitPos		= vTargetPOS + (vTargetNOR*m_fRadius);	// �ʱⰪ ���� - ���鿡�� �ణ ����.
		m_vPos_Prev		= m_vInitPos;							// �浹 üũ�� �����ϰ� �ϱ� ���ؼ�...
		m_vPos			= m_vInitPos;							// �ʱⰪ ����
		m_vInitVelocity	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vVelocity		= m_vInitVelocity;						// �ʱⰪ ���� - ź������ ���⼭ ��.
		ComPuteHighData ();										// �ʱⰪ ����

		return FALSE;
	}
	else
	{
		// �ְ���ġ�� �߻� ���� ����� �ǹǷ� �ö� �� ���ٴ� ������ ���ص� �ȴ�.
		//	t = sqrtf ( (2*(�ְ���ġ-��ǥ��ġ))/g );
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

	//if ( fTime <= 0.f )		// �̷��� ������ ������. ��� �ɷȴ�.
	//{
	//	m_vPos_Prev = vTargetPOS + (vTargetNOR*0.01f);

	//	return FALSE;
	//}

	ComPuteVelocity ( vVelocity, fTime );					// �浹 ���������� ���� ����

	fPowerXZ = sqrtf( (vVelocity.x*vVelocity.x)+(vVelocity.y*vVelocity.y)+(vVelocity.z*vVelocity.z) );
	//fPowerY = fabsf ( vVelocity.y );
	D3DXVec3Normalize ( &vDir, &vVelocity );				// Normalize

	//	�Ի�, ��� ���ʹ� ���� ���Ϳ��� �Ѵ�.
	//	�ݻ�		   �Ի�	���
	//	R	= 2 * Dot3( -I * N ) * N + I 
	D3DXVECTOR3	_vDir = -vDir;
	float fDot3 = D3DXVec3Dot ( &_vDir, &vTargetNOR );

	//if ( fDot3 <= 0.f )		fDot3 = -fDot3;			// �ݴ� ���� �־ �̵��ϵ��� �Ѵ�.

	vDir = 2 * fDot3 * vTargetNOR + vDir;			// �ݻ� ���� ����45
	D3DXVec3Normalize ( &vDir, &vDir );				// �ݻ� ���� ����

	fTime = m_fTime - fTime;
//	if ( (fPowerY<1.0f) && (vDir.y>0.f) )	fPowerY = 0.f;	// �ö󰥷��� �Ҷ��� �ö󰡱� ���� �ּ� ���� ���Ѵ�.
	//if ( (fPowerXZ<0.1f) )					fPowerXZ = 0.f;	// �ö󰥷��� �Ҷ��� �ö󰡱� ���� �ּ� ���� ���Ѵ�.

	m_fTime				= 0.f;											// �ʱⰪ ����
	m_vInitPos			= vTargetPOS;// + (vTargetNOR*m_fRadius);	// �ʱⰪ ���� - ���鿡�� �ణ ����.
	m_vPos_Prev			= m_vInitPos;									// �浹 üũ�� �����ϰ� �ϱ� ���ؼ�...
	m_vPos				= m_vInitPos;									// �ʱⰪ ����
	m_vInitVelocity.x	= vDir.x*fPowerXZ;								// �ʱⰪ ���� - ź������ ���⼭ ��.
	m_vInitVelocity.y	= vDir.y*fPowerXZ;						// �ʱⰪ ���� - ź������ ���⼭ ��.
	m_vInitVelocity.z	= vDir.z*fPowerXZ;								// �ʱⰪ ���� - ź������ ���⼭ ��.
	//m_vInitVelocity.x	= m_vInitVelocity.x - (m_vInitVelocity.x*m_fFriction*m_fElapsedTime);
	//m_vInitVelocity.z	= m_vInitVelocity.z - (m_vInitVelocity.z*m_fFriction*m_fElapsedTime);
	m_vVelocity			= m_vInitVelocity;								// �ʱⰪ ���� - ź������ ���⼭ ��.
	ComPuteHighData ();													// �ʱⰪ ����

	FrameMove ( fTime );
	//Render ();				// ERROR �����ؾ���

	//// �� ���� �� �̻� �ö��� ���� �� �̴�.
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
		//	���� �ð��� ��ġ�� ���
		m_vPos.x = m_vInitPos.x + (m_vInitVelocity.x*m_fTime);
		m_vPos.z = m_vInitPos.z + (m_vInitVelocity.z*m_fTime);
		m_vPos.y = m_vInitPos.y + (m_vInitVelocity.y*m_fTime) - (0.5f*GRAVITY*m_fTime*m_fTime);	// (vt) - (1/2gt^2)
		CheckObjectMove ();
	//}


	//if ( (m_vPos.y<vTargetPOS.y) && (vDir.y>=0.f) )		// ���� �Ŀ��� �� �Ʒ��� ������ ���̻� �ö� �� ���� �� �̴�.
	//{
	//	m_vPos.y		= vTargetPOS.y;
	//	m_vInitPos.y	= vTargetPOS.y;
	//	m_vPos_Prev		= m_vPos_Prev + (vTargetNOR*0.01f);	// �浹 üũ�� �����ϰ� �ϱ� ���ؼ�...
	//	m_vDir.y		= 0.f;
	//	m_fHighTime		= 0.f;			// �������� ���� ���̴�.
	//	m_fHighY		= m_vInitPos.y;	// �������� ���� ���̴�.

	//	m_emObjectMove	= EMOM_GROUND;
	//	m_emBestHigh	= EMBH_NAVER;

	//	if ( (fPowerY==0.f) && (fPowerXZ==0.f) )	m_emObjectMove = EMOM_DONTMOVE;		// ������ �� ����.
	//}

	return TRUE;
}

namespace DXPHYSICAL
{
	// �ƽ����� �۾��� 1.75M �ɸ��� �������� �ͼ� 17.5�� �ȴ�. 
	// �׷��Ƿ� �߷��� 10��� �Ǿ�� �����Ͱ� �´�.
	const float GRAVITY = 9.8f*10.f;		// 9.8m/s^2
	const float RISE_SOMEPOWER = 0.5f;		// ���ӿ��� 2 �����迡�� 5cm, 0.05M �̳�.

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
	//	sData.m_vDir.y	= sData.m_vDir.y + (-GRAVITY * fElapsedTime);	// �߷�

	//	// fVelocity
	//	sData.m_fVelocity = D3DXVec3Length ( &sData.m_vDir );
	//	sData.m_fVelocity = sData.m_fVelocity*fAirResistRate;			// ���� ����
	//	
	//	// vDir #2
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );				// ������ ����ȭ

	//	// vPos
	//	sData.m_vPos = sData.m_vPos + (sData.m_vDir*sData.m_fVelocity*fElapsedTime);	// ��ġ = ����ġ + (����*�ӵ�*�ð�)
	//}

	//void COMPUTE_GROUNDMOVE ( float fElapsedTime, CPhysicalData& sData, const float& _fHeight, const D3DXVECTOR3& vNor )
	//{
	//	float fAirResistRate	= 1.f - (sData.m_fAirResist * fElapsedTime);

	//	// ���߰���� ���Ѵ�.
	//	D3DXVECTOR3 vMoveDir = D3DXVECTOR3(0.f,-1.f,0.f);
	//	float fTemp = D3DXVec3Dot ( &vNor, &vMoveDir );
	//	if ( fTemp >= 0.f )		return;				// �̰� �������� � ���̴�.

	//	fTemp = 1.f - fTemp;

	//	// ������ ������ �̵� ����
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
	//	sData.m_vDir = sData.m_vDir + (vMoveDir * (GRAVITY * fTemp * fElapsedTime)); // �߷� * ���߰�� * �ð�

	//	// fVelocity
	//	sData.m_fVelocity = D3DXVec3Length ( &sData.m_vDir );
	//	sData.m_fVelocity = sData.m_fVelocity*fAirResistRate;						// ���� ����

	//	// vDir #2
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );							// ������ ����ȭ

	//	// vPos
	//	sData.m_vPos	= sData.m_vPos + (sData.m_vDir * sData.m_fVelocity * fElapsedTime);		// ��ġ = ����ġ + (����*�ӵ�*�ð�)
	//	sData.m_vPos.y	= (sData.m_vPos.y > _fHeight) ? sData.m_vPos.y : _fHeight+0.01f;		// �ణ �÷���� �Ѵ�.
	//}

	D3DXVECTOR3 COMPUTE_DIR_V ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const float& fVelocity )
	{
		//						
		// ����					�Ÿ� �߷�	�ʱ�ӷ�
		//	a = 1/2 * (sin-1)( ( r  * g ) /  s^2 )

		D3DXVECTOR3 vDir = vEnd - vStart;
		float fLength = sqrtf((vDir.x*vDir.x) + (vDir.z*vDir.z));

		float fTemp = (fLength * GRAVITY) / (fVelocity * fVelocity);

		if ( fTemp>1.f )	fTemp = 1.f;

		fTemp = 0.5f * asinf(fTemp);			// ���� ����
		fTemp = (D3DX_PI*0.5f)-fTemp;

		float fWidth = cosf(fTemp) * fVelocity;	// �ʺ�

		vDir.y = sinf(fTemp) * fVelocity;		// ���̰�

		vDir.x = (fWidth/fLength)*vDir.x;		// ������ ����
		vDir.z = (fWidth/fLength)*vDir.z;		// ������ ����

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
		//	�ʱ�ӷ�	�Ÿ� �߷�	����
		//	s = sqrt ( ( r  * g ) / sin(2a)  )

		float fLength2 = sqrtf((_vDir.x*_vDir.x) + (_vDir.z*_vDir.z));	// ���� ���ϱ�
		float fAngel = atanf ( _vDir.y/fLength2 );						// ���� ���ϱ�

		D3DXVECTOR3 vDir	= vEnd - vStart;
		float		fLength = sqrtf((vDir.x*vDir.x) + (vDir.z*vDir.z));
		float		fTemp	= (fLength * GRAVITY) / sinf ( 2*fAngel );	

		fTemp = sqrtf( fTemp );								// �ʱ�ӷ� ����

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
	//	//	�Ի�, ��� ���ʹ� ���� ���Ϳ��� �Ѵ�.
	//	//	�ݻ�		   �Ի�	���
	//	//	R	= 2 * Dot3( -I * N ) * N + I 

	//	D3DXVECTOR3	_vDir = -sData.m_vDir;
	//	float fDot3 = D3DXVec3Dot ( &_vDir, &vNorDir );

	//	if ( fDot3 <= 0.f )		return FALSE;				// ������ �����Ѵ�.

	//	sData.m_vDir = 2 * fDot3 * vNorDir + sData.m_vDir;					// �ݻ� ���� ����
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );					// �ݻ� ���� ����

	//	sData.m_fVelocity = (sData.m_fVelocity * sData.m_fElastic);			// �ӵ� ����
	//	sData.m_fVelocity = (sData.m_fVelocity>0.f) ? sData.m_fVelocity : 0.f ;

	//	return TRUE;
	//}

	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vCrashPos, const D3DXVECTOR3& vPosPrev, const D3DXVECTOR3& vNorDir )
	//{
	//	//	�Ի�, ��� ���ʹ� ���� ���Ϳ��� �Ѵ�.
	//	//	�ݻ�		   �Ի�	���
	//	//	R	= 2 * Dot3( -I * N ) * N + I 

	//	D3DXVECTOR3	_vDir = -sData.m_vDir;
	//	float fDot3 = D3DXVec3Dot ( &_vDir, &vNorDir );

	//	if ( fDot3 <= 0.f )		return FALSE;					// ������ �����Ѵ�.

	//	sData.m_vDir = 2 * fDot3 * vNorDir + sData.m_vDir;						// �ݻ� ���� ����
	//	D3DXVec3Normalize ( &sData.m_vDir, &sData.m_vDir );						// �ݻ� ���� ����

	//	sData.m_fVelocity = (sData.m_fVelocity * sData.m_fElastic) - GRAVITY;				// �ӵ� ����
	//	sData.m_fVelocity = (sData.m_fVelocity>0.f) ? sData.m_fVelocity : 0.f ;			// �ӵ� ����

	//	//_vDir = vCrashPos - vPosPrev;
	//	//float fLength1 = D3DXVec3Length ( &_vDir );
	//	//_vDir = vCrashPos - vPos;
	//	//float fLength2 = D3DXVec3Length ( &_vDir );
	//	//fLength1 = ( fLength2 <= fLength1 ) ? fLength2 : fLength1;
	//	//if ( fLength1 == 0.f )	fLength1 = 0.01f;

	//	sData.m_vPos = vCrashPos + (sData.m_vDir * 0.0001f);				// ��ġ�� ���ؼ� ����

	//	return TRUE;
	//}
}


