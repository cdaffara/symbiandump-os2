/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* FSDialog class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
class MSecurityDialog;

class FSDialog
	{
 public:
	inline static MSecurityDialog* Dialog();
	static void InitialiseL();
	static void Cleanup();

 private:
	static MSecurityDialog* iDialog;
	};

inline MSecurityDialog* FSDialog::Dialog()
	{
	return iDialog;
	}
