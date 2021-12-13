/*! \file WusDV_Util.cpp
	\brief File d'implementazione di alcune funzioni di utilit?generale.*/

#include "pch.h"
#include <math.h>
#include "WusDV_Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float RoundTo(float value, short decimal)
{
	value *= (float)pow(10.0, decimal);
	value += .5f;
	value = (float)floor(value);
	value /= (float)pow(10.0, decimal);
	return value;
}
