// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32file.h>
#include <e32test.h>

static RTest TheTest(_L("t_dblongcol"));
static CTrapCleanup* TheTrapCleanup;
const TInt KTestCleanupStack=0x40;
RFs TheFsSession;

_LIT( KTestDir, "c:\\DBMS-TST\\" );
_LIT( KSearchTestDbPath, "c:\\DBMS-TST\\eposlmserachtest.ldb" );

//Delete "aFullName" file.
static void DeleteDataFile(const TDesC& aFullName)
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

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteDataFile(KSearchTestDbPath);
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::DeleteDataFile(KSearchTestDbPath);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static void LeaveL(TInt aLine, TInt aError)
	{
	RDebug::Print(_L("*** Leave. Error: %d, line: %d\r\n"), aError, aLine);
	User::Leave(aError);
	}
static void LeaveIfErrorL(TInt aLine, TInt aError)
	{
	if(aError < KErrNone)
		{
		LeaveL(aLine, aError);
		}
	}
#define __LEAVE(err) LeaveL(__LINE__, err)
#define __LEAVE_IF_ERROR(err) LeaveIfErrorL(__LINE__, err)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//
// Prepare the test directory.
//
static void SetupTestDirectory()
    {
	TInt r=TheFsSession.MkDir(KTestDir);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	r=TheFsSession.SetSessionPath(KTestDir);
	TEST2(r,KErrNone);
	}

