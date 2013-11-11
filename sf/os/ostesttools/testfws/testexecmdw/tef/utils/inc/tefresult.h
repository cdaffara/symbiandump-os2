/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This contains TEFResult.h
*
*/



#ifndef __TEF_RESULT_H__
#define __TEF_RESULT_H__

class MTEFResult
/**
 * @internalComponent
 * @test
 *
 * Test error handling
 */
	{
public:
	/*
	 * Set the error
	 *
	 * @param	- aError the error to set
	 */
	virtual void		SetError(const TInt aError) =0;

	/*
	 * Set the asynchronus error
	 *
	 * @param	aIndex - block index that the error belongs to
	 * @param	aError - the error to set
	 */
	virtual void		SetAsyncError(const TInt aIndex, const TInt aError) =0;

	/*
	 * Set the block result
	 *
	 * @param	aResult - the result to set
	 *
	 * @see		TVerdict
	 */
	virtual void		SetBlockResult(const TVerdict aResult) =0;

	/*
	 * Get the block result
	 *
	 * @return	the result
	 *
	 * @see		TVerdict
	 */
	virtual TVerdict	BlockResult() =0;
	};

#endif // __TEF_RESULT_H__
