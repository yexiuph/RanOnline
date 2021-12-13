// "Portions Copyright (C) Steve Rabin, 2000"
//	Edit by Jung DH, 2002.
//
#include "pch.h"
#include "./profile.h"
#include "./DebugSet.h"
#include "./DxInputDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPROFILE	g_profile_1;
CPROFILE	g_profile_2;
BOOL		g_bProfile = FALSE;

namespace
{
	#define _HISTORY_RATE_		( 5000.0f )
	#define _GETCURTIME_		( timeGetTime() )
	#define _GETELAPSEDTIME_	( (float)( m_endProfile - m_startProfile ) )
};

CPROFILE::CPROFILE () 
	: m_startProfile(0)
	, m_endProfile(0)
{
}

CPROFILE::~CPROFILE ()
{
}

void CPROFILE::Init (void)
{
	unsigned int i;

	for( i=0; i<NUM_PROFILE_SAMPLES; i++ )
	{
		m_samples[i].bValid = false;
		m_history[i].bValid = false;
	}

	m_startProfile = _GETCURTIME_;
}

void CPROFILE::BlockStart ()
{
	if ( m_startProfile==0 )	return;

	unsigned int i;	//	Reset samples for next frame
	
	for (i=0; i<NUM_PROFILE_SAMPLES; ++i)
	{
		m_samples[i].bValid = false;
	}

	DxInputDevice &IDev = DxInputDevice::GetInstance();
	if (IDev.GetKeyState(DIK_LCONTROL)&DXKEY_PRESSED)
	{
		if (IDev.GetKeyState(DIK_F12)&DXKEY_UP)
		{
			for (i=0; i<NUM_PROFILE_SAMPLES; ++i)
			{
				m_history[i].bValid = false;
			}
		}
	}

	m_startProfile = _GETCURTIME_;
}

void CPROFILE::BlockEnd ()
{
	if (m_startProfile==0)
	{
		return;
	}
	else
	{
		m_endProfile = _GETCURTIME_;
	}
}

void CPROFILE::Begin(char* name)
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i = 0;

	while ( i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true )
	{
		if( strcmp( m_samples[i].szName, name ) == 0 )	//Found the sample
		{
			m_samples[i].iOpenProfiles++;
			m_samples[i].iProfileInstances++;
			m_samples[i].fStartTime = _GETCURTIME_;
			GASSERT( m_samples[i].iOpenProfiles == 1 );	//max 1 open at once
			return;
		}
		i++;	
	}

	if( i >= NUM_PROFILE_SAMPLES )
	{
		GASSERT( !"Exceeded Max Available Profile Samples" );
		return;
	}

	StringCchCopy( m_samples[i].szName, 256, name );
	m_samples[i].bValid = true;
	m_samples[i].iOpenProfiles = 1;
	m_samples[i].iProfileInstances = 1;
	m_samples[i].fAccumulator = 0;
	m_samples[i].fStartTime = _GETCURTIME_;
	m_samples[i].fChildrenSampleTime = 0;
}

void CPROFILE::End ( char* name )
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i = 0;
	unsigned int numParents = 0;

	while (i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true)
	{
		if (strcmp(m_samples[i].szName, name) == 0)	//Found the sample
		{
			unsigned int inner = 0;
			int parent = -1;
			unsigned long fEndTime = _GETCURTIME_;
			m_samples[i].iOpenProfiles--;

			while (m_samples[inner].bValid == true)		//Count all parents and find the immediate parent
			{
				if (m_samples[inner].iOpenProfiles > 0)	//Found a parent (any open profiles are parents)
				{
					numParents++;
					if (parent < 0)	//Replace invalid parent (index)
					{
						parent = inner;
					}
					else if (m_samples[inner].fStartTime >= m_samples[parent].fStartTime)	//Replace with more immediate parent
					{
						parent = inner;
					}
				}
				inner++;
			}

			m_samples[i].iNumParents = numParents;	//Remember the current number of parents of the sample

			if (parent >= 0)
			{
				m_samples[parent].fChildrenSampleTime += fEndTime -	m_samples[i].fStartTime;	//Record this time in fChildrenSampleTime (add it in)
			}

			m_samples[i].fAccumulator += fEndTime - m_samples[i].fStartTime;	//Save sample time in accumulator
			return;
		} // if
		i++;
	} // while
}

