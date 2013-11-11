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
//#define ENABLE_INFO

 /*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/
#include <eikcmbut.h>
#include <bautils.h>
#include <iniparser.h>
#include <s32file.h>

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/

#include "stat_window.h"
#include "assert.h"
#include "filedump.h"
#include "stat.h"

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/

/*************************************************************************
 *
 * Definitions - COMMON
 *
 ************************************************************************/

// Control sections
#define KControlPanel			100
#define KVersionPanel			101
#define KConnectPanel			102

// Position Constants
#define KButtonBorderOffset		4
#define KConnectLabelWidth		100

// Background Colours
#define KBackgroundColour		KMistyGray
#define KButtonColour			KLightGray
#define KLightGray				TRgb(0xC0C0C0)
#define KMistyGray				TRgb(0xFFC0C0)

// Addresses Contants
#define KDefaultAddressSerial		0
#define KDefaultAddressInfraRed		0
#define KSerialPortCount			6
#define KInfraRedPortCount			4

// Maximum address field length
#define KAddressTextLimit 16
#define MAXMESSAGESIZE 128

// Hardcoded comms parameters
//#define KSerialBaudRate			_L("38400")
//#define KIRBaudRate				_L("38400")
#define KSerialBaudRate			_L("115200")
#define KIRBaudRate				_L("115200")

//version info
#define KVersionNotKnown		_L("Version not known.")
#define KBuildInfoFilePath		_L("Z:\\system\\data\\BuildInfo.txt")
#define KBuildNumberFlag		_L8("ManufacturerSoftwareBuild")
const TInt KOffsetToBuildNumber = 27; // Length of ManufacturerSoftwareBuild + 2 spaces 

// Stored comms data
// This is a hard coded choice of drive depending uopn the
// target platform.  See CStatApiCommandDecoder::ScreenCapture
// for code whch determines if a drive is available.
#if defined __WINSCW__
	_LIT( KIniFile, "C:\\statui.ini" );
#else // defined __WINSCW__
	_LIT( KIniFile, "D:\\statui.ini" );
#endif // defined __WINSCW__

// Coms fields in ini file
_LIT( KIniFormat, "%S= %d\r\n" );
_LIT( KIniConnectType, "ConnectType" );
_LIT( KIniAddress, "Address" );
_LIT( KIniRunning, "Running" );
_LIT( KIniConnectAutomatically, "ConnectAutomatically" );

LOCAL_D MNotifyLogMessage *iMsg = NULL;
LOCAL_D	RFs iFsSession;

#define UndefinedData				(-1)
#define DefaultConnect				ESerial
#define DefaultAddress				1
#define DefaultRunning				0
#define DefaultConnectAutomatically	1

/*************************************************************************
 *
 * Definitions - SERIES 60
 *
 ************************************************************************/
#ifdef SYMBIAN_DIST_SERIES60

#define KButtonHeight				18
#define KButtonWidth				82
#define KStatusLabelWidth			(KButtonBorderOffset)

#define KConnectPanelTop			(3*KButtonBorderOffset + 2*KButtonHeight)
#define KEditControlFudgeTop		1
#define KConnectPanelFudge			6

#define KEditControlFlags			(CEikEdwin::ELineCursor | CEikEdwin::EAvkonEditor | CEikEdwin::EAvkonDisableCursor)

#define KConnectingStatusStr		_L("STAT is connecting.")
#define KWaitingForDataStatusStr	_L("STAT is waiting for data.")

#define KButtonsVisible				EFalse
#endif

/*************************************************************************
 *
 * Definitions - TECHVIEW
 *
 ************************************************************************/
#ifdef SYMBIAN_DIST_TECHVIEW

#define KButtonHeight				24
#define KButtonWidth				82
#define KStatusLabelWidth			(2*KButtonBorderOffset + KButtonWidth)

#define KConnectPanelTop			(4*KButtonBorderOffset + 3*KButtonHeight)
#define KEditControlFudgeTop		0
#define KConnectPanelFudge			0

#define KEditControlFlags			(CEikEdwin::ELineCursor)

#define KConnectingStatusStr		_L("STAT is waiting for a connection.")
#define KWaitingForDataStatusStr	_L("STAT is waiting to receive data.")

#define KButtonsVisible				ETrue
#endif

/*************************************************************************
 *
 * Definitions - UIQ
 *
 ************************************************************************/
#ifdef SYMBIAN_DIST_UIQ

#define KButtonHeight				20
#define KButtonWidth				60

#define KStatusLabelWidth			(2*KButtonBorderOffset + KButtonWidth)

#define KConnectPanelTop			(4*KButtonBorderOffset + 3*KButtonHeight)
#define KEditControlFudgeTop		0
#define KConnectPanelFudge			0

#define KEditControlFlags			(CEikEdwin::ELineCursor)

#define KConnectingStatusStr		_L("STAT is waiting for a connection.")
#define KWaitingForDataStatusStr	_L("STAT is waiting to receive data.")

#define KButtonsVisible				ETrue
#endif

/*************************************************************************
 *
 * CStatWindow - Construction
 *
 ************************************************************************/
CStatWindow *CStatWindow::NewL( const TRect& rect, MStatController *aStatController )
{
	CStatWindow* self = new(ELeave) CStatWindow();
	CleanupStack::PushL( self );
	self->ConstructL( rect, aStatController );
	CleanupStack::Pop();
	return self;
}

CStatWindow::CStatWindow()
{
	iStatusLabel = NULL;
	iErrorLabel = NULL;
	iVersionLabel = NULL;
	iTransportLabel = NULL;
	iAddressLabel = NULL;
	iTransportEdit = NULL;
	iAddressEdit = NULL;
	iActionButton = NULL;
	iExitButton = NULL;
	iInfoLabel = NULL;
	iLogButton = NULL;
	iClearLabelsOnly = 0;
	bIsForeground = EFalse;
	iIniData = NULL;
	iAOConnection = NULL;
}

CStatWindow::~CStatWindow()
{
	delete iIniData;
	delete iStatusLabel;
	delete iErrorLabel;
	delete iVersionLabel;
	delete iTransportLabel;
	delete iAddressLabel;
	delete iTransportEdit;
	delete iAddressEdit;
	delete iActionButton;
	delete iExitButton;
	delete iInfoLabel;
	delete iLogButton;

	if( iMsg->IsInitialised() )
		{
		iMsg->CloseFile();
		}

	iFsSession.Close();

	delete iMsg;
	iMsg =	NULL;

	asserte(NULL==iAOConnection);
}

