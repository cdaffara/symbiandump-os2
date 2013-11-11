/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "apiteststeps.h"
#include "cleanuputils.h"

	
CPkixCertAddSupportedOidsStep::CPkixCertAddSupportedOidsStep()
	{
	SetTestStepName(KAddSupportedOidsTestStep);
	}

//1. make a copy(A) of the supported oids at the start
//2. add in the oids from the config file(B), make a copy of these and add to A
//3. get the supported oids now(C)
//4. iterate over C, for each oid find copies in A and remove them.  if it is not found we error
//5. if A is non empty we error
void CPkixCertAddSupportedOidsStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	//1. make a copy(A) of the supported oids at the start
	
	const RPointerArray<TDesC>& suppOids = iCertChain->SupportedCriticalExtensions();	
	//CleanupClosePushL(suppOids);	//don't need to do this as a ref and done in stepbase destructor
	RPointerArray<HBufC> startSuppOids;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(startSuppOids);
		
	HBufC* des;
	TInt i;
	for (i=0; i < suppOids.Count(); ++i)
		{
		des = (suppOids[i])->AllocLC();
		startSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}
	
	
	
	//2. add in the oids from the config file(B)
	iCertChain->AddSupportedCriticalExtensionsL(iProcessedOids);
	
	//make a copy of these and add to A
	for (i=0; i < iProcessedOids.Count(); ++i)
		{
		des = (iProcessedOids[i])->AllocLC();
		startSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}
	
	//3. get the supported oids now(C)
	const RPointerArray<TDesC>& suppOids2 = iCertChain->SupportedCriticalExtensions();
	
	
	//4. iterate over C, for each oid find copies in A and remove them.  if it is not found we error
	TBool found;
	for (i=0; i < suppOids2.Count(); ++i)
		{
		found = EFalse;
		for (TInt j=0; j < startSuppOids.Count(); ++j)
			{
			if ((*startSuppOids[j]) == (*suppOids2[i]))
				{
				found = ETrue;
				delete (startSuppOids[j]);
				startSuppOids.Remove(j);
				--j;
				}
			}	//startSuppOids loop
		if (!found)	
			{
			ERR_PRINTF2(_L("Extra ERROR: OID in result: %S"), suppOids2[i]);
			User::Leave(KErrGeneral);
			}			
		}	//suppOids2 loop
	
	
	//5. if A is non empty we error
	if (startSuppOids.Count() != 0)
		{
		for (TInt j=0; j < startSuppOids.Count(); ++j)
			{	
			ERR_PRINTF2(_L("ERROR: OID missing from result: %S"), startSuppOids[j]);
			}
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(2, iCertChain);
	}


///////////////////////////////////////////////////////////////////



CPkixCertRemoveSupportedOidsStep::CPkixCertRemoveSupportedOidsStep()
	{
	SetTestStepName(KRemoveSupportedOidsTestStep);
	}


//1. make a copy(A) of the supported oids at the start
//2. remove the oids read from the config file(B)
//3. make a copy(C) of the supported oids now
//4. add B to C
//5. iterate over A, for each oid find and remove first occurence only in C.  if not found we error
//6. iterate over B removing each from C
//7. if C is non empty we error	
void CPkixCertRemoveSupportedOidsStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	//1. make a copy(A) of the supported oids at the start
	
	
	
	const RPointerArray<TDesC>& suppOids = iCertChain->SupportedCriticalExtensions();	
	//CleanupClosePushL(suppOids);	//don't need to do this as a ref and done in stepbase destructor
	RPointerArray<HBufC> startSuppOids;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(startSuppOids);
		
	HBufC* des;
	TInt i;
	for (i=0; i < suppOids.Count(); ++i)
		{
		des = (suppOids[i])->AllocLC();
		startSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}

	
	//2. remove the oids read from the config file(B)
	iCertChain->RemoveSupportedCriticalExtensions(iProcessedOids);
	
	//3. make a copy(C) of the supported oids now
	const RPointerArray<TDesC>& suppOids2 = iCertChain->SupportedCriticalExtensions();
	
	RPointerArray<HBufC> endSuppOids;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(endSuppOids);
		
	for (i=0; i < suppOids2.Count(); ++i)
		{
		des = (suppOids2[i])->AllocLC();
		endSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}


	////////////
	//4. add B to C
	for (i=0; i < iProcessedOids.Count(); ++i)
		{
		des = (iProcessedOids[i])->AllocLC();
		endSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}
		
	
	///////////
	//5. iterate over A, for each oid find and remove first occurence only in C.  if not found we error
	TBool found;
	for (i=0; i < startSuppOids.Count(); ++i)
		{
		found = EFalse;
		for (TInt j=0; j < endSuppOids.Count(); ++j)
			{
			if ((*endSuppOids[j]) == (*startSuppOids[i]))
				{
				found = ETrue;
				delete (endSuppOids[j]);
				endSuppOids.Remove(j);
				break;
				}
			}
		if (!found)
			{
			ERR_PRINTF2(_L("ERROR: Extra OID removed from result: %S"), startSuppOids[i]);
			User::Leave(KErrGeneral);
			}
		}
		
	//6. iterate over B removing each from C
	for (i=0; i < iProcessedOids.Count(); ++i)
		{
		for (TInt j=0; j < endSuppOids.Count(); ++j)
			{
			if ((*endSuppOids[j]) == (*iProcessedOids[i]))
				{
				delete (endSuppOids[j]);
				endSuppOids.Remove(j);
				break;
				}
			}
		}
	
	//7. if C is non empty we error		
	if (endSuppOids.Count() != 0)
		{	
		for (TInt j=0; j < endSuppOids.Count(); ++j)
			{	
			ERR_PRINTF2(_L("ERROR: Extra/duplicate OID found in result: %S"), endSuppOids[j]);
			}
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(3, iCertChain);
	}

