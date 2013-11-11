/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32base.h>
#include <ecom/ecom.h>
#include "ct/ccttokentype.h"
#include "ct/ccttokentypeinfo.h"
#include "ct/interfaceuid.hrh"
#include "cttokentypedeletehandler.h"

const TUid KInterfaceUID = {CT_INTERFACE_UID};

/**
 * Class that can be pushed onto the clean-up stack to delete the contents of an
 * RPointerArray if a leave occurs.
 */
template <class T> class TArrayDeleter
	{
public:
	TArrayDeleter(RPointerArray<T>& aArray) : iArray(aArray) {};

	void Release() { iArray.ResetAndDestroy();};

private:
	RPointerArray<T>& iArray;
	};

/**
 * Implement a delete handler that informs ecom that the object has been
 * deleted.
 */
NONSHARABLE_CLASS(CEComDeleteHandler) : public CCTTokenTypeDeleteHandler
	{
public:
	CEComDeleteHandler();
	virtual ~CEComDeleteHandler();

	CCTTokenType* CreateImplementationL(TUid aUid);

private:
	TUid iDtorIDKey;
	};

CEComDeleteHandler::CEComDeleteHandler()
	{
	}

CEComDeleteHandler::~CEComDeleteHandler()
	{
	REComSession::DestroyedImplementation(iDtorIDKey);
	}

// Get the implementation from ECom using this class to hold the desructor id key
CCTTokenType* CEComDeleteHandler::CreateImplementationL(TUid aUid)
	{
	TAny* ptr = REComSession::CreateImplementationL(aUid, iDtorIDKey);
	return static_cast<CCTTokenType*>(ptr);
	}

// Create the implemenation and set up delete handler
CCTTokenType* CCTTokenType::CreateTokenTypeLC(TUid aUid)
	{
	// Create delete handler
	CEComDeleteHandler* deleteHandler = new (ELeave) CEComDeleteHandler();
	CleanupStack::PushL(deleteHandler);

	CCTTokenType* ccttokentypePtr = deleteHandler->CreateImplementationL(aUid);
	
	// Set delete handler, transfers ownership
	ccttokentypePtr->iDeleteHandler = deleteHandler;
	CleanupStack::Pop(deleteHandler);

	CleanupStack::PushL(ccttokentypePtr);
	return ccttokentypePtr;
	}

// Opens a token type by Info object.
EXPORT_C CCTTokenType* CCTTokenType::NewL(const CCTTokenTypeInfo& aInfo, RFs aFs)
	{
	CCTTokenType* ccttokentypePtr = CreateTokenTypeLC(aInfo.Type());
	
	// Call CCTTokenType's ConstructL to set the label and UID
	ccttokentypePtr->ConstructL(aInfo.Type(), aInfo.Label(), aFs);
	CleanupStack::Pop(ccttokentypePtr);

	return ccttokentypePtr;
	}

// Oprens a token type by UID. The complication here is that we need to
// get the label from ECom
EXPORT_C CCTTokenType* CCTTokenType::NewL(TUid aUID, RFs aFs)
	{
	CCTTokenType* ccttokentypePtr = CreateTokenTypeLC(aUID);

	// We need to search the ECom registry to get the label.
	TPtrC label(0,0);
	RImplInfoPtrArray infoArray;
	TArrayDeleter<CImplementationInformation> deleter(infoArray);
	CleanupReleasePushL(deleter);

	REComSession::ListImplementationsL(KInterfaceUID, infoArray);

	TInt index = KErrNotFound;	
	for (TInt i = 0 ; i < infoArray.Count() ; ++i) 
		{		
		// Don't accept RAM based plugins
		if (infoArray[i]->RomBased() && infoArray[i]->ImplementationUid() == aUID)
			{
			index = i;
			break;
			}
		}

	// This will only fail if we were passed a a valid UID for a different type
	// of ECom interface.
	_LIT(KPanic, "CTFRAMEWORK");
	__ASSERT_ALWAYS(index != KErrNotFound, User::Panic(KPanic, 1));

	label.Set(infoArray[index]->DisplayName());

	ccttokentypePtr->ConstructL(aUID, label, aFs);
	CleanupStack::PopAndDestroy(); // deleter
	CleanupStack::Pop(ccttokentypePtr);
	return ccttokentypePtr;
	}
