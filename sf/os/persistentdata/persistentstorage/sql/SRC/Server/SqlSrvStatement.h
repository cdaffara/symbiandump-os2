// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NTT DOCOMO, INC - Fix for defect 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#ifndef __SQLSRVSTATEMENT_H__
#define __SQLSRVSTATEMENT_H__

#include <e32base.h>
#include <s32mem.h>					//TBufBuf
#include "SqlUtil.h"				//Sql2OsErrCode()
#include "SqlBufFlat.h"				//RSqlBufFlat
#include "SqlSrvStatementUtil.h"	//Global SQL statement related functions

//Forward declarations
struct sqlite3;
struct sqlite3_stmt;
class RWriteStream;
class CSqlSrvStatement;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////                HSqlSrvStmtParamBuf              //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Buffer for storing binary/text parameter values (an input from client side write stream objects or bound text/binary parameters).
Every CSqlSrvStatement object maintains a collection of HSqlSrvStmtParamBuf objects.

Please, note that:

 - The life-time of the HSqlSrvStmtParamBuf objects is controlled by a set of flags:
	 = iStatementFinalized - False by default, set to true if the bound statement has been finalized.
	                         When DoRelease() is called, the method checks this poreperty and if the statement has been finalized,
	                         then obviously no more parameter binding operations can be performed and it is safe to destroy the 
	                         current HSqlSrvStmtParamBuf object. Otherwise the HSqlSrvStmtParamBuf object won't be destroyed, 
	                         it will be marked as "dead" - see the "iAlive" property;
	 = iAlive			   - True by default, set to false by the DoRelease() call, if the object cannot be destroyed because 
	 					     the bound statement is not finalized yet;
	 = iBufType		       - EBufIpcStream if the current HSqlSrvStmtParamBuf object is an IPC stream buffer, EBufSimpleBind 
	 						 if the  object holds a parameter value, that has been bound not using an IPC stream. This property 
	 						 is checked when the bound statement gets finalized - inside the NotifyStatementFinalized() function. 
	 						 If the current object is not an IPC stream buffer, then it is safe to destroy the object because
	 						 the bound statement is finalized;
   
   All these checks allow keeping the buffer object alive when it is needed and code like the code bellow should work without problems:
@code
	RSqlStatement stmt;			//Prepare a statement with a binary/text parameter
	....
	RSqlParamWriteStream strm;
	strm.BindBinary(stmt,...);	//Use a stream object to bind the parameter value
	strm.WriteL(...);
	strm.CommitL();				//The stream commit call will bind the streamed parameter value to the statement
	strm.Close();				//The stream close operation should not destroy the HSqlSrvStmtParamBuf object, because the parameter
								//value is kept in the buffer, and when that value was bound to the statement, the
								//SQLITE_STATIC constant was used, which means - SQLite won't make a copy of the parameter value,
								//SQLite will keep a pointer to the parameter value and use it for the Exec() operation.
	stmt.Exec();				//At this point the SQLite library should be able to get the parameter data and use it.
@endcode

 - Another reason for keeping the HSqlSrvStmtParamBuf object alive during the life-time of the CSqlSrvStatement object is that
   the RSqlStatement::Reset() call should not destroy the buffer. The client should be able to resuse
   the buffered parameter data for the next RSqlStatement::Exec() call. For example:
@code
	RSqlStatement stmt;			//Prepare a statement with two  binary parameters
	....
	RSqlParamWriteStream strm1, strm2;
	
	strm1.BindBinary(stmt,...);	//Use a stream object to bind the 1st parameter value
	strm1.WriteL(...);
	strm1.CommitL();			
	strm1.Close();				
	
	strm2.BindBinary(stmt,...);	//Use a stream object to bind the 2nd parameter value
	strm2.WriteL(...);
	strm2.CommitL();			
	strm2.Close();				
	
	stmt.Exec();				//Do the operation using the bound parameter values
	stmt.Reset();				//This call should not destroy the HSqlSrvStmtParamBuf objects that keep the parameter values

	strm2.BindBinary(stmt,...);	//Use a stream object to bind the 2nd parameter value
	strm2.WriteL(...);
	strm2.CommitL();			
	strm2.Close();				

	stmt.Exec();				//Do the operation using the bound parameter values. 
								//The operation should be able to use the old 1st parameter value without any problems,
								//and also the new 2nd parameter value.
