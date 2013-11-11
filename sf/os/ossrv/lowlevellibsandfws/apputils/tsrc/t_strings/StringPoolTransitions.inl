// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the transition classes upon the RStringPool class methods.
// 
//

#include "short.h"
#include "long.h"
#include "irrelevant.h"
#include "shortcs.h"
#include "longcs.h"
#include "irrelevantcs.h"
#include "short2.h"
#include "short3.h"
#include "short4.h"
#include "shortcs2.h"
#include "shortcs3.h"
#include "shortcs4.h"
#include "securitypolicypluginstrings.h"

// ______________________________________________________________________________
//
_LIT(KStringPoolNewLTransition,"CStringPool_NewL_Transition");

inline CStringPool_NewL_Transition::CStringPool_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_NewL_Transition::TransitMethodL()
	{
	_LIT(KStringPoolNewLTransitMethod, "CStringPool::NewL transition");
	Context().DataLogger().LogInformation(KStringPoolNewLTransitMethod);
	Context().iStringPool.OpenL();
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");

	Context().iA = p.OpenStringL(KStringA());
	Context().iB = p.OpenStringL(KStringB());
	Context().iD = p.OpenStringL(KStringD());

	Context().iFA = p.OpenFStringL(KStringFA());
	Context().iFB = p.OpenFStringL(KStringFB());
	Context().iFD = p.OpenFStringL(KStringFD());

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);

	Context().iInt = -1;
	Context().iFint = -1;
	}

inline CStringPool_UnitTestContext& CStringPool_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolDtorTransition,"CStringPool_Dtor_Transition");

inline CStringPool_Dtor_Transition::CStringPool_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_Dtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolDtorTransitMethod, "CStringPool::Dtor transition");
	Context().DataLogger().LogInformation(KStringPoolDtorTransitMethod);

	Context().iA.Close();
	Context().iB.Close();
	Context().iD.Close();

	Context().iFA.Close();
	Context().iFB.Close();
	Context().iFD.Close();

	Context().iStringPool.Close();

	

	}

inline CStringPool_UnitTestContext& CStringPool_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// Add additional Transition class Implementations here...

// ______________________________________________________________________________
//
_LIT(KRStringPoolAeqATransition,"CRStringPool_AeqA_Transition");

inline CStringPool_AeqA_Transition::CStringPool_AeqA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRStringPoolAeqATransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AeqA_Transition::TransitMethodL()
	{
	_LIT(KRStringPoolAeqATransitMethod, "CRStringPool::AeqA transition");
	Context().DataLogger().LogInformation(KRStringPoolAeqATransitMethod);
	RString temp = Context().iA;
	Context().iResult = Context().iA == temp;;
	}

inline CStringPool_UnitTestContext& CStringPool_AeqA_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAneATransition,"CStringPool_AneA_Transition");

inline CStringPool_AneA_Transition::CStringPool_AneA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAneATransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AneA_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAneATransitMethod, "CStringPool::AneA transition");
	Context().DataLogger().LogInformation(KStringPoolAneATransitMethod);
	RString temp = Context().iA;
	Context().iResult = Context().iA != temp;;
	}

inline CStringPool_UnitTestContext& CStringPool_AneA_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAeqBTransition,"CStringPool_AeqB_Transition");

inline CStringPool_AeqB_Transition::CStringPool_AeqB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAeqBTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AeqB_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAeqBTransitMethod, "CStringPool::AeqB transition");
	Context().DataLogger().LogInformation(KStringPoolAeqBTransitMethod);
	Context().iResult = Context().iA == Context().iB;
	}

inline CStringPool_UnitTestContext& CStringPool_AeqB_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAneBTransition,"CStringPool_AneB_Transition");

inline CStringPool_AneB_Transition::CStringPool_AneB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAneBTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AneB_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAneBTransitMethod, "CStringPool::AneB transition");
	Context().DataLogger().LogInformation(KStringPoolAneBTransitMethod);
	Context().iResult = Context().iA != Context().iB;
	}

inline CStringPool_UnitTestContext& CStringPool_AneB_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAeqDTransition,"CStringPool_AeqD_Transition");

inline CStringPool_AeqD_Transition::CStringPool_AeqD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAeqDTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AeqD_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAeqDTransitMethod, "CStringPool::AeqD transition");
	Context().DataLogger().LogInformation(KStringPoolAeqDTransitMethod);
	Context().iResult = Context().iA == Context().iD;
	}

inline CStringPool_UnitTestContext& CStringPool_AeqD_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAneDTransition,"CStringPool_AneD_Transition");

inline CStringPool_AneD_Transition::CStringPool_AneD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAneDTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_AneD_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAneDTransitMethod, "CStringPool::AneD transition");
	Context().DataLogger().LogInformation(KStringPoolAneDTransitMethod);
	Context().iResult = Context().iA != Context().iD;
	}

inline CStringPool_UnitTestContext& CStringPool_AneD_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolDescTransition,"CStringPool_Desc_Transition");

