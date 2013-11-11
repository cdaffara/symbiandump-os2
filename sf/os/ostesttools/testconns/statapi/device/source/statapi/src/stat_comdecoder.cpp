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



#include <hal.h>

#ifndef LIGHT_MODE
	#include <eikenv.h>
	#include <apgtask.h>	//for TApaTask usage
	#include <apgwgnam.h>	//for CApaWindowGroupName usage
#endif
#include "assert.h"
#include "stat.h"
#include "statapi_commanddecoder.h"
#include "keycodes.h"
#include "modifiers.h"
#include "statapi_deviceinformation.h"
#include "datasupplier_memory.h"
#include "datasupplier_file.h"
#include "dataconsumer_memory.h"
#include "test/testshareddata.h"
#include "test/tefshareddata.h"

#ifndef LIGHT_MODE
#include <s32mem.h>
#include "appinstall.h"
#endif // ifndef LIGHT_MODE
// This redefinition is because CTRL ends in an L and so is picked up (incorrectly) by leavescan
#define	  CTRLA CTRL

#define   ESTATNoMoreKeys (EStdKeyDecBrightness+1)

#define	  ESMLShift		1
#define	  ESMRShift		2
#define	  ESMLAlt		4
#define   ESMRAlt		8
#define   ESMLCtrl	    16
#define   ESMRCtrl	    32
#define   ESMLFunc		64
#define	  ESMRFunc		128

const TInt KMaxSharedDataName	= 128;
const TInt KMaxSharedDataString	= 1048;

#ifndef LIGHT_MODE
_LIT8( newLine, "\r\n" );
_LIT8( driveFormat, "%c,%S,%Ld%S" );
_LIT8( entryFormat, "%S,%d,%S,%d%S" );
_LIT8( dateFormat, "%02d/%02d/%04d %02d:%02d" );

#endif // ifndef LIGHT_MODE

//-----------------------------------------------------------------------------------
//constructor
CStatApiCommandDecoder::CStatApiCommandDecoder()
: iFs( NULL ), pMsg(NULL)
{
}

//-----------------------------------------------------------------------------------

CStatApiCommandDecoder* CStatApiCommandDecoder::NewL(RFs *const aSession, MNotifyLogMessage *const aMsg)
{	
	CStatApiCommandDecoder* self = new(ELeave) CStatApiCommandDecoder();

	CleanupStack::PushL(self);
	
	self -> ConstructL(aSession, aMsg);
	
	CleanupStack::Pop();
	
	return self;
}

//-----------------------------------------------------------------------------------

void CStatApiCommandDecoder::ConstructL(RFs *const aSession, MNotifyLogMessage *const aMsg)
{
#ifndef LIGHT_MODE
	// Try to connect to the window session server.
	// Do not leave if there is an error as we may be in a
	// text only mode (text shell) and the window server is not
	// available.
	// We carry on for now but certain features of the command
	// decoder will become 'unsupported' and will fail.
	//creation of a new windows server session
	if(iWs.Connect() != KErrNone) {
		return;
	}
	// This line is for debug purposes.
	// It is replicated in the function 'IsWindowServerAvailable'.
	// TInt wsHandle = iWs.WsHandle();
#endif // ifndef LIGHT_MODE

	iFs = aSession;
	pMsg = aMsg;

#ifndef LIGHT_MODE
	if(IsWindowServerAvailable())
		{
		// Only start the font and bitmap server if we are in
		// window shell mode.
		User::LeaveIfError(FbsStartup());
		User::LeaveIfError(RFbsSession::Connect());	//font and bitmap server

		// Create the data we use for screen capture.
		// Do this just once and re-use for each screen image.
	    iDevice = new(ELeave) CWsScreenDevice(iWs);	//Create new bitmap
	    iDevice -> Construct();

		// create new bitmap
		screenBitmap = new(ELeave) CWsBitmap(iWs);	//Create new bitmap

		// set the display mode for the bitmap
		TInt gray;	
		TInt color;
		TDisplayMode mode = iWs.GetDefModeMaxNumColors(color, gray);

		TSize size = iDevice -> SizeInPixels();

		TInt ret = screenBitmap->Create(size, mode);

		if (ret != KErrNone)
			{
			User::Leave(ret);
			}
		}
#endif // ifndef LIGHT_MODE

    HAL::Get(HALData::EMachineUid, iUIDValue);;
    
    //set system drive
   
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
	
	iSystemDrive.Append(TDriveUnit(defaultSysDrive).Name());
	iSystemDrive.Append(KFileSeparator);

    
    

	RefreshStatus( );
}

