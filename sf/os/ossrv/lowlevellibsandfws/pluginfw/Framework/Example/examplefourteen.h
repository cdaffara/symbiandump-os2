// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The definition of a class to be provided by ECom. Uses the CExampleInterface class as a base.
// 
//

/**
 @file
 @internalComponent
*/

#include "Interface.h"
#include "ExampleResolver.h"
#include "TestUtilities.h"	// For __FILE__LINE__

const TUid KImplUid = {0x10009E48};

// ____________________________________________________________________________
// 
/**	Intended usage: This class implements the functionality promised by 
	the CExampleInterface definition class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes.
 */
class CImplementationClassFourteen : public CExampleInterface
{
// Methods
public:
	static CImplementationClassFourteen* NewL(TAny* aInitParams);

	virtual ~CImplementationClassFourteen();

	void DoMethodL();

	TInt FireAndForget();

 	TUid ImplId();

private:

	CImplementationClassFourteen();

	void ConstructL(TAny* aInitParams);

// Provide the CActive overloads
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
/** A place for allocating some memory in the ConstructL */
	HBufC*	iInternalDescriptor;
/** An int to be stored in TLS to test its usage */	
	TInt	iTLSInt;

};  // End of CImplementationClassFourteen definition
