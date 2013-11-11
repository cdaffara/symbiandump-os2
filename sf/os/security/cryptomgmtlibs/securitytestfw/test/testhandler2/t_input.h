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


#ifndef __T_INPUT_H__
#define __T_INPUT_H__

#include <e32cons.h>
#include <f32file.h>
#include <e32std.h>

/**
 * This class provides useful functions to read and parse the input file.
 */
class Input
	{
public: // static functions no object required
	/*
	Parses the parameters aBuf to find the section of the script within parameters aStart and aEnd
	NB use these if you want just want to use the result temporarily, because it just points
	to the result in aBuf

	Parameters
	const TDesC& aBuf	[IN] - Buffer to search
	const TDesC& aStart	[IN] - Start tag t be searched for
	const TDesC& aEnd   [IN] - (Optional)End tag t be searched for (Optional, if not supplied function will default aStart with a / i.e. if aStart <tag> aEnd will be </tag>
	TInt& aPos			[IN/OUT]- (Optional)Position in buffer to start searching, this will be updated with the position after the search (Optional) default zero 
	TInt& aError		[OUT]- (Optional)Returns negative number if start tag cannot be found (Optional)

	Returns
	TPtrC		- Returns a TPtr to the position in the buffer between the start and end tags
	*/
	// base functions for parsing tokens in scripts, but better to use functions below
	IMPORT_C static TPtrC8 ParseElement(const TDesC8& aBuf, const TDesC8& aStart);
	IMPORT_C static TPtrC8 ParseElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd);
	IMPORT_C static TPtrC8 ParseElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd, TInt& aPos);
	IMPORT_C static TPtrC8 ParseElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd, TInt& aPos, TInt& aError);
	IMPORT_C static TPtrC16 ParseElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd);
	IMPORT_C static TPtrC16 ParseElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd, TInt& aPos);
	IMPORT_C static TPtrC16 ParseElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd, TInt& aPos, TInt& aError);
	IMPORT_C static TPtrC ParseElement(const TDesC& aBuf, const TDesC& aStart);
	IMPORT_C static TPtrC ParseElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos);
	IMPORT_C static TPtrC ParseElement(const TDesC& aBuf,  const TDesC& aStart, TInt& aPos, TInt& aError);
	/**
	 * Reads the between the start and end tags assuming the data is in ASCII
	 * hex format.  Returns an HBufC8* which is a binary representation of that
	 * ASCII hex.  (ie a string of length 6 becomes a string of length 3)
	 */
	IMPORT_C static HBufC8* ParseElementHexL(const TDesC8& aBuf, const TDesC8& aStart);
	/**
	 * Returns ETrue if the the value was "True" or "ETrue" and returns EFalse
	 * if the value was "False" or "EFalse" (case insensitive comparisons all
	 * round).  Leaves if none of the above is true 
	 */
	IMPORT_C static TBool ParseElementBoolL(const TDesC8& aBuf, const TDesC8& aStart);
	/**
	 * Parses the parameters aBuf to find the section of the script within
	 * parameters aStart and aEnd, and parses the contants as an integer.  Both
	 * decimal ('123') and hex ('0x7b') format strings are supported.
	 *
	 * @param aBuf		[IN] - Buffer to search
	 * @param aStart	[IN] - Start tag t be searched for
	 * @param aEnd   	[IN] - (Optional)End tag t be searched for (Optional, if not supplied function will default aStart with a / i.e. if aStart <tag> aEnd will be </tag>
	 * @param aPos		[IN/OUT]- (Optional)Position in buffer to start searching, this will be updated with the position after the search (Optional) default zero 
	 * @param aError	[OUT]- (Optional)Returns negative number if start tag cannot be found (Optional)
	 *
	 * @return The parsed integer value, or zero.
	 */
	IMPORT_C static TUint ParseIntElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd);
	IMPORT_C static TUint ParseIntElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd, TInt& aPos);
	IMPORT_C static TUint ParseIntElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd, TInt& aPos, TInt& aError);
	IMPORT_C static TUint ParseIntElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd);
	IMPORT_C static TUint ParseIntElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd, TInt& aPos);
	IMPORT_C static TUint ParseIntElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd, TInt& aPos, TInt& aError);
	IMPORT_C static TUint ParseIntElement(const TDesC& aBuf, const TDesC& aStart);
	IMPORT_C static TUint ParseIntElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos);
	IMPORT_C static TUint ParseIntElement(const TDesC& aBuf,  const TDesC& aStart, TInt& aPos, TInt& aError);
	/*
	Searches the list of standard error codes (e32err.h 0 to -46) finding the matching error code for the string 

	Parameters
	const TDesC& aResult  	[IN] - Error code string to be searched from
	TInt &aError 	 		[OUT] - Matching error code if found

	Returns
	TBool			- True if error code was found, False if it was not
	*/
	IMPORT_C static TBool GetExpectedResultL(const TDesC& aResult /*in*/, TInt &aError /*out*/);
	/*
	Searches the list of standard error codes (e32err.h 0 to -46) finding the matching text representaion of the error code

	Parameters
	 TInt &aError 	 	[IN] - Error code to return the string of
	HBufC*& aResult   	[OUT] -Reference to a pointer, pointing to the error string of the matching error code (Dont forget to free it)

	Returns
	TBool			- True if error code was found, False if it was not
	*/
	IMPORT_C static TBool GetExpectedResultL(const TInt &aError /*in*/, HBufC*& aResult /*out*/);
	/*
	Parses the parameters aBuf to find the section of the script within parameters aStart and aEnd, 
	then allocs the required memory and places a copy of this section in aMember
	use these if you want the result copied into a new area of memory to use, for example when
	stored a member data

	Parameters
	HBufC*& aMember		[OUT] - Reference to a pointer that will point to a copy of the section of aBuf 
	const TDesC& aBuf	[IN] - Buffer to search
	const TDesC& aStart	[IN] - Start tag t be searched for
	const TDesC& aEnd   [IN] - (Optional) End tag t be searched for (, if not supplied function will default aStart with a / i.e. if aStart <tag> aEnd will be </tag>
	TInt& aPos			[IN/OUT]- (Optional) Position in buffer to start searching, this will be updated with the position after the search (Optional) default zero 
	TInt& aError		[OUT]- (Optional) Returns negative number if start tag cannot be found (Optional)

	Returns
	TPtrC		- Returns a TPtr to the position in the buffer between the start and 
	end tags
	*/
	IMPORT_C static void ParseElementL(HBufC*& aMember, const TDesC& aBuf, 
		const TDesC& aStart);
	IMPORT_C static void ParseElementL(HBufC8*& aMember, const TDesC& aBuf, 
		const TDesC& aStart);
	IMPORT_C static void ParseElementL(HBufC*& aMember, const TDesC& aBuf,
		const TDesC& aStart, TInt& aPos);
	IMPORT_C static void ParseElementL(HBufC8*& aMember, const TDesC& aBuf,
		const TDesC& aStart, TInt& aPos);
	IMPORT_C static void ParseElementL(HBufC*& aMember, const TDesC& aBuf,  
		const TDesC& aStart, TInt& aPos, TInt& aError);
	IMPORT_C static void ParseElementL(HBufC8*& aMember, const TDesC& aBuf,  
		const TDesC& aStart, TInt& aPos, TInt& aError);
	/*
	Reads an entire file into memory into a HBuf8

	Parameters
	const TDesC& aFilename 	[IN] - Name of file to read
	const TDesC& aPath		[IN] - (Optional) Path of file, if aPath is missing the path is taken from aFilename
	RFs &aFs				[IN] - Reference to the file system handle class

	Returns
	HBufC8*		-  Pointer to the 8 bit file data
	*/
	IMPORT_C static HBufC8* ReadFileL(const TDesC& aFilename, RFs& aFs);
	IMPORT_C static HBufC8* ReadFileL(const TDesC& aFilename, const TDesC& aPath, RFs& aFs);
	IMPORT_C static HBufC8* ReadFileLC(const TDesC& aFilename, RFs& aFs);
	IMPORT_C static HBufC8* ReadFileLC(const TDesC& aFilename, const TDesC& aPath, RFs& aFS);
	/*
	Reads a ascii hex file into a binary HBufC i.e. a file containing "FFAA11" will be converted into a 3 byte buffer

	Parameters
	const TDesC& aFilename 	[IN] - Name of file including path to read
	RFs &aFs			[IN] - Reference to the file system handle class

	Returns
	HBufC8*		-  Pointer to the 8 bit file data
	*/
	IMPORT_C static HBufC8* ReadHexFileLC(const TDesC& aFilename, RFs& aFs);
	/**
	 * This function reads an entire Unicode file and returns it as an HBufC.
	 * This function only works for Unicode builds of EPOC.
	 */
	IMPORT_C static HBufC* ReadFile16LC(const TDesC& aFilename, RFs& aFS);
	/*
	Parses the parameters aBuf to find multiple sections of the script within parameters aStart and aEnd, then adds them to the aElements list
	For instance, if the aBuf argument is <StartTag>abc<EndTag><StartTag>xyz<EndTag>,  then aElements will contain 2 elements: abc and xyz.

	const TDesC& aBuf	[IN] - Buffer to search
	const TDesC& aStart	[IN] - Start tag to be searched for
	const TDesC& aEnd   [IN]- End tag t be searched for
	RPointerArray<HBufC>& aElements [OUT] - Array of sections found
	TInt& aPos		[IN/OUT]- Position in buffer to start searching, this will be updated with the position after the search

	Returns
	None
	 */
	IMPORT_C static void ParseElementListL(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, 
		RPointerArray<HBufC>& aElements, TInt& aPos);

	
	/**
	 * Parse a string containg a capability name.
	 * @leave KErrArgument If the string is not a recognised capability name.
	 */
	IMPORT_C static TCapability ParseCapabilityNameL(const TDesC8& aName);
	
	/**
	 * Parse a section of script file describing a capability set.
	 * 
	 * The script contains zero or more capabilities contained in <capability>
	 * tags.
	 */
	IMPORT_C static void ParseCapabilitySetL(const TDesC8& aIn, TCapabilitySet& aOut);

	/**
	 * Parse a section of script file describing a security policy.
	 * 
	 * The script contains zero or more capabilities contained in <capability>
	 * tags.
	 */
	IMPORT_C static void ParseSecurityPolicyL(const TDesC8& aIn, TSecurityPolicy& aOut);
	
	};

#endif