//-----------------------------------------------------------------------------------
//destructor
CStatApiCommandDecoder::~CStatApiCommandDecoder()
{
#ifndef LIGHT_MODE
	// Clean-up objects created for screen capture.
     if(iDevice != NULL)
     {
         delete iDevice;
     }
 
    if(screenBitmap != NULL)
     {
         delete screenBitmap;
     }

     RFbsSession::Disconnect();
     iWs.Close();
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//
// Do whatever the user requested
//
TInt CStatApiCommandDecoder::ExecuteCommand(TUint commandtype,
	MDataConsumer *const aDataConsumer, MDataSupplier **aDataSupplier)
{
	TInt ret = KErrNone;
	TInt exp = KErrNone;

	TInt commandLength = 0;

	if (aDataConsumer)
		{
		aDataConsumer->GetTotalSize(commandLength);
		pMsg->Msg (_L("DECODER: Decoding [%c] with [%d] bytes of data"), commandtype, commandLength);
		}
	else
        {
		pMsg->Msg (_L("DECODER: Decoding [%c]"), commandtype);
        }

	// make local copy but not for raw data (Transfer file command), Keypress or WriteTEFSharedData
	command.Zero();
	if (commandtype != 'T' && commandtype != 'K' && commandtype != 'O' &&
		aDataConsumer && commandLength)
		{
		command.Copy( *(aDataConsumer) );
		}

	// Screen saver activates regardless of activity.
	// Will check for navigation and keypresses here and reset the 
	// inactivity timer.
	// Ultimately, this should be handled by lower layer key 
	// event processing.
	if (commandtype == 'K' || commandtype == 'L' || 
		commandtype == 'M' || commandtype == 'H')
	{
		User::ResetInactivityTime();
	}

	// Check if we are doing a command that depends upon the
	// window session server or the application architecture
	// (which may not be available if we are in text shell mode).
	// If there is no window server or application architecture
	// then we do not support:
	// SendText, SendSystemKey, SendCombinationKeys,
	// SendKeyHold, StopApplicationL, OpenIcon,
	// OpenFile, StartESHell, Screenshot and
	// StartApplication.
	if (commandtype == 'K' || commandtype == 'L' || 
		commandtype == 'M' || commandtype == 'H' ||
		commandtype == 'C' || commandtype == 'I' ||
		commandtype == 'F' || commandtype == '!' ||
		commandtype == 'A' || commandtype == 'S' )
		{

		if( ! IsWindowServerAvailable())
			{
			ret = KErrNotSupported;
			}
		}	

	if(ret == KErrNone)
		{
		//check message header type
		switch (commandtype)
			{

			case ('K'):
				{
                if (aDataConsumer && commandLength)
                    {
					ret = SendText(aDataConsumer, commandLength);
                    }
				}
				break;
			case ('L'):
				ret = SendSystemKeys();
				break;
			case ('M'):
				ret = SendCombinationKeys();
				break;
			case ('H'):
				ret = SendKeyHold();
				break;
			case ('A'):
				TRAP( exp, (ret = StartApplicationL()) );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('J'):
				TRAP( exp, (ret = StartProgramL(aDataSupplier)) );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('C'):
				TRAP(ret, StopApplicationL());
				break;
			case ('F'):
				TRAP( exp, (ret = OpenFileL()) );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('I'):
				ret = OpenIcon();
				break;
			case ('!'):
                if(command.Length() == 0)
                    {
                    TRAP(ret, StartEShellL());
                    }
                else
                    {
                    if(command == _L("!"))
                        {
                        TRAP(ret, StopEShellL());
                        }
                    }
				break;
			case ('U'):
				TRAP( exp, ret = DeleteFileL() );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('2'):
                if(aDataConsumer) 
                    {
                    TRAP( exp, ret = RenameFileL(aDataConsumer) );
                    ret = ((exp != KErrNone) ? exp : ret);
                    }
				break;
			case ('1'):
                if(aDataConsumer)
                    {
                    ret = CheckLocation(aDataConsumer);
                    }
				break;
			case ('Y'):
				ret = CreateFolder();
				break;
			case ('Z'):
				ret = RemoveFolder();
				break;
			case ('T'):
                if(aDataConsumer)
                    {
                    ret = TransferFile(aDataConsumer);
                    }
				break;
			case ('R'):
                if(aDataConsumer)
                    {
                    ret = ReceiveFile(commandtype, aDataConsumer, aDataSupplier);
                    }
				break;
			case ('X'):
			case ('S'):
                if(aDataConsumer)
                    {
                    ret = ReceiveFile(commandtype, aDataConsumer, aDataSupplier);
                    }
				//DeleteLastFile();
				break;
			case ('D'):
				ret = DeviceInfo(aDataSupplier);
				break;
			case ('G'):
				SaveLogFile(aDataSupplier);
				break;
			case ('W'):
				ret = GetDiskDriveInformationL(aDataSupplier);
				break;
			case ('V'):
                if(aDataConsumer)
                    {
                    ret = GetDirectoryInformationL(aDataConsumer, aDataSupplier);
                    }
				break;
			case ('B'):
				RefreshStatus( );
				break;
			case ('E'):	// end of script
				break;
			case ('?'):	// resync command
				break;
			case ('N'): // Retrieve TEF shared data
				TRAP( exp, ret = ReadTEFSharedDataL( aDataSupplier ) );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('O'): // Update TEF shared data
                if(aDataConsumer)
                    {
                    TRAP( exp, ret = WriteTEFSharedDataL(aDataConsumer, commandLength) );
                    ret = ((exp != KErrNone) ? exp : ret);
                    }
				break;
			case ('|'):
				//emulator restart
				HAL::Set(HAL::ECustomRestart,1);
				break;
			case ('3'):
				TRAP( exp, (ret = ProgramStatusL(command, aDataSupplier)));
				ret = ((exp != KErrNone) ? exp : ret);
				break;
			case ('~'):
				TRAP( exp, (ret = StopProgramL(command)) );
				ret = ((exp != KErrNone) ? exp : ret);
				break;
				
#ifndef LIGHT_MODE
			case ('+'):	// install command
				//modify the name in case doesn't contain drive
				//if the filname starts with a back slash.
//				if(	command.Find(KFileSeparator)== 0 )
//					{
//					//replace the filename with one refering to system drive
//					command.Delete(0,1);
//					command.Insert(0, iSystemDrive);
//					}
				updatePathWithSysDrive(command) ; 
				ret = CAppInstall::Install(command);
				break;	
			case ('-'):	// uninstall command
				ret = CAppInstall::Uninstall(command);
				break;	
#endif // ifndef LIGHT_MODE
			
			default:
				ret = KErrNotSupported;     
			}
		}

	// put the error code into the response
	if(ret != KErrNone)
		{
		if(*aDataSupplier)
			{
			(*aDataSupplier)->Delete( );
			*aDataSupplier = NULL;
			}
		AppendErrorCode(ret, aDataSupplier);
		}

	return ret;
}

//------------------------------------------------------------------------------
//
// Initialise our command handling status.
//
void CStatApiCommandDecoder::RefreshStatus(void)
{
	iName = 1;
}


//------------------------------------------------------------------------------
//
// Delete a file once we've finished with it
//
TInt CStatApiCommandDecoder::DeleteLastFile()
{
	TInt exp = KErrNone;
	command = filename;
	TRAP( exp, DeleteFileL(FALSE) );
	return exp;
}


//------------------------------------------------------------------------------
// Function used to decode data received and send text to an app
// We need to avoid using 'command' buffer because we don't know
// how big the actual data will be whereas the other commands all fit
// easily into the 1024 length limit...
//
TInt CStatApiCommandDecoder::SendText(
		MDataConsumer *const aDataConsumer, int commandLength)
{
	// Check we have a window server because it is needed by
	// DoKeyEvent.
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt err = KErrNone;
	TInt i = 0;

	HBufC8	*keyData = HBufC8::New( commandLength );

	if( ! keyData )
		{
		err = KErrNoMemory;
		}

	if( err == KErrNone )
		{
		err = aDataConsumer->GetData( *keyData );
		}

	if( err == KErrNone )
		{
		TBuf16<1> x;

		for(i=0;i<commandLength;i++)
			{
			x.Zero();
			x.Append((*keyData)[i]);
			DoKeyEvent(static_cast<TInt>(x[0]));

			// Do not delay this thread for any time but simply
			// give up the rest of our time-slice to allow
			// the system to process the key event.
			User::After(0);
			}

		// Yield here to give the OS time to actually complete this command.
		User::After(0);
		}

	if( keyData )
		{
		delete keyData;
		keyData = NULL;
		}

	return (err);
}


//------------------------------------------------------------------------------
//
//Function used to decode data received and send system keys to an app (ie - Menu, LeftArrow etc)
//

TInt CStatApiCommandDecoder::SendSystemKeys()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt ret = KErrArgument;
	TInt i = 0;
	TBuf<140> scancodevalue = ENUM_TEXTSCANCODEArray[i];	//scancode strings stored here
	TBuf<100> keycodevalue = ENUM_TEXTKEYArray[i];			//keycode strings entered here

	// look for correct system command from scancodes first
	while (scancodevalue.Compare(TPtrC(NO_MORE_SCANCODEKEYS)) != 0)
	{
		if (command.Compare(scancodevalue) == 0)
		{
			//send using SimulateRawEvent...
			TRawEvent rawEventa;
			rawEventa.Set(TRawEvent::EKeyDown, ENUM_VALSCANCODEArray[i]);
			iWs.SimulateRawEvent(rawEventa);
			iWs.Flush();
			
			rawEventa.Set(TRawEvent::EKeyUp, ENUM_VALSCANCODEArray[i]);
			iWs.SimulateRawEvent(rawEventa);
			iWs.Flush();

			ret = KErrNone;
			break;
		}
			
		i++;	
		scancodevalue = ENUM_TEXTSCANCODEArray[i];
	}

	//if there is no scancode then now try the remaining key codes...
	if(ret != KErrNone)
	{
		i = 0;	//reinitialise
		
		while (keycodevalue.Compare(TPtrC(NO_MORE_KEYS)) != 0)
		{
			if (command.Compare(keycodevalue) == 0)
			{
				// set the event
				DoKeyEvent(ENUM_VALKEYArray[i]);
				ret = KErrNone;
				break;
			}
				
			i++;	
			keycodevalue = ENUM_TEXTKEYArray[i];
		}
	}

	return (ret);
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}


//------------------------------------------------------------------------------
//
//Function used to decode data received and send combination keys to the device (ie - Ctrl+S)
//

