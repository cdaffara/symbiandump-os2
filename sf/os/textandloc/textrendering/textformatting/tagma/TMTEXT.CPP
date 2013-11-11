/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TmText.h"
#include <txtfrmat.h>
#include <txtetext.h>
#include "TMSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TMTEXTTraces.h"
#endif


/**
 * Creates a CTmText object using the specified device map and formatting parameters.
 */
EXPORT_C CTmText* CTmText::NewL(MGraphicsDeviceMap& aDevice,const TTmFormatParamBase& aFormatParam)
	{
	CTmText* t = new(ELeave) CTmText;
	CleanupStack::PushL(t);
	t->iImp = new(ELeave) CTmTextImp(aDevice,aFormatParam);
	CleanupStack::Pop();
	return t;
	}

/**
 * Creates a CTmText object using the specified device map and formatting
 * parameters that have default values (as in the default constructor for
 * TTmFormatParamBase) apart from the wrap width and flags. The values of the
 * flags are those used in for TTmFormatParamBase::iFlags.
 */
EXPORT_C CTmText* CTmText::NewL(MGraphicsDeviceMap& aDevice,TInt aWrapWidth,TInt aFlags)
	{
	TTmFormatParamBase param;
	param.iWrapWidth = aWrapWidth;
	param.iFlags = aFlags;
	return NewL(aDevice,param);
	}

CTmText::CTmText()
	{
	}
/**
 * Destroys a CTmText object.
 */
EXPORT_C CTmText::~CTmText()
	{
	delete iImp;
	}

/**
 * Inserts the text aText at the document position aPos. The other arguments
 * are all pointers which have default values of null. The first two,
 * aCharFormat and aParFormat, specify the format for the inserted text. If
 * either is null the format immediately before the insertion point is used.
 *
 * Put the coordinates of the area that has been reformatted and needs to be
 * redrawn, relative to the origin of the CText object, in aRedrawRect. Put the
 * amount by which the text after the redrawn text needs to be scrolled up or
 * down in aScroll: this is the amount by which the reformatted text has
 * changed in height, and thus is positive if the text after the redrawn text
 * has increased in height, which is usually, but not always, the case.
 */
EXPORT_C void CTmText::InsertL(TInt aPos,const TDesC& aText,
	const TTmCharFormat* aCharFormat,const RTmParFormat* aParFormat,
	TRect* aRedrawRect,TInt* aScroll)
	{
	iImp->InsertL(aPos,aText,aCharFormat,aParFormat,aRedrawRect,aScroll);
	}

/**
 * Retrieves the current formatting parameters.
 */
EXPORT_C void CTmText::GetFormat(TTmFormatParamBase& aFormatParam) const
	{
	iImp->GetFormat(aFormatParam);
	}

/**
 * Sets the wrap width and reformat the text.
 */
EXPORT_C void CTmText::SetWrapWidthL(TInt aWrapWidth)
	{
	iImp->SetWrapWidthL(aWrapWidth);
	}

/**
 * Set the formatting parameters and reformat the text.
 */
EXPORT_C void CTmText::ChangeFormatL(const TTmFormatParamBase& aFormatParam)
	{
	iImp->ChangeFormatL(aFormatParam);
	}

/**
 * Deletes all the text.
 */
EXPORT_C void CTmText::Clear()
	{
	iImp->Clear();
	}

/**
 * Specifies custom drawing or layout for the object, or if aCustom is NULL,
 * removes any customization. Reformats the object according to the new
 * customized layout. See the MTmCustom class for details. Custom drawing
 * allows background graphics to be drawn, and custom layout allows, among
 * other things, control over the way line height is calculated and special
 * characters are displayed.
 */
EXPORT_C void CTmText::CustomizeL(const MTmCustom* aCustom)
	{
	iImp->CustomizeL(aCustom);
	}

/**
 * Returns the number of bytes of memory used by a CTmText object, allowing 8
 * bytes overhead per heap allocation.
*/
EXPORT_C TInt CTmText::MemoryUsed() const
	{
	return sizeof(*this) + iImp->MemoryUsed() + 8;
	}

/**
 * Returns a const reference to the CTmTextLayout object owned by this object.
 * This function implements MTmTextLayoutForwarder::TextLayout.
 */
