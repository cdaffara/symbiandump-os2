/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
////////////////////////////////////////////////////////////////////////
//
// Source file for the implementation of the 
// application UI class - TForm18030_AppUi
//
////////////////////////////////////////////////////////////////////////
#include <e32math.h> 
#include <e32test.h>
#include "TForm18030.h"

static const TUint32 HugeBufSize = 32760;

TText hugeGBK[HugeBufSize];

TText hugeGB18030[HugeBufSize * 2];

static void prepareHugeBuffer()
	{
	TUint32 i;
	for (i = 0; i < HugeBufSize; i++)
		{
		hugeGBK[i] = 0x3400 + i;
		}
	for (i = 0; i < HugeBufSize; i++)
		{
		unsigned int cp = 0x20000 + i;
		TText hi = (cp - 0x10000) / 0x400 + 0xD800;
		TText lo = (cp - 0x10000) % 0x400 + 0xDC00;

		hugeGB18030[i*2] = hi;
		hugeGB18030[i*2 + 1] = lo; 
		}
	}

static const TUint32 RandBufSizeMask = 0x3FFF;
static void prepareRandomBuffer(TText *& buffer, TUint32 & length)
	{
	if (buffer != NULL)
		{
		User::Free(buffer);
		}
	length = (Math::Random() & RandBufSizeMask) + 1;
	buffer = (TText*) User::Alloc(length * sizeof(TText));
	if (buffer == NULL)
		{
		User::Panic(_L("Mem Alloc"), 12345);
		}
	}


static void rbFillCharacter(TUint32 ch, TUint32 & i, TUint32 length, TText *buffer)
	{

	if (ch >= 0x10000 && ch <= 0x10FFFF)
		{
		if (i < length - 2)
			{
			TText hi = ((ch - 0x10000) / 0x400) + 0xD800;
			TText lo = ((ch - 0x10000) % 0x400) + 0xDC00;
			buffer[i++] = hi;
			buffer[i++] = lo;
			}
		else
			{
			buffer[i++] = 0;
			}
		}
	else
		{
		buffer[i++] = (TText) (ch);
		}
	}

static void rbInjectCharRange(TText * buffer, TUint32 length, TUint32 & i, TUint32 base, TUint32 volumn )
	{
	if (buffer == NULL)
		{
		User::Panic(_L("NULL argument"), 12345);
		}


    while(1)
    	{
    	TUint32 ch = Math::Random() % volumn;
    	ch = ch + base; 
        TChar c(ch);
        TChar::TCharInfo ci;
        c.GetInfo(ci);
        if(((ci.iCategory & TChar::EUnassignedGroup) == TChar::EUnassignedGroup) || ((ci.iCategory & TChar::EControlGroup) == TChar::EControlGroup))
        	continue;
        rbFillCharacter(ch, i, length, buffer);
        break;
        }
	}
static const TUint32 LatinCharVolumn = 97;
static const TUint32 LatinCharBase = 32;
static const TUint32 CJKBMPCharVolumn = 20923;
static const TUint32 CJKBMPBase = 0x4E00;
static const TUint32 CJKExtACharVolumn = 6581;
static const TUint32 CJKExtABase = 0x3400;
static const TUint32 CJKExtBCharVolumn = 42710;
static const TUint32 CJKExtBBase = 0x20000;


static const TUint32 CharacterCategoryVolumn = 10;
static void fillRandomBuffer(TText * buffer, TUint32 & length)
	{
	TUint32 index = 0;
	TUint32 chcnt = 0;
	while (index < length - 1)
		{
		int cat = Math::Random() % CharacterCategoryVolumn;
		chcnt++;
		switch (cat)
			{
			case 0:
				rbInjectCharRange(buffer, length, index,
						LatinCharBase, LatinCharVolumn);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				rbInjectCharRange(buffer, length, index,
						CJKBMPBase, CJKBMPCharVolumn);
				break;
			case 6:
			case 7:
				rbInjectCharRange(buffer, length, index,
							CJKExtABase, CJKExtACharVolumn);
				break;
			case 8:
			case 9:
				//rbInjectCharRange(buffer, length, index,
				//						0, 65536);
				rbInjectCharRange(buffer, length, index,
							CJKExtBBase, CJKExtBCharVolumn);
				break;
			}
		}

	buffer [index] = 0;
	length = chcnt;
	}


