/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
*/

#ifndef __PKCS8TESTER_H__
#define __PKCS8TESTER_H__

CASN1EncSequence*  GenerateL(TDesC& fileName);
void GenerateAndSaveL(TDesC& origin, TDesC& destination);

class Pkcs8Utils 
	{
public:
	/**
	 * Extracts the nPos command line argument.
	 *
	 * @param nPos The position of the command line argument to extract.
	 *
	 * @return A descriptor containing the command line argument.
	 */	
	 static  HBufC* GetArgumentL(TInt nPos);

	/**
	* Decodes a pkcs8 file
	*
	* @param fileName The name of the file containing the pkcs8 key
	*
	* @return A structure containing the decoded data.
	*/
	 static CDecPKCS8Data* getPkcsDataL(TDesC& fileName);
	
	/**
	* Writes the given ASN1 sequence DER encoded to the given fileName
	*
	* @param fn The file name where the sequecne will be stored.
	* @param seq The ASN1 sequence to write.
	*/
	 static void WriteSequenceL(const TDesC& fn, CASN1EncSequence& seq);
	};
	
#endif