const CTmTextLayout& CTmText::TextLayout() const
	{
	return iImp->TextLayout();
	}

template<class T> TInt RRefCountedArray<T>::Insert(const T& aEntry,TInt& aIndex)
	{
	//+ replace this linear search with a binary search
	int n = iArray.Count();
	for (int i = 0; i < n; i++)
		if (aEntry == iArray[i].iEntry)
			{
			iArray[i].iRefCount++;
			aIndex = i;
			return KErrNone;
			}
	aIndex = n;
	TCountedEntry new_entry;
	new_entry.iRefCount = 1;
	int error = iArray.Append(new_entry);
	if (!error)
		error = iArray[iArray.Count() - 1].iEntry.Copy(aEntry);
	return error;
	}

template<class T> void RRefCountedArray<T>::Remove(TInt aIndex)
	{
	if (--iArray[aIndex].iRefCount == 0)
		iArray.Remove(aIndex);
	}

template<class T> TInt RRefCountedArray<T>::MemoryUsed() const
	{
	TInt bytes = sizeof(*this);
	if (iArray.Count())
		bytes += User::AllocLen(&iArray[0]) + 8;
	return bytes;
	} 

CTmTextImp::~CTmTextImp()
	{
	iCharFormatRun.Close();
	iParFormatRun.Close();
	iCharFormat.Close();
	for (int i = 0; i < iParFormat.Count(); i++)
		iParFormat[i].Close();
	iParFormat.Close();
	}

void CTmTextImp::InsertL(TInt aPos,const TDesC& aText,
						 const TTmCharFormat* aCharFormat,const RTmParFormat* aParFormat,
						 TRect* aRedrawRect,TInt* aScroll)
	{
	if (0 > aPos)
	    {
	    OstTrace0( TRACE_DUMP, CTMTEXTIMP_INSERTL, "EBadArg" );
	    }
	__ASSERT_DEBUG(0 <= aPos, TmPanic(EBadArg));

	// Don't insert zero-length text.
	if (aText.Length() == 0)
		return;

	// Clamp aPos to the end of the document.
	if (iTextLayout.EndChar() <= aPos)
		aPos = Max(0,iTextLayout.EndChar() - 1);

	// If the document is empty and character or paragraph format is unspecified, use a default format.
	TTmCharFormat default_char_format;
	default_char_format.iFontSpec.SetHeight(12 * 20);	// set height, which must be in twips, to 12pt
	RTmParFormat default_par_format;
	if (iText.Length() == 0)
		{
		if (aCharFormat == NULL)
			aCharFormat = &default_char_format;
		if (aParFormat == NULL)
			aParFormat = &default_par_format;
		}

	// Insert the character format.
	int error = 0;
	int char_format_index = -1;
	if (aCharFormat)
		error = iCharFormat.Insert((TMyCharFormat&)*aCharFormat,char_format_index);
	
	// Insert the paragraph format.
	int par_format_index = -1;
	if (!error && aParFormat)
		error = iParFormat.Insert((RMyParFormat&)*aParFormat,par_format_index);

	// Insert the character format run.
	if (!error)
		error = iCharFormatRun.Insert(aPos,aText.Length(),char_format_index);

	// If the paragraph format has changed extend it to the paragraph bounds.
	int old_par_format_index = -1;
	if (!error && iText.Length() > 0)
		{
		if (par_format_index != -1)
			old_par_format_index = iParFormatRun.Index(aPos);
		if (par_format_index != old_par_format_index)
			{
			int par_start = ParagraphStart(aPos);
			int par_end = ParagraphEnd(aPos);
			if (iText.Length() == par_end - 1)
				--par_end;
			error = iParFormatRun.Set(par_start, par_end - par_start,
				par_format_index);
			}
		}

	// Insert the paragraph format run.
	if (!error)
		error = iParFormatRun.Insert(aPos,aText.Length(),par_format_index);

	// Insert the text.
	if (!error)
		{
		TRAP(error,iText.InsertL(aPos,aText));
		}

	default_par_format.Close();

	User::LeaveIfError(error);

	if (iTextLayout.EndChar() > 0)
		{
		TTmReformatParam reformat_param;
		reformat_param.iStartChar = aPos;
		reformat_param.iOldLength = 0;
		reformat_param.iNewLength = aText.Length();
		reformat_param.iParFormatChanged = par_format_index != old_par_format_index;
		TTmReformatResult result;
		iTextLayout.FormatL(iFormatParam,reformat_param,result);
		if (aRedrawRect)
			*aRedrawRect = result.iRedrawRect;
		if (aScroll)
			*aScroll = result.iHeightChange;
		}
	else
		{
		iTextLayout.SetTextL(*this,iFormatParam);
		if (aRedrawRect)
			*aRedrawRect = TRect(0,0,iTextLayout.LayoutWidth(),iTextLayout.LayoutHeight());
		if (aScroll)
			*aScroll = 0;
		}
	}