TInt CStatApiCommandDecoder::SendCombinationKeys()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt plusposition = 0;					//used for combination key data
	TInt i = 0;
	TInt counter = 0;
	TInt index = 0;

	TBuf<100> input = _L("");
	
	TBuf<20> myModifierKey[4];		//there are 4 maximum combination keys that can be used for one command - they will be stored here
	
	TKeyEvent event;		//the key event structure
	TEventModifier em[5];	//modifier keys required for each of the modifiers available (shift, ctrl, func)
	TKeyCode kp = EKeyNull;
	
	//*****Initialisation section*****
	
	int a = 0;
	for (a=0;a<4;a++)
	{
		myModifierKey[a] = _L("");
		myModifierKey[a].SetLength(0);
	}

	int b = 0;
	for (b=0;b<5;b++)
	{
		em[b] = (TEventModifier)0;
	}
	
	//********************************
	
	TInt DataLength = command.Length();	//copy passed data length into local variable

	//The following section splits the data up and puts them into the array
	while ((DataLength >= 1) && (counter <= 4))		//while length > 1
	{
		plusposition = command.Locate('+');	//search for this char

		if (plusposition != KErrNotFound)
		{
			myModifierKey[counter] = command.Left(plusposition);	//chars left of plus are stored
			command.Delete(0, (plusposition + 1));					//delete all chars from pos 0 up to and including '+' with null terminator
			counter++;
		}
		else	//no more '+' in string
		{
			myModifierKey[counter] = command.Left(command.Length());//store remaining string into array
			command.Delete(0, plusposition + 1);					//delete remaining string
			counter = 5;										//make sure exits
		}
	}
	
	counter = 0;
	event.iCode = 0;
	event.iScanCode = 0;
	event.iRepeats = 0;
	event.iModifiers = 0;	//initialisation
	
	//now check the new data in each of the elements in the array
	while (counter <= 4)
	{
		input = myModifierKey[counter];	//assign value from string

		if (input == _L("Ctrl")  || input == _L("LeftCtrl")  || input == _L("RightCtrl") 
			|| input == _L("Shift") || input == _L("LeftShift") || input == _L("RightShift") 
			|| input == _L("Func") || input == _L("LeftFunc") ||  input == _L("RightFunc") 
			|| input == _L("Alt") ||  input == _L("LeftAlt") || input == _L("RightAlt"))
		{
			TBuf<100> value;
			value = ENUM_TEXTMODIFIERSArray[i];	//assign initial data in array to value (element 0)

			while (value.Compare(TPtrC(NO_MORE_MODIFIERKEYS)) != 0)	//make sure not end of array
			{
				if (input.Compare(value) == 0)	//if the string has been found in the text array
				{
					em[counter] = ENUM_VALMODIFIERSArray[i];	//assign em element with the same value from the val array by comparing counter num (location)
					i = 0;	//reinitialise so that the search can restart at the beginning of the enumerated type list
					break;
				}
				
				i++;
				value = ENUM_TEXTMODIFIERSArray[i];	//update contents of value for the next pass
			}	//end while
		}
		else	//no combination keys left
		{
			if (input.Length() > 1)	//if the remaining data is longer than 1 char (ie - could be 'LeftArrow')
			{
				TBuf<100> valuekey;
				valuekey = ENUM_TEXTKEYArray[i];	//assign initial data in array to value (element 0)

				while (valuekey.Compare(TPtrC(NO_MORE_KEYS)) != 0)	//make sure not end of array
				{
					if (input.Compare(valuekey) == 0)	//if the string has been found in the text array
					{
						//kp.iScanCode = ENUM_VALKEYArray[i]; 
						kp = ENUM_VALKEYArray[i]; //assign kp with the same value from the val array

						if(kp < ENonCharacterKeyBase)
						{
							event.iCode = kp;
							em[2] = EModifierPureKeycode;
							index = 3;
							counter = 5;
						}
						else
						{
							event.iCode = kp;
							index = 3;
							counter = 5;
						}

						break;

					}	//end if
				
					i++;	
					valuekey = ENUM_TEXTKEYArray[i];	//update contents of value for the next pass
				}
			}
			else	//must be single char combination, so pass into iCode and set counter to four to exit!
			{
				input.LowerCase();

				while (index < 3)	//need to check array and see if need to send CTRL or not
				{
					if ((em[index] == EModifierLeftCtrl) || (em[index] == EModifierRightCtrl) || (em[index] == EModifierCtrl))
					{
						event.iCode = CTRLA(input.Ptr()[0]);
						counter = 4;
						break;
					}
					else
					{
						event.iCode = input.Ptr()[0];
						index++;
						counter = 4;
					}
				}
			}

			event.iModifiers = em[0] | em[1] | em[2];	//used to set up Ctrl+Shift+...etc
		
		}	//end else

		counter++;
		input = _L("");	//reinitialise for next pass
	
	}	//end while


	iWs.SimulateKeyEvent(event);		// send keystroke
	iWs.Flush();						// flush client-side window server buffer

	return KErrNone;
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//Function used to hold down keys
//
////////////////////////////////////////////////////////////////////////////////////////////
TInt CStatApiCommandDecoder::ConvertKeycodeToScancode( TDesC& key, TInt& scancode, TInt& modifiers )
{
	TInt i, j;
	TBuf<20> base_punctuation( _L("`,./\\;'#[]-=/*-") );
	TBuf<20> shift_punctuation( _L("?>?|:@~{}_+XX_") );
	TBuf<20> numeric_punctuation( _L(")!\"?%^&*(") );

	// clear the modifiers field
	modifiers = 0;

	// make sure the key is not completely empty
	if( key.Length() == 0 ) {
		return -1;
	}

	// If the key is longer than one char then it must be a special key that we need to
	// lookup in the table. If it's not in the table then return error
	if( key.Length() > 1 ) {
		for( i = 0; ENUM_TEXTSCANCODEArray[i] != NO_MORE_SCANCODEKEYS; i++ ) {
			if( key.Compare(TPtrC(ENUM_TEXTSCANCODEArray[i])) == 0 ) {
				scancode = ENUM_VALSCANCODEArray[i];
				return 0;
			}
		}
		return -1;
	}

	// get the first key (saves getting it heaps of times)
	TChar keycode( key.Ptr()[0] );

	// handle lowercase alpha characters
	if( keycode.IsAlpha() && keycode.IsLower() ) {
		keycode.UpperCase();
		scancode = keycode;
		return 0;
	}

	// handle uppercase alpha characters
	if( keycode.IsAlpha() && keycode.IsUpper() ) {
		scancode = keycode;
		modifiers = ESMLShift;
		return 0;
	}

	// handle numbers
	if( keycode.IsDigit() ) {
		scancode = keycode;
		return 0;
	}

	// handle punctuation -- just offset by 36 for some unknown reason
	if( keycode.IsPunctuation() ) {

		j = base_punctuation.Locate( keycode );
		if( j != KErrNotFound ) {
			scancode = 120 + j;
			return 0;
		}

		j = shift_punctuation.Locate( keycode );
		if( j != KErrNotFound ) {
			scancode = 120 + j;
			modifiers = ESMLShift;
			return 0;
		}

		j = numeric_punctuation.Locate( keycode );
		if( j != KErrNotFound ) {
			scancode = '0' + j;
			modifiers = ESMLShift;
			return 0;
		}

		scancode = keycode;
		return 0;
	}

	// For some reason certain characters are not recognized as punctuation though they
	// should be. These are handled specifically here.
	bool found = false;
	switch( keycode ) {

	case 0x24:
		scancode = 0x34;
		modifiers = ESMLShift;
		found = true;
		break;

	case 0xa3:
		scancode = 0x33;
		modifiers = ESMLShift;
		found = true;
		break;

	case 0x80:
		scancode = 0x34;
//		modifiers = ESMRFunc | ESMLCtrl;
		modifiers = ESMRAlt;
		found = true;
		break;

	case 0xa6:
		scancode = 0x78;
		modifiers = ESMRFunc;
		found = true;
		break;
	}

	if (found)
		return 0;
	else
	// key not found
		return -1;
}

TInt CStatApiCommandDecoder::SendKeyHold()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TLex Lexcounter;
	TInt key_action = 0;
	TRawEvent rawEventa;
	TInt scancodefound;
	TInt scancode;
	TInt modifiers;

	// H commands must have the format <HC,N> where 'C' is the command to be executed and
	// N is either -1 (key up) or 0 (key down)
	TInt commaposition = command.Locate(',');
	if( commaposition == KErrNotFound ) {
		return KErrArgument;
	}

	// convert the key action from a string to an integer	
	Lexcounter = command.Mid( commaposition + 1 );
	Lexcounter.Val( key_action );

	// get the scancode for the key
	command.SetLength( commaposition );
	scancodefound = ConvertKeycodeToScancode( command, scancode, modifiers );
	if( scancodefound == -1 ) {
		return KErrArgument;
	}

	// set the appropriate modifier keys
	if( modifiers & ESMLShift ) {
		rawEventa.Set(TRawEvent::EKeyDown, EStdKeyLeftShift);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMRFunc ) {
		rawEventa.Set(TRawEvent::EKeyDown, EStdKeyRightFunc);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMRAlt ) {
		rawEventa.Set(TRawEvent::EKeyDown, EStdKeyRightAlt);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMLCtrl ) {
		rawEventa.Set(TRawEvent::EKeyDown, EStdKeyLeftCtrl);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}

	// now handle the key_action -1 == key_up, 0 == key_down, > 0 == milliseconds to hold the key down
	switch( key_action ) {

	case -1:
		rawEventa.Set(TRawEvent::EKeyUp, scancode);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
		break;

	case 0:
		rawEventa.Set(TRawEvent::EKeyDown, scancode);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
		break;

	default:
		rawEventa.Set(TRawEvent::EKeyDown, scancode);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();

		key_action *= 1000;
		User::After(TTimeIntervalMicroSeconds32(key_action));
		
		rawEventa.Set(TRawEvent::EKeyUp, scancode);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
		break;
	}

	// release appropriate function keys
	if( modifiers & ESMLShift ) {
		rawEventa.Set(TRawEvent::EKeyUp, EStdKeyLeftShift);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMRFunc ) {
		rawEventa.Set(TRawEvent::EKeyUp, EStdKeyRightFunc);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMRAlt ) {
		rawEventa.Set(TRawEvent::EKeyUp, EStdKeyRightAlt);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}
	if( modifiers & ESMLCtrl ) {
		rawEventa.Set(TRawEvent::EKeyUp, EStdKeyLeftCtrl);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
	}

	return KErrNone;
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}


