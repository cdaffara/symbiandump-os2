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

#ifndef __PFSDUMP_H__
#define __PFSDUMP_H__

#ifndef __MSVCDOTNET__
#pragma warning(disable: 4710)
#endif //__MSVCDOTNET__

int const MajorVersion=1;
int const MinorVersion=6;
int const Build=66;

#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#ifdef __MSVCDOTNET__
#include <strstream>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
#else //!__MSVCDOTNET__
#include <strstrea.h>
#include <fstream.h>
#include <iomanip.h>
#endif //__MSVCDOTNET__

char const* const Margin="  ";

template <class T> inline T min(T a,T b) {return a<b ? a : b;}
template <class T> inline T max(T a,T b) {return a>b ? a : b;}

class Handle
	{
	enum
		{
		IndexMask=0xffffff,
		GenMask=0xf000000,
		GenShift=24,
		TocBase=0x40000000,
		AvailBit=0x80000000,
		TocDelta=AvailBit
		};
public:
	inline Handle() {}
	inline Handle(unsigned aValue) :iValue(aValue) {}
	inline bool IsNull() const {return iValue==0;}
	inline int Avail() const {return iValue&AvailBit;}
	inline int IsDelta() const {return iValue&TocDelta;}
	inline bool IsTocBase() const {return iValue==TocBase;}
	inline int Generation() const {return (iValue&GenMask)>>GenShift;}
	inline int Index() const {return iValue&IndexMask;}
	inline int operator==(Handle aHandle) const {return iValue==aHandle.iValue;}
	inline int operator!=(Handle aHandle) const {return iValue!=aHandle.iValue;}
private:
	unsigned long iValue;
//
	friend class StreamId;
	friend ostream& operator<<(ostream&,Handle);
	};

class StreamId : public Handle
	{
	enum {Mask=0xfffffff};
public:
	inline StreamId(Handle aHandle) :Handle(aHandle.iValue&Mask) {}
	};

class FramePos
	{
public:
	inline FramePos() {}
	inline FramePos(int aPos) :iValue(aPos) {}
	inline int Pos() const {return iValue;}
private:
	int iValue;
//
	friend ostream& operator<<(ostream&,FramePos);
	};

class FileOffset
	{
public:
	inline FileOffset(int anOffset) :iValue(anOffset) {}
	inline int Offset() const {return iValue;}
// offset/pos conversion
	FileOffset(FramePos aPos);
	operator FramePos() const;
private:
	int iValue;
//
	friend ostream& operator<<(ostream&,FileOffset);
	};

class FrameDes
	{
	enum {TypeMask=0xc000,TypeShift=14,LengthMask=0x3fff};
public:
	enum {First=0x20,Size=2,Interval=0x4002,FullShift=14};
	enum {Free=0,Data,Toc,Continuation};
//
	inline FrameDes() {}
	inline FrameDes(unsigned aDes) :iDes((unsigned short)aDes) {}
	inline int Type() const {return iDes>>TypeShift;}
	inline int Length() const {return iDes&LengthMask;}
private:
	unsigned short iDes;
//
	friend ostream& operator<<(ostream&,FrameDes);
	friend istream& operator>>(istream&,FrameDes&);
	};

class Header
	{
	enum {dirty=0x1,backupshift=1};
public:
	enum {tocoffset=-12};
public:
	struct Reloc
		{
		Handle iHandle;
		FramePos iPos;
		};
	enum {Offset=0x10,Size=14};
public:
	inline int Empty() const {return Toc().Pos()<0;}
	inline int Dirty() const {return iBackupToc&dirty;}
	FramePos Toc() const;
	Reloc const* GetReloc() const;
private:
	long iBackupToc;
	union
		{
		struct
			{
			long iZero;
			long iPos;
			} iToc;
		struct
			{
			unsigned long iHandle;
			long iPos;
			} iReloc;
		};
	unsigned short iCrc;
	};
ostream& operator<<(ostream&,Header const&);

