/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "TXTSTD.H"
#include "TXTINDEX.H"
#include "TXTFRMAT.H"


// Allows CParaAttribs on the CleanupStack to be destroyed correctly when included as a PushL(TCleanupItem).
GLDEF_C void ReleaseOnCleanup(TAny* aParaAttribs)
	{
	((CParaAttribs*)aParaAttribs)->Release();
	}
	
// Allows CParaFormat on the CleanupStack to be destroyed correctly when included as a PushL(TCleanupItem).
void ResetOnCleanup(TAny* aParaFormat)
	{
	reinterpret_cast<CParaFormat*>(aParaFormat)->Strip();
	}
	
GLDEF_C void Panic(TETextPanic aPanic)
	{
	_LIT(KETextPanic,"ETEXT");
	User::Panic(KETextPanic,aPanic);
	}


