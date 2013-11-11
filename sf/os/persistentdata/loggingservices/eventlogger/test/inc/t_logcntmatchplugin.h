// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header file of the ECom impletementation of  for testing contact matching.
//

#ifndef __CNTMATCHLOG_H__
#define __CNTMATCHLOG_H__

#include <e32base.h>
#include <logcntmodel.h>
#include <badesca.h>
#include <e32des16.h>

//The contact items
const TInt KNumberOfItems = 7; 

_LIT(KNumber1, "447700900000");
_LIT(KNumber2, "+441632960000");
_LIT(KNumber3, "07700900001");
_LIT(KNumber4, "447756900111");
_LIT(KNumber5, "447756900111");
_LIT(KNumber6, "1234567890");
_LIT(KNumber7, "0123456789");

_LIT(KFirstName1, "Barney");
_LIT(KFirstName2, "Elma");
_LIT(KFirstName3, "Peter");
_LIT(KFirstName4, "Abc");
_LIT(KFirstName5, "Rtyu");
_LIT(KFirstName6, "Zxcvb");
_LIT(KFirstName7, "AAA");

_LIT(KLastName1, "Rubble");
_LIT(KLastName2, "Fudd");
_LIT(KLastName3, "Harper");
_LIT(KLastName4, "Lkjhgf");
_LIT(KLastName5, "Poiuytqwe");
_LIT(KLastName6, "Mnbvcxz");
_LIT(KLastName7, "BBB");

/** Plugin implementation that provide contacts matching functionality. 
 
@internalAll
*/
class CLogTestCntMatch : public CLogCntModel
    {
public:
    static CLogTestCntMatch* NewL();
    virtual void OpenContactsL();
    virtual void CloseContacts();
    virtual TLogContactItemId MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight);
    virtual void ReadContactNameL(TLogContactItemId aContactId, TDes &aName, TLogContactNameFormat aNameFormat);

private:
    ~CLogTestCntMatch();
    CLogTestCntMatch();
    
    void ConstructL();
    CDesCArray* iTelNumbers;
    CDesCArray* iContactFirstNames;
    CDesCArray* iContactLastNames;
    };
	
#endif
