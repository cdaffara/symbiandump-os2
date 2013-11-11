/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "RegularExpression.h"

#define KNumOfRecognisedChars 13

// Panic is implemented in NumberGrouping.cpp
GLDEF_C void Panic(TNumberGroupingPanic aPanic);

CRegularExpression::CRegularExpression()
{
}

CRegularExpression* CRegularExpression::NewL(RPointerArray<TDesC>* aPatterns)
{
	CRegularExpression* s = NewLC(aPatterns);
	CleanupStack::Pop();
	return s;
}

CRegularExpression* CRegularExpression::NewLC(RPointerArray<TDesC>* aPatterns)
{
	CRegularExpression* s = new(ELeave)CRegularExpression();
	CleanupStack::PushL(s);
	s->ConstructL(aPatterns);
	return s;
}

void CRegularExpression::ConstructL(RPointerArray<TDesC>* aPatterns)
{
	GenerateStateTablesL(aPatterns);
}

CRegularExpression::~CRegularExpression()
{
	for(TInt i = 0; i < iStateMachines.Count(); i++)
		delete iStateMachines[i];
	
	iStateMachines.Close();
}

TInt CRegularExpression::Search(const TDesC& aString)
{
	return SearchFrom(0, aString);
}

TInt CRegularExpression::SearchFrom(TInt aIndex, const TDesC& aString)
{
	for(TInt i = aIndex; i < iStateMachines.Count(); ++i)
	{
		if(iStateMachines[i]->Matches(aString))
			return i;
	}

	return KErrNotFound;
}

// Do the parsing
void CRegularExpression::GenerateStateTablesL(RPointerArray<TDesC>* aPatterns)
{	
	for(TInt nPatternCount = 0; nPatternCount < aPatterns->Count(); ++nPatternCount)
	{
		const TDesC& desPattern = *((*aPatterns)[nPatternCount]) ;	// I know it looks horrific, but it makes the
																	// rest of the method _so_ much easier to read
		
		CStateMachine* StateMachine = CStateMachine::NewLC(KNumOfRecognisedChars, desPattern.Length());
		User::LeaveIfError(iStateMachines.Append(StateMachine));
		CleanupStack::Pop(); // now the ownership transfers to iStateMachines

		// parse the desPattern

		enum TParseState
		{
			EAny,
			EOpenBracketFound,
			EChoiceOrRange,
			EChoice,
			ERange,
			ELookingForCloseBracket
		};

		TParseState	parseState = EAny;

		TInt		nPatternIndex = 0;
		TInt		nState = 0;

		TBuf<10>	bufChoice;
		bufChoice.Zero();

		TBuf<20>	bufParsingError = _L("Parsing syntax error");

		while(nPatternIndex < desPattern.Length() )
		{
			TChar cChar = desPattern[nPatternIndex];
			TBool bEndOfParse = EFalse;

			if(nPatternIndex == desPattern.Length() - 1) // end of the parse
				bEndOfParse = ETrue;
		
			switch(cChar)
			{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				switch(parseState)
				{
				case EAny:
					iStateMachines[nPatternCount]->AddStateTransistionL(cChar, nState, bEndOfParse?
																					KStateMatched:
																					(nState + 1)); 
					++nState;
					break;
				case EOpenBracketFound:
					bufChoice.Append(cChar);
					parseState = EChoiceOrRange;					
					break;
				case EChoiceOrRange:
				case EChoice:
					bufChoice.Append(cChar);
					parseState = EChoice;
					break;
				case ERange:
					bufChoice.Append(cChar);
					parseState = ELookingForCloseBracket;
					break;
				//case ELookingForCloseBracket:						
				default:
					User::Panic(bufParsingError, KErrSyntaxError);
					break;
				}
				break;
			case '[':
				switch(parseState)
				{
				case EAny:						
					bufChoice.Zero();
					parseState = EOpenBracketFound;
					break;
				//case EOpenBracketFound:			
				//case EChoiceOrRange:			
				//case EChoice:					
				//case ERange:					
				//case ELookingForCloseBracket:	
				default:
					User::Panic(bufParsingError, KErrSyntaxError);
					break;
				
    			}	
				break;
			case ']':
				switch(parseState)
				{
				case EChoice:
					{
						for(TInt i = 0; i < bufChoice.Length(); ++i)
						{
							TChar cChar = bufChoice[i];
							
							iStateMachines[nPatternCount]->AddStateTransistionL(cChar.GetNumericValue(), 
																				nState, 
																				bEndOfParse?
																					KStateMatched:
																					(nState + 1));
						}
						
						++nState;
						parseState = EAny;
					}
					break;
				case ELookingForCloseBracket:
					{
						TChar cTopOfRange = bufChoice[1];
						TChar cBottomOfRange = bufChoice[0];

						TInt nTopOfRange = cTopOfRange.GetNumericValue();		// if they're not there then something
						TInt nBottomOfRange = cBottomOfRange.GetNumericValue();	// is seriously amiss...
						
						if(nTopOfRange < nBottomOfRange)
							User::Panic(bufParsingError, KErrSyntaxError);
					
						for(TInt i = nBottomOfRange; i <= nTopOfRange; ++i)
						{
							iStateMachines[nPatternCount]->AddStateTransistionL(i, nState, bEndOfParse?
																							KStateMatched:
																							(nState + 1));
						}
						++nState;
						parseState = EAny;
					}
					break;
				//case EAny:						
				//case EOpenBracketFound:			
				//case EChoiceOrRange:			
				//case ERange:		
				default:
					User::Panic(bufParsingError, KErrSyntaxError);
					break;
				}	
				break;
			case '-':
				switch(parseState)
				{
				case EChoiceOrRange:			
					parseState = ERange;
					break;	
				//case EAny:						
				//case EOpenBracketFound:			
				//case ELookingForCloseBracket:	
				//case ERange:					
				//case EChoice:
				default:
					User::Panic(bufParsingError, KErrSyntaxError);
					break;
				}
				break;
			case '.':
			case '+':
				switch(parseState)
				{
				case EAny:						
					iStateMachines[nPatternCount]->AddStateTransistionL(cChar, nState, bEndOfParse?
																					KStateMatched:
																					(nState + 1));
					++nState;
					break;
				//case EOpenBracketFound:			
				//case EChoiceOrRange:			
				//case ELookingForCloseBracket:	
				//case ERange:					
				//case EChoice:
				default:
					User::Panic(bufParsingError, KErrSyntaxError);
					break;					
				}
				break;
			default:
				User::Panic(bufParsingError, KErrSyntaxError);
				break;
			}

			++nPatternIndex;
		}

		if(parseState != EAny)
			User::Panic(bufParsingError, KErrSyntaxError);
	}
}

void CRegularExpression::GetWildcardVersionOfPattern( 
    TInt aPatternIndex, TText aWildcard, TDes& aWildcardedPattern) const
    {
    __ASSERT_ALWAYS( (0 <= aPatternIndex) && (aPatternIndex < iStateMachines.Count()),
        Panic(ENumberGroupingNoSuchStateMachine) );
    iStateMachines[aPatternIndex]->GetWildcardVersionOfPattern( aWildcard, aWildcardedPattern );
    }

// End of File
