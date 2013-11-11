/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Code used to actually access lookup tables. Included by
* automatically generated files which define the tables to
* implement the functions prototyped in Charconv_table_utilties.h 
*
*/


/**
 
 This function attempts to look up "Foreign" character and return
 the Unicode character defined as being the appropriate representation
 for this character set.
 @param aRow The most significant 8 bits of the input charachter.
 @param aColumn The least significant 8 bits of the input charachter.
 @return KUnicodeUnmappedChar if no mapping is defined for the input character, otherwise a valid unicode character.
 @internalTechnology
*/
TUint16 lookupUnicodeChar(TUint aRow, TUint aColumn)
	{
	const TUint16* unicodeColumn ;
	TUint16 unicodeChar ;
	
	unicodeColumn = ForeignToUnicodeRowMappings[aRow] ;
	if (unicodeColumn)
		{
		if ((aColumn >= unicodeColumn[KlowestIndex]) &&
		    (aColumn <= unicodeColumn[KhighestIndex]))
			{
			aColumn += KFirstDataIndex ;
			aColumn -= unicodeColumn[KlowestIndex] ;
			unicodeChar = unicodeColumn[aColumn] ;		                                         
			}
		else
			{
			unicodeChar = KUnicodeUnmappedChar ;	
			}
		}
	else
		{
			unicodeChar = KUnicodeUnmappedChar ;				
		}
	return unicodeChar ;
	}



/**
This function attempts to look up a Unicode character and return
the "Foreign" character defined as being the appropriate representation
for this character set.
@param aRow The most significant 8 bits of the input character.
@param aColumn The least significant 8 bits of the input character.
@return KForeignUnmappedChar if no mapping is defined for the input character, otherwise a valid unicode character.
@internalTechnology
*/	
TUint16 lookupForeignChar(TUint aRow, TUint aColumn)
	{
	
	const TUint16* foreignColumn  ;
	TUint16 foreignChar ;
	
	foreignColumn = UnicodeToForeignRowMappings[aRow] ;
	if (foreignColumn)
		{
		if ((aColumn >= foreignColumn[KlowestIndex]) &&
		    (aColumn <= foreignColumn[KhighestIndex]))
			{
			aColumn += KFirstDataIndex ;
			aColumn -= foreignColumn[KlowestIndex] ;
			foreignChar = foreignColumn[aColumn] ;		                                         
			}
		else
			{
			foreignChar = KForeignUnmappedChar ;	
			}
		}
	else
		{
			foreignChar = KForeignUnmappedChar ;				
		}
	return foreignChar ;
	}


/**
This function returns the value of KUnicodeUnmappedChar (may vary for
different character sets..
@return Value of KUnicodeUnmappedChar for this particular implementation
@internalComponent
*/		
TUint16 getUnicodeUnmappedCharacter(void) 
	{
	return KUnicodeUnmappedChar ;
	}


/**
This function returns the value of KForeignUnmappedChar (may vary for
different character sets..
@return Value of KForeignUnmappedChar for this particular implementation
@internalComponent
*/	
TUint16 getForeignUnmappedCharacter(void) 
	{
	return KForeignUnmappedChar ;
	}
