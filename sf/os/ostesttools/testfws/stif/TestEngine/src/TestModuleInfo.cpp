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
* CTestModuleList class and CTestModuleInfo class member functions.
*
*/

// INCLUDE FILES
#include "TestModuleInfo.h"
#include <e32svr.h>
#include "Logging.h"

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

#define LOGGER iLogger

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: CTestModuleInfo

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleInfo::CTestModuleInfo()
    {
	iLogger = NULL;
    iModuleName = NULL;
    iIniFileName = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: aModuleName: module name
                aLogger: pointer to stif logger

    Return Values: None

    Errors/Exceptions:

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleInfo::ConstructL(TDesC& aModuleName, CStifLogger* aLogger)
    {
	iLogger = aLogger;

	if(iModuleName)
	    {
        delete iModuleName;
		iModuleName = NULL;
        }
    iModuleName = aModuleName.AllocL();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: NewL

    Description: Two-phased constructor.

    Parameters: aModuleName: module name
                aLogger: pointer to stif logger

    Return Values: CTestModuleInfo* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleInfo* CTestModuleInfo::NewL(TDesC& aModuleName, CStifLogger* aLogger)
    {
    CTestModuleInfo* self = new (ELeave) CTestModuleInfo();
    CleanupStack::PushL(self);
    self->ConstructL(aModuleName, aLogger);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: ~CTestModuleInfo

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleInfo::~CTestModuleInfo()
    {
    delete iModuleName;
    delete iIniFileName;

    iCfgFiles.ResetAndDestroy();
    iCfgFiles.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: SetIniFile

    Description: Set ini file

    Parameters: aIniFileName: initialization file name

    Return Values: KErrNone if everything went ok
                   KErrAlreadyExists if ini file was already set
                   Symbian error code otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::SetIniFile(TDesC& aIniFileName)
    {
	if(iIniFileName)
	    {
        return KErrAlreadyExists;
        }

    TRAPD(err, iIniFileName = aIniFileName.AllocL());
    if(err != KErrNone)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not create descriptor (CTestModuleInfo::SetIniFile) for string '%S'. Error %d."), &aIniFileName, err));
            }
        else
            {
            RDebug::Print(_L("Could not create descriptor (CTestModuleInfo::SetIniFile) for string '%S'. Error %d."), &aIniFileName, err);
            }
        return err;
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: AddCfgFile

    Description: Add config file if it is not yet added to the array

    Parameters: aCfgFileName: file name

    Return Values: KErrNone if everything went ok
                   KErrAlreadyExists if config file is already present
                   Symbian error code otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::AddCfgFile(TDesC& aCfgFileName)
    {
    //Check if given file name does not exist already
    TInt cnt = iCfgFiles.Count();
    TInt i;
    TFileName cfgFileName;

    //Check if given module is already on the list
    for(i = 0; i < cnt; i++)
        {
        iCfgFiles[i]->GetCfgFileName(cfgFileName);
        if(cfgFileName == aCfgFileName)
            {
            return KErrAlreadyExists;
			}
        }

    //Add to array
    TInt err = KErrNone;
    CTestCaseFileInfo* tmp = NULL;
    TRAP(err, tmp = CTestCaseFileInfo::NewL(aCfgFileName, iLogger));
    if(err != KErrNone)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not create CTestCaseFileInfo object (CTestModuleInfo::AddCfgFile). Error %d."), err));
            }
        else
            {
            RDebug::Print(_L("Could not create CTestCaseFileInfo object (CTestModuleInfo::AddCfgFile). Error %d."), err);
            }
		return err;
        }
    if(!tmp)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not create CTestCaseFileInfo object (CTestModuleInfo::AddCfgFile)")));
            }
        else
            {
            RDebug::Print(_L("Could not create CTestCaseFileInfo object (CTestModuleInfo::AddCfgFile)"));
            }
		return KErrGeneral;
        }

    err = iCfgFiles.Append(tmp);
    if(err != KErrNone)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not append CTestCaseFileInfo object to array (CTestModuleInfo::AddCfgFile). Error %d."), err));
            }
        else
            {
            RDebug::Print(_L("Could not append CTestCaseFileInfo object to array (CTestModuleInfo::AddCfgFile). Error %d."), err);
            }
        delete tmp;
        return err;
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: CountCfgFiles

    Description: Return number of config files

    Parameters: None

    Return Values: number of config files

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::CountCfgFiles(void)
    {
    return iCfgFiles.Count();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: GetModuleName

    Description: Get module name

    Parameters: None

    Return Values:

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::GetModuleName(TDes& aModuleName)
    {
    aModuleName.Zero();
    if(iModuleName)
        {
        aModuleName.Copy(*iModuleName);
        }
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: GetIniFileName

    Description: Get module ini file name

    Parameters: None

    Return Values:

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::GetIniFileName(TDes& aIniFileName)
    {
    aIniFileName.Zero();
    if(iIniFileName)
        {
        aIniFileName.Copy(*iIniFileName);
        }
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: GetCfgFileName

    Description: Get config file name

    Parameters: aIndex: index of config file

    Return Values: TDesC&: config file name

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleInfo::GetCfgFileName(TInt aIndex, TDes& aCfgFileName)
    {
    aCfgFileName.Zero();
    TInt cnt = iCfgFiles.Count();

    //Check index
    if(aIndex >= cnt || aIndex < 0)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Given index reaches out of array (CTestModuleInfo::GetCfgFileName). Given index: %d. Max allowed index: %d"), aIndex, cnt - 1));
            }
        else
            {
            RDebug::Print(_L("Given index reaches out of array (CTestModuleInfo::GetCfgFileName). Given index: %d. Max allowed index: %d"), aIndex, cnt - 1);
            }
        return KErrArgument;
        }

    TInt ret = iCfgFiles[aIndex]->GetCfgFileName(aCfgFileName);
    return ret;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleInfo

    Method: GetUncheckedCfgFile

    Description: Get first unchecked test case file

    Parameters: None

    Return Values: Pointer to CTestCaseFileInfo object if found
                   Null otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseFileInfo* CTestModuleInfo::GetUncheckedCfgFile(void)
    {
    TInt cnt = iCfgFiles.Count();
    TInt i;

    for(i = 0; i < cnt; i++)
        {
        if(!iCfgFiles[i]->IsChecked())
            {
            return iCfgFiles[i];
            }
        }

    return NULL;
    }

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: CTestModuleList

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleList::CTestModuleList()
    {
	iLogger = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: aLogger: pointer to stif logger

    Return Values: None

    Errors/Exceptions:

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleList::ConstructL(CStifLogger* aLogger)
    {
	iLogger = aLogger;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: NewL

    Description: Two-phased constructor.

    Parameters: aLogger: pointer to stif logger

    Return Values: CTestModuleList* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleList* CTestModuleList::NewL(CStifLogger* aLogger)
    {
    CTestModuleList* self = new (ELeave) CTestModuleList();
    CleanupStack::PushL(self);
    self->ConstructL(aLogger);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: ~CTestModuleList

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleList::~CTestModuleList()
    {
    iTestModules.ResetAndDestroy();
    iTestModules.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: AddTestModule

    Description: Adds new test module to list if specified module is not already added.

    Parameters: aModuleName: module name

    Return Values: KErrNone if opereation succeeded
                   KErrAlreadyExists if module is already present on the list
                   Symbian error code otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleList::AddTestModule(TDesC& aModuleName)
    {
    //Check if given module is already on the list
    CTestModuleInfo* tmp = GetModule(aModuleName);
    if(tmp)
        {
        return KErrAlreadyExists;
        }

    //There is no module on the list, so create new info object
    TInt err = KErrNone;
    TRAP(err, tmp = CTestModuleInfo::NewL(aModuleName, iLogger));
    if(err != KErrNone)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not create CTestModuleInfo object (CTestModuleList::AddTestModule). Error %d."), err));
            }
        else
            {
            RDebug::Print(_L("Could not create CTestModuleInfo object (CTestModuleList::AddTestModule). Error %d."), err);
            }
		return err;
        }
    if(!tmp)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not create CTestModuleInfo object (CTestModuleList::AddTestModule)")));
            }
        else
            {
            RDebug::Print(_L("Could not create CTestModuleInfo object (CTestModuleList::AddTestModule)"));
            }
		return KErrGeneral;
        }

    //Add to list
    err = iTestModules.Append(tmp);
    if(err != KErrNone)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Could not append CTestModuleInfo object to array (CTestModuleList::AddTestModule). Error %d."), err));
            }
        else
            {
            RDebug::Print(_L("Could not append CTestModuleInfo object to array (CTestModuleList::AddTestModule). Error %d."), err);
            }
        delete tmp;
        return err;
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: Count

    Description: Counts test modules

    Parameters: None

    Return Values: Number of test modules on list

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleList::Count()
    {
	return iTestModules.Count();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: GetModule

    Description: Searches test module with given name

    Parameters: aModuleName: module name

    Return Values: Pointer to found object or NULL when not found

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleInfo* CTestModuleList::GetModule(TDesC& aModuleName)
    {
    TInt cnt = iTestModules.Count();
    TInt i;
    CTestModuleInfo* tmp;
    TName moduleName;

    //Check if given module is already on the list
    for(i = 0; i < cnt; i++)
        {
        tmp = iTestModules[i];
        tmp->GetModuleName(moduleName);
        if(moduleName == aModuleName)
            {
            return tmp;
			}
        }
    return NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: GetModule

    Description: Searches test module with given index

    Parameters: aIndex: module index

    Return Values: Pointer to found object or NULL when not found

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleInfo* CTestModuleList::GetModule(TInt aIndex)
    {
    TInt cnt = iTestModules.Count();

    //Check index
    if(aIndex >= cnt || aIndex < 0)
        {
        if(iLogger)
            {
            __TRACE(KError, (_L("Given index reaches out of array (CTestModuleList::GetModule). Given index: %d. Max allowed index: %d"), aIndex, cnt - 1));
            }
        else
            {
            RDebug::Print(_L("Given index reaches out of array (CTestModuleList::GetModule). Given index: %d. Max allowed index: %d"), aIndex, cnt - 1);
            }
        return NULL;
        }

    //return module
    return iTestModules[aIndex];
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleList

    Method: GetUncheckedCfgFile

    Description: Get first unchecked test case file (module: testscripter and testcombiner)

    Parameters: None

    Return Values: Pointer to CTestCaseFileInfo object if found
                   Null otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestCaseFileInfo* CTestModuleList::GetUncheckedCfgFile(void)
    {
    CTestCaseFileInfo* ret = NULL;

    TBuf<12> modname(_L("testscripter"));
    CTestModuleInfo* mod = GetModule(modname);

    if(mod)
        {
        ret = mod->GetUncheckedCfgFile();
        }

    if(!ret)
	    {
        modname.Copy(_L("testcombiner"));
        mod = GetModule(modname);
        if(mod)
            {
            ret = mod->GetUncheckedCfgFile();
            }
        }

    return ret;
    }

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: CTestCaseFileInfo

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseFileInfo::CTestCaseFileInfo()
    {
	iLogger = NULL;
    iCfgFileName = NULL;
    iChecked = EFalse;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: aCfgFileName: config file name
                aLogger: pointer to Stif logger

    Return Values: None

    Errors/Exceptions:

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseFileInfo::ConstructL(TDesC& aCfgFileName, CStifLogger* aLogger)
    {
	iLogger = aLogger;

	if(iCfgFileName)
	    {
        delete iCfgFileName;
		iCfgFileName = NULL;
        }
    iCfgFileName = aCfgFileName.AllocL();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: NewL

    Description: Two-phased constructor.

    Parameters: aCfgFileName: config file name
                aLogger: pointer to stif logger

    Return Values: CTestCaseFileInfo* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseFileInfo* CTestCaseFileInfo::NewL(TDesC& aCfgFileName, CStifLogger* aLogger)
    {
    CTestCaseFileInfo* self = new (ELeave) CTestCaseFileInfo();
    CleanupStack::PushL(self);
    self->ConstructL(aCfgFileName, aLogger);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: ~CTestCaseFileInfo

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseFileInfo::~CTestCaseFileInfo()
    {
    delete iCfgFileName;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: GetCfgFileName

    Description: Get config file name

    Parameters: None

    Return Values: TDesC& : config file name

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestCaseFileInfo::GetCfgFileName(TDes& aCfgFileName)
    {
    aCfgFileName.Zero();
    if(iCfgFileName)
        {
        aCfgFileName.Copy(*iCfgFileName);
        }
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: SetChecked

    Description: Set configuration file as already checked

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void CTestCaseFileInfo::SetChecked(void)
    {
    iChecked = ETrue;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseFileInfo

    Method: IsChecked

    Description: Return if config file has been checked

    Parameters: None

    Return Values: 0 if test case file has not been checked yet
                   other value otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestCaseFileInfo::IsChecked(void)
    {
    return iChecked;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
