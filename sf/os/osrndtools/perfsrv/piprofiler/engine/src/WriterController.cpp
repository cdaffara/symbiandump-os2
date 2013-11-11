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
* Description:  
*
*/


#include <piprofiler/EngineUIDs.h>
#include <piprofiler/ProfilerTraces.h>

#include "WriterController.h"

const TInt KMaxWriterPluginCount = 10;


CWriterController* CWriterController::NewL(CProfilerSampleStream& aStream)
    {
    CWriterController* self = new( ELeave ) CWriterController(aStream);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

CWriterController::CWriterController(CProfilerSampleStream& aStream) : 
    iStream(aStream)
    {
    }

void CWriterController::ConstructL()
	{
	// initiate writer plugin list

	}


void CWriterController::InitialiseWriterListL()
    {
    // create new writer plugin array
    iPluginArray = new(ELeave) CArrayPtrFlat<CWriterPluginInterface>( KMaxWriterPluginCount );
    
    // create new writer plugin loader
    iPluginLoader = CWriterPluginLoader::NewL();
    iPluginLoader->SetObserver( this );
    iPluginLoader->LoadAsyncL( iPluginArray );

    LOGTEXT(_L(" RWriterController::InitialiseWriterList - exit"));	
    }

CWriterController::~CWriterController()
    {
    LOGTEXT(_L("RWriterController::~RWriterController" ));
    if ( iPluginArray )
        {
        // destroy the plugin instances
        // empty loaded plugins from array
        for(TInt i(0);i<iPluginArray->Count();i++)
            {
            if(iPluginArray->At(i))
                {
                delete iPluginArray->At(i);
                iPluginArray->At(i) = NULL;
                }
            }
        iPluginArray->Reset();
        delete iPluginArray;
        iPluginArray = NULL;
        }
    
    if ( iPluginLoader )
        {
        iPluginLoader->AbortAsyncLoad();
        delete iPluginLoader;
        iPluginLoader = NULL;
        }

    }


void CWriterController::InitialisePluginStream()
	{
	LOGTEXT(_L("RWriterController::InitialisePluginStream - entry"));
	if( iPluginArray )
		{
		TInt pluginCount(iPluginArray->Count());
	
		LOGSTRING2("RWriterController::InitialisePluginStream - plugin count %d, searching through", pluginCount);
	  	for(TInt i=0;i<pluginCount;i++)
	  	    {
  			LOGSTRING2("RWriterController::InitialisePluginStream - getting plugin n:o: %d...", i);
	    	CWriterPluginInterface* plugin = iPluginArray->At(i);
	  		LOGSTRING2("RWriterController::InitialisePluginStream - writer found, 0x%x, initializing stream...", plugin->Id());
    		plugin->SetStream(iStream);
	  		LOGTEXT(_L("RSamplerController::InitialisePluginStream - succeeded!"));
	  	    }
	  	}
	LOGTEXT(_L("RSamplerController::InitialisePluginStream - exit"));

	}

CArrayPtrFlat<CWriterPluginInterface>* CWriterController::GetPluginList()
    {
	return iPluginArray;
    }

void CWriterController::HandlePluginLoaded( KWriterPluginLoaderStatus aStatus )
    {
    
    switch(aStatus)
        {
        case 0:
            LOGSTRING2("RWriterController - one plugin loaded, status: %d", aStatus);
            break;
        case 1:
            LOGSTRING2("RWriterController - a plugin load failed: %d", aStatus);
            break;
        case 2:
            LOGSTRING2("RWriterController - plugin loading aborted: %d", aStatus);
            break;
        case 3:
            LOGSTRING2("RWriterController - all plugins loaded: %d", aStatus);
            // set stream after all loaded writer plugins
            InitialisePluginStream();
            break;
        case 4:
            LOGSTRING2("RWriterController - error in loading plugins: %d", aStatus);
            break;
        default:
            break;
        }
    }



TUid CWriterController::GetPluginUID(TInt traceId)
    {
	LOGSTRING2(" RWriterController::GetPluginUID - checking UID for traceId = %d",traceId);	
	// this part has to be changed for each new writer

    if( iPluginArray && iPluginArray->Count() > 0 )
        {
        for(TInt i=0;i<iPluginArray->Count();i++)
            {
            CWriterPluginInterface* plugin = iPluginArray->At(i); 
            if(plugin->Id().iUid == traceId)
                {
                LOGSTRING2(" RWriterController::GetPluginUID - got: 0x%X",plugin->Id());	
                return plugin->Id();
                }
            }
        }	
	return KWriterNoneSelected;

    }

CWriterPluginInterface* CWriterController::GetActiveWriter()
    {
    CWriterPluginInterface* plugin = NULL;

	if( iPluginArray )
		{
		TInt count(iPluginArray->Count());
		
		for(TInt i=0;i<count;i++)
			{
			plugin = iPluginArray->At(i); 
			if(plugin->GetEnabled())
				{
				return plugin;
				}
			}
		}

    return (CWriterPluginInterface*)0;
    }

TUint32 CWriterController::GetWriterType(TUint32 writerId)
    {
    TUid id;
    
    id = this->GetPluginUID(writerId);
    
    if(id != KWriterNoneSelected)
        return GetPlugin(id)->GetWriterType();
    else
        return 0;
    }

CWriterPluginInterface* CWriterController::GetPlugin(TUid aUid)
    {
    if( iPluginArray && iPluginArray->Count() > 0 )
        {
        for(TInt i=0;i<iPluginArray->Count();i++)
            {
            CWriterPluginInterface* plugin = iPluginArray->At(i); 
            // check if searched uid found
            if(plugin->Id().iUid == aUid.iUid)
                {
                // return pointer to found plugin
                return plugin;
                }
            }
        }
    // return null plugin
	return (CWriterPluginInterface*)0;
    }

TInt CWriterController::StartSelectedPlugin()
	{
	LOGTEXT(_L("RWriterController::StartSelectedPlugin - entry"));
	
    CWriterPluginInterface* plugin = GetActiveWriter();
    
    if(plugin)
        {
        return plugin->Start();
		}
    
	LOGTEXT(_L("RWriterController::StartSelectedPlugin - exit"));
	return KErrNotFound;
	}

void CWriterController::StopSelectedPlugin()
	{
	LOGTEXT(_L("RWriterController::StopSelectedPlugin - entry"));
    
    CWriterPluginInterface* plugin = GetActiveWriter();
    
	if(plugin)
		{
		plugin->Stop();
		}
	LOGTEXT(_L("RWriterController::StopSelectedPlugin - exit"));
	}

/** Set selected plugin active **/
void CWriterController::SetPluginActive(TUid uid, const TWriterPluginValueKeys aKey)
    {
	CWriterPluginInterface* plugin = NULL;
	_LIT(KDummy, "");
	TBuf<1> buf;
	buf.Append(KDummy);

	for(TInt i(0);i<iPluginArray->Count();i++)
	    {
	    plugin = iPluginArray->At(i);
	    if(plugin->Id().iUid == uid.iUid)
	        {
	        plugin->SetValue(aKey, buf);
	        }
	    else
	        {
	        plugin->SetValue(EWriterPluginDisabled, buf);
	        }
	    }
    }

TInt CWriterController::SetPluginSettings(TUid aUid, TDes& aDes)
    {
	LOGSTRING2(" CWriterController::SetPluginSettings, traceId = 0x%X", aUid.iUid);	
	GetPlugin(aUid)->SetValue(EWriterPluginSettings, aDes);
	return KErrNone;
    }

/** Get settings for a specific plugin **/
void CWriterController::GetPluginSettings(TUid uid, TDes& aVal)
    {
	GetPlugin(uid)->GetValue(EWriterPluginSettings, aVal);
    }


// end of file
