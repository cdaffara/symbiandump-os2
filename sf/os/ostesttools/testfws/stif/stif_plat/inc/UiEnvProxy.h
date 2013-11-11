/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains the header file of the UI Proxy 
* interface.
*
*/

#ifndef UIENVPROXY_H_
#define UIENVPROXY_H_

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// Interface class for the proxy betweene testscripter and AppUi.
class CUiEnvProxy: public CBase
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
	    // None
	
	public: // New functions
		/**
		 * Parses key code.
		 */
		virtual TInt ParseKeyCode( const TDesC& aKeyCodeName, TUint& aKeyCode ) const = 0;
		
		/**
		 * Parses key scan code.
		 */
		virtual TInt ParseKeyScanCode( const TDesC& aKeyScanCodeName, TInt& aKeyScanCode ) const = 0;
		
		/**
		 * Parses key modifier.
		 */
		virtual TInt ParseModifier( const TDesC& aModifierName, TUint& aModifier ) const = 0;
	
		/**
		 * Parse pointer event type
		 */
		virtual TInt ParsePointerEventType( const TDesC& aPointerEventTypeName, TUint& aPointerEventType ) const = 0;
	
		/**
		 * Brings UI control container to foreground.
		 */
		virtual void BringToForeground() = 0;
		
		/**
		 * Sends UI control container to background.
		 */
		virtual void SendToBackground() = 0;
		
		/**
		 * Sends local key event to UI control.
		 */
		virtual void PressKeyL( TRequestStatus* aStatus, TUint aKeyCode, TInt aKeyScanCode = 0, 
							   TUint aKeyModifiers = 0, TInt keyRepeats = 0 ) = 0;
		
		/**
		 * Sends local key event to UI control.
		 */
		virtual void PressKeyL( TUint aKeyCode, TInt aKeyScanCode = 0, 
							   TUint aKeyModifiers = 0, TInt keyRepeats = 0 ) = 0;

		/**
		 * Sends text to UI control.
		 */
		virtual void TypeTextL( TRequestStatus* aStatus, const TDesC& aText ) = 0;

		/**
		 * Sends text to UI control.
		 */
		virtual void TypeTextL( const TDesC& aText ) = 0;		
		
		/**
		 * Send pointer event
		 */
		virtual void SendPointerEventL( TUint aType, const TPoint& aPosition ) = 0;

		/**
		 * Send local pointer event
		 */
		virtual void SendPointerEventL( TRequestStatus* aStatus, TUint aType, const TPoint& aPosition ) = 0;

    public: // Functions from base classes
        // None

    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	// None
	
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
        // None
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};

#endif // UIENVPROXY_H_

// End of File