inline CStringPool_Desc_Transition::CStringPool_Desc_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolDescTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_Desc_Transition::TransitMethodL()
	{
	_LIT(KStringPoolDescTransitMethod, "CStringPool::Desc transition");
	Context().DataLogger().LogInformation(KStringPoolDescTransitMethod);
	Context().iResult = Context().iADes.Compare(Context().iA.DesC());
	}

inline CStringPool_UnitTestContext& CStringPool_Desc_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolCopyTransition,"CStringPool_Copy_Transition");

inline CStringPool_Copy_Transition::CStringPool_Copy_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolCopyTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_Copy_Transition::TransitMethodL()
	{
	_LIT(KStringPoolCopyTransitMethod, "CStringPool::Copy transition");
	Context().DataLogger().LogInformation(KStringPoolCopyTransitMethod);
	Context().iC = Context().iA.Copy();
	Context().iResult = Context().iA == Context().iC;
	}

inline CStringPool_UnitTestContext& CStringPool_Copy_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolCDesCTransition,"CStringPool_CDesC_Transition");

inline CStringPool_CDesC_Transition::CStringPool_CDesC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolCDesCTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_CDesC_Transition::TransitMethodL()
	{
	_LIT(KStringPoolCDesCTransitMethod, "CStringPool::CDesC transition");
	Context().DataLogger().LogInformation(KStringPoolCDesCTransitMethod);
	Context().iResult = Context().iADes.Compare(Context().iC.DesC());
	}

inline CStringPool_UnitTestContext& CStringPool_CDesC_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolCloseTransition,"CStringPool_Close_Transition");

inline CStringPool_Close_Transition::CStringPool_Close_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolCloseTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_Close_Transition::TransitMethodL()
	{
	_LIT(KStringPoolCloseTransitMethod, "CStringPool::Close transition");
	Context().DataLogger().LogInformation(KStringPoolCloseTransitMethod);
	Context().iC.Close();
	}

inline CStringPool_UnitTestContext& CStringPool_Close_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolIntTransition,"CStringPool_Int_Transition");

inline CStringPool_Int_Transition::CStringPool_Int_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolIntTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_Int_Transition::TransitMethodL()
	{
	_LIT(KStringPoolIntTransitMethod, "aaCStringPool::Int transition");
	Context().DataLogger().LogInformation(KStringPoolIntTransitMethod);
	Context().iResult = Context().iTE.Index(Context().iTestTable) == Context().iInt;
	}

inline CStringPool_UnitTestContext& CStringPool_Int_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}
// ______________________________________________________________________________
//
_LIT(KStringPoolStringTokenTransition,"CStringPool_StringToken_Transition");

inline CStringPool_StringToken_Transition::CStringPool_StringToken_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolStringTokenTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_StringToken_Transition::TransitMethodL()
	{
	_LIT(KStringPoolStringTokenTransitMethod, "CStringPool::StringToken transition");
	Context().DataLogger().LogInformation(KStringPoolStringTokenTransitMethod);
	RStringToken t = Context().iA;
	Context().iC = Context().iStringPool.String(t);
	Context().iResult = Context().iC == Context().iA;
	}

inline CStringPool_UnitTestContext& CStringPool_StringToken_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}























































// ______________________________________________________________________________
//
_LIT(KRstringPoolFAeqATransition,"CRStringPool_AeqA_Transition");

inline CStringPool_FAeqA_Transition::CStringPool_FAeqA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KRstringPoolFAeqATransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAeqA_Transition::TransitMethodL()
	{
	_LIT(KRstringPoolFAeqATransitMethod, "CRStringPool::AeqA transition");
	Context().DataLogger().LogInformation(KRstringPoolFAeqATransitMethod);
	RStringF temp = Context().iFA;
	Context().iResult = Context().iFA == temp;;
	}

inline CStringPool_UnitTestContext& CStringPool_FAeqA_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFAneATransition,"CStringPool_FAneA_Transition");

inline CStringPool_FAneA_Transition::CStringPool_FAneA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFAneATransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAneA_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFAneATransitMethod, "CStringPoolF::AneA transition");
	Context().DataLogger().LogInformation(KStringPoolFAneATransitMethod);
	RStringF temp = Context().iFA;
	Context().iResult = Context().iFA != temp;;
	}

inline CStringPool_UnitTestContext& CStringPool_FAneA_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFAeqBTransition,"CStringPool_FAeqB_Transition");

inline CStringPool_FAeqB_Transition::CStringPool_FAeqB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFAeqBTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAeqB_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFAeqBTransitMethod, "CStringPoolF::AeqB transition");
	Context().DataLogger().LogInformation(KStringPoolFAeqBTransitMethod);
	Context().iResult = Context().iFA == Context().iFB;
	}

inline CStringPool_UnitTestContext& CStringPool_FAeqB_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFAneBTransition,"CStringPool_FAneB_Transition");

inline CStringPool_FAneB_Transition::CStringPool_FAneB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFAneBTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAneB_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFAneBTransitMethod, "CStringPoolF::AneB transition");
	Context().DataLogger().LogInformation(KStringPoolFAneBTransitMethod);
	Context().iResult = Context().iFA != Context().iFB;
	}