void CStatWindow::ConstructL( const TRect& rect, MStatController *aStatController )
{
	TBuf<16> choiceItem;

	User::LeaveIfError( iFsSession.Connect() );
	iMsg = new FileDump();
	
	
	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	
	TBuf<KMaxFileName> statLogFile;
	
	TDriveNumber defaultSysDrive(EDriveC);
	RLibrary pluginLibrary;
	TInt pluginErr = pluginLibrary.Load(KFileSrvDll);
	
	if (pluginErr == KErrNone)
		{
		typedef TDriveNumber(*fun1)();
		fun1 sysdrive;
	
	#ifdef __EABI__
		sysdrive = (fun1)pluginLibrary.Lookup(336);
	#else
		sysdrive = (fun1)pluginLibrary.Lookup(304);
	#endif
		
		if(sysdrive!=NULL)
			{
			defaultSysDrive = sysdrive();
			}
		}
	pluginLibrary.Close();
	
	statLogFile.Append(TDriveUnit(defaultSysDrive).Name());
	statLogFile.Append(KFileSeparator);
	statLogFile.Append(KStatLogFile);
	
	iMsg->Init( iFsSession, statLogFile, NULL );

	// Store the pointer to the STAT controller
	iStatController = aStatController;

	// create a window and set the size to the full screen
	CreateWindowL();
	SetRect(rect);

	// create all sub components -- note that I should push stuff onto the cleanup stack
	iStatusLabel = new(ELeave) CEikLabel();
	iStatusLabel->SetContainerWindowL(*this);
	iStatusLabel->SetRect( GetControlPosition(KStatusLabel) );
	iStatusLabel->SetAlignment( EHLeftVCenter );
	iStatusLabel->SetFont( iEikonEnv->AnnotationFont() );
	iStatusLabel->SetBufferReserveLengthL( 200 );
	iStatusLabel->SetTextL( _L("STAT is not running") );
	iStatusLabel->ActivateL( );

	iErrorLabel = new(ELeave) CEikLabel();
	iErrorLabel->SetContainerWindowL(*this);
	iErrorLabel->SetRect( GetControlPosition(KErrorLabel) );
	iErrorLabel->SetAlignment( EHLeftVCenter );
	iErrorLabel->SetFont( iEikonEnv->AnnotationFont() );
	iErrorLabel->SetBufferReserveLengthL( 200 );
	iErrorLabel->SetTextL( _L("No errors") );
	iErrorLabel->ActivateL( );

	// The info-label is only for debugging. It prints strings sent up to the
	// UI from the transport and engine. It's too much code to take the control 
	// out completely so I'm just disabling it (making it invisible).
	iInfoLabel = new(ELeave) CEikLabel();
	iInfoLabel->SetContainerWindowL(*this);
	iInfoLabel->SetRect( GetControlPosition(KInfoLabel) );
	iInfoLabel->SetAlignment( EHLeftVCenter );
	iInfoLabel->SetFont( iEikonEnv->AnnotationFont() );
	iInfoLabel->SetBufferReserveLengthL( 200 );
	iInfoLabel->SetTextL( _L("") );
	iInfoLabel->ActivateL( );
	// iInfoLabel->MakeVisible( EFalse );
	iInfoLabel->MakeVisible( ETrue );

	iActionButton = new(ELeave) CEikCommandButton();
	iActionButton->SetContainerWindowL(*this);
	iActionButton->SetDisplayContent( CEikCommandButton::ETextOnly );
	iActionButton->SetTextL( _L("Start") );
	iActionButton->SetRect( GetControlPosition(KActionButton) );
	iActionButton->SetObserver( this );
	iActionButton->SetButtonLayout( (CEikCommandButton::TLayout)CEikCommandButton::EDenseFont );
	iActionButton->MakeVisible( KButtonsVisible );
	iActionButton->OverrideColorL( EColorButtonFaceClear, KButtonColour );
	iActionButton->OverrideColorL( EColorButtonFaceSet, KButtonColour );
	iActionButton->OverrideColorL( EColorButtonFaceSetPressed, KButtonColour );
	iActionButton->OverrideColorL( EColorButtonFaceClearPressed, KButtonColour );
	iActionButton->ActivateL();
	
	iExitButton = new(ELeave) CEikCommandButton();
	iExitButton->SetContainerWindowL(*this);
	iExitButton->SetDisplayContent( CEikCommandButton::ETextOnly );
	iExitButton->SetTextL( _L("Exit") );
	iExitButton->SetRect( GetControlPosition(KExitButton) );
	iExitButton->SetObserver( this );
	iExitButton->SetButtonLayout( (CEikCommandButton::TLayout)0 );
	iExitButton->MakeVisible( KButtonsVisible );
	iExitButton->OverrideColorL( EColorButtonFaceClear, KButtonColour );
	iExitButton->OverrideColorL( EColorButtonFaceSet, KButtonColour );
	iExitButton->OverrideColorL( EColorButtonFaceSetPressed, KButtonColour );
	iExitButton->OverrideColorL( EColorButtonFaceClearPressed, KButtonColour );
	iExitButton->ActivateL();

	iLogButton = new(ELeave) CEikCommandButton();
	iLogButton->SetContainerWindowL(*this);
	iLogButton->SetDisplayContent( CEikCommandButton::ETextOnly );
	iLogButton->SetTextL( _L("Logging") );
	iLogButton->SetRect( GetControlPosition(KLogButton) );
	iLogButton->SetObserver( this );
	iLogButton->SetButtonLayout( (CEikCommandButton::TLayout)0 );
	iLogButton->SetBehavior( EEikButtonLatches );
	iLogButton->MakeVisible( KButtonsVisible );
	iLogButton->OverrideColorL( EColorButtonFaceClear, KButtonColour );
	iLogButton->OverrideColorL( EColorButtonFaceSet, KButtonColour );
	iLogButton->OverrideColorL( EColorButtonFaceSetPressed, KButtonColour );
	iLogButton->OverrideColorL( EColorButtonFaceClearPressed, KButtonColour );
	iLogButton->ActivateL();
	iLoggingEnabled = EFalse;

	iVersionLabel = new(ELeave) CEikLabel();
	iVersionLabel->SetContainerWindowL(*this);
	iVersionLabel->SetRect( GetControlPosition(KVersionLabel) );
	iVersionLabel->SetAlignment( EHLeftVCenter );
	iVersionLabel->SetFont( iEikonEnv->AnnotationFont() );
	iVersionLabel->SetBufferReserveLengthL( 200 );
	SetVersionLabelL();;
	iVersionLabel->ActivateL( );

	iTransportLabel = new(ELeave) CEikLabel();
	iTransportLabel->SetContainerWindowL(*this);
	iTransportLabel->SetRect( GetControlPosition(KTransportLabel) );
	iTransportLabel->SetAlignment( EHLeftVCenter );
	iTransportLabel->SetFont( iEikonEnv->AnnotationFont() );
	iTransportLabel->SetBufferReserveLengthL( 200 );
	iTransportLabel->SetTextL( _L("Connect Type") );
	iTransportLabel->ActivateL( );

	iAddressLabel = new(ELeave) CEikLabel();
	iAddressLabel->SetContainerWindowL(*this);
	iAddressLabel->SetRect( GetControlPosition(KAddressLabel) );
	iAddressLabel->SetAlignment( EHLeftVCenter );
	iAddressLabel->SetFont( iEikonEnv->AnnotationFont() );
	iAddressLabel->SetBufferReserveLengthL( 200 );
	iAddressLabel->SetTextL( _L("Address") );
	iAddressLabel->ActivateL( );

	iTransportEdit = new(ELeave) CStatChoice();
	iTransportEdit->SetContainerWindowL(*this);
	iTransportEdit->ConstructL( KEditControlFlags ); 
	iTransportEdit->SetRect( GetControlPosition(KTransportEdit) );
	iTransportEdit->SetDimmed( EFalse );
	iTransportEdit->SetReadOnly( ETrue );
	iTransportEdit->SetFocus( ETrue );

	choiceItem.Copy( _L("TCPIP") );
	iTransportEdit->AddItemL( choiceItem );
	choiceItem.Copy( _L("Serial") );
	iTransportEdit->AddItemL( choiceItem );
	choiceItem.Copy( _L("InfraRed") );
	iTransportEdit->AddItemL( choiceItem );
	choiceItem.Copy( _L("Bluetooth") );
	iTransportEdit->AddItemL( choiceItem );

	iTransportEdit->SetCurrentItem( ESerial );
	iTransportEdit->OverrideColorL( EColorControlDimmedBackground, KButtonColour );	
	iTransportEdit->ActivateL();
	iTransportEdit->ClearSelectionL();
	iTransportEdit->SetCursorPosL( 0, EFalse );
	iTransportEdit->SetCurrentItem( 1 );

	iAddressEdit = new(ELeave) CStatChoice();
	iAddressEdit->SetContainerWindowL(*this);
	iAddressEdit->ConstructL( KEditControlFlags ); 
	iAddressEdit->SetRect( GetControlPosition(KAddressEdit) );
	iAddressEdit->SetDimmed( EFalse );
	iAddressEdit->SetReadOnly( ETrue );
	iAddressEdit->SetFocus( ETrue );
	iAddressEdit->OverrideColorL( EColorControlDimmedBackground, KButtonColour );	
	iAddressEdit->ActivateL();
	iAddressEdit->ClearSelectionL();
	iAddressEdit->SetCursorPosL( 0, EFalse );

	// Get the ini data from the user preference file.
	iIniData = new(ELeave) CStatIniData;
	iIniData->ReadIniData( KIniFile );
	// Set the user's preferred transpor connection
	// if one was specified in the ini file data.
	if( iIniData->iConnectType != UndefinedData )
	{
		iTransportEdit->SetCurrentItem( iIniData->iConnectType );
	}

	// call ontransportchange so that iAddressEdit is set up properly
	OnTransportChange();

	// Set the user's preferred transport address
	// if one was specified in the ini file data.
	if( iIniData->iAddress != UndefinedData )
	{
		iAddressEdit->SetCurrentItem( iIniData->iAddress );
	}

	// Check if the the applicatoin was running when it
	// was shut down last time (this can happen if the application
	// was not shut down gracefully through an 'exit', maybe
	// it was rebooted) and the user has specified that we
	// connect automatically.
	TBool connectAutomatically = FALSE;

	if( (iIniData->iIsRunning != UndefinedData) &&
		(iIniData->iConnectAutomatically != UndefinedData))
	{
		connectAutomatically = ((iIniData->iIsRunning) && 
					(iIniData->iConnectAutomatically));
	}
	// End of reading ini data.

	// activate the window (which in turn activates all sub components
	ActivateL();
	iTransportEdit->ClearSelectionL();
	iAddressEdit->ClearSelectionL();
	DrawNow();

	// Check if we need to connect now.
	if(connectAutomatically)
	{
		HandleActionL();
	}
}

