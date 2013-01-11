/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/

#if !defined ( _MAINFRM_H )
#define _MAINFRM_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC ( CMainFrame )

public      :

    CMainFrame ( ) ;

public      :

    virtual BOOL PreCreateWindow ( CREATESTRUCT & cs ) ;

public      :
    virtual ~CMainFrame ( ) ;
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCrashFinderMainFrame)
public      :

protected   :
    virtual int OnCreate ( LPCREATESTRUCT lpCreateStruct ) ;
    //}}AFX_VIRTUAL

#ifdef _DEBUG
    virtual void AssertValid ( ) const ;
    virtual void Dump ( CDumpContext & dc ) const ;
#endif

protected   :

    CStatusBar  m_wndStatusBar ;
    CToolBar    m_wndToolBar ;

protected   :
    //{{AFX_MSG(CMainFrame)
    afx_msg void OnClose( ) ;
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP ( )

} ;

#endif // _MAINFRM_H


