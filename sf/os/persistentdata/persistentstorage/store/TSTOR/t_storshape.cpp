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

#include <s32file.h>
#include <e32test.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_SHAPE.DAT");

class CShape;
enum TShape {ENotAShape,ESquare,ECircle};
typedef CShape* (*TShapeCtor)();
class CShape : public CBase
	{
public:
	static TShapeCtor Ctor(TShape type);
//
	virtual void ExternalizeL(RWriteStream& aStream) const=0;
	virtual void InternalizeL(RReadStream& aStream)=0;
//
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(const CStreamStore& aStore,TStreamId anId);
//
	virtual TPtrC Name() const=0;
	virtual TSize Extent() const=0;
	virtual TPoint Centre() const=0;
	};
class CSquare : public CShape
	{
public:
	static CShape* New();
	CSquare() {}
	CSquare(const TPoint& aCentre,TInt aSide);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	TPtrC Name() const;
	TSize Extent() const;
	TPoint Centre() const;
private:
	TRect iRect;
	};
class CCircle : public CShape
	{
public:
	static CShape* New();
	CCircle() {}
	CCircle(const TPoint& aCentre,TInt aRadius);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	TPtrC Name() const;
	TSize Extent() const;
	TPoint Centre() const;
private:
	TPoint iCentre;
	TInt iRadius;
	};

class TShapeHolder
	{
public:
	TShapeHolder();
	TShapeHolder(TShape aType,CShape* aShape);
//
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	void StoreComponentsL(CStreamStore& aStore,CStoreMap& aMap) const;
	void RestoreComponentsL(const CStreamStore& aStore);
//
	void ExternalizeSerialL(RWriteStream& aStream) const;
	void InternalizeSerialL(RReadStream& aStream);
//
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(const CStreamStore& aStore,TStreamId anId);
//
	CShape* Shape() const;
private:
	TShape iType;
	TSwizzle<CShape> iShape;
	};