/*************************************************************************
 *
 * CStatWindow - From CCoeControl
 *
 ************************************************************************/
void CStatWindow::SetForeground( TBool aIsForeground )
{
	bIsForeground = aIsForeground;
}

/*************************************************************************
 *
 * CStatWindow - From CCoeControl
 *
 ************************************************************************/
TInt CStatWindow::CountComponentControls() const
{
	return KControlCount;
}

CCoeControl* CStatWindow::ComponentControl(TInt aIndex) const
{
	switch( aIndex ) {
	case KStatusLabel:
		return iStatusLabel;
	case KErrorLabel:
		return iErrorLabel;
	case KVersionLabel:
		return iVersionLabel;
	case KTransportLabel:
		return iTransportLabel;
	case KAddressLabel:
		return iAddressLabel;
	case KTransportEdit:
		return iTransportEdit;
	case KAddressEdit:
		return iAddressEdit;
	case KActionButton:
		return iActionButton;
	case KExitButton:
		return iExitButton;
	case KInfoLabel:
		return iInfoLabel;
	case KLogButton:
		return iLogButton;
	}
	return NULL;
}

void CStatWindow::Draw( const TRect& /*aRect*/ ) const
{
	TRect r;

	// all draw operations must be done via the graphics context
	CWindowGc &gc = SystemGc();

	// draw the control panel
	ClearControlPanel();

	// if we signalled to only draw the label panel then stop here
	if( iClearLabelsOnly ) {
		iClearLabelsOnly = 0;	// Ensure this is mutable and so can
								// be updated from within the 'const'
								// declared 'Draw' mehod.
		return;
	}

	// draw the connect panel
	gc.SetBrushColor( KRgbWhite );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor( KRgbBlack );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	r = GetControlPosition( KConnectPanel );
	gc.DrawRect( r );

	// draw the version panel
	gc.SetBrushColor( KBackgroundColour );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor( KRgbBlack );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	r = GetControlPosition( KVersionPanel );
	gc.DrawRect( r );

	// draw a border on the text box with focus on series 60
	DrawEditBorders();
}

