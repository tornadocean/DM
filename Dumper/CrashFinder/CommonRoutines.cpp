/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "Resource.h"
#include "CommonRoutines.h"
#include "CrashFinder.h"

BOOL InitializeSymbolEngine ( CSymbolEngine & cSymEng )
{
    // Since I am not using the symbol engine with a real process, I
    // will generate a unique value just for this document to use as the
    // hProcess.
    srand ( (unsigned)time ( NULL ) ) ;
    HANDLE hRandHandle = (HANDLE)rand ( ) ;

    BOOL bRet = cSymEng.SymInitialize ( hRandHandle , NULL , FALSE ) ;
    if ( FALSE == bRet )
    {
        ASSERT ( TRUE == bRet ) ;
        TRACE ( "m_cSymEng.SymInitialize FAILED!!!\n" ) ;
        return ( FALSE ) ;
    }

    // Set the symbol engine to load line information.  This must be
    // because the symbol engine does not load source-lines by default.
    // I also turn on the OMAP lookups so that the super-special OMAP
    // moved blocks will get looked at as well.  Trust me, don't ask.
    DWORD dwOpts = SymGetOptions ( ) ;
    SymSetOptions ( dwOpts                   |
                    SYMOPT_LOAD_LINES        |
                    SYMOPT_OMAP_FIND_NEAREST  ) ;

    return ( TRUE ) ;
}

int LookUpAddressInformation ( CSymbolEngine &    cSymEng    ,
                               DWORD              dwAddr     ,
                               CString &          sModule    ,
                               IMAGEHLP_SYMBOL &  stIHSym    ,
                               DWORD &            dwFnDispl  ,
                               IMAGEHLP_LINE &    stIHLine   ,
                               DWORD &            dwSrcDispl  )
{
    BOOL bSymFound  = TRUE ;
    BOOL bLineFound = TRUE ;
    BOOL bModFound  = TRUE ;

    // Try and find the module.  If this cannot be found, then it is no
    // use looking anything else up.
    CImageHlp_Module cMod ;
    bModFound = cSymEng.SymGetModuleInfo ( dwAddr , &cMod ) ;
    if ( FALSE == bModFound )
    {
        bSymFound = FALSE ;
        bLineFound = FALSE ;
    }
    else
    {
        // Pull out the module.
        sModule = cMod.ImageName ;

        // Look up the symbol information.
        BOOL bRet = cSymEng.SymGetSymFromAddr ( dwAddr        ,
                                                &dwFnDispl    ,
                                                &stIHSym       ) ;
        if ( FALSE == bRet )
        {
            bSymFound = FALSE ;
        }

        // Look up the line stuff.
        bRet = cSymEng.SymGetLineFromAddr ( dwAddr        ,
                                            &dwSrcDispl   ,
                                            &stIHLine      ) ;
        if ( FALSE == bRet )
        {
            bLineFound = FALSE ;
        }
    }

    int iRet = 0 ;

    if ( TRUE == bSymFound )
    {
        iRet |= eLUSYM ;
    }
    if ( TRUE == bLineFound )
    {
        iRet |= eLULINE ;
    }
    if ( TRUE == bModFound )
    {
        iRet |= eLUMOD ;
    }

    return ( iRet ) ;
}

