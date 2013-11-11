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


#include "StateMachine.h"

// CStateMachine
CStateMachine::CStateMachine(TInt aMaxNumberChars, TInt aMaxNumberStates) : iMaxNumberChars(static_cast<TInt>(aMaxNumberChars)),
																			iMaxNumberStates(static_cast<TInt>(aMaxNumberStates))
{
}

CStateMachine* CStateMachine::NewL(TInt aMaxNumberChars, TInt aMaxNumberStates)
{
	CStateMachine* s = NewLC(aMaxNumberChars, aMaxNumberStates);
	CleanupStack::Pop();
	return s;
}

CStateMachine* CStateMachine::NewLC(TInt aMaxNumberChars, TInt aMaxNumberStates)
{
	CStateMachine* s = new(ELeave)CStateMachine(aMaxNumberChars, aMaxNumberStates);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
}

CStateMachine::~CStateMachine()
{
	for(TInt i = 0; i < iMaxNumberChars; ++i)
		delete iStateTable[i];
	
	delete iStateTable;
}

void CStateMachine::ConstructL()
{
	iStateTable = new TInt*[iMaxNumberChars];	

	for(TInt i = 0; i < iMaxNumberChars; ++i)
	{
		iStateTable[i] = new TInt[iMaxNumberStates];
		for(TInt j = 0; j < iMaxNumberStates; j++)
			iStateTable[i][j] = 0;
	}
}

void CStateMachine::AddStateTransistionL(TChar aChar, TInt aState, TInt aNextState)
{
	RArray<TInt> Dummy;
	TInt CharIndex = MapIndex(aChar, Dummy);
	Dummy.Close();
	
	AddStateTransistionL(CharIndex, aState, aNextState);
}

void CStateMachine::AddStateTransistionL(TInt aIndex, TInt aState, TInt aNextState)
{
    if(aIndex < 0 || aIndex > iMaxNumberChars || aState > iMaxNumberStates || aNextState > iMaxNumberStates)
		User::Leave(KErrGeneral);

	iStateTable[static_cast<TInt>(aIndex)][static_cast<TInt>(aState)] = static_cast<TInt>(aNextState);
}

TBool CStateMachine::Matches(const TDesC& aString)
{
	// Walk the state table and return true if the string matches the pattern.
	
	TInt			nTableState = KStateNoMatch;
	RArray<TInt>	arrIndices;
	TBool			bFound = EFalse;

	for(TInt nStringIndex = 0; nStringIndex < aString.Length(); ++nStringIndex)
	{
		arrIndices.Reset();
		
		TChar	charChar = aString[nStringIndex];
		TInt	nTableIndex = MapIndex(charChar, arrIndices);

		if(nTableIndex == -1) 
			return EFalse;
		
		TInt nCount = arrIndices.Count();
		
		if(nCount)
		{
			for(TInt i = 0; i < nCount; i++)
			{
				nTableIndex = arrIndices[i];
				TInt NewTableState = iStateTable[nTableIndex][nTableState];
				
				if( NewTableState != KStateNoMatch || 
					(NewTableState == KStateNoMatch && i == (arrIndices.Count() - 1)) ||
					NewTableState == KStateMatched)
				{
					nTableState = NewTableState;
					break;
				}
			}
		}
		else
		{
			nTableState = iStateTable[nTableIndex][nTableState];
		}

		if(nTableState == KStateNoMatch)
			break;

		if(nTableState == KStateMatched)
		{
			bFound = ETrue;
			break;
		}
	}

	arrIndices.Close();
	
	return bFound;
}

TInt CStateMachine::MapIndex(TChar aChar, RArray<TInt>& aIndices)
{
    // Check the type of aChar and return the relevant index or indicies
	if(aChar.IsDigit())
	{
		TInt nIndex = static_cast<TInt>(aChar.GetNumericValue());
		
		TInt ret = KErrNone;
		ret |= aIndices.Append(nIndex);
		ret |= aIndices.Append(KCharacterDot);
		__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
		if (KErrNone != ret) return ret;

		return nIndex;
	}

	if(aChar == '+')	return KCharacterPlus;
	if(aChar == '.')	return KCharacterDot;

    return -1;  // TO DO : define 
}

void CStateMachine::GetWildcardVersionOfPattern( 
    TText aWildcardChar, 
    TDes& aWildcardedPattern ) const
    {
    aWildcardedPattern.SetLength(0);
    TInt maxLength = aWildcardedPattern.MaxLength();
    // There is a column in the StateTable for each character in the regular expression. The first character
    // of the regexp in column [0], last in column [Length()-1]
    // The non-zero values found within a column of the StateTable represent the characters
    // that are valid at that position in a candidate match.
    // An example pattern is calculated by examining the StateTable.
    // For each column, count the number of matching ( i.e. not KStateNoMatch) states
    // If only one, then place that character in the example pattern.
    // If more than one, put the wildcard in.
    for(TInt stateIndex = 0; stateIndex < iMaxNumberStates && stateIndex < maxLength; stateIndex++)
        {
        TInt matches = 0;
        TInt matchedIndex = -1;
        for (TInt charIndex = 0; charIndex < iMaxNumberChars; charIndex++ )
            {
            TInt nextState = iStateTable[charIndex][stateIndex];

            if ( nextState != KStateNoMatch )
                {
                matches++;
                matchedIndex = charIndex;
                if (matches > 1 )
                    break;
                }
            }
        if ( matches == 0 ) // Have found an empty column.  Unused part of state machine. Stop filling
            {
            break;
            }
        else if ( matches > 1 )
            aWildcardedPattern.Append(aWildcardChar);
        else
            {
            if ( 0 <= matchedIndex && matchedIndex <= 9 ) // Must be a numeric digit
                {
                aWildcardedPattern.Append((TText)(matchedIndex+'0'));
                }
            else if ( matchedIndex == KCharacterDot )
                {
                aWildcardedPattern.Append(aWildcardChar);
                }
            else if ( matchedIndex == KCharacterPlus )
                {
                aWildcardedPattern.Append('+');
                }
            else
                {
                aWildcardedPattern.Append(aWildcardChar);
                }
            }
        }

    }
// End of File

