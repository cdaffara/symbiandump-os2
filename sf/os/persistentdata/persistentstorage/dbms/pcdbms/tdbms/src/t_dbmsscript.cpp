// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <d32dbms.h>
#include <f32file.h>
#include <e32test.h>
#include <e32math.h>

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_DBMSSCRIPT.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_DBMSSCRIPT.CRC");
#endif
#endif

LOCAL_D RTest test(_L("T_DBMSSCRIPT"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RFs TheFs;
LOCAL_D RDbView TheView;
//

#ifdef __TOOLS2__
const TPtrC KTestDatabase=_L(".\\dbms-tst\\t_script.db");
#else
const TPtrC KTestDatabase=_L("C:\\dbms-tst\\t_script.db");
#endif

const TPtrC KRomScriptFile=_L("z:\\test\\t_script.txt");

#ifndef __TOOLS2__
const TPtrC KScriptFile=_L("c:\\dbms-tst\\t_script.txt");
#else
const TPtrC KScriptFile=_L(".\\dbms-tst\\t_script.txt");
#endif

#ifndef __TOOLS2__
const TPtrC KOutputFile=_L("c:\\dbms-tst\\t_script.log");
#else
const TPtrC KOutputFile=_L(".\\dbms-tst\\t_script.log");
#endif

const TInt KTestCleanupStack=0x20;
const TPtrC KDDLKeywords[]={_L("CREATE"),_L("DROP"),_L("ALTER")};
const TPtrC KDMLKeywords[]={_L("INSERT"),_L("DELETE"),_L("UPDATE")};
const TPtrC KQueryKeywords[]={_L("SELECT")};
const TPtrC KScriptKeywords[]={_L("PRINT"),_L("ROWS"),_L("COMPARE"),_L("ERROR"),_L("!"),
							   _L("POPULATE"),_L("RESULTS"),_L("BUILD"),_L("QUERY"),
							   _L("NORMAL"),_L("FOLDED"),_L("COLLATED"),_L("START"),_L("STOP"),
							   _L("LOAD"),_L("ECHO"),_L("WINDOW"),_L("ACCESS")};
const TPtrC KRowIdColName=_L("Rw");
enum TKeyword {EPrint,ERows,ECompare,EError,EComment,EPopulate,EResults,EBuild,EQuery,ENormal,
               EFolded,ECollated,EStart,EStop,ELoad,EEcho,EWindow,EAccess,ENumKeywords,EUnknown};
//
typedef TBuf<256> TScriptLine;
typedef TBuf<256> TScriptToken;
//
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

class TScript;
class TTimer
	{
public:
	inline TTimer(TScript& aScript);
	void Start(const TDesC& aDes);
	void Stop(const TDesC& aDes);
private:
	TUint iTicks;
	TScript& iScript;
	};

class TResults
	{
public:
	inline TResults(); 
public:
	RDbView& iView;
	TInt iError;
	TInt iRows;
	TInt iLineNo;
	TScriptLine iLine;
	};

class TSqlStatement
	{
private:
	enum TSqlType {EDML,EDDL,EQuery,EUnknown};
public:
	inline TSqlStatement(const TDesC& aSql,TResults& aResults); 
	TInt Execute(TDbTextComparison aTextComparison=EDbCompareNormal,
		RDbRowSet::TAccess aAccess=RDbRowSet::EReadOnly,TBool aWindow=EFalse) const;
private:
	void ExecuteSql(TDbTextComparison aTextComparison) const;
	void ExecuteQuery(RDbRowSet::TAccess aAccess,TBool aWindow) const;
	TSqlType SqlType() const;
private:
	const TDesC& iSql;
	TResults& iResults;
	};

class TScript
	{
public:
	TScript();
#ifdef __TOOLS2__
	~TScript() { iFile.Close(); };
#endif
	TInt ReadNextStatement();
	void GetNextTokenFromStatement(TDes& aToken);
	void GetNextTokenFromLine(TDes& aToken);
	TPtrC Statement();
	TInt IntValue();
	void WriteLine(const TDesC& aLine);
	void WriteError(const TDesC& aLine);
	void WriteSqlError(const TResults& aResults,const TDesC& aLine); 
	void WriteComment(const TDesC& aLine);
	TKeyword Keyword(const TDesC& aKeyword) const;
	void ConsumeLine();
	void ConsumeStatement();
	inline TInt LineNo() const;
private:
	TInt ReadNextLine();
	TInt AppendNextLine();
	TBool IsStatement();
private:
	TFileText iInput;
	TFileText iOutput;
	TScriptLine iBuf;
	TLex iStatement;
	TLex iLine;
	TInt iLineNo;
#ifdef __TOOLS2__
	RFile iFile;
#endif
	};

class TScriptEngine
	{
public:
	inline TScriptEngine();
	void RunL();
private:
	void ExecuteL();
	TInt ExecuteScriptL();
	TInt ExecuteSql(const TDesC& aSql);
	TInt ExecuteSql();
	// keyword operations
	void DoPrintL();
	void DoComment();
	void DoError();
	void DoEcho();
	void DoWindow();
	void DoAccess();
	void DoRows();
	void DoCompareL();
	void DoPopulate();
	void DoResultsL();
	void DoBuildTable();
	void DoQuery();
	void DoTextComparison(TKeyword aKeyword);
	void DoStartTimer();
	void DoStopTimer();
	void DoLoadDb();
	//
	void PrintL(RDbRowSet& aRowSet);
	void CompareL(RDbRowSet& aRowSet);
	void CompareValues(RDbRowSet& aRowSet,TDbColNo ColNo,TDbColType aType,const TDesC& aToken);
	void FatalError(const TDesC& aLine);
	void FatalError();
	void FatalSqlError(const TDesC& aLine);
	void TestForNoError(); 
private:
	TScript iScript;
	TResults iResults;
	TDbTextComparison iTextComparison;
	TTimer iTimer;
	TBool iEcho;
	TBool iWindow;
	RDbRowSet::TAccess iAccess;
	
	};

//
// class TTimer
//

inline TTimer::TTimer(TScript& aScript)
	: iScript(aScript)
	{}

void TTimer::Start(const TDesC& aDes)
	{
	TScriptLine line;
	line.Format(_L("%S: "),&aDes);
	iScript.WriteLine(line);
	iTicks=User::TickCount();
	}

void TTimer::Stop(const TDesC& aDes)
	{
	TScriptLine line;
	line.Format(_L("%S: "),&aDes);
#ifdef __EPOC32__
#define TICK_TIME 15625
#else
#define	TICK_TIME 100000
#endif
	TInt microSec=(User::TickCount()-iTicks)*TICK_TIME;
	TUint sec=microSec/1000000;
	TUint centi=(microSec/10000)-sec*100;
	line.AppendFormat(_L("%u.%02us\n"),sec,centi);
	iScript.WriteLine(line);
	}

//
// class TResults
//

inline TResults::TResults()
	: iView(TheView),iError(KErrNone)
	{}


//
// class TSqlStatement
//

inline TSqlStatement::TSqlStatement(const TDesC& aSql,TResults& aResults)
	: iSql(aSql),iResults(aResults)
	{}

void TSqlStatement::ExecuteSql(TDbTextComparison aTextComparison) const
//
// executes DML or DDL
//
	{
	TInt r=TheDatabase.Execute(iSql,aTextComparison);
	if (r<0)
		iResults.iError=r;
	else
		{
		iResults.iError=KErrNone;
		iResults.iRows=r;
		}
	}

void TSqlStatement::ExecuteQuery(RDbRowSet::TAccess aAccess,TBool aWindow) const
	{
	iResults.iView.Close();			// discard any previous queries
	TInt& err=iResults.iError;
	if (aWindow)
		err=iResults.iView.Prepare(TheDatabase,iSql,KDbUnlimitedWindow,aAccess);
	else
		err=iResults.iView.Prepare(TheDatabase,iSql,aAccess);
	if (err==KErrNone)
		err=iResults.iView.EvaluateAll();
	}

TSqlStatement::TSqlType TSqlStatement::SqlType() const
//
// determines the type of sql statement by matching keywords
//
	{
	for (TUint i=0;i<ARRAY_SIZE(KDDLKeywords);++i)
		{
		if (iSql.FindF(KDDLKeywords[i])==0)		
			return EDDL;
		}
	for (TUint ii=0;ii<ARRAY_SIZE(KDMLKeywords);++ii)
		{
		if (iSql.FindF(KDMLKeywords[ii])==0)
			return EDML;
		}
	for (TUint j=0;j<ARRAY_SIZE(KQueryKeywords);++j)
		{
		if (iSql.FindF(KQueryKeywords[j])==0)
			return EQuery;
		}
	return EUnknown;
	}

TInt TSqlStatement::Execute(TDbTextComparison aTextComparison,RDbRowSet::TAccess aAccess,TBool aWindow) const
//
// executes the sql statement
//
	{
	TInt r=KErrNone;
	switch (SqlType())
		{
	case EDDL:
	case EDML:
		ExecuteSql(aTextComparison);
		break;
	case EQuery:
		ExecuteQuery(aAccess,aWindow);
		break;
	case EUnknown:
	default:
		r=KErrNotFound;
		break;
		}
	return r;
	}


//
// class TScript
//

TScript::TScript()
	: iLineNo(0)
	{
	RFile file;
	TInt r=file.Open(TheFs,KScriptFile,EFileRead);
#ifndef __TOOLS2__
	if (r!=KErrNone)
		r=file.Open(TheFs,KRomScriptFile,EFileRead);
#endif
	test(r==KErrNone);
	iInput.Set(file);

#ifdef __TOOLS2__
	RFile file2;
	test(file2.Replace(TheFs,KOutputFile,EFileWrite)==KErrNone);
	iFile = file2;
	iOutput.Set(file2);
#else
	test(file.Replace(TheFs,KOutputFile,EFileWrite)==KErrNone);
	iOutput.Set(file);
#endif
	}

inline TInt TScript::LineNo() const
	{return iLineNo;}

TBool TScript::IsStatement()
//
//	checks for keywords which possibly conform to the usual statement format (ie end in ';')
//
	{
	if (iLine.Remainder().Length()==0)	// null statement
		return ETrue;
	TPtrC line=iLine.Remainder();
	TKeyword keyword=Keyword(line);
	switch (keyword)
		{
	case EPrint: 
	case ERows:
	case EBuild:
	case EError:
	case EQuery:
	case ENormal:
	case EFolded:
	case ECollated:
	case EStart:
	case EStop:
	case ELoad:
	case EEcho:
	case EWindow:
	case EAccess:
	case EUnknown:						// could be sql
		return ETrue;
	case EComment:
	case EPopulate:
	case ECompare:
	case EResults:
		iStatement=line;				// not a statement, so make it the whole line
		return EFalse;
	default:
		test(0);
		return EFalse;
		}
	}

TInt TScript::ReadNextStatement()
//
// reads the next non-blank statement or line
//
	{
	TInt r=ReadNextLine();
	if (r!=KErrNone || !IsStatement())
		return r;
	TChar c=0;
	while (c!=';')
		{
		c=iLine.Get();
		if (!c)							// nothing left to read
			{
			r=AppendNextLine();
			if (r!=KErrNone)
				return r;
			}
		}
	iLine.UnGet();						// the semi-colon
	iStatement=iLine.MarkedToken();
	iLine.Get();						// the semi-colon
	iLine.SkipSpaceAndMark();
	return KErrNone;
	}

TPtrC TScript::Statement()
	{
	return iStatement.Remainder();
	}

void TScript::ConsumeLine()
	{
	iLine=TPtrC();
	iStatement=TPtrC();
	}

void TScript::ConsumeStatement()
	{
	iLine=iLine.Remainder();
	iStatement=TPtrC();
	}

TInt TScript::ReadNextLine()
//
// reads the next non-blank line into iLine
//
	{
	while (iLine.Remainder().Length()==0)
		{
		TInt r=iInput.Read(iBuf);
		if (r!=KErrNone)
			return r;
		if (iBuf.Length()>0 && iBuf[0]==0xfeff)	// unicode stream marker
			iBuf.Delete(0,1);
		iLineNo++;
		iLine=iBuf;
		iLine.SkipSpaceAndMark();
		} 
	return KErrNone;
	}

TInt TScript::AppendNextLine()
//
// adds next line from file to iLine
//
	{
	TScriptLine line;
	do {
		TInt r=iInput.Read(line);
		if (r!=KErrNone)
			return r;
		iLineNo++;
		} while (line.Length()==0);
	iBuf=iLine.MarkedToken();
	iBuf.Append(line);
	iLine=iBuf;
	return KErrNone;
	}

void TScript::WriteError(const TDesC& aLine)
	{
	TScriptLine line;
	line.Format(_L("Error at line %d: %S\n"),iLineNo,&aLine);
	WriteLine(line);
	test.Printf(line);
	}

void TScript::WriteSqlError(const TResults& aResults,const TDesC& aLine)
	{
	TScriptLine line;
	line.Format(_L("Error at line %d: %S :-\n"),aResults.iLineNo,&aLine);
	WriteLine(line);
	test.Printf(line);
	line.Format(_L("\t%S\n"),&aResults.iLine);
	WriteLine(line);
	test.Printf(line);
	}

void TScript::WriteLine(const TDesC& aLine)
	{
	TScriptLine l=aLine;
	l.Append('\r');
	iOutput.Write(l);
	}

void TScript::WriteComment(const TDesC& aLine)
	{
	TScriptLine line;	
	line.Format(_L("\n%S"),&aLine);
	WriteLine(line);
	}

TInt TScript::IntValue()
//
// returns the integer n from the ' = n ' which must follow in the statement
//
	{
	TScriptToken keyword;
	GetNextTokenFromStatement(keyword);
	if (keyword.Compare(_L("="))!=0)
		WriteError(_L("expected '=' missing"));
	iStatement.SkipSpaceAndMark();
	TInt num=0;
	TInt err=iStatement.Val(num);
	if (err!=KErrNone)
		WriteError(_L("expected number missing"));
	return num;
	}

TKeyword TScript::Keyword(const TDesC& aKeyword) const
	{
	for (TInt ii=0; ii<ENumKeywords; ++ii)
		{
		if (aKeyword.FindF(KScriptKeywords[ii])==0)
			return TKeyword(ii);
		}
	return EUnknown;
	}

void TScript::GetNextTokenFromStatement(TDes& aToken)
//
// gets the next token from iStatement
//
	{
	iStatement.SkipSpaceAndMark();
	TUint c;
	do
		{
		c=iStatement.Get();
		if (c=='=' || c=='!')
			break;
		} while (iStatement.Peek().IsAlphaDigit());
	aToken=iStatement.MarkedToken();
	iStatement.SkipSpaceAndMark();
	}

void TScript::GetNextTokenFromLine(TDes& aToken)
//
// gets the next token from iLine
//
	{
	iLine.SkipSpaceAndMark();
	TUint c=0;
	TChar cc=c;
	TBool literal=EFalse;
	do
		{
		c=iLine.Get();
		if (!c)
			{
			AppendNextLine();
			iLine.SkipSpaceAndMark();
			c=iLine.Get();
			}
		if (c=='\'' || c=='#')
			literal=!literal;
		if ((c==',' || c=='(' || c==')' || c=='{' || c=='}' || c=='!' ) && !literal)
			break;
		cc=iLine.Peek();
		} while (cc.IsAlphaDigit() || literal || TUint(cc)=='.' || TUint(cc)=='+' || TUint(cc)=='-');
	aToken=iLine.MarkedToken();
	if (TUint(cc)==';')					// ignore semi-colons - they're optional
		iLine.Get();
	iLine.SkipSpaceAndMark();
	if (c=='!')
		{
		iStatement=iLine.Remainder();
		TPtrC comment=iStatement.Remainder();
		WriteComment(comment); 
		iLine=TPtrC();
		GetNextTokenFromLine(aToken);
		}
	}


//
// class TScriptEngine
//

TScriptEngine::TScriptEngine()
	: iTextComparison(EDbCompareNormal),iTimer(iScript),iEcho(EFalse),iWindow(EFalse),iAccess(RDbRowSet::EReadOnly)
	{} 


void TScriptEngine::RunL() 
//
// runs the script file
//
	{
	while (iScript.ReadNextStatement()!=KErrEof)
		ExecuteL();
	TestForNoError(); 
	}

void TScriptEngine::TestForNoError() 
	{
	if (iResults.iError!=KErrNone)
		{
		TScriptLine line;
		line.Format(_L("unexpected error %d"),iResults.iError);
		FatalSqlError(line);
		}
	}

void TScriptEngine::ExecuteL()
	{
	if (ExecuteScriptL()!=KErrNone && ExecuteSql()!=KErrNone)
		FatalSqlError(_L("syntax error"));
	}

TInt TScriptEngine::ExecuteScriptL()
	{
	TKeyword keyword=iScript.Keyword(iScript.Statement());
	if (keyword!=EError)
		TestForNoError(); 
	switch (keyword)
		{
	case EPrint:	
		DoPrintL();
		break;
	case ERows:
		DoRows();
		break;
	case ELoad:
		DoLoadDb();
		break;
	case EEcho:
		DoEcho();
		break;
	case EWindow:
		DoWindow();
		break;
	case EAccess:
		DoAccess();
		break;
	case ECompare:
		DoCompareL();
		break;
	case EError:
		DoError();
		break;
	case EPopulate:
		DoPopulate();
		break;
	case EComment:
		DoComment();
		break;
	case EResults:
		DoResultsL();
		break;
	case EBuild:
		DoBuildTable();
		break;
	case EQuery:
		DoQuery();
		break;
	case ENormal:
	case EFolded:
	case ECollated:
		DoTextComparison(keyword);
		break;
	case EStart:
		DoStartTimer();
		break;
	case EStop:
		DoStopTimer();
		break;
	case EUnknown:
	default:
		return KErrNotFound;
		}
	return KErrNone;
	}

TInt TScriptEngine::ExecuteSql()
	{
	return ExecuteSql(iScript.Statement());
	}

TInt TScriptEngine::ExecuteSql(const TDesC& aSql)
	{
	if (iEcho)
		{
		TScriptLine line(_L("\nSQL:\t"));
		line.Append(aSql);
		iScript.WriteLine(line);
		}
	iResults.iLineNo=iScript.LineNo();
	iResults.iLine=aSql;
	TSqlStatement statement(aSql,iResults);
	return statement.Execute(iTextComparison,iAccess,iWindow);
	}

void TScriptEngine::DoStartTimer()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("START"))==0);
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("TIMER"))==0);
	//
	iTimer.Start(_L("Timer started"));
	}

