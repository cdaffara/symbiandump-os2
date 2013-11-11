// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32std.h>
#include <e32err.h>
#include <e32debug.h>
#include <e32property.h>
#include <bacntf.h>
#include <bautils.h>
#include "LocaleRepository.h"
#include "InitialiseLocale.h"
#include <hal.h>

using namespace NCentralRepositoryConstants ;


// Text constants for debug messages
#ifdef _DEBUG
_LIT (KBadChangeNotification, "InitialiseLocale: Bad Change Notification") ;
#endif

//
// CExtendedLocaleManager::LocaleChanged
//
// Callback function to be attached to an instance of 
// CEnvironmentChangeNotifier to persist system locale data.
//
// Note that this has to be a static member function so that
// it can be encapsulated into a TCallback object for use with
// a CEnvironmentChangeNotifier - we get round this by passing
// in a "this" pointer so that while remaining a static function
// it can access all the member data anyway!
TInt CExtendedLocaleManager::LocaleChanged(TAny* aPtr)
	{
	CExtendedLocaleManager* thisLocaleManager = (CExtendedLocaleManager *) aPtr ;
	
	if(!thisLocaleManager->iChangeNotifier)	
		{
		#ifdef _DEBUG		
		RDebug::Print(KBadChangeNotification);
		#endif
		return EFalse;
		} 


 	TInt stat = thisLocaleManager->iChangeNotifier->Change();
	if(stat & EChangesLocale)
		{
		//
	   	// System Locale data has been updated, persist
	   	// changes by writing to repository.
		TRAPD(err, thisLocaleManager->PersistLocaleL()); // No way to handle errors within this callback function, so ignore them
		if(err != KErrNone)	
			{
			err=KErrNone;
			}
		}
	if(stat & EChangesSystemTime)
		{
		// changes by writing to HAL
		TInt ret = BaflUtils::PersistHAL();
		__ASSERT_DEBUG(ret == KErrNone, User::Invariant() );
		}
	
	return ETrue;
	}


//
// CExtendedLocaleManager::NewLC() 
// CExtendedLocaleManager::NewL() 
//
// Usual 2 phase construction factory NewL NewLC classes
// 	
CExtendedLocaleManager* CExtendedLocaleManager::NewLC() 
	{
	CExtendedLocaleManager* me = new(ELeave)CExtendedLocaleManager();
	CleanupStack::PushL(me) ;
	me->ConstructL() ;
	return me;
	}
	
	
CExtendedLocaleManager* CExtendedLocaleManager::NewL() 
	{
	CExtendedLocaleManager* me = CExtendedLocaleManager::NewLC() ;
	CleanupStack::Pop(me) ;
	return me;
	}
	
		


//
// CExtendedLocaleManager::CExtendedLocaleManager	
// 
// Constructor - doesn't really do anything!
//
CExtendedLocaleManager::CExtendedLocaleManager() 
	{
	}



//
// CExtendedLocaleManager::ConstructL()
//
// Second phase constructor, initialises locale data from persisted (or default)
// values in repository, creates an instance of CEnvironmentChangeNotifier, and
// attaches the callback function, passing in our "this" pointer to get round the
// requirement for the callback function to be a static. 
//
void CExtendedLocaleManager::ConstructL()
	{
	
	TUid LocalePropertyUid;
	LocalePropertyUid.iUid = KUidLocalePersistProperties ;
	
	TInt error = RProperty::Define((TUint)EInitialisationState, RProperty::EInt,TSecurityPolicy::EAlwaysPass,TSecurityPolicy(ECapabilityWriteDeviceData));
	if(error != KErrAlreadyExists)
	{
	User::LeaveIfError(error);
	
	//If the property definition fails bail out. Note that if the 
	// property has already been defined (because the locale initialisation code
	// has already been called) this call will return KErrAlreadyExists so locale
	// initialisation will only take place once. 
	//
	// Set property to indicate initialisation in progress
	User::LeaveIfError(RProperty::Set(LocalePropertyUid, EInitialisationState, ELocaleInitialisationInProgress)) ;

	TUid LocaleRepositoryUid;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid;

	
	// Initialise system locale data from repository contents.
	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid); 
	TRAPD(err, InitialiseLocaleL(*repository));  //this function should no longer attempt to set the failure flag 
	if (err != KErrNone)
		{
		// Failed to read DLL name, load corresponding Locale aspect or read value from DLL,
		// Set property to indicate initialisation failure and bail out!
		User::LeaveIfError(RProperty::Set(LocalePropertyUid, EInitialisationState, ELocaleInitialisationFailed)) ;
		}
	CleanupStack::PopAndDestroy(repository); 
	
	}
	
	//After Initialisation, monitor the system locale for changes. If initialiselocale.exe is restarted, 
	//then initialisation will not be executed, going to monitor directly.

	//
	// Create and install an CActiveScheduler to look after the active object
	// in the change notifier
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
		

	// Create the change notifier and install the callback function
	const TCallBack myCallBackFunction(&CExtendedLocaleManager::LocaleChanged, this);
	iChangeNotifier = CEnvironmentChangeNotifier::NewL(CActive::EPriorityStandard, myCallBackFunction);
	iChangeNotifier->Start();

	// Set property to indicate initialisation complete
	User::LeaveIfError(RProperty::Set(LocalePropertyUid, EInitialisationState, ELocaleInitialisationComplete));
	
	//now call RProcess::Rendezvous to indicate initialiseLocale is completed with no error
	RProcess().Rendezvous(KErrNone);

	// All ready to go! Start the active sheduler. 
	CActiveScheduler::Start();	

	}


