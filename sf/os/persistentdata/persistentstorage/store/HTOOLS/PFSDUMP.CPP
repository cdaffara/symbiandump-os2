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

#include "PFSDUMP.H"

// entry point

int main(int argc,char *argv[])
	{
	if (TheProgram.ProcessCommandLine(argc,argv))
		{
		TheProgram.ExplainUsage();
		return 1;
		}
	TheProgram.Run();
	return 0;
	}

// Class Program

class Program TheProgram;

Program::Program()
	: iFile(NULL),iOptions(0),iBin(&iBinBuf),iErrors(0)
	{
	cout << setfill('0');
	}

void Program::Information()
	{
	if (!Option(quiet))
		cout << "\nEPOC PermanentFileStore Dump Utility   Version " << MajorVersion << '.' \
			<< setw(2) << MinorVersion << "(build " << setw(3) << setfill('0') << Build \
			<< ")\nCopyright (c) Symbian Software Limited 1997-2004. All rights reserved.\n\n" << flush;
	cout << hex;
	cerr << hex;
	}

void Program::ExplainUsage()
	{
	Information();
	cout << "Usage:  PFSDUMP [options] storefile\n" \
			" -v    verbose: dump contents\n" \
			" -f    generate a frame dump\n" \
			" -r<n> use nth previous store revision\n" \
			" -c    format output for comparison (not for frame dumps)\n"
			" -q    quiet: suppress logo\n" << flush;
	}

int Program::ProcessCommandLine(int argc,char ** argv)
	{
	if (argc<2)
		return 1;

	int options=0;
	while (--argc>0)
		{
		istrstream arg(*++argv);
		int c=arg.get();
		if (c=='/' || c=='-')
			{
			while ((c=arg.get())!=EOF)
				{
				switch (c)
					{
				case 'c': case 'C':
					options|=compare;
					break;
				case 'f': case 'F':
					options|=frame;
					break;
				case 'q': case 'Q':
					options|=quiet;
					break;
				case 'r': case 'R':
					arg >> iTocRevision;
					if (arg.fail() || iTocRevision<0)
						return 1;
					break;
				case 'v': case 'V':
					options|=verbose;
					break;
				default:			// unrecognised option
					return 1;
					}
				}
			}
		else if (iFile!=NULL)
			return 1;		// two filenames?
		else
			iFile=arg.str();
		}
	if (options&frame)
		options&=~compare;
	iOptions=options;
	return iFile==NULL;
	}

void Program::Run()
//
// The main part of the program
//
	{
	Information();
	StoreFile store(iFile);
	if (iErrors)
		cerr << endl;
	if (store.Empty())
		{
		cout << "Store is empty" << endl;
		return;
		}
	if (Option(frame))
		{
		store.EmitHeader();
		store.EmitFrames();
		}
	store.EmitToc();
	if (!Option(frame))
		store.EmitStreams();
	}

void Program::Abort(char const* aMessage)
	{
	cerr << aMessage << endl;
	exit(3);
	}

void Program::Corrupt(char const* aMessage)
//
// terminate after detecting a fatal corruption error
//
	{
	cerr << "\nfatal error: " << aMessage << "\ncannot continue\n" << flush;
	exit(2);
	}

ostream& Program::Error()
	{
	++TheProgram.iErrors;
	return cerr << "error: ";
	}

ostream& Program::Warning()
	{
	++TheProgram.iErrors;
	return cerr << "warning: ";
	}

// Bin stream buffer

binbuf::binbuf()
	: streambuf()
	{}

int binbuf::overflow(int)
	{return 0;}

int binbuf::underflow()
	{return EOF;}

// Class StoreFile

int StoreFile::OutWidth;