void TScriptEngine::DoStopTimer()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("STOP"))==0);
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("TIMER"))==0);
	//
	iTimer.Stop(_L("Timer stopped"));
	}

void TScriptEngine::DoTextComparison(TKeyword aKeyword)
	{
	TPtrC line;
	switch (aKeyword)
		{
	case ENormal:
		iTextComparison=EDbCompareNormal;
		line.Set(_L("[Normal text comparison]"));
		break;
	case EFolded:
		iTextComparison=EDbCompareFolded;
		line.Set(_L("[Folded text comparison]"));
		break;
	case ECollated:
		iTextComparison=EDbCompareCollated;
		line.Set(_L("[Collated text comparison]"));
		break;
	default:
		test(0);
		}
	iScript.WriteLine(line);
	}

void TScriptEngine::DoComment()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("!"))==0);
	//
	TPtrC comment=iScript.Statement();
	iScript.WriteComment(comment);
	iScript.ConsumeLine();
	}

void TScriptEngine::DoError()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("ERROR"))==0);
	//
	TScriptLine line;
	if (iScript.Statement().Length()==0)
		{
		if (iResults.iError>=0)
			FatalSqlError(_L("no error when one was expected"));
		line=_L("\t\tERROR OK");
		}
	else
		{
		TInt err=iScript.IntValue();
		if (iResults.iError!=err)
			{
			line.Format(_L("expected error %d, actual error %d"),err,iResults.iError);
			FatalSqlError(line);
			}
		line.Format(_L("\t\tERROR=%D OK"),err);
		}
	iResults.iError=0;
	iScript.WriteLine(line);
	}