BOOL GetAddressSymbolString ( CSymbolEngine & cSymEng       ,
                              DWORD           dwAddr        ,
                              CString &       strOutString  ,
                              LPCTSTR         szSeparators    )
{
    // Since the largest symbol that the MS code generators can
    // handle is 256 that is all that is needed.
    IMAGEHLP_SYMBOL * pstIHSym = (IMAGEHLP_SYMBOL *)
                new BYTE [ sizeof ( IMAGEHLP_SYMBOL ) + 256 ] ;
    memset ( pstIHSym , NULL , sizeof ( IMAGEHLP_SYMBOL ) + 256 ) ;
    pstIHSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL ) + 256 ;
    pstIHSym->MaxNameLength = 256 ;

    // The line information.
    CImageHlp_Line stIHLine ;

    // The displacements.
    DWORD dwFnDispl  ;
    DWORD dwSrcDispl ;

    // The module string.
    CString sModule ;

    // Do the look up.
    int iRes = LookUpAddressInformation ( cSymEng    ,
                                          dwAddr     ,
                                          sModule    ,
                                          *pstIHSym  ,
                                          dwFnDispl  ,
                                          stIHLine   ,
                                          dwSrcDispl  ) ;

    if ( iRes < 0 )
    {
        delete [] pstIHSym ;
        return ( FALSE ) ;
    }

    CCrashFinderApp * pApp = (CCrashFinderApp*)AfxGetApp ( ) ;
    ASSERT ( NULL != pApp ) ;
    if ( NULL == pApp )
    {
        delete [] pstIHSym ;
        return ( FALSE ) ;
    }


    CString strTemp ;

    strOutString = _T ( "" ) ;

    // The address can always be displayed.
    strTemp.Format ( _T ( "%08X" ) , dwAddr ) ;

    strOutString += strTemp ;

    strOutString += szSeparators ;

    if ( eLUMOD == ( eLUMOD & iRes ) )
    {
        // Pull off the path information so the dialog does not
        // have to be a mile wide.
        TCHAR szFile[ MAX_PATH ] ;
        TCHAR szExt[ MAX_PATH ] ;

        _tsplitpath ( (LPCTSTR)sModule ,
                      NULL             ,
                      NULL             ,
                      szFile           ,
                      szExt             ) ;

        strOutString += szFile ;
        strOutString += szExt ;
    }
    else
    {
        strTemp.LoadString ( IDS_MODNOTFOUND ) ;
        strOutString += strTemp ;
    }

    strOutString += szSeparators ;

    if ( eLUSYM == ( eLUSYM & iRes ) )
    {
        strOutString += (LPCTSTR)pstIHSym->Name ;
        if ( ( TRUE == pApp->m_bShowDisplacements ) &&
             ( 0    != dwFnDispl                  )   )
        {
            strTemp.Format ( IDS_DISPLACEMENTFMT , dwFnDispl ) ;
            strOutString += strTemp ;
        }
    }
    else
    {
        strTemp.LoadString ( IDS_FUNCNOTFOUND ) ;
        strOutString += strTemp ;
    }

    strOutString += szSeparators ;

    if ( eLULINE == ( eLULINE & iRes ) )
    {
        strOutString += stIHLine.FileName ;
        if ( eLULINE == ( eLULINE & iRes ) )
        {
            strTemp.Format ( IDS_LINEFMT , stIHLine.LineNumber ) ;
            strOutString += strTemp ;

            if ( ( TRUE == pApp->m_bShowDisplacements ) &&
                 ( 0    != dwSrcDispl                 )   )
            {
                strTemp.Format ( IDS_DISPLACEMENTFMT ,
                                 dwSrcDispl           ) ;
            }
        }
    }

    // I am done so get rid of the memory that I allocated earlier.
    delete [] pstIHSym ;

    return ( TRUE ) ;
}



BOOL CrashFinderTreeDisplay ::
                    InitializeTreeControl ( CTreeCtrl *     pTree   ,
                                            CSymbolEngine * pSymEng  )
{
    // Set the tree and the symbol engine.
    m_pTree = pTree ;
    m_pSymEng = pSymEng ;

    // Create the font to use.
    m_pFont = new CFont ;

    // TODO TODO - Make this a system wide option!
    BOOL bRet =
            m_pFont->CreatePointFont ( 100 , _T ( "Courier New" ) ) ;
    ASSERT ( bRet ) ;
    m_pTree->SetFont ( m_pFont , TRUE ) ;

    // Add the image list to the tree control.
    CImageList * pImageList = new CImageList ( ) ;

    VERIFY ( pImageList->Create ( BITMAP_WIDTH  ,
                                  BITMAP_HEIGHT ,
                                  ILC_MASK      ,
                                  2             ,
                                  4              ) ) ;

    CBitmap cBitmap ;
    int     iRet    ;

    VERIFY ( cBitmap.LoadBitmap ( IDB_BADITEM ) ) ;

    iRet = pImageList->Add ( &cBitmap , (COLORREF)0xFFFFFF ) ;
    ASSERT ( -1 != iRet ) ;

    cBitmap.DeleteObject ( ) ;

    VERIFY ( cBitmap.LoadBitmap ( IDB_GOODITEM ) ) ;

    iRet = pImageList->Add ( &cBitmap , (COLORREF)0xFFFFFF ) ;
    ASSERT ( -1 != iRet ) ;

    cBitmap.DeleteObject ( ) ;

    VERIFY ( cBitmap.LoadBitmap ( IDB_INFOITEM ) ) ;

    iRet = pImageList->Add ( &cBitmap , (COLORREF)0xFFFFFF ) ;
    ASSERT ( -1 != iRet ) ;

    cBitmap.DeleteObject ( ) ;

    m_pTree->SetImageList ( pImageList , TVSIL_NORMAL ) ;

    return ( TRUE ) ;
}

