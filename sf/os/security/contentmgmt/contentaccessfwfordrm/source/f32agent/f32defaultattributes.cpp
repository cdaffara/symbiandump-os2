/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <apgcli.h>
#include <f32file.h>

#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/virtualpath.h>
#include <caf/caferr.h>
#include "f32defaultattributes.h"


using namespace ContentAccess;

TInt TF32DefaultAttributes::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& )
	{
	TInt err = KErrNone;
	// All files handled by the F32 agent are unprotected and have the same
	// standard set of attributes
	switch(aAttribute)
		{
		case EIsForwardable:
		case EIsModifyable:
		case EIsCopyable:
		case ECanPlay:
		case ECanPrint:
		case ECanExecute:
		case ECanView:
		case ECanRewind:
		case ECopyPaste:
		case ECanMove:
		case ECanRename:
		case ECanAutomaticConsume:
			aValue = ETrue;
			break;
		case EIsMediaPlayerOnly:
		case EIsAutomatedUseOnly:
		case EIsProtected:			
		case EPreviewAvailable:
			aValue = EFalse;
			break;
		case ETrackNumber:
		case EContentCDataInUse:
		case EContentVersion:
		default:
			err = KErrCANotSupported;
			break;
		};
	return err;
	}

TInt TF32DefaultAttributes::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUri) 
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt value=0;
	TInt err = KErrNone;
	TInt numAttributes = aAttributeSet.Count();
	
	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		attribute = aAttributeSet[i];
		err = GetAttribute(attribute, value, aUri);
		aAttributeSet.SetValue(attribute, value, err);
		}	
	return KErrNone;
	}

TInt TF32DefaultAttributes::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUri)
	{
	TInt err = KErrNone;
	TBuf8 <KMaxDataTypeLength> mimeType;

	switch(aAttribute)
		{
		case EMimeType:
			TRAP(err, GetMimeTypeL(aUri, mimeType));
			if(err == KErrNone)
				{
				aValue.Copy(mimeType);
				}
			break;
		default:
			err = KErrCANotSupported;
			break;
		};
	return err;
	}

TInt TF32DefaultAttributes::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUri)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt err = KErrNone;
	TBuf <KMaxDataTypeLength> buf;

	TInt numAttributes = aStringAttributeSet.Count();

	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		buf.SetLength(0);
		attribute = aStringAttributeSet[i];
		err = GetStringAttribute(attribute, buf, aUri);
		err = aStringAttributeSet.SetValue(attribute,buf, err);
		if(err != KErrNone)
			{
			return err;
			}
		}	
	return KErrNone;
	}

void TF32DefaultAttributes::GetMimeTypeL(const TDesC& aURI, TDes8& aMimeType)
	{
	TUid appUid(KNullUid);
	TDataType dataType;
	RApaLsSession apparcSession;
	
	// Connect to Apparc
	User::LeaveIfError(apparcSession.Connect());
	CleanupClosePushL(apparcSession);

	User::LeaveIfError(apparcSession.AppForDocument(aURI, appUid, dataType));
	
	if(dataType.Des8().Length() == 0)
		{
		User::Leave(KErrNotFound);
		}

	aMimeType.Append(dataType.Des8());
	CleanupStack::PopAndDestroy(&apparcSession);
	}


TInt TF32DefaultAttributes::GetAttribute(TInt aAttribute, TInt& aValue, RFile& /*aFile*/)
	{
	return GetAttribute(aAttribute, aValue, KNullDesC);
	}
	
TInt TF32DefaultAttributes::GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt value = 0;
	TInt err = KErrNone;
	TInt numAttributes = aAttributeSet.Count();
	
	// loop through all the attributes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		attribute = aAttributeSet[i];
		err = GetAttribute(attribute, value, aFile);
		aAttributeSet.SetValue(attribute, value, err);
		}	
	return KErrNone;
	}
	
TInt TF32DefaultAttributes::GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile)
	{
	TInt err = KErrNone;
	TBuf8 <KMaxDataTypeLength> mimeType;

	switch(aAttribute)
		{
		case EMimeType:
			TRAP(err, GetMimeTypeL(aFile, mimeType));
			if(err == KErrNone)
				{
				aValue.Copy(mimeType);
				}
			break;
		default:
			err = KErrCANotSupported;
			break;
		};
	return err;
	}
	