void CPROFILE::DumpOutputToView (int _OUTCHANNEL)
{
	if ( m_startProfile==0 )	return;

	unsigned int i=0;

	float fDxTIME = _GETELAPSEDTIME_;
	if (fDxTIME <= 0.0f)
		return;

	CDebugSet::ToView ( _OUTCHANNEL, 0, "  Ave :   Min :   Max :   # : Profile Name" );
	CDebugSet::ToView ( _OUTCHANNEL, 1, "--------------------------------------------" );

	while (i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true)
	{		
		unsigned int indent = 0;
		float sampleTime = 0;
		float percentTime = 0;
		float aveTime = 0;
		float minTime = 0;
		float maxTime = 0;
		char name[256] = "";
		char indentedName[256] = "";
		char ave[16] = "";
		char min[16] = "";
		char max[16] = "";
		char num[16] = "";
				
		if (m_samples[i].iOpenProfiles < 0)
		{
			char szDump[256] = "";
			StringCchPrintf(szDump, 256, "%s, ProfileEnd() called without a ProfileBegin()", m_samples[i].szName);
			GASSERT (!szDump);
		}
		else if (m_samples[i].iOpenProfiles > 0)
		{
			char szDump[256] = "";
			StringCchPrintf(szDump, 256, "%s, ProfileBegin() called without a ProfileEnd()", m_samples[i].szName);
			GASSERT (!szDump);
		}

		sampleTime = (float)( m_samples[i].fAccumulator - m_samples[i].fChildrenSampleTime );
		if (sampleTime < 0)
		{
			sampleTime = 0;
		}

		percentTime = ( ( sampleTime / fDxTIME ) * 100.0f );
		if (percentTime < 0)
		{
			percentTime = 0;
		}

		aveTime = minTime = maxTime = percentTime;

		//Add new measurement into the history and get the ave, min, and max
		StoreInHistory ( m_samples[i].szName, percentTime );
		GetFromHistory ( m_samples[i].szName, &aveTime, &minTime, &maxTime );

		//Format the data
		StringCchPrintf( ave, 16, "%3.1f", aveTime );
		StringCchPrintf( min, 16, "%3.1f", minTime );
		StringCchPrintf( max, 16, "%3.1f", maxTime );
		StringCchPrintf( num, 16, "%3d", m_samples[i].iProfileInstances );

		StringCchCopy (indentedName, 256, m_samples[i].szName);
		for (indent=0; indent<m_samples[i].iNumParents; ++indent)
		{
			StringCchPrintf(name, 256, "   %s", indentedName);
			StringCchCopy(indentedName, 256, name);
		}

		CDebugSet::ToView( _OUTCHANNEL, 2+i, "%5s : %5s : %5s : %3s : %s", ave, min, max, num, indentedName );
		i++;
	} // while
	
	CDebugSet::ToView(_OUTCHANNEL, 2+i+0, "--------------------------------------------");
	CDebugSet::ToView(_OUTCHANNEL, 2+i+1, "Reset Stats : Ctrl + F12");
}

void CPROFILE::DumpOutputToFile ()
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i=0;

	float fDxTIME = _GETELAPSEDTIME_;
	if (fDxTIME <= 0.0f)
		return;

	CDebugSet::ToLogFile ( "  Ave :   Min :   Max :   # : Profile Name" );
	CDebugSet::ToLogFile ( "--------------------------------------------" );

	while (i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true)
	{		
		unsigned int indent = 0;
		float sampleTime = 0;
		float percentTime = 0;
		float aveTime = 0;
		float minTime = 0;
		float maxTime = 0;
		char name[256] = "";
		char indentedName[256] = "";
		char ave[16] = "";
		char min[16] = "";
		char max[16] = "";
		char num[16] = "";

		if (m_samples[i].iOpenProfiles < 0)
		{
			char szDump[256] = "";
			StringCchPrintf(szDump,
				            256, 
						    _T("%s, ProfileEnd() called without a ProfileBegin()"),
						    m_samples[i].szName);
			GASSERT(!szDump);
		}
		else if (m_samples[i].iOpenProfiles > 0)
		{
			char szDump[256] = "";
			StringCchPrintf (szDump,
				             256,
							 _T("%s, ProfileBegin() called without a ProfileEnd()"),
							 m_samples[i].szName);
			GASSERT ( !szDump );
		}

		sampleTime = (float)( m_samples[i].fAccumulator - m_samples[i].fChildrenSampleTime );
		if (sampleTime < 0)
		{
			sampleTime = 0;
		}

		percentTime = ( ( sampleTime / fDxTIME ) * 100.0f );
		if (percentTime < 0)
		{
			percentTime = 0;
		}

		aveTime = minTime = maxTime = percentTime;

		//Add new measurement into the history and get the ave, min, and max
		StoreInHistory ( m_samples[i].szName, percentTime );
		GetFromHistory ( m_samples[i].szName, &aveTime, &minTime, &maxTime );

		//Format the data
		StringCchPrintf( ave, 16, "%3.1f", aveTime );
		StringCchPrintf( min, 16, "%3.1f", minTime );
		StringCchPrintf( max, 16, "%3.1f", maxTime );
		StringCchPrintf( num, 16, "%3d", m_samples[i].iProfileInstances );

		StringCchCopy( indentedName, 256, m_samples[i].szName );
		for( indent=0; indent<m_samples[i].iNumParents; indent++ )
		{
			StringCchPrintf ( name, 256, "   %s", indentedName );
			StringCchCopy( indentedName, 256, name );
		}

		CDebugSet::ToLogFile( "%5s : %5s : %5s : %3s : %s", ave, min, max, num, indentedName );

		i++;
	} // while
	
	CDebugSet::ToLogFile ( "--------------------------------------------" );
}

