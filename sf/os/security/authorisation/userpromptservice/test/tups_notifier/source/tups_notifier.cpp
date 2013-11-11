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


#include "tups_notifier.h"

#include <barsread2.h>
#include <eikenv.h>
#include <coeaui.h>
#include <techview/eikcmbut.h>
#include <techview/eikbtgpc.h> 
#include <techview/eiktxlbx.h>
#include <techview/eiktxlbm.h>
#include <techview/eiklabel.h>
#include <s32mem.h>

_LIT(KResourceFileName, "z:\\resource\\apps\\tups_notifierapp.rsc");

using namespace UserPromptService;

LOCAL_C void CreateNotifiersL(CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers)

	   {
	   CUpsTestNotifier* notifier = CUpsTestNotifier::NewL();
	   CleanupStack::PushL(notifier);
	   aNotifiers->AppendL(notifier);
	   CleanupStack::Pop(notifier);
	   }

LOCAL_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
	{
	CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers=new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
	if (notifiers)	
		{
		TRAPD(err, CreateNotifiersL(notifiers));
		if(err)
			{
			TInt count = notifiers->Count();
			while(count--)
				{
				(*notifiers)[count]->Release();
				}
			delete notifiers;
			notifiers = NULL;
			}
		}
	   return(notifiers);
	   }

/// Ecom plugin implementation for UI notifier
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KUpsTestNotifierImplementation, NotifierArray)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}

CUpsTestNotifier* CUpsTestNotifier::NewL()
/**
Factory method for creating a techview user prompt service notifier.
@return The new notifier object.
*/
	{
	CUpsTestNotifier* self = new (ELeave) CUpsTestNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUpsTestNotifier::CUpsTestNotifier()
/**
Constructor
*/
	{
	iEikEnv = CEikonEnv::Static();
	iInfo.iUid = TUid::Uid(KUpsTestNotifierImplementation);	
	iInfo.iChannel = TUid::Null();
	iInfo.iPriority = ENotifierPriorityHigh;
	}

void CUpsTestNotifier::ConstructL()
	{
	}

void CUpsTestNotifier::Release()
/**
*/
	{
	iEikEnv->DeleteResourceFile(iResourceFileOffset);
	delete this;
	}

// Called by notifier framework to get implmentation details
CUpsTestNotifier::TNotifierInfo CUpsTestNotifier::RegisterL()
	{
	// Load resource file for access later
	RFs &fs = iEikEnv->FsSession();
	TFileName fileName(KResourceFileName);
	BaflUtils::NearestLanguageFile(fs, fileName);
	iResourceFileOffset = iEikEnv->AddResourceFileL(fileName);

	return iInfo;
	}

CUpsTestNotifier::TNotifierInfo CUpsTestNotifier::Info() const
	{
	return iInfo;
	}

void CUpsTestNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	// Entry point for asynchronous request
	TRAPD(err, DoStartL(aBuffer, aReplySlot, aMessage));
	if(err != KErrNone)
		{
		aMessage.Complete(err);
		}
	}

TPtrC8 CUpsTestNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	// Synchronous operation is not supported.
	User::Leave(KErrNotSupported);
	return TPtrC8();
	}

void CUpsTestNotifier::DoStartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
/**
Displays the problem
@param aBuffer	   The buffer containing the prompt data.
@param aReplySlot  The slot in the messasge to write the result to.
@param aMessage	   The message from the User Prompt Service.
*/
	{
	CPromptData* promptData = CPromptData::NewL();
	CleanupStack::PushL(promptData);

	RDesReadStream rs;
	rs.Open(aBuffer);
	CleanupClosePushL(rs);	
	rs >> *promptData;
	CleanupStack::PopAndDestroy(&rs);

	TPromptResult result;	
	if (CheckScriptedResponseL(result))
		{
		TPckgC<TPromptResult> resultPckg(result);
		aMessage.WriteL(aReplySlot, resultPckg);
		aMessage.Complete(KErrNone);
		}
	else
		{
		CreateAndDisplayDialogL(aMessage, aReplySlot, *promptData);
		}
	CleanupStack::PopAndDestroy(promptData);
	}