StoreFile::StoreFile(char const* aFile)
	{
#ifdef __MSVCDOTNET__
	iFile.open(aFile, ios::binary);
#else //!__MSVCDOTNET__
	iFile.open(aFile, ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__
	if (!iFile)
		Program::Abort("Unable to open file or file does not exist");
//
	unsigned long uid;
	iFile.read((char*)&uid,sizeof(uid));
	iFile.seekg(0,ios::end);
	int size=iFile.tellg();
	if (size<FrameDes::First || uid!=PermanentFileStoreUid)
		Program::Abort("Not a permanent file store");
//
	iSize=size;
	if (TheProgram.Option(Program::compare))
		OutWidth=7;
	else
		{
		int width=0;
		while (size)
			{
			++width;
			size>>=4;
			}
		OutWidth=width;
		}
//
	cout << "Dumping " << aFile << "\n\n";
//
	iFile.seekg(Header::Offset,ios::beg);
	iFile.read((char*)&iHeader,Header::Size);
//
	if (Empty())
		return;
	LoadFrames();
	LoadToc();
	}

StoreFile::~StoreFile()
	{
	iFile.close();
	}

void StoreFile::LoadFrames()
	{
	FrameDes frame;
	int offset=FrameDes::First;
	int full=FrameDes::First+FrameDes::Interval;
	int diff=FrameDes::First;

	while (offset-FrameDes::Size<iSize)
		{
		if (offset==full)
			{
			full+=FrameDes::Interval;
			diff+=FrameDes::Size;
			}
		if (offset>iSize)
			{
			Program::Warning() << "incomplete link at " << FramePos(offset-diff) << endl;
			break;
			}
		iFile.seekg(offset-FrameDes::Size,ios::beg);
		iFile >> frame;
		iFrames.Add(FramePos(offset-diff),frame);
		int length=frame.Length();
		if (length==0)
			{
			if (full>iSize && offset>iSize)
				Program::Warning() << "incomplete frame at " << FramePos(offset-diff) << endl;
			offset=full;
			}
		else
			{
			int newoffset=offset+length+FrameDes::Size;
			if (newoffset>=full || newoffset-FrameDes::Size>iSize)
				{
				Program::Error() << "bad link at " << FramePos(offset-diff) << ", skipping to next anchor link" << endl;
				offset=full;
				}
			else
				{
				offset=newoffset;
				if (full-offset<=FrameDes::Size)
					offset=full;
				}
			}
		}
	iFrames.Complete();
	}

void StoreFile::LoadToc()
	{
	FramePos toc=iHeader.Toc();
	Stream stream(iFrames,toc);
	if (!stream.IsGood())
		{
		Program::Error() << "invalid toc address " << toc << endl;
		return;
		}
	if (stream.Type()!=FrameDes::Toc)
		{
		Program::Error() << "toc address " << toc << ": refers to non-toc frame"<< endl;
		return;
		}

// find the requested store revision
	Frames::Iterator f=stream.Frame();
	Frames::Iterator const first=iFrames.Begin();
	for (int rev=TheProgram.TocRevision();rev;--rev)
		{
		do	{
			if (--f<first)
				Program::Abort("Store revision not found");
			} while (f->iDes.Type()!=FrameDes::Toc);
		}

	iToc.Load(iFile,iFrames,f,iHeader.GetReloc());

// verify the Toc stream references
	Toc::Iterator const end=iToc.End();
	for (Toc::Iterator iter=iToc.Begin();iter<end;++iter)
		{
		if (iter->iHandle.IsNull())
			Program::Error() << "missing entry in toc-delta for index " << (1+iter-iToc.Begin()) << endl;
		else if (!iter->iHandle.Avail() && iter->Pos().Pos()>=0)
			{
			f=iFrames.Find(iter->Pos());
			if (f==NULL)
				Program::Error() << "invalid stream reference in toc entry " << iter->iHandle << endl;
			else if (iter->Pos().Pos()>=toc.Pos())
				Program::Error() << "virtual stream reference in toc entry " << iter->iHandle << endl;
			else if (f->iDes.Type()!=FrameDes::Data)
				Program::Error() << "toc entry " << iter->iHandle << ": refers to non-data frame" << endl;
			}
		}
	}

void StoreFile::EmitHeader()
	{
	cout << iHeader;
	}

void StoreFile::EmitFrames()
	{
	int verbose=TheProgram.Option(Program::verbose);
	Frames::Iterator const end=iFrames.End();
	for (Frames::Iterator iter=iFrames.Begin();iter<end;++iter)
		{
		FramePos pos=iter->iPos;
		cout << "Frame at " << pos << ": " << iter->iDes << endl;
		if (!verbose)
			continue;

	// dump contents
		int length=iter->iDes.Length();
		if (length==0)
			{	// full frame
			if (iter+1==end)	// no more
				continue;
			length=iter[1].iPos.Pos()-pos.Pos();
			}
		HexDump hex;
		iFile.seekg(FileOffset(pos).Offset(),ios::beg);
		hex.Dump(iFile,length);
		hex.Flush();
		cout << endl;
		}
	cout << endl;
	}

void StoreFile::EmitToc()
	{
	cout << iToc;
	}

void StoreFile::EmitStreams()
	{
	int verbose=TheProgram.Option(Program::verbose);
	cout << endl;
	Toc::Iterator const end=iToc.End();
	for (Toc::Iterator iter=iToc.Begin();iter<end;++iter)
		{
		if (!iter->iHandle.Avail())
			{
			cout << "Stream " << iter->iHandle;
			if (iter->Pos().Pos()==-1)
				{
				cout << " is empty\n";
				continue;
				}
			if (!TheProgram.Option(Program::compare))
				cout << " at " << iter->Pos();
			Stream stream(iFrames,iter->Pos());
			if (!stream.IsGood() || stream.Type()!=FrameDes::Data)
				{
				cout << " is invalid\n";
				continue;
				}
			int len=stream.Length();
			cout << ", " << dec << len << hex << " byte" << (len==1 ? "\n" : "s\n");
			if (!verbose)
				continue;

		// dump contents
			HexDump hex;
			Frames::Iterator f=stream.Frame();
			do
				{
				FramePos pos=f->iPos;
				int len=f++->iDes.Length();
				if (len==0)
					len=f->iPos.Pos()-pos.Pos();
				iFile.seekg(FileOffset(pos).Offset(),ios::beg);
				hex.Dump(iFile,len);
				} while (f->iDes.Type()==FrameDes::Continuation);
			hex.Flush();
			cout << endl;
			}
		}
	}

// Class HexDump

HexDump::HexDump()
	: iPos(0),iByte(0)
	{
	Reset();
	}

HexDump::~HexDump()
	{
	Flush();
	}

void HexDump::Reset()
	{
	memset(iChar,' ',HexInterval+3);
	iChar[HexInterval+3]=0;
	iPtr=&iChar[2];
	}

void HexDump::Dump(istream& aStream,int aBytes)
	{
	while (--aBytes>=0 && !aStream.eof())
		{
		if (iByte==0)
			cout << Margin << FileOffset(iPos) << ':';
		int c=aStream.get();
		cout << ' ' << setw(2) << c;
		*iPtr++=char(isprint(c) ? c : '.');
		if (++iByte==HexInterval/2)
			{
			cout << ' ';
			++iPtr;
			}
		else if (iByte==HexInterval)
			Flush();
		}
	}

void HexDump::Flush()
	{
	if (iByte)
		{
		for (int ii=iByte;ii<HexInterval;++ii)
			cout << "   ";
		if (iByte<HexInterval/2)
			cout << ' ';
		cout << iChar << endl;
		iPos+=iByte;
		iByte=0;
		Reset();
		}
	}

// Toc

Toc::Toc()
	: iPos(0), iRep(NULL), iAvail(0)
	{
	memset(&iHeader,0,sizeof(iHeader));
	}

Toc::~Toc()
	{
	free(iRep);
	}


void Toc::Base(const char* aPtr,int aCount)
	{
	Entry* e=iRep;
	for (int i=1;i<=aCount;++e,++i)
		{
		e->iHandle=i;	// set the index part
		memcpy((char*)e+Entry::BaseRedundant,aPtr,Entry::BaseSize);
		aPtr+=Entry::BaseSize;
		}
	}

void Toc::Load(istream& aStream,Frames const& aFrames,Frames::Iterator aFrame,Header::Reloc const* aReloc)
	{
	iPos = aFrame->iPos;

	Stream toc1(aFrame);
	void* toc = toc1.Load(aStream);
	const char* p = reinterpret_cast<char*>(toc);
	memcpy(&iHeader,p,Head::Size);
	p+=Head::Size;
	int n = iHeader.iCount;
	if (n < 0)
		{
		memset(&iHeader,0,Head::Size);
		Program::Error() << "corrupt toc" << endl;
		return;
		}
	iRep=static_cast<Entry*>(malloc(n*sizeof(Entry)));
	if (iRep==NULL)
		Program::Abort("Out of memory");

	if (iHeader.IsDelta())
		{
		// verify the delta header
		memcpy(&iDelta,p,DeltaHead::Size);
		p+=DeltaHead::Size;
		int dn = iDelta.iCount;
		if (toc1.Length() != Head::Size + DeltaHead::Size + dn * Entry::DeltaSize)
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "incomplete toc" << endl;
			return;
			}
		
		// find the toc-base
		FramePos tocbase(iDelta.iBase + Header::tocoffset);
		if (aReloc && aReloc->iHandle.IsTocBase())
			tocbase = aReloc->iPos;
		Stream toc2(aFrames,tocbase);
		if (!toc2.IsGood())
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "invalid toc-base address " << tocbase << endl;
			return;
			}
		if (toc2.Type()!=FrameDes::Toc)
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "toc-base address " << tocbase << ": refers to non-toc frame"<< endl;
			return;
			}
		
		// validate and load the toc-base
		void* tocb = toc2.Load(aStream);
		const char* p2 = reinterpret_cast<char*>(tocb);
		Head headbase;
		memcpy(&headbase,p2,Head::Size);
		p2+=Head::Size;
		if (headbase.IsDelta())
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "toc-base is a toc-delta"<< endl;
			return;
			}
		int bn = headbase.iCount;
		if (bn > n)
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "toc-base is larger than toc"<< endl;
			return;
			}
		Base(p2,bn);
		free(tocb);

		// validate and update with the toc-delta
		int last = 0;
		while (--dn>=0)
			{
			Entry e;
			memcpy(&e,p,Entry::DeltaSize);
			p+=Entry::DeltaSize;
			int ix = e.iHandle.Index();
			if (ix<=0 || ix > n)
				{
				memset(&iHeader,0,Head::Size);
				Program::Error() << "toc-delta entry " << e.iHandle << " is outside toc"<< endl;
				return;
				}
			if (ix <= last)
				{
				memset(&iHeader,0,Head::Size);
				Program::Error() << "toc-delta entry " << e.iHandle << " is out of order"<< endl;
				return;
				}
			iRep[ix-1] = e;
			last = ix;
			}
		}
	else
		{
		if (toc1.Length() != Head::Size + n * Entry::BaseSize)
			{
			memset(&iHeader,0,Head::Size);
			Program::Error() << "incomplete toc" << endl;
			return;
			}
		Base(p,n);
		}
	free(toc);

	// apply the relocation
	if (aReloc && !aReloc->iHandle.IsTocBase())
		{
		int ix=aReloc->iHandle.Index();
		if (ix<=0 || ix>n)
			Program::Corrupt("invalid index in relocation patch");

		Entry& e=iRep[ix-1];
		if (e.iHandle.Generation()!=aReloc->iHandle.Generation())
			Program::Corrupt("incorrect generation in relocation patch");
		e.iPos=aReloc->iPos.Pos();
		}

	// count the available entries
	int avail=0;
	for (int i=0;i<n;++i)
		{
		if (iRep[i].iHandle.Avail())
			++avail;
		}
	iAvail=avail;

