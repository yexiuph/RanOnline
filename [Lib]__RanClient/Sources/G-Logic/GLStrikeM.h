#ifndef GLSTRIKEM_H_
#define GLSTRIKEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace STRIKE
{
	struct SSTRIKE
	{
		float		fLeng;
		D3DXVECTOR3	vPos;

		SSTRIKE () :
			fLeng(0.0f),
			vPos(0,0,0)
		{
		}

		SSTRIKE ( D3DXVECTOR3 _vpos, D3DXVECTOR3 _vtarget ) :
			vPos(_vpos)
		{
			D3DXVECTOR3 vLeng = _vtarget - vPos;
			fLeng = D3DXVec3LengthSq ( &vLeng );
		}
	};

	class SSTRIKE_COMP
	{
	public:
		SSTRIKE_COMP () {}

	public:
		bool operator() ( const SSTRIKE &lvalue, const SSTRIKE &rvalue )
		{
			return lvalue.fLeng < rvalue.fLeng;
		}
	};

	static const int nSTRIKE_SIZE=4;
	inline std::string GETSTRIKE_NAME ( int nIndex )
	{
		if ( nIndex >= nSTRIKE_SIZE )	return std::string("");

		static std::string strSTRIKES[nSTRIKE_SIZE] =
		{
			STRACE_STRIKE01,
			STRACE_STRIKE02,
			STRACE_STRIKE11,
			STRACE_STRIKE12
		};

		return strSTRIKES[nIndex];
	}

	class CSELECTOR
	{
	public:
		typedef std::vector<SSTRIKE>	STRIKEARRAY;
		typedef STRIKEARRAY::iterator	STRIKEARRAY_ITER;

	protected:
		D3DXVECTOR3				m_vTARPOS;
		DxCharPart*				m_pCharPart;

		STRIKEARRAY				m_arrayStrikes;

	public:
		CSELECTOR ( const D3DXVECTOR3 &_tarpos, DxCharPart *_pcharpart ) :
			m_vTARPOS(_tarpos),
			m_pCharPart(_pcharpart)
		{
		}

	protected:
		void SLOT_SELECT ( EMPIECECHAR emPIECE )
		{
			D3DXVECTOR3		vNORMAL;
			D3DXVECTOR3		vSTRIKE;

			for ( int i=0; i<nSTRIKE_SIZE; ++i )
			{
				if ( m_pCharPart[emPIECE].GetTracePos(GETSTRIKE_NAME(i)) )
				{
					m_pCharPart[emPIECE].CalculateVertexInflu ( GETSTRIKE_NAME(i), vSTRIKE, vNORMAL );
					m_arrayStrikes.push_back ( SSTRIKE(vSTRIKE,m_vTARPOS) );
				}
			}
		}

	public:
		BOOL SELECT ( const SANIMSTRIKE &sStrikeEff, SSTRIKE &sStrike )
		{
			m_arrayStrikes.clear();
			SLOT_SELECT(sStrikeEff.m_emPiece);

			if ( m_arrayStrikes.empty() )	return FALSE;

			std::sort ( m_arrayStrikes.begin(), m_arrayStrikes.end(), SSTRIKE_COMP() );

			//	타격지점 찾기.
			sStrike = *m_arrayStrikes.begin();

			return TRUE;
		}
	};
};

namespace IMPACT_ARROW
{
	struct SSTRIKE
	{
		float		fLeng;
		D3DXVECTOR3	vPos;

		SSTRIKE () :
			fLeng(0.0f),
			vPos(0,0,0)
		{
		}

		SSTRIKE ( D3DXVECTOR3 _vpos, D3DXVECTOR3 _vtarget ) :
			vPos(_vpos)
		{
			D3DXVECTOR3 vLeng = _vtarget - vPos;
			fLeng = D3DXVec3LengthSq ( &vLeng );
		}
	};

	class SSTRIKE_COMP
	{
	public:
		SSTRIKE_COMP () {}

	public:
		bool operator() ( const SSTRIKE &lvalue, const SSTRIKE &rvalue )
		{
			return lvalue.fLeng < rvalue.fLeng;
		}
	};

	static const int nSTRIKE_SIZE=1;
	inline std::string GETSTRIKE_NAME ( int nIndex )
	{
		if ( nIndex >= nSTRIKE_SIZE )	return std::string("");

		static std::string strSTRIKES[nSTRIKE_SIZE] =
		{
			STRACE_ARROW01
		};

		return strSTRIKES[nIndex];
	}

	class CSELECTOR
	{
	public:
		typedef std::vector<SSTRIKE>	STRIKEARRAY;
		typedef STRIKEARRAY::iterator	STRIKEARRAY_ITER;

	protected:
		D3DXVECTOR3				m_vTARPOS;
		DxCharPart*				m_pCharPart;

		STRIKEARRAY				m_arrayStrikes;

	public:
		CSELECTOR ( const D3DXVECTOR3 &_tarpos, DxCharPart *_pcharpart ) :
			m_vTARPOS(_tarpos),
			m_pCharPart(_pcharpart)
		{
		}

	protected:
		void SLOT_SELECT ( EMPIECECHAR emPIECE )
		{
			D3DXVECTOR3		vNORMAL;
			D3DXVECTOR3		vSTRIKE;

			for ( int i=0; i<nSTRIKE_SIZE; ++i )
			{
				if ( m_pCharPart[emPIECE].GetTracePos(GETSTRIKE_NAME(i)) )
				{
					m_pCharPart[emPIECE].CalculateVertexInflu ( GETSTRIKE_NAME(i), vSTRIKE, vNORMAL );
					m_arrayStrikes.push_back ( SSTRIKE(vSTRIKE,m_vTARPOS) );
				}
			}
		}

	public:
		BOOL SELECT ( SSTRIKE &sStrike )
		{
			m_arrayStrikes.clear();
			for ( int i=0; i<PIECE_SIZE; ++i )		SLOT_SELECT ( EMPIECECHAR(i) );

			if ( m_arrayStrikes.empty() )	return FALSE;

			std::sort ( m_arrayStrikes.begin(), m_arrayStrikes.end(), SSTRIKE_COMP() );

			//	타격지점 찾기.
			sStrike = *m_arrayStrikes.begin();

			return TRUE;
		}
	};
};

#endif // GLSTRIKEM_H_