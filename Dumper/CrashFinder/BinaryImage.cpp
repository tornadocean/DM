/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.

With excellent updates from Scott Bloom, Ching Ming Kwok,
Jeff Shanholtz, and Pablo Presedo!
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "BinaryImage.h"
#include "BinaryImageOptionsDlg.h"

#define MakePtr( cast , ptr , AddValue ) \
                                 (cast)( (DWORD)(ptr)+(DWORD)(AddValue))


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__ ;
#endif

// MFC stuff.
IMPLEMENT_SERIAL ( CBinaryImage , CObject , 1 )

CBinaryImage :: CBinaryImage ( void )
              : CObject ( )     ,
                m_szFullName ( )
{
    ASSERT ( this ) ;
    m_dwLoadAddress     = 0     ;
    m_dwCharacteristics = 0     ;
    m_bIsValid          = FALSE ;
    m_dwExtraData       = 0     ;
    m_eFailCode         = eNoFailure ;

    m_dwTimeStamp       = 0 ;
    m_eSymType          = SymNone ;
    m_szImageName[ 0 ]  = _T ( '\0' ) ;
    m_szLoadedImageName[ 0 ] = _T ( '\0' ) ;
}

void CBinaryImage :: Serialize ( CArchive & ar )
{
    ASSERT ( this ) ;
    if( ar.IsStoring( ) )
    {
        ar << m_szFullName    ;
    }
    else
    {
        ar >> m_szFullName    ;
    }
}

////////////////////////////////////////////////////////////////////////
//                Public Construction and Desctruction
////////////////////////////////////////////////////////////////////////
CBinaryImage :: CBinaryImage ( LPCTSTR szFileName )
              : CObject ( )     ,
                m_szFullName ( )
{
    ASSERT ( this ) ;
    m_dwLoadAddress = 0 ;
    m_szFullName = szFileName ;
    m_bIsValid = FALSE ;
    m_dwCharacteristics = 0 ;
    m_dwExtraData = 0 ;
    m_eFailCode = eNoFailure ;

    m_dwTimeStamp       = 0 ;
    m_eSymType          = SymNone ;
    m_szImageName[ 0 ]  = _T ( '\0' ) ;
    m_szLoadedImageName[ 0 ] = _T ( '\0' ) ;

    // Validate the image without showing errors.
    IsValidImage ( FALSE ) ;
}

CBinaryImage :: CBinaryImage ( const CBinaryImage & cInit )
{
    ASSERT ( this ) ;
    m_szFullName        = cInit.m_szFullName        ;
    m_dwLoadAddress     = cInit.m_dwLoadAddress     ;
    m_dwCharacteristics = cInit.m_dwCharacteristics ;
    m_bIsValid          = cInit.m_bIsValid          ;
    m_dwExtraData       = cInit.m_dwExtraData       ;
    m_eFailCode         = cInit.m_eFailCode         ;

    m_dwTimeStamp       = cInit.m_dwTimeStamp ;
    m_eSymType          = cInit.m_eSymType ;
    _tcscpy ( m_szImageName , cInit.m_szImageName ) ;
    _tcscpy ( m_szLoadedImageName , cInit.m_szLoadedImageName ) ;
}

CBinaryImage :: ~CBinaryImage ( void )
{
    ASSERT ( this ) ;
}

////////////////////////////////////////////////////////////////////////
//                     Public Validation Methods
////////////////////////////////////////////////////////////////////////
// Returns TRUE if the image passed in the constructor is a valid
// binary image.  Returns FALSE otherwise.
BOOL CBinaryImage :: IsValidImage ( BOOL bShowErrors )
{
    ASSERT ( this ) ;
    // If the m_bIsValid value was already true, then the image is OK.
    if ( TRUE == m_bIsValid )
    {
        return ( m_bIsValid ) ;
    }

    // The error message holder.
    CString sMsg ;

    // First check if the file actually exists.
    CFileFind cFFile ;

    BOOL bRet = cFFile.FindFile ( m_szFullName ) ;
    if ( FALSE == bRet )
    {
        if ( TRUE == bShowErrors )
        {
            sMsg.FormatMessage ( IDS_FILENOTFOUND , m_szFullName ) ;
            AfxMessageBox ( sMsg ) ;
        }
        m_bIsValid = FALSE ;
        m_eFailCode = eNotFound ;
        return ( FALSE ) ;
    }

    // The file exists, now see if it is a real PE file.
    LOADED_IMAGE stLI ;
    if ( ( FALSE == MapAndLoad ( (LPTSTR)(LPCTSTR)m_szFullName ,
                                  NULL                         ,
                                  &stLI                        ,
                                  TRUE                         ,
                                  TRUE                          ) ) ||
         ( IMAGE_NT_SIGNATURE != stLI.FileHeader->Signature       )   )
    {
        if ( TRUE == bShowErrors )
        {
            sMsg.FormatMessage ( IDS_NOTPEIMAGE , m_szFullName ) ;
            AfxMessageBox ( sMsg ) ;
        }
        m_bIsValid = FALSE ;
        m_eFailCode = eNoPEImage ;
        return ( FALSE ) ;
    }

    // The load address is ALWAYS based off the current image.
    m_dwLoadAddress = stLI.FileHeader->OptionalHeader.ImageBase ;

    // Save off the characteristics.
    m_dwCharacteristics = stLI.Characteristics ;

    // Unload the image.
    bRet = UnMapAndLoad ( &stLI ) ;
    ASSERT ( TRUE == bRet ) ;

    m_bIsValid = ( bRet == TRUE ) ;
    return ( m_bIsValid ) ;
}

