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
* Description: This file contains STIFUnit module implementation.
*
*/

/******************************************************************************/
/*
 *
 * Test module configuration 
*/

// Version
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// Uncomment following defines, if you want to provide implementation
// of enlisted virtual methods of test module.
// Implementation part is located at the end of this file.
//#define STIFUNIT_OOMTESTQUERYL
//#define STIFUNIT_OOMTESTINITIALIZEL
//#define STIFUNIT_OOMHANDLEWARNINGL
//#define STIFUNIT_OOMTESTFINALIZEL
// Uncomment following define, if you want to increase heap or stack size.
// #define STIFUNIT_SETHEAPANDSTACKSIZE
/******************************************************************************/


/*
 * Test module internals. Please do not edit them. 
*/

// Includes
#include <e32base.h>

// Literals
_LIT( KUnitLogPath, "\\logs\\testframework\\STIFUnitXXX\\" ); 
_LIT( KUnitLogFile, "STIFUnitXXX.txt" ); 
_LIT( KLogStart, "STIFUnitXXX logging starts!" );

// Defines
#define STIF_UNIT_TEST_CASES "../src/STIFUnitXXXCases.cpp"
#define STIF_UNIT_MODULE_CLASS_NAME CSTIFUnitXXX
#define STIF_UNIT_MODULE_NAME _L("STIFUnitXXX.dll")

// Include STIF unit generic file
#include <StifUnitGeneric.h>

/*
 * Implementation of setHeapAndStack virtual methods.
 * To changes heap and stack size provide new values to iTestThreadStackSize, iTestThreadMinHeap and iTestThreadMaxHeap. 
*/

#ifdef STIFUNIT_SETHEAPANDSTACKSIZE
EXPORT_C TInt SetRequirements( CTestModuleParam*& aTestModuleParam, 
                               TUint32& aParameterValid )
    {
    aParameterValid = KStifTestModuleParameterChanged;
    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL(); 
    // Stack size 
    param->iTestThreadStackSize= 16384; // 16K stack 
    // Heap sizes 
    param->iTestThreadMinHeap = 4096; // 4K heap min 
    param->iTestThreadMaxHeap = 1048576;// 1M heap max 

    return KErrNone;
    }
#undef STIFUNIT_SETHEAPANDSTACKSIZE
#endif

/*
 * User implementation of OOM virtual methods.
 * Providing own implementation requires uncommenting defines at the
 * beginnig of this file.   
*/
#ifdef STIFUNIT_OOMTESTQUERYL
TBool CSTIFUnitXXX::OOMTestQueryL( const TFileName& /*aTestCaseFile*/, 
                                   const TInt aCaseNumber, 
                                   TOOMFailureType& aFailureType, 
                                   TInt& aFirstMemFailure, 
                                   TInt& aLastMemFailure)
    {
    }
#undef STIFUNIT_OOMTESTQUERYL
#endif

#ifdef STIFUNIT_OOMTESTINITIALIZEL
void CSTIFUnitXXX::OOMTestInitializeL( const TFileName& /*aTestCaseFile*/, 
                                       const TInt /*aCaseNumber*/ )
    {
    }
#undef STIFUNIT_OOMTESTINITIALIZEL
#endif

#ifdef STIFUNIT_OOMHANDLEWARNINGL
void CSTIFUnitXXX::OOMHandleWarningL( const TFileName& /*aTestCaseFile*/,
                                      const TInt /*aCaseNumber*/, 
                                      TInt& /*aFailNextValue*/)
    {
    }
#undef STIFUNIT_OOMHANDLEWARNINGL
#endif
    
#ifdef STIFUNIT_OOMTESTFINALIZEL
void CSTIFUnitXXX::OOMTestFinalizeL( const TFileName& /*aTestCaseFile*/, 
                                     const TInt /*aCaseNumber*/ )
    {
    }
#undef STIFUNIT_OOMTESTFINALIZEL
#endif

// End of File