TBool CUpsTestNotifier::CheckScriptedResponseL(TPromptResult& aResult)
/**
If a scripted response has been supplied in \upsrefnotifier.txt on the system drive 
then this will be used instead of diplaying a dialog.

@param 	aPromptData	The dialog data supplied by the notifier.
@param 	aResult		The result object to populate if a response has been rescripted.
@return	ETrue, if a response has been scripted; otherwise, EFalse is returned.
*/	
	{
	TBool usedScriptedResponse(EFalse);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TBuf<21> name(_L("!:\\tupsnotifier.txt"));
	name[0] = fs.GetSystemDriveChar();
	RFile file;	
	TInt err = file.Open(fs, name, EFileShareReadersOnly | EFileRead);
	if (err == KErrNone)
		{
		CleanupClosePushL(file);
		TInt size;
		User::LeaveIfError(file.Size(size));
		if (size < 1024)
			{
			RBuf8 buf8;
			buf8.CreateL(size);			
			CleanupClosePushL(buf8);		
			User::LeaveIfError(file.Read(buf8));
			
			RBuf buf;
			buf.CreateL(size);
			CleanupClosePushL(buf);
			buf.Copy(buf8);
			
			aResult.iDestination = KErrNotFound;
			if (buf.FindF(_L("Yes")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::EYes;
				RDebug::Printf("UPS notifier scripted response = Yes");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("No")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::ENo;
				RDebug::Printf("UPS notifier scripted response = No");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("SessionYes")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::ESessionYes;
				RDebug::Printf("UPS notifier scripted response = SessionYes");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("SessionNo")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::ESessionNo;
				RDebug::Printf("UPS notifier scripted response = SessionNo");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("Session")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::ESessionYes;
				RDebug::Printf("UPS notifier scripted response = SessionYes");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("Always")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::EAlways;				
				aResult.iDestination = 0;
				RDebug::Printf("UPS notifier scripted response = Always");
				usedScriptedResponse = ETrue;
				}
			else if (buf.FindF(_L("Never")) != KErrNotFound)
				{
				aResult.iSelected = CPolicy::ENever;
				aResult.iDestination = 0;
				RDebug::Printf("UPS notifier scripted response = Never");
				usedScriptedResponse = ETrue;
				}
			else
				{
				RDebug::Printf("UPS notifier - unknown scripted response = %S", &buf8);
				}
			CleanupStack::PopAndDestroy(2, &buf8);	// buf8, buf
			}
		CleanupStack::PopAndDestroy(&file);
		}
	
	CleanupStack::PopAndDestroy(&fs);	// fs
	return usedScriptedResponse;
	}
	
void CUpsTestNotifier::Cancel()
	{
	}


TPtrC8 CUpsTestNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return TPtrC8();
	}


void CUpsTestNotifier::CreateAndDisplayDialogL(const RMessagePtr2 &aMessage, TInt aReplySlot,
	const CPromptData& aPromptData)
	{
	CEikDialog* d = new (ELeave) CUpsTestNotifierDialog(aMessage, aReplySlot, aPromptData);
	User::LeaveIfError(d->ExecuteLD(R_UPSDLG_DIALOG));
	}

//// Dialog implementation

CUpsTestNotifierDialog::CUpsTestNotifierDialog(const RMessagePtr2 &aMessage, TInt aReplySlot,
											 const CPromptData& aPromptData) : 
	CEikDialog(), 
	iMessage(aMessage),
	iReplySlot(aReplySlot),
	iPromptData(aPromptData)
/**
Constructor
*/
	{
	}

void CUpsTestNotifierDialog::PreLayoutDynInitL()
/**
Setup dialog text
*/
	{
	SetActiveButtonsL();
	
	CEikLabel* label(0);
	TBuf<KDisplayCharWidth> buf;
	HBufC* labelTxt(0);
	
	label = static_cast<CEikLabel*>(Control(EUpsDlgApplicationLabel));
	labelTxt = CEikonEnv::Static()->AllocReadResourceAsDes16LC(R_UPSDLG_APPLICATION_LABEL);
	label->SetTextL(buf);
	AppendTextWithEllipsisL(*labelTxt, label, iPromptData.iClientName);
	CleanupStack::PopAndDestroy(labelTxt);

	label = static_cast<CEikLabel*>(Control(EUpsDlgVendorLabel));
	buf.Zero();
	labelTxt = CEikonEnv::Static()->AllocReadResourceAsDes16LC(R_UPSDLG_VENDOR_LABEL);
	label->SetTextL(buf);
	AppendTextWithEllipsisL(*labelTxt, label, iPromptData.iVendorName);
	CleanupStack::PopAndDestroy(labelTxt);

	label = static_cast<CEikLabel*>(Control(EUpsDlgDestinationLabel));
	buf.Zero();
	labelTxt = CEikonEnv::Static()->AllocReadResourceAsDes16LC(R_UPSDLG_DESTINATION_LABEL);
	label->SetTextL(buf);
	AppendTextWithEllipsisL(*labelTxt, label, *(iPromptData.iDescriptions[0]));
	CleanupStack::PopAndDestroy(labelTxt);

	label = static_cast<CEikLabel*>(Control(EUpsDlgServiceLabel));
	buf.Zero();
	labelTxt = CEikonEnv::Static()->AllocReadResourceAsDes16LC(R_UPSDLG_SERVICE_LABEL);
	label->SetTextL(buf);
	
	HBufC* serviceName = GetServiceNameLC();
	AppendTextWithEllipsisL(*labelTxt, label, *serviceName);	
	CleanupStack::PopAndDestroy(2, labelTxt); // labelText, serviceName
	}
	
