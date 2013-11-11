/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
 

#ifndef HELLOTRACEFN_H_
#define HELLOTRACEFN_H_

#include <e32base.h>    // CBase

class CHelloTraceFn : public CBase
    {
public:
    
    // Note the Trace Builder will only parse this if it's in a cpp file
    // but not from here :(
    // Also if you add a new enum you need to reparse the enum - it doesn't
    // get automatically reparsed by the Trace Compiler :(
    enum TFnEnum
        {
        EExample1,
        EExample2 = 2, 
        // EExample3 = EExample2 + 1, // The Trace Builder can't parse this :(
        };
    
public:
    static CHelloTraceFn* NewL();
    static CHelloTraceFn* NewLC();
    ~CHelloTraceFn();
   
public: // Fn entry / exit tracing examples 
    void Simple();
    void OutputsParams(TInt aParam, TFnEnum aEnumParam);
    void OutputsParams2(TInt aParam, TFnEnum aEnumParam);
    void OutputsParams3(TInt aParam, TFnEnum aEnumParam);
    void OutputsSignedIntegers(TInt8 aParam8,   TInt16 aParam16, 
                               TInt32 aParam32, TInt64 aParam64);
    void OutputsUnsignedIntegers(TUint8 aParam8,   TUint16 aParam16, 
                                 TUint32 aParam32, TUint64 aParam64);
     
    TInt OutputsTIntReturnValue();
    TInt64 OutputsTInt64ReturnValue();
    TFnEnum OutputsTFnEnumReturnValue();

    CActive* OutputsUnknownPtrType(CActive* aActive);
    CActive& OutputsUnknownRefType(CActive& aActive);
    
    void OutputMissingParams(TInt aUsedParam, TInt aCommentParam, TInt aMissingParam);

#ifdef _DEBUG    
    void PreProcessedFn(TInt aDbgParam);
#else
    void PreProcessedFn();
#endif
    
    static void StaticOutputsParamsL(TInt aParam);  
    
    // duplicate all the function and put the impl in the header file HelloTraceFnDup.h
    void SimpleDup();
    void OutputsParamsDup(TInt aParam, TFnEnum aEnumParam);
    void OutputsSignedIntegersDup(TInt8 aParam8,   TInt16 aParam16, 
                               TInt32 aParam32, TInt64 aParam64);
    void OutputsUnsignedIntegersDup(TUint8 aParam8,   TUint16 aParam16, 
                                 TUint32 aParam32, TUint64 aParam64);
     
    TInt OutputsTIntReturnValueDup();
    TInt64 OutputsTInt64ReturnValueDup();
    TFnEnum OutputsTFnEnumReturnValueDup();

    CActive* OutputsUnknownPtrTypeDup(CActive* aActive);
    CActive& OutputsUnknownRefTypeDup(CActive& aActive);
    
    void OutputMissingParamsDup(TInt aUsedParam, TInt aCommentParam, TInt aMissingParam);

#ifdef _DEBUG    
    void PreProcessedFnDup(TInt aDbgParam);
#else
    void PreProcessedFnDup();
#endif
    static void StaticOutputsParamsDupL(TInt aParam);  
    
    // duplicate all the function again and put the impl in the header file HelloTraceFnDup2.h
    void SimpleDup2();
    void OutputsParamsDup2(TInt aParam, TFnEnum aEnumParam);
    void OutputsSignedIntegersDup2(TInt8 aParam8,   TInt16 aParam16, 
                               TInt32 aParam32, TInt64 aParam64);
    void OutputsUnsignedIntegersDup2(TUint8 aParam8,   TUint16 aParam16, 
                                 TUint32 aParam32, TUint64 aParam64);
     
    TInt OutputsTIntReturnValueDup2();
    TInt64 OutputsTInt64ReturnValueDup2();
    TFnEnum OutputsTFnEnumReturnValueDup2();

    CActive* OutputsUnknownPtrTypeDup2(CActive* aActive);
    CActive& OutputsUnknownRefTypeDup2(CActive& aActive);
    
    void OutputMissingParamsDup2(TInt aUsedParam, TInt aCommentParam, TInt aMissingParam);

#ifdef _DEBUG    
    void PreProcessedFnDup2(TInt aDbgParam);
#else
    void PreProcessedFnDup2();
#endif
    static void StaticOutputsParamsDup2L(TInt aParam);  


private:
    CHelloTraceFn();
    };

#endif /* HELLOTRACEFN_H_ */
