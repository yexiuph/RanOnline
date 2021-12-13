#pragma once

namespace REPORTFILES
{
	void MakeFileList(
		const char *szPath );

	void PathRecurse(
		const std::string strRootPath,
		const std::string strCurPath );
};