inline CStringPool_UnitTestContext& CStringPool_FAneB_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFAeqDTransition,"CStringPool_FAeqD_Transition");

inline CStringPool_FAeqD_Transition::CStringPool_FAeqD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFAeqDTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAeqD_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFAeqDTransitMethod, "CStringPoolF::AeqD transition");
	Context().DataLogger().LogInformation(KStringPoolFAeqDTransitMethod);
	Context().iResult = Context().iFA == Context().iFD;
	}

inline CStringPool_UnitTestContext& CStringPool_FAeqD_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFAneDTransition,"CStringPool_FAneD_Transition");

inline CStringPool_FAneD_Transition::CStringPool_FAneD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFAneDTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FAneD_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFAneDTransitMethod, "CStringPoolF::AneD transition");
	Context().DataLogger().LogInformation(KStringPoolFAneDTransitMethod);
	Context().iResult = Context().iFA != Context().iFD;
	}

inline CStringPool_UnitTestContext& CStringPool_FAneD_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolFDescTransition,"CStringPool_FDesc_Transition");

inline CStringPool_FDesc_Transition::CStringPool_FDesc_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFDescTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FDesc_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFDescTransitMethod, "CStringPoolF::Desc transition");
	Context().DataLogger().LogInformation(KStringPoolFDescTransitMethod);
	Context().iResult = Context().iADes.Compare(Context().iFA.DesC());
	}

inline CStringPool_UnitTestContext& CStringPool_FDesc_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFCopyTransition,"CStringPool_FCopy_Transition");

inline CStringPool_FCopy_Transition::CStringPool_FCopy_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFCopyTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FCopy_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFCopyTransitMethod, "CStringPoolF::Copy transition");
	Context().DataLogger().LogInformation(KStringPoolFCopyTransitMethod);
	Context().iFC = Context().iFA.Copy();
	Context().iResult = Context().iFA == Context().iFC;
	}

inline CStringPool_UnitTestContext& CStringPool_FCopy_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolFCDesCTransition,"CStringPool_FCDesC_Transition");

inline CStringPool_FCDesC_Transition::CStringPool_FCDesC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFCDesCTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FCDesC_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFCDesCTransitMethod, "CStringPoolF::CDesC transition");
	Context().DataLogger().LogInformation(KStringPoolFCDesCTransitMethod);
	Context().iResult = Context().iADes.Compare(Context().iFC.DesC());
	}

inline CStringPool_UnitTestContext& CStringPool_FCDesC_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFCloseTransition,"CStringPool_FClose_Transition");

inline CStringPool_FClose_Transition::CStringPool_FClose_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFCloseTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FClose_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFCloseTransitMethod, "CStringPoolF::Close transition");
	Context().DataLogger().LogInformation(KStringPoolFCloseTransitMethod);
	Context().iFC.Close();
	}

inline CStringPool_UnitTestContext& CStringPool_FClose_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolFIntTransition,"CStringPool_FInt_Transition");

inline CStringPool_FInt_Transition::CStringPool_FInt_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFIntTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FInt_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFIntTransitMethod, "CStringPoolF::Int transition");
	Context().DataLogger().LogInformation(KStringPoolFIntTransitMethod);
	Context().iResult = Context().iTFE.Index(Context().iTestTable) == Context().iFint;
	}

inline CStringPool_UnitTestContext& CStringPool_FInt_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}
// ______________________________________________________________________________
//
_LIT(KStringPoolFStringTokenTransition,"CStringPool_FStringToken_Transition");

inline CStringPool_FStringToken_Transition::CStringPool_FStringToken_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolFStringTokenTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_FStringToken_Transition::TransitMethodL()
	{
	_LIT(KStringPoolFStringTokenTransitMethod, "CStringPoolF::StringToken transition");
	Context().DataLogger().LogInformation(KStringPoolFStringTokenTransitMethod);
	RStringTokenF t = Context().iFA;
	Context().iFC = Context().iStringPool.StringF(t);
	Context().iResult = Context().iFC == Context().iFA;
	}

inline CStringPool_UnitTestContext& CStringPool_FStringToken_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}
// ______________________________________________________________________________
//
_LIT(KStringPoolSmallCtorTransition,"CStringPool_ShortCtor_Transition");

inline CStringPool_ShortCtor_Transition::CStringPool_ShortCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolSmallCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_ShortCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolSmallCtorTransitMethod, "CStringPool::SmallCtor transition");
	Context().DataLogger().LogInformation(KStringPoolSmallCtorTransitMethod);

	//Set the table used in this test
	Context().iTestTable=Short::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");
	
	
	Context().iA = p.OpenStringL(KStringA());
	CleanupClosePushL(Context().iA);
	Context().iB = p.OpenStringL(KStringB());
	CleanupClosePushL(Context().iB);
	Context().iD = p.OpenStringL(KStringD());
	CleanupClosePushL(Context().iD);

	Context().iFA = p.OpenFStringL(KStringFA());
	CleanupClosePushL(Context().iFA);
	Context().iFB = p.OpenFStringL(KStringFB());
	CleanupClosePushL(Context().iFB);
	Context().iFD = p.OpenFStringL(KStringFD());
	CleanupClosePushL(Context().iFD);


