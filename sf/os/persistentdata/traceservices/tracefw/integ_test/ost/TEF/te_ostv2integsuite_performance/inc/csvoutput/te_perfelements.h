// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file te_perfelements.h
 @internalTechnology
*/
#if (!defined TE_OUTPUTSANITYELEMENTS_H)
#define TE_OUTPUTSANITYELEMENTS_H

#include <e32def.h>
#include <e32math.h>

static const TInt KNumberOfColumns = 15;

enum TColumnType
	{
	EApiName,
	EApiId,
	EApiSize, 
	ESizeOfUptData, 
//	EUptSchema, 
	EUptContext, 
	EUptPc, 
	ETestClassification, 
	ETestModuleUid, 
	EClassificationFilterMatch, 
	EModuleUidFilterMatch,
	ETraceCompiledIn, 
	EMinimumExecutionTime, 
	ETypicalExecutionTime, 
	EApiStackUsage, 
	EApiHeapUsage
	};


class TElement
	{
public:
	TElement();
	/**
	 * Returns the accepted variation as a percentage
	 */
	TInt	AcceptedVariation(){return iAcceptedVariation[iType];}
	TInt	Type()const {return iType;}
	void	SetType(TInt aType){iType = aType;}
	TInt	Value()const {return iValue;}
	void	SetValue(TInt aValue){iValue = aValue;}
	
private:
	//Thie is the element type i.e. one of the TColumnType
	TInt iType;
	//This is the value of the element
	TInt iValue;
	//This holds the acceptable variation of the element
	TInt iAcceptedVariation[KNumberOfColumns]; 
	};
		
	
class TFailedResults 
	{
public:
	TFailedResults()
	: iLineNumber(0), iElementType(0), iVariation (0), iError(0)
	{};
	TInt iLineNumber;
	TInt iElementType;
	TInt iVariation;
	TInt iError;
	};


#endif //TE_OUTPUTSANITYELEMENTS_H