/////////////////////////////////////////////////////////////////////

CPkixCertSetSupportedOidsStep::CPkixCertSetSupportedOidsStep()
	{
	SetTestStepName(KSetSupportedOidsTestStep);
	}


//1. set the supported oids to the ones from the config(A)
//2. make a copy(B) of supported oids
//3. iterate over B, remove every match found in copy(C) taken from A.  if not found in C we error
//4. if C not empty we error
void CPkixCertSetSupportedOidsStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	//1. set the supported oids to the ones from the config(A)
	iCertChain->SetSupportedCriticalExtensionsL(iProcessedOids);
	
	//2. make a copy(B) of supported oids
	const RPointerArray<TDesC>& suppOids = iCertChain->SupportedCriticalExtensions();
	
	RPointerArray<HBufC> endSuppOids;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(endSuppOids);
	
	HBufC* des;
	TInt i;	
	for (i=0; i < suppOids.Count(); ++i)
		{
		des = (suppOids[i])->AllocLC();
		endSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}
	
	//3. iterate over B, remove every match found in copy(C) taken from A.  if not found in C we error
	
	RPointerArray<TDesC> oidsCopy;
	CleanupClosePushL(oidsCopy);
	
	for (i=0; i < iOids.Count(); ++i)
		{
		oidsCopy.AppendL(&(iOids[i]));
		}
	
	TBool found;
	for (i=0; i < endSuppOids.Count(); ++i)
		{
		found = EFalse;
		for (TInt j=(oidsCopy.Count() - 1); j >= 0; --j)
			{
			if ((*oidsCopy[j]) == (*endSuppOids[i]))
				{
				found = ETrue;
				oidsCopy.Remove(j);
				}
			}
		if (!found)	
			{
			ERR_PRINTF2(_L("ERROR: Extra OID found in result: %S"), endSuppOids[i]);
			User::Leave(KErrGeneral);
			}
		}
	
	//4. if C not empty we error	
	if (oidsCopy.Count() != 0)
		{
		for (TInt j=0; j < oidsCopy.Count(); ++j)
			{	
			ERR_PRINTF2(_L("ERROR: OID missing from result: %S"), oidsCopy[j]);
			}		
		User::Leave(KErrGeneral);
		}	
	
	CleanupStack::PopAndDestroy(3, iCertChain);
	}
	
	
////////////////////////////////////////////////

CPkixCertResetSupportedOidsStep::CPkixCertResetSupportedOidsStep()
	{
	SetTestStepName(KResetSupportedOidsTestStep);
	}

//1. make a copy(A) of supported oids
//2. set supported oids to list from config
//3. reset supported oids
//4. get supported oids(B)
//5. iterate over B, find and remove first match in A.  if not found we error
//6. if A is not empty we error
void CPkixCertResetSupportedOidsStep::PerformTestL()
	{
	CPkixCertStepBase::PerformTestL();
	
	//1. make a copy(A) of supported oids
	const RPointerArray<TDesC>& suppOids = iCertChain->SupportedCriticalExtensions();	
	//CleanupClosePushL(suppOids);	//don't need to do this as a ref and done in stepbase destructor
	RPointerArray<HBufC> startSuppOids;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(startSuppOids);
		
	HBufC* des;
	TInt i;
	for (i=0; i < suppOids.Count(); ++i)
		{
		des = (suppOids[i])->AllocLC();
		startSuppOids.AppendL(des);
		CleanupStack::Pop(des);
		}

	//2. set supported oids to list from config
	iCertChain->SetSupportedCriticalExtensionsL(iProcessedOids);
	//3. reset supported oids
	iCertChain->ResetSupportedCriticalExtsToDefaultL();
	//4. get supported oids(B)
	const RPointerArray<TDesC>& suppOids2 = iCertChain->SupportedCriticalExtensions();
	
	//5. iterate over B, find and remove first match in A.  if not found we error
	TBool found;
	for (i=0; i < suppOids2.Count(); ++i)
		{
		found = EFalse;
		for (TInt j=0; j < startSuppOids.Count(); ++j)
			{
			if ((*suppOids2[i]) == (*startSuppOids[j]))
				{
				found = ETrue;
				delete (startSuppOids[j]);
				startSuppOids.Remove(j);
				break;
				}
			}
		if (!found)	
			{
			ERR_PRINTF2(_L("ERROR: Extra OID found in result: %S"), suppOids2[i]);
			User::Leave(KErrGeneral);
			}
		}
		
	//6. if A is not empty we error
	if (startSuppOids.Count() != 0)
		{
		for (TInt j=0; j < startSuppOids.Count(); ++j)
			{	
			ERR_PRINTF2(_L("ERROR: OID missing from result: %S"), startSuppOids[j]);
			}
		User::Leave(KErrGeneral);
		}		
	
	CleanupStack::PopAndDestroy(2, iCertChain);	
	}
	
	