// verify the available list
	Handle link=iHeader.iAvail;
	if (!link.IsNull())
		{
		int ix=link.Index();
		if (!link.Avail() || ix<=0 || ix >iHeader.iCount)
			{
eAvail:		Program::Error() << "corrupt available link in toc header " << link << endl;
			return;
			}
		Entry const* en=&(*this)[ix];
		if (en->iHandle!=link)
			goto eAvail;
		for (;;)
			{
			if (--avail<0)
				{
				Program::Error() << "corrupt available list, possible circular reference" << endl;
				return;
				}
			Handle next=en->Link();
			if (next.IsNull())
				break;
			ix=next.Index();
			if (!next.Avail() || ix<=0 || ix >iHeader.iCount)
				{
eLink:			Program::Error() << "corrupt link in toc entry " << link << endl;
				return;
				}
			en=&(*this)[ix];
			if (en->iHandle!=next)
				goto eLink;
			link=next;
			}
		}
	if (avail!=0)
		Program::Error() << "corrupt available list: free index leakage" << endl;
	}

ostream& operator<<(ostream& aStream,Toc const& aToc)
	{
	int all=TheProgram.Option(Program::frame);

	Toc::Head const& head=aToc.iHeader;
	if (TheProgram.Option(Program::compare))
		aStream << "Toc: ";
	else
		aStream << "Toc at " << aToc.iPos << " with ";
	aStream << dec << head.iCount << (head.iCount==1 ? " entry: " : " entries: ") \
		<< head.iCount-aToc.iAvail  << " allocated, " << aToc.iAvail << " free\n" << hex;
	if (!all)
		aStream << "root is " << head.Root() << '\n';
	else
		{
		aStream << "first available is " << head.iAvail << '\n';
		Toc::Iterator const end=aToc.End();
		for (Toc::Iterator iter=aToc.Begin();iter<end;++iter)
			{
			aStream << (iter->iHandle==head.Root() ? "* " : Margin) << iter->iHandle;
			if (iter->iHandle.Avail())
				aStream << " free -> " << iter->Link() << '\n';
			else if (iter->Pos().Pos()==-1)
				aStream << " empty\n";
			else
				aStream << " alloc at " << iter->Pos() << '\n';
			}
		}
	return aStream << flush;
	}

