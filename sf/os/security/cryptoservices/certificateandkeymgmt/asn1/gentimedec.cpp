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
* This file contains the implementation of the General Time ASN1 class.
* Fully support three forms of ASN.1 generalizedTime
* Fraction of second accepts 1,2 or 3 digits input. This is to maximize the tolerance of client input 
*
*/


#include <asn1dec.h>

EXPORT_C TASN1DecGeneralizedTime::TASN1DecGeneralizedTime()
	{
	}

TTime TASN1DecGeneralizedTime::GetTimeL(const TDesC8& aSource)



	{
	// __OFFSET__ Extract corrected time to include offset too.

	// Did this checking ought to be done on creation rather than when we attempt to get the result?

	// I guess we ought to check that the contents we've got are long enough to contain a time!
	if (aSource.Length()<13)
		{
		User::Leave(KErrArgument);
		}

	// Check all digits the main bit of time are valid, YYYY:MM:DD HH:MM
	TInt i;
	for (i=0;i<12;i++)
		{
		TUint8 j;
		j=(TUint8)(aSource[i]-'0');
		if (j>=10)
			{
			User::Leave(KErrArgument);
			}
		}
	//check seconds are valid if present
	i = 12;
	if(aSource.Length() > 13)
		{
			if ((aSource[i] < '0') || (aSource[i] > '9')
					|| (aSource[i+1] < '0') || (aSource[i+1] > '9'))
				User::Leave(KErrArgument);
		}
	
	i = 14;
	//if the time string is longer 
	if(aSource.Length() > 14)
		{
		TInt countDot = 0;
		TInt countOperator = 0;
		TInt countZ = 0;
		
		//check if the extra bytes are valid
		while(i < aSource.Length())
			{
				switch(aSource[i])
				{
				case '.': //'.' can't appear more than once and at least one digit should follow
					countDot++;
					if(countDot > 1)
						User::Leave(KErrArgument);
					
					if(aSource.Length()-1 ==  i) //error, '.' is last char in time string
						User::Leave(KErrArgument);
					else
						{
							if(aSource[i+1] - '0' < 0 || aSource[i+1] - '0' > 9)
								User::Leave(KErrArgument);
						}
					break;
				
				case '+':
				case '-': // '+' and '-' can't appear more than once and at least four digits should follow
						  // After that, we reach either EOF or 'Z', no more char could follow
					countOperator++;
					if(countOperator > 1)
						User::Leave(KErrArgument);
					
					if(aSource.Length()-1 < i + 4) //error, not enough digits following operator
						User::Leave(KErrArgument);
					else
						{
						for(TInt j = i+1; j <= i+4; j++)
							if(aSource[j] - '0' < 0 || aSource[j] -'0' > 9)
								User::Leave(KErrArgument);
						
						
						if(aSource.Length() > i + 6)//error, unexpected rubbish
							User::Leave(KErrArgument);
						if(aSource.Length() == i + 6 && aSource[i+5] != 'Z')
							User::Leave(KErrArgument);
						}
					
					
					break;
				case 'Z': //'Z' can't appear more than once and must be the last char in time string
					countZ++;
					if(countZ > 1)
						User::Leave(KErrArgument);
					
					if(aSource.Length()-1 > i) //error, something following 'Z'
						User::Leave(KErrArgument);
					
					
					break;
				default: //so is this a digit?
					if(aSource[i] -'0' < 0 || aSource[i] -'0' > 9)
						User::Leave(KErrArgument);
				}
				i++;
			}
			
		
		}
	// Uh-oh, looks like we're going to have to pull each bit manually and pop it in a TDateTime thing
	TInt Year,Month,Day,Hour,Minute,Second=0;		// Only set seconds to zero 'cos they are optional, everything else is going to be set
	TInt MicroSecond = 0;
	TInt offsetIntHours = 0, offsetIntMinutes = 0;
	TTimeIntervalHours offsetHours = 0;
	TTimeIntervalMinutes offsetMinutes = 0;
	Year=(aSource[0]-'0')*1000+(aSource[1]-'0')*100+(aSource[2]-'0')*10+(aSource[3]-'0');
	Month=(aSource[4]-'0')*10+aSource[5]-'0';
	Month--;										// Because the enum for months starts at 0 not 1
	Day=(aSource[6]-'0')*10+aSource[7]-'0';
	Day--;										// Because the enum for days starts at 0 not 1
	Hour=(aSource[8]-'0')*10+aSource[9]-'0';
	Minute=(aSource[10]-'0')*10+aSource[11]-'0';
	TInt Pos=12;

	
	if (aSource.Length()>13)
		{
			// seconds
			Second = (aSource[Pos++]-'0')*10;
			Second += aSource[Pos++]-'0';
			
		}
	
	// check range for various components
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
	
	//optional 3 bytes fraction of seconds
	//Converts the fraction of seconds to microsecond
	if(aSource.Length() > Pos && aSource[Pos] == '.')
		{
		Pos++;			
		
		TInt multiplier = 100000;
			
		// parse fraction of second
		while(Pos <= 17 && Pos < aSource.Length())
			{
			if(!((aSource[Pos] >= '0') && (aSource[Pos] <= '9')))
				break; //fraction ended
					
			MicroSecond += (aSource[Pos++]-'0') * multiplier;
			multiplier /= 10;
			
			}			
		
		}
	
	if (aSource.Length()>Pos)
		{
		TBool aheadUTC = EFalse;
		switch (aSource[Pos])
			{
		case 'Z':
			// Zulu - nothing more to do
			break;
		case '+':
			//'+' indicates local time is ahead of UTC, need to substract the offset later
			aheadUTC = ETrue;
			// The case fall-through is intentional, both cases have the same functionality, with a different
			// aheadUTC value
		case '-':
			{//brackets are required to prevent armv5 warning and gccxml errors
			
			
			//'-' indicates local time is behind of UTC, need to add the offset later
			//if we are in here with '-', aheadUTC has already been set to the correct value
			
			Pos++;
			
						
			//parse
			TInt multiplier = 10;
			for(TInt i = 0; i <= 3; i++)
				{
					if(i < 2)
						offsetIntHours += (aSource[Pos + i]-'0') * multiplier; 
					else
						offsetIntMinutes += (aSource[Pos + i]-'0') * (TInt)multiplier; 
							
					multiplier = (multiplier == 10)? 1 : 10;
							
									
						
				}
			
			//expected range,
			//offset hour range: 0-23
			//offset minute range: 0-59
			if(offsetIntHours < 0 || offsetIntHours > 23 || 
					offsetIntMinutes < 0 || offsetIntMinutes > 59)
				User::Leave(KErrArgument);
			
			offsetHours = aheadUTC ? -offsetIntHours : offsetIntHours ;
			offsetMinutes =  aheadUTC ? -offsetIntMinutes :  offsetIntMinutes;
			
			break;
			}
		default:
			// Error!
			User::Leave(KErrArgument);
			break;
			}
		}
	
	

	TDateTime D(Year, month,Day,Hour,Minute,Second,MicroSecond);
	
	TTime T(D);
	T = T + offsetHours;
	T = T + offsetMinutes;
	return T;
	}

EXPORT_C TTime TASN1DecGeneralizedTime::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	TTime t = GetTimeL(gen.GetContentDER());
	aPos+=gen.LengthDER();
	return t;
	}

EXPORT_C TTime TASN1DecGeneralizedTime::DecodeDERL(const TASN1DecGeneric& aGen)

	{
	return GetTimeL(aGen.GetContentDER());
	}