//	Context().iTE = p.String(0,Context().iTestTable);
	Context().iTFE = p.StringF(0,Context().iTestTable);

	// Create an empty string 
	Context().iEmptyOne = RStringF() ;
	

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);

	Context().iInt = 0;
	Context().iFint = 0;
	Context().iNotFound = -1;
	CleanupStack::Pop(6);
	}

inline CStringPool_UnitTestContext& CStringPool_ShortCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolIrrelevantCtorTransition,"CStringPool_IrrelevantCtor_Transition");

inline CStringPool_IrrelevantCtor_Transition::CStringPool_IrrelevantCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KStringPoolIrrelevantCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_IrrelevantCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolIrrelevantCtorTransitMethod, "CStringPool::IrrelevantCtor transition");
	Context().DataLogger().LogInformation(KStringPoolIrrelevantCtorTransitMethod);

	//Set the table used in this test
	Context().iTestTable=Irrelevant::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");

	Context().iA = p.OpenStringL(KStringA());
	Context().iB = p.OpenStringL(KStringB());
	Context().iD = p.OpenStringL(KStringD());

	Context().iFA = p.OpenFStringL(KStringFA());
	Context().iFB = p.OpenFStringL(KStringFB());
	Context().iFD = p.OpenFStringL(KStringFD());

//	Context().iTE = p.String(0,Context().iTestTable);
	Context().iTFE = p.StringF(0,Context().iTestTable);

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);


	Context().iFint = 0;
	}

inline CStringPool_UnitTestContext& CStringPool_IrrelevantCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolLongCtorTransition,"CStringPool_LongCtor_Transition");

inline CStringPool_LongCtor_Transition::CStringPool_LongCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KStringPoolLongCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_LongCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolLongCtorTransitMethod, "CStringPool::LongCtor transition");
	Context().DataLogger().LogInformation(KStringPoolLongCtorTransitMethod);

	//Set the table used in this test
	Context().iTestTable=Long::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");

	Context().iA = p.OpenStringL(KStringA());
	Context().iB = p.OpenStringL(KStringB());
	Context().iD = p.OpenStringL(KStringD());

	Context().iFA = p.OpenFStringL(KStringFA());
	Context().iFB = p.OpenFStringL(KStringFB());
	Context().iFD = p.OpenFStringL(KStringFD());

//	Context().iTE = p.String(0,Context().iTestTable);
	Context().iTFE = p.StringF(0,Context().iTestTable);

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);

	Context().iInt = 0;
	Context().iFint = 0;
	}

inline CStringPool_UnitTestContext& CStringPool_LongCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

//____________________________________________________________________
//
_LIT(KStringPoolStringF_Index_TableTransition,"CStringPool_StringF_Index_Table_Transition");

inline CStringPool_StringF_Index_Table_Transition::CStringPool_StringF_Index_Table_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolStringF_Index_TableTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_StringF_Index_Table_Transition::TransitMethodL()
	{
	_LIT(KStringPoolStringF_Index_TableTransitMethod, "CStringPool::StringF_Index_Table transition");
	Context().DataLogger().LogInformation(KStringPoolStringF_Index_TableTransitMethod);

	Context().iResult = Context().iTFE == Context().iFA;
	}

inline CStringPool_UnitTestContext& CStringPool_StringF_Index_Table_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolshortCSCtorTransition,"CStringPool_ShortCSCtor_Transition");

inline CStringPool_ShortCSCtor_Transition::CStringPool_ShortCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolshortCSCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_ShortCSCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolshortCSCtorTransitMethod, "CStringPool::ShortCSCtor transition");
	Context().DataLogger().LogInformation(KStringPoolshortCSCtorTransitMethod);


	//Set the table used in this test
	Context().iTestTable=ShortCS::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");


	Context().iA = p.OpenStringL(KStringA());
	CleanupClosePushL(Context().iA);
	Context().iB = p.OpenStringL(KStringB());
	CleanupClosePushL(Context().iB);
	Context().iD = p.OpenStringL(KStringD());
	CleanupClosePushL(Context().iD);

	Context().iFA = p.OpenFStringL(KStringFA());
	CleanupClosePushL(Context().iFA);
	Context().iFB = p.OpenFStringL(KStringFB());
	CleanupClosePushL(Context().iFB);
	Context().iFD = p.OpenFStringL(KStringFD());
	CleanupClosePushL(Context().iFD);

	Context().iTE = p.String(0,Context().iTestTable);
//	Context().iTFE = p.StringF(0,Context().iTestTable);


	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);

	
	Context().iInt = 0;
	CleanupStack::Pop(6);
	}

inline CStringPool_UnitTestContext& CStringPool_ShortCSCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}
// ______________________________________________________________________________
//
_LIT(KStringPoolString_Index_TableTransition,"CStringPool_String_Index_Table_Transition");