void CUpsTestNotifierDialog::SetActiveButtonsL() 
	{	
	CEikButtonGroupContainer& bg = ButtonGroupContainer();		
	CEikCommandButton* btn(0);
			
	btn = bg.ButtonById(EBtnIdYes);	
	btn->SetDimmed((iPromptData.iOptions & CPolicy::EYes) == 0);
	
	btn = bg.ButtonById(EBtnIdNo);	
	btn->SetDimmed((iPromptData.iOptions & CPolicy::ENo) == 0);
	
	btn = bg.ButtonById(EBtnIdSession);	
	btn->SetDimmed((iPromptData.iOptions & CPolicy::ESessionYes) == 0);
	
	btn = bg.ButtonById(EBtnIdAlways);	
	btn->SetDimmed((iPromptData.iOptions & CPolicy::EAlways) == 0);

	btn = bg.ButtonById(EBtnIdNever);	
	btn->SetDimmed((iPromptData.iOptions & CPolicy::ENever) == 0);
	}
	
HBufC* CUpsTestNotifierDialog::GetServiceNameLC()
/**
Gets the localised service name from the dialog resources OR returns a string
containing the service id.
@return A descriptor containing the name of the service.
*/
	{
	HBufC8* resData = CEikonEnv::Static()->AllocReadResourceAsDes8LC(R_SERVICE_DESCRIPTIONS);
	RResourceReader r;
	r.OpenL(*resData);
	CleanupClosePushL(r);
	
	HBufC* serviceName(0);
	TInt n = r.ReadUint16L();
	for (TInt i = 0; i < n; ++i)
		{
		TSecureId sid(r.ReadUint32L());
		TUid serviceId = TUid::Uid(r.ReadUint32L());
		HBufC* name = r.ReadHBufCL();
		if (sid == iPromptData.iServerSid && serviceId == iPromptData.iServiceId)
			{
			serviceName = name;			
			break;
			}
		delete name;
		}
				
	if (! serviceName)	// no localised service name so display service id
		{
		TBuf<10> tmp;
		tmp.AppendFormat(_L("0x%08d"), iPromptData.iServiceId.iUid);		
		serviceName = tmp.AllocL();
		}
	CleanupStack::PopAndDestroy(2, resData);
	CleanupStack::PushL(serviceName);
	return serviceName;
	}

void CUpsTestNotifierDialog::AppendTextWithEllipsisL(
	const TDesC& aTag, CEikLabel* aLabel, const TDesC& aText)
	{
	_LIT(KSEllipsis, "...");
	_LIT(KSeperator, ": ");
	
	TBuf<KDisplayCharWidth> text;
	text.Append(aTag);
	text.Append(KSeperator);
	
	if ((aText.Length() + text.Length())  <= KDisplayCharWidth)
		{
		text.Append(aText);
		aLabel->SetTextL(text);
		}
	else
		{
		text.Append(aText.Left(KDisplayCharWidth - (text.Length() + 3)));
		text.Append(KSEllipsis);
		aLabel->SetTextL(text);
		}
	}

TBool CUpsTestNotifierDialog::OkToExitL(TInt aButtonId)
	{
	UserPromptService::TPromptResult result;
	result.iDestination = KErrNotFound;
	switch(aButtonId)
		{
		case EBtnIdYes:
			result.iSelected = CPolicy::EYes;
			break;
		case EBtnIdNo:
			result.iSelected = CPolicy::ENo;
			break;
		case EBtnIdSession:
			result.iSelected = CPolicy::ESessionYes;
			break;
		case EBtnIdAlways:
			result.iSelected = CPolicy::EAlways;
			result.iDestination = 0;
			break;
		case EBtnIdNever:
			result.iSelected = CPolicy::ENever;
			result.iDestination = 0;
			break;
		default:
			ASSERT(EFalse);
		}	

	TPckgC<TPromptResult> resultPckg(result);
	iMessage.WriteL(iReplySlot, resultPckg);
	iMessage.Complete(KErrNone);
	return ETrue;
	}

// End of file