@endcode
								
 - The HSqlSrvStmtParamBuf object can be synch-ed just once. The reason for that is to prevent the double commit-s, 
   that will occur as a result of MStreamBuf::Close() calls on the client side, where the Close() method will try to sync the buffer.
   The single sync will prevent the SQL server from crashing, if the order of the RSqlStatement::Close() and
   RSqlParamWriteStream::Close() calls is reversed, e.g.:
@code
	RSqlStatement stmt;		
	....
	RSqlParamWriteStream strm;
	strm.BindBinary(stmt,...);
	strm.WriteL(...);
	strm.CommitL();		
	stmt.Exec();
	stmt.Close();
	strm.Close();		
@endcode
   
 - For BC reasons, even though the HSqlSrvStmtParamBuf object can be synch-ed just once (the operation is controlled by the iSynchDone
   flag), the client side still will be able to update the buffer content, because the related HIpcStream stream object keeps a copy
   of the buffer's base address. Since the parameter is bound using the SQLITE_STATIC constant (SQLite won't make a copy of the 
   parameter value), at the moment of the Exec() call, SQLite will use the give pointer to the parameter value and will pick up
   the updated buffer even without a commit call;
 								
@endcode
 
 - The iSynchDone flag controls the number of buffer commit operations. It is false by default, 
   set to true by the DoSynchL() call. The HSqlSrvStmtParamBuf object can be synch-ed just once, because DoSynchL() 
   calls sqlite3_bind_text/binary() and the expectation is that the whole buffer with the parameter data is ready for binding;
   If the buffer is not commited by calling DoSynchL(), the buffer still will be commited by the DoSynchL() call performed from
   MStreamBuf::Close(). But in a case of a sync failure, no error will be reported to the client side;
 
@see HIpcStream 
@see CSqlSrvStatement
 
