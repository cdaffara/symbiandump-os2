/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This module contains implementation of 
* CSTIFTestFrameworkSettings class member functions.
*
*/

// INCLUDE FILES
#include "STIFTestFrameworkSettings.h"
#include "TestReport.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: CSTIFTestFrameworkSettings

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFTestFrameworkSettings::CSTIFTestFrameworkSettings()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFTestFrameworkSettings::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CSTIFTestFrameworkSettings*: pointer to 
                                                CSTIFTestFrameworkSettings
                                                object

    Errors/Exceptions: None 

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CSTIFTestFrameworkSettings* CSTIFTestFrameworkSettings::NewL()
    {
    CSTIFTestFrameworkSettings* self = 
                        new ( ELeave ) CSTIFTestFrameworkSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: ~CSTIFTestFrameworkSettings

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CSTIFTestFrameworkSettings::~CSTIFTestFrameworkSettings()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetReportModeL

    Description: Get report mode settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                TUint& aSetting: inout: Parsed setting.

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetReportModeL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        TUint& aSetting )
    {
    TPtrC string;

    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( aTag, ENoTag ) );
    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        ret = item->GetString( _L( "" ), string );
        while( ret == KErrNone )
            {
            HBufC* stringHbuf = string.AllocL();
            TPtr modifiableString = stringHbuf->Des();
            modifiableString.UpperCase();

            if( modifiableString == _L( "EMPTY" ) )
                {
                // No test report
                aSetting = CTestReport::ETestReportBlank;
                }
            else if( modifiableString == _L( "SUMMARY" ) )
                {
                // Increment aSetting
                aSetting += CTestReport::ETestReportSummary;
                }
            else if( modifiableString == _L( "ENVIRONMENT" ) )
                {
                // Increment aSetting
                aSetting += CTestReport::ETestReportEnvironment;
                }
            else if( modifiableString == _L( "TESTCASES" ) )
                {
                // Increment aSetting
                aSetting += CTestReport::ETestReportCases;
                }
            else if( modifiableString == _L( "FULLREPORT" ) )
                {
                // Set aSettings to max value and break
                aSetting = CTestReport::ETestReportFull;
                delete stringHbuf;
                break;
                }
            else if( modifiableString == _L( "#" ) )
                {
                delete stringHbuf;
                break;
                }
            else
                {
                delete stringHbuf;
                delete item;
                return KErrArgument;
                }
            delete stringHbuf;
            ret = item->GetNextString( string );
            }
        }

    delete item;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetFileSetting

    Description: Get file settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                TPtrC& aFilePath: inout: Test file setting.

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetFileSetting( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        TPtrC& aSetting )
    {
    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( 
                                            aTag,
                                            ENoTag ) );
    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        ret = item->GetString( _L( "" ), aSetting );
        }

    delete item;

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetFormatL

    Description: Get Test report format settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                CStifLogger::TLoggerType& aLoggerType: inout: Test report type
                setting.

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetFormatL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag, 
                                        CStifLogger::TLoggerType& aLoggerType )
    {
    TBool xml;
    return GetFormatL(aSectionParser, aTag, aLoggerType, xml);
    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetFormatL

    Description: Get Test report format settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                CStifLogger::TLoggerType& aLoggerType: inout: Test report type setting.
                TBool& aXML: inout: true if xml format is set

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetFormatL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag, 
                                        CStifLogger::TLoggerType& aLoggerType,
                                        TBool& aXML )
    {
    aXML = EFalse;
    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( aTag, ENoTag ) );

    TPtrC string;

    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        ret = item->GetString( _L( "" ), string );
        if ( ret != KErrNone )
            {
            delete item;
            return ret;
            }
        }

    HBufC* stringHbuf = string.AllocL();
    TPtr modifiableString = stringHbuf->Des();
    modifiableString.UpperCase();

    if( modifiableString == _L( "TXT" ) )
        {
        aLoggerType = CStifLogger::ETxt;
        }
    else if( modifiableString == _L( "HTML" ) )
        {
        aLoggerType = CStifLogger::EHtml;
        }
    else if( modifiableString == _L( "DATA" ) )
        {
        aLoggerType = CStifLogger::EData;
        }
    else if( modifiableString == _L( "XML" ) )
        {
        aLoggerType = CStifLogger::ETxt;
        aXML = ETrue;
        }
    else
        {
        delete stringHbuf;
        delete item;
        return KErrArgument;
        }

    delete stringHbuf;
    delete item;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetOutputL

    Description: Get Test report output settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                CStifLogger::TOutput& aOutput: inout: Test report output setting.

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetOutputL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        CStifLogger::TOutput& aOutput )
    {
    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( aTag, ENoTag ) );

    TPtrC string;

    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        ret = item->GetString( _L( "" ), string );
        if ( ret != KErrNone )
            {
            delete item;
            return ret;
            }
        }

    HBufC* stringHbuf = string.AllocL();
    TPtr modifiableString = stringHbuf->Des();
    modifiableString.UpperCase();

    if( modifiableString == _L( "FILE" ) )
        {
        aOutput = CStifLogger::EFile;
        }
    else if( modifiableString == _L( "RDEBUG" ) )
        {
        aOutput = CStifLogger::ERDebug;
        }
    else
        {
        delete stringHbuf;
        delete item;
        return KErrArgument;
        }

    delete stringHbuf;
    delete item;

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetBooleanSettingsL

    Description: Get boolean type of settings

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                TBool& aSetting: inout: Parsed setting.

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetBooleanSettingsL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        TBool& aSetting )
    {
    TPtrC string;

    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( aTag, ENoTag ) );
    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        ret = item->GetString( _L( "" ), string );
        if( ret != KErrNone )
            {
            delete item;
            return ret;
            }

        HBufC* stringHbuf = string.AllocL();
        TPtr modifiableString = stringHbuf->Des();
        modifiableString.UpperCase();

        // Parsed parameter is false
        if( modifiableString == _L( "NO" ) )
            {
            aSetting = EFalse;
            }
        // Parsed parameter is true
        else if( modifiableString == _L( "YES" ) )
            {
            aSetting = ETrue;
            }
        else
            {
            delete stringHbuf;
            delete item;
            return KErrArgument;
            }
        delete stringHbuf;
        }

    delete item;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestFrameworkSettings

    Method: GetOverwriteL

    Description: Get Test report file creation mode settings
                 (overwrite settings)

    Parameters: CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TPtrC aTag: in: Tag to setting to be found.
                TBool& aOverwrite: inout: Overwrite setting.

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves is memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestFrameworkSettings::GetOverwriteL( 
                                        CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        TBool& aOverwrite )
    {
    CStifItemParser* item = NULL;
    TRAPD( ret, item = aSectionParser->GetItemLineL( aTag, ENoTag ) );
    if( ret != KErrNone )
        {
        delete item;
        return ret;
        }
    if( !item )
        {
        delete item;
        return KErrNotFound;
        }
    else
        {
        TPtrC string;
        ret = item->GetString( _L( "" ), string );
        if( ret != KErrNone )
            {
            delete item;
            return ret;
            }

        HBufC* stringHbuf = string.AllocL();
        TPtr modifiableString = stringHbuf->Des();
        modifiableString.UpperCase();

        if( modifiableString == _L( "APPEND" ) )
            {
            aOverwrite = EFalse;
            }
        else if( modifiableString == _L( "OVERWRITE") )
            {
            aOverwrite = ETrue;
            }
        else
            {
            delete stringHbuf;
            delete item;
            return KErrArgument;
            }
        delete stringHbuf;
        }

    delete item;

    return KErrNone;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