// Class Stream

int Stream::Length() const
	{
	int total=0;
	Frames::Iterator f=iFrame;
	do	{
		int len=f->iDes.Length();
		if (len==0)
			len=f[1].iPos.Pos()-f[0].iPos.Pos();
		total+=len;
		} while ((++f)->iDes.Type()==FrameDes::Continuation);
	return total;
	}

void* Stream::Load(istream& aStream) const
	{
	int size = Length();
	void* data = malloc(size);
	if (data==NULL)
		Program::Abort("Out of memory");

	char* read=reinterpret_cast<char*>(data);
	Frames::Iterator f = iFrame;
	do
		{
		FramePos pos=f->iPos;
		int len=f++->iDes.Length();
		if (len==0)
			len=f->iPos.Pos()-pos.Pos();
		aStream.seekg(FileOffset(pos).Offset(),ios::beg);
		aStream.read(read,len);
		read+=len;
		} while (f->iDes.Type()==FrameDes::Continuation);

	return data;
	}

// Class Frames

Frames::Frames()
	: iSize(0),iElements(0),iRep(NULL)
	{}

Frames::~Frames()
	{
	free(iRep);
	}

void Frames::Add(FramePos aPos,FrameDes aDes)
	{
	if (iElements==iSize)
		{
		iSize=iSize==0 ? 128 : iSize+iSize;
		void* rep=realloc(iRep,iSize*sizeof(*iRep));
		if (rep==NULL)
			Program::Abort("Out of memory");
		iRep=(Element*)rep;
		}
	Element& element=iRep[iElements++];
	element.iPos=aPos;
	element.iDes=aDes;
	}

