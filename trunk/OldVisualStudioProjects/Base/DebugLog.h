#pragma once

#include "Common.h"
#include <string>
#include <streambuf>
#include <ostream>
#include <windows.h>

namespace Base 
{
	// This actually writes out the text 
	class DebugBuf : public std::streambuf
	{
	public:
		
		DebugBuf() 
		{
			setp(buffer, buffer+BUFFER_LENGTH-1);
		}
		int sync ()
		{ 
			std::streamsize n = static_cast<std::streamsize>(pptr () - pbase ());
			if ( n > 0 )
			{
				char tmp[BUFFER_LENGTH];
				for (int i = 0; i < n; ++i)
					tmp[i] = pbase()[i];
				tmp[n] = 0;
				OutputDebugString( tmp ) ;
//				return n;
				pbump (-n);  // Reset pptr().
				return 0;

			}
			
			return EOF;
		}
		int overflow (int ch)
		{ 
			std::streamsize n = static_cast<std::streamsize>(pptr () - pbase ());
			if (n && !sync())
				return 0;
			if (ch != EOF)
			{
				char cbuf[2];
				cbuf[0] = ch;
				cbuf[1] = 0;
				OutputDebugString( cbuf );
				return 0;
			}
			return EOF;
		}

	private:
		enum { BUFFER_LENGTH = 128 };
		char buffer[BUFFER_LENGTH];
	};

	// the actual stream objects
	class DebugLog : public std::ostream
	{
	public:
		DebugLog()  : std::ostream(&buffer) {}
		~DebugLog() {}
	private:
		DebugBuf buffer;
	};

	// 
	extern DebugLog debugLog;

}