void TScriptEngine::DoRows()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("ROWS"))==0);
	//
	TScriptLine line;
	TInt rows=iScript.IntValue();
	if (iResults.iRows!=rows)
		{
		line.Format(_L("expected rows %d, actual rows %d"),rows,iResults.iRows);
		FatalSqlError(line);
		}
	line.Format(_L("\t\tROWS=%D OK"),rows);
	iScript.WriteLine(line);
	}

void TScriptEngine::DoLoadDb()
	{
	// test its right function
	TScriptToken token;
	iScript.GetNextTokenFromStatement(token);
	test(token.CompareF(_L("LOAD"))==0);
	//
	TFileName database(iScript.Statement());
	TheDatabase.Close();

	TInt err = TheCrcChecker.GenerateCrcL(KTestDatabase);
	test(err==KErrNone);

	TScriptLine line;
	line.Format(_L("Opening database: %S"),&database);
	iScript.WriteLine(line);
	test(TheDatabase.Open(TheFs,database)==KErrNone);
	}

void TScriptEngine::DoEcho()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("ECHO"))==0);
	//
	iScript.GetNextTokenFromStatement(keyword);
	if (keyword.CompareF(_L("OFF"))==0)
		{
		iEcho=EFalse;
		iScript.WriteLine(_L("Echo is off"));
		}
	else if (keyword.CompareF(_L("ON"))==0)
		{
		iEcho=ETrue;
		iScript.WriteLine(_L("Echo is on"));
		}
	else
		FatalError(_L("Expected ON|OFF to follow ECHO statement"));
	}

