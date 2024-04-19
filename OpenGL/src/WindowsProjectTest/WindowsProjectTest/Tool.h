#include <Windows.h>
#include <string>
#include <cstdio>
#include <cstdarg>

namespace GT {
	class UTool {
	public:

	public:
		static void DebugPrint(const wchar_t* format, ...)
		{
			wchar_t buffer[256];

			va_list args;
			va_start(args, format);
			vswprintf(buffer, sizeof(buffer), format, args);
			va_end(args);

			OutputDebugString(buffer);
		}
	};
}