//------------------------------------------------------------------------------
//
//Function used to start application(s)
//
TInt CStatApiCommandDecoder::StartApplicationL()
{	
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt ret = KErrNone;

	TBuf<KMaxFileName> docName;
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

	TInt commaposition = command.Locate(',');
	if (commaposition != KErrNotFound)
	{
		// set the application name
		cmdLine -> SetExecutableNameL(command.Mid(0, commaposition));

		// use document name supplied in command string
		docName = command.Mid(commaposition + 1);
	}
	else
	{
		TBuf<10> timestring;
		TTime DateAndTime;
		DateAndTime.HomeTime();
		DateAndTime.FormatL(timestring, (_L("%J%T%S%*B")));

		// create a default document name based on the system date and time
		
		docName.Append(iSystemDrive);
		docName.Append(timestring);

		// set the application name
		cmdLine -> SetExecutableNameL(command);
	}

	cmdLine -> SetDocumentNameL(docName);
	cmdLine -> SetCommandL(EApaCommandCreate);

	if( ret == KErrNone )
		{
		//initialise
		ret = iApaS.Connect();
		if(ret != KErrNone)
			pMsg->Msg (_L("DECODER: Application Architecture server error [%c]"), ret);
		}

	if( ret == KErrNone )
		{
		// startup
		ret = iApaS.StartApp(*cmdLine);
		if(ret != KErrNone)
			pMsg->Msg (_L("DECODER: Unable to start application [%c]"), ret);
		}

	//cleanup
	iApaS.Close();
	CleanupStack::PopAndDestroy();

	if( ret == KErrNone )
		{
		// need to sleep a bit here to give the OS time to actually start the application
		User::After(3000000);
		}

	return (ret);
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//
//Function used to start executables(s)
//
TInt CStatApiCommandDecoder::StartProgramL(MDataSupplier **aDataSupplier)
{	
	TInt ret = KErrNone;

	TBuf<KMaxFileName> programName;
	TBuf<KMaxFileName> parameters;

	TInt commaposition = command.Locate(',');
	if (commaposition != KErrNotFound)
	{
		// set the program name
		programName = command.Mid(0, commaposition);

		// set the parameters
		parameters = command.Mid(commaposition + 1);
	}
	else
	{
		programName = command;
	}

	updatePathWithSysDrive(programName) ; 
	updatePathWithSysDrive(parameters) ; 
	RProcess newProcess;
	ret = newProcess.Create(programName,parameters);
	
	if(ret == KErrNone)
		{
		
		TProcessId handle = newProcess.Id();
		
		if(ret == KErrNone)
		{
			newProcess.Resume();
			newProcess.Close(); // get rid of our handle
		}
		
		// Return the PID
		
		CDataSupplierMemory	*pDataSupplier = CDataSupplierMemory::NewL( );
		
		TBuf8<50> handleNumber;
		
		handleNumber.Num(handle.Id());
		
		TInt err;

		if (pDataSupplier)
			{
			// Set the data in the data supplier.
			err = pDataSupplier->SetData( handleNumber );

			if (err != KErrNone)
				{
				pMsg->Msg(_L("Failed to set data in data supplier") );
				}
			}
		else
			{
			err = KErrNoMemory;
			pMsg->Msg(_L("Failed to create data supply object") );
			}

		if (err == KErrNone)
			{
			*aDataSupplier =	pDataSupplier;
			}
		else
			{
			if (pDataSupplier)
				{
				pDataSupplier->Delete( );
				pDataSupplier = NULL;
				}
			}
		}
	
	return ret;
}

//------------------------------------------------------------------------------
//
//Function used to close application in foreground
//
void CStatApiCommandDecoder::StopApplicationL()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt AppIdentifier = 0;
	TThreadId ThreadIdentifier;

	AppIdentifier = iWs.GetFocusWindowGroup();	//retrieves identifier of the window group that currently has the keyboard focus
	
	TInt ret = iWs.GetWindowGroupClientThreadId(AppIdentifier, ThreadIdentifier);
	if(ret == KErrNone)
	{
		TInt prev = 0;
		TInt winGid = iWs.FindWindowGroupIdentifier(prev, ThreadIdentifier); //Get identifier by thread ID
		
		TApaTask task(iWs);
		task.SetWgId(winGid);
		
		if (task.Exists())
		{
			CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(iWs, winGid);

			// check if application is busy
			if (wgName -> IsSystem() || wgName -> IsBusy() || !wgName -> RespondsToShutdownEvent())
				ret = KErrAccessDenied;

			CleanupStack::PopAndDestroy(); // wgName
			
			if(ret == KErrNone)
				task.SendSystemEvent(EApaSystemEventShutdown);	//close app
		}
		else
		{
			ret = KErrNotFound;	//return error if task is non-existent
		}
	}

	User::Leave(ret);
#endif // ifndef LIGHT_MODE
}

//
//Function used to close application in foreground
//
void CStatApiCommandDecoder::StopEShellL()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt AppIdentifier = 0;
	TThreadId ThreadIdentifier;

	AppIdentifier = iWs.GetFocusWindowGroup();	//retrieves identifier of the window group that currently has the keyboard focus
	
	TInt ret = iWs.GetWindowGroupClientThreadId(AppIdentifier, ThreadIdentifier);
	if(ret == KErrNone)
	{
		TInt prev = 0;
		TInt winGid = iWs.FindWindowGroupIdentifier(prev, ThreadIdentifier); //Get identifier by thread ID
		
		TApaTask task(iWs);
		task.SetWgId(winGid);
		
		if (task.Exists())
		{
			CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(iWs, winGid);

			// check if application is busy
			if (wgName -> IsSystem())
				ret = KErrAccessDenied;
			else
				{
				RThread newThread;
				RProcess newProcess;

				ret = newThread.Open(ThreadIdentifier);
				if(ret == KErrNone)
					{
					ret = newThread.Process(newProcess);
					if(ret == KErrNone)
						{
                        if(newProcess.FileName().Right(10).CompareF(_L("eshell.exe")) == 0)
                            {
                            newProcess.Kill(1);
                            }
                        else
                            {
                            ret = KErrNotFound;
                            }
                        newProcess.Close();	
						}
					newThread.Close();
					}
				}

			CleanupStack::PopAndDestroy(); // wgName
		}
		else
		{
			ret = KErrNotFound;	//return error if task is non-existent
		}
	}

	User::Leave(ret);
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//
//Function used to read some TEF shared data
//

TInt CStatApiCommandDecoder::ReadTEFSharedDataL( MDataSupplier **aDataSupplier )
	{
	TInt ret = KErrNone;
	
	TBuf<KMaxSharedDataName>	sharedDataName;

	// Set the shared data name
	TInt length = command.Length();
	if( length > 0 && length < KMaxSharedDataName )
		{
		sharedDataName.Zero();
		sharedDataName.Copy( command );	
		sharedDataName.ZeroTerminate();
		}
	else
		{
		ret = KErrArgument;
		}
	
	// Now read the shared data area
	if( ret == KErrNone )
		{
		// Read the shared data from the global RChunk
		HBufC*	buffer = NULL;
		CTestSharedData* sharedDataObject = NULL;
		CTEFSharedData<CTestSharedData> *tefSharedData = CTEFSharedData<CTestSharedData>::NewLC(	sharedDataObject,
																									sharedDataName );
		if( sharedDataObject != NULL )
			{
			tefSharedData->EnterCriticalSection();
			TRAP( ret, buffer = HBufC::NewL(sharedDataObject->TextLength()) );
			if( ret == KErrNone )
				{
				TPtr bufferPtr( buffer->Des() );
				sharedDataObject->GetText( bufferPtr );
				tefSharedData->ExitCriticalSection();

				// Create the response data supplier
				CDataSupplierMemory	*pDataSupplier = NULL;
				TRAP( ret, pDataSupplier = CDataSupplierMemory::NewL() );
				if( pDataSupplier && ret == KErrNone )
					{
					HBufC8 *buffer8 = NULL;
					TRAP( ret, buffer8 = HBufC8::NewL(buffer->Length()) );
					if( ret == KErrNone )
						{
						TPtr8 buffer8Ptr( buffer8->Des() );
						buffer8Ptr.Copy( buffer->Des() );
						
						// Set the data in the data supplier.
						ret = pDataSupplier->SetData( *buffer8 );

						// Cleanup
						delete buffer8;
						}

					if( ret != KErrNone )
						{
						pMsg->Msg(_L("Failed to set data in data supplier") );
						}
					}
				else
					{
					ret = KErrNoMemory;
					pMsg->Msg(_L("Failed to create data supply object") );
					}

				if (ret == KErrNone)
					{
					*aDataSupplier =	pDataSupplier;
					}
				else
					{
					if (pDataSupplier)
						{
						pDataSupplier->Delete( );
						pDataSupplier = NULL;
						}
					}
				}

			delete buffer;
			}
		else
			{
			ret = KErrNotFound;
			}
		CleanupStack::PopAndDestroy(tefSharedData);
		}

	return ret;
	}