////////////////////////////////////////////////////////////////////////
//                 Public Property Retrieval Methods
////////////////////////////////////////////////////////////////////////
// Returns the load address for this image.
DWORD CBinaryImage :: GetLoadAddress ( void ) const
{
    ASSERT ( this ) ;
    return ( m_dwLoadAddress ) ;
}

// Returns the full name for this image.
const CString CBinaryImage :: GetFullName ( void ) const
{
    ASSERT ( this ) ;
    return ( m_szFullName ) ;
}

// Returns just the name of the image.
const CString CBinaryImage :: GetName ( void ) const
{
    ASSERT ( this ) ;

    TCHAR szFName [ MAX_PATH ] ;
    TCHAR szExt [ MAX_PATH ] ;

     _tsplitpath ( m_szFullName , NULL , NULL , szFName , szExt ) ;

     CString szTemp = szFName ;

     szTemp += szExt ;

     return ( szTemp ) ;
}

DWORD CBinaryImage :: GetCharacteristics ( void ) const
{
    ASSERT ( this ) ;
    return ( m_dwCharacteristics ) ;
}

// Returns a string that contains any additional information that
// the class would like to return about this image.
const CString CBinaryImage :: GetAdditionalInfo ( void ) const
{
    ASSERT ( this ) ;
    CString szTemp ;
    return ( szTemp ) ;
}

// Gets the extra data for this class.
DWORD CBinaryImage :: GetExtraData ( void ) const
{
    ASSERT ( this ) ;
    return ( m_dwExtraData ) ;
}

////////////////////////////////////////////////////////////////////////
//                  Public Property Setting Methods
////////////////////////////////////////////////////////////////////////
BOOL CBinaryImage :: SetProperties ( void )
{
    ASSERT ( this ) ;
    CBinaryImageOptionsDlg cDlg ;
    cDlg.m_dwAddr = m_dwLoadAddress ;
    cDlg.m_strImageName = GetName ( ) ;
    if ( IDOK == cDlg.DoModal ( ) )
    {
        if ( cDlg.m_dwAddr != m_dwLoadAddress )
        {
            m_dwLoadAddress = cDlg.m_dwAddr ;
            return ( TRUE ) ;
        }
    }
    return ( FALSE ) ;
}

void CBinaryImage :: SetLoadAddress ( DWORD dwAddr )
{
    ASSERT ( this ) ;
    m_dwLoadAddress = dwAddr ;
}

void CBinaryImage :: SetExtraData ( DWORD dwData )
{
    ASSERT ( this ) ;
    m_dwExtraData = dwData ;
}


////////////////////////////////////////////////////////////////////////
//                          Public Operators
////////////////////////////////////////////////////////////////////////
CBinaryImage & CBinaryImage:: operator = ( const CBinaryImage & rhs )
{
    ASSERT ( this ) ;
    if ( this != &rhs )
    {
        m_szFullName        = rhs.m_szFullName        ;
        m_dwLoadAddress     = rhs.m_dwLoadAddress     ;
        m_dwCharacteristics = rhs.m_dwCharacteristics ;
        m_bIsValid          = rhs.m_bIsValid          ;
        m_dwExtraData       = rhs.m_dwExtraData       ;
        m_eFailCode         = rhs.m_eFailCode         ;
        m_dwTimeStamp       = rhs.m_dwTimeStamp ;
        m_eSymType          = rhs.m_eSymType ;
        _tcscpy ( m_szImageName , rhs.m_szImageName ) ;
        _tcscpy ( m_szLoadedImageName , rhs.m_szLoadedImageName ) ;
    }
    return ( *this ) ;
}