//
// CExtendedLocaleManager::~CExtendedLocaleManager()
// 
// Destructor to tidy up on shutdown (if needed!).
CExtendedLocaleManager::~CExtendedLocaleManager()	
	{
	delete iChangeNotifier;
	delete iActiveScheduler;
	}





//
// CExtendedLocaleManager::PersistLocaleL
//
// Persist system locale data by writing it to a repository
void CExtendedLocaleManager::PersistLocaleL()
	{
	TUid LocaleRepositoryUid;
	LocaleRepositoryUid.iUid = KLocalePersistRepositoryUid ;

	CRepository* repository = CRepository::NewLC(LocaleRepositoryUid) ;

	// Refresh extended locale with current system settings
   	iExtendedLocale.LoadSystemSettings();
   	
	// Perform repository update in a transaction so that we're
	// guaranteed to retain consistent state of repository contents.
	User::LeaveIfError(repository->StartTransaction(CRepository::EReadWriteTransaction));
	repository->CleanupFailTransactionPushL();

	TBuf<KMaxFileName> DllName ;

	//
	// Locale DLL for Language Settings
	User::LeaveIfError(iExtendedLocale.GetLocaleDllName(ELocaleLanguageSettings, DllName));
	User::LeaveIfError(repository->Set(KLocaleLanguageDll, DllName));
	
	//	
	// Locale DLL for Collation/Charset settings
	User::LeaveIfError(iExtendedLocale.GetLocaleDllName(ELocaleCollateSetting, DllName));
	User::LeaveIfError(repository->Set(KLocaleCollationDll, DllName));
		
	//
	// Locale DLL for Locale settings
	User::LeaveIfError(iExtendedLocale.GetLocaleDllName(ELocaleLocaleSettings, DllName));
	User::LeaveIfError(repository->Set(KLocaleLocaleDll, DllName));
	
	//
	#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
	// Locale DLL for Time and date settings
	User::LeaveIfError(iExtendedLocale.GetLocaleDllName(ELocaleTimeDateSettings, DllName));
	User::LeaveIfError(repository->Set(KLocaleTimeDateDll, DllName));
	#endif
			
	//
	// Locale customisations (contents of embedded TLocale)
	TLocale* locale = iExtendedLocale.GetLocale() ;
	User::LeaveIfError(repository->Set(KLocaleCountryCode, locale->CountryCode()));
	User::LeaveIfError(repository->Set(KLocaleUtcOffset, User::UTCOffset().Int()));
	User::LeaveIfError(repository->Set(KLocaleDateFormat, locale->DateFormat()));
	User::LeaveIfError(repository->Set(KLocaleTimeFormat, locale->TimeFormat()));
	User::LeaveIfError(repository->Set(KLocaleCurrencySymbolPosition, locale->CurrencySymbolPosition()));
	User::LeaveIfError(repository->Set(KLocaleCurrencySpaceBetween, locale->CurrencySpaceBetween()));
	User::LeaveIfError(repository->Set(KLocaleCurrencyDecimalPlaces, locale->CurrencyDecimalPlaces()));
	User::LeaveIfError(repository->Set(KLocaleCurrencyNegativeInBrackets, locale->CurrencyNegativeInBrackets()));
	User::LeaveIfError(repository->Set(KLocaleCurrencyTriadsAllowed, locale->CurrencyTriadsAllowed()));
	User::LeaveIfError(repository->Set(KLocaleThousandsSeparator, (TInt)locale->ThousandsSeparator()));
	User::LeaveIfError(repository->Set(KLocaleDecimalSeparator, (TInt)locale->DecimalSeparator()));
	
	TInt index ;
	for (index = 0; index < KMaxDateSeparators; index++)
		{
		User::LeaveIfError(repository->Set(KLocaleDateSeparatorBase+index, (TInt)locale->DateSeparator(index)));
		}
		
	for (index = 0; index < KMaxTimeSeparators; index++)
		{
		User::LeaveIfError(repository->Set(KLocaleTimeSeparatorBase +index, (TInt)locale->TimeSeparator(index)));
		}
		
 	User::LeaveIfError(repository->Set(KLocaleAmPmSpaceBetween, locale->AmPmSpaceBetween()));
	User::LeaveIfError(repository->Set(KLocaleAmPmSymbolPosition, locale->AmPmSymbolPosition()));
	User::LeaveIfError(repository->Set(KLocaleWorkDays, (TInt)locale->WorkDays()));
	User::LeaveIfError(repository->Set(KLocaleStartOfWeek, locale->StartOfWeek()));
	User::LeaveIfError(repository->Set(KLocaleClockFormat, locale->ClockFormat()));
	User::LeaveIfError(repository->Set(KLocaleUnitsGeneral, locale->UnitsGeneral()));
	User::LeaveIfError(repository->Set(KLocaleUnitsDistanceShort, locale->UnitsDistanceShort()));
	User::LeaveIfError(repository->Set(KLocaleUnitsDistanceLong, locale->UnitsDistanceLong()));
	User::LeaveIfError(repository->Set(KLocaleCurrencyNegativeFormat, locale->NegativeCurrencyFormat()));
	User::LeaveIfError(repository->Set(KLocaleCurrencyNegativeLoseSpace, locale->NegativeLoseSpace()));
	User::LeaveIfError(repository->Set(KLocaleCurrencySymbolOpposite, locale->NegativeCurrencySymbolOpposite()));
	
	TCurrencySymbol currencySymbol ;
	currencySymbol.Set() ;
	User::LeaveIfError(repository->Set(KLocaleCurrencySymbol, currencySymbol));
	
	for (index = 0; index < 3; index++)
		{
		User::LeaveIfError(repository->Set(KLocaleLanguageDowngradeBase+index, locale->LanguageDowngrade(index)));
		}
		
	User::LeaveIfError(repository->Set(KLocaleDigitType, locale->DigitType()));
	User::LeaveIfError(repository->Set(KLocaleDeviceTimeState, locale->DeviceTime()));
	
	// Completed sucessfully, commit transaction and close registry
	TUint32 transactionErr ;
 	User::LeaveIfError(repository->CommitTransaction(transactionErr));
 	CleanupStack::Pop(); // pop repository->CleanupFailTransactionPushL item
	CleanupStack::PopAndDestroy (repository);	
	}
	
	
	
	
