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
* This file contains the implementation of the UTC time ASN1 object.
* At this moment in time this class will only deal with Zulu times and 
* doesn't have any kind of handling for time offsets. This is not going to be 
* a problem for certificate implementations as the PKIX (RFC2459) profile
* defines that certificate validity periods are specified as Zulu times.
* Points that need updating if handling for offsets are marked with __OFFSET__
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecUTCTime::TASN1DecUTCTime()
	{
	}


TTime TASN1DecUTCTime::GetTimeL(const TDesC8& aSource)

	{
	// __OFFSET__ Extract corrected time to include offset too.

	// Did this checking ought to be done on creation rather than when we attempt to get the result?
	// YES!

	// I guess we ought to check that the contents we've got are long enough to contain a time!
	if (aSource.Length()<11)
		{
		User::Leave(KErrArgument);
		}

	// Check all digits the main bit of time are valid - this doesn't check the seconds or offset
	TInt i;
	for (i=0;i<10;i++)
		{
		TUint8 j;
		j=(TUint8)(aSource[i]-'0');
		if (j>=10)
			{
			User::Leave(KErrArgument);
			}
		}
	// Uh-oh, looks like we're going to have to pull each bit manually and pop it in a TDateTime thing
	TInt Year,Month,Day,Hour,Minute,Second=0;		// Only set seconds to zero 'cos they are optional, everything else is going to be set
	Year=(aSource[0]-'0')*10+aSource[1]-'0';
	Year+=(Year<50)?2000:1900;						//  collection to fit with PKIX UTC/Generalised time rollover at 2049/2050
	Month=(aSource[2]-'0')*10+aSource[3]-'0';
	Month--;										// Because the enum for months starts at 0 not 1
	Day=((aSource[4]-'0')*10+aSource[5]-'0') -1;//added -1 for offset from zero(!)
	Hour=(aSource[6]-'0')*10+aSource[7]-'0';
	Minute=(aSource[8]-'0')*10+aSource[9]-'0';
	TInt Pos=10;

	if (aSource.Length()>11)
		{
		if ((aSource[Pos]>='0')&&(aSource[Pos]<='9'))
			{
			// seconds
			Second=(aSource[Pos++]-'0')*10;
			Second += aSource[Pos++]-'0';
			}
		}
	if (aSource.Length()>Pos)
		{
		switch (aSource[Pos])
			{
		case 'Z':
			// Zulu - nothing more to do
			break;
		case '+':
		case '-':
			// __OFFSET__ Extract corrected time to include offset too.
		    User::Leave(KErrNotSupported);
		    break;
		default:
			// Error!
			User::Leave(KErrArgument);
			break;
			}
		}
	else
		{
		User::Leave(KErrArgument);
		}

	if (Month<EJanuary || Month>EDecember)
		{
		User::Leave(KErrArgument);
		}
	TMonth month = (TMonth)Month;

	if (	(Day<0 || Day>=Time::DaysInMonth(Year,month))	||
			(Hour<0 || Hour>=24)	||
			(Minute<0 || Minute>=60)	||
			(Second<0 || Second>=60)	)
		{
		User::Leave(KErrArgument);
		}

	TDateTime D(Year,month,Day,Hour,Minute,Second,0);
	
	TTime T(D);
	return T;
	}

// __OFFSET__ Add GetOffset() method

EXPORT_C TTime TASN1DecUTCTime::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	TTime t = GetTimeL(gen.GetContentDER());
	aPos+=gen.LengthDER();
	return t;
	}

EXPORT_C TTime TASN1DecUTCTime::DecodeDERL(const TASN1DecGeneric& aGen)

	{
	return GetTimeL(aGen.GetContentDER());
	}