void TScriptEngine::DoWindow()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("WINDOW"))==0);
	//
	iScript.GetNextTokenFromStatement(keyword);
	if (keyword.CompareF(_L("OFF"))==0)
		{
		iWindow=EFalse;
		iScript.WriteLine(_L("Window is off"));
		}
	else if (keyword.CompareF(_L("ON"))==0)
		{
		iWindow=ETrue;
		iScript.WriteLine(_L("Window is on"));
		}
	else
		FatalError(_L("Expected ON|OFF to follow WINDOW statement"));
	}

void TScriptEngine::DoAccess()
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("ACCESS"))==0);
	//
	iScript.GetNextTokenFromStatement(keyword);
	if (keyword.CompareF(_L("UPDATE"))==0)
		{
		iAccess=RDbRowSet::EUpdatable;
		iScript.WriteLine(_L("Access is updateable"));
		}
	else if (keyword.CompareF(_L("READ"))==0)
		{
		iAccess=RDbRowSet::EReadOnly;
		iScript.WriteLine(_L("Access is read only"));
		}
	else if (keyword.CompareF(_L("INSERT"))==0)
		{
		iAccess=RDbRowSet::EInsertOnly;
		iScript.WriteLine(_L("Access is insert only"));
		}
	else
		FatalError(_L("Expected UPDATE|INSERT|READ to follow ACCESS statement"));
	}

