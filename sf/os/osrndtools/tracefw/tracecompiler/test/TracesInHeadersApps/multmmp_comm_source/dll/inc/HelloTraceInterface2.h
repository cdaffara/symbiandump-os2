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


#ifndef __HELLOTRACEINTERFACE2_H__
#define __HELLOTRACEINTERFACE2_H__

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf

const TInt KHelloTraceDllBufferLength = 10;
typedef TBuf<KHelloTraceDllBufferLength> THelloTraceDllExampleString;

class CHelloTraceInterface : public CBase
    {
public:
    IMPORT_C static CHelloTraceInterface* NewL();
    IMPORT_C static CHelloTraceInterface* NewLC();
    IMPORT_C ~CHelloTraceInterface();

public:
    IMPORT_C static TVersion Version();
    IMPORT_C const TPtrC String() const;
    IMPORT_C void AddCharL(const TChar& aChar);
    IMPORT_C void RemoveLast();
    IMPORT_C void ResetString();
    
    IMPORT_C static TVersion VersionDup();
    IMPORT_C const TPtrC StringDup() const;
    IMPORT_C void AddCharDupL(const TChar& aChar);
    IMPORT_C void RemoveLastDup();
    IMPORT_C void ResetStringDup();

    IMPORT_C static TVersion VersionDup2();
    IMPORT_C const TPtrC StringDup2() const;
    IMPORT_C void AddCharDup2L(const TChar& aChar);
    IMPORT_C void RemoveLastDup2();
    IMPORT_C void ResetStringDup2();
private:
    CHelloTraceInterface();
    void ConstructL();

private:
    // data
    THelloTraceDllExampleString* iString;
    };

#endif  // __HELLOTRACEINTERFACE2_H__

