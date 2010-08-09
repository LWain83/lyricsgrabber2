#pragma once

// Change these values to use different versions
#define WINVER		0x0501
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

#include "../../ATLHelpers/ATLHelpers.h"

#include <atlframe.h>
#include <atldlgs.h>
#include <atlctrlx.h>
#include <atlddx.h>
#include <atlmisc.h>

#include "../../SDK/foobar2000.h"
#include "../../lyrics_grabber_sdk/lyrics_grabber_sdk.h"

const int infinite = ~0;