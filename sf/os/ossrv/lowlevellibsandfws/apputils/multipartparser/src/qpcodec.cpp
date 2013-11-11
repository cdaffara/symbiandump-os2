// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDE FILES
// 
//

#include <e32std.h>
#include <bafl/qpcodec.h>

/**
Decode the string
@param aSrcString source string
@param rDestString Destination string
@return  KErrNone  if decode is complete
@return  KErrCorrupt if string is not QuotedPrintable compliant
*/
EXPORT_C TInt QuotedPrintableCodec::Decode( const TDesC8& aSrcString, TDes8& rDestString )
	{
	
#ifdef _DEBUG
	TInt KPanicInvalidSMTPLine = 6;
	_LIT(KDllName,"MultipartParser");
#endif
	
	const TUint8 KImcvSP			= ' ';
	const TUint8 KImcvCR			= '\r';
	const TUint8 KImcvLF			= '\n';	
	const TUint8 KImcvTab			= '\t';
	const TUint8 KImcvEquals		= '=';
	TUint8 qpCharacter = KImcvEquals;
	
	TInt error = KErrNone;
	
	__ASSERT_DEBUG(aSrcString.Length(), User::Panic( KDllName ,KPanicInvalidSMTPLine));

	rDestString = KNullDesC8;

	TPtrC8 source( aSrcString.Ptr(), aSrcString.Length() );
	const TUint8* pSource = source.Ptr();
	const TUint8* pEnd = pSource+aSrcString.Length();
	
	// find out if this is a blank line, if so then we'll add a paragraph delimiter instead
	// assume it's blank and then look for non-blank characters
	// avoid the CRLF at the end of the line (we know it's there thanks to the assertion above)

	TBool blankLine = ETrue; 
	while (pSource < pEnd-2) 
		{
		if (*pSource!=KImcvSP && *pSource!=KImcvTab)
			{
			blankLine = EFalse;
			break;
			}
		pSource++;
		}

	if ( blankLine )
		{
		rDestString.Copy( aSrcString );
		return KErrNone;
		}

	TInt outputLength=0;
	TUint8 loBits;
	TUint8 hiBits;
	TUint8 asciiValue;
	pSource = source.Ptr();	// reset to start of source data
	const TUint8 zero = '0';
	const TUint8 alphaAdjust = 55;  // 'A' is ascii 65 so we need to subtract 55 from 
									// alphabetical hex digits to get their numeric value
	while( pSource < pEnd )
		{
		if (*pSource != qpCharacter )
			{
			//  Quoted character or Attachment bound, just bung it on & move to the next one
			// *ptr++ = *pSource;
			outputLength++;
			rDestString.Append( *pSource );
			}
		else	// check for encoded character
			{
			// start looking at the next two characters, if they are there.

			if ( pSource+2 < pEnd )
				{
				pSource++;

				// check for '=' at EOL => this is a soft break, so remove it
				if (*pSource != KImcvCR) 
					{
					if(*pSource != KImcvLF) 
						{
					 	 // now decode hex value into ASCII code : hi-order bits come first
						 hiBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						 pSource++;
						 loBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						 asciiValue = (TUint8)( (hiBits<<4) + loBits);
						 // bung the character thus formed onto the decoded string
						 rDestString.Append( asciiValue );
						 // *ptr++ = asciiValue;
						 outputLength++;
						}
					}
				else
					{
					pSource++;
					if(*pSource != KImcvLF)
						{
						error=KErrCorrupt;
						pSource-=2;
						rDestString.Append( *pSource );
						pSource++;
						rDestString.Append( *pSource );
						pSource++;
						outputLength+=2;
						}
					}
				}
			else
				{
				// copy the rest of the data & use up the input string in the process.

				while (pSource < pEnd)
					{
					error=KErrCorrupt; // not QP compliant
					//*ptr++ = *pSource++;
					outputLength++;
					rDestString.Append( *pSource );
					pSource++;
					}
				}
			} // check for '=' char
		
		pSource++;  // next source charactery
	} // while

	rDestString.SetLength(outputLength);

	return error;
	}

/**
Check for digit
@param aChar charcter to be check
@return  ETrue if passed charcter is digit
@return  EFalse if passed charcter is not digit
*/	
TBool QuotedPrintableCodec::IsDigit( TChar aChar )
{
	return ( (aChar >= '0') && (aChar <= '9') );
}
