#ifndef S_USER_CHECKER_H_
#define S_USER_CHECKER_H_

#include <afxcoll.h> // MFC CMapStringToPtr head file

namespace SERVER_UTIL
{
	class CUserChecker
	{
	private:
		CUserChecker(void);
		~CUserChecker(void);

		static CUserChecker* SelfInstance;

	public:
		static CUserChecker* GetInstance();
		static void	ReleaseInstance();

	public:
		bool IsAvailable(CString strUserID);
		void RemoveUser(CString strUserID);

	protected:
		void DeleteAll();
		void Insert(CString strUserID);
		bool Find(CString strUserID);		
		CMapStringToPtr m_UserMap;
		CRITICAL_SECTION m_CriticalSection; // criticalsection object

	};
}

#endif // S_USER_CHECKER_H_