static TInt SearchForL( const TPtrC& aSearchString, RDbNamedDatabase& aDb )
	{
	_LIT( KSQLSearchStatement, "select Notes from CDs where Notes like '%S'" );
	TBuf<512> query;
	query.Format( KSQLSearchStatement, &aSearchString );

	// Display query
	_LIT( KQueryFormatter, "\r\n %S\r\n" );
	TBuf<512> msg;
	msg.Format( KQueryFormatter, &query );
	TheTest.Printf( msg );

	// create a view on the database
	RDbView view;
	// use EDbCompareCollated in order to search case-insensitive
	__LEAVE_IF_ERROR( view.Prepare( aDb, TDbQuery( query, EDbCompareCollated ) ) );
	__LEAVE_IF_ERROR( view.EvaluateAll() );

	// iterate across the row set
	TInt noOfMatches = 0;
	for ( view.FirstL(); view.AtRow(); view.NextL() )
		{
		// retrieve the row
		view.GetL();
		noOfMatches++;
		}

	// Display no of matches
	_LIT( KNoOfMatchFormatter, " Found %d matches\r\n" );
	msg.Format( KNoOfMatchFormatter, noOfMatches );
	TheTest.Printf( msg );

	// close the view
	view.Close();
	return noOfMatches;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0645
@SYMTestCaseDesc        Searching for data from a database
@SYMTestPriority        Medium
@SYMTestActions         Tests for EDbColText,EDbColLongText column type
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestSearchL( TInt aIndex )
	{
	// Default database
	_LIT( KComposer1, "Elgar" );
	_LIT( KCol1, "Artist" );
	_LIT( KCol2, "Notes" );
	_LIT( KTable, "CDs" );

	TInt err = TheFsSession.Delete( KSearchTestDbPath );
	if ( ( err != KErrNone ) && ( err != KErrNotFound ) && ( err != KErrPathNotFound ) )
		{
		__LEAVE( err );
		}

	RDbNamedDatabase db;
	CleanupClosePushL( db );
	__LEAVE_IF_ERROR( db.Create( TheFsSession, KSearchTestDbPath ) );

	//
	// Create the database table
	//
	// Create a table definition
	CDbColSet* columns = CDbColSet::NewLC();
	// add the columns
	columns->AddL( TDbCol( KCol1, EDbColText ) );
	if ( aIndex == 0 )
		columns->AddL( TDbCol( KCol2, EDbColText ) );
	else
		columns->AddL( TDbCol( KCol2, EDbColLongText ) );
	// if the column is of type "EDbColText" instead,
	// all searching is working properly
	// Create a table
	__LEAVE_IF_ERROR( db.CreateTable( KTable, *columns ) );
	// cleanup the column set
	CleanupStack::PopAndDestroy( columns );

	//
	// Add data
	//
	// create a view on the database
	_LIT( KSQLStatement, "select Artist,Notes from CDs order by Artist" );
	RDbView view;
	__LEAVE_IF_ERROR( view.Prepare( db, TDbQuery( KSQLStatement, EDbCompareNormal ) ) );
	__LEAVE_IF_ERROR( view.EvaluateAll() );

	// Get the structure of rowset
	CDbColSet* colSet = view.ColSetL();
	// insert a row
	view.InsertL();
	view.SetColL( colSet->ColNo( KCol1 ), KComposer1 ); // Artist
	// Use the stream
	// RDbColWriteStream out;
	// TDbColNo col = colSet->ColNo( KCol2 ); // Ordinal position of long column
	//
	// out.OpenLC( view, col );
	// out.WriteL( _L( "Some additional comment here." ) );
	// out.Close();
	//
	// CleanupStack::PopAndDestroy(); // out
	view.SetColL( colSet->ColNo( KCol2 ), _L( "Some additional comment here." ) );
	view.PutL();
	// close the view
	view.Close();
	delete colSet;

	//
	// Display the data
	//
	_LIT( KRowFormatter, "\r\n Artist: %S \r\n Notes: %S \r\n" );
	__LEAVE_IF_ERROR( view.Prepare( db, TDbQuery( KSQLStatement, EDbCompareNormal ) ) );
	__LEAVE_IF_ERROR( view.EvaluateAll() );

	// Get the structure of the rowset
	colSet = view.ColSetL();
	// iterate across the row set
	for ( view.FirstL(); view.AtRow(); view.NextL() )
		{
		// retrieve the row
		view.GetL();
		// while the rowset is on this row, can use a TPtrC to
		// refer to any text columns
		TPtrC artist = view.ColDes( colSet->ColNo( KCol1 ) );
		// and a stream for long columns
		RDbColReadStream in;
		TDbColNo col = colSet->ColNo( KCol2 ); // Ordinal position of long column
		TBuf<256> notes; // Buffer for out notes
		in.OpenLC( view, col );
		in.ReadL( notes, view.ColLength( col ) );
		in.Close();
		CleanupStack::PopAndDestroy(); // in
		// Display the artist and notes
		TBuf<512> msg;
		msg.Format( KRowFormatter, &artist, &notes );
		TheTest.Printf( msg );
		}
	// close the view
	view.Close();
	delete colSet;

	//
	// Search for the data
	//
	TInt result;
	result = SearchForL( _L( "Some*" ), db ); // matches
	TEST(result == 1);
	result = SearchForL( _L( "some*" ), db ); // defect causes no match, should match
	TEST(result == 1);
	result = SearchForL( _L( "*some*" ), db ); // matches
	TEST(result == 1);
	result = SearchForL( _L( "s?me*" ), db ); // matches
	TEST(result == 1);
	result = SearchForL( _L( "Some additional comment here." ), db ); // matches
	TEST(result == 1);
	result = SearchForL( _L( "some additional comment here." ), db ); // defect causes no match, should match
	TEST(result == 1);

	CleanupStack::PopAndDestroy( &db );
	}

//
// Initialise the cleanup stack.
//
static void setupCleanup()
	{
	TheTrapCleanup=CTrapCleanup::New();
	TEST(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(r,KErrNone);
	}

//
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
	{
	TheTest.Title();
	setupCleanup();
	__UHEAP_MARK;

	TInt err = TheFsSession.Connect();
	TEST2(err, KErrNone);
	::SetupTestDirectory();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0645 Testing DBMS - EDbColText "));
	TRAP(err, TestSearchL( 0 ));
	TEST2(err,KErrNone);
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0645 Testing DBMS - EDbColLongText "));
	TRAP(err, TestSearchL( 1 ));
	TEST2(err,KErrNone);
	::DeleteDataFile(KSearchTestDbPath);
	TheTest.End();

	TheFsSession.Close();

	__UHEAP_MARKEND;
	delete TheTrapCleanup;
	TheTest.Close();
	return 0;
	}