void Frames::Complete()
//
// add a terminating entry
//
	{
	Add(0,0);
	--iElements;
	}

Frames::Iterator Frames::Find(FramePos aPos) const
	{
	return (Element const*)bsearch(&aPos,iRep,iElements,sizeof(*iRep),Compare);
	}

int Frames::Compare(void const* aLeft,void const* aRight)
	{
	int left=static_cast<FramePos const*>(aLeft)->Pos();
	int right=static_cast<Element const*>(aRight)->iPos.Pos();
	if (left<right)
		return -1;
	if (left>right)
		return 1;
	return 0;
	}

// Header

FramePos Header::Toc() const
	{
	return tocoffset+(!Dirty() && iToc.iZero==0 ? iToc.iPos : iBackupToc>>backupshift);
	}

Header::Reloc const* Header::GetReloc() const
	{
	return (Dirty() || iToc.iZero==0) ? NULL : reinterpret_cast<Reloc const*>(&iReloc);
	}

ostream& operator<<(ostream& aStream,Header const& aHeader)
	{
	aStream << "Header is " << (aHeader.Dirty() ? "dirty" : "clean");
	Header::Reloc const* reloc=aHeader.GetReloc();
	if (reloc!=NULL)
		{
		aStream << "\npending relocation of ";
		if (reloc->iHandle.IsTocBase())
			aStream << "toc-base";
		else
			aStream << "stream " << StreamId(reloc->iHandle);
		aStream << " to " << reloc->iPos;
		}
	return aStream << "\n\n" << flush;
	}

// FileOffset

FileOffset::FileOffset(FramePos aPos)
// calculate the file offset for a streampos
	{
	int pos=aPos.Pos();
	int n=pos>>FrameDes::FullShift;
	pos+=FrameDes::Size*n+FrameDes::First;
	iValue=pos;
	}

FileOffset::operator FramePos() const
	{
	int pos=iValue-FrameDes::First;
	int n=pos/FrameDes::Interval;
	pos-=n*FrameDes::Size;
	return FramePos(pos);
	}

ostream& operator<<(ostream& aStream,FileOffset anOffset)
	{
	return aStream << setw(StoreFile::OutWidth) << anOffset.iValue;
	}

// Handle

ostream& operator<<(ostream& aStream,Handle aHandle)
	{
	if (aHandle.IsNull())
		aStream << "Null";
	else
		aStream << setw(6) << aHandle.Index() << ':' << aHandle.Generation();
	return aStream;
	}

// FramePos

ostream& operator<<(ostream& aStream,FramePos aPos)
	{
	return aStream << setw(StoreFile::OutWidth) << aPos.iValue << '[' << FileOffset(aPos) << ']';
	}

// FrameDes

istream& operator>>(istream& aStream,FrameDes& aFrame)
	{
	return aStream.read((char*)&aFrame,FrameDes::Size);
	}

ostream& operator<<(ostream& aStream,FrameDes aFrame)
	{
	static char const* FrameType[]={"free","data","toc","continuation"};
	aStream << FrameType[aFrame.Type()] << " (";
	int length=aFrame.Length();
	if (length==0)
		aStream << "full";
	else
		aStream << dec << length << hex;
	return aStream << ')';
	}

