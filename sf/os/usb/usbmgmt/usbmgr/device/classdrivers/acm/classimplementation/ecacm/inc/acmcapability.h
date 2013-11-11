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
* Defines USB CDC Control Class supported feature subset
*
*/

/**
 @file
*/

#ifndef __ACMCAPABILITY_H__
#define __ACMCAPABILITY_H__

/** 
 *	Country Code Selection -------------------------------
 */
#undef	 ENABLE_ACM_CF_COUNTRY_SETTING
#define DISABLE_ACM_CF_COUNTRY_SETTING

#if( defined(DISABLE_ACM_CF_COUNTRY_SETTING) && defined(ENABLE_ACM_CF_COUNTRY_SETTING) )

	#error Country-Setting Feature Cannot be Both Enabled and Disabled

#elif defined(DISABLE_ACM_CF_COUNTRY_SETTING)

	// feature is not present

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

	// feature is present

#else

	#error Uncontrolled Country-Setting Feature

#endif

/** 
 *	Abstract State Selection (Idle) ----------------------
 */
#undef	 ENABLE_ACM_CF_IDLE_SETTING
#define DISABLE_ACM_CF_IDLE_SETTING

#if( defined(DISABLE_ACM_CF_IDLE_SETTING) && defined(ENABLE_ACM_CF_IDLE_SETTING) )

	#error Abstract Idle Setting Feature Cannot be Both Enabled and Disabled

#elif defined(DISABLE_ACM_CF_IDLE_SETTING)

	// feature is not present

#elif defined(ENABLE_ACM_CF_IDLE_SETTING)

	// feature is present

#else

	#error Uncontrolled Abstract Idle Setting Feature

#endif

/** 
 *	Abstract State Selection (Multiplex) -----------------
 */
#undef	 ENABLE_ACM_CF_DATA_MULTIPLEX
#define DISABLE_ACM_CF_DATA_MULTIPLEX

#if( defined(DISABLE_ACM_CF_DATA_MULTIPLEX) && defined(ENABLE_ACM_CF_DATA_MULTIPLEX) )

	#error Abstract Data Multiplex Feature Cannot be Both Enabled and Disabled

#elif defined(DISABLE_ACM_CF_DATA_MULTIPLEX)

	// feature is not present

#elif defined(ENABLE_ACM_CF_DATA_MULTIPLEX)

	// feature is present

#else

	#error Uncontrolled Abstract Data Multiplex Feature

#endif

#endif // __ACMCAPABILITY_H__
