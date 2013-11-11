/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "wmdrmcontentparser.h"
#include <caf/caferr.h>

using namespace ReferenceTestAgent;
using namespace ContentAccess;

EXPORT_C CWmdrmContentParser* CWmdrmContentParser::NewL(const TDesC8& aHeaderData)
	{
	CWmdrmContentParser* self = new (ELeave) CWmdrmContentParser;
	CleanupStack::PushL(self);
	self->ConstructL(aHeaderData);
	CleanupStack::Pop(self);
	return self;
	}


CWmdrmContentParser::CWmdrmContentParser()
	{
	}


void CWmdrmContentParser::ConstructL(const TDesC8& aHeaderData)
	{
	iHeaderData = aHeaderData.AllocL();	
	
	// Initialise default attributes.
	CreateAttributeSetL();
	CreateStringAttributeSetL();
	}
	
void CWmdrmContentParser::CreateAttributeSetL()
	{
	// Initialise attributes
	SetAttributeL(EIsProtected, ETrue, KErrNone);
	SetAttributeL(EIsForwardable, EFalse, KErrNone);
	SetAttributeL(EIsModifyable, EFalse, KErrNone);
	SetAttributeL(EIsCopyable, EFalse, KErrNone);
	SetAttributeL(ECanPlay, EFalse, KErrNone);
	SetAttributeL(ECanPrint, EFalse, KErrNone);
	SetAttributeL(ECanExecute, EFalse, KErrNone);
	SetAttributeL(ECanView, EFalse, KErrNone);
	SetAttributeL(ERightsNone, ETrue, KErrNone);
	SetAttributeL(ERightsPending, EFalse, KErrNone);
	SetAttributeL(ERightsHaveExpired, EFalse, KErrNone);
	SetAttributeL(EPreviewAvailable, EFalse, KErrNone);
	SetAttributeL(ECanRewind, EFalse, KErrNone);
	SetAttributeL(ECopyPaste, EFalse, KErrNone);
	SetAttributeL(ERightsConsumable, EFalse, KErrNone);
	SetAttributeL(ERightsStateless, EFalse, KErrNone);
	SetAttributeL(ECanMove, ETrue, KErrNone);
	SetAttributeL(ECanRename, ETrue, KErrNone);
	SetAttributeL(ECanAutomaticConsume, EFalse, KErrNone);
	}
	
void CWmdrmContentParser::CreateStringAttributeSetL()
	{
	// Initialise string attributes.
	_LIT(KDescription, "WMDRM");
	SetStringAttributeL(EDescription , KDescription, KErrNone);
	
	_LIT(KMimeType, "application/vnd.drm.asf");

	SetStringAttributeL(EMimeType, KMimeType, KErrNone);
	}

void CWmdrmContentParser::SetAttributeL(TInt aAttribute, TInt aValue, TInt aErrorCode)	
	{
	iAttributeSet.AddL(aAttribute);
	User::LeaveIfError(iAttributeSet.SetValue(aAttribute, aValue, aErrorCode));
	}
	
void CWmdrmContentParser::SetStringAttributeL(TInt aAttribute, const TDesC& aValue, TInt aErrorCode)	
	{
	iStringAttributeSet.AddL(aAttribute);
	User::LeaveIfError(iStringAttributeSet.SetValue(aAttribute, aValue, aErrorCode));
	}
	
CWmdrmContentParser::~CWmdrmContentParser()
	{
	delete iHeaderData;
	
	iAttributeSet.Close();
	iStringAttributeSet.Close();
	}

EXPORT_C TInt CWmdrmContentParser::GetAttribute(TInt aAttribute, TInt& aValue) const
	{
	return iAttributeSet.GetValue(aAttribute, aValue);
	}
	
EXPORT_C TInt CWmdrmContentParser::GetStringAttribute(TInt aAttribute, TDes& aValue) const
	{
	return iStringAttributeSet.GetValue(aAttribute, aValue);
	}
	
EXPORT_C TInt CWmdrmContentParser::GetAttributeSet(RAttributeSet &aAttributeSet) const
	{
	TInt value = 0;
	TInt err = KErrNone;
	for(TInt i = 0; i < aAttributeSet.Count(); i++)
		{
		TInt attribute = aAttributeSet[i];
		err = GetAttribute(attribute, value);
		if(err != KErrNone)
			{
			break;
			}
	
		aAttributeSet.SetValue(attribute, value, err);
		}
	return err;
	}

EXPORT_C void CWmdrmContentParser::GetStringAttributeSetL(RStringAttributeSet &aStringAttributeSet) const
	{
	for(TInt i = 0; i < aStringAttributeSet.Count(); i++)
		{
		TInt attribute = aStringAttributeSet[i];
		HBufC* value = HBufC::NewLC(iStringAttributeSet.GetValueLength(attribute));
		TPtr valuePtr = value->Des();
		User::LeaveIfError(GetStringAttribute(attribute, valuePtr));
		aStringAttributeSet.SetValue(attribute, valuePtr, KErrNone);
		CleanupStack::PopAndDestroy(value);
		}
	}