void CStatWindow::DrawEditBorders( ) const
{

	// Only draw a border for the focussed textbox
#ifdef SYMBIAN_DIST_SERIES60
	TRect r;
	CWindowGc &gc = SystemGc();

	if( iTransportEdit->IsFocused() ) {
		gc.SetBrushStyle( CGraphicsContext::ENullBrush );
		gc.SetPenColor( KLightGray );
		gc.SetPenStyle( CGraphicsContext::ESolidPen );
		r = GetControlPosition( KTransportEdit );
		r.iTl.iY -= 2;
		r.iTl.iX -= 1;
		gc.DrawRect( r );
	}

	if( iAddressEdit->IsFocused() ) {
		gc.SetBrushStyle( CGraphicsContext::ENullBrush );
		gc.SetPenColor( KLightGray );
		gc.SetPenStyle( CGraphicsContext::ESolidPen );
		r = GetControlPosition( KAddressEdit );
		r.iTl.iY -= 2;
		r.iTl.iX -= 1;
		gc.DrawRect( r );
	}
#endif
}
 
void CStatWindow::HandleControlEventL( CCoeControl* aControl, TCoeEvent /*aEventType*/ )
{
	TInt control;

	// Set the control to the aControl ptr. For Series-60 this will really be the 
	// control ID (passed from CStatAppUi::HandleControlEventL). On other platforms
	// this will really be a ptr -- which we convert below.
	control = (TInt)aControl;

	// Convert aControl into control identifier on non-series 60 platforms. This reduces
	// the amount of DFRD specific code.
#ifndef SYMBIAN_DIST_SERIES60
	if( aControl == iActionButton ) {
		control = KActionButton; 
	} else if( aControl == iExitButton ) {
		control = KExitButton;
	} else if( aControl == iLogButton ) {
		control = KLogButton;
	} else {
		return;
	}
#endif
	
	// Now handle the button events
	switch( control ) {

	// If we are idle then start the engine, if we are anything else then stop the engine
	case KActionButton:
		HandleActionL();
		break;
			
	// Send shutdown event to the appui, will end up in CStatAppUi::HandleControlEventL
	case KExitButton:
		if( iStatStatus == EIdle ) {
			ReportEventL( EEventRequestExit );
		}
		break;
	
	// For the logging button we want to toggle the current setting
	case KLogButton:
		iLoggingEnabled = ((iLoggingEnabled == EFalse) ? ETrue : EFalse);
		break;
	}
}

void CStatWindow::HandleActionL( void )
{
	TInt expInfo = KErrNone;
	TInt expLabel = KErrNone;
	TStatConnectType selectedTransport;
	TBuf<KAddressTextLimit> tbAddress;

	// if we are not in the idle state then stop the current session
	if( iStatStatus != EIdle ) {
		iStatController->StopSession( 1 );

		if(iAOConnection)
		{
			iAOConnection->CloseSocket();
			delete iAOConnection;
			iAOConnection = NULL;
		}

		// Store the fact that we have stopped in the
		// user preference ini data.
		iIniData->iIsRunning = FALSE;
		iIniData->WriteIniData( KIniFile );
		// End of writing ini data.

		return;
	}

	// otherwise -- first update the GUI
	TRAP( expInfo, (iInfoLabel->SetTextL(_L(""))) );
	TRAP( expLabel, (iErrorLabel->SetTextL(_L(""))) );
	if( (expInfo == KErrNone) && (expLabel == KErrNone) ) {
		iClearLabelsOnly = 1;
		DrawNow();
	}

	// get the transport and the basic address info
	selectedTransport = (enum TStatConnectType)iTransportEdit->CurrentItem();
	iAddressEdit->GetText( tbAddress );

	// get a simple pointer to the address so we can munge it
	char* p = (char*)tbAddress.Ptr();

	// now do any transport specific munging 
	switch( selectedTransport ) {

	// for serial - subtract one from the port count and add the baud rate
	case ESerial:
		(*p) -= 1;
		tbAddress.Append( _L("|") );
		tbAddress.Append( KSerialBaudRate );
		break;

	// for IR - subtract one from the port count and add the baud rate
	case EInfrared:
		(*p) -= 1;
		tbAddress.Append( _L("|") );
		tbAddress.Append( KIRBaudRate );
		break;
	case ETCPIP:
		//active object used to start ppp connection, only started if using tcpip 
		asserte(NULL==iAOConnection);
		iAOConnection=CActiveConnection::NewL(_L("81.89.143.203"),80);
		//if we are using tcpip
		iAOConnection->Start();
		break;
	case EBluetooth:
		break;
	// Add a case handler for the 'number of enumerations' to prevent
	// a compile warning.
	case ENumConnectTypes:
		;
		break;
	}

	// Store the session options in the user preference file
	// so we can prepare the interface next time.
	iIniData->iConnectType = selectedTransport;
	iIniData->iAddress = iAddressEdit->CurrentItem();
	iIniData->iIsRunning = TRUE;
	iIniData->WriteIniData( KIniFile );
	// End of writing ini data.

	// start the session
	iStatController->StartSession( selectedTransport, &tbAddress, this, &iFsSession, iMsg );
	return;
}

/*************************************************************************
 *
 * CStatWindow - Handle User Input
 *
 ************************************************************************/
