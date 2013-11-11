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



/*************************************************************************
 *
 * Switches
 *
 ************************************************************************/
#ifndef __STATUI_H__
#define __STATUI_H__

/*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/
#include <eikenv.h>
#include <eikappui.h>
#include <eikproc.h>
#include <eikdialg.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikedwin.h>
#include <coeutils.h>
#include <barsread.h>
#include <eikfctry.h>
#include <eiklabel.h>

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/
#include "stat_interfaces.h"
#include "activeconnection.h"

/*************************************************************************
 *
 * Definitions
 *
 ************************************************************************/

// Control ID Constants
#define KStatusLabel			0
#define KErrorLabel				1
#define KVersionLabel			2
#define KTransportLabel			3
#define KAddressLabel			4
#define KTransportEdit			5
#define KAddressEdit			6
#define KActionButton			7
#define KExitButton				8
#define KInfoLabel				9
#define KLogButton				10
#define KControlCount			11

#define		KMaxChoiceItemSize			16
#define		KMaxChoiceItems				8

/*************************************************************************
 *
 * CStatChoice - Multi-choice box. This control is based on a read-only 
 * CEikEdwin. It is initialised with a set of strings. Users can cycle
 * through the option either by tapping on the control or using the 
 * left-right keys.
 *
 ************************************************************************/
class CStatChoice : public CEikEdwin
{
public:
	// Clean up
	~CStatChoice();

	// User interface to get and set items
	TInt AddItemL( TDesC &aItemStr );
	TDesC *CurrentItemStr();
	void SetCurrentItem( TInt aIndex );
	TInt CurrentItem();
	void ClearAllItems();

	// Handle User Input
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void SetNextItem();
	void SetPrevItem();

private:
	// Attributes
	TInt iItemCount, iCurrentItem;
	TBuf<KMaxChoiceItemSize> *iChoiceItems[KMaxChoiceItems];
};

/*************************************************************************
 *
 * CStatWindow - STAT GUI. Single window.
 *
 ************************************************************************/
class CStatWindow : public CCoeControl, public MCoeControlObserver, public MNotifyUI
{
public:
	// Constructors
	CStatWindow *NewL( const TRect& rect, MStatController *aStatController );
	CStatWindow();
	~CStatWindow();
	void ConstructL( const TRect& rect, MStatController *aStatController );

	// Public Interface
	void SetForeground( TBool aIsForeground );
	TCommStatus GetCommStatus();

	// Handle User Input
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	// From MNotifyUI
	void HandleStatusChange( TInt aSessionId, TCommStatus aNewStatus );
	void HandleError( TInt aError, void *);//aErrorData);
	void HandleInfo( const TDesC *aInfo );

protected:
	/*********************************************************************
	 *
	 * CStatIniData - Data structure used to write to and read from a user
	 * preference file that stores interface options between sessions.
	 *
	 *********************************************************************/
	class CStatIniData : public CBase
	{
	public:
		CStatIniData(void);
		~CStatIniData();

	public:
		TBool WriteIniData(const TDesC& aName) const;
		TBool ReadIniData(const TDesC& aName);

	public:
		TStatConnectType iConnectType;
		TInt iAddress;
		TBool iIsRunning;
		TBool iConnectAutomatically;

	private:
		HBufC* GetFileContents(const TDesC& aName) const;
	};

private:
	// From CCoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl( TInt aIndex ) const;
	void Draw(const TRect& aRect) const;

	// Helper Functions
	TRect GetControlPosition( TInt aIndex ) const;
	TInt PositionInRange( TPoint aPosition, TRect aArea ) const;
	TInt GetControlFromPoint( TPoint aPosition ) const;
	void OnTransportChange();
	void HandleActionL( void );
	void ClearControlPanel() const;
	void DrawEditBorders() const; 
	void	SetVersionLabelL();
private:
	// Controls
	CEikCommandButton *iActionButton, *iExitButton, *iLogButton;
	CEikLabel *iStatusLabel, *iErrorLabel, *iInfoLabel, *iVersionLabel;
	CEikLabel *iTransportLabel, *iAddressLabel;
	CStatChoice *iTransportEdit, *iAddressEdit;

	// Attributes
	TInt iLoggingEnabled;
	TBool bIsForeground;
	TCommStatus iStatStatus;
	MStatController *iStatController;
	mutable TInt iClearLabelsOnly;	// Ensure this is mutable and so can
									// be updated from within the 'const'
									// declared 'Draw' mehod.

	CActiveConnection *iAOConnection;	// Active object whose
										// only task is to open
										// a TCP socket to initialise
										// the socket server.

	// Ini data that we retain for the lifetime of the window.
	CStatIniData *iIniData;
};

#endif //__STATUI_H__
