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
// The ECom impletementation of testing contact matching.
//

#include "t_logcntmatchplugin.h"
#include <e32def.h>
#include <ecom/implementationproxy.h>
   
/** Creates a CLogTestCntMatch object
*/
CLogTestCntMatch* CLogTestCntMatch::NewL()
    {
    RDebug::Print(_L("CLogTestCntMatch::NewL()"));
    CLogTestCntMatch* self = new (ELeave) CLogTestCntMatch;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CLogTestCntMatch::ConstructL()
    {
    iTelNumbers = new(ELeave) CDesCArrayFlat(KNumberOfItems);
    iContactFirstNames = new(ELeave) CDesCArrayFlat(KNumberOfItems);
    iContactLastNames = new(ELeave) CDesCArrayFlat(KNumberOfItems);
    }

CLogTestCntMatch::CLogTestCntMatch()
    {
    }

CLogTestCntMatch::~CLogTestCntMatch()
    {
    delete iTelNumbers;
    delete iContactFirstNames;
    delete iContactLastNames;
    }

/** Interface of ECOM. In real phone, it should opens the contacts DB
*/
void CLogTestCntMatch::OpenContactsL()
    {
    RDebug::Print(_L("Test Mock Contact Matching: OpenContactsL "));
    
    iTelNumbers->Reset();
    iContactFirstNames->Reset();
    iContactLastNames->Reset();
    
    iTelNumbers->AppendL(KNumber1);
    iContactFirstNames->AppendL(KFirstName1);
    iContactLastNames->AppendL(KLastName1); 
    
    iTelNumbers->AppendL(KNumber2);
    iContactFirstNames->AppendL(KFirstName2);
    iContactLastNames->AppendL(KLastName2);       
    
    iTelNumbers->AppendL(KNumber3);
    iContactFirstNames->AppendL(KFirstName3);
    iContactLastNames->AppendL(KLastName3);       
    
    iTelNumbers->AppendL(KNumber4);
    iContactFirstNames->AppendL(KFirstName4);
    iContactLastNames->AppendL(KLastName4);       
    
    iTelNumbers->AppendL(KNumber5);
    iContactFirstNames->AppendL(KFirstName5);
    iContactLastNames->AppendL(KLastName5);       
    
    iTelNumbers->AppendL(KNumber6);
    iContactFirstNames->AppendL(KFirstName6);
    iContactLastNames->AppendL(KLastName6);       
    
    iTelNumbers->AppendL(KNumber7);
    iContactFirstNames->AppendL(KFirstName7);
    iContactLastNames->AppendL(KLastName7);       
    }
 
/** Interface of ECOM. In real phone, it should closes the contacts DB
*/
void CLogTestCntMatch::CloseContacts()
    {
    RDebug::Print(_L("OpenContactsL is called"));
    iContactFirstNames->Reset();
    iContactLastNames->Reset();
    iTelNumbers->Reset();
    }
    
/** Attempts to find a contact item ID for the contact items which contains
    the specified telephone number in a telephone, fax or SMS type field for tests.
    If more than one contact item contains the telephone number this should be 
    treated the same as no contact found.
    @param aNumber Phone number string
    @param aMatchLengthFromRight Number of digits from the right of the phone number to use. 
    @return The contact Id found that contains the phone number. KLogNullContactId if none or more than one is found.
*/
TLogContactItemId CLogTestCntMatch::MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight)
    {
    RDebug::Print(_L("CLogTestCntMatch::MatchPhoneNumberL()"));
    __ASSERT_ALWAYS(iTelNumbers->Length() == KNumberOfItems && 
                iContactFirstNames->Length()==KNumberOfItems &&
                iContactLastNames->Length()==KNumberOfItems, _L("CLogTestCntMatch::MatchPhoneNumberL"));   
    
    TLogContactItemId contactId = KLogNullContactId;
    TInt numHit = 0;
    const TInt KMinLength = 7;
    TInt pos = -1;
    if(aNumber.Length() >= KMinLength)
        {
        for (TInt ii = 0; ii < KNumberOfItems && numHit < 2; ++ii)
             {
             TPtrC number((*iTelNumbers)[ii]);
             TInt numDigToMatch = aNumber.Length()<aMatchLengthFromRight?aNumber.Length():aMatchLengthFromRight;
             if(number.Length() < numDigToMatch)
                 {//This is not matched
                 continue;
                 }
             if(number.Right(numDigToMatch).CompareF(aNumber.Right(numDigToMatch)) == 0)
                  {
                  pos = ii;
                  ++numHit;
                  }
             }
      
        // Only set contactId if we have exactly one match
        if (numHit == 1)
            {
            contactId = static_cast<TLogContactItemId>(pos)+1;
            }
      }
    
    return contactId;
    }

/** Gets the text data for the family and given name fields of a given contact Id for tests.
    
    @param aContactId Contact Id to find data for
    @param aName On return contains the family and given name in the desired format if found, leave with KErrNotFound otherwise.
    @param aNameFormat Desired format of returned string - Chinese or Western format
*/
void CLogTestCntMatch::ReadContactNameL(TLogContactItemId aContactId, TDes &aName, TLogContactNameFormat aNameFormat)
    {
    RDebug::Print(_L("CLogTestCntMatch::ReadContactNameL()"));
    __ASSERT_ALWAYS(iTelNumbers->Length() == KNumberOfItems && 
                 iContactFirstNames->Length()==KNumberOfItems &&
                 iContactLastNames->Length()==KNumberOfItems, _L("CLogTestCntMatch::ReadContactNameL"));
    
    if(aContactId < 1 && aContactId > KNumberOfItems)
        {
        User::Leave(KErrNotFound);
        }
     if(aNameFormat == ELogWesternFormat)
        {
        aName.Append((*iContactFirstNames)[aContactId-1]);
        aName.Append(' ');
        aName.Append((*iContactLastNames)[aContactId-1]);
        }
    else //ELogChineseFormat
        {
        aName.Append((*iContactLastNames)[aContactId-1]); 
        aName.Append((*iContactFirstNames)[aContactId-1]);
        aName.Append(' ');
       }
    }
		
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x2000862a, CLogTestCntMatch::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