LOCAL_D RTest test(_L("t_storshape"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
//LOCAL_D CBufStore* TheStore;
LOCAL_D CFileStore* TheStore;
LOCAL_D RStoreWriteStream TheSink;
LOCAL_D RStoreReadStream TheSource;
//LOCAL_D RFileWriteStream TheSink;
//LOCAL_D RFileReadStream TheSource;

/**
@SYMTestCaseID          PDS-STORE-CT-4025
@SYMTestCaseDesc	    Basic test for CStoreMap Forget() and Unbind() API
@SYMTestPriority 	    High
@SYMTestActions  	    Unbind stream from Map, forget sgtream from map
@SYMTestExpectedResults map stream ID should be NUll
@SYMDEF                 DEF135804
*/
LOCAL_C void testExtraStoreMapAPIsL()
	{
	test.Next(_L("@SYMTestCaseID PDS-STORE-CT-4025"));
	CShape* shape=new(ELeave) CCircle(TPoint(70,80),40);
	CleanupStack::PushL(shape);
	CStoreMap* map=CStoreMap::NewLC(*TheStore);
	TStreamId id = shape->StoreL(*TheStore);
	
	map->BindL(shape,id);
	test(id == map->At(shape));
	//Unbind the twizzle(shape) and test to make sure it is unbinded
	map->Unbind(shape);
	test(KNullStreamId == map->At(shape));
	
	map->BindL(shape,id);
	test(shape == map->Label(id));
	//Forget the stream id and test to make sure it is forgotten
	map->Forget(id);
	test(shape != map->Label(id));
	CleanupStack::PopAndDestroy(2,shape);
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1200
@SYMTestCaseDesc	    Shape streaming test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for streaming of different shapes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testShapesL()
    {
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1200 Shape streaming "));

	TParsePtrC parse(KFileLocationSpec);
//
//	TheStore=CDirectFileStore::ReplaceLC(TheFs,KTestFile,EFileRead|EFileWrite);
	TheStore=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());
//
	RStoreWriteStream snk;
	TStreamId id=snk.CreateL(*TheStore);
	TShapeHolder hldr(ESquare,new(ELeave) CSquare(TPoint(20,30),40));
	hldr.ExternalizeSerialL(snk);
	delete hldr.Shape();
	hldr=TShapeHolder(ECircle,new(ELeave) CCircle(TPoint(70,80),40));
	hldr.ExternalizeSerialL(snk);
	delete hldr.Shape();
	snk.Close();
	RStoreReadStream src;
	src.OpenL(*TheStore,id);
	hldr.InternalizeSerialL(src);
	delete hldr.Shape();
	hldr.InternalizeSerialL(src);
	delete hldr.Shape();
	src.Close();
//
	hldr=TShapeHolder(ESquare,new(ELeave) CSquare(TPoint(20,30),40));
	id=hldr.StoreL(*TheStore);
	delete hldr.Shape();
	hldr.RestoreL(*TheStore,id);
	delete hldr.Shape();
//
	CShape* shape=new(ELeave) CCircle(TPoint(70,80),40);
	CStoreMap* map=CStoreMap::NewL(*TheStore);
	TStreamId id2 = shape->StoreL(*TheStore);

	testExtraStoreMapAPIsL();
	
	map->BindL(shape,id2);
	snk=RStoreWriteStream(*map);
	id=snk.CreateL(*TheStore);
	snk<<shape;
	snk.Close();
	delete shape;
	src.OpenL(*TheStore,id);
	src>>id;
	src.Close();
	shape=new(ELeave) CCircle;
	shape->RestoreL(*TheStore,id);
	delete map;
	TRAPD(r,shape->RestoreL(*TheStore,id));
	test(r==KErrNotFound);
	delete shape;
//
	CleanupStack::PopAndDestroy();
    }

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	r=TheFs.MkDir(parse.DriveAndPath());
	test(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(parse.DriveAndPath());
	test(r==KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
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

//
// Test the streaming framework.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	TRAPD(r,testShapesL());
	test(r==KErrNone);

	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::DeleteDataFile(parse.FullName());

	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
    }

TShapeCtor CShape::Ctor(TShape type)
	{
	switch (type)
		{
	case ESquare:
		return &CSquare::New;
	case ECircle:
		return &CCircle::New;
	default:
		return NULL;
		}
	}

TStreamId CShape::StoreL(CStreamStore& aStore) const
	{
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}

void CShape::RestoreL(const CStreamStore& aStore,TStreamId anId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,anId);
	InternalizeL(stream);
	CleanupStack::PopAndDestroy();
	}

CShape* CSquare::New()
	{
	return new CSquare;
	}

CSquare::CSquare(const TPoint& aCentre,TInt aSide)
	{
	TInt offset=aSide/2;
	iRect.iTl.iX=aCentre.iX-offset;
	iRect.iTl.iY=aCentre.iY-offset;
	iRect.iBr.iX=iRect.iTl.iX+aSide;
	iRect.iBr.iY=iRect.iTl.iY+aSide;
	}

void CSquare::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iRect.iTl.iX);
	aStream.WriteUint32L(iRect.iTl.iY);
	aStream.WriteUint32L(iRect.iBr.iX);
	aStream.WriteUint32L(iRect.iBr.iY);
	}

void CSquare::InternalizeL(RReadStream& aStream)
	{
	iRect.iTl.iX=aStream.ReadUint32L();
	iRect.iTl.iY=aStream.ReadUint32L();
	iRect.iBr.iX=aStream.ReadUint32L();
	iRect.iBr.iY=aStream.ReadUint32L();
	}

TPtrC CSquare::Name() const
	{
	return _L("Square");
	}

TSize CSquare::Extent() const
	{
	return (iRect.iBr-iRect.iTl).AsSize();
	}

