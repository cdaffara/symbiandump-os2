// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32mem.h>
#include "uloggersession.h"
#include "uloggershared.h"
#include "uloggerclient.h"
#include "uloggershared.h"
#include "uloggertools.h"

namespace Ulogger {


CULoggerSession* CULoggerSession::NewL(RThread& aClient, CULoggerServer& aServer)
	{
	CULoggerSession* self = CULoggerSession::NewLC(aClient, aServer);
	CleanupStack::Pop(); //self
	return self;
	}//</NewL>




CULoggerSession* CULoggerSession::NewLC(RThread& aClient, CULoggerServer& aServer)
	{
	CULoggerSession* self = new(ELeave) CULoggerSession( aClient, aServer );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}//</NewLC>




void CULoggerSession::ConstructL()
	{
	iServer.IncrementSessions();
	}//</ConstructL>



CULoggerSession::CULoggerSession(RThread& aClient, CULoggerServer& aServer)
: CSession2(),
iServer( aServer ),
iClient( aClient )
	{

	}//</constructor>



CULoggerSession::~CULoggerSession()
{
	iServer.DecrementSessions();
}//</destructor>






/** Process message from 'client' (actually another instance of ourselves)
*/
void CULoggerSession::ServiceL(const RMessage2& aMessage)
	{
	TInt error = KErrNone;
	switch(aMessage.Function())
		{
		case ERunAsService:
			{
			iServer.RunAsService(ETrue);
			error = KErrNone;
			}
		break;

		case EDontRunAsService:
			{
			iServer.RunAsService(EFalse);
			error = KErrNone;
			}
		break;

		case EStart:
			{
			//Starts the BTrace
			//r= iServer.Start();
			if(!iServer.ServerState())
				error = iServer.RestartOutputting();
			else
				error = KErrInUse;
			}
		break;
		
		case EStop:
			{		
			//Stops the BTrace
			error = iServer.Stop();
			//iServer.UnconfigureCurrentChannel();
			}
		break;
		
		case ERestart:
			{
			error = iServer.RestartOutputting();
			}
		break;
		
		case ESetPrimaryFilter:
			{
			//Set filter 1
			RArray<TUint32> filterArray;
			CleanupClosePushL(filterArray);

			TInt elementsCount = aMessage.Int1();
			if(elementsCount > 0)
				{
				HBufC8* desData = HBufC8::NewLC(elementsCount);
				TPtr8 readPtr(desData->Des());
				aMessage.ReadL(0, readPtr);
		
				CArrayFixFlat<TUint8> *array = new (ELeave) CArrayFixFlat<TUint8>(1);
				CleanupStack::PushL(array);
		
				TUint8 tmp(1);
				InternalizeFromBufL(readPtr, *array, tmp);
		
				for(TInt i(0); i < array->Count(); ++i)
					{
					filterArray.AppendL((TUint32)array->At(i));
					}
			
				error = iServer.SetActiveFilterL(filterArray, EPrimaryFilter);

				CleanupStack::PopAndDestroy(3, &filterArray); // and array, desData
				}
			else
				{
				error = KErrArgument;
				}
			}
			break;

		case ERemovePrimaryFilter:
			{
			//remove primaryfilters
			TInt elementsCount = aMessage.Int1();
			if(elementsCount > 0)
				{
				RArray<TUint32> filterArray;
				CleanupClosePushL(filterArray);
				HBufC8* desData = HBufC8::NewLC(elementsCount);
				TPtr8 readPtr(desData->Des());
				aMessage.ReadL(0, readPtr);

				CArrayFixFlat<TUint8> *array = new (ELeave) CArrayFixFlat<TUint8>(1);
				CleanupStack::PushL(array);

				TUint8 tmp(1);
				InternalizeFromBufL(readPtr, *array, tmp);

				for(TInt i(0); i < array->Count(); ++i)
					{
					filterArray.AppendL((TUint32)array->At(i));
					}

				error = iServer.RemoveActiveFilter(filterArray,EPrimaryFilter);

				CleanupStack::PopAndDestroy(3, &filterArray); // and array, desData
				}
			else
				{
				error = KErrArgument;
				}
			}
			break;
		
		case EGetPrimaryFilters:
			{
			//Get primaryfilters
			RArray<TUint32> listBuffer;
			error = iServer.GetActiveFilters(listBuffer,EPrimaryFilter);

			CArrayFixFlat<TUint8> *fixArray = new (ELeave)CArrayFixFlat<TUint8>(1);
			CleanupStack::PushL(fixArray);

			for(TInt i=0; i<listBuffer.Count(); ++i)
				fixArray->AppendL((TUint8)listBuffer[i]);
			HBufC8* desData = ExternalizeToBufL((const CArrayFix<TUint8>&) *fixArray, sizeof(TUint8));
			CleanupStack::PopAndDestroy(); //fixArray

			aMessage.Write(0,*desData);

			delete desData;
			desData = NULL;
			listBuffer.Close();
			}
			break;
		case ESetSecondaryFilter:
			{
			//Set filter 2
			TInt elementsCount = aMessage.Int1();
			if(elementsCount > 0)
				{
				HBufC8* desData = HBufC8::NewLC(elementsCount*sizeof(TUint32));
				TPtr8 readPtr(desData->Des());
				aMessage.ReadL(0, readPtr);
			
				RArray<TUint32> filterArray;
				TUint32 tmp=1;
				InternalizeFromBufL(readPtr, filterArray, tmp);
					
				error = iServer.SetActiveFilterL(filterArray,ESecondaryFilter);

				CleanupStack::PopAndDestroy(); //desData
				filterArray.Close();
				}
			else
				error = KErrArgument;
			}
			break;
		
		case ERemoveSecondaryFilter:
			{
			//remove secondaryfilters
			TInt elementsCount = aMessage.Int1();
			if(elementsCount > 0)
				{
				HBufC8* desData = HBufC8::NewLC(elementsCount*sizeof(TUint32));
				TPtr8 readPtr(desData->Des());
				aMessage.ReadL(0, readPtr);

				RArray<TUint32> filterArray;

				TUint32 tmp=1;
				InternalizeFromBufL(readPtr, filterArray, tmp);

				error = iServer.RemoveActiveFilter(filterArray,ESecondaryFilter);

				CleanupStack::PopAndDestroy(); //desData
				filterArray.Close();
				}
			else
				error = KErrArgument;
			}
			break;

		case EGetSecondaryFilters:
			{		
			//Get secondary filters
			RArray<TUint32> listBuffer;
			error=iServer.GetActiveFilters(listBuffer,ESecondaryFilter);

			HBufC8* desData = ExternalizeToBufL(listBuffer, sizeof(TUint32));
			TPtr8 readPtr2(desData->Des());
			aMessage.Write(0,readPtr2);

			delete desData;
			desData = NULL;
			listBuffer.Close();
			}
		break;

		case EEnableSecondaryFiltering:
			{
			error = iServer.SetSecondaryFiltering(KEnable);
			}
			break;

		case EDisableSecondaryFiltering:
			{
			error = iServer.SetSecondaryFiltering(KDisable);
			}
			break;

		case EGetSecondaryFiltering:
			{
			TBool enabled = EFalse;
			iServer.GetSecondaryFiltering(enabled);
			SendIntegerL(aMessage, (TInt)enabled);
			}
			break;

		case ESetActivePlugin:
			{
			//Set the active plugin
			HBufC8* pluginName = NULL;
			TInt dataSize1 = aMessage.GetDesLength(0);
			if(dataSize1 > 0)
				{
				pluginName = HBufC8::NewLC( dataSize1 );
				TPtr8 namePtr(pluginName->Des());
				aMessage.ReadL(0,namePtr);
				error = iServer.SetActiveOutputPlugin(pluginName->Des());
				CleanupStack::PopAndDestroy(1);
				}
			else
				error = KErrArgument;
			}
			break;

		case ESetActiveInputPlugin:
			{
			//Set the active control plugin
			HBufC8* pluginName = NULL;
			TInt dataSize1 = aMessage.GetDesLength(0);
			if(dataSize1 > 0)
				{
				pluginName = HBufC8::NewLC( dataSize1 );
				TPtr8 namePtr(pluginName->Des());
				aMessage.ReadL(0,namePtr);
				error = iServer.SetActiveInputPlugin(pluginName->Des());
				CleanupStack::PopAndDestroy(1); //pluginName
				}
			else
				error = KErrArgument;
			}
		break;

		case ESetPluginSettings:
			{
			//Set the channel settings
			HBufC8* pluginName = NULL;
			HBufC8* setting = NULL;
			
			RArray<TPtrC8> value;
			TInt dataSize1 = aMessage.GetDesLength(0);
			TInt dataSize2 = aMessage.GetDesLength(1);
			CArrayPtrFlat<HBufC8> *ptrArray = new (ELeave) CArrayPtrFlat<HBufC8>(1);
			CleanupStack::PushL(ptrArray);
			
			if(dataSize1>0 && dataSize2>0)
				{
				pluginName = HBufC8::NewLC( dataSize1 );
				setting = HBufC8::NewLC( dataSize2 );
				TPtr8 data1Ptr(pluginName->Des());		
				TPtr8 data2Ptr(setting->Des());	
				
				aMessage.ReadL(0,data1Ptr);	
				aMessage.ReadL(1,data2Ptr);	
				
				_LIT8(KSeparator,";");
				TInt pos = KErrNone;
				
				while(pos != KErrNotFound)
					{
					pos = setting->Des().Find(KSeparator);
					if(pos > 0)	
						{
						TPtrC8 ptrVal(setting->Des().Left(pos));
						HBufC8 *bufDes = HBufC8::NewLC(ptrVal.Length()+8);
						bufDes->Des().Copy(ptrVal);
						CleanupStack::Pop(bufDes);
						ptrArray->AppendL(bufDes);
						setting->Des().Delete(0,pos+1);
						}
					}
				
				for(TInt i=0; i<ptrArray->Count(); ++i)
					value.AppendL(*(ptrArray->At(i)));
				error = iServer.SetPluginSettings(pluginName->Des(), value);
				CleanupStack::PopAndDestroy(2);
				}
			else
				error = KErrArgument;

			CleanupStack::PopAndDestroy(ptrArray);
			}
			break;

		case ERemovePluginSettings:
			{
			TInt dataSize = aMessage.GetDesLength(0);
			TBuf8<KMaxPluginName> pluginName;
			if (dataSize > 0)
				{
				aMessage.ReadL(0, pluginName);
				error = iServer.RemovePluginSettingsL(pluginName);
				}
			else
				error = KErrArgument;
			}
		break;

		case EGetPluginSettings:
			{
			TInt dataSize1 = aMessage.GetDesLength(0);
			TBuf8<128> mediaName;			

			if(dataSize1 > 0)
				{
				aMessage.ReadL(0, mediaName);
				RArray<TPtrC8> tmpArray;
				error = iServer.GetOptionsSettingsL(mediaName,tmpArray);

				//<rewrite array and add separator to each record>
				_LIT8(KFormatString,"%S;");//';' is a separator
				RArray<TPtrC8> tmpArray2;
				//count total_length + separators
				TInt length = 1;
				TInt i=0;
				for(i=0; i<tmpArray.Count(); ++i)
					length+=tmpArray[i].Length()+1;

				HBufC8* b = HBufC8::NewLC(length);
				for(i=0; i<tmpArray.Count(); ++i)
					b->Des().AppendFormat(KFormatString, &tmpArray[i]);
				//</rewrite array and add separator to each record>

				aMessage.Write(1,*b);
				CleanupStack::PopAndDestroy(); // b
				}
			else
				error = KErrArgument;
			}
			break;

		case EGetInstalledPlugins:
			{
			//Get the installed plugins	
				RArray<TPtrC8> tmpArray;
				iServer.GetInstalledOutputPlugins(tmpArray);	

				//<rewrite array and add separator to each record>
				_LIT8(KFormatString,"%S;");//';' is a separator
				RArray<TPtrC8> tmpArray2;
				//count total_length + separators
				TInt length = 1;
				TInt i=0;
				for(i=0; i<tmpArray.Count(); ++i)
					length+=tmpArray[i].Length()+1;

				HBufC8* b = HBufC8::NewLC(length);
				for(i=0; i<tmpArray.Count(); ++i)
					b->Des().AppendFormat(KFormatString, &tmpArray[i]);
				//</rewrite array and add separator to each record>

				aMessage.Write(0,*b);
				CleanupStack::PopAndDestroy(); // b	
			}
		break;

		case EGetInputPlugins:
			{
			//Get the installed plugins	
			RArray<TPtrC8> tmpArray;
			error = iServer.GetInstalledInputPluginsL(tmpArray);
			if(!error)
				{
			//<rewrite array and add separator to each record>
			_LIT8(KFormatString,"%S;");//';' is a separator
			//count total_length + separators
			TInt length = 1;
			TInt i=0;
			for(i=0; i<tmpArray.Count(); ++i)
				length+=tmpArray[i].Length()+1;

			HBufC8* b = HBufC8::NewLC(length);
			for(i=0; i<tmpArray.Count(); ++i)
				b->Des().AppendFormat(KFormatString, &tmpArray[i]);
			//</rewrite array and add separator to each record>

			aMessage.Write(0,*b);
			CleanupStack::PopAndDestroy(); // b	
				}
			}
			break;

		case EGetActivePlugin:
			{
			//Reads the active plugin
			RArray<TPtrC8> tmpArray;
			error = iServer.GetActiveOutputPlugin(tmpArray);
			if(!error)
				{
				//<rewrite array and add separator to each record>
				_LIT8(KFormatString,"%S;");//';' is a separator
				RArray<TPtrC8> tmpArray2;
				//count total_length + separators
				TInt length = 1;
				TInt i=0;
				for(i=0; i<tmpArray.Count(); ++i)
					length+=tmpArray[i].Length()+1;
	
				HBufC8* b = HBufC8::NewLC(length);
				for(i=0; i<tmpArray.Count(); ++i)
					b->Des().AppendFormat(KFormatString, &tmpArray[i]);
				//</rewrite array and add separator to each record>
	
				aMessage.Write(0,*b);
				CleanupStack::PopAndDestroy(); // b
				}
			}
			break;

		case EGetActiveInputPlugin:
			{
			RArray<TPtrC8> tmpArray;
			error = iServer.GetActiveInputPlugin(tmpArray);
			if(!error)
				{
				if(tmpArray.Count() == 0)
					error = KErrNotFound;
				//<rewrite array and add separator to each record>
				_LIT8(KFormatString,"%S;");//';' is a separator
				RArray<TPtrC8> tmpArray2;
				//count total_length + separators
				TInt length = 1;
				TInt i=0;
				for(i=0; i<tmpArray.Count(); ++i)
					length+=tmpArray[i].Length()+1;

				HBufC8* b = HBufC8::NewLC(length);
				for(i=0; i<tmpArray.Count(); ++i)
					b->Des().AppendFormat(KFormatString, &tmpArray[i]);
				//</rewrite array and add separator to each record>

				aMessage.Write(0,*b);
				CleanupStack::PopAndDestroy(); // b
				}
			}
		break;

		case EDeactivateInputPlugin:
			{
			HBufC8* pluginName = NULL;
			TInt dataSize1 = aMessage.GetDesLength(0);
			if(dataSize1 > 0)
				{
				pluginName = HBufC8::NewLC( dataSize1 );
				TPtr8 namePtr(pluginName->Des());
				aMessage.ReadL(0,namePtr);
				error = iServer.DeActivateInputPlugin(pluginName->Des());
				CleanupStack::PopAndDestroy(); //pluginName
				}
			}
			break;
		case EResizeTraceBuffer:
			{
			//Resize buffer size
			TInt bufferSize = aMessage.Int0();
			error = iServer.SetBufferSizeL(bufferSize);
			}
			break;

		case EGetTraceBufferSize:
			{
			//Get buffer size
			TInt bufferSize = 0;
			error = iServer.GetBufandDataNotifyValuesL(KBuffer,bufferSize);
			SendIntegerL(aMessage, bufferSize);
			}
			break;

		case ESetBufferMode:
			{
			TInt bufferMode = aMessage.Int0();
			if(bufferMode == ECircularBuffer)
				error  = iServer.SetBufferMode(KCircular);
			else if(bufferMode == EStraightBuffer)
				error  = iServer.SetBufferMode(KStraight);
			else
				error = KErrArgument;
			}
			break;

		case EGetBufferMode:
			{
			//Get buffer size
			TInt bufferMode = 3;
			error = iServer.GetBufandDataNotifyValuesL(KBufferMode,bufferMode);
			SendIntegerL(aMessage, bufferMode);
			}
			break;

		case ESetDataNotificationSize:
			{
			//Set data notification size
			TInt dataNotification = aMessage.Int0();
			error = iServer.SetDataNotificationSizeL(dataNotification);
			}
			break;

		case EGetDataNotificationSize:
			{
			//Get Data Notification size
			TInt dataNotification = 0;
			error = iServer.GetBufandDataNotifyValuesL(KDataNotification,dataNotification);
			SendIntegerL(aMessage, dataNotification);
			}
			break;

		default:
			{
		    error = KErrArgument;
			}
			break;
		}
    aMessage.Complete(error);
	}

void CULoggerSession::SendIntegerL(const RMessage2& aMessage, TUint32 aInt)
	{
	HBufC8* size = HBufC8::NewLC(32);
	size->Des().Num(aInt);
	TPtr8 ptr(size->Des());
	aMessage.Write(0,ptr);
	CleanupStack::PopAndDestroy();
	}
}//namespace