void TScriptEngine::DoResultsL()
	{
	// test its right function
	TScriptToken token;
	iScript.GetNextTokenFromLine(token);
	test(token.CompareF(_L("RESULTS"))==0);
	//
	iScript.GetNextTokenFromLine(token);
	if (token.Compare(_L("{"))!=0)		
		FatalError(_L("missing '{'"));
	iScript.GetNextTokenFromLine(token);					// first value
	TLex value;
	RDbRowSet& rowset=iResults.iView;
	CDbColSet* colset=rowset.ColSetL();
	CleanupStack::PushL(colset);
	TDbColNo colno=colset->ColNo(KRowIdColName);
	CArrayFixFlat<TInt>* rowIdScript=new CArrayFixFlat<TInt>(4);
	CleanupStack::PushL(rowIdScript);
	CArrayFixFlat<TInt>* rowIdView=new CArrayFixFlat<TInt>(4);
	CleanupStack::PushL(rowIdView);
	rowset.BeginningL();
	while (rowset.NextL())				
		{
		rowset.GetL();
		TUint rIdScript;
		value=token;
		if (value.Val(rIdScript)!=KErrNone)
			{
			TScriptLine line;
			line.Format(_L("Unable to extract row id from \"%S\""),&token);
			FatalError(line);
			}
		TUint rIdView=rowset.ColUint(colno);
		rowIdScript->AppendL(rIdScript);
		rowIdView->AppendL(rIdView);
		iScript.GetNextTokenFromLine(token);			
		if (token.Compare(_L(","))==0 || token.Compare(_L("}"))==0)
			{
			if (rowIdScript->Count())
				{
				TKeyArrayFix key(0,ECmpTInt);
				rowIdScript->Sort(key);
				rowIdView->Sort(key);
				for (TInt ii=0;ii<rowIdScript->Count();++ii)
					{
					TInt expectedId=(*rowIdScript)[ii];
					TInt actualId=(*rowIdView)[ii];
					if (actualId!=expectedId)
						{
						TScriptLine line;
						line.Format(_L("expected row id %d, actual row id %d"),actualId,expectedId);
						FatalError(line);
						}
					}
				rowIdScript->Reset();
				rowIdView->Reset();
				}
			if (token.Compare(_L(","))==0)
				iScript.GetNextTokenFromLine(token);
			}
		}
	if (token.Compare(_L("}"))!=0)
		FatalError(_L("too many results expected"));
	CleanupStack::PopAndDestroy(3);	// arrays + colset
	iScript.ConsumeStatement();
	}

