/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Defines a read-only interface for UPS Database and classes to keep filter and record objects.
*
*/


/**
 @file
 @publishedPartner
 @released
*/
 
#ifndef UPSDB_H
#define UPSDB_H

#include <s32file.h>
#include <d32dbms.h>
#include <ups/upstypes.h>

namespace UserPromptService
{

_LIT(KDecisionViewPanic,"UpsDbView");

NONSHARABLE_CLASS(CDecisionView):public CActive
/**
   Stores a handle to a view object and provides a method to retrieve a record from the view.
 */
 	{	
 	friend class CDecisionDb;
 	friend class CDecisionDbW;
 	
public:	
 	IMPORT_C static CDecisionView *NewLC();
 	
 	~CDecisionView();
 	
	IMPORT_C void EvaluateView(TRequestStatus& aStatus);	
 	IMPORT_C CDecisionRecord* NextDecisionL();

protected:
 	CDecisionView();
 	static CDecisionRecord* GenerateRecordL(RDbRowSet& aRowSet, CDbColSet* aColSet);	
	/**
		From CActive. Handles an evaluation request completion event and evaluate next part if required.
	 */
	void RunL();	
	/**
		From CActive. Cancels evaluation.
	 */
	void DoCancel();	
	/**
		From CActive. Handles errors.
	 */
	TInt RunError(TInt aError);
	 	
private:
	/**
	   Handle to the view object generated from an SQL query
	 */
	RDbView iDbView;	
	/**
	   Pointer to the decision database column set definition
	 */
	CDbColSet *iColSet;	
	/**
	   Copy of the status variable of the client which requested the evaluation service
	 */
	TRequestStatus* iClientStatus;
 	};


NONSHARABLE_CLASS(CDecisionDb):public CBase
/**
	A read-only interface for UPS decision database. Provide methods to query database.
 */
	{
public:
	IMPORT_C CDecisionView *CreateViewL(CDecisionFilter& aFilter);
	
	~CDecisionDb();

protected:
	CDecisionDb();
	void CreateSqlStatementLC(CDecisionFilter& aFilter, RBuf& aSql);	
	void DoubleSingleQuotesL(const TDesC& aSource, RBuf& aDestination);
	
private:
	const TDesC* GetComparisonOperator(const TUint32& aFlag) const;
	void AppendQueryInteger(const TUint32& aValue, TDes& aSql, TBool& aMultiple, const TUint32& aFlag, const TUint32& aSetCol, const TDesC& aColName);
	void AppendQueryStringL(const TDesC& aValue, TDes& aSql, TBool& aMultiple, const TUint32& aFlag, const TUint32& aSetCol, const TDesC& aColName);
	
protected:
	/**
	 * Handle to the decision store database
	 */
	RDbStoreDatabase iDatabase;
	/**
	 * Pointer to the database file storage object
	 */
	CFileStore* iStore;	
	};

}//namespace UserPromptService

#endif //UPSDB_H
