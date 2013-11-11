// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "uloggerfileplugin.h"

namespace Ulogger
{
/**
Public Destructor
*/
CFileWriter::~CFileWriter()
	{
	if(iIsOpen)
	    {
	    iLog.Close();
	    }
	iFs.Close();
	}

/**
Creates an instance of CFileWriter object
@return a pointer to the new created CFileWriter Object
@leave KErrNoMemory if no memory
*/
 CFileWriter* CFileWriter::NewL()
	{
	CFileWriter *me = new (ELeave) CFileWriter;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
	return me;
	}

void CFileWriter::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iIsOpen = EFalse;
	iFileName.Copy(KLogDefaultFileName);	
	}

//Default constructor	
CFileWriter::CFileWriter()
:iMutex(NULL)
	{
	}
	
void CFileWriter::CloseOutputPlugin()
	{	
	if(!iIsOpen)
	    {
	    return;
	    }
	iLog.Flush();
	iLog.Close();
	iIsOpen = EFalse;
	}

TInt CFileWriter::ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration> &aSettings)
	{	
	CloseOutputPlugin();
	
	TInt i = aSettings.Count();
	while(i-->0)
		{
		TPluginConfiguration* set = aSettings[i];
		if(set->Key().Compare(KOutputPath) == 0)
			{
			if(set->Value().Length() < iFileName.MaxLength())
			    iFileName.Copy(set->Value());
			else
				return KErrGeneral;
			}		
		}
	return OpenLog();
	}

TInt CFileWriter::OpenLog()
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
    		    return err;
    		err = iLog.Create(iFs, iFileName, EFileWrite | EFileShareAny);
    		}
		if(err!=KErrNone)
		    return err;
	    iIsOpen = ETrue;
        }
    return err;		
	}
	
TInt CFileWriter::Write(const TDesC8& aText)
	{	
	return WriteToFile(aText);
	}

    		
TInt CFileWriter::WriteToFile(const TDesC8& aText)
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


TAny* CFileWriter::GetInterfaceL(TPluginInterface aInterfaceId)
	{
	if(aInterfaceId == MOutputPlugin::iInterfaceId)
		return static_cast<MOutputPlugin*>(this);
	else
		return NULL;
	}

} // namespace

//ECOM interface implementations
	
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
           IMPLEMENTATION_PROXY_ENTRY(0x102836CA, Ulogger::CFileWriter::NewL)
    };


// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