TInt TF32DefaultAttributes::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt err = KErrNone;
	TBuf <KMaxDataTypeLength> buf;

	TInt numAttributes = aStringAttributeSet.Count();

	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		buf.SetLength(0);
		attribute = aStringAttributeSet[i];
		err = GetStringAttribute(attribute, buf, aFile);
		err = aStringAttributeSet.SetValue(attribute,buf, err);
		if(err != KErrNone)
			{
			return err;
			}
		}	
	return KErrNone;
	}
	
void TF32DefaultAttributes::GetMimeTypeL(RFile& aFile, TDes8& aMimeType)
	{
	TUid appUid(KNullUid);
	TDataType dataType;
	RApaLsSession apparcSession;
	
	// Connect to Apparc
	User::LeaveIfError(apparcSession.Connect());
	CleanupClosePushL(apparcSession);

	User::LeaveIfError(apparcSession.AppForDocument(aFile, appUid, dataType));
	
	if(dataType.Des8().Length() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	aMimeType.Append(dataType.Des8());
	CleanupStack::PopAndDestroy(&apparcSession);
	}

TInt TF32DefaultAttributes::CheckUniqueId(const TDesC& aUniqueId)
	{
	// The only UniqueId values that make sense for the F32 agent are
	// a zero length descriptor (indicating the application is referring to the entire file 
	// or KDefaultContentObject which is also the entire file in the case of the F32 agent
	if(aUniqueId.Length() == 0 || aUniqueId == KDefaultContentObject())
		{
		return KErrNone;	
		}
	else 
		{
		return KErrNotFound;	
		}
	}
	
TInt TF32DefaultAttributes::CheckVirtualPath(const TVirtualPathPtr& aVirtualPath)
	{
	// check the Unique Id
	TInt err = CheckUniqueId(aVirtualPath.UniqueId());
	
	// check if the file exists
	
	
	return err;
	}	
	
TUint TF32DefaultAttributes::GetFileMode(TContentShareMode aMode)
	{
	
	TUint fileMode = EFileStream | EFileRead;

	if(aMode  == EContentShareReadWrite)
		{
		fileMode |= EFileShareReadersOrWriters;
		}
	else if(aMode == EContentShareExclusive)
		{
		fileMode  |= EFileShareExclusive;
		}
	else
		{
		fileMode |= EFileShareReadersOnly;
		}
		
	return fileMode;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
TInt TF32DefaultAttributes::GetAttribute(const TDesC8& /*aHeaderData*/, TInt aAttribute, TInt& aValue)
	{
	return GetAttribute(aAttribute, aValue, KNullDesC);
	}
	
TInt TF32DefaultAttributes::GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt value = 0;
	TInt err = KErrNone;
	TInt numAttributes = aAttributeSet.Count();
	
	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		attribute = aAttributeSet[i];
		err = GetAttribute(aHeaderData, attribute, value);
		aAttributeSet.SetValue(attribute, value, err);
		}	
	return KErrNone;
	}
	
TInt TF32DefaultAttributes::GetStringAttribute(const TDesC8& /*aHeaderData*/, TInt aAttribute, TDes& aValue)
	{
	TInt err = KErrNone;
	TBuf8 <KMaxDataTypeLength> mimeType;

	switch(aAttribute)
		{
		case EMimeType:
			aValue.Copy(KNullDesC());
			break;
		case EContentID:
			aValue.Copy(KDefaultContentObject());
			break;
		default:
			err = KErrCANotSupported;
			break;
		};
	return err;
	}
	
TInt TF32DefaultAttributes::GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt err = KErrNone;
	TBuf <KMaxDataTypeLength> buf;

	TInt numAttributes = aStringAttributeSet.Count();

	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		buf.SetLength(0);
		attribute = aStringAttributeSet[i];
		err = GetStringAttribute(aHeaderData, attribute, buf);
		err = aStringAttributeSet.SetValue(attribute, buf, err);
		if(err != KErrNone)
			{
			return err;
			}
		}	
	return KErrNone;
	}

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
