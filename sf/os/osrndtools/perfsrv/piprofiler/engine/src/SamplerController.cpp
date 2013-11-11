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


#include "SamplerController.h"
//#include <piprofiler/EngineUIDs.h>

// CONSTANTS
const TInt KMaxSamplerPluginCount = 20;
const TInt KMaxExtraSettingsItemCount = 6;

// LITERALS
_LIT8(KSamplingPeriod, "sampling_period_ms");
_LIT8(KNewLine8, "\n");
_LIT8(KEquals8, "=");
_LIT8(KSettingsText, " settings");
_LIT(KNewLine, "\n");
_LIT(KEquals, "=");
_LIT(KCommentSeparator, " ; ");

CSamplerController* CSamplerController::NewL(CProfilerSampleStream& aStream)
    {
    CSamplerController* self = new( ELeave ) CSamplerController(aStream);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

void CSamplerController::ConstructL()
	{
	// initiate sampler plugin list
	InitialiseSamplerListL();
	}


CSamplerController::CSamplerController(CProfilerSampleStream& aStream) : 
    iStream(aStream)
	{
	}

void CSamplerController::InitialiseSamplerListL()
    {
    // create new sampler plugin array
    iPluginArray = new (ELeave) CArrayPtrFlat<CSamplerPluginInterface>( KMaxSamplerPluginCount );
    
    // create plugin loader instance
    iPluginLoader = CSamplerPluginLoader::NewL();
    
    // register sampler controller to get notifications of succesfull plugin load
    iPluginLoader->SetObserver( this );
    
    // load sampler plugins asynchronously
    iPluginLoader->LoadAsyncL( iPluginArray );
    
    LOGTEXT(_L(" RSamplerController::InitialiseUserSideSamplerList - exit"));	
    }

CSamplerController::~CSamplerController()
	{
	LOGTEXT(_L("CSamplerController::~CSamplerController - entry" ));
	
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
    
	LOGTEXT(_L("CSamplerController::~CSamplerController - exit" ));
	}

void CSamplerController::SetObserver(MSamplerControllerObserver* aObserver)
    {
    iObserver = aObserver;
    }
    
TInt CSamplerController::UpdateSavedSamplerAttributesL(CDesC8ArrayFlat* aSavedLineArray, CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    TInt err(KErrNone);
    TInt count(iPluginArray->Count());
    // all plugins get their own settings among whole lump of setting strings
    CSamplerPluginInterface* plugin = NULL;
    
    // loop through the plugin array
    for(TInt i(0);i<count;i++)
        {
        // get each plugin at a time
        plugin = iPluginArray->At(i);
        
        // call each plugin to sort out its own settings 
        err = plugin->ConvertRawSettingsToAttributes(aSavedLineArray);
        
        // get plugin specific attributes, array may contain attributes of several sub samplers
        plugin->GetAttributesL(aAttributes); 
        }
    
    return err;
    }

TInt CSamplerController::SetSamplerSettingsL(TInt aUid, TSamplerAttributes& aAttributes)
    {
    // parse right plugin based on UID
    CSamplerPluginInterface* plugin = GetPlugin(TUid::Uid(aUid));
    
    // set the sampler attributes of a sampler plugin
    plugin->SetAttributesL(aAttributes);
    
    return KErrNone;
    }

void CSamplerController::GetSamplerAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    CSamplerPluginInterface* plugin = NULL;
    
    TInt count(iPluginArray->Count());
    
    // get first all the attributes from all the sampler plugins listed in iPluginArray
    for(TInt i(0);i<count;i++)
        {
        // get the plugin first
        plugin = iPluginArray->At(i);
        
        // get plugin specific attributes, array may contain attributes of several sub samplers
        plugin->GetAttributesL(aAttributes); 
        }
    }

void CSamplerController::ComposeAttributesToSettingsFileFormat(RFile& aFile, CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    // write immediately to settings file
    ComposeSettingsText(aFile, aAttributes);
    }