TKeyResponse CStatWindow::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
{
	TKeyResponse ret = EKeyWasNotConsumed;

	// we process up and down at this level to allow moving the focus, all other
	// keys are passed to the edit box with the focus
	if( aKeyEvent.iScanCode == EStdKeyUpArrow ) 
	{
		iTransportEdit->SetFocus( ETrue );
		iAddressEdit->SetFocus( EFalse );
		DrawNow();
		return EKeyWasConsumed;
	} 
	else if( aKeyEvent.iScanCode == EStdKeyDownArrow ) 
	{
		iTransportEdit->SetFocus( EFalse );
		iAddressEdit->SetFocus( ETrue );
		DrawNow();
		return EKeyWasConsumed;
	}
	
	// pass the key to the control with the focus -- only if we are Idle
	if( iStatStatus == EIdle ) 
	{
		if( iAddressEdit->IsFocused() ) 
		{
			ret = iAddressEdit->OfferKeyEventL( aKeyEvent, aType );
		} 
		else if( iTransportEdit->IsFocused() ) 
		{
			ret = iTransportEdit->OfferKeyEventL( aKeyEvent, aType );
			OnTransportChange();
		}
	}
	return ret;
}

void CStatWindow::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TInt control;

	// Find out where this event occured so we can send it to the right control
	control = GetControlFromPoint( aPointerEvent.iPosition );
	switch( control ) {

	case KActionButton:
		iTransportEdit->SetFocus( EFalse );
		iAddressEdit->SetFocus( EFalse );
		iActionButton->HandlePointerEventL( aPointerEvent );
		break;

	case KExitButton:
		iTransportEdit->SetFocus( EFalse );
		iAddressEdit->SetFocus( EFalse );
		iExitButton->HandlePointerEventL( aPointerEvent );
		break;

	case KLogButton:
		iLogButton->HandlePointerEventL( aPointerEvent );
		break;

	case KTransportEdit:
		if( !iTransportEdit->IsDimmed() ) {
			iTransportEdit->SetFocus( ETrue );
			iAddressEdit->SetFocus( EFalse );
			iTransportEdit->HandlePointerEventL( aPointerEvent );
			OnTransportChange();
		}
		break;

	case KAddressEdit:
		if( !iAddressEdit->IsDimmed() ) {
			iTransportEdit->SetFocus( EFalse );
			iAddressEdit->SetFocus( ETrue );
			iAddressEdit->HandlePointerEventL( aPointerEvent );
		}
		break;

	default:
		if( aPointerEvent.iType != TPointerEvent::EButton1Down ) 
			break;
		iTransportEdit->SetFocus( EFalse );
		iAddressEdit->SetFocus( EFalse );
		DrawNow();
		break;
	}
}

/*************************************************************************
 *
 * CStatWindow - MNotifyUI
 *
 ************************************************************************/
void CStatWindow::HandleStatusChange( TInt /*aSessionId*/, TCommStatus aNewStatus )
{
	TInt exp = KErrNone;

	TPtrC status[] = {
		_L( "STAT is not running." ),
		_L( "STAT is initialising." ),
		_L( "STAT is initialised." ),
		KConnectingStatusStr,
		_L( "STAT is connected." ),
		_L( "STAT is disconnecting." ),
		_L( "STAT is disconnected." ),
		_L( "STAT is releasing." ),
		_L( "STAT is sending data."),
		KWaitingForDataStatusStr,
		_L( "<error>" )
	};

	// save the new status
	iStatStatus = aNewStatus;

	// set the status label
	assert( (aNewStatus >= EIdle) && (aNewStatus < ELast) );
	TRAP( exp, (iStatusLabel->SetTextL(status[aNewStatus])) );

	// enable / disable the appropriate buttons
	switch( iStatStatus ) {

	case EIdle:
		TRAP( exp, (iActionButton->SetTextL(_L("Start"))) );
		iActionButton->SetDimmed( EFalse );
		iExitButton->SetDimmed( EFalse );
		iActionButton->DrawNow();
		iExitButton->DrawNow();
		iTransportEdit->SetDimmed( EFalse );
		iAddressEdit->SetDimmed( EFalse );
		iLogButton->SetDimmed( EFalse );
		// Remove the call to OnTransport change because the
		// transport has not changed and we have just set
		// the state of the interface components here.
		// OnTransportChange();
		break;

	case EDisconnecting: 
	case EDisconnected: 
	case EReleasing: 
		TRAP( exp, (iActionButton->SetTextL(_L("Stop"))) );
		iActionButton->SetDimmed( ETrue );
		iExitButton->SetDimmed( ETrue );
		iActionButton->DrawNow();
		iExitButton->DrawNow();
		iTransportEdit->SetDimmed( ETrue );
		iAddressEdit->SetDimmed( ETrue );
		iLogButton->SetDimmed( ETrue );
		break;

	case EConnected:
		DrawNow();
		/* fall through */

	case ESendPending:
	case EReceivePending:
	case EInitialising: 
	case EInitialised: 
	case EConnecting: 
		TRAP( exp, (iActionButton->SetTextL(_L("Stop"))) );
		iActionButton->SetDimmed( EFalse );
		iExitButton->SetDimmed( ETrue );
		iActionButton->DrawNow();
		iExitButton->DrawNow();
		iTransportEdit->SetDimmed( ETrue );
		iAddressEdit->SetDimmed( ETrue );
		iLogButton->SetDimmed( ETrue );
		break;

	case ELast:
		assert( !"Illegal case" );
		break;
	}

	// If we are in the foreground then redraw the screen
	iClearLabelsOnly = 1;
	DrawNow();
}

void CStatWindow::HandleError( TInt aError, void* aErrorData )
{
	TInt exp;
	TBuf<MAXMESSAGESIZE> msg; 

	// If aError is < 0 then it is an EPOC error code, if it is > 0 then
	// it is one of my error codes. aErrorData is defined for each code
	switch( aError ) 
	{
		case KSTErrReadFailure:
			msg.SetLength( 0 );
			msg.Append( _L("Read failed (") );
			msg.AppendNum( (int)aErrorData );
			msg.Append( _L(").") );
			TRAP( exp, (iErrorLabel->SetTextL(msg)) );
			break;

		//This means that a disconnect has come across from the PC.  Because we are not sure
		//of why this occured (i.e. could be end of script or an error) we will not display
		//a message to ensure that the user can at least see what the error was (if there was one)
		case KErrDisconnected:
			break;

		case KErrAccessDenied:
			TRAP( exp, (iErrorLabel->SetTextL(_L("Comm port access denied."))) );
			break;
	
		case KErrNotSupported:
			TRAP( exp, (iErrorLabel->SetTextL(_L("Unsupported comm port."))) );
			break;

		case KErrCancel:
			TRAP( exp, (iErrorLabel->SetTextL(_L("Operation cancelled."))) );
			break;

		default:
			TRAP( exp, (iErrorLabel->SetTextL(_L("Unknown error occured."))) );
			break;
	}
}