//------------------------------------------------------------------------------
//
//Function used to write some TEF shared data
//

TInt CStatApiCommandDecoder::WriteTEFSharedDataL( MDataConsumer *const aDataConsumer, int commandLength )
	{
	TInt ret = KErrNone;
	
	// Retrieve the data
	if( commandLength <= KMaxSharedDataString )
		{
		HBufC8* sharedDataString = HBufC8::NewLC( commandLength );
		if( aDataConsumer != NULL )
			{
			ret = aDataConsumer->GetData( *sharedDataString );

			// Set the shared data name
			if( ret == KErrNone )
				{
				TBuf<KMaxSharedDataName>	sharedDataName;
				TInt commaposition = sharedDataString->Locate(',');
				if( commaposition != KErrNotFound )
					{
					if( commaposition < KMaxSharedDataName )
						{
						sharedDataName.Zero();
						sharedDataName.Copy(sharedDataString->Mid(0, commaposition));
						sharedDataName.ZeroTerminate();

						// Set the shared data value
						HBufC* sharedDataValue = HBufC::NewLC(commandLength-commaposition+1);
						TPtr ptrSharedDataValue(sharedDataValue->Des());
						ptrSharedDataValue.Zero();
						ptrSharedDataValue.Copy(sharedDataString->Mid(commaposition+1));
						ptrSharedDataValue.ZeroTerminate();

						// Now update the shared data area
						if( ret == KErrNone )
							{
							// Update the shared data with the data passed in
							CTestSharedData* sharedDataObject = NULL;
							CTEFSharedData<CTestSharedData> *tefSharedData = CTEFSharedData<CTestSharedData>::NewLC(	sharedDataObject,
																													sharedDataName );
							if( sharedDataObject != NULL )
								{
								tefSharedData->EnterCriticalSection();
								// Copy the data contents across
								// No need to check the data length as it was checked above
								sharedDataObject->SetText( *sharedDataValue );
								tefSharedData->ExitCriticalSection();
								}
							else
								{
								ret = KErrNotFound;
								}

							CleanupStack::PopAndDestroy(tefSharedData);
							}
						
						// Cleanup
						CleanupStack::PopAndDestroy(sharedDataValue);	
						}
					else
						{
						ret = KErrArgument;
						}
					}
				else
					{
					ret = KErrArgument;
					}
				}			
			}
		else
			{
			ret = KErrArgument;
			}
		CleanupStack::PopAndDestroy(sharedDataString);
		}
	else
		{
		ret = KErrTooBig;
		}
	
	return ret;
	}

//------------------------------------------------------------------------------
//
//Function used to open file(s)
//

TInt CStatApiCommandDecoder::OpenFileL()
{
#ifndef LIGHT_MODE
	TInt ret = KErrArgument;

	TBuf<KMaxFileName> docName;
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

	TInt commaposition = command.Locate(',');
	if (commaposition != KErrNotFound)
	{
		// set the application name
		cmdLine -> SetExecutableNameL(command.Mid(0, commaposition));

		// use document name supplied in command string
		docName = command.Mid(commaposition + 1);

		updatePathWithSysDrive(docName);
		cmdLine -> SetDocumentNameL(docName);
		cmdLine -> SetCommandL(EApaCommandOpen);

		//initialise
		TRAP(ret, iApaS.Connect())
		if(ret != KErrNone)
			pMsg->Msg(_L("DECODER: Application Architecture server error...%d"), ret);

		// startup
		TRAP(ret, iApaS.StartApp(*cmdLine))
		if(ret != KErrNone)
			pMsg->Msg(_L("DECODER: Unable to start application...%d"), ret);

		//cleanup
		iApaS.Close();

		// need to sleep a bit here to give the OS time to actually start the application
		User::After(3000000);
	}

	CleanupStack::PopAndDestroy();
	return ret;
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}
//------------------------------------------------------------------------------
//
//Function used to capture screens
//

void CStatApiCommandDecoder::ScreenCaptureL()
{
#ifndef LIGHT_MODE
		
	//_LIT(KDriveD,  "d:\\");

	// Optimize screen captures by only creating device objects 
	// once and re-using them.
	// Create screen device object is now done in 
	// CStatApiCommandDecoder::ConstructL and removed 
	// in CStatApiCommandDecoder::~CStatApiCommandDecoder().

    User::LeaveIfError(iDevice->CopyScreenToBitmap(screenBitmap));

	// retrieve machine id information
	TBuf<20> buffer;
    buffer.Num(iUIDValue);

    TBuf<KMaxFileName> tempfilename;
    
    tempfilename.Append(iSystemDrive); 
	tempfilename.Append(buffer);
	tempfilename.Append(_L("00"));				//substation id

	// create date/time string for filename
	TBuf<25> datestring;
	TTime DateAndTime;
	DateAndTime.HomeTime();
	DateAndTime.FormatL(datestring, (_L("%D%M%*Y%1%2%3%J%T%S")));
	tempfilename.Append(datestring);

	//bitmap image extension
	tempfilename.Append(_L(".mbm"));

	// save the image to file
    User::LeaveIfError(screenBitmap->Save(tempfilename));
    {
		filename = tempfilename;
    }
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//function used to simulate a screen press using stylus

TInt CStatApiCommandDecoder::OpenIcon()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TInt ret = KErrArgument;

	TInt commaposition = command.Locate(',');
	if (commaposition != KErrNotFound)
	{
		// get x coord
		TInt x = 0;
		TLex Lexcoord = command.Mid(0, commaposition);
		Lexcoord.Val(x);

		// get y coord
		TInt y = 0;
		Lexcoord = command.Mid(commaposition + 1);
		Lexcoord.Val(y);

		//mouse click down
		TRawEvent rawEventa;
		rawEventa.Set(TRawEvent::EButton1Down, x, y);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
		User::After(TTimeIntervalMicroSeconds32(100000));

		//mouse click up
		rawEventa.Set(TRawEvent::EButton1Up, x, y);
		iWs.SimulateRawEvent(rawEventa);
		iWs.Flush();
		User::After(TTimeIntervalMicroSeconds32(100000));
		ret = KErrNone;
	}

	return (ret);
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}


//------------------------------------------------------------------------------
//function used to invoke the ESHELL command interpreter
//
// This is done alternatively to starting an app because the ESHELL
// cannot be invoked from it's current location in z:
//
// So we copy it first to C: then start it...
//
void CStatApiCommandDecoder::StartEShellL()
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	_LIT(KNewFile, "z:\\sys\\bin\\EShell.exe");
	

	// set command line
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();	//creation of command line structure
	cmdLine -> SetExecutableNameL(KNewFile);
	
	// call it
	User::LeaveIfError(iApaS.Connect());					//initialise
	User::LeaveIfError(iApaS.StartApp(*cmdLine));			//start the app
	iApaS.Close();
	
	// clean up
	CleanupStack::PopAndDestroy(1);	// cmdLine

	// need to sleep a bit here to give the OS time to actually start the application
	User::After(3000000);

	User::Leave(KErrNone);
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//function used to delete file from the device
//
TInt CStatApiCommandDecoder::DeleteFileL(TInt bDisplayMessage)
{
	CFileMan *theFile = CFileMan::NewL(*iFs);
	CleanupStack::PushL(theFile);

	updatePathWithSysDrive(command) ; 
	TInt err = theFile->Delete(command);
	if (err!= KErrNone && err!= KErrNotFound)
	{
		if (bDisplayMessage)
			pMsg->Msg(_L("DECODER: Error deleting file %S...%d"), &command, err);
	}
	
	CleanupStack::PopAndDestroy();
	return err;
}