//             The second phase constructor of the application UI class.
//             The application UI creates and owns the one and only view.
// 
void TForm18030_AppUi::ConstructL()
    {
    CConsoleBase * console;
    RTest test(_L("T_FORM18030"));
    
    test.Title();
    
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    test.Start(_L("TFORM18030 TESTING"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    
	           // BaseConstructL() completes the UI framework's
	           // construction of the App UI.
    BaseConstructL();
	           // Create the single application view in which to
	           // draw the text "Hello World!", passing into it
	           // the rectangle available to it.
	iAppView = TForm18030_AppView::NewL(ClientRect());
	   
	
	prepareHugeBuffer();
	
	test.Next(_L("Surrogate Test 1"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item0);
    User::After(2000000);
    
    test.Next(_L("Surrogate Test 2"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item1);
    User::After(2000000);
    
    test.Next(_L("BMP Test"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item2);
    User::After(2000000);
    
    test.Next(_L("Surrogate Negative Test"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item3);
    User::After(2000000);
    
    test.Next(_L("GBK Large Buffer Test"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item4);
    User::After(2000000);
    
    test.Next(_L("GB18030 Large Buffer Test"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item5);
    User::After(2000000);
    
    test.Next(_L("Random Test"));
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    HandleCommandL(ETForm18030Item6);
    User::After(2000000);
    
    test.End();
    console = test.Console();
    test.SetConsole(NULL);
    console->Delete(console);
    
    test.Close();
    
    
    HandleCommandL(EEikCmdExit);
	}


//             The app Ui owns the two views and is. 
//             therefore, responsible for destroying them
//
TForm18030_AppUi::~TForm18030_AppUi()
	{
	delete iAppView;
	}



//             Called by the UI framework when a command has been issued.
//             In this example, a command can originate through a 
//             hot-key press or by selection of a menu item.
//             The command Ids are defined in the .hrh file
//             and are 'connected' to the hot-key and menu item in the
//             resource file.
//             Note that the EEikCmdExit is defined by the UI
//             framework and is pulled in by including eikon.hrh
//
void TForm18030_AppUi::HandleCommandL(TInt aCommand)
	{
	_LIT(Sur1,"\xD800\xDD0A\xD800\xDD37\xD840\xDC00\xD869\xDED6\xD87E\xDC00\xD87E\xDE1D");
	_LIT(Sur2,"\xD834\xDF00\xD87E\xDF00\xD834\xDF56");
	_LIT(Bmp1,"\x220\x60B\x9FA6\x9FBB\xFA30\xFA6A\xFA70\xFAD9\xFF5F\xFF60");
	_LIT(Neg1,"\xFFC8\xFDC8\xD800\xDC60");
	
	
	switch (aCommand)
		{
		      // Just issue simple info messages to show that
		      // the menu items have been selected
	case ETForm18030Item0:
//----------------------------------------------------------------------------------------------

/**
   @SYMTestCaseID           TI18N-FORM-CT-4028
   @SYMPREQ                 2471
   @SYMTestCaseDesc         Check FORM Unicode 5.0 support
   @SYMTestPriority         High 
   @SYMTestStatus           Implemented
   @SYMTestActions          1. Construct string with surrogate characters
                            2. Call FORM’s formatting 
                            3. Construct string with invalid surrogate characters
                            4. Call FORM’s formatting
                            5. Construct string with newly added UCD
                            6. Call FORM’s formatting
   @SYMTestExpectedResults  Characters are correctly drawn
 */
//----------------------------------------------------------------------------------------------
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), Sur1);
		break;
	case ETForm18030Item1:
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), Sur2);
		break;
	
//----------------------------------------------------------------------------------------------

/**
   @SYMTestCaseID           TI18N-FORM-CIT-4029
   @SYMPREQ                 2471
   @SYMTestCaseDesc         Check FORM Unicode 5.0 support, negative input
   @SYMTestPriority         High 
   @SYMTestStatus           Implemented
   @SYMTestActions          1. Get BMP character, not mapped to valid GB18030 character
                            2. Draw it on WORD.exe
                            3. Get non-BMP character, not mapped to valid GB18030 character
                            4. Draw it on WORD.exe
   @SYMTestExpectedResults  Placeholder is drawn
 */
//----------------------------------------------------------------------------------------------
	case ETForm18030Item2:
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), Bmp1);
		break;
		
	case ETForm18030Item3:
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), Neg1);
		break;
               // Exit the application. The call is
		       // implemented by the UI framework.


//----------------------------------------------------------------------------------------------

/**
   @SYMTestCaseID           TI18N-FORM-CIT-4030
   @SYMPREQ                 2471
   @SYMTestCaseDesc         Check FORM performance
   @SYMTestPriority         High 
   @SYMTestStatus           Implemented
   @SYMTestActions          1. Pass GBK huge data to FORM before the component update
                            2. Collect performance baseline
                            3. Pass GBK huge data to FORM after the component update
                            4. Collect performance data
                            5. Pass GB18030 huge data to FORM after the component update
                            6. Collect performance data
   @SYMTestExpectedResults  The drawback is less than 5% 
 */
//----------------------------------------------------------------------------------------------
	case ETForm18030Item4:
		{
		TTime start, end;
		start.UniversalTime();
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtBC"), TPtrC(hugeGBK, HugeBufSize));
		end.UniversalTime();
		}
		break;

	case ETForm18030Item5:
		{
		TTime start, end;
		start.UniversalTime();
		((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), TPtrC(hugeGB18030, HugeBufSize*2));
		end.UniversalTime();
		}
		break;
		
	case ETForm18030Item6:
		{
		
		TText * buf = NULL;
		TUint32 len;
		TUint32 loop = 100;
		for (TUint32 i = 0 ; i< loop; i++)
			{
			buf = NULL;
			prepareRandomBuffer(buf, len);
			fillRandomBuffer(buf, len);
			((TForm18030_AppView*)iAppView)->ChangeEditorText(_L("TestExtB"), TPtrC(buf));
			User::Free(buf);
			}
		}
		break;
		
	case EEikCmdExit: 
		Exit();
		break;
		}
	}

