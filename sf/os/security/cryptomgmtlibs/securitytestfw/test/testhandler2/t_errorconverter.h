/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_ERRORCONVERTER_H__
#define __T_ERRORCONVERTER_H__

#include <e32std.h>

class CErrorItem;

/**
 * This class provides useful functions to read and parse the input file.
 */
class CErrorConverter : public CBase
	{
public:
	IMPORT_C static CErrorConverter* NewL(void);
	IMPORT_C static CErrorConverter* NewLC(void);
	IMPORT_C virtual ~CErrorConverter();

public: 
	/*
	Searches the list of standard error codes (e32err.h 0 to -46) finding the matching error code for the string 

	Parameters
	const TDesC& aResult  	[IN] - Error code string to be searched from
	TInt &aError 	 		[OUT] - Matching error code if found

	Returns
	TBool			- True if error code was found, False if it was not
	*/
	IMPORT_C TBool GetExpectedResultL(const TDesC& aResult /*in*/, TInt &aError /*out*/);
	/*
	Searches the list of standard error codes (e32err.h 0 to -46) finding the matching text representaion of the error code

	Parameters
	 TInt &aError 	 	[IN] - Error code to return the string of
	HBufC*& aResult   	[OUT] -Reference to a pointer, pointing to the error string of the matching error code (Dont forget to free it)

	Returns
	TBool			- True if error code was found, False if it was not
	*/
	IMPORT_C TBool GetExpectedResultL(const TInt &aError /*in*/, HBufC*& aResult /*out*/);

	IMPORT_C void AddErrorItem(const TDesC& aErrorName, const TInt &aErrorCode);
private:
	CErrorConverter(){};
	void ConstructL(void);
	void CreateErrorMapL(void);
private:
	RPointerArray<CErrorItem> iErrorMap;
	};

#endif

