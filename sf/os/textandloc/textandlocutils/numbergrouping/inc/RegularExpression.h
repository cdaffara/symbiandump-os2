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



#ifndef C_REGULAR_EXPRESSION_H
#define C_REGULAR_EXPRESSION_H

#include "StateMachine.h"

NONSHARABLE_CLASS(CRegularExpression) : public CBase
{
public:
	static CRegularExpression* NewL(RPointerArray<TDesC>* aPatterns);
	static CRegularExpression* NewLC(RPointerArray<TDesC>* aPatterns);
	virtual ~CRegularExpression();

	TInt	Search(const TDesC& aString);
	TInt	SearchFrom(TInt aIndex, const TDesC& aString);

    /**
    * This routine writes into a client supplied descriptor a string that is a wildcarded version of
    * one of the pattern matches in the engine. The pattern(statemachine) at index aPattenIndex is used.
    *
    * It writes characters into the supplied descriptor to a length of the maximum number of 
    * characters to match. 
    *
    * For each character index, if there is only one possible valid character, it puts in that 
    * character.  If there are more than one, then the supplied aWildcard is written in.
    * 
    * @param    aPattenIndex        The index of the pattern in the engine to use.
    *                               for the N patterns supplied at construction.
    * @param    aWildcardChar       The character to put in the example pattern if there is no single 
    *                               valid character at that point
    * @param    aWildcardedPattern     Descriptor to write the wildcarded matching pattern into
    */
    void    GetWildcardVersionOfPattern( TInt aPatternIndex, TText aWildcard, TDes& aWildcardedPattern) const;

protected:
	CRegularExpression();
	
	void			ConstructL(RPointerArray<TDesC>* aPatterns);
	virtual void	GenerateStateTablesL(RPointerArray<TDesC>* aPatterns);
	
	RPointerArray<CStateMachine>	iStateMachines;
};

#endif // C_REGULAR_EXPRESSION_H

// End of File
