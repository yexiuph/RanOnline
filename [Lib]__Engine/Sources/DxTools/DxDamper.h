#pragma once

template<class TYPE >
class DxDamper
{
protected:
	float	m_fDamping;
	TYPE	m_OldVelocity;

public:
	void SetDamping ( float fDamping )
	{
		m_fDamping = fDamping ;
	}

public:
	TYPE Calculate ( TYPE NewVel, float fElapsedTime )
	{
		float fElap = ( fElapsedTime * 10.0f );
		fElap = min(fElap,0.4f);

		m_OldVelocity = m_OldVelocity + ( NewVel - m_OldVelocity ) * m_fDamping * fElap;
		return m_OldVelocity;
	}

public:
	DxDamper () :
		m_fDamping(0.0f)
	{
	}
	DxDamper ( float fDamping, TYPE InitVel ) :
		m_fDamping(fDamping),
		m_OldVelocity(InitVel)
	{
	}

	~DxDamper ()
	{
	}
};