//
// Initialise System Locale data from persisted (or default) data held in a
//repository.
void CExtendedLocaleManager::InitialiseLocaleL(CRepository& aRepository)
	{

	// Get names of DLLs to be loaded	
	TBuf<KMaxFileName> DllName ;
	
	#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
	// Collation/Charset settings
	User::LeaveIfError(aRepository.Get(KLocaleCollationDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(ELocaleCollateSetting,DllName));
		
	// Language Settings
	User::LeaveIfError(aRepository.Get(KLocaleLanguageDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(ELocaleLanguageSettings,DllName));
	
	// Locale settings
	User::LeaveIfError(aRepository.Get(KLocaleLocaleDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(ELocaleLocaleSettings,DllName));
	
	// Time and Date settings
	User::LeaveIfError(aRepository.Get(KLocaleTimeDateDll, DllName));	
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(ELocaleTimeDateSettings,DllName));
	
	#else
	
	User::LeaveIfError(aRepository.Get(KLocaleCollationDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(DllName));

	User::LeaveIfError(aRepository.Get(KLocaleLanguageDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(DllName));

	User::LeaveIfError(aRepository.Get(KLocaleLocaleDll, DllName));
	User::LeaveIfError(iExtendedLocale.LoadLocaleAspect(DllName));
	
	#endif

	iExtendedLocale.SaveSystemSettings();
	
	// Locale customisations
	TLocale* locale = iExtendedLocale.GetLocale() ;

	TInt intValue ;	
	User::LeaveIfError(aRepository.Get(KLocaleCountryCode, intValue));
	locale->SetCountryCode(intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleUtcOffset, intValue));
	User::SetUTCOffset(intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleDateFormat, intValue));
	locale->SetDateFormat((TDateFormat)intValue) ;
	
	User::LeaveIfError(aRepository.Get(KLocaleTimeFormat, intValue));
	locale->SetTimeFormat((TTimeFormat)intValue) ;
	
	User::LeaveIfError(aRepository.Get(KLocaleCurrencySymbolPosition, intValue));
	locale->SetCurrencySymbolPosition((TLocalePos)intValue) ;
	
	User::LeaveIfError(aRepository.Get(KLocaleCurrencySpaceBetween, intValue));
	locale->SetCurrencySpaceBetween((TBool)intValue) ;
	
	User::LeaveIfError(aRepository.Get(KLocaleCurrencyDecimalPlaces, intValue));
	locale->SetCurrencyDecimalPlaces((TBool)intValue) ;
		
	User::LeaveIfError(aRepository.Get(KLocaleCurrencyNegativeInBrackets, intValue));
	locale->SetCurrencyNegativeInBrackets((TBool)intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleCurrencyTriadsAllowed, intValue));
	locale->SetCurrencyTriadsAllowed((TBool)intValue) ;

	TCurrencySymbol currencySymbol ;
	User::LeaveIfError(aRepository.Get(KLocaleCurrencySymbol, currencySymbol));
	iExtendedLocale.SetCurrencySymbol(currencySymbol) ;

	User::LeaveIfError(aRepository.Get(KLocaleThousandsSeparator, intValue));
	locale->SetThousandsSeparator(intValue) ;
		
	User::LeaveIfError(aRepository.Get(KLocaleDecimalSeparator, intValue));
	locale->SetDecimalSeparator(intValue) ;

	TInt index ;
	for (index = 0; index < KMaxDateSeparators; index++)
		{
		User::LeaveIfError(aRepository.Get(KLocaleDateSeparatorBase+index, intValue));
		locale->SetDateSeparator(intValue, index) ;
		}
		
	for (index = 0; index < KMaxTimeSeparators; index++)
		{
		User::LeaveIfError(aRepository.Get(KLocaleTimeSeparatorBase+index, intValue));
		locale->SetTimeSeparator(intValue, index) ;
		}

	User::LeaveIfError(aRepository.Get(KLocaleAmPmSpaceBetween, intValue));
	locale->SetAmPmSpaceBetween(intValue) ;
		
	User::LeaveIfError(aRepository.Get(KLocaleAmPmSymbolPosition, intValue));
	locale->SetAmPmSymbolPosition((TLocalePos)intValue) ;
		
	User::LeaveIfError(aRepository.Get(KLocaleWorkDays, intValue));
	locale->SetWorkDays(intValue) ;
	
	User::LeaveIfError(aRepository.Get(KLocaleStartOfWeek, intValue));
	locale->SetStartOfWeek((TDay)intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleClockFormat, intValue));
	locale->SetClockFormat((TClockFormat)intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleUnitsGeneral, intValue));
	locale->SetUnitsGeneral((TUnitsFormat)intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleUnitsDistanceShort, intValue));
	locale->SetUnitsDistanceShort((TUnitsFormat)intValue) ;

	User::LeaveIfError(aRepository.Get(KLocaleUnitsDistanceLong, intValue));
	locale->SetUnitsDistanceLong((TUnitsFormat)intValue) ;		

	User::LeaveIfError(aRepository.Get(KLocaleCurrencyNegativeFormat, intValue));
	locale->SetNegativeCurrencyFormat((TLocale::TNegativeCurrencyFormat)intValue) ;		

	User::LeaveIfError(aRepository.Get(KLocaleCurrencyNegativeLoseSpace, intValue));
	locale->SetNegativeLoseSpace(intValue) ;	

	User::LeaveIfError(aRepository.Get(KLocaleCurrencySymbolOpposite, intValue));
	locale->SetNegativeCurrencySymbolOpposite(intValue) ;	

	for (index = 0; index < 3; index++)
		{
		User::LeaveIfError(aRepository.Get(KLocaleLanguageDowngradeBase+index, intValue));
		locale->SetLanguageDowngrade(index, (TLanguage)intValue) ;
		}

	User::LeaveIfError(aRepository.Get(KLocaleDigitType, intValue));
	locale->SetDigitType((TDigitType)intValue) ;	
		
	User::LeaveIfError(aRepository.Get(KLocaleDeviceTimeState, intValue));
	locale->SetDeviceTime((TLocale::TDeviceTimeState)intValue) ;	
	
	//
	// Sucessfully loaded a complete set of persisted/default Locale data - write to
	// system locale data.
	User::LeaveIfError(iExtendedLocale.SaveSystemSettings());
		
	}
	
	
// Entry point for the executable
TInt E32Main()
	{
	TInt result = KErrNone;
	CTrapCleanup* tc = CTrapCleanup::New();
	if(!tc)
		{
		User::Panic(KLocaleInitPanic, KErrNoMemory) ;
		}
	__UHEAP_MARK;
	
	CExtendedLocaleManager* localeManager = NULL ;
	TRAPD(err, (localeManager = CExtendedLocaleManager::NewL()));
	if(err != KErrNone)
		{
		result = err;		
		}	
	__UHEAP_MARKEND;
	
	delete localeManager;
	delete tc;
	return result;
	}