inline CStringPool_String_Index_Table_Transition::CStringPool_String_Index_Table_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolString_Index_TableTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_String_Index_Table_Transition::TransitMethodL()
	{
	_LIT(KStringPoolString_Index_TableTransitMethod, "CStringPool::String_Index_Table transition");
	Context().DataLogger().LogInformation(KStringPoolString_Index_TableTransitMethod);

	Context().iResult = Context().iTE == Context().iA;
	}

inline CStringPool_UnitTestContext& CStringPool_String_Index_Table_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}



// ______________________________________________________________________________
//
_LIT(KStringPoolIrrelevantCSCtorTransition,"CStringPool_IrrelevantCSCtor_Transition");

inline CStringPool_IrrelevantCSCtor_Transition::CStringPool_IrrelevantCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolIrrelevantCSCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_IrrelevantCSCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolIrrelevantCSCtorTransitMethod, "CStringPool::IrrelevantCSCtor transition");
	Context().DataLogger().LogInformation(KStringPoolIrrelevantCSCtorTransitMethod);
	
	//Set the table used in this test
	Context().iTestTable=IrrelevantCS::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");

	Context().iA = p.OpenStringL(KStringA());
	Context().iB = p.OpenStringL(KStringB());
	Context().iD = p.OpenStringL(KStringD());

	Context().iFA = p.OpenFStringL(KStringFA());
	Context().iFB = p.OpenFStringL(KStringFB());
	Context().iFD = p.OpenFStringL(KStringFD());

	Context().iTE = p.String(0,Context().iTestTable);
//	Context().iTFE = p.StringF(0,Context().iTestTable);

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);


	Context().iFint = 0;
	Context().iInt = 0;
	}

inline CStringPool_UnitTestContext& CStringPool_IrrelevantCSCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolLargeCSCtorTransition,"CStringPool_LargeCSCtor_Transition");

inline CStringPool_LargeCSCtor_Transition::CStringPool_LargeCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolLargeCSCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_LargeCSCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolLargeCSCtorTransitMethod, "CStringPool::LargeCSCtor transition");
	Context().DataLogger().LogInformation(KStringPoolLargeCSCtorTransitMethod);
	
	//Set the table used in this test
	Context().iTestTable=LongCS::Table;
	Context().iStringPool.OpenL(Context().iTestTable);
	RStringPool p = Context().iStringPool;
	
	_LIT8(KStringA, "String");
	_LIT8(KStringB, "Another String");
	_LIT8(KStringD, "String");

	_LIT8(KStringFA, "String");
	_LIT8(KStringFB, "Another String");
	_LIT8(KStringFD, "sTRING");

	Context().iA = p.OpenStringL(KStringA());
	Context().iB = p.OpenStringL(KStringB());
	Context().iD = p.OpenStringL(KStringD());

	Context().iFA = p.OpenFStringL(KStringFA());
	Context().iFB = p.OpenFStringL(KStringFB());
	Context().iFD = p.OpenFStringL(KStringFD());

	Context().iTE = p.String(0,Context().iTestTable);
//	Context().iTFE = p.StringF(0,Context().iTestTable);

	Context().iADes.Set(KStringA);
	Context().iBDes.Set(KStringB);
	Context().iDDes.Set(KStringD);

	Context().iFADes.Set(KStringFA);
	Context().iFBDes.Set(KStringFB);
	Context().iFDDes.Set(KStringFD);

	Context().iInt = 0;
	Context().iFint = 0;
	}

inline CStringPool_UnitTestContext& CStringPool_LargeCSCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}
// ______________________________________________________________________________
//
_LIT(KStringPoolShortCtor_CBTransition,"CStringPool_Short_CB_Transition");

inline CStringPool_Short_CB_Transition::CStringPool_Short_CB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolShortCtor_CBTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}





inline void CStringPool_Short_CB_Transition::TransitMethodL()
	{
	_LIT(KStringPoolShortCtor_CBTransitMethod, "CStringPool::ShortCtor_CB transition");
	Context().DataLogger().LogInformation(KStringPoolShortCtor_CBTransitMethod);
	
	//Set the table used in this test
	Context().iTestTable=Short::Table;

	Context().iMyExtObj.a=0;
	Context().iMyExtObj2.a=0;
	Context().iMyExtObj3.a=0;
	Context().iStringPool.OpenL(Context().iTestTable,Context().iMyExtObj);
	CleanupClosePushL(Context().iStringPool);
	Context().iStringPool.OpenL(Context().iTestTable,Context().iMyExtObj2);
	Context().iStringPool.OpenL(Context().iTestTable,Context().iMyExtObj3);
	Context().iStringPool.Close();
	Context().iResult= (Context().iMyExtObj.a) && (Context().iMyExtObj2.a)&&(Context().iMyExtObj3.a);
	CleanupStack::Pop();//Context().iStringPool 
	}

inline CStringPool_UnitTestContext& CStringPool_Short_CB_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleTableCICtorTransition,"CStringPool_MultipleTableCICtor_Transition");

