#pragma once
#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include <string>

class SystemTraits
{
public:
	static std::string GetExePath()
	{
#if (defined(_WIN32) || defined(_WIN64))
		char	exefile[MAX_PATH] = { 0 };
		DWORD	length = GetModuleFileNameA(NULL, exefile, MAX_PATH-1);
		if (0 == length)
		{
			return std::string();
		}
#else
		char	exefile[PATH_MAX] = { 0 };
		ssize_t length = readlink("/proc/self/exe", exefile, PATH_MAX-1);
		if (length < 0)
		{
			return std::string();
		}
#endif
		exefile[length] = '\0';

		return exefile;
	}

	static std::string GetExeDir()
	{
		std::string path = GetExePath();
#if (defined(_WIN32) || defined(_WIN64))
		std::string::size_type pos = path.rfind('\\');
#else
		std::string::size_type pos = path.rfind('/');
#endif
		if (std::string::npos == pos)
		{
			return std::string();
		}
		path.resize(pos);
		return path;
	}

	static std::string GetExeName()
	{
		std::string path = GetExePath();
#if (defined(_WIN32) || defined(_WIN64))
		std::string::size_type pos = path.rfind('\\');
#else
		std::string::size_type pos = path.rfind('/');
#endif
		if (std::string::npos == pos
			|| pos + 1 >= path.length())
		{
			return std::string();
		}

		return path.substr(pos + 1);
	}

};

