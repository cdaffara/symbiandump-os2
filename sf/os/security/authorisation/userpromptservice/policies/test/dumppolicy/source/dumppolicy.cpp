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


#include <bacline.h>
#include <e32cons.h>
#include <e32debug.h>
#include <e32std.h>
#include <f32file.h>
#include <utf.h>

#include <ups/policy.h>
#include "../../../source/policyreader.h" 

_LIT(KAppName, "dumppolicy");

using namespace UserPromptService;

/**
 Class that prints text to the console and optionally a log file.
 */
class CPrinter : public CBase
	{
public:
	static CPrinter* NewLC(CConsoleBase* aConsole);
	
	static CPrinter* NewLC(CConsoleBase* aConsole, RFile& aFile);
	void PrintL(TRefByValue<const TDesC16> aFormat, ...);
	~CPrinter();
	
private:
	CPrinter(CConsoleBase* aConsole);
	
	/** Console object to print text to */
	CConsoleBase* iCon;
	
	/* Optional file handle to write text to */
	RFile iFile;
	
	/* Whether to log the output to the file */
	TBool iLogToFile;
	
	/** Temporary buffer */
	TBuf<256> iBuffer;
	};

	
CPrinter* CPrinter::NewLC(CConsoleBase* aConsole, RFile& aFile) 
/**
Creates a new printer object and places the pointer on the cleanup stack.
@param	aConsole	The console object to print text to.
@param	aFile		A handle to a file to write the text to. The handle is duplicated internally.					
@return A pointer to the new printer object.
*/
	{
	CPrinter* self = CPrinter::NewLC(aConsole);
	User::LeaveIfError(self->iFile.Duplicate(aFile));
	self->iLogToFile = ETrue;
	return self;
	}

CPrinter* CPrinter::NewLC(CConsoleBase* aConsole) 
/**
Creates a new printer object and places the pointer on the cleanup stack.
@param	aConsole The console object to print text to.
@return A pointer to the new printer object.
*/
	{
	CPrinter* self = new(ELeave) CPrinter(aConsole);
	CleanupStack::PushL(self);
	return self;
	}

void CPrinter::PrintL(TRefByValue<const TDesC16> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
		
	iBuffer.Zero();
	iBuffer.AppendFormatList(aFormat, list);
		
	iCon->Printf(iBuffer);	
	if (iLogToFile)
		{
		HBufC8* utf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(iBuffer);
		CleanupStack::PushL(utf8);
		User::LeaveIfError(iFile.Write(*utf8));
		CleanupStack::PopAndDestroy(utf8);
		}	
	
	VA_END(list);
	}

CPrinter::CPrinter(CConsoleBase* aConsole) : iCon(aConsole)
/**
 Constructor
*/
	{
	}

CPrinter::~CPrinter() 
/**
 Destructor
*/
	{
	iFile.Close();
	}

void PrintSystemServerSecurity(CPrinter* aPrinter, CPolicy *aPolicy)
/**
Prints the system server security configuration.
*/
	{

	TPtrC p;
	switch (aPolicy->SystemServerSecurity())
		{
	case CPolicy::ESystemServerSecurityPassedOrFailed:
		p.Set(_L("ESystemServerSecurityPassedOrFailed"));
		break;
	case CPolicy::ESystemServerSecurityPassed:
		p.Set(_L("ESystemServerSecurityPassed"));	
		break;
	case CPolicy::ESystemServerSecurityFailed:
		p.Set(_L("ESystemServerSecurityFailed"));
		break;
	default:
		p.Set(_L("*** UNKNOWN ***"));
		break;				
		}
	TBuf<80> buf;
	buf.AppendFormat(_L(" System Server Security: %S\n"), &p);
	aPrinter->PrintL(buf);
	}

/**
 Prints a object to the supplied printer object.
 @param aPrinter	The printer.
 @param aPolicy		The policy to print.
 */
void PrintPolicy(CPrinter* aPrinter, CPolicy *aPolicy)
	{
	_LIT16(KYes, "EYes");
	_LIT16(KNo, "ENo");
	_LIT16(KSessionYes, "ESessionYes");	
	_LIT16(KSessionNo, "ESessionNo");	
	_LIT16(KAlways, "EAlways");
	_LIT16(KNever, "ENever");
	_LIT16(KSpace, " ");
		
	TBuf<256> tmp;			
	TInt sidClasses = aPolicy->SidClasses().iSidClasses;
	aPrinter->PrintL(_L("            SID Classes: 0x%04x\n"), sidClasses);
	tmp.Zero();
	
	const RArray<TSecureId>& sidList = aPolicy->SidList();	
	aPrinter->PrintL(_L("               SID List:"));	
	
	TInt sidCount = sidList.Count();	
	for (TInt i = 0; i < sidCount; ++i)
		{		
		aPrinter->PrintL(_L(" 0x%08x"), sidList[i].iId);
		}
	aPrinter->PrintL(_L("\n"));
	
	PrintSystemServerSecurity(aPrinter, aPolicy);
	
	tmp.Copy(aPolicy->Destination());
	aPrinter->PrintL(_L("            Destination: %S\n"), &tmp);	
	
	TInt options = aPolicy->Options();
	tmp.Zero();
	if (options & CPolicy::EYes)
		{
		tmp.Append(KYes);
		tmp.Append(KSpace);
		}
	if (options & CPolicy::ENo)
		{
		tmp.Append(KNo);
		tmp.Append(KSpace);
		}
	if (options & CPolicy::ESessionYes)
		{
		tmp.Append(KSessionYes);
		tmp.Append(KSpace);
		}
	if (options & CPolicy::EAlways)
		{
		tmp.Append(KAlways);
		tmp.Append(KSpace);
		}
	if (options & CPolicy::ENever)
		{
		tmp.Append(KNever);
		tmp.Append(KSpace);
		}		
	if (options & CPolicy::ESessionNo)
		{
		tmp.Append(KSessionNo);
		tmp.Append(KSpace);
		}
	
	aPrinter->PrintL(_L("                Options: %S\n"), &tmp);
	aPrinter->PrintL(_L("       Policy Evaluator: 0x%08x\n"), aPolicy->PolicyEvaluator());
	aPrinter->PrintL(_L("         Dialog Creator: 0x%08x\n"), aPolicy->DialogCreator());
	aPrinter->PrintL(_L("\n"));
	}