void CStatWindow::HandleInfo( const TDesC *aInfo )
{
	TInt exp;
	TRAP( exp, (iInfoLabel->SetTextL(*aInfo)) );
	iClearLabelsOnly = 1;
	DrawNow();
}

/*************************************************************************
 *
 * CStatWindow - Helper Functions
 *
 ************************************************************************/
TCommStatus CStatWindow::GetCommStatus()
{
	return iStatStatus;
}

void CStatWindow::ClearControlPanel() const
{
	TRect r;

	// all draw operations must be done via the graphics context
	CWindowGc &gc = SystemGc();

	// draw the control panel
	gc.SetBrushColor( KBackgroundColour );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenColor( KRgbBlack );
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	r = GetControlPosition( KControlPanel );
	gc.DrawRect( r );
}

void CStatWindow::OnTransportChange()
{
	TInt i;
	TInt exp;
	TBuf<1> null( _L("") );
	TBuf<9> portname( _L("1") );
	TInt currentItem = iTransportEdit->CurrentItem();

	// Clear the Address field and set text to NULL
	iAddressEdit->ClearAllItems();
	TRAP( exp, (iAddressEdit->SetTextL(&null)) );
	TRAP( exp, (iAddressEdit->ClearSelectionL()) );
	
	// Now set the items
	switch( currentItem ) {

	case EBluetooth:
	case ETCPIP:
		iAddressEdit->SetFocus( EFalse );
		iAddressEdit->SetDimmed( ETrue );
		break;

	case ESerial:
		for( i = 0; i < KSerialPortCount; i++ ) {
			TRAP( exp, (iAddressEdit->AddItemL(portname)) );
			((short*)portname.PtrZ())[0]++;
		}
		iAddressEdit->SetCurrentItem( KDefaultAddressSerial );
		iAddressEdit->SetDimmed( EFalse );
		break;

	case EInfrared:
		for( i = 0; i < KInfraRedPortCount; i++ ) {
			TRAP( exp, (iAddressEdit->AddItemL(portname)) );
			((short*)portname.PtrZ())[0]++;
		}
		iAddressEdit->SetCurrentItem( KDefaultAddressInfraRed );
		iAddressEdit->SetDimmed( EFalse );
	}

	// Redraw so the changes take effect
	iAddressEdit->DrawNow();

}

TInt CStatWindow::PositionInRange( TPoint aPosition, TRect aArea ) const
{
	if( (aPosition.iX <= aArea.iBr.iX) && (aPosition.iX >= aArea.iTl.iX) && (aPosition.iY <= aArea.iBr.iY) && (aPosition.iY >= aArea.iTl.iY) )
		return 1;
	return 0;
}

TInt CStatWindow::GetControlFromPoint( TPoint aPosition ) const
{
	TRect rActionButton, rExitButton, rLogButton;
	TRect rAddressEdit, rTransportEdit;

	// Get the positions of the controls that can handle pointer events
	rActionButton = GetControlPosition( KActionButton );
	rExitButton = GetControlPosition( KExitButton );
	rLogButton = GetControlPosition( KLogButton );
	rTransportEdit = GetControlPosition( KTransportEdit );
	rAddressEdit = GetControlPosition( KAddressEdit );

	// See if any match 
	if( PositionInRange(aPosition,rTransportEdit) )
		return KTransportEdit;
	else if( PositionInRange(aPosition,rAddressEdit) )
		return KAddressEdit;
	else if( PositionInRange(aPosition,rLogButton) )
		return KLogButton;
	else if( PositionInRange(aPosition,rActionButton) )
		return KActionButton;
	else if( PositionInRange(aPosition,rExitButton) )
		return KExitButton;

	return KControlCount;
}

TRect CStatWindow::GetControlPosition( TInt aControlId ) const
{
	TRect controlRect;

	switch( aControlId ) {

	case KActionButton:
		controlRect = Rect();
		controlRect.iBr.iX -= KButtonBorderOffset;
		controlRect.iTl.iX = controlRect.iBr.iX - KButtonWidth;
		controlRect.iTl.iY += KButtonBorderOffset;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KExitButton:
		controlRect = Rect();
		controlRect.iBr.iX -= KButtonBorderOffset;
		controlRect.iTl.iX = controlRect.iBr.iX - KButtonWidth;
		controlRect.iTl.iY += 2*KButtonBorderOffset + KButtonHeight;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KLogButton:
		controlRect = Rect();
		controlRect.iBr.iX -= KButtonBorderOffset;
		controlRect.iTl.iX = controlRect.iBr.iX - KButtonWidth;
		controlRect.iTl.iY += 3*KButtonBorderOffset + 2*KButtonHeight;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KStatusLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY += KButtonBorderOffset;
		controlRect.iBr.iX -= KStatusLabelWidth;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KErrorLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY += 2*KButtonBorderOffset + KButtonHeight;
		controlRect.iBr.iX -= KStatusLabelWidth;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KInfoLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY += 3*KButtonBorderOffset + 2*KButtonHeight;
		controlRect.iBr.iX -= KStatusLabelWidth;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KVersionLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY = controlRect.iBr.iY - 2*KButtonHeight;
		controlRect.iBr.iX -= KButtonBorderOffset;
		break;

	case KTransportLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY = KConnectPanelTop + KButtonBorderOffset + KConnectPanelFudge;
		controlRect.iBr.iX += 2*KButtonBorderOffset + KConnectLabelWidth;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KAddressLabel:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset;
		controlRect.iTl.iY = KConnectPanelTop + 2*KButtonBorderOffset + KButtonHeight + KConnectPanelFudge;
		controlRect.iBr.iX += 2*KButtonBorderOffset + KConnectLabelWidth;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KTransportEdit:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset + KConnectLabelWidth;
		controlRect.iTl.iY = KConnectPanelTop + KButtonBorderOffset + KEditControlFudgeTop + KConnectPanelFudge;
		controlRect.iBr.iX -= KButtonBorderOffset;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KAddressEdit:
		controlRect = Rect();
		controlRect.iTl.iX += 2*KButtonBorderOffset + KConnectLabelWidth;
		controlRect.iTl.iY = KConnectPanelTop + 2*KButtonBorderOffset + KButtonHeight + KEditControlFudgeTop + KConnectPanelFudge;
		controlRect.iBr.iX -= KButtonBorderOffset;
		controlRect.iBr.iY = controlRect.iTl.iY + KButtonHeight;
		break;

	case KControlPanel:
		controlRect = Rect();
		controlRect.iBr.iY = KConnectPanelTop;
		break;

	case KConnectPanel:
		controlRect = Rect();
		controlRect.iTl.iY = KConnectPanelTop;
		controlRect.iBr.iY -= 2*KButtonHeight;
		break;

	case KVersionPanel:
		controlRect = Rect();
		controlRect.iTl.iY = controlRect.iBr.iY - 2*KButtonHeight;
		break;

	default:
		assert( !"Unknown Control" );
		break;
	}

	return controlRect;
}
void	CStatWindow::SetVersionLabelL()
{
	TBuf<32> versionstring;

	RFs& fs=CCoeEnv::Static()->FsSession();
	TBool fileopen=EFalse;
	if(BaflUtils::FileExists(fs,KBuildInfoFilePath))
		{
		if (!(fs.IsFileOpen(KBuildInfoFilePath,fileopen)))
			{													
			RFile file;			
			User::LeaveIfError(file.Open(fs, KBuildInfoFilePath, EFileShareAny));					
			TBuf8<256> buf;
			User::LeaveIfError(file.Read(buf));
			TInt startOfData =0;
			startOfData = buf.Find(KBuildNumberFlag);
			if(!(startOfData==KErrNotFound))  // if build number flag present
				{
				startOfData += KOffsetToBuildNumber;
				if(buf.Length()>startOfData)  // if build number present
					{
					TPtrC8 buildNumPtr = buf.Mid(startOfData, buf.Length()-startOfData);
					versionstring.Copy(buildNumPtr);
					}
				}
			file.Close();
			}
		}
		else
			versionstring.Copy(KVersionNotKnown);	
	iVersionLabel->SetTextL(versionstring);

}
/*************************************************************************
 *
 * CStatChoice - Complete
 *
 ************************************************************************/
