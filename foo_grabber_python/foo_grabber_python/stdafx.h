// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#ifdef _WTL_USE_CSTRING
#undef _WTL_USE_CSTRING
#endif

#ifndef _WTL_NO_CSTRING
#define _WTL_NO_CSTRING
#endif

#define _ATL_NO_COM

#include <atlbase.h>
#include <atlapp.h>

#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atlcrack.h>

#include <string>
#include <boost/python.hpp>


#include "../../SDK/foobar2000.h"
#include "../../lyrics_grabber_sdk/lyrics_grabber_sdk.h"

const int infinite = ~0;