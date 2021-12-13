#include "pch.h"

#include "./DxEffectShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////	Note : 정적 변수 생성.
////
//DWORD	DxEffectShadow::m_dwDiffuseShadow;
//
///*
////	Note : 
////
//char DxEffectShadow::m_strEffShadow[] =
//{
//	"vs.1.0	;\n" //
//
//	"m4x4 oPos, v0, c22		\n"
//
//	"mov oD0, v5	\n"
//
//	"mov oT0, v7	\n"
//};
//*/
//
////	Note : DxEffectShadow
////
//DWORD	DxEffectShadow::m_dwShaderDiffuseVS[] = {
//	0xfffe0100, 0x0004fffe, 0x454c4946, 0x6e696172,
//	0x76766e2e, 0x006c2d00, 0x0002fffe, 0x454e494c,
//	0x00000001, 0x0002fffe, 0x454e494c, 0x00000002,
//	0x0002fffe, 0x454e494c, 0x00000002, 0x00000014,
//	0xc00f0000, 0x90e40000, 0xa0e40016, 0x0002fffe,
//	0x454e494c, 0x00000003, 0x0002fffe, 0x454e494c,
//	0x00000003, 0x00000001, 0xd00f0000, 0x90e40005,
//	0x0002fffe, 0x454e494c, 0x00000004, 0x0002fffe,
//	0x454e494c, 0x00000004, 0x00000001, 0xe00f0000,
//	0x90e40007, 0x0000ffff
//};