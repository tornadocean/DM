/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#ifndef _ABOUT_H
#define _ABOUT_H

#include "resource.h"
#include "BigIcon.h"
#include "StatLink.h"

class CColorStaticText : public CStatic
{
public  :
    DECLARE_DYNAMIC ( CColorStaticText )
    CColorStaticText ( )
    {
        m_color = RGB ( 255 , 0 , 0 ) ;
    }
    ~CColorStaticText ( )
    {
    }

    // The color for the text.
    COLORREF m_color;

protected:

    // message handlers
    DECLARE_MESSAGE_MAP()
#if _MSC_VER <= 1310
    afx_msg UINT OnNcHitTest(CPoint point);
#else
    afx_msg LRESULT OnNcHitTest(CPoint point);
#endif
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


class CAboutDlg : public CDialog
{
public      :

    CAboutDlg ( void ) ;

    virtual ~CAboutDlg ( void ) ;

    enum { IDD = IDD_ABOUTBOX } ;

protected   :
    virtual BOOL OnInitDialog ( void ) ;
    virtual void DoDataExchange ( CDataExchange * pDX ) ;

    // The big icon control.
    CBigIcon m_cBigIcon ;
    // The font for the big text.
    CFont * m_pcFont    ;
    // The hyper link.
    CStaticLink m_cWebLink ;
    // The red font stuff
    CColorStaticText m_cRedText ;


    DECLARE_MESSAGE_MAP ( )
} ;

#endif  // _ABOUT_H


