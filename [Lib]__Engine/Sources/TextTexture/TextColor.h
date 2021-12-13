#ifndef _TEXT_COLOR_H_
#define _TEXT_COLOR_H_

#pragma once

/**
 * \struct	COLOR_16
 * \date	2007/11/30
 * \author	Jun-Hyuk Choi
 * \brief	A4R4G4B4 색상 구조체 ( 0 ~ 15 )
 */
struct COLOR_16
{
	WORD argb;

	COLOR_16()
		: argb( 0 )
	{
	}

	COLOR_16( WORD _argb )
		: argb( _argb )
	{
	}

	COLOR_16( BYTE a, BYTE r, BYTE g, BYTE b )
	{
		argb = (WORD)( ( ( a & 0xF ) << 12 ) | ( ( r & 0xF ) << 8 ) | ( ( g & 0xF ) << 4 ) | ( b & 0xF ) );
	}

	operator WORD() const
	{
		return argb;
	}

	COLOR_16& operator = ( const WORD& _argb )
	{
		argb = _argb;
		return *this;
	}

	BYTE GetA()	{ return (BYTE)( ( argb >> 12 ) & 0xF ); }
	BYTE GetR()	{ return (BYTE)( ( argb >> 8 ) & 0xF ); }
	BYTE GetG()	{ return (BYTE)( ( argb >> 4 ) & 0xF ); }
	BYTE GetB()	{ return (BYTE)( argb & 0xF ); }
};

/**
 * \struct	COLOR_32
 * \date	2007/11/30
 * \author	Jun-Hyuk Choi
 * \brief	A8R8G8B8 색상 구조체 ( 0 ~ 255 )
 */
struct COLOR_32
{
	DWORD argb;

	COLOR_32()
		: argb( 0 )
	{
	}

	COLOR_32( DWORD _argb )
		: argb( _argb )
	{
	}

	COLOR_32( BYTE a, BYTE r, BYTE g, BYTE b )
	{
		argb = (DWORD)( ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b ) );
	}

	operator DWORD() const
	{
		return argb;
	}

	COLOR_32& operator = ( const DWORD& _argb )
	{
		argb = _argb;
		return *this;
	}

	BYTE GetA()	{ return (BYTE)( argb >> 24 ); }
	BYTE GetR()	{ return (BYTE)( argb >> 16 ); }
	BYTE GetG()	{ return (BYTE)( argb >> 8 ); }
	BYTE GetB()	{ return (BYTE)( argb ); }
};

/**
 * \func	Conv16to32
 * \date	2007/11/30
 * \author	Jun-Hyuk Choi
 * \see		TextTexture16.cpp
 * \brief	A4R4G4B4 색상을 A8R8G8B8 색상으로 변환 ( 무손실 )
 */
extern COLOR_32 Conv16to32( COLOR_16 src );

/**
 * \func	Conv32to16
 * \date	2007/11/30
 * \author	Jun-Hyuk Choi
 * \see		TextTexture16.cpp
 * \brief	A8R8G8B8 색상을 A4R4G4B4 색상으로 변환 ( 손실 )
 */
extern COLOR_16 Conv32to16( COLOR_32 src );

#endif