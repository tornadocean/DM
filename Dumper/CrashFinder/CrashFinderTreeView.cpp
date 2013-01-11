/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "CrashFinder.h"

#include "CrashFinderDoc.h"
#include "CrashFinderTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__ ;
#endif

IMPLEMENT_DYNCREATE ( CCrashFinderTreeView , CTreeView )

//lint -e648
BEGIN_MESSAGE_MAP ( CCrashFinderTreeView , CTreeView )
//{{AFX_MSG_MAP(CCrashFinderTreeView)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ( )
//lint +e648

CCrashFinderTreeView :: CCrashFinderTreeView ( )
                      : CTreeView ( )
{
    m_dwDefaultStyle |= TVS_HASLINES |
                        TVS_LINESATROOT |
                        TVS_HASBUTTONS ;
    ASSERT ( this ) ;
}

CCrashFinderTreeView :: ~CCrashFinderTreeView ( )
{
    ASSERT ( this ) ;
}

#ifdef _DEBUG
void CCrashFinderTreeView :: AssertValid ( ) const
{
    ASSERT ( this ) ;
    CView::AssertValid ( ) ;
}

void CCrashFinderTreeView :: Dump ( CDumpContext & dc ) const
{
    ASSERT ( this ) ;
    CView::Dump ( dc ) ;
}

CCrashFinderDoc * CCrashFinderTreeView :: GetDocument ( )
{
    ASSERT ( m_pDocument->IsKindOf ( RUNTIME_CLASS( CCrashFinderDoc )));
    return ( (CCrashFinderDoc *)m_pDocument ) ;
}
#endif //_DEBUG