//------------------------------------------------------------------------------
//function used to rename file from the device
//
TInt CStatApiCommandDecoder::RenameFileL(MDataConsumer *const aDataConsumer)
{

	TInt err = KErrNone;

	CFileMan *theFile = CFileMan::NewL(*iFs);
	CleanupStack::PushL(theFile);

    TBuf<KMaxPath>	dataFiles;

	TBuf<KMaxPath>	dataFileNameFrom;
	TBuf<KMaxPath>	dataFileNameTo;

	dataFiles.Copy( *aDataConsumer );

	if( err == KErrNone )
		{
		// locate the destination part of the filename
		TInt commaposition = dataFiles.Locate(',');
		if (commaposition != KErrNotFound)
			{
			//name of existing file
			dataFileNameFrom.Zero();
			dataFileNameFrom.Copy(dataFiles.Left(commaposition));
			filename.ZeroTerminate();
				
			//name of destination file
			dataFileNameTo.Zero();
			dataFileNameTo.Copy(dataFiles.Right(dataFiles.Length()-commaposition-1));
			dataFileNameTo.ZeroTerminate();
			
			updatePathWithSysDrive(dataFileNameFrom) ; 
			updatePathWithSysDrive(dataFileNameTo) ; 
			err = theFile->Copy( dataFileNameFrom, dataFileNameTo );
				
			if( err == KErrNone )
				{
				err = theFile->Delete(dataFileNameFrom);	
				}
				
			}
		else
			{
			err = KErrNotFound;
			}
		}

	CleanupStack::PopAndDestroy();
	return err;
}



//------------------------------------------------------------------------------
//function used to check if a location exists on the device
//
TInt CStatApiCommandDecoder::CheckLocation(MDataConsumer *const aDataConsumer)
{

	TInt err = KErrNone;
	TEntry aEntry;
	TVolumeInfo aVolume;
	TInt aDrive;
	TBuf<KMaxPath> aPath;
	
	aPath.Copy(*aDataConsumer);
	updatePathWithSysDrive(aPath);
		
 	iFs->CharToDrive((char)aPath[0],aDrive);

	//try to get drive info
	err = iFs->Volume(aVolume,aDrive);
	
	if(err == KErrNone && aPath.Length() > 3)
		{
		//try to get file info
		err = iFs->Entry(aPath,aEntry);
		}
	
	return err;
	
}




//------------------------------------------------------------------------------
//function used to create a folder on the device
//
TInt CStatApiCommandDecoder::CreateFolder()
{
	updatePathWithSysDrive(command);
	TInt err = iFs->MkDirAll(command);

	// no error or already exists are acceptable return codes
	if (err == KErrAlreadyExists)
		err = KErrNone;

	return err;
}

//------------------------------------------------------------------------------
//function used to remove a folder from the device
//
TInt CStatApiCommandDecoder::RemoveFolder()
{

	updatePathWithSysDrive(command) ; 
	TInt err = iFs->RmDir(command);

	// no error or not found are acceptable return codes
	if (err == KErrNotFound)
		err = KErrNone;

	return err;
}

//------------------------------------------------------------------------------

TInt CStatApiCommandDecoder::TransferFile(MDataConsumer *const aDataConsumer)
{
	TInt err = KErrNone;

	// the Transfer command comes in 2 parts - first the name, then the contents
	if (iName)
		{
		pMsg->Msg(_L("DECODER: Saving filename..."));

		TBuf<KMaxPath>	dataFileName;
		dataFileName.Copy(*aDataConsumer ) ; 
		updatePathWithSysDrive(dataFileName) ; 
		
		if( err == KErrNone )
			{
			// locate the destination part of the filename
			TInt commaposition = dataFileName.Locate(',');
			if (commaposition != KErrNotFound)
				{
				// use document name supplied in command string
				filename.Zero();
				filename.Copy(dataFileName.Mid(commaposition + 1));
				filename.ZeroTerminate();
				pMsg->Msg(_L("DECODER: File name (%S)."), &filename);
				iName = 0;
				err = KErrNone;
				}
			}
		}
	else
		{
		// save the file
		pMsg->Msg(_L("DECODER: Saving file %S"), &filename);

		if( aDataConsumer != NULL )
			{
			err = aDataConsumer->SaveData( filename );
			}
			else
			{
			CDataConsumerMemory	*dataConsumer = NULL;
			TRAP(err, dataConsumer = CDataConsumerMemory::NewL( ));
			
			if( err == KErrNone )
				{
				err = dataConsumer->SaveData( filename );
				}

			if( NULL != dataConsumer )
				{
				dataConsumer->Delete();
				dataConsumer = NULL;
				}
			}

		iName = 1;
		}

	return err;
}


//------------------------------------------------------------------------------

TInt CStatApiCommandDecoder::ReceiveFile(TUint commandtype, 
		MDataConsumer *const aDataConsumer, MDataSupplier **aDataSupplier)
{
	TInt err = KErrNone;

	// take screenshot first?  filename will be generated automatically
	if (commandtype == 'S')
	{
		// If this assert triggers it is because we are running in
		// text shell mode without a window server and the call
		// requires the window server to be available.
		// A command filter in ExecuteCommand should have prevented
		// this call from being made.
		assert(IsWindowServerAvailable());

		TRAP(err, ScreenCaptureL());
	}
	else
	{
		TPath	dataFileName;

		dataFileName.Copy( *aDataConsumer );
		updatePathWithSysDrive(dataFileName) ; 
		if( err == KErrNone )
			{
			// get the filename from the command
			TInt commaposition = dataFileName.Locate(',');
			if (commaposition != KErrNotFound)
				filename.Copy(dataFileName.Mid(0, commaposition));
			else
				filename.Copy(dataFileName);
			}
	}

	MDataSupplier	*pDataSupplier = NULL;

	// Use a data supplier class.
	// Initialise it with the file name and allocate
	// an initial data buffer.

	if (err == KErrNone)
	{
		// Create the data supplier.
		pDataSupplier = CDataSupplierFile::NewL( );

		if (pDataSupplier)
		{
			// Set the data in the data supplier.
			TBuf8<KMaxPath>	filePath;
			filePath.Copy( filename );

			err = pDataSupplier->SetData( filePath );

			if (err != KErrNone)
			{
				TBuf16<KMaxPath>	wFilePath;
				wFilePath.Copy( filePath );
				pMsg->Msg(_L("Failed to set data in data supplier, (%d)."), err );
				pMsg->Msg(_L("File path %S."), &wFilePath );
			}
		}
		else
		{
			err = KErrNoMemory;
			pMsg->Msg(_L("Failed to create data supply object") );
		}
	}

	// If we are sending a screen image file then we 
	// use another data supplier class as the file is to be deleted.
	if (commandtype == 'S')
	{
		TInt	size = 0;
		HBufC8	*buffer = NULL;

		if (err == KErrNone)
		{
			err = pDataSupplier->GetTotalSize( size );

			if( err != KErrNone )
			{
				pMsg->Msg(_L("Failed to get total size of screen image file") );
			}
		}

		if (err == KErrNone)
		{
			buffer = HBufC8::New(size);

			if( ! buffer )
			{
				err = KErrNoMemory;
				pMsg->Msg(_L("Failed to allocate buffer for screen image file") );
			}
		}

		if (err == KErrNone)
		{
			err = pDataSupplier->GetData( *buffer, size, size );

			if (err != KErrNone)
			{
				pMsg->Msg(_L("Failed to read data from screen image data supplier") );
			}
		}

		if (err == KErrNone)
		{
			pDataSupplier->Delete( );

			pDataSupplier = CDataSupplierMemory::NewL( );

			if( ! pDataSupplier )
			{
				err = KErrNoMemory;
				pMsg->Msg(_L("Failed to create data supply object") );
			}
		}

		if (err == KErrNone)
		{
			err = pDataSupplier->SetData( *buffer );

			if (err != KErrNone)
			{
				pMsg->Msg(_L("Failed to set data in data supplier") );
			}
		}
	
		if( NULL != buffer )
		{
			delete buffer;
			buffer = NULL;
		}
	}

	if (err == KErrNone)
		{
		*aDataSupplier =	pDataSupplier;
		}
	else
		{
		if (pDataSupplier)
			{
			pDataSupplier->Delete( );
			pDataSupplier = NULL;
			}
		}

	return err;
}


//------------------------------------------------------------------------------