void TScriptEngine::DoBuildTable()
//
// same as Sql create statement, but adds a counter
//
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("BUILD"))==0);
	//
	TScriptLine sql(_L("CREATE "));
	sql.Append(iScript.Statement());
	TInt pos=sql.Find(_L("("));
	sql.Insert(++pos,_L("Rw COUNTER,"));
	iScript.ConsumeStatement();
	ExecuteSql(sql);
	}

void TScriptEngine::DoQuery()
//
// same as Sql select statement, but makes sure counter is included
//
	{
	// test its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("QUERY"))==0);
	//
	TScriptLine sql(iScript.Statement());
	if (sql.Find(_L("*"))!=0)
		{
		sql.Insert(0,_L(","));
		sql.Insert(0,KRowIdColName);
		}
	sql.Insert(0,_L("SELECT "));
	iScript.ConsumeStatement();
	ExecuteSql(sql);
	}

void TScriptEngine::FatalError(const TDesC& aLine)
	{
	iScript.WriteError(aLine);
	test(0);
	}

void TScriptEngine::FatalError()
	{
	FatalError(_L("wrong expected value"));
	}

void TScriptEngine::FatalSqlError(const TDesC& aLine)
	{
	iScript.WriteSqlError(iResults,aLine);
	test(0);
	}

void TScriptEngine::DoPopulate()
	{
	// check its right function
	TScriptToken token;
	iScript.GetNextTokenFromLine(token);
	test(token.CompareF(_L("POPULATE"))==0);
	//
	TScriptLine sqlbase=_L("INSERT INTO ");
	iScript.GetNextTokenFromLine(token);	// table name
	sqlbase.AppendFormat(_L("%S "),&token);	
	iScript.GetNextTokenFromLine(token);
	if (token.Compare(_L("("))==0)			// optional column names present?
		{			
		for (;;) 							
			{
			sqlbase.AppendFormat(token);
			if (token.Compare(_L(")"))==0)
				break;
			iScript.GetNextTokenFromLine(token);		
			} 
		iScript.GetNextTokenFromLine(token);
		}
	if (token.Compare(_L("{"))!=0)		
		FatalError(_L("missing '{'"));
	sqlbase.AppendFormat(_L(" VALUES ("));
	iScript.GetNextTokenFromLine(token);	// first value
	TheDatabase.Begin();					// all in same transaction
	for (;;)
		{
		if (token.Compare(_L("}"))==0)	
			break;
		TScriptLine sql=sqlbase;
		for (;;)
			{
			sql.Append(token);
			iScript.GetNextTokenFromLine(token);
			if (token.Compare(_L(","))==0)
				{
				sql.Append(token);			// comma
				iScript.GetNextTokenFromLine(token);
				}
			else
				break;
			}
		sql.AppendFormat(_L(")"));
		ExecuteSql(sql);
		TestForNoError();
		}
	TheDatabase.Commit();
	iScript.ConsumeStatement();
	}

void TScriptEngine::DoPrintL() 
	{
	// check its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromStatement(keyword);
	test(keyword.CompareF(_L("PRINT"))==0);
	//
	iScript.GetNextTokenFromStatement(keyword);
	if (keyword.CompareF(_L("VIEW"))==0)
		PrintL(iResults.iView);
	else if (keyword.CompareF(_L("TABLE"))==0)
		{
		iScript.GetNextTokenFromStatement(keyword);	// name of table
		RDbTable table;
		TInt err=table.Open(TheDatabase,keyword,table.EReadOnly);
		if (err!=KErrNone)
			FatalError(_L("unable to open table"));
		PrintL(table);
		table.Close();
		}
	else
		FatalError(_L("expected VIEW or TABLE keyword not present"));
	}

void TScriptEngine::DoCompareL() 
	{
	// check its right function
	TScriptToken keyword;
	iScript.GetNextTokenFromLine(keyword);
	test(keyword.CompareF(_L("COMPARE"))==0);
	//
	iScript.GetNextTokenFromLine(keyword);
	if (keyword.CompareF(_L("VIEW"))==0)
		CompareL(iResults.iView);
	else if (keyword.CompareF(_L("TABLE"))==0)
		{
		iScript.GetNextTokenFromLine(keyword);	// name of table
		RDbTable table;
		TInt err=table.Open(TheDatabase,keyword,table.EReadOnly);
		if (err!=KErrNone)
			FatalError(_L("unable to open table"));
		CompareL(table);
		table.Close();
		}
	else
		FatalError(_L("expected VIEW or TABLE keyword not present"));
	}