inline CStringPool_MultipleTableCICtor_Transition::CStringPool_MultipleTableCICtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleTableCICtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleTableCICtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleTableCICtorTransitMethod, "CStringPool::MultipleTableCICtor transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleTableCICtorTransitMethod);

	Context().iStringPool.OpenL();	

	_LIT8(KStringA, "AnotherString");
	CleanupClosePushL(Context().iFF);
	Context().iFF=Context().iStringPool.OpenFStringL(KStringA);	// Dynamic string
	CleanupStack::Pop();	// iFF
	

	Context().iTestTable=Short::Table;
	Context().iStringPool.OpenL(Context().iTestTable);	

	Context().iTestTable2=Short3::Table;	// Non-duplicate table
	Context().iStringPool.OpenL(Context().iTestTable2);	

	Context().iTestTable3=Short2::Table;
	Context().iStringPool.OpenL(Context().iTestTable3);	

	Context().iTestTable4=Short4::Table;	// Duplicate of a duplicate table
	Context().iStringPool.OpenL(Context().iTestTable4);	
	Context().iStringPool.OpenL(Context().iTestTable4);	// Add same table twice

	Context().iFA=Context().iStringPool.StringF(Short::EString, Context().iTestTable);	// String
	Context().iFB=Context().iStringPool.StringF(Short2::EString, Context().iTestTable3); // String
	Context().iFC=Context().iStringPool.StringF(Short3::EString3, Context().iTestTable2); // String3, non-duplicate
	Context().iFD=Context().iStringPool.StringF(Short4::EAnotherString , Context().iTestTable4); // AnotherString3 duplicate of a duplicate

	Context().iFE=Context().iStringPool.StringF(Short2::EAnotherString , Context().iTestTable3); // Duplicate of dynamic string
	
	Context().iIndex1=Context().iFA.Index(Context().iTestTable);	// 0
	Context().iIndex2=Context().iFB.Index(Context().iTestTable3);	// 1
	Context().iIndex3=Context().iFC.Index(Context().iTestTable2);	// 1
	Context().iIndex4=Context().iFD.Index(Context().iTestTable4);	// 0

	
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleTableCICtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}



// ______________________________________________________________________________
//
_LIT(KStringPoolMutlipleCICeqCTransition,"CStringPool_MutlipleCICeqC_Transition");

inline CStringPool_MutlipleCICeqC_Transition::CStringPool_MutlipleCICeqC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMutlipleCICeqCTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MutlipleCICeqC_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMutlipleCICeqCTransitMethod, "CStringPool::MutlipleCICeqC transition");
	Context().DataLogger().LogInformation(KStringPoolMutlipleCICeqCTransitMethod);
	_LIT8(KStringC, "String3");
	Context().iResult = (Context().iFC.DesC().CompareF(KStringC))==0;
	}

inline CStringPool_UnitTestContext& CStringPool_MutlipleCICeqC_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCIStringToIndexTransition,"CStringPool_MultipleCIStringToIndex_Transition");

inline CStringPool_MultipleCIStringToIndex_Transition::CStringPool_MultipleCIStringToIndex_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCIStringToIndexTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCIStringToIndex_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCIStringToIndexTransitMethod, "CStringPool::MultipleCIStringToIndex transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCIStringToIndexTransitMethod);
	if (Context().iIndex1==0 && Context().iIndex2==1 && Context().iIndex3==1 && Context().iIndex4==0)
		Context().iResult = ETrue;
	else
		Context().iResult = EFalse;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCIStringToIndex_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCIDynamicStringTransition,"CStringPool_MultipleCIDynamicString_Transition");

inline CStringPool_MultipleCIDynamicString_Transition::CStringPool_MultipleCIDynamicString_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCIDynamicStringTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCIDynamicString_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCIDynamicStringTransitMethod, "CStringPool::MultipleCIDynamicString transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCIDynamicStringTransitMethod);
	Context().iResult = Context().iFE==Context().iFF;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCIDynamicString_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCIIndexToStringDynamicTransition,"CStringPool_MultipleCIIndexToStringDynamic_Transition");

inline CStringPool_MultipleCIIndexToStringDynamic_Transition::CStringPool_MultipleCIIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCIIndexToStringDynamicTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCIIndexToStringDynamic_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCIIndexToStringDynamicTransitMethod, "CStringPool::MultipleCIIndexToStringDynamic transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCIIndexToStringDynamicTransitMethod);
	_LIT8(KStringE, "AnotherString");
	Context().iResult = (Context().iFE.DesC().CompareF(KStringE))==0;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCIIndexToStringDynamic_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// For case sensitive


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::MultipleTableCSCtor method.
 */
class CStringPool_MultipleTableCSCtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleTableCSCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleTableCSCtor_Transition is fully constructed.
	*/
	CStringPool_MultipleTableCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleTableCSCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleTableCSCtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleTableCSCtor_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleTableCSCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleTableCSCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleTableCSCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleTableCSCtor_Transition

// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleTableCSCtorTransition,"CStringPool_MultipleTableCSCtor_Transition");

