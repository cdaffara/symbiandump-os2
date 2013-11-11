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
*
*/



#ifndef C_STATE_MACHINE_H
#define C_STATE_MACHINE_H

#include "NumberGroupingStd.h"

NONSHARABLE_CLASS(CStateMachine) : public CBase
{
public:
	static CStateMachine*	NewL(TInt aMaxNumberChars, TInt aMaxNumberStates);
	static CStateMachine*	NewLC(TInt aMaxNumberChars, TInt aMaxNumberStates);

	virtual ~CStateMachine();

	void			AddStateTransistionL(TChar aChar, TInt aState, TInt aNextState);
	void			AddStateTransistionL(TInt aDigit, TInt aState, TInt aNextState);

	TBool			Matches(const TDesC& aString);
    
    /**
    * This routine writes into a client supplied descriptor a string that is a wildcard version
    * of the pattern match for this state machine.  It writes characters into the supplied descriptor
    * to a length of the maximum number of characters to match. 
    * For each character index, if there is only one possible valid character, it puts in that 
    * character.  If there are more than one, then the supplied aWildcardChar is written in.
    * The character '.' in the state machine is not treated as a single character, but as a range,
    * and so the wildcard character is written in for the '.'.
    * 
    * aWildcardPattern must have MaxLength which is equal or greater than iMaxNumberStates
    * 
    * @param    aWildcardChar   The character to put in aExamplePattern if there is no single 
    *                           valid character at that point
    * @param    aWildcardedPattern Descriptor to write the wildcarded number pattern into
    */
    void            GetWildcardVersionOfPattern( 
        TText aWildcardChar, 
        TDes& aWildcardedPattern ) const;

private:
	CStateMachine(TInt aMaxNumberChars, TInt aMaxNumberStates);

	void			ConstructL();
	virtual TInt	MapIndex(TChar aChar, RArray<TInt>& aIndices);
	
	TInt**			iStateTable;
	TInt			iMaxNumberChars;
	TInt			iMaxNumberStates;
};

#endif // C_STATE_MACHINE_H
// End of File