BOOL CrashFinderTreeDisplay ::
            InsertImageStatsIntoTree ( HTREEITEM       hParent  ,
                                       CBinaryImage *  pImage    )
{
    CString strText ;
    HTREEITEM hRet ;

    strText.Format ( IDS_IMAGEBASE , pImage->GetLoadAddress ( ) ) ;
    hRet = m_pTree->InsertItem ( strText    ,
                                 STATE_INFO ,
                                 STATE_INFO ,
                                 hParent     ) ;
    ASSERT ( NULL != hRet ) ;
    if ( NULL != hRet )
    {
        // Convert the time into a readable format and get rid of the
        // annoying '\n'.
        TCHAR szTime [ 30 ] ;
        DWORD64 dwTime = pImage->GetTimeStamp( ) ;
        _tcscpy ( szTime , _tctime ( (time_t*)&dwTime ) ) ;
        szTime[ 24 ] = '\0' ;

        strText.Format ( IDS_TIMESTAMP , szTime ) ;
        hRet = m_pTree->InsertItem ( strText    ,
                                     STATE_INFO ,
                                     STATE_INFO ,
                                     hParent     ) ;
        ASSERT ( NULL != hRet ) ;
        if ( NULL != hRet )
        {
            strText.LoadString ( IDS_SYMBOLTYPE ) ;
            switch ( pImage->GetSymType ( ) )
            {
                case SymNone            :
                    strText += _T ( "SymNone" ) ;
                    break;

                case SymCoff            :
                    strText += _T ( "SymCoff" ) ;
                    break;

                case SymCv              :
                    strText += _T ( "SymCv" ) ;
                    break;

                case SymPdb             :
                    strText += _T ( "SymPdb" ) ;
                    break;

                case SymExport          :
                    strText += _T ( "SymExport" ) ;
                    break;

                case SymDeferred        :
                    strText += _T ( "SymDeferred" ) ;
                    break;

                case SymSym             :
                    strText += _T ( "SymSym" ) ;
                    break;

                case SymDia             :
                    strText += _T ( "SymDia" ) ;
                    break ;

                default :
                    ASSERT ( FALSE ) ;
                    strText += _T ( "**UNKNOWN!!!!!\r\n" );
                    break;
            }
            hRet = m_pTree->InsertItem ( strText    ,
                                         STATE_INFO ,
                                         STATE_INFO ,
                                         hParent     ) ;
            ASSERT ( NULL != hRet ) ;
            if ( NULL != hRet )
            {
                strText.Format ( IDS_IMAGENAME                  ,
                                 pImage->GetSymbolImageName ( )  );
                hRet = m_pTree->InsertItem ( strText    ,
                                             STATE_INFO ,
                                             STATE_INFO ,
                                             hParent     ) ;
                ASSERT ( NULL != hRet ) ;
                if ( NULL != hRet )
                {
                    strText.Format (IDS_LOADEDIMAGE          ,
                                    pImage->GetLoadedSymbolImageName());
                    hRet = m_pTree->InsertItem ( strText    ,
                                                 STATE_INFO ,
                                                 STATE_INFO ,
                                                 hParent     ) ;
                    ASSERT ( NULL != hRet ) ;
                }
            }
        }
    }
    return ( NULL != hRet ) ;
}