void CSamplerController::ComposeSettingsText(RFile& aFile, CArrayFixFlat<TSamplerAttributes>* aAttrArray)
    {
    // temporary buffer for a setting line
    TBuf<384> settingLine;
    TBuf8<384> settingLine8;
    TInt itemCount(0);
    TBuf<266> tBuf;
    
    TSamplerAttributes attr;
    
    for(TInt i(0);i<aAttrArray->Count();i++)
        {
        // get the attribute container 
        attr = aAttrArray->At(i);
        
        // add the name and description of the sampler in brackets first
        settingLine8.Copy(KBracketOpen);
        settingLine8.Append(attr.iShortName);
        settingLine8.Append(KBracketClose);
        settingLine8.Append(KCommentSeparator());
        settingLine8.Append(attr.iName);
        settingLine8.Append(KSettingsText);
        settingLine8.Append(KNewLine8);
        aFile.Write(settingLine8);
        
        // enabled
        settingLine8.Copy(KEnabled);
        settingLine8.Append(KEquals8);
        settingLine8.Append(Bool2Str(attr.iEnabled));
        settingLine8.Append(KNewLine8);
        aFile.Write(settingLine8);
        
        // sampling rate (if set)
        if( attr.iSampleRate != -1 )
            {
            settingLine8.Copy(KSamplingPeriod);
            settingLine8.Append(KEquals8);
            settingLine8.Append(Int2Str(attr.iSampleRate));
            settingLine8.Append(KNewLine8);
            aFile.Write(settingLine8);
            }
        
        itemCount = attr.iItemCount;
        
        // check if item count set is sane, max extra settings item count 6
        if(itemCount > KMaxExtraSettingsItemCount)
            {
            // probably forgot to set the item count value in plugin => safe to set it 0
            itemCount = 0;
            }
        
        // setting items
        for (TInt j(0);j<itemCount;j++)
            {
            switch(j)
                {
                case 0: // settingItem1
                    {
                    settingLine.Copy(attr.iSettingItem1.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem1.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem1.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                case 1: // settingItem2
                    {
                    settingLine.Copy(attr.iSettingItem2.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem2.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem2.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                case 2: // settingItem3
                    {
                    settingLine.Copy(attr.iSettingItem3.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem3.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem3.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                case 3: // settingItem4
                    {
                    settingLine.Copy(attr.iSettingItem4.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem4.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem4.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                case 4: // settingItem5
                    {
                    settingLine.Copy(attr.iSettingItem5.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem5.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem5.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                case 5: // settingItem6
                    {
                    settingLine.Copy(attr.iSettingItem6.iSettingText);
                    settingLine.Append(KEquals());
                    settingLine.Append(attr.iSettingItem6.iValue);
                    settingLine.Append(KCommentSeparator());
                    settingLine.Append(attr.iSettingItem6.iUIText);
                    settingLine.Append(KNewLine());
                    CnvUtfConverter::ConvertFromUnicodeToUtf8(settingLine8, settingLine);
                    aFile.Write(settingLine8);
                    break;
                    }
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TBool value.
// ----------------------------------------------------------------------------
//
inline void CSamplerController::Str2Bool(const TDesC8& aBuf, TBool& aValue)
    {
    if (aBuf.CompareF(KFalse) == 0)
        aValue = EFalse;
    else
        aValue = ETrue;
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TInt value.
// ----------------------------------------------------------------------------
//
inline void CSamplerController::Str2Int(const TDesC8& aBuf, TInt& aValue)
    {
    TLex8 conv;
    conv.Assign(aBuf);
    
    if (conv.Val(aValue) != KErrNone)
        aValue = 0;
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TInt value.
// ----------------------------------------------------------------------------
//
inline void CSamplerController::Str2Int(const TDesC8& aBuf, TUint32& aValue)
    {
    TInt temp(0);
    
    TLex8 conv;
    conv.Assign(aBuf);
    
    if (conv.Val(temp) != KErrNone)
        aValue = 0;
    else
        aValue = (TUint32)temp;
    }

// ----------------------------------------------------------------------------
// Converts given boolean into a descriptor.
// ----------------------------------------------------------------------------
//
inline TBuf8<16> CSamplerController::Bool2Str(const TBool& aValue)
    {
    TBuf8<16> buf;
    
    if (aValue)
        buf.Copy(KTrue);
    else
        buf.Copy(KFalse);
    
    return buf;
    }

// ----------------------------------------------------------------------------
// Converts given integer into a descriptor.
// ----------------------------------------------------------------------------
//
inline TBuf8<16> CSamplerController::Int2Str(const TInt& aValue)
    {
    TBuf8<16> buf;
    buf.AppendNum(aValue);
   
    return buf;
    }


void CSamplerController::HandlePluginLoaded( KSamplerPluginLoaderStatus aStatus )
    {
    
    // process status value
    switch(aStatus)
        {
        case 0:
            LOGSTRING2("RSamplerController - one plugin loaded, status: %d", aStatus);
            break;
        case 1:
            LOGSTRING2("RSamplerController - a plugin load failed: %d", aStatus);
            break;
        case 2:
            LOGSTRING2("RSamplerController - plugin loading aborted: %d", aStatus);
            break;
        case 3:
            LOGSTRING2("RSamplerController - all plugins loaded: %d", aStatus);
            TRAPD(err, iPluginLoader->SortPluginsL(iPluginArray));
            if(err != KErrNone)
                {
                LOGTEXT(_L("Sampler controller unable to sort plugins"));
                }
            
            // call engine to finalize the startup
            TRAPD(result, iObserver->HandleSamplerControllerReadyL(););
            if(result != KErrNone)
                {
                LOGTEXT(_L("Failed to notify engine"));
                }
            break;
        case 4:
            LOGSTRING2("RSamplerController - error in loading plugins: %d", aStatus);
            break;
        default:
            break;
        }
    }


CSamplerPluginInterface* CSamplerController::GetPlugin(TUid aUid)
	{
	LOGTEXT(_L("RSamplerController::GetPlugin - entry"));
	// check that plugin array contains samplers
	if( iPluginArray && iPluginArray->Count() > 0 )
	  {
    	for(TInt i=0;i<iPluginArray->Count();i++)
    		{
	    	CSamplerPluginInterface* plugin = iPluginArray->At(i);
	    	TUid uid = plugin->Id(-1);	// get parent uid first
	    	if(uid == aUid)
	    		{
	    		LOGTEXT(_L("CSamplerController::GetPlugin() - main plug-in found!"));
    			return plugin;
				} 

            if(plugin->SubId(aUid) != KErrNotFound)
                {
                LOGTEXT(_L("CSamplerController::GetPlugin() - subsampler found!"));
                return plugin;
                }
    		}
		}	
	LOGTEXT(_L("CSamplerController::GetPlugin() - No plug-in found for UID"));

	return (CSamplerPluginInterface*)0;
	}

// start user mode samplers
void CSamplerController::StartSamplerPluginsL()
    {
    CSamplerPluginInterface* plugin = NULL;

	if( iPluginArray )
		{
		TInt count(iPluginArray->Count());
	
		for(TInt i(0);i<count;i++)
			{
			plugin = iPluginArray->At(i);
			// check if some error received when starting profiling
			TRAPD(err, plugin->ResetAndActivateL(iStream));
			LOGSTRING2(" RSamplerController::StartSamplerPlugin - plugin activated (0x%X)", plugin->Id(-1));  
			if( err != KErrNone)
				{
				LOGSTRING2(" RSamplerController::StartSamplerPlugin - error %d", i);  
				// handle received error, need to update UI!
				iObserver->HandleError(err);
				}
            }
	    }
    }

// stop user mode samplers
TInt CSamplerController::StopSamplerPlugins()
	{
	TInt count(0);

	if( iPluginArray && iPluginArray->Count() > 0 )
		{
		TInt i(0);
		CSamplerPluginInterface* plugin = NULL;
		// stop kernel mode samplers
    	for(;i<iPluginArray->Count();i++)
    		{
			plugin = iPluginArray->At(i); 
			TUint32 id = plugin->Id(-1).iUid;
            LOGSTRING2(" CSamplerController::StopSamplerPlugins - traceId = %d",
                        id);
            // stop only started samplers
            if(plugin->Enabled())
                {
                // stop selected plugin
                plugin->StopSampling();
                // check if user mode sampler, special flush needed to direct data to stream
                if(plugin->GetSamplerType() == PROFILER_USER_MODE_SAMPLER)
                    {
                    LOGTEXT(_L(" CSamplerController::StopSamplerPlugins - flushing user mode sampler stream"));
                    plugin->Flush();
                    }
                }
            count++;
    		}
		}
	return count; 	
	}

// end of file




