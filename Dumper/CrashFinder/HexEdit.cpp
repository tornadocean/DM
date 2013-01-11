/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#include "StdAfx.h"
#include "HexEdit.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__ ;
#endif

BEGIN_MESSAGE_MAP ( CHexEdit , CEdit )
    ON_WM_CHAR ( )
END_MESSAGE_MAP ( )

CHexEdit :: CHexEdit ( )
          : CEdit ( )
{
    ASSERT ( this ) ;
}

void CHexEdit :: OnChar ( UINT nChar , UINT nRepCnt , UINT nFlags )
{
    ASSERT ( this ) ;

    BOOL bAllowed = FALSE ;

    if ( nChar < 0x20 )
    {
        bAllowed = TRUE ;
    }
    else if ( ( (TCHAR)nChar >= _T ( '0' ) ) &&
              ( (TCHAR)nChar <= _T ( '9' ) )    )
    {
        bAllowed = TRUE ;
    }
    else if ( ( (TCHAR)nChar >= _T ( 'A' ) ) &&
              ( (TCHAR)nChar <= _T ( 'F' ) )    )
    {
        bAllowed = TRUE ;
    }
    else if ( ( (TCHAR)nChar >= _T ( 'a' ) ) &&
              ( (TCHAR)nChar <= _T ( 'f' ) )    )
    {
        bAllowed = TRUE ;
    }
    else if ( ( (TCHAR)nChar == _T ( ' ' ) ) ||
              ( (TCHAR)nChar == _T ( ';' ) ) ||
              ( (TCHAR)nChar == _T ( ',' ) )    )
    {
        bAllowed = TRUE ;
    }

    if ( TRUE == bAllowed )
    {
        CEdit::OnChar ( nChar , nRepCnt , nFlags ) ;
    }
    else
    {
        MessageBeep ( MB_ICONEXCLAMATION ) ;
    }
}

const LPCTSTR k_DELIMS = _T ( " ,;" ) ;

void CHexEdit :: GetNumbers ( CDWordArray & cArray )
{
    // Get the array into shape.
    cArray.RemoveAll ( ) ;

    CString strRaw ;
    GetWindowText ( strRaw ) ;

    TCHAR * szBuff = (TCHAR*)new TCHAR[ strRaw.GetLength ( ) +
                                        1 * sizeof ( TCHAR )   ] ;

    _tcscpy ( szBuff , (LPCTSTR)strRaw ) ;

    // Use good ol' strtok to do the deed.
    TCHAR * pCurr = _tcstok ( szBuff , k_DELIMS ) ;
    while ( NULL != pCurr )
    {

        // Try and convert it to an value.  I'm taking the easy way out.
        DWORD dwAddr ;
        int iRet = _stscanf ( pCurr       ,
                              _T ( "%x" ) ,
                              &dwAddr       ) ;

        if ( 1 != iRet )
        {
            CString sMsg ;
            sMsg.FormatMessage ( IDS_LOADADDRBADCONVERSION ,
                                 pCurr                      ) ;
            AfxMessageBox ( sMsg ) ;
        }
        else
        {
            cArray.Add (  dwAddr ) ;
        }

        pCurr = _tcstok ( NULL , k_DELIMS ) ;
    }
    delete [] szBuff ;
}