BOOL CrashFinderTreeDisplay ::
                    InsertImageInTree ( CBinaryImage *  pImage     ,
                                        int             iState     ,
                                        HTREEITEM       hItemAfter )
{
    // The return value.
    BOOL bRet = FALSE ;

    // Get the application.
    CCrashFinderApp * pApp = (CCrashFinderApp*)AfxGetApp ( ) ;
    ASSERT ( NULL != pApp ) ;
    if ( NULL == pApp )
    {
        return ( FALSE ) ;
    }

    // Put the string into the tree.
    HTREEITEM hParentItem =
                    m_pTree->InsertItem ( pApp->ShowFullPaths ( )
                                            ? pImage->GetFullName ( )
                                            : pImage->GetName ( )    ,
                                          iState                     ,
                                          iState                     ,
                                          TVI_ROOT                   ,
                                          hItemAfter                  );
    ASSERT ( NULL != hParentItem ) ;
    if ( NULL != hParentItem )
    {
        // Put a pointer to the image in the item data. The pointer
        // makes it easy to update the module symbol information
        // whenever the view changes.
        bRet = m_pTree->SetItemData ( hParentItem , (DWORD)pImage ) ;
        ASSERT ( bRet ) ;

        // Only if the item was inserted and was deemed valid can I
        // do the child node stats.
        CString strString ;
        if ( ( TRUE == bRet ) && ( STATE_VALIDATED == iState ) )
        {
            bRet = InsertImageStatsIntoTree ( hParentItem , pImage ) ;
            ASSERT ( TRUE == bRet ) ;
            // Force the item to be selected.
            bRet = m_pTree->SelectItem ( hParentItem ) ;
            ASSERT ( bRet ) ;
        }
        else
        {
            DWORD dwStringID ;
            switch ( pImage->GetBinaryError ( ) )
            {
                case eNotFound  :
                    dwStringID = IDS_ERR_NOTFOUND ;
                    break ;
                case eAddressConflict :
                    dwStringID = IDS_ERR_LOADCONFLICT ;
                    break ;
                case eNoPEImage :
                    dwStringID = IDS_ERR_NOTAPEFILE ;
                    break ;
                case eNoSymbolsAtAll :
                    dwStringID = IDS_ERR_NOSYMBOLS ;
                    break ;
                case eNoPDBSymbols :
                    dwStringID = IDS_ERR_NOPDBSYMBOLS ;
                    break ;
                default :
                    dwStringID = IDS_ERR_UNKNOWNERROR ;
                    break ;

            }
            strString.FormatMessage ( dwStringID , pImage->GetName ( ));
            // Show this message as the item's data.
            m_pTree->InsertItem ( strString   ,
                                  STATE_INFO  ,
                                  STATE_INFO  ,
                                  hParentItem  ) ;
            // Expand the tree to show the error.
            m_pTree->Expand ( hParentItem , TVE_EXPAND ) ;
        }
    }
    else
    {
        // I couldn't add it.
        bRet = FALSE ;
    }
    return ( bRet ) ;
}

BOOL CrashFinderTreeDisplay ::
                         DeleteImageStatsFromTree ( HTREEITEM  hParent )
{
    HTREEITEM hCurrChildItem = m_pTree->GetChildItem ( hParent ) ;
    while ( NULL != hCurrChildItem )
    {
        // Get the next item.
        HTREEITEM hNextChildItem = m_pTree->GetNextItem(hCurrChildItem ,
                                                        TVGN_NEXT     );
        // Delete the current item.
        BOOL bRet = m_pTree->DeleteItem ( hCurrChildItem ) ;
        ASSERT ( TRUE == bRet ) ;
        hCurrChildItem = hNextChildItem ;
    }
    return ( TRUE ) ;
}

BOOL CrashFinderTreeDisplay ::
                UpdateImageStats ( HTREEITEM       hParent          ,
                                   CBinaryImage *  pImage           ,
                                   BOOL            bPrevWasGoodLoad  )
{
    BOOL bRet = FALSE ;

    // Check that the item has properly loaded previously.
    if ( TRUE == bPrevWasGoodLoad )
    {
        // The image had a good load previously so all I need to do is
        // update the load address, which is the first child.
        HTREEITEM hChild = m_pTree->GetChildItem ( hParent ) ;
        ASSERT ( NULL != hChild ) ;
        if ( NULL != hChild )
        {
            CString strText ;

            strText.Format ( IDS_IMAGEBASE              ,
                             pImage->GetLoadAddress ( )  ) ;

            bRet = m_pTree->SetItemText ( hChild , strText ) ;
        }
    }
    else
    {
        // This is a node that had only the warning message as a child
        // node.

        // Remove all child nodes.
        DeleteImageStatsFromTree ( hParent ) ;

        // Get all the module information.
        // The module information.
        CImageHlp_Module cModInfo ;

        CString strString ;

        // Look up this module's information.
        // Try and get the symbol information for this module.
        bRet = m_pSymEng->SymGetModuleInfo(pImage->GetLoadAddress(),
                                           &cModInfo              );

        ASSERT ( FALSE != bRet ) ;
        if ( TRUE == bRet )
        {
            bRet = InsertImageStatsIntoTree ( hParent , pImage) ;
            ASSERT ( TRUE == bRet ) ;
        }
        else
        {
            strString.FormatMessage ( IDS_SYMMODULENOTLOADED ,
                                      pImage->GetName ( )     );
            // Show this message as the item's data.
            m_pTree->InsertItem ( strString   ,
                                  STATE_INFO  ,
                                  STATE_INFO  ,
                                  hParent      ) ;
        }
    }
    return ( bRet ) ;
}