void TScriptEngine::CompareL(RDbRowSet& aRowSet) 
	{
	TScriptToken token;
	iScript.GetNextTokenFromLine(token);
	TBool rowIdMode=EFalse;
	CArrayFixFlat<TInt>* rowIdToTest=new CArrayFixFlat<TInt>(4);
	CleanupStack::PushL(rowIdToTest);
	if (token.Compare(_L("("))==0)				// optional row ids present?
		{
		rowIdMode=ETrue;
		iScript.GetNextTokenFromLine(token);	// first value
		TLex value;
		TInt rowId;
		for (;;) 							
			{
			value=token;
			test (value.Val(rowId)==KErrNone);
			rowIdToTest->AppendL(rowId);		// add row id to array
			iScript.GetNextTokenFromLine(token);
			if (token.Compare(_L(")"))==0)
				break;
			if (token.Compare(_L(","))==0)
				iScript.GetNextTokenFromLine(token);					
			} 
		iScript.GetNextTokenFromLine(token);
		}
	if (token.Compare(_L("{"))!=0)		
		FatalError(_L("missing '{'"));
	TInt columns=aRowSet.ColCount();
	CDbColSet* colset=aRowSet.ColSetL();
	aRowSet.BeginningL();
	while (aRowSet.NextL())
		{
		aRowSet.GetL();
		if (rowIdMode)
			{
			TInt currentId=aRowSet.ColUint(colset->ColNo(KRowIdColName));
			TBool toTest=EFalse;
			for (TInt jj=0; jj<rowIdToTest->Count(); ++jj)
				{
				if (currentId==(*rowIdToTest)[jj])
					toTest=ETrue;
				}
			if (!toTest)
				continue;
			}
		for (TInt ii=1;ii<=columns;++ii)
			{
			if (rowIdMode && ii==colset->ColNo(KRowIdColName))	// ignore row id column
				continue;
			const TDbCol& col=(*colset)[ii];
			iScript.GetNextTokenFromLine(token);				// value
			if (token.Compare(_L(","))==0)
				iScript.GetNextTokenFromLine(token);			// ignore comma
			if (aRowSet.IsColNull(ii))
				{
				if (token.CompareF(_L("NULL"))!=0)
					FatalError(_L("NULL expected"));
				continue;
				}
			CompareValues(aRowSet,ii,col.iType,token); 
			}
		}
	delete colset;
	CleanupStack::PopAndDestroy();				// rowIdToTest
	iScript.GetNextTokenFromLine(token);		// look for closing '}'
	if (token.Compare(_L("}"))!=0)		
		FatalError(_L("missing '}'"));
	iScript.ConsumeStatement();
	}

void TScriptEngine::CompareValues(RDbRowSet& aRowSet,TDbColNo ColNo,TDbColType aType,const TDesC& aToken) 
//
// compares the value from a rowset aRowset, colimn number aColNo and of type aType, with the value
// contained in the descriptor aToken
//
	{
	TLex value=aToken;
	switch (aType)
		{
	case EDbColInt32:
		{
		TInt num;
		test (value.Val(num)==KErrNone);
		if (num!=aRowSet.ColInt(ColNo))
			FatalError();
		break;
		}
	case EDbColInt8:
		{
		TInt8 num8;
		test (value.Val(num8)==KErrNone);
		if (num8!=aRowSet.ColInt8(ColNo))
			FatalError();
		break;
		}
	case EDbColInt16:
		{
		TInt16 num16;
		test (value.Val(num16)==KErrNone);
		if (num16!=aRowSet.ColInt16(ColNo))
			FatalError();
		break;
		}
	case EDbColInt64:
		{
		TInt64 num64;
		test (value.Val(num64)==KErrNone);
		if (num64!=aRowSet.ColInt64(ColNo))
			FatalError();
		break;
		}
	case EDbColUint8:
		{
		TUint8 numu8;
		test (value.Val(numu8,EDecimal)==KErrNone);
		if (numu8!=aRowSet.ColUint8(ColNo))
			FatalError();
		break;
		}
	case EDbColUint16:
		{
		TUint16 numu16;
		test (value.Val(numu16,EDecimal)==KErrNone);
		if (numu16!=aRowSet.ColUint16(ColNo))
			FatalError();
		break;
		}
	case EDbColUint32:
		{
		TUint32 numu32;
		test (value.Val(numu32,EDecimal)==KErrNone);
		if (numu32!=aRowSet.ColUint32(ColNo))
			FatalError();
		break;
		}
	case EDbColReal32:
		{
		TReal32 numr32;
		test (value.Val(numr32)==KErrNone);
		if (numr32!=aRowSet.ColReal32(ColNo))
			FatalError();
		break;
		}
	case EDbColReal64:
		{
		TReal64 numr64;
		test (value.Val(numr64)==KErrNone);
		if (numr64!=aRowSet.ColReal64(ColNo))
			FatalError();
		break;
		}
	case EDbColText8:
	case EDbColText16:
		{
		TPtrC text=aToken.Mid(1,aToken.Length()-2);			// skip quotes
		if (text.CompareF(aRowSet.ColDes(ColNo))!=0)
			FatalError();
		break;
		}
	case EDbColDateTime:
		{
		TScriptLine time=aToken.Mid(1,aToken.Length()-2);	// skip hashes
		TTime t1;
		t1.Parse(time);
		TTime t2(aRowSet.ColTime(ColNo).DateTime());
		if (t1!=t2)
			FatalError();
		break;
		}
	default:
		break;
		}
	}