@internalComponent
*/
NONSHARABLE_CLASS(HSqlSrvStmtParamBuf) : public TBufBuf
	{
public:
	enum TDataType {EBinary, EText16};
	enum TBufType {EBufSimpleBind, EBufIpcStream};
	
	static inline HSqlSrvStmtParamBuf* NewL(CSqlSrvStatement& aStatement, TInt aParamIndex, HSqlSrvStmtParamBuf::TDataType aDataType, 
										    HSqlSrvStmtParamBuf::TBufType aBufType);
	inline void Reset(HSqlSrvStmtParamBuf::TDataType aDataType, HSqlSrvStmtParamBuf::TBufType aBufType);
	
	inline const TPtrC8 SetDataL(const TDesC8& aData);
	inline const TPtrC8 Data() const;
	inline HSqlSrvStmtParamBuf::TDataType DataType() const;
	
	void NotifyStatementFinalized();
	inline TInt ParamIndex() const;
	
private:
	inline HSqlSrvStmtParamBuf(CSqlSrvStatement& aStatement, TInt aParamIndex, HSqlSrvStmtParamBuf::TDataType aDataType, 
							   HSqlSrvStmtParamBuf::TBufType aBufType);
	virtual ~HSqlSrvStmtParamBuf();
	inline void ConstructL();
	virtual void DoSynchL();
	virtual void DoRelease();
	
private:
	enum {EExpandSize = 512};
	
	CSqlSrvStatement&	iStatement;			//The bound CSqlSrvStatement object
	CBufFlat* 			iBuf;				//Parameter buffer - not owned by HSqlSrvStmtParamBuf
	TInt 				iParamIndex;		//The parameter index
	TBool				iStatementFinalized;//True if the bound statement object has been finalized
	TBool				iAlive;				//True if DoRelease() has not been called yet
	HSqlSrvStmtParamBuf::TDataType iDataType;//The parameter type
	HSqlSrvStmtParamBuf::TBufType iBufType;	//IPC stream buf or a simple "bind param" buf
	TBool				iSynchDone;			//True if the buffer data has been bound to the statement
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////                CSqlSrvStatement                	   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQL statement handle.

CSqlSrvStatement is a server side class which processes the client side requests for:
 - preparing 16-bit and 8-bit SQL statements;
 - executing prepared SQL statement;
 - retrieving parameter names and column names;
 - binding SQL parameters;
 - retrieving column values;
 - reseting the prepared SQL statement;
 - moving the cursor to the next record;

@see CSqlSrvStatement::NewLC()
@see CSqlSrvStatement::NewLC()
@see CSqlSrvStatement::BindL()
@see CSqlSrvStatement::Next()
@see CSqlSrvStatement::Reset()
@see CSqlSrvStatement::Exec()
@see CSqlSrvStatement::ColumnNamesL()
@see CSqlSrvStatement::ParamNamesL()
@see CSqlSrvStatement::ColumnValuesL()
@see CSqlSrvStatement::ColumnSource()

@see HSqlSrvStmtParamBuf

@internalComponent
*/
NONSHARABLE_CLASS(CSqlSrvStatement) : public CBase
	{
public:
	static CSqlSrvStatement* NewLC(sqlite3* aDbHandle, const TDesC16& aSqlStmt, TInt& aColumnCount, TInt& aParamCount);
	static CSqlSrvStatement* NewLC(sqlite3* aDbHandle, const TDesC8& aSqlStmt, TInt& aColumnCount, TInt& aParamCount);
	virtual ~CSqlSrvStatement();

	void BindL(const RSqlBufFlat& aParamBuf);	
	inline TInt Next();
	inline TInt Reset();
	inline TInt Exec();
	
	inline const RSqlBufFlat& BufFlatL(TSqlBufFlatType aWhat) const;
	const RSqlBufFlat& ColumnNamesL();
	const RSqlBufFlat& ParamNamesL();
	const RSqlBufFlat& ColumnValuesL();
	TInt ColumnSource(TInt aColumnIndex, TPtrC8& aColumnSource) const;
	
	TInt ColumnInt(TInt aColIdx) const;
	TInt64 ColumnInt64(TInt aColIdx) const;
	TReal ColumnReal(TInt aColIdx) const;
	TPtrC ColumnTextL(TInt aColIdx) const;
	TPtrC8 ColumnBinary(TInt aColIdx) const;
	
	const RSqlBufFlat& GetDeclColumnTypesL();
	
	HSqlSrvStmtParamBuf* GetParamBufL(TInt aParamIndex, HSqlSrvStmtParamBuf::TDataType aDataType, HSqlSrvStmtParamBuf::TBufType aBufType);
	void BindParamBufL(TInt aParamIndex);
		
private:
	inline CSqlSrvStatement();
	inline void ConstructL(sqlite3* aDbHandle, const TDesC16& aSqlStmt);
	inline void ConstructL(sqlite3* aDbHandle, const TDesC8& aSqlStmt);
	void DoCommonConstructL();
	void DestroyParamBufArray();
	void ExtendParamBufArrayL(TInt aParamIndex);
	TPtrC8 CopyAndStoreParamL(TInt aParamIndex, HSqlSrvStmtParamBuf::TDataType aDataType, const TDesC8& aParamValue);

private:
	sqlite3_stmt*		iStmtHandle;		//SQL statement handle
	TInt 				iColumnCount;		
	TInt 				iParamCount;		
	TSqlBufFlatType		iBufFlatType;		//What is in iFlatBuf data member
	RSqlBufFlat  		iBufFlat;			//Flat buffer used for: column names, parameter names, column values
	RArray<HSqlSrvStmtParamBuf*> iParamBufArray;//An array with pointers to HSqlSrvStmtParamBuf buffers for the text/binary parameters

	};

#include "SqlSrvStatement.inl"

#endif//__SQLSRVSTATEMENT_H__