static void PrintAuthPolicyL(CPrinter* aPrinter, TAuthorisationPolicy aAuthPolicy)
/**
 Prints the authorisation policy.
 @param aPrinter	The printer object.
 @param aAuthPolicy	The authorisation policy.
 */
	{
	TBuf<80> buf;
	TPtrC authPol;
	switch (aAuthPolicy)
		{
		case EAlwaysCheck:
			authPol.Set(_L("EAlwaysCheck"));
			break;
		case ECheckPostManufacture:
			authPol.Set(_L("ECheckPostManufacture"));
			break;
		case ECheckUnprotectedSids:
			authPol.Set(_L("ECheckUnprotectedSids"));
			break;
		case ECheckIfFailed:
			authPol.Set(_L("ECheckIfFailed"));
			break;
		case ENeverCheck:
			authPol.Set(_L("ENeverCheck"));
			break;
		default:
			authPol.Set(_L("*** UNKNOWN ***"));
			break;
		}
	buf.AppendFormat(_L("     Authorisation Policy: %S\n"), &authPol);
	aPrinter->PrintL(buf);
	}

static void PrintPoliciesL(CPrinter* aPrinter, CPolicyReader* aReader)
/**
 Prints all of the policies returned by a CPolicyReader object.
 @param aPrinter The printer object.
 @param aReader	 The policy reader.
 */
	{
	TPolicyHeader hdr = aReader->Header();
	aPrinter->PrintL(_L("*** Policy Header ***\n"));
	aPrinter->PrintL(_L("            Policy Format: %d\n"), hdr.iFormatVersion);
	aPrinter->PrintL(_L("            Major Version: %d\n"), hdr.iMajorVersion);
	aPrinter->PrintL(_L("            Minor Version: %d\n"), hdr.iMajorVersion);
	aPrinter->PrintL(_L(" Default Policy Evaluator: 0x%08x\n"), hdr.iDefaultPolicyEvaluator);
	aPrinter->PrintL(_L("   Default Dialog Creator: 0x%08x\n"), hdr.iDefaultDialogCreator);	
	PrintAuthPolicyL(aPrinter, hdr.iAuthPolicy);
	aPrinter->PrintL(_L("\n"));
	
	CPolicy* p;
	TInt i = 0;
	while ((p = aReader->NextPolicyL()) != 0)
		{
		TBuf<32> buf;
		buf.AppendFormat(_L("*** Policy %d ***\n"), i);
		aPrinter->PrintL(buf);
		
		PrintPolicy(aPrinter, p);
		delete p;
		++i;
		}	
	}

static void MainL(void)
/**
Takes a User Prompt Service policy resource file and dumps it as human readable text to the 
console. The user may also specify the name of an output file on the command line. If so, text
is also written to this file.
*/
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
		
	CConsoleBase* console = Console::NewL(KAppName, TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	
	if (args->Count() > 1)
		{		
		CPolicyReader* reader = CPolicyReader::NewLC(fs, args->Arg(1));		
		CPrinter* printer(0);
		if (args->Count() > 2)
			{
			RFile outFile;
			User::LeaveIfError(outFile.Replace(fs, args->Arg(2), EFileShareExclusive | EFileWrite));
			CleanupClosePushL(outFile);
			printer = CPrinter::NewLC(console, outFile);		
			
			CleanupStack::Pop(printer); 
			CleanupStack::PopAndDestroy(&outFile);
			CleanupStack::PushL(printer);
			}
		else
			{
			printer = CPrinter::NewLC(console);	
			}
		__UHEAP_MARK;
		PrintPoliciesL(printer, reader);
		__UHEAP_MARKEND;
		
		if (args->Count() < 3)
			{
			// If no output file is specified then pause after finishing
			// because the console will vanish when it is closed.
			console->Printf(_L("Press any key to continue\r\n"));
			console->Getch();
			}		
		CleanupStack::PopAndDestroy(2, reader); // printer, reader		
		}
	else 
		{
		console->Printf(_L("Usage: dumppolicy.exe policy.rsc <output.txt>\r\n"));
		console->Printf(_L("Press any key to continue\r\n"));
		console->Getch();
		}
	
	CleanupStack::PopAndDestroy(3, &fs); // args, console, fs
	}

GLDEF_C TInt E32Main()
/**
Creats clean up stack and invokes real main function.
*/
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	__UHEAP_MARK;
	TRAPD(err, MainL());
	__UHEAP_MARKEND;
	delete cleanup;
	return err;
	}