TInt CStatApiCommandDecoder::DeviceInfo(MDataSupplier **aDataSupplier)
{
	TBuf8<20> buffer = _L8("");
	TBuf8<150> finalbuffer = _L8("\r\n");

	TInt DeviceInfoValue;
	TInt err = KErrNone;
	TInt i = 0;

	//device info required: UID, manufacturer, ROM, RAM, etc
	HALData::TAttribute AttribArray[14] =	{
											 HALData::EMachineUid, 
											 HALData::ECPU,
											 HALData::ECPUABI,
											 HALData::EDeviceFamily, 
											 HALData::EManufacturer,

											 HALData::EModel, 
											 HALData::EDeviceFamilyRev, 
											 HALData::EManufacturerHardwareRev,
											 HALData::EManufacturerSoftwareRev,
											 HALData::EManufacturerSoftwareBuild,
											 HALData::EMemoryPageSize,
											 HALData::EMemoryRAM,
											 HALData::EMemoryRAMFree,
											 HALData::EMemoryROM
											};

	finalbuffer.Append(_L("STAT Version "));
	
	buffer.Num(KVersionMajor);
	finalbuffer.Append(buffer);
	finalbuffer.Append(_L("."));
	
	buffer.Num(KVersionMinor);
	finalbuffer.Append(buffer);
	finalbuffer.Append(_L("."));
	
	buffer.Num(KVersionPatch);
	finalbuffer.Append(buffer);
	finalbuffer.Append(_L("\r\n"));
		
	while(i < 14)	//retrieve all the information and write to a buffer
	{
		// get the attribute value
		HAL::Get(AttribArray[i], DeviceInfoValue);
		buffer.Num(DeviceInfoValue);

		// need to convert the value to a meaningful string
		if (i < 5)
		{
			GetData(i, buffer);
		}
			
		finalbuffer.Append(buffer);		
		finalbuffer.Append(_L8("\r\n"));
		i++;
	}

	CDataSupplierMemory	*pDataSupplier = CDataSupplierMemory::NewL( );

	if (pDataSupplier)
		{
		// Set the data in the data supplier.
		err = pDataSupplier->SetData( finalbuffer );

		if (err != KErrNone)
			{
			pMsg->Msg(_L("Failed to set data in data supplier") );
			}
		}
	else
		{
		err = KErrNoMemory;
		pMsg->Msg(_L("Failed to create data supply object") );
		}

	if (err == KErrNone)
		{
		*aDataSupplier =	pDataSupplier;
		}
	else
		{
		if (pDataSupplier)
			{
			pDataSupplier->Delete( );
			pDataSupplier = NULL;
			}
		}

	return err;
}

//------------------------------------------------------------------------------

TInt CStatApiCommandDecoder::SaveLogFile(MDataSupplier **aDataSupplier)
{
	TInt err = KErrNone;

	CDataConsumerMemory	*data = NULL;

	// if the log file is not enabled then we return an empty buffer
	if( pMsg->IsInitialised() == EFalse )
		{
		return KErrNone;
		}

	//open the file an copy it into the return buffer
	data = CDataConsumerMemory::NewL( );

	if( ! data )
		{
		err = KErrNoMemory;
		}
	
	if( err == KErrNone )
		{
		pMsg->CloseFile();
		}



	TBuf<KMaxPath>	filePath;
	TBuf<17> statLogFile;
	statLogFile.Append(iSystemDrive);
	statLogFile.Append(KStatLogFile);
	if( err == KErrNone )
		{
		
		RFile	file;

		err = file.Temp( *iFs, iSystemDrive,
							filePath, EFileShareExclusive );

		if( err == KErrNone )
			{
			file.Close( );

	    	CFileMan* fileMan = CFileMan::NewL( *iFs );
	    	err =	fileMan->Copy(statLogFile,filePath);
			delete fileMan;
			fileMan =	NULL;
			}
		}

	if( err == KErrNone )
		{
		TBuf8<KMaxPath>	cFilePath;
		cFilePath.Copy( filePath );
		err = data->AddData( cFilePath );
		}

	if( err == KErrNone )
		{
		err = ReceiveFile('R', data, aDataSupplier);
		}

	
	if( err == KErrNone )
		{
		// reset the logger 
		pMsg->Init( *iFs, statLogFile, NULL );
		}

	if( data )
		{
		data->Delete( );
		data = NULL;
		}

	return err;
}

//------------------------------------------------------------------------------