BOOL CBinaryImage :: GetModules ( LPCTSTR          pModuleName ,
                                  CMapStringToPtr& ModuleList   )
{
    HINSTANCE hModule = NULL ;
    CString strName ( pModuleName ) ;
    strName.MakeUpper ( ) ;
    ModuleList[strName] = NULL ;

    hModule = LoadLibraryEx ( pModuleName ,
                              NULL        ,
                              DONT_RESOLVE_DLL_REFERENCES ) ;
    if ( NULL == hModule )
    {
        m_eFailCode = eNotFound ;
        return ( FALSE ) ;
    }

    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule ;

    // Is this the MZ header?
    if ( ( TRUE == IsBadReadPtr ( pDOSHeader                   ,
                                  sizeof ( IMAGE_DOS_HEADER ) ) ) ||
        ( IMAGE_DOS_SIGNATURE != pDOSHeader->e_magic            )   )
    {
        m_eFailCode = eNoPEImage ;
        FreeLibrary ( hModule ) ;
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Get the PE header.
    PIMAGE_NT_HEADERS pNTHeader = MakePtr ( PIMAGE_NT_HEADERS    ,
                                            pDOSHeader           ,
                                            pDOSHeader->e_lfanew  ) ;

    // Is this a real PE image?
    if ( ( TRUE == IsBadReadPtr ( pNTHeader                    ,
                                  sizeof ( IMAGE_NT_HEADERS ) ) ) ||
         ( IMAGE_NT_SIGNATURE != pNTHeader->Signature           )   )
    {
        m_eFailCode = eNoPEImage ;
        FreeLibrary ( hModule ) ;
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Get the pointer to the imports section.
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc =
                            MakePtr ( PIMAGE_IMPORT_DESCRIPTOR ,
                                      pDOSHeader               ,
                        pNTHeader->OptionalHeader.
                            DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].
                                                      VirtualAddress ) ;
    // Does it have an imports section?
    if ( 0 == pImportDesc )
    {
        m_eFailCode = eNoPEImage ;
        FreeLibrary ( hModule ) ;
        return ( FALSE ) ;
    }
    // Loop through the import module descriptors looking for the
    // module whose name matches szImportMod.
    LPVOID pPtr;
    while ( NULL != pImportDesc->Name )
    {
        CString szModName = MakePtr ( PSTR              ,
                                      pDOSHeader        ,
                                      pImportDesc->Name  ) ;
        if ( TRUE == SearchForModule ( szModName ) )
        {
            szModName.MakeUpper ( ) ;
            if ( FALSE == ModuleList.Lookup ( szModName , pPtr ) )
            {
                ModuleList[szModName] = NULL ;
                if ( FALSE == GetModules ( szModName , ModuleList) )
                {
                    m_eFailCode = eNoPEImage ;
                    FreeLibrary ( hModule ) ;
                    return ( FALSE ) ;
                }
            }
        }

        // Look at the next one.
        pImportDesc++;
    }

    FreeLibrary ( hModule ) ;
    return ( TRUE ) ;
}

BOOL CBinaryImage :: SearchForModule ( CString& szModName )
{
    CString strName ;
    CString szExt ;

    int i = szModName.Find ( _T ( '.' ) ) ;

    if ( -1 != i )
    {
        strName = szModName.Left ( i ) ;
        szExt = szModName.Mid ( i ) ;
    }
    else
    {
        strName = szModName ;
        szExt = _T ( '.' ) ;
    }
    TCHAR szBuff [ MAX_PATH + 1 ];
    LPTSTR pFilePart ;
    DWORD len = SearchPath ( NULL       ,
                             strName    ,
                             szExt      ,
                             MAX_PATH   ,
                             szBuff     ,
                             &pFilePart  ) ;
    if ( ( len > 0 ) && ( len < MAX_PATH ) )
    {
        szModName = szBuff ;
        return ( TRUE ) ;
    }
    else
    {
        szModName = strName + szExt ;
        return ( FALSE ) ;
    }
}

void CBinaryImage :: SetBinaryError ( BinaryFailureEnum eCode )
{
    m_eFailCode = eCode ;
}

BinaryFailureEnum CBinaryImage :: GetBinaryError ( void )
{
    return ( m_eFailCode ) ;
}

DWORD CBinaryImage :: GetTimeStamp ( void )
{
    return ( m_dwTimeStamp ) ;
}

SYM_TYPE CBinaryImage :: GetSymType ( void )
{
    return ( m_eSymType ) ;
}

LPCTSTR CBinaryImage :: GetSymbolImageName ( void )
{
    return ( m_szImageName ) ;
}

LPCTSTR CBinaryImage :: GetLoadedSymbolImageName ( void )
{
    return ( m_szLoadedImageName ) ;
}

void CBinaryImage :: SetSymbolInformation ( CImageHlp_Module & cModInfo)
{
    m_dwTimeStamp = cModInfo.TimeDateStamp ;
    m_eSymType    = cModInfo.SymType ;
    _tcscpy ( m_szImageName , cModInfo.ImageName ) ;
    _tcscpy ( m_szLoadedImageName , cModInfo.LoadedImageName ) ;
}
