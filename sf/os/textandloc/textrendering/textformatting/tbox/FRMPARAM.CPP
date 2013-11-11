/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* FORM global parameters; held in thread-local storage.
*
*/


#include "FRMPARAM.H"
#include <e32base.h>

/** Sets the thread-local storage to aParam.
@param aParam
	Pointer to the MFormParam implementation, or null, to cancel using system
	colours.
*/
EXPORT_C void MFormParam::Set(const MFormParam* aParam)
	{
	Dll::SetTls((void*)aParam);
	};

/** Gets the thread-local storage value.
@return Pointer to the MFormParam implementation. */
EXPORT_C const MFormParam* MFormParam::Get()
	{
	return (const MFormParam*)Dll::Tls();
	}

EXPORT_C void MFormParam::Reserved()
	{
	}