CStatChoice::~CStatChoice( )
{
	ClearAllItems();
}

void CStatChoice::ClearAllItems()
{
	int i;

	// delete all the existing items
	for( i = 0; i < iItemCount; i++ )
		delete iChoiceItems[i];

	// set the count to zero
	iItemCount = 0;
	iCurrentItem = 0;
}

TInt CStatChoice::AddItemL( TDesC &aItemStr )
{
	// make sure that there is room left at the inn
	assert( iItemCount < KMaxChoiceItems );

	// create the new item
	iChoiceItems[iItemCount] = new(ELeave) TBuf<KMaxChoiceItemSize>(aItemStr);
	iItemCount++;

	// done
	return KErrNone;
}

TDesC *CStatChoice::CurrentItemStr()
{
	// make sure there is something to return
	if( iItemCount == 0 )
		return NULL;

	// sanity check the index
	assert( (iCurrentItem >= 0) && (iCurrentItem < iItemCount) );

	// return a pointer to the descriptor
	return iChoiceItems[iCurrentItem];
}

void CStatChoice::SetCurrentItem( TInt aIndex )
{
	TInt exp;

	// check the passed index
	if( (aIndex < 0) || (aIndex >= iItemCount) ) 
		return;
	iCurrentItem = aIndex;

	// set the text
	TRAP( exp, (SetTextL(iChoiceItems[iCurrentItem])) );
	TRAP( exp, (SetCursorPosL(0,EFalse)) );
	TRAP( exp, ClearSelectionL() );
	DrawNow();
}

TInt CStatChoice::CurrentItem()
{
	return iCurrentItem;
}

void CStatChoice::SetNextItem()
{
	TInt item;
	item = (iCurrentItem + 1) % iItemCount;
	SetCurrentItem( item );
}

void CStatChoice::SetPrevItem()
{
	TInt item;
	item = iCurrentItem - 1;
	if( item == -1 )
		item = iItemCount - 1;
	SetCurrentItem( item );
}

TKeyResponse CStatChoice::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
{
	if( !IsDimmed() ) {
		if( (aType == EEventKeyDown) && (aKeyEvent.iScanCode == EStdKeyLeftArrow) ) {
			SetPrevItem();
		} else if( (aType == EEventKeyDown) && (aKeyEvent.iScanCode == EStdKeyRightArrow) ) {
			SetNextItem();
		}
	}
	return EKeyWasConsumed;
}

void CStatChoice::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
	if( !IsDimmed() ) {
		if( aPointerEvent.iType == TPointerEvent::EButton1Up )
			SetNextItem();
	}
}

/*************************************************************************
 *
 * Class CStatWindow::CStatIniData
 *
 ************************************************************************/

/*************************************************************************
 *
 * Class CStatWindow::CStatIniData - Construction
 *
 ************************************************************************/

CStatWindow::CStatIniData::CStatIniData(void) :
	iConnectType(static_cast<TStatConnectType>(UndefinedData)),
	iAddress(UndefinedData),
	iIsRunning(UndefinedData),
	iConnectAutomatically(UndefinedData)
{
}

CStatWindow::CStatIniData::~CStatIniData()
{
}

TBool CStatWindow::CStatIniData::WriteIniData(const TDesC& aName) const
{
	// Write data to an ini file in the same format such that
	// the CIniData class can read it.
	// Wouldn't it be nice if CIniData wrote data as well as reading
	// it?

	TBool result = TRUE;
	TInt errNum = KErrNone;

	RFs fSession;

	if( result == TRUE )
	{
		errNum = fSession.Connect();

		result = ( errNum == KErrNone );

		if( result == TRUE )
		{
			RFile	file;

			errNum = file.Replace( fSession, aName, EFileWrite );

			result = ( errNum == KErrNone );

			if( result == TRUE )
			{
				const int iniLineBufferLength = 256;
				TBuf<iniLineBufferLength> iniFileBuffer;
				TBuf8<iniLineBufferLength> writeBuffer;

				// Write the connect type.
				iniFileBuffer.Format( KIniFormat, &KIniConnectType,
					iConnectType );
				writeBuffer.Copy( iniFileBuffer );
				file.Write( writeBuffer );

				// Write the address.
				iniFileBuffer.Format( KIniFormat, &KIniAddress,
					iAddress );
				writeBuffer.Copy( iniFileBuffer );
				file.Write( writeBuffer );

				// Write the running status.
				iniFileBuffer.Format( KIniFormat, &KIniRunning,
					iIsRunning );
				writeBuffer.Copy( iniFileBuffer );
				file.Write( writeBuffer );

				// Write the connect-automatically preference.
				iniFileBuffer.Format( KIniFormat, &KIniConnectAutomatically,
					iConnectAutomatically );
				writeBuffer.Copy( iniFileBuffer );
				file.Write( writeBuffer );

				file.Close();
			}

			fSession.Close();
		}
	}

	return (result);
}

