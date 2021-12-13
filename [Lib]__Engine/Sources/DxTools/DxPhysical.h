#pragma once

class CPhysicalData
{
private:
	float		m_fTime;
	float		m_fElapsedTime;

private:
	static const float GRAVITY;

private:
	float		m_fFriction;		// 마찰계수			종류마다		- 로 계산.. 높으면 속도가 많이 떨어진다.
	float		m_fAirResist;		// 공기 저항력		종류마다		% 로 계산.. 높으면 속도가 많이 떨어진다.
	float		m_fElastic;			// 탄성력			종류마다		% 로 계산.. 높으면 많이 올라간다.

private:
	D3DXVECTOR3	m_vInitPos;			// 물체의 처음 위치
	D3DXVECTOR3	m_vInitVelocity;	// 물체의 처음 속도

private:
	enum	EMOBJECTMOVE
	{
		EMOM_UP			= 0,	// 올라갈 때
		EMOM_DOWN		= 1,	// 내려갈 때
		EMOM_GROUND		= 2,	// 지면 운동 할 때
		EMOM_DONTMOVE	= 3,	// 움직이지 않을 때
	};
	enum	EMBESTHIGH
	{
		EMBH_NONE	= 0,	//	아직 안되었다.
		EMBH_OK		= 1,	//	최고로 올라 갔다.
		EMBH_NAVER	= 2,	//	이젠 결코 최고가 될 수 없다.
	};
	EMOBJECTMOVE	m_emObjectMove;
	EMBESTHIGH		m_emBestHigh;

	float		m_fHighY;			// 물체의 최고 높이				// 초기 데이터(위치, 방향, 속력)로 계산이 나온다.
	float		m_fHighTime;		// 물체의 최고 높이때의 시간

private:
	D3DXVECTOR3	m_vDir;				// 물체의 이동 방향
	D3DXVECTOR3	m_vDirAccel;		// 물체의 가속도 방향
	D3DXVECTOR3	m_vVelocity;		// 물체의 속도
	float		m_fVelocity;		// 물체의 속력
	float		m_fAccel;			// 물체의 가속도

public:
	D3DXVECTOR3	m_vPos;				// 물체의 위치
	D3DXVECTOR3	m_vPos_Prev;		// 물체의 이전 위치
	float		m_fRadius;			// 물체의 반지름	사용자 정의

public:
	void		InitData ( D3DXVECTOR3 vPos, D3DXVECTOR3 vVelocity, float fRadius, float fFriction, float fElastic );

	void		FrameMove ( float fElapsedTime );

	void		Render ();
	void		Render ( D3DXVECTOR3 vEndPos, float fAccel );

	D3DXVECTOR3 GetVec3Direct ();

	// 물체의 크기 때문에 충돌위치가 틀릴 수 있다. 그것을 수정하기 위해서 이것이 있다. 음. 충돌위치는 Picking에 의해 나온 위치다.
	// vVector - 이전위치에서 현재 위치까지의 Vector.... 물체의 크기도 포함되어 있다.
	BOOL		IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vNorDir, const D3DXVECTOR3& vVector, const int nColl );

	BOOL		IsCheckBestHigh ()		{ return (m_emBestHigh==EMBH_OK) ? 1 : 0; }
	BOOL		IsCheckDontMove ()		{ return (m_emObjectMove==EMOM_DONTMOVE) ? 1 : 0; }

	void		SetDontMove()
	{
		m_emObjectMove = EMOM_DONTMOVE;
		m_emBestHigh = EMBH_NAVER;
	}

protected:
	// 방향벡터와.. 충돌물체의 노멀을 알면, 충돌후의 방향벡터를 얻을 수 있다. 추가로 탄성력으로 속도 값의 변화를 준다.
	BOOL		IsReflect ( const D3DXVECTOR3& vTargetPOS, const D3DXVECTOR3& vNorDir );

private:
	// 움직여 볼까나. ^^; 힘과 방향이면, 간다... 궤적을 알아내야 할꺼 같은.
	void		ComPuteVelocity ( D3DXVECTOR3& vVelocity, const float fTime );

	void		ComPuteHighData ();

	void		CheckObjectMove ();

public:
	CPhysicalData ();

	~CPhysicalData ();
};

namespace DXPHYSICAL
{
	extern const float GRAVITY;
	extern const float RISE_SOMEPOWER;

	// 자유낙하 운동에 의한 충돌시간 검출
	BOOL CHECK_SKY_GROUND ( float fElapsedTime, float& fPos, float& fTar, float& fDir, float& fVelocity );

	//// 자유낙하 운동
	//void COMPUTE_FREEFALLING ( float fElapsedTime, CPhysicalData& sData );

	//// 지면에서의 이동
	//void COMPUTE_GROUNDMOVE ( float fElapsedTime, CPhysicalData& sData, const float& _fHeight, const D3DXVECTOR3& vNor );

	// 시작 지점과 목표 지점을 알고, 속도도 알고,, 방향을 찾는다.
	D3DXVECTOR3 COMPUTE_DIR_V ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const float& fVelocity );

	// 시작 지점과 목표 지점을 알고, 각도 대충 정해서 방향을 찾는다.
	D3DXVECTOR3 COMPUTE_DIR_H ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fHeight );

	// 시작 지점과 목표 지점을 알고, 방향도 알고,, 힘을 찾는다.
	float COMPUTE_POWER ( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const D3DXVECTOR3& _vDir );

	// 방향벡터로 목표 지점을 통과 했는지 찾는다.
	BOOL CHECK_PASS ( const D3DXVECTOR3& vInitDir, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTarget );

	//// 방향벡터와.. 충돌물체의 노멀을 알면, 충돌후의 방향벡터를 얻을 수 있다. 추가로 탄성력으로 속도 값의 변화를 준다.
	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vNorDir );

	//BOOL COMPUTE_REFLECT_DIR ( CPhysicalData& sData, const D3DXVECTOR3& vCrashPos, const D3DXVECTOR3& vPosPrev, const D3DXVECTOR3& vNorDir );
};
