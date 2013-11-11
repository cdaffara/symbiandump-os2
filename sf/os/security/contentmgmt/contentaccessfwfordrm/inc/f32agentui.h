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
* This defines an interface used by the F32 Agent to display information on
* the screen. The default implementation does nothing. Licensees wishing to allow the F32
* agent to display information on the screen should replace this DLL with
* a DLL that does display information on screen.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __F32AGENTUI_H__
#define __F32AGENTUI_H__

#include <e32base.h>

class RFile;

namespace ContentAccess
	{
	/** Interface that must be implemented in order for the F32 agent to display
	information on the screen
	*/
	class CF32AgentUi : public CBase
		{
	public:
		/** The implementation of this API will display a dialog asking the user to confirm deletion
		of a file. The implementation will display the dialog if the appropriate setting,
		TAgentProperty::EAgentPropertyAgentUI, has been set using CF32AgentUi::SetProperty, enabling 
		the Agent UI. If the user selects to delete the file ETrue will be returned, EFalse if the 
		file is not to be deleted. If the Agent UI is not enabled a default value of ETrue will be 
		returned.
						
		The function should only return once the dialog has completed

		@param aFileName The name of the file that is about to be deleted
		@return Whether to delete the file
		@return ETrue If the file is to be deleted 
		@return EFalse If the file is not to be deleted
		*/
		virtual TBool ConfirmDeleteL(const TDesC& aFileName) = 0;	

	
		/** Display information describing the supplied file
	
		The file will already be open before calling this function.
		The function should only return once the dialog has completed
	
		@param aInfo The information to display, from ContentAccess::TDisplayInfo
		@param aFile The file to display
		@return KErrNone or one of the system wide error codes
		*/
		virtual void DisplayInfoL(TInt aInfo, RFile& aFile) = 0;	
	
		/** The implementation of this API will action/remember Agent properties applicable to the Agent UI.
		
		Currently the F32 Agent only supports one UI property - TAgentProperty::EAgentPropertyAgentUI, 
		enabling or disabling the Agent UI. The implementation of this function should store the property 
		that has been set and subsequent requests to the interface should be dealt with in accordance
		with these properties.
		
		@see CF32AgentUi::ConfirmDeleteL		
   	
   		The implementation will return KErrNone if the property has been set correctly or KErrCANotSupported 
   		if the property supplied is not supported.		
				
		@param aProperty The property to set, from ContentAccess::TAgentProperty
		@param aValue The value of the property
		@return Whether the property was set
		@return KErrNone if the property was set correctly
		@return KErrCANotSupported if the property is not supported
		*/
		virtual TInt SetProperty(TInt aProperty, TInt aValue) = 0;
		};

	/** Factory function that creates the F32 Agent UI 
	@return a CF32AgentUi object 
	*/
	class TF32AgentUiFactory
		{
	public:
		IMPORT_C static CF32AgentUi* CreateF32AgentUiL();
		};
	}



#endif // __F32AGENTUI_H__