TPoint CSquare::Centre() const
	{
	return TPoint((iRect.iBr.iX-iRect.iTl.iX)/2,(iRect.iBr.iY-iRect.iTl.iY)/2);
	}

CShape* CCircle::New()
	{
	return new CCircle;
	}

CCircle::CCircle(const TPoint& aCentre,TInt aRadius)
	: iCentre(aCentre),iRadius(aRadius)
	{}

void CCircle::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iCentre.iX);
	aStream.WriteUint32L(iCentre.iY);
	aStream.WriteUint32L(iRadius);
	}

void CCircle::InternalizeL(RReadStream& aStream)
	{
	iCentre.iX=aStream.ReadUint32L();
	iCentre.iY=aStream.ReadUint32L();
	iRadius=aStream.ReadUint32L();
	}

TPtrC CCircle::Name() const
	{
	return _L("Circle");
	}

TSize CCircle::Extent() const
	{
	TInt diameter=iRadius*2;
	return TSize(diameter,diameter);
	}

TPoint CCircle::Centre() const
	{
	return iCentre;
	}

TShapeHolder::TShapeHolder()
	: iType(ENotAShape),iShape(NULL)
	{}

TShapeHolder::TShapeHolder(TShape aType,CShape* aShape)
	: iType(aType),iShape(aShape)
	{
	__ASSERT_DEBUG((iType==ENotAShape)==(aShape==NULL),User::Panic(_L("gargl"),0));
	}

void TShapeHolder::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iType);
	aStream<<iShape;
	}

void TShapeHolder::InternalizeL(RReadStream& aStream)
	{
	TShape type=TShape(aStream.ReadUint8L());
	if ((type==ENotAShape)!=(CShape::Ctor(type)==NULL))
		User::Leave(KErrCorrupt); // representation violation!!!
//
	aStream>>iShape;
	iType=type;
	}

void TShapeHolder::StoreComponentsL(CStreamStore& aStore,CStoreMap& aMap) const
	{
	if (iShape!=NULL)
		{
		TStreamId id=iShape->StoreL(aStore);
		aMap.BindL(iShape,id);
		}
	}

void TShapeHolder::RestoreComponentsL(const CStreamStore& aStore)
	{
	TShapeCtor ctor=CShape::Ctor(iType);
	CShape* shape=NULL;
	if (ctor!=NULL)
		{
		User::LeaveIfNull(shape=(*ctor)());
		CleanupStack::PushL(shape);
		shape->RestoreL(aStore,iShape.AsId());
		CleanupStack::Pop();
		}
	iShape=shape;
	}

void TShapeHolder::ExternalizeSerialL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iType);
	if (iShape!=NULL)
		aStream<<*iShape;
	}

void TShapeHolder::InternalizeSerialL(RReadStream& aStream)
	{
	TShape type=TShape(aStream.ReadUint8L());
	TShapeCtor ctor=CShape::Ctor(type);
	if ((type==ENotAShape)!=(ctor==NULL))
		User::Leave(1832); // representation violation!!!
//
	CShape* shape=NULL;
	if (ctor!=NULL)
		{
		User::LeaveIfNull(shape=(*ctor)());
		CleanupStack::PushL(shape);
		aStream>>*shape;
		CleanupStack::Pop();
		}
	iType=type;
	iShape=shape;
	}

TStreamId TShapeHolder::StoreL(CStreamStore& aStore) const
	{
	CStoreMap* map=CStoreMap::NewLC(aStore);
	StoreComponentsL(aStore,*map);
//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
	stream.CommitL();
//
	map->Reset();
	CleanupStack::PopAndDestroy(2);
	return id;
	}

void TShapeHolder::RestoreL(const CStreamStore& aStore,TStreamId anId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,anId);
	InternalizeL(stream);
	CleanupStack::PopAndDestroy();
//
	RestoreComponentsL(aStore);
	}

CShape* TShapeHolder::Shape() const
	{
	return iShape;
	}