class Frames
	{
public:
	struct Element
		{
		FramePos iPos;
		FrameDes iDes;
		};
	typedef Element const* Iterator;
public:
	Frames();
	~Frames();
//
	void Add(FramePos aPos,FrameDes aDes);
	void Complete();
	Iterator Find(FramePos aPos) const;
	inline Iterator Begin() const {return iRep;}
	inline Iterator End() const {return iRep+iElements;}
private:
	static int Compare(void const* aLeft,void const* aRight);
private:
	int iSize;
	int iElements;
	Element* iRep;
	};

class Stream
	{
public:
	inline Stream(Frames const& aFrames,FramePos aPos)	:iFrame(aFrames.Find(aPos)) {}
	inline Stream(Frames::Iterator aFrame) :iFrame(aFrame) {}
	inline int IsGood() const {return iFrame!=NULL;}
	inline int Type() const {return iFrame->iDes.Type();}
	inline Frames::Iterator Frame() const {return iFrame;}
	int Length() const;
	void* Load(istream& aStream) const;
private:
	Frames::Iterator iFrame;
	};

class Toc
	{
public:
	struct Head
		{
		enum {Size=12};
		inline StreamId Root() const {return StreamId(iRoot);}
		inline int IsDelta() const {return iRoot.IsDelta();}
		Handle iRoot;
		Handle iAvail;
		long iCount;
		};
	struct DeltaHead
		{
		enum {Size = 7};
		long iBase;
		unsigned short iMagic;
		unsigned char iCount;
		};
	struct Entry
		{
		enum {BaseSize=5,BaseRedundant=3,DeltaSize=8};
		inline Handle Link() const {return iLink;}
		inline FramePos Pos() const {return iPos;}
	//
		Handle iHandle;
		union
			{
			unsigned long iLink;
			long iPos;
			};
		};
	typedef Entry const* Iterator;
public:
	Toc();
	~Toc();
//
	void Load(istream& aStream,Frames const& aFrames,Frames::Iterator aFrame,Header::Reloc const* aReloc);
	inline Head const& Header() const {return iHeader;}
	inline Iterator Begin() const {return iRep;}
	inline Iterator End() const {return iRep+iHeader.iCount;}
	inline Entry const& operator[](int anIndex) {return iRep[anIndex-1];}
private:
	void Base(const char* aPtr,int aCount);
private:
	FramePos iPos;
	Head iHeader;
	DeltaHead iDelta;
	Entry* iRep;
	int iAvail;
//
	friend ostream& operator<<(ostream&,Toc const&);
	};

class HexDump
	{
	enum {HexInterval=0x10};
public:
	HexDump();
	~HexDump();
//
	void Reset();
	void Dump(istream& aStream,int aBytes);
	void Flush();
private:
	int iPos;
	int iByte;
	char* iPtr;
	char iChar[HexInterval+4];
	};

class StoreFile
	{
	enum {PermanentFileStoreUid=268435536};
public:
	StoreFile(char const* aFile);
	~StoreFile();
//
	inline int Empty() const {return iHeader.Empty();}
	void EmitHeader();
	void EmitFrames();
	void EmitToc();
	void EmitStreams();
private:
	void LoadFrames();
	void LoadToc();
public:
	static int OutWidth;
private:
	ifstream iFile;
	int iSize;
	Header iHeader;
	Frames iFrames;
	Toc iToc;
	};

class binbuf : public streambuf
	{
public:
	binbuf();
private:
	int overflow(int);
	int underflow();
	};

class Program
	{
public:
	enum {frame=0x1,quiet=0x2,verbose=0x4,compare=0x8};
public:
	Program();
//
	int ProcessCommandLine(int argc,char ** argv);
	void Information();
	void ExplainUsage();
	void Run();
	inline int Option(int type) const {return iOptions&type;}
	inline int TocRevision() const {return iTocRevision;}
//
	static void Abort(char const* message);
	static void Corrupt(char const* message);
	static ostream& Error();
	static ostream& Warning();
private:
	char const* iFile;
	int iOptions;
	int iTocRevision;
	binbuf iBinBuf;
	ostream iBin;
	int iErrors;
	};

extern Program TheProgram;

#endif
