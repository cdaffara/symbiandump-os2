// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file te_fileplugin.cpp
 @internalTechnology
*/

#include <e32std.h>
#include <e32debug.h>
#include <ecom/implementationproxy.h>

#include "te_fileplugin.h"

#if defined(__LIGHTLOGGER_ENABLED)
#include "lightlogger.h"
#endif



namespace Ulogger
{

/**
Public Destructor
*/
CTestFileWriter::~CTestFileWriter()
	{
	if(iIsOpen)
	    {
	    iLog.Close();
	    }
	iFs.Close();
	}

/**
Creates an instance of CTestFileWriter object
@return a pointer to the new created CTestFileWriter Object
@leave KErrNoMemory if no memory
*/
CTestFileWriter* CTestFileWriter::NewL()
	{
	CTestFileWriter *me = new (ELeave) CTestFileWriter;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
	return me;
	}

void CTestFileWriter::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iIsOpen = EFalse;
	iErrorCode = 0;
	iCountDown = 3;
	iFileName.Copy(KTestLogDefaultFileName);
	}

//Default constructor	
CTestFileWriter::CTestFileWriter():iMutex(NULL)
	{
	}
	

void CTestFileWriter::CloseOutputPlugin()
	{	
	if(!iIsOpen)
	    {
	    return;
	    }
	iLog.Flush();
	iLog.Close();
	iIsOpen = EFalse;
	}

void CTestFileWriter::DoClearLog()
	{
	OpenLog();
	if(iIsOpen)
	    {
	    iLog.Close();
	    iIsOpen = EFalse;
	    iFs.Delete(iFileName);
	    }
	}

TInt CTestFileWriter::ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aConfigs)
	{
	CloseOutputPlugin();
	
	TInt i = aConfigs.Count();
	while(i-->0)
		{
		TPluginConfiguration* set = aConfigs[i];
		if(set->Key().Compare(KTestOutputPath) == 0)
			{
			if(set->Value().Length() < iFileName.MaxLength())
			    {
			    iFileName.Copy(set->Value());
			    }
			}	
		else if(set->Key().Compare(KErrorCode) == 0)
			{
			iErrorCode = KErrGeneral;
			}
		}
	return KErrNone;
	}

	
TInt CTestFileWriter::OpenLog()
    {
    TInt err = KErrNone;
    if(!iIsOpen)
        {
    	//Check that the log file exists, if not create a blank one.
    	TInt err = iLog.Open(iFs, iFileName, EFileWrite | EFileShareAny);
    	if(err==KErrPathNotFound || err==KErrNotFound)
    		{
    		err = iFs.MkDirAll(iFileName);
    		if(err!=KErrNone && err!=KErrAlreadyExists)
    		    {
    		    return err;
    		    }
    		err = iLog.Create(iFs, iFileName, EFileWrite | EFileShareAny);
    		}
		if(err!=KErrNone)
		    {
		    return err;
		    }
	    iIsOpen = ETrue;
        }
    return err;		
	}
   	
TInt CTestFileWriter::Write(const TDesC8& aText)
	{
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CFileWriter::Write")
	#endif
	
	if(!iErrorCode)
		return WriteToFile(aText);
	else
		{
		if(--iCountDown > 0)
			return KErrNone;
		else
			{
			RDebug::Printf("Returning error code (%d) from test plugin", iErrorCode);
			User::InfoPrint(_L("writing with error - ok"));
			return iErrorCode;
			}
		}
	}

TBool CTestFileWriter::DoOwnProcessing()
	{
	return EFalse;
	}
	
TInt CTestFileWriter::WriteToFile(const TDesC8& aText)
	{
	TInt err = KErrNone;        
	if(!iIsOpen)
	    {
        err = OpenLog();
        if(err!=KErrNone)
            return err;
	    }
    iLog.Write(0x7FFFFFFF, aText);	    
        
	return err;
	}


TAny* CTestFileWriter::GetInterfaceL(CPlugin::TPluginInterface aInterfaceId)
	{
	if(aInterfaceId == MOutputPlugin::iInterfaceId)
		return static_cast<MOutputPlugin*>(this);
	else
		return NULL;
	}	

}// namespace

//ECOM interface implementations
	
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
           // {{0x102836BF},Ulogger::CTestFileWriter::NewL}
           IMPLEMENTATION_PROXY_ENTRY(0x102836BF, Ulogger::CTestFileWriter::NewL)
    };


// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
