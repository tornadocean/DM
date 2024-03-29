/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#if !defined ( _STDAFX_H )
#define _STDAFX_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define _CRT_SECURE_NO_DEPRECATE 

#define VC_EXTRALEAN

#define WINVER 0x0500

#include <afxwin.h>
#include <afxext.h>
#include <AFXCVIEW.H>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif


#include <imagehlp.h>
#include <time.h>

//{{AFX_INSERT_LOCATION}}

#endif // !_STDAFX_H
