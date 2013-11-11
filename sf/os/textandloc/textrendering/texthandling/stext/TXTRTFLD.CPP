/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "FLDINFO.H"
#include "FLDSET.H"
#include "TXTRICH.H"

#include "TXTSTD.H"
#include "TXTINDEX.H"
#include "ParseLst.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTRTFLDTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTETEXT_INTERNAL.H"
#endif 

EXPORT_C void CRichText::UpdateFieldL(TInt aPos)
// Updates the field at aPos, adjusting the rich text index by the required amount.
//
/** Re-evaluates the field which covers the document position specified. Re-evaluating 
a field means calculating the field's new value, then inserting that value 
into the text object, replacing the previous value.

Notes:

fields have a maximum length of 20 characters

the first time a field is updated, the position specified should be the position 
at which the field was inserted

@param aPos A document position in the field to be updated. Must be a valid 
position, or a panic occurs. */
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_UPDATEFIELDL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

 	TFindFieldInfo fieldInfo;
	if (iFieldSet->FindFields(fieldInfo, aPos))
		{// a field exists at aPos, so update it.
		HBufC* valueBuf = HBufC::NewL(KMaxFieldBufferSize); // will hold the new value

		/*
		Calculate new field value and insert it.
		Don't put valueBuf on the cleanup stack before calling NewFieldValueL because NewFieldValueL
		sometimes changes valueBuf, in which case it itself puts it on the cleanup stack.
		*/
		iFieldSet->NewFieldValueL(valueBuf, fieldInfo.iFirstFieldPos);  // get the new value
		CleanupStack::PushL(valueBuf);
		DoPtInsertL(fieldInfo.iFirstFieldPos, *valueBuf);  // insert the new text into the document
		TBool indexPresent = IndexPresent();
		if (indexPresent)
			{
			TRAPD(ret, iIndex->InsertL(aPos, valueBuf->Des(), *iGlobalParaFormatLayer));
			if (ret != KErrNone)
				{
				DoPtDelete(fieldInfo.iFirstFieldPos, valueBuf->Length());
				User::Leave(ret);
				}
			}
		//
		// Now delete the old field value from the text.
		TIndexDeleteInfo indexInfo;
		if (indexPresent)
			{
			TRAPD(ret, iIndex->SetForDeleteL(indexInfo, fieldInfo.iFirstFieldPos + valueBuf->Length(), fieldInfo.iFirstFieldLen));
			if (ret != KErrNone)
				{// Maintain rich text invariant.  Both old&new values now make up the current field value.
				iFieldSet->NotifyFieldUpdate(aPos, valueBuf->Length() + fieldInfo.iFirstFieldLen);
				User::Leave(ret);
				}// Not the greatest but at least the text stream is consistent with the rich text index.
			iIndex->DeleteNow(indexInfo);
			}
		DoPtDelete(fieldInfo.iFirstFieldPos + valueBuf->Length(), fieldInfo.iFirstFieldLen);  // delete the old text of the field
		iFieldSet->NotifyFieldUpdate(aPos, valueBuf->Length());  // inform the field set
		//
		iParserData->MergeRange(aPos,fieldInfo.iFirstFieldLen,valueBuf->Length());
		CallEditObserver(aPos, valueBuf->Length() - fieldInfo.iFirstFieldLen);
		CleanupStack::PopAndDestroy(); // valueBuf
		}

	__TEST_INVARIANT;
	}