#if defined __CINIDATA_H__
TBool CStatWindow::CStatIniData::ReadIniData(const TDesC& aName )
{
	// Read configuration data from ini file if we are using
	// CIniData and its supporting DLL.

	TBool result = TRUE;
	TPtrC iniText;
	TLex lexResult;
	TInt lexValue;

	CIniData* lIniFile = NULL;

	// If the file can not be opened then we store
	// the error and do not process any further.
	if( result == TRUE )
	{
		// Open ini data file object.
		TRAPD( r, lIniFile = CIniData::NewL( aName ) );
		result = ( r == KErrNone );
	}

	// If any ini file data value is not found then we ignore
	// the error and carry on and try to read the next.
	// The caller will know what was read by the values in the
	// ini data.

	// Read the connect type.
	if( result == TRUE )
	{
		if( lIniFile->FindVar( KIniConnectType, iniText ) )
		{
			lexResult = iniText;
			if( KErrNone == lexResult.Val( lexValue ) )
			{
				iConnectType = static_cast<TStatConnectType>(lexValue);
			}
		}
	}	

	// Read the address.
	if( result == TRUE )
	{
		if( lIniFile->FindVar( KIniAddress, iniText ) )
		{
			lexResult = iniText;
			if( KErrNone == lexResult.Val( lexValue ) )
			{
				iAddress = lexValue;
			}
		}
	}	

	// Read the running status.
	if( result == TRUE )
	{
		if( lIniFile->FindVar( KIniRunning, iniText ) )
		{
			lexResult = iniText;
			if( KErrNone == lexResult.Val( lexValue ) )
			{
				iIsRunning = lexValue;
			}
		}
	}	

	// Read the connect-automatically preference.
	if( result == TRUE )
	{
		if( lIniFile->FindVar( KIniConnectAutomatically, iniText ) )
		{
			lexResult = iniText;
			if( KErrNone == lexResult.Val( lexValue ) )
			{
				iConnectAutomatically = lexValue;
			}
		}
	}	

	delete lIniFile;
	lIniFile = NULL;

	return (result);
}
#else // defined __CINIDATA_H__
TBool CStatWindow::CStatIniData::ReadIniData(const TDesC& aName )
{
	const TInt KOffsetToData = 2; // The length of '= '.

	// Read configuration data from ini file if we are using
	// our own code.  There is no advantage in this and
	// the downside is maintenance and code size
	// but support for CIniData is uncertain and it
	// does not have a write facility anyway.

	TBool result = TRUE;
	HBufC *data = NULL;

	if( result == TRUE )
	{
		data = GetFileContents( aName );

		result = ( NULL != data );
	}

	if( result == TRUE )
	{
		asserte( data != NULL );

		TInt fieldIndex = 0;
		TInt val = -1;
		TLex value;

		// Get the connect type.
		fieldIndex = data->Find( KIniConnectType );

		if( KErrNotFound != fieldIndex )
		{
			fieldIndex += ( KIniConnectType.iTypeLength + KOffsetToData );
			value = data->Ptr() + fieldIndex;
			val = -1;
			value.Val(val);
			iConnectType = static_cast<TStatConnectType>(val);
		}

		// Get the address.
		fieldIndex = data->Find( KIniAddress );

		if( KErrNotFound != fieldIndex )
		{
			fieldIndex += ( KIniAddress.iTypeLength + KOffsetToData );
			value = data->Ptr() + fieldIndex;
			val = -1;
			value.Val(val);
			iAddress = val;
		}

		// Get the running status.
		fieldIndex = data->Find( KIniRunning );

		if( KErrNotFound != fieldIndex )
		{
			fieldIndex += ( KIniRunning.iTypeLength + KOffsetToData );
			value = data->Ptr() + fieldIndex;
			val = -1;
			value.Val(val);
			iIsRunning = val;
		}

		// Get the connect-automatically preference.
		fieldIndex = data->Find( KIniConnectAutomatically );

		if( KErrNotFound != fieldIndex )
		{
			fieldIndex += ( KIniConnectAutomatically.iTypeLength + KOffsetToData );
			value = data->Ptr() + fieldIndex;
			val = -1;
			value.Val(val);
			iConnectAutomatically = val;
		}
	}

	delete data;
	data = NULL;

	return (result);
}

HBufC* CStatWindow::CStatIniData::GetFileContents(const TDesC& aName) const
{
	TBool result = TRUE;
	TInt errNum = KErrNone;
	HBufC8 *local = NULL;
	HBufC *retVal = NULL;
	TInt size = 0;

	RFs fSession;

	if( result == TRUE )
	{
		errNum = fSession.Connect();

		result = ( errNum == KErrNone );

		if( result == TRUE )
		{
			RFile file;

			errNum = file.Open( fSession, aName, EFileRead | EFileStream | EFileShareAny );

			result = ( errNum == KErrNone );

			if( result == TRUE )
			{
				errNum = file.Size( size );

				result = ( errNum == KErrNone );

				if( result == TRUE )
				{
					TRAP(errNum, local = HBufC8::NewL(size));

					result = ( errNum == KErrNone );
				}

				if( result == TRUE )
				{
					TPtr8 pBuffer( local->Des() );

					errNum = file.Read( pBuffer );

					result = ( errNum == KErrNone );
				}

				file.Close();
			}

			fSession.Close();
		}
	}

	if( TRUE == result )
	{
		// Need to copy the 8 bit read buffer to a 16 bit buffer.
		TRAP(errNum, retVal = HBufC::NewL(size));

		result = ( errNum == KErrNone );

		if( result == TRUE )
		{
			retVal->Des().Copy( local->Des() );
		}
	}

	delete local;
	local = NULL;

	return ( retVal );
}
#endif // defined __CINIDATA_H__
