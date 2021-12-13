#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "Navigations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Navigations::Navigations()
{
}

Navigations::~Navigations()
{
	CleanUp();
}

void Navigations::CleanUp()
{
	std::for_each( m_mapNavigations.begin(), m_mapNavigations.end(), std_afunc::DeleteMapObject() );
	m_mapNavigations.clear();
}

