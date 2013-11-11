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

/**
 @internalComponent
*/

#ifndef __TEXTBASEPANIC_H__
#define __TEXTBASEPANIC_H__

//#ifdef __GDI_H__
void TextBasePanic(TTextBasePanic aPanicCode);
void TextBasePanicWithCondAndInfo(TTextBasePanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);
//#endif 

void TextBasePanicLogWithInfo(const TDesC& aCommand, const TDesC& aCondition, const TDesC& aFileName, TInt aLine);

// helper macros
#define TEXTBASE_ASSERT_FILENAME(f)	_LIT(KPanicFileName, f);
#define TEXTBASE_ASSERT_PANICNAME(p)	_LIT(KPanicEnum ## p, # p);
#define TEXTBASE_ASSERT_PANICCOMMAND(a)	_LIT(KPanicCommand, a);
#define TEXTBASE_ASSERT_CONDITION(c)	_LIT(KPanicCondition, c);

/* Use these macros for GDI Asserts & Panics with diagnostic logging:
   TEXTBASE_ASSERT_ALWAYS, TEXTBASE_ASSERT_ALWAYS_GENERAL, TEXTBASE_ASSERT_DEBUG
   and TEXTBASE_ASSERT_DEBUG_GENERAL
   Note that the "do { ... } while (0)" construction forces the invocation to have a ';'
   and makes the macro expansion safe in nested "if ... else ..." clauses that forget to use the
   { braces } in the Coding Standard.
 */

#define TEXTBASE_ASSERT_ALWAYS(c, p) \
		do  { \
			if (!(c)) \
				{ \
				TEXTBASE_ASSERT_FILENAME(__FILE__); \
				TEXTBASE_ASSERT_PANICNAME(p); \
				TEXTBASE_ASSERT_CONDITION(#c); \
				TextBasePanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)
	
#define TEXTBASE_ASSERT_ALWAYS_GENERAL(c, a) \
		do  { \
			if (!(c)) \
				{ \
				TEXTBASE_ASSERT_FILENAME(__FILE__); \
				TEXTBASE_ASSERT_PANICCOMMAND(#a); \
				TEXTBASE_ASSERT_CONDITION(#c); \
				TextBasePanicLogWithInfo(KPanicCondition, KPanicCommand, KPanicFileName, __LINE__); \
				a ; \
				} \
			} while (0)

#if defined(_DEBUG)
#define TEXTBASE_ASSERT_DEBUG(c, p) \
		do  { \
			if (!(c)) \
				{ \
				TEXTBASE_ASSERT_FILENAME(__FILE__); \
				TEXTBASE_ASSERT_PANICNAME(p); \
				TEXTBASE_ASSERT_CONDITION(#c); \
				TextBasePanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)

#define TEXTBASE_ASSERT_DEBUG_GENERAL(c, a) \
		do  { \
			if (!(c)) \
				{ \
				TEXTBASE_ASSERT_FILENAME(__FILE__); \
				TEXTBASE_ASSERT_PANICCOMMAND(#a); \
				TEXTBASE_ASSERT_CONDITION(#c); \
				TextBasePanicLogWithInfo(KPanicCondition, KPanicCommand, KPanicFileName, __LINE__); \
				a ; \
				} \
			} while (0)
			
#else
#define TEXTBASE_ASSERT_DEBUG(c, p)
#define TEXTBASE_ASSERT_DEBUG_GENERAL(c, p)

#endif

#endif
