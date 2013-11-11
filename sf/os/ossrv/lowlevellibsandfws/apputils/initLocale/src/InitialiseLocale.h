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

/**
 @file
 @internalTechnology
*/
  
#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>
#include <bacntf.h>
#include <bautils.h>
#include <e32const.h>
#include <centralrepository.h>


//
// UID to identify a publish/subscribe used to indicate
// progress of locale initialisation and prevent repeated
//initialisation.
const TInt32 KUidLocalePersistProperties = 0x1020E4D2 ;

_LIT(KLocaleInitPanic, "Locale Initialisation: ");

//
// Publish subscribe keys associated with locale initialisation.
enum LocalePersistPropertyKeys
	{
	EInitialisationState // Only one property in use at the moment
	};
	
//
// Possible values of publish subscribe key EInitialisationState
enum LocalePersistInitialisationState
	{
	ELocaleInitialisationInProgress,
	ELocaleInitialisationComplete,
	ELocaleInitialisationFailed
	};
	


//
// class CExtendedLocaleManager
//
// Initialises system-wide locale data and then persists
// new values to a repository on changes.
class CExtendedLocaleManager : CBase
	{
public:
	static CExtendedLocaleManager* NewL();
	static CExtendedLocaleManager* NewLC();
	CExtendedLocaleManager();
	~CExtendedLocaleManager();

	
private:	
	void ConstructL();
	static TInt LocaleChanged(TAny*) ;
	void PersistLocaleL() ;
	void InitialiseLocaleL(CRepository& aRepository) ;
	
private:	
	CEnvironmentChangeNotifier* iChangeNotifier	;
	CActiveScheduler*	iActiveScheduler ;
	TExtendedLocale iExtendedLocale;
	};
