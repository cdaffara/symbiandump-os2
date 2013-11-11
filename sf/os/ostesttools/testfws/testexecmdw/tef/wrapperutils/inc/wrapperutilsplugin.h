/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _CWrapperUtilsPlugin__
#define _CWrapperUtilsPlugin__

#include <f32file.h>

class CWrapperUtilsPlugin : public CBase
/**
@internalComponent
@test
*/
	{
public:
	virtual void WaitForSystemStartL();
	virtual TDriveNumber GetSystemDrive();
	};  

#endif