inline CStringPool_MultipleTableCSCtor_Transition::CStringPool_MultipleTableCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleTableCSCtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleTableCSCtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleTableCSCtorTransitMethod, "CStringPool::MultipleTableCSCtor transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleTableCSCtorTransitMethod);
	
	Context().iStringPool.OpenL();	

	_LIT8(KStringA, "AnotherString");
	CleanupClosePushL(Context().iF);
	Context().iF=Context().iStringPool.OpenStringL(KStringA);	// Dynamic string
	CleanupStack::Pop();	// iF
	

	Context().iTestTable=ShortCS::Table;
	Context().iStringPool.OpenL(Context().iTestTable);	

	Context().iTestTable2=ShortCS3::Table;	// Non-duplicate table
	Context().iStringPool.OpenL(Context().iTestTable2);	

	Context().iTestTable3=ShortCS2::Table;
	Context().iStringPool.OpenL(Context().iTestTable3);	

	Context().iTestTable4=ShortCS4::Table;	// Duplicate of a duplicate table
	Context().iStringPool.OpenL(Context().iTestTable4);	
	Context().iStringPool.OpenL(Context().iTestTable4);	// Add same table twice

	Context().iA=Context().iStringPool.String(ShortCS::EString, Context().iTestTable);	// String
	Context().iB=Context().iStringPool.String(ShortCS2::EString, Context().iTestTable3); // String
	Context().iC=Context().iStringPool.String(ShortCS3::EString3, Context().iTestTable2); // String3, non-duplicate
	Context().iD=Context().iStringPool.String(ShortCS4::EAnotherString , Context().iTestTable4); // AnotherString3 duplicate of a duplicate

	Context().iE=Context().iStringPool.String(ShortCS2::EAnotherString , Context().iTestTable3); // Duplicate of dynamic string
	
	Context().iIndex1=Context().iA.Index(Context().iTestTable);		// 0
	Context().iIndex2=Context().iB.Index(Context().iTestTable3);	// 1
	Context().iIndex3=Context().iC.Index(Context().iTestTable2);	// 1
	Context().iIndex4=Context().iD.Index(Context().iTestTable4);	// 2

	
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleTableCSCtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMutlipleCSCeqCTransition,"CStringPool_MutlipleCSCeqC_Transition");

inline CStringPool_MutlipleCSCeqC_Transition::CStringPool_MutlipleCSCeqC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMutlipleCSCeqCTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MutlipleCSCeqC_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMutlipleCSCeqCTransitMethod, "CStringPool::MutlipleCSCeqC transition");
	Context().DataLogger().LogInformation(KStringPoolMutlipleCSCeqCTransitMethod);
	_LIT8(KStringC, "String3");
	Context().iResult = (Context().iC.DesC().CompareF(KStringC))==0;
	}

inline CStringPool_UnitTestContext& CStringPool_MutlipleCSCeqC_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCSStringToIndexTransition,"CStringPool_MultipleCSStringToIndex_Transition");

inline CStringPool_MultipleCSStringToIndex_Transition::CStringPool_MultipleCSStringToIndex_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCSStringToIndexTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCSStringToIndex_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCSStringToIndexTransitMethod, "CStringPool::MultipleCSStringToIndex transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCSStringToIndexTransitMethod);
	if (Context().iIndex1==0 && Context().iIndex2==1 && Context().iIndex3==1 && Context().iIndex4==2)
		Context().iResult = ETrue;
	else
		Context().iResult = EFalse;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCSStringToIndex_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCSDynamicStringTransition,"CStringPool_MultipleCSDynamicString_Transition");

inline CStringPool_MultipleCSDynamicString_Transition::CStringPool_MultipleCSDynamicString_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCSDynamicStringTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCSDynamicString_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCSDynamicStringTransitMethod, "CStringPool::MultipleCSDynamicString transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCSDynamicStringTransitMethod);
	Context().iResult = Context().iE==Context().iF;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCSDynamicString_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleCSIndexToStringDynamicTransition,"CStringPool_MultipleCSIndexToStringDynamic_Transition");

inline CStringPool_MultipleCSIndexToStringDynamic_Transition::CStringPool_MultipleCSIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleCSIndexToStringDynamicTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleCSIndexToStringDynamic_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleCSIndexToStringDynamicTransitMethod, "CStringPool::MultipleCSIndexToStringDynamic transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleCSIndexToStringDynamicTransitMethod);
	_LIT8(KStringE, "AnotherString");
	Context().iResult = (Context().iE.DesC().CompareF(KStringE))==0;
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleCSIndexToStringDynamic_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleTableDtorTransition,"CStringPool_MultipleTableDtor_Transition");

inline CStringPool_MultipleTableDtor_Transition::CStringPool_MultipleTableDtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolMultipleTableDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_MultipleTableDtor_Transition::TransitMethodL()
	{
	_LIT(KStringPoolMultipleTableDtorTransitMethod, "CStringPool::MultipleTableDtor transition");
	Context().DataLogger().LogInformation(KStringPoolMultipleTableDtorTransitMethod);
	Context().iA.Close();
	Context().iB.Close();
	Context().iC.Close();
	Context().iD.Close();
	Context().iE.Close();
	Context().iF.Close();

	Context().iFA.Close();
	Context().iFB.Close();
	Context().iFC.Close();
	Context().iFD.Close();
	Context().iFE.Close();
	Context().iFF.Close();

	Context().iStringPool.Close();
	}

