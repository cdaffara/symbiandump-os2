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

#ifndef UTRACEMACROS_H
#define UTRACEMACROS_H


/**
Dummy class to toss away overflow
@internalComponent
*/
#ifndef __KERNEL_MODE__
class TTruncateOverflow16 : public TDes16Overflow
	{
	public:
	virtual void Overflow(TDes&) {};
	};
/**
Dummy class to toss away overflow
@internalComponent
*/
class TTruncateOverflow8 : public TDes8Overflow
	{
	public:
	virtual void Overflow(TDes8&) {};
	};
#endif  //__KERNEL_MODE__

struct TUint96
{
	TUint96(TUint32 aData1, TUint32 aData2, TUint32 aData3)
	:iData1(aData1), iData2(aData2), iData3(aData3)
		{
		}
	TUint32 iData1;
	TUint32 iData2;
	TUint32 iData3;
};

	/**@internalComponent*/
#define UTRACE_HEADER(aSize,aClassification,aCategory,aContext,aPc)																\
	((((aSize)+(4&(TBool)aContext<<2)+(4&(TBool)aPc<<2))<<BTrace::ESizeIndex*8)										\
	+(((BTrace::EContextIdPresent & (aContext<<3))|(BTrace::EPcPresent & (aPc<<4)))<<BTrace::EFlagsIndex*8)			\
	+((aClassification)<<BTrace::ECategoryIndex*8)																				\
	+((aCategory)<<BTrace::ESubCategoryIndex*8))

/**@internalComponent*/
#define UTRACE_PRIMARY_0(aClassification,aCategory,aContext,aPc,aFormatId)	\
    BTrace::OutX \
        (UTRACE_HEADER(8,(aClassification),(aCategory),(aContext),(aPc)), (TUint32)(aFormatId),0,0)

/**@internalComponent*/
#define UTRACE_PRIMARY_1(aClassification,aCategory,aContext,aPc,aFormatId, aData1)	\
    BTrace::OutX \
        (UTRACE_HEADER(12,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aFormatId),(TUint32)(aData1),0)

/**@internalComponent*/
#define UTRACE_PRIMARY_2(aClassification,aCategory,aContext,aPc,aFormatId,aData1,aData2) \
    BTrace::OutX \
		(UTRACE_HEADER(16,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aFormatId),(TUint32)(aData1),(TUint32)(aData2))

/**@internalComponent
 * You have to use the ANY macro instread or we can't provide aFormatId
 */
//#define UTRACE_PRIMARY_3(aClassification,aCategory,aContext,aPc,aFormatId,aData1,aData2,aData3) \
	(BTrace::OutX)\
		(UTRACE_HEADER(16,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aFormatId),(TUint32)(aData1),(TUint32)(aData2))

/**@internalComponent*/
#define UTRACE_PRIMARY_ANY(aClassification,aCategory,aContext,aPc,aData1,aData,aDataSize) \
	BTrace::OutBig(UTRACE_HEADER(8,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aData1),aData,(TInt)(aDataSize))

/**@internalComponent*/
#define UTRACE_SECONDARY_0(aClassification,aModuleUid,aCategory,aContext,aPc,aFormatId)	\
    BTrace::OutFilteredX(UTRACE_HEADER(12,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aModuleUid),(TUint32)(aFormatId),0)

/** @internalComponent */
#define UTRACE_SECONDARY_1(aClassification,aModuleUid,aCategory,aContext,aPc,aFormatId, aData1) \
    BTrace::OutFilteredX(UTRACE_HEADER(16,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aModuleUid),(TUint32)(aFormatId),(TUint32)(aData1))

/** @internalComponent 
 * You have to use the ANY macro instead.
 */
#define UTRACE_SECONDARY_2(aClassification,aModuleUid,aCategory,aContext,aPc,aData1,aData2) \
	BTrace::OutFilteredX(UTRACE_HEADER(16,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aModuleUid),(TUint32)(aData1),(TUint32)(aData2))

/** @internalComponent 
 * You have to pack the aFormatId as part of the aData because there is no extra space for it
 */
#define UTRACE_SECONDARY_ANY(aClassification,aModuleUid,aCategory,aContext,aPc,aData,aDataSize) \
	BTrace::OutFilteredBig(UTRACE_HEADER(8,(aClassification),(aCategory),(aContext),(aPc)),(TUint32)(aModuleUid),aData,(TInt)(aDataSize))

#endif /*UTRACEMACROS_H*/
