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
// XPATH error codes
//



/**
 @file
 @publishedAll
 @released
*/

#ifndef XMLENGDOMXPATHERRORS_H
#define XMLENGDOMXPATHERRORS_H

/**
 *  Syntax error compiling XPath expression.
 *  @see TXmlEngXPathEvaluator::CreateExpressionL
 */ 
const TInt KXmlEngErrXPathSyntax = -32399;
/**
 * General error evaluating XPath expression.
 */ 
const TInt KXmlEngErrXPathResult = -32398;

#endif // XMLENGDOMXPATHERRORS_H