inline CStringPool_UnitTestContext& CStringPool_MultipleTableDtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolBEU55DJG3Transition,"CStringPool_BEU55DJG3_Transition");

inline CStringPool_BEU55DJG3_Transition::CStringPool_BEU55DJG3_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolBEU55DJG3Transition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_BEU55DJG3_Transition::TransitMethodL()
	{
	_LIT(KStringPoolBEU55DJG3TransitMethod, "CStringPool::BEU55DJG3 transition");
	_LIT(KMinus1, "The Index() function returns = KErrNotFound");
	_LIT(KOther, "The Index() function returns something != -1");
	Context().DataLogger().LogInformation(KStringPoolBEU55DJG3TransitMethod);
	TInt result;
	result = Context().iEmptyOne.Index(Context().iTestTable);
	if (result == KErrNotFound)
		{
		Context().iResult = 1; //ETrue 
		Context().DataLogger().LogInformation(KMinus1);
		}
	else
		{
		Context().iResult = 0 ;//EFalse
		Context().DataLogger().LogInformation(KOther);
		}

	}

inline CStringPool_UnitTestContext& CStringPool_BEU55DJG3_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolKRN56NDEZTransition,"CStringPool_KRN56NDEZ_Transition");

inline CStringPool_KRN56NDEZ_Transition::CStringPool_KRN56NDEZ_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolKRN56NDEZTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_KRN56NDEZ_Transition::TransitMethodL()
	{
	_LIT(KStringPoolKRN56NDEZTransitMethod, "CStringPool::KRN56NDEZ transition");
	_LIT(KTestOK,"\n Test Passed");
	_LIT(KTestFailed,"\nTest Failed");
	

	Context().DataLogger().LogInformation(KStringPoolKRN56NDEZTransitMethod);

	//Open 2 tables with some string duplicates and open them
	Context().iStringPool.OpenL(Short::Table);
	Context().iStringPool.OpenL(Short2::Table);

	Context().iFAString = Context().iStringPool.StringF(Short2::EString,Short2::Table);
	const TStringTable* originalTable= Context().iFAString.OriginalTableRef();

	Context().iFADuplicateString = Context().iStringPool.StringF(Short::EString,Short::Table);
	const TStringTable* ShortTable= Context().iFA.OriginalTableRef();


	if (ShortTable==originalTable)
		{
		Context().iResult=ETrue;
		Context().DataLogger().LogInformation(KTestOK);
		}
	else
		{
		Context().iResult=EFalse;
		Context().DataLogger().LogInformation(KTestFailed);
		}

	}

inline CStringPool_UnitTestContext& CStringPool_KRN56NDEZ_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolAPY57TEH3Transition,"CStringPool_APY57TEH3_Transition");

inline CStringPool_APY57TEH3_Transition::CStringPool_APY57TEH3_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolAPY57TEH3Transition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_APY57TEH3_Transition::TransitMethodL()
	{
	_LIT(KStringPoolAPY57TEH3TransitMethod, "CStringPool::APY57TEH3 transition");
	Context().DataLogger().LogInformation(KStringPoolAPY57TEH3TransitMethod);
	
	
	Context().iResult=(Context().iEmptyOne!=Context().iTFE);


	}

inline CStringPool_UnitTestContext& CStringPool_APY57TEH3_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolHAD57SK27Transition,"CStringPool_HAD57SK27_Transition");

inline CStringPool_HAD57SK27_Transition::CStringPool_HAD57SK27_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KStringPoolHAD57SK27Transition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CStringPool_HAD57SK27_Transition::TransitMethodL()
	{
	_LIT(KStringPoolHAD57SK27TransitMethod, "CStringPool::HAD57SK27 transition");
	Context().DataLogger().LogInformation(KStringPoolHAD57SK27TransitMethod);
	

	// Create string pool and load HTTP static table
	RStringPool pool;

	pool.OpenL();

	_LIT8(KUndeletable_EX, "_ex");
	Context().iUndeletable_EX = pool.OpenStringL(KUndeletable_EX);

	_LIT8(KUndeletable_A," _a");
	Context().iUndeletable_A = pool.OpenStringL(KUndeletable_A());

	// Load SecurityPolicyProperties static table
	pool.OpenL(SecurityPolicyPluginStrings::Table);

	// Close strings opened
	Context().iUndeletable_EX.Close();
	Context().iUndeletable_A.Close();

	// Close the string pool
	pool.Close();
	//if at this point the m pool string pool closes fine .the test has passed
	_LIT(KTestHAD57SK27OK,"\n Defect HAD57SK27OK test : OK\n");
	Context().DataLogger().LogInformation(KTestHAD57SK27OK);
	Context().iResult=ETrue;

	}

inline CStringPool_UnitTestContext& CStringPool_HAD57SK27_Transition::Context() const
	{
	return REINTERPRET_CAST(CStringPool_UnitTestContext&,iUTContext);
	}

