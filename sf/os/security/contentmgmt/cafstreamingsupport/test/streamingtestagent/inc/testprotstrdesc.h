// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the test protected stream description object for the streaming test agent.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TESTPROTSTRDESC_H_
#define TESTPROTSTRDESC_H_

#include <caf/streaming/protectedstreamdesc.h>

namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestProtectedStreamDesc) : public CProtectedStreamDesc
	/**
	 	Represents parameters for the test protected stream.
	 */
		{
	public:
		IMPORT_C static CTestProtectedStreamDesc* NewL(const TDesC& aOutput);
		IMPORT_C ~CTestProtectedStreamDesc();
		IMPORT_C CKeyStreamSink* CreateKeyStreamSinkLC() const;
	
	private:
		CTestProtectedStreamDesc();
		void ConstructL(const TDesC& aOutput);
		
	private:
		HBufC* iOutput; ///< The fully qualified path of the file which the decrypted key is written.
		};
	}//End of the namespace StreamAccess
#endif /*TESTPROTSTRDESC_H_*/
