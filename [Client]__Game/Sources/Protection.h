#pragma once

#define PROTECT CProtection::GetInstance()

class CProtection
{
private:
	CProtection(void);
	CProtection(const CProtection&);
	CProtection& operator=(const CProtection&);
	~CProtection();


	typedef struct tagDebugReg7
	{
		unsigned L0 :1; // 
		unsigned G0 :1; // 
		unsigned L1 :1; // 
		unsigned G1 :1; // 
		unsigned L2 :1; // 
		unsigned G2 :1; // 
		unsigned L3 :1; // 
		unsigned G3 :1; // 
		unsigned GL :1; // 
		unsigned GE :1; // 
		unsigned undefined1 :3; // 001 
		unsigned GD :1; // 
		unsigned undefined2 :2; // 00 
		unsigned RW0 :2;
		unsigned LEN0 :2;
		unsigned RW1 :2;
		unsigned LEN1 :2;
		unsigned RW2 :2;
		unsigned LEN2 :2;
		unsigned RW3 :2;
		unsigned LEN3 :2;
	} DebugReg7;

public:

	HKEY hKey; 
	DWORD dwData;
	LONG lRet;
	LPCSTR lpValueName;

	CONTEXT CONTEXTDebug;
	DebugReg7 *DR_7;

	static CProtection& GetInstance();
	void Init();


	void RegistryRecord();//������Ʈ�����
	void RegistryDelete();//������Ʈ����ϻ���

	BOOL Pesudo_IsDebuggerPresent();//����� ������ ����

	void WindowsSyStemCompulsoryDown();//������ �ý��� ���� ����
};