void CPROFILE::DumpOutputToNon ()
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i=0;

	float fDxTIME = _GETELAPSEDTIME_;
	if (fDxTIME <= 0.0f)
		return;

	while (i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true)
	{		
		unsigned int indent = 0;
		float sampleTime = 0;
		float percentTime = 0;
		float aveTime = 0;
		float minTime = 0;
		float maxTime = 0;

		if ( m_samples[i].iOpenProfiles < 0 )
		{
			char szDump[256] = "";
			StringCchPrintf( szDump, 256, "%s, ProfileEnd() called without a ProfileBegin()", m_samples[i].szName );
			GASSERT ( !szDump );	
		}
		else if ( m_samples[i].iOpenProfiles > 0 )
		{
			char szDump[256] = "";
			StringCchPrintf( szDump, 256, "%s, ProfileBegin() called without a ProfileEnd()", m_samples[i].szName );
			GASSERT ( !szDump );
		}

		sampleTime = (float)( m_samples[i].fAccumulator - m_samples[i].fChildrenSampleTime );
		if (sampleTime < 0)
		{
			sampleTime = 0;
		}

		percentTime = ( ( sampleTime / fDxTIME ) * 100.0f );
		if (percentTime < 0)
		{
			percentTime = 0;
		}

		aveTime = minTime = maxTime = percentTime;

		//Add new measurement into the history and get the ave, min, and max
		StoreInHistory(m_samples[i].szName, percentTime);
		GetFromHistory(m_samples[i].szName, &aveTime, &minTime, &maxTime);

		i++;
	} // while
}

void CPROFILE::StoreInHistory ( char* name, float percent )
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i = 0;
	float oldRatio = 0;
	float newRatio = _GETELAPSEDTIME_ / _HISTORY_RATE_;

	if ( newRatio > 1.0f )
	{
		newRatio = 1.0f;
	}
	oldRatio = 1.0f - newRatio;

	while (i < NUM_PROFILE_SAMPLES && m_history[i].bValid == true)
	{
		if (strcmp(m_history[i].szName, name) == 0)	//Found the sample
		{
			m_history[i].fAve = (m_history[i].fAve*oldRatio) + (percent*newRatio);

			if ( percent < m_history[i].fMin )
			{
				m_history[i].fMin = percent;
			}
			else
			{
				m_history[i].fMin = (m_history[i].fMin*oldRatio) + (percent*newRatio);
			}

			if ( m_history[i].fMin < 0.0f )
			{
				m_history[i].fMin = 0.0f;
			}

			if ( percent > m_history[i].fMax )
			{
				m_history[i].fMax = percent;
			}
			else
			{
				m_history[i].fMax = (m_history[i].fMax*oldRatio) + (percent*newRatio);
			}

			return;
		} // if
		i++;
	} // while

	if (i < NUM_PROFILE_SAMPLES)	//Add to history
	{
		StringCchCopy( m_history[i].szName, 256, name );
		m_history[i].bValid = true;
		m_history[i].fAve = m_history[i].fMin = m_history[i].fMax = percent;
	}
	else
	{
		GASSERT ( !"Exceeded Max Available Profile Samples!");
	}
}

void CPROFILE::GetFromHistory ( char* name, float* ave, float* min, float* max )
{
	if (m_startProfile==0)
	{
		return;
	}

	unsigned int i = 0;
	while (i < NUM_PROFILE_SAMPLES && m_history[i].bValid == true)
	{
		if (strcmp(m_history[i].szName, name) == 0)
		{  //Found the sample
			*ave = m_history[i].fAve;
			*min = m_history[i].fMin;
			*max = m_history[i].fMax;
			return;
		}
		i++;
	}
	*ave = *min = *max = 0.0f;
}