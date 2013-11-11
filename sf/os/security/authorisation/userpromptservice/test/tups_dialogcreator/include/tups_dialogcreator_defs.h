/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file  
 @test
*/

#ifndef TUPSDIALOGCREATORDEFS_H
#define TUPSDIALOGCREATORDEFS_H

namespace UserPromptService
	{
	
	#ifdef DIALOG_01
	
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D11;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 1;
	
	#endif
	
	
	#ifdef DIALOG_02
	
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D21;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 2;
	
	#endif
	
	
	#ifdef DIALOG_03
	
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D31;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 3;
	
	#endif
	
	
	#ifdef DIALOG_SESSION
		
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D41;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 4;	
		
	#endif
	
	
	#ifdef DIALOG_ALWAYS	
		
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D51;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 5;	
				
	#endif
	
	
	#ifdef DIALOG_NEVER
		
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D61;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 6;	
		
	#endif
	
	#ifdef DIALOG_07
	
	static const TInt32 KTestDialogCreatorImplementationId = 0xE1111D71;
	static const TInt32 KTestNotifierImplementationId = 0xE1111F11;
	static const TInt KDialogCreatorInstance = 7;
	
	#endif
	
	}
#endif // TUPSDIALOGCREATORDEFS_H