void CTmTextImp::ChangeFormatL(const TTmFormatParamBase& aFormatParam)
	{
	iFormatParam = aFormatParam;
	ReformatL();
	}

void CTmTextImp::ReformatL()
	{
	if (iText.Length() > 0)
		iTextLayout.SetTextL(*this,iFormatParam);
	}

void CTmTextImp::Clear()
	{
	iText.Reset();
	iCharFormatRun.Reset();
	iParFormatRun.Reset();
	iCharFormat.Reset();
	for (int i = 0; i < iParFormat.Count(); i++)
		iParFormat[i].Close();
	iParFormat.Reset();
	iTextLayout.Clear();
	}

void CTmTextImp::CustomizeL(const MTmCustom* aCustom)
	{
	iCustom = aCustom;
	ReformatL();
	}

TInt CTmTextImp::MemoryUsed() const
	{
	return sizeof(*this) +
		   iText.MemoryUsed() - sizeof(iText) +
		   iCharFormatRun.MemoryUsed() - sizeof(iCharFormatRun) +
		   iParFormatRun.MemoryUsed() - sizeof(iParFormatRun) +
		   iCharFormat.MemoryUsed() - sizeof(iCharFormat) +
		   iParFormat.MemoryUsed() - sizeof(iParFormat) +
		   iTextLayout.MemoryUsed() - sizeof(iTextLayout);
	}

MGraphicsDeviceMap& CTmTextImp::FormatDevice() const
	{
	return iDevice;
	}

MGraphicsDeviceMap& CTmTextImp::InterpretDevice() const
	{
	return iDevice;
	}

TInt CTmTextImp::DocumentLength() const
	{
	return iText.Length();
	}

void CTmTextImp::GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const
	{
	static const TText end_par = CEditableText::EParagraphDelimiter;
	TInt index = 0;
	TInt textLength = iText.Length();
	if (aPos > textLength + 1)
	    {
	    OstTrace0( TRACE_DUMP, CTMTEXTIMP_GETTEXT, "EIndexOutOfRange" );
	    }
	__ASSERT_DEBUG(aPos <= textLength + 1, TmPanic(EIndexOutOfRange));
	if (aPos < textLength)
		{
		TPtrC p = iText.PtrC(aPos);
		TInt pLength = p.Length();
		TRun charRun;
		TInt charRunStart = iCharFormatRun.RunAndStartPos(aPos, charRun);
		TInt remainingLength = charRunStart + charRun.iLength - aPos;
		aText.Set(p.Ptr(), remainingLength < pLength? remainingLength : pLength);
		index = charRun.iIndex;
		}
	else if (aPos == textLength)
		{
		aText.Set(&end_par,1);
		if (0 < aPos)
			index = iCharFormatRun.Index(aPos - 1);
		else
 			{
			TTmCharFormat d;
			aFormat = d;
			return;
			}
		}
	else
		{
		aText.Set(0, 0);
		return;
		}
	aFormat = iCharFormat[index];
	}

void CTmTextImp::GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const
	{
	if (0 > aPos || aPos > iText.Length())
	    {
	    OstTrace0( TRACE_DUMP, CTMTEXTIMP_GETPARAGRAPHFORMATL, "EBadArg" );
	    }
	__ASSERT_DEBUG(0 <= aPos && aPos <= iText.Length(), TmPanic(EBadArg));
	TInt textLength = iText.Length();
	if (0 == textLength)
		{
		RTmParFormat d;
		CleanupClosePushL(d);
		aFormat.CopyL(d);
		CleanupStack::PopAndDestroy(&d);
		return;
		}
	if (aPos != textLength)
		++aPos;
	aFormat.CopyL(iParFormat[iParFormatRun.Index(aPos)]);
	}

TInt CTmTextImp::ParagraphStart(TInt aPos) const
	{
	while (aPos > 0)
		{
		TPtrC text = iText.BackPtrC(aPos);
		const TText *p = text.Ptr();
		const TText *q = p + text.Length();
		while (p < q)
			{
			q--;
			aPos--;
			if (*q == CEditableText::EParagraphDelimiter)
				return aPos + 1;
			}
		}
	return 0;
	}

TRgb CTmTextImp::SystemColor(TUint aColorIndex,TRgb aDefaultColor) const
	{
	if (iCustom)
		return iCustom->SystemColor(aColorIndex,aDefaultColor);
	else
		return MTmSource::SystemColor(aColorIndex,aDefaultColor);
	}

TInt CTmTextImp::Stretch(TUint aChar) const
	{
	if (iCustom)
		return iCustom->Stretch(aChar);
	else
		return MTmSource::Stretch(aChar);
	}

TUint CTmTextImp::Map(TUint aChar) const
	{
	if (iCustom)
		return iCustom->Map(aChar);
	else
		return MTmSource::Map(aChar);
	}

void CTmTextImp::SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const
	{
	if (iCustom)
		iCustom->SetLineHeight(aParam,aAscent,aDescent);
	else
		MTmSource::SetLineHeight(aParam,aAscent,aDescent);
	}

void CTmTextImp::DrawBackground(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
								const TLogicalRgb& aBackground,TRect& aRectDrawn) const
	{
	if (iCustom)
		iCustom->DrawBackground(aGc,aTextLayoutTopLeft,aRect,aBackground,aRectDrawn);
	else
		MTmSource::DrawBackground(aGc,aTextLayoutTopLeft,aRect,aBackground,aRectDrawn);
	}

void CTmTextImp::DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
								  const TTmLineInfo& aLineInfo) const
	{
	if (iCustom)
		iCustom->DrawLineGraphics(aGc,aTextLayoutTopLeft,aRect,aLineInfo);
	else
		MTmSource::DrawLineGraphics(aGc,aTextLayoutTopLeft,aRect,aLineInfo);
	}

void CTmTextImp::DrawText(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						  const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
						  const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	if (iCustom)
		iCustom->DrawText(aGc,aTextLayoutTopLeft,aRect,aLineInfo,aFormat,aText,aTextOrigin,aExtraPixels);
	else
		MTmSource::DrawText(aGc,aTextLayoutTopLeft,aRect,aLineInfo,aFormat,aText,aTextOrigin,aExtraPixels);
	}

TInt CTmTextImp::RRunArray::SplitRun(TInt aRunIndex,TInt aOffset)
	{
	TRun& run = iRun[aRunIndex];
	TRun new_run;
	new_run.iIndex = run.iIndex;
	new_run.iLength = run.iLength - aOffset;
	int error = iRun.Insert(new_run, aRunIndex + 1);
	if (!error)
		run.iLength = aOffset;
	return error;
	}

/*
Find the run containing aPos. The position at the end of a run is in the run.
The position at the start of a run is NOT in the run, but in the preceding run, if any.
*/
void CTmTextImp::RRunArray::FindRun(TInt aPos,TInt& aRunIndex,TInt& aOffset) const
	{
	TInt error = FindRunNonstrict(aPos, aRunIndex, aOffset);
	if (error)
	    {
	    OstTrace0( TRACE_DUMP, RRUNARRAY_FINDRUN, "ETextRunNotFound" );
	    }
	__ASSERT_DEBUG(!error, TmPanic(ETextRunNotFound));
	}

TInt CTmTextImp::RRunArray::FindRunNonstrict(TInt aPos,TInt& aRunIndex,TInt& aOffset) const
	{
	int n = iRun.Count();
	int start = 0;
	int end = 0;
	for (int i = 0; i < n; i++)
		{
		if (0 > iRun[i].iLength)
		    {
		    OstTrace0( TRACE_DUMP, RRUNARRAY_FINDRUNNONSTRICT, "EInvalidTextRunLength" );
		    }
		__ASSERT_DEBUG(0 <= iRun[i].iLength, TmPanic(EInvalidTextRunLength));
		if (0 > iRun[i].iIndex)
		    {
		    OstTrace0( TRACE_DUMP, DUP1_RRUNARRAY_FINDRUNNONSTRICT, "EInvalidTextRunIndex" );
		    }
		__ASSERT_DEBUG(0 <= iRun[i].iIndex, TmPanic(EInvalidTextRunIndex));
		end += iRun[i].iLength;
		if (end >= aPos)
			{
			aRunIndex = i;
			aOffset = aPos - start;
			return KErrNone;
			}
		start = end;
		}

	aRunIndex = aOffset = -1;
	return KErrNotFound;
	}

/* Find the latest run that starts less than or equal to aPos. Return it
 * in aRunOut, and return its start position.
 */
TInt CTmTextImp::RRunArray::RunAndStartPos(TInt aPos, TRun& aRunOut) const
	{
	TInt runs = iRun.Count();
	if (0 >= runs)
	    {
	    OstTrace0( TRACE_DUMP, RRUNARRAY_RUNANDSTARTPOS, "ETextRunNotFound" );
	    }
	__ASSERT_DEBUG(0 < runs, TmPanic(ETextRunNotFound));
	TInt start = 0;
	TInt end = 0;
	for (TInt i = 0; i != runs && end <= aPos; ++i)
		{
		aRunOut = iRun[i];
		start = end;
		end = start + aRunOut.iLength;
		}
	return start;
	}

// Insert a run of length aLength and attribute aIndex at aPos; if aIndex is < 0, use the index at aPos.
TInt CTmTextImp::RRunArray::Insert(TInt aPos,TInt aLength,TInt aIndex)
	{
	if (0 >= aLength)
	    {
	    OstTrace0( TRACE_DUMP, RRUNARRAY_INSERT, "EInvalidTextRunLength" );
	    }
	__ASSERT_DEBUG(0 < aLength, TmPanic(EInvalidTextRunLength));
	if (0 > aIndex && 0 >= iRun.Count())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_RRUNARRAY_INSERT, "EParagraphFormatRequired" );
	    }
	__ASSERT_DEBUG(0 <= aIndex || 0 < iRun.Count(), TmPanic(EParagraphFormatRequired));

	int error = 0;
	TRun new_run;
	if (iRun.Count() == 0)
		{
		new_run.iLength = 0;
		new_run.iIndex = aIndex;
		error = iRun.Append(new_run);
		if (error)
			return error;
		}

	// Find the run; see if the index matches; if on a join between runs, check both.
	int run_index, offset;
	FindRun(aPos,run_index,offset);
	if (offset == iRun[run_index].iLength && iRun[run_index].iIndex != aIndex && run_index < iRun.Count() - 1)
		{
		run_index++;
		offset = 0;
		}
	TRun& run = iRun[run_index];

	if (0 > offset || offset > run.iLength)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_RRUNARRAY_INSERT, "EInvariant" );
	    }
	__ASSERT_DEBUG(0 <= offset && offset <= run.iLength, TmPanic(EInvariant));

	// If aIndex is negative, it means don't change the attribute.
	if (aIndex < 0)
		aIndex = run.iIndex;

	// No need to create a new run; just extend the existing one
	if (run.iIndex == aIndex)
		run.iLength += aLength;

	// Split the run if necessary, then add a new run.
	else
		{
		if (run.iLength == offset)
			++run_index;
		else if (offset != 0)
			{
			error = SplitRun(run_index,offset);
			run_index++;
			}
		if (!error)
			{
			new_run.iIndex = aIndex;
			new_run.iLength = aLength;
			error = iRun.Insert(new_run, run_index);
			}
		}

	return error;
	}

TInt CTmTextImp::RRunArray::Set(TInt aPos,TInt aLength,TInt aIndex)
	{
	if (0 > aLength)
	    {
	    OstTrace0( TRACE_DUMP, RRUNARRAY_SET, "EInvalidTextRunLength" );
	    }
	__ASSERT_DEBUG(0 <= aLength, TmPanic(EInvalidTextRunLength));
	if (0 > aIndex)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_RRUNARRAY_SET, "EInvalidTextRunIndex" );
	    }
	__ASSERT_DEBUG(0 <= aIndex, TmPanic(EInvalidTextRunIndex));

	if (aLength == 0)
		return KErrNone;

	// Find the run start.
	int start_run_index, start_run_offset;
	FindRun(aPos,start_run_index,start_run_offset);

	// Adjust the run start if there is no change to the attribute; return if this eliminates the range to be set.
	if (iRun[start_run_index].iIndex == aIndex)
		{
		int n = iRun[start_run_index].iLength - start_run_offset;
		aPos += n;
		aLength -= n;
		if (aLength <= 0)
			return KErrNone;
		}

	// Find the run end.
	int end_run_index, end_run_offset;
	FindRun(aPos + aLength,end_run_index,end_run_offset);

	// Adjust the run end if there is no change to the attribute; return if this eliminates the range to be set.
	if (iRun[end_run_index].iIndex == aIndex)
		{
		aLength -= end_run_offset;
		if (aLength <= 0)
			return KErrNone;
		}

	// Determine the change in the number of runs. The maximum increase is 2.
	int runs_increase = start_run_index - end_run_index + 2;

	// Insert extra runs if needed.
	int error = KErrNone;
	if (runs_increase > 0)
		{
		error = SplitRun(start_run_index,start_run_offset);
		runs_increase--;
		if (start_run_index == end_run_index)
			end_run_offset -= start_run_offset;
		end_run_index++;
		}
	if (!error && runs_increase > 0)
		{
		error = SplitRun(end_run_index,end_run_offset);
		end_run_index++;
		end_run_offset = 0;
		runs_increase--;
		}
	if (!error)
		{
		// Delete unneeded runs.
		while (runs_increase < 0)
			{
			iRun.Remove(start_run_index + 2);
			end_run_index--;
			++runs_increase;
			}

		// Adjust the size of the start run.
		iRun[start_run_index].iLength = start_run_offset;

		// Set the run after the start run to the new attribute.
		TRun& run = iRun[start_run_index + 1];
		run.iLength = aLength;
		run.iIndex = aIndex;

		// Adjust the size of the end run.
		iRun[end_run_index].iLength -= end_run_offset;
		}

	return error;
	}

void CTmTextImp::RRunArray::Delete(TInt aPos,TInt aLength)
	{
	if (0 > aLength)
	    {
	    OstTrace0( TRACE_DUMP, RRUNARRAY_DELETE, "EInvalidTextRunLength" );
	    }
	__ASSERT_DEBUG(0 <= aLength, TmPanic(EInvalidTextRunLength));

	if (aLength == 0)
		return;

	// Find the start.
	int start_run_index, start_run_offset;
	FindRun(aPos,start_run_index,start_run_offset);

	// Find the end.
	int end_run_index, end_run_offset;
	FindRun(aPos + aLength,end_run_index,end_run_offset);

	// If the runs are the same it's trivial.
	if (start_run_index == end_run_index)
		{
		iRun[start_run_index].iLength -= (end_run_offset - start_run_offset);
		return;
		}

	// Shorten the start and end runs and determine the range of runs to delete.
	iRun[start_run_index].iLength = start_run_offset;
	int first_deleted_run = start_run_index + 1;
	if (start_run_offset == 0)
		first_deleted_run--;
	iRun[end_run_index].iLength -= end_run_offset;
	int last_deleted_run = end_run_index - 1;
	if (iRun[end_run_index].iLength == 0)
		last_deleted_run++;

	// Delete runs.
	for (int i = first_deleted_run; i <= last_deleted_run; i++)
		iRun.Remove(first_deleted_run);
	}

TInt CTmTextImp::RRunArray::Index(TInt aPos) const
	{
	int run_index, offset;
	TInt error = FindRunNonstrict(aPos,run_index,offset);
	return error? error : iRun[run_index].iIndex;
	}

TInt CTmTextImp::RRunArray::MemoryUsed() const
	{
	TInt bytes = sizeof(*this);
	if (iRun.Count())
		bytes += User::AllocLen(&iRun[0]) + 8;
	return bytes;
	} 

EXPORT_C void CTmText::Spare1()
	{
	TmPanic(EUnimplemented);
	}
