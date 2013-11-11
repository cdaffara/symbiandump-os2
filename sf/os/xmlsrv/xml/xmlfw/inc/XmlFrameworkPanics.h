// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).

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

//



#ifndef XMLFRAMEWORKPANICS_H_

#define XMLFRAMEWORKPANICS_H_



namespace Xml

{





// Panic Codes



/**

Indicates various Panic error conditions within the framework.



@internalTechnology

*/

enum TXmlFrameworkPanic

	{

	/**

	Indicates that a leave code other than that expected was received.

	*/

	EXmlFrameworkPanicUnexpectedLeaveCode		= 0,



	/**

	Indicates that a state other than that expected was reached.

	*/

	EXmlFrameworkPanicUnexpectedState			= 1,



	/**

	Indicates that logic other than that expected was reached.

	*/

	EXmlFrameworkPanicUnexpectedLogic			= 2,



	/**

	Indicates that internal document positioning has gone beyond EOF and has failed.

	*/

	EXmlFrameworkPanicPositionMisallignment		= 3,



	/**

	Indicates that a reference count has reached a negative value.

	*/

	EXmlFrameworkPanicReferenceCountNegative	= 4,



	/**

	Pointer is null, cannot dereference.

	*/

	EXmlFrameworkPanicNullPointer				= 5,



	};



/**

This method panics the thread



@param aError The reason for the panic

@internalTechnology

*/

void Panic(TXmlFrameworkPanic aError);



}

#endif /*XMLFRAMEWORKPANICS_H_*/