#ifndef LIGHT_MODE
TInt CStatApiCommandDecoder::GetDiskDriveInformationL(MDataSupplier **aDataSupplier)
#else // ifndef LIGHT_MODE
TInt CStatApiCommandDecoder::GetDiskDriveInformationL(MDataSupplier **)
#endif // ifndef LIGHT_MODE
{
#ifndef LIGHT_MODE
	TInt err = KErrNone;

	// Get the disk drive information and save to the data supplier object
	// as a text string.

	static const TInt	bufferGrow =	(4*1024);
	RBufWriteStream	strWrite;
	CBufFlat*	dynBuffer =	NULL;

	if( err == KErrNone )
		{
		dynBuffer =	CBufFlat::NewL(bufferGrow);

		if( dynBuffer == NULL )
			{
			err =	KErrNoMemory;
			}
		}

	if( err == KErrNone )
		{
		strWrite.Open( *dynBuffer, 0 );
		}

	if( err == KErrNone )
		{
		RFs fileSystem;

		err = fileSystem.Connect();

		if( err == KErrNone )
			{
			TDriveList	driveList;

			if( err == KErrNone )
				{
				err = fileSystem.DriveList( driveList );
				}

			if( err == KErrNone )
				{
				TBuf8<KMaxFileName> name;		// Store volume name
												// in 8 bit format.
				TBuf8<KMaxFileName*2> buffer;	// Store volume information.

				TVolumeInfo	volumeInfo;
				TInt	drive = 0;
				for( drive = 0; drive < KMaxDrives; drive++ )
					{
					if( driveList[drive] )
						{
						err = fileSystem.Volume( volumeInfo, drive );

						if( err == KErrNone )
							{
								// Copy the volume name to an 8 bit buffer.
								name.Copy( volumeInfo.iName );
								// Format the text string.
								buffer.Format( driveFormat, drive + 'A',
									&name, volumeInfo.iSize, &newLine );
								// Add details of the current drive to our
								// buffer.
								TRAP( err, strWrite.WriteL( buffer ) );
							}
						}
					}	
				}
			fileSystem.Close();
			}
		}

	if( err == KErrNone || err == KErrNotReady )
		{
		TRAP( err, strWrite.WriteL( newLine ) );

		CDataSupplierMemory	*pDataSupplier = CDataSupplierMemory::NewL( );

		if (pDataSupplier)
			{
			// Set the data in the data supplier.
			err = pDataSupplier->SetData( dynBuffer->Ptr( 0 ) );

			if (err != KErrNone)
				{
				pMsg->Msg(_L("Failed to set data in data supplier") );
				}
			}
		else
			{
			err = KErrNoMemory;
			pMsg->Msg(_L("Failed to create data supply object") );
			}

		if (err == KErrNone)
			{
			*aDataSupplier =	pDataSupplier;
			}
		else
			{
			if (pDataSupplier)
				{
				pDataSupplier->Delete( );
				pDataSupplier = NULL;
				}
			}
		}

	strWrite.Close( );

	if( dynBuffer != NULL )
		{
		delete dynBuffer;
		dynBuffer = NULL;
		}

	return err;
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------

#ifndef LIGHT_MODE
TInt CStatApiCommandDecoder::GetDirectoryInformationL(MDataConsumer *const aDataConsumer, 
					MDataSupplier **aDataSupplier)
#else // ifndef LIGHT_MODE
TInt CStatApiCommandDecoder::GetDirectoryInformationL(MDataConsumer *const, 
					MDataSupplier **)
#endif // ifndef LIGHT_MODE
{
#ifndef LIGHT_MODE
	TInt err = KErrNone;

	// Get the directory entries for a specified directory 
	// and save to the data supplier object as a text string.

	TBuf<KMaxPath>	directoryName;

	static const TInt	bufferGrow =	(4*1024);
	RBufWriteStream	strWrite;
	CBufFlat*	dynBuffer =	NULL;

	if( err == KErrNone )
		{
		directoryName.Copy( *aDataConsumer );
		}

	if( err == KErrNone )
		{
		dynBuffer =	CBufFlat::NewL(bufferGrow);

		if( dynBuffer == NULL )
			{
			err =	KErrNoMemory;
			}
		}

	if( err == KErrNone )
		{
		strWrite.Open( *dynBuffer, 0 );
		}

	if( err == KErrNone )
		{
		RFs fileSystem;

		err = fileSystem.Connect();

		if( err == KErrNone )
			{
			CDir *entryList = NULL;

			if( err == KErrNone )
				{
				err = fileSystem.GetDir( directoryName, KEntryAttMaskSupported, EDirsFirst | ESortByName,
								entryList );
				}

			if( err == KErrNone )
				{
				TBuf8<KMaxFileName> name;		// Store entry name
												// in 8 bit format.
				TBuf8<KMaxFileName*2> buffer;	// Store entry information.

				static const TInt	maxDateLength = 32;
				TBuf8<maxDateLength> dateBuffer;

				TDateTime	dateTime;

				TInt	count = entryList->Count();
				TInt	entry = 0;
				for( entry = 0; (entry < count) && (err == KErrNone); entry++ )
					{
					const TEntry &anEntry = entryList->operator[](entry);

					// Format the date into readable text.
					dateTime = anEntry.iModified.DateTime();
					dateBuffer.Format( dateFormat, dateTime.Day() + 1, dateTime.Month() + 1,
									dateTime.Year(), dateTime.Hour(), dateTime.Minute() );

					// Copy the volume name to an 8 bit buffer.
					name.Copy( anEntry.iName );
					// Format the text string.
					buffer.Format( entryFormat, &name,
							anEntry.iAtt, &dateBuffer, anEntry.iSize, &newLine );
					// Add details of the current drive to our
					// buffer.
					TRAP( err, strWrite.WriteL( buffer ) );
					}
				}

			if( entryList != NULL )
				{
					delete entryList;
					entryList = NULL;
				}
			fileSystem.Close();
			}
		}

	if( err == KErrNone )
		{
		TRAP( err, strWrite.WriteL( newLine ) );

		CDataSupplierMemory	*pDataSupplier = CDataSupplierMemory::NewL( );

		if (pDataSupplier)
			{
			// Set the data in the data supplier.
			err = pDataSupplier->SetData( dynBuffer->Ptr( 0 ) );

			if (err != KErrNone)
				{
				pMsg->Msg(_L("Failed to set data in data supplier") );
				}
			}
		else
			{
			err = KErrNoMemory;
			pMsg->Msg(_L("Failed to create data supply object") );
			}

		if (err == KErrNone)
			{
			*aDataSupplier =	pDataSupplier;
			}
		else
			{
			if (pDataSupplier)
				{
				pDataSupplier->Delete( );
				pDataSupplier = NULL;
				}
			}
		}

	strWrite.Close( );

	if( dynBuffer != NULL )
		{
		delete dynBuffer;
		dynBuffer = NULL;
		}

	return err;
#else // ifndef LIGHT_MODE
	return (KErrNotSupported);
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------

#ifndef LIGHT_MODE
void CStatApiCommandDecoder::DoKeyEvent(TInt key)
#else // ifndef LIGHT_MODE
void CStatApiCommandDecoder::DoKeyEvent(TInt)
#endif // ifndef LIGHT_MODE
{
#ifndef LIGHT_MODE
	// If this assert triggers it is because we are running in
	// text shell mode without a window server and the call
	// requires the window server to be available.
	// A command filter in ExecuteCommand should have prevented
	// this call from being made.
	assert(IsWindowServerAvailable());

	TKeyEvent event;				//the key event structure
	event.iCode = key;				// pass character code into the struct value
	event.iScanCode = 0;
	event.iModifiers = 0;
	event.iRepeats = 0;				// no repeats for keypress

	iWs.SimulateKeyEvent(event);	// send keystroke
	iWs.Flush();					// flush client-side window server buffer
#endif // ifndef LIGHT_MODE
}

//------------------------------------------------------------------------------
//Get the required device information
TInt CStatApiCommandDecoder::GetData(TInt i, TDes8& info)
{
	TInt err = KErrNone;
	TInt a = 0;
	TBuf8<150> devicevalue = _L8("");
	TBool found = EFalse;

	devicevalue.Num(ENUM_VALDEVICEINFORMATIONArray[i][a]);	//assign initial data in array to value (element 0)
	while (ENUM_DEVICEINFORMATIONArray[i][a] != NULL)
	{
		if (info.Compare(devicevalue) == 0)	//if the string has been found in the value array
		{
			info = ENUM_DEVICEINFORMATIONArray[i][a];	//set the value to the string equivalent
			found = ETrue;
			break;
		}

		a++;
		devicevalue.Num(ENUM_VALDEVICEINFORMATIONArray[i][a]);	//update contents of value for the next pass
	}	//end while

	if (!found)
		info = _L8("Not available");

	return err;
}

//------------------------------------------------------------------------------
void CStatApiCommandDecoder::AppendErrorCode(TInt ret, MDataSupplier **aDataSupplier)
{
	HBufC8 *buffer = HBufC8::New(10);
	if (buffer)
	{
		// add the error code
		TPtr8 pBuffer(buffer->Des());
		pBuffer.Zero();
		pBuffer.Num(ret);
		pBuffer.ZeroTerminate();

		*aDataSupplier = CDataSupplierMemory::NewL( );
		if (*aDataSupplier)
		{
			// Set the data in the data supplier.
			(*aDataSupplier)->SetData( *buffer );
		}

		delete buffer;
		buffer = NULL;
	}
}

TInt CStatApiCommandDecoder::StopProgramL(const TDesC& aHandleNumber)
{
	TLex conv(aHandleNumber);
	
	TInt64 handleNumber;
	
	TInt err = conv.Val(handleNumber);
	
	if(err==KErrNone)
		{
		RProcess newProcess;
		err = newProcess.Open(TProcessId(handleNumber));
		if(err==KErrNone)
			{
			newProcess.Kill(1);
			}
		newProcess.Close();	
		}
	
	return err;
}


TInt CStatApiCommandDecoder::ProgramStatusL(const TDesC& aHandleNumber, MDataSupplier **aDataSupplier)
{
	
	CDataSupplierMemory	*pDataSupplier = CDataSupplierMemory::NewL( );
	
	TBuf8<50> exitData;

	TLex conv(aHandleNumber);
	
	TInt64 handleNumber;
	
	TInt err = conv.Val(handleNumber);
	
	if(err==KErrNone)
		{
		RProcess newProcess;
		TInt status = newProcess.Open(TProcessId(handleNumber));
		
		switch(status)
			{
			case KErrNone:
				exitData.Num(ETrue);
				break;
			default:
				exitData.Num(EFalse);
			}
			
		if (pDataSupplier)
			{
			// Set the data in the data supplier.
			err = pDataSupplier->SetData( exitData );

			if (err != KErrNone)
				{
				pMsg->Msg(_L("Failed to set data in data supplier") );
				}
			}
		else
			{
			err = KErrNoMemory;
			pMsg->Msg(_L("Failed to create data supply object") );
			}
		
		if (err == KErrNone)
			{
			*aDataSupplier =	pDataSupplier;
			}
		else
			{
			if (pDataSupplier)
				{
				pDataSupplier->Delete( );
				pDataSupplier = NULL;
				}
			}
		
		newProcess.Close();	
		}
	
	return err;
}


void CStatApiCommandDecoder::updatePathWithSysDrive(TBuf<1024>& pathToChange )
{
	//try it here...
   	//check for the sysdrive being diff from C:
	TBufC<KMaxPath> workingPath(pathToChange) ; 
	TInt indx = workingPath.Des().Find(KReDrive) ; 
	while(	indx > KErrNotFound )
	 	{
		workingPath.Des().Replace(indx,2,getSystemDrive());
		pathToChange = workingPath.Des() ; 
		indx = workingPath.Des().Find(KReDrive) ; 
	 	}
	
}

void CStatApiCommandDecoder::updatePathWithSysDrive(TBuf<KMaxPath>& pathToChange )
	{
	TBufC<KMaxPath> workingPath(pathToChange) ; 
	TInt indx = workingPath.Des().Find(KReDrive) ; 
	while(	indx > KErrNotFound )
	 	{
		workingPath.Des().Replace(indx,2,getSystemDrive());
		pathToChange = workingPath.Des() ; 
		indx = workingPath.Des().Find(KReDrive) ; 
	 	}
	
	}

TDriveName CStatApiCommandDecoder::getSystemDrive()
{

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
	
	//TDriveName drive( TDriveUnit(defaultSysDrive).Name()) ;
	return TDriveUnit(defaultSysDrive).Name() ; 
	
}