void TScriptEngine::PrintL(RDbRowSet& aRowSet) 
	{
	iScript.WriteLine(TPtrC());
	TInt columns=aRowSet.ColCount();	
	CDbColSet* colset=aRowSet.ColSetL();
	TScriptLine line;
	for (TInt i=1;i<=columns;++i)
		{
		const TDbCol& col=(*colset)[i];
		line.AppendFormat(_L("%S\t"),&col.iName);
		}
	iScript.WriteLine(line);
	aRowSet.BeginningL();
	while (aRowSet.NextL())
		{
		line=TPtrC();
		for (TInt ii=1;ii<=columns;++ii)
			{
			const TDbCol& col=(*colset)[ii];
			aRowSet.GetL();
			if (aRowSet.IsColNull(ii))
				{
				line.AppendFormat(_L("NULL\t"));
				continue;
				}
			switch (col.iType)
				{
			case EDbColInt32:
				line.AppendFormat(_L("%d\t"),aRowSet.ColInt(ii));
				break;
			case EDbColInt8:
				line.AppendFormat(_L("%d\t"),aRowSet.ColInt8(ii));
				break;
			case EDbColInt16:
				line.AppendFormat(_L("%d\t"),aRowSet.ColInt16(ii));
				break;
			case EDbColInt64:
				line.AppendFormat(_L("%ld\t"),aRowSet.ColInt64(ii));
				break;
			case EDbColUint8:
				line.AppendFormat(_L("%u\t"),aRowSet.ColUint8(ii));
				break;
			case EDbColUint16:
				line.AppendFormat(_L("%u\t"),aRowSet.ColUint16(ii));
				break;
			case EDbColUint32:
				line.AppendFormat(_L("%u\t"),aRowSet.ColUint(ii));
				break;
			case EDbColReal32:
				line.AppendFormat(_L("%f\t"),aRowSet.ColReal32(ii));
				break;
			case EDbColReal64:
				line.AppendFormat(_L("%f\t"),aRowSet.ColReal64(ii));
				break;
			case EDbColText:
				line.Append(aRowSet.ColDes(ii));
				line.Append('\t');
				break;
			case EDbColDateTime:
				{
				TDateTime time(aRowSet.ColTime(ii).DateTime());
				line.AppendFormat(_L("%d:%d:%d %d/%d/%d"),time.Hour(),time.Minute(),time.Second(),time.Day(),time.Month(),time.Year());
				}	
				break;
			case EDbColLongText:
				{
				RDbColReadStream blob;
				blob.OpenLC(aRowSet,ii);
				TScriptLine text;
				blob.ReadL(text,aRowSet.ColLength(ii));
				CleanupStack::PopAndDestroy();
				line.AppendFormat(_L("%S\t"),&text);
				}
			default:
				break;
				}
			}
		iScript.WriteLine(line);
		}
	iScript.WriteLine(TPtrC());
	delete colset;
	}


LOCAL_C void CreateDatabaseL()
//
// Create the database
//
	{
	test(TheDatabase.Replace(TheFs,KTestDatabase)==KErrNone);
	}

LOCAL_C void CloseDatabase()
//
// Close the database
//
	{
	TheDatabase.Close();

	TInt err = TheCrcChecker.GenerateCrcL(KTestDatabase);
	test(err==KErrNone);
	}

LOCAL_C void SetupTestDirectory()
//
// Prepare the test directory.
//
    {
	TInt err=TheFs.Connect();
	test(err==KErrNone);
//
	err=TheFs.MkDir(KTestDatabase);
	test(err==KErrNone || err==KErrAlreadyExists);
	}

LOCAL_C void SetupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(err,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(err==KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0632
@SYMTestCaseDesc        Executes the script files
@SYMTestPriority        Medium
@SYMTestActions         Start the script engine 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void RunScriptL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0632 Running script "));
	CreateDatabaseL();
	TScriptEngine script;
	script.RunL();
	CloseDatabase();
	TheView.Close();
	}

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

GLDEF_C TInt E32Main()
    {
	test.Title();
	SetupTestDirectory();
	SetupCleanup();
	__UHEAP_MARK;
//
	TRAPD(err,RunScriptL());
	test(err==KErrNone);

	//deletion of data files must be done before call to end - DEF047652
	::DeleteDataFile(KTestDatabase);
	::DeleteDataFile(KOutputFile);//Comment this line if you want topo keep t_script.log file.	
	

#ifndef __linux__
#ifndef __TOOLS2__ 
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	test(err==KErrNone);
	test(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	test(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif
#endif
	
	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;

	TheFs.Close();
	test.Close();

	return 0;
    }
