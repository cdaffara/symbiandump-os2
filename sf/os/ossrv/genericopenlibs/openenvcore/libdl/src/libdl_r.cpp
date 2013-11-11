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
// Name        : libdl_r.cpp
// Part of     : libdl library
// Implements the libdl's _r dl functions
//



// INCLUDE FILES
#include <f32file.h>   	//class TParsePtr
#include <stdlib.h>   	//getenv
#include <string.h>	    //strlen
#include "loadeddlls.h"	//CLoadedDlls	
#include "libdl_r.h"	
#include <dlfcn.h>		//RTLD_DEFAULT, RTLD_NEXT etc.
#include <e32std.h> 	//RPointerArray
#include <utf.h>        //CnvUtfConverter

#ifdef __EPOC32__
class XLibdlTLSCleanup
{
public:
	~XLibdlTLSCleanup()
	{
		Dll::FreeTls();
	}
};
XLibdlTLSCleanup gLibdlTLSCleanup;
#endif


#ifdef __WINSCW__ 
#include <emulator.h> //GetProcAddress, GetModuleHandle
#include <pls.h> // For emulator WSD API 
const TUid KLibdlUid3 = {0x10281F32}; 
#endif//defined(__WINSCW__) || defined(__WINS__)

static const TInt KFourByteOffset = sizeof(int);
static const TInt KTwoByteOffset = sizeof(short);

//error strings
static const char* KDlOpenErrNoSupport = "dlopen: Wild character not supported";
static const char* KDlOpenErrLoading = "dlopen: Load failed";
static const char* KDlOpenErrNoMemory = "dlopen: Reached the DLL upperlimit for the current process";
static const char* KDlSymErrNotFound = "dlsym: Symbol not found";
static const char* KDlSymErrNoHandleSupport = "dlsym: support for special handles is not available";
static const char* KDlSymErrNoSupport = "dlsym: Symbol lookup on non STDDLL not supported";
static const char* KDlSymErrBadHandle = "dlsym: Stale handle passed to dlsym";
static const char* KDlSymErrBadOrdinal = "dlsym: No function at the specified ordinal";
static const char* KDlSymErrNotStdExe = "dlsym: Not stdexe";
static const char* KDlSymErrNoMemory = "dlsym: Not enough memory";
static const char* KDlCloseErrBadHandle = "dlclose: Stale handle passed to dlclose()";


static void* SearchName(const TE32ExpSymInfoHdr* aSymInfoHeader,const char *aName);

static void SetError(const char* aErrString);

static void* DlSymByDependencyOrdering(const void* aHandle,const char* aName);

static void* DlSymByLoadOrdering(const char* aName);

#if defined(__WINSCW__) || defined(__WINS__)

static TInt SearchNameInExe(const TE32ExpSymInfoHdr* aSymInfoHeader,
							const char* aName, void*& aSymAddress);
static TInt SearchSymbolByLoadordering(const TE32ExpSymInfoHdr* symInfoHeader,
									   const char* aFileName, const char* aName,
									   RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
									   void*& aSymAddress);

#else

static TInt SearchSymbolByLoadordering(const TE32ExpSymInfoHdr* aSymInfoHeader,
									   const char* aName,
									   RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
									   void*& aSymAddress);

#endif//defined(__WINSCW__) || defined(__WINS__)


#if ( (defined(__WINSCW__) || defined(__WINS__)) )

class TImageData
	{
	public:
	TImageData(){}
	TImageData(char* aFileName, TE32ExpSymInfoHdr* aSymInfoHeader) : 
							iFileName(aFileName), iSymInfoHeader(aSymInfoHeader)
								{
								}
	char* iFileName;
	TE32ExpSymInfoHdr* iSymInfoHeader;
	};

//WSD solution for emulator
static CLoadedDlls* LoadedDlls()
	{
	// Access the PLS of this process
	CLoadedDlls* LoadedDlls = Pls<CLoadedDlls>(KLibdlUid3); 
	return LoadedDlls;  

	}
#else 

static CLoadedDlls* LoadedDlls()
	{
	static CLoadedDlls LoadedDlls;
	return &LoadedDlls;
	}
	
#endif//(defined(__WINSCW__) || defined(__WINS__)

//-----------------------------------------------------------------------------
//Function Name : void* __dlopen_r(const char* filename, int flag)
//Description   : To open the given dll filename.
//Return Value  : Valid handle value if no error, Null if dll couldn't be loaded 
//-----------------------------------------------------------------------------
void* __dlopen_r(const char* filename,const int flag)
	{
	//convert filename to TFileName
	TPtrC8 ptr8( (unsigned char*)filename);
	TFileName fileNameBuf;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(fileNameBuf, ptr8);
	TParsePtr filePathName(fileNameBuf);
	
	//if file name contains wild character
	if ( filePathName.IsWild() )
		{
		SetError(KDlOpenErrNoSupport);
		return NULL;
		}
	//to load dll	
	RLibrary library;	
	TInt err;
	TBool isLibraryLoaded = EFalse;
	RFs fs;
	err = fs.Connect();
	if ( KErrNone == err )
		{
		TUint tempAtt;
		//if path is there load from this path
		if ( filePathName.PathPresent() )
			{
			err = fs.Att(filePathName.FullName(), tempAtt);
			fs.Close();	
			if ( KErrNotFound != err && KErrPathNotFound != err)
				{
				err = library.Load(filePathName.FullName());
				if ( KErrNone != err )
					{
					SetError(KDlOpenErrLoading);
					return NULL;
					}
				isLibraryLoaded = ETrue;	
				}
			}
		else//if there is no path its only file name
			{
			TPtrC fileName(filePathName.NameAndExt());
			char* envPathName = getenv("LD_LIBRARY_PATH");
			if ( envPathName )
				{
				TPtrC8 tempPtr8((unsigned char*)envPathName);
				TFileName envPathBuf;
				CnvUtfConverter::ConvertToUnicodeFromUtf8(envPathBuf, tempPtr8);
				TPtrC envPath(envPathBuf);
				TChar delim(';');
				TFileName fileToLoad;
				TInt pos = envPath.Locate(delim);
				//if delim does not found and still envPath contains value
				//i.e. this one is last path without delim(';') so take 
				//this also, for this length is checked
				while ( KErrNotFound != pos || envPath.Length())
					{
					//if there is no delim
					if (KErrNotFound == pos )
						{// so last path without delim
						pos = envPath.Length();
						}
					TPtrC thisPath(envPath.Left(pos));
					fileToLoad = thisPath;
					fileToLoad.Trim();
					//to check ";;" and ";   ;"
					if (fileToLoad.Length())
						{
						//if path does not conatin trailing \ add one
						if ( L'\\' != fileToLoad[fileToLoad.Length()-1] )
							{
							fileToLoad.Append(TChar(L'\\'));					
							}
						fileToLoad.Append(fileName);
					
						err = fs.Att(fileToLoad, tempAtt);
						if ( KErrNotFound != err && KErrPathNotFound != err)
							{
							//load file from this path
							err = library.Load(fileToLoad);
							if ( KErrNone == err )
								{
								// dll loaded successfully from thispath
								isLibraryLoaded = ETrue;	
								break;
								}	
							}
						}
					if ( pos == envPath.Length())		
						{
						break;
						}
					else
						{
						envPath.Set(envPath.Mid(pos + 1));
						}
					pos = envPath.Locate(delim);
					}
				fs.Close();	
				}
			else//load dll if only name is there and LD_LIBRARY_PATH path not set
				{
				fs.Close();	
				TInt err = library.Load(filePathName.NameAndExt());
				if ( KErrNone != err )
					{
					SetError(KDlOpenErrLoading);
					return NULL;
					}
				isLibraryLoaded = ETrue;		
				}
			}
	
		//if library is loaded	
		if ( isLibraryLoaded )	
			{
			//handle value to return 
			void* handle  = NULL;
			//lock list before any operation
			LoadedDlls()->Lock();
			//is this dll already there
			TInt listIdx = LoadedDlls()->Find(library);
			//if not already open
			if ( KErrNotFound == listIdx )
				{				
				TDllEntry dllEntry(library, flag);
				dllEntry.iSymbolHeader = (TE32ExpSymInfoHdr*)library.Lookup(0);
				
				err = dllEntry.iLibrary.Duplicate(RThread());	
				//Close the library irrespective of the result of duplicate
				library.Close();
				if ( KErrNone != err )
					{
					SetError(KDlOpenErrLoading);
					LoadedDlls()->UnLock();
					return NULL;
					}
				//add to list	
				err = LoadedDlls()->Add(dllEntry);
				handle = (void*) dllEntry.iLibrary.Handle();
				LoadedDlls()->UnLock();
				if ( KErrNone != err )
					{
					SetError(KDlOpenErrNoMemory);
					dllEntry.iLibrary.Close();
					return NULL;
					}
				}
			else//if this dll is already in list.
				{
				//close library we already have loaded
				library.Close();
				TDllEntry& dllEntry = LoadedDlls()->At(listIdx);
				//if flag is RTLD_GLOBAL store it otherwise ignore
				//as RTLD_LAZY and RTLD_NOW both means RTLD_NOW
				//so can be ignored. RTLD_LOCAL does not change previous flag
				if ( flag & RTLD_GLOBAL )
					{
					dllEntry.iFlags |= RTLD_GLOBAL;
					}
				dllEntry.iRefCount++;	
				handle = (void*) dllEntry.iLibrary.Handle();
				LoadedDlls()->UnLock();	
				}	
			return handle;			
			}
		}
	SetError(KDlOpenErrLoading);
	return NULL;		
	}

//-----------------------------------------------------------------------------
//Function Name : void* __dlsym_r(const void* handle,const char* name) 
//Description   : To return address of name i.e. symbol.
//Return Value  : Valid handle value if no error, Null if dll couldn't be found 
//-----------------------------------------------------------------------------
void* __dlsym_r(const void* handle,const char* name) 
	{
	//if handle is RTLD_NEXT or RTLD_SELF
	if ( RTLD_NEXT == handle || RTLD_SELF == handle )
		{
		SetError(KDlSymErrNoHandleSupport);
		return NULL;	
		}
		
	//if handle is RTLD_DEFAULT
	if (RTLD_DEFAULT == handle)
		{
		return DlSymByLoadOrdering(name);
		}
			
	TLex8 lex((unsigned char*)name);
	TInt tempOrdinal = 0;
	
	//if name is an ordinal no
	if ( KErrNone == lex.Val(tempOrdinal) )
		{
		// address to return
		void* symAddr = NULL;
		
		//find Symbol address by ordinal number
			
		//check whether there is something remaing in lex after 
		//conveting to int i.e. string containg only digit. 
		//and also ordinal no is not zero
		if ( 0 == lex.Remainder().Length() && tempOrdinal >= 0)
			{
			LoadedDlls()->Lock();
			TInt idx = LoadedDlls()->Find(handle);
			//check if handle not found	
			if ( KErrNotFound == idx )
				{
				LoadedDlls()->UnLock();
				SetError(KDlSymErrBadHandle);
				return NULL;
				}
			TDllEntry& dllEntry = LoadedDlls()->At(idx);
			symAddr = (void*) dllEntry.iLibrary.Lookup(tempOrdinal);
			LoadedDlls()->UnLock();
			}
		if ( !symAddr )
			{
			SetError(KDlSymErrBadOrdinal);
			}
		return symAddr;			
		}
	else
		{//when handle is specified
		return DlSymByDependencyOrdering(handle, name);
		}
	}

//-----------------------------------------------------------------------------
//Function Name : const char* __dlerror_r()
//Description   : To return latest error occured in any of dlopen, dlsym, 
//				  dlclose.
//Return Value  : char* to error string if some error occured since last access
//				  to this function else NULL
//-----------------------------------------------------------------------------
const char* __dlerror_r()
	{
	const char* errStr = (char*) Dll::Tls();;
	//reset Tls to NULL
	Dll::SetTls(NULL);
	//return pointer to error string
	return errStr;
	}

//-----------------------------------------------------------------------------
//Function Name : int __dlclose_r(const void* handle)
//Description   : To close the given dll handle.
//Return Value  : 0 if no error otherwise -1
//-----------------------------------------------------------------------------
int __dlclose_r(const void* handle)
	{
	if ( handle == RTLD_DEFAULT )
		{
		return 0;
		}
	LoadedDlls()->Lock();
	//need this index to delete from LoadedDlls if refcount is zero
	TInt listIdx = LoadedDlls()->Find(handle); 
	if ( KErrNotFound == listIdx )
		{
		LoadedDlls()->UnLock();
		SetError(KDlCloseErrBadHandle);
		return -1;
		}
	TDllEntry& dllEntry = LoadedDlls()->At(listIdx);
	dllEntry.iRefCount--;
	//if iRefCount is zero remove from list
	if( !dllEntry.iRefCount )
		{
		dllEntry.iLibrary.Close();
		LoadedDlls()->Remove(listIdx);
		}
	LoadedDlls()->UnLock();
	return 0;
	}

//-----------------------------------------------------------------------------
//Function Name : void* DlSymByLoadOrdering(const char* name)
//Description   : To search symbol named name in exe, its dependent dlls and dependent of 
//				  dependents in Load-ordering. And also in dlls opened using
//				  dlopen with RTLD_GLOBAL flag. Also sets proper error if  
//				  occur.	
//Return Value  : Valid address if name found otherwise NULL.
//-----------------------------------------------------------------------------
void* DlSymByLoadOrdering(const char* name)
	{
	// Get 0th ordinal of calling EXE.
	TE32ExpSymInfoHdr* symInfoHeader  = (TE32ExpSymInfoHdr*) RProcess::ExeExportData();
	//Check whether it's valid
	if ( !symInfoHeader )
		{
		SetError(KDlSymErrNotStdExe);
		return NULL;
		}
	
	void* symAddr = NULL;
		
	//Used to check circular dependency
	RPointerArray<TE32ExpSymInfoHdr> searchedDlls;
	
#if defined(__WINSCW__) || defined(__WINS__)
	//search name using load ordering starting dependencies of EXE
	TInt err = SearchNameInExe(symInfoHeader, name, symAddr);
	if( KErrNone == err )
		{
		return symAddr;
		}

	//now search exe dependencies and dependencies of dependencies.
		
	//searched dll is passed to avoid searching dlls which are searched in previous 
	//SearchSymbolByLoadordering function call i.e. this would be reused in next call
	// to the function
	//"D u m m y" is just a dummy name to make this function more general
	//as this is called to search in exe dependencies and exe itself is searched above,
	//so we need to search only dependencies.
	err = SearchSymbolByLoadordering(symInfoHeader, "D u m m y", name, searchedDlls, symAddr);
#else
	
	//search name using load ordering starting from EXE then dependencies of EXE
	//and also dependencies of dependencies.
	
	//searched dll is passed to avoid searching dlls which are searched in previous 
	//SearchSymbolByLoadordering function call i.e. this would be reused in next call
	// to the function
	TInt err = SearchSymbolByLoadordering(symInfoHeader, name, searchedDlls, symAddr);
#endif
	if( KErrNone == err )
		{
		searchedDlls.Close();
		return symAddr;
		}
	if ( KErrNoMemory == err )
		{
		searchedDlls.Close();
		SetError(KDlOpenErrNoMemory);	
		return NULL;		
		}

	LoadedDlls()->Lock();
	TInt count = LoadedDlls()->Count();
	//search all dlls opened using RTLD_GLOBAL.
	for (TInt i = 0; i < count; ++i)
		{
		TDllEntry& dllEntry = LoadedDlls()->At(i);
		
		if ( dllEntry.iFlags & RTLD_GLOBAL )
			{
			symInfoHeader = dllEntry.iSymbolHeader;
			//search by load ordering in this image
#if defined(__WINSCW__) || defined(__WINS__)
			TBuf8<KMaxFileName> fileName;
			if(CnvUtfConverter::ConvertFromUnicodeToUtf8(fileName,dllEntry.iLibrary.FileName()) != 0)
			{
			 	LoadedDlls()->UnLock();
			 	searchedDlls.Close();
			 	return NULL;
			}
			
			err = SearchSymbolByLoadordering(symInfoHeader, (char*)fileName.PtrZ(), name, searchedDlls, symAddr);
#else	
			err = SearchSymbolByLoadordering(symInfoHeader, name, searchedDlls, symAddr);
#endif				
			if ( KErrNone == err )
				{
				LoadedDlls()->UnLock();	
				searchedDlls.Close();
				return symAddr;
				}
			
			if ( KErrNoMemory == err )
				{
				LoadedDlls()->UnLock();	
				searchedDlls.Close();
				SetError(KDlSymErrNoMemory);	
				return NULL;		
				}
			}
		}
	LoadedDlls()->UnLock();	
	searchedDlls.Close();	
	//Symbol not found return NULL
	SetError(KDlSymErrNotFound);
	return NULL;	
	}
	
	
#if defined(__WINSCW__) || defined(__WINS__)

//-----------------------------------------------------------------------------
//Function Name : TInt SearchSymbolByLoadordering(TE32ExpSymInfoHdr* symInfoHeader
//							, char* aFileName, const char* aName, 
//							RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
//							void*& aSymAddress)
//Description   : To search for symbol named name in statically dependent dll 
//				  on aSymInfoHeader, it may be belonging to EXE or a DLL, also 
//				  dependencies of dependencies using Load-ordering. It also sets
//				  valid address in aSymAddress if name found, and all sll it searched in 
//				  aSearchedDlls param.	
//Return Value  : KErrNone if symbol found otherwise system wide error codes.
//-----------------------------------------------------------------------------
TInt SearchSymbolByLoadordering(const TE32ExpSymInfoHdr* symInfoHeader, const char* aFileName,
							    const char* aName,
							    RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
							    void*& aSymAddress)
	{
	//This is used as stack for Load-ordering or Depthfirst search.
	RArray<TImageData> dependentDllStack;
	//here on emulator we need two things one is Name to find symbol in that list,
	//second is TE32ExpSymInfoHdr to find dependencies
	TImageData imageData((char*)aFileName, (TE32ExpSymInfoHdr*)symInfoHeader);
	aSymAddress = NULL;
	//Add exe on the stack
	TInt err = dependentDllStack.Append(imageData);
	if ( KErrNone != err )
		{
		dependentDllStack.Close();
		return KErrNoMemory;
		}

	TE32ExpSymInfoHdr* tempSymInfoHeader = NULL;
	TInt lastIdx = 0;
	char* curDll = NULL;
	char* dependentDll = NULL;
	HMODULE handleToDll = NULL;
	TPtrC8 tempPtr;
	TBuf<KMaxFileName> dependentfileName;
	//Retunn address would be stored in this
	void* symAddr = NULL;
	//user to maintain load ordering
	RArray<TImageData> curDependentDll;
	TImageData tempImageData;
	//Depth First search for Load-ordering.
	while ( lastIdx >= 0 )
		{
		//Take first dll on the stack i.e. topmost
	    imageData = dependentDllStack[lastIdx];
	    symInfoHeader = imageData.iSymInfoHeader;
		//Remove from stack
	    dependentDllStack.Remove(lastIdx);

	    if(!symInfoHeader)
	    	{
	    	//skip non-std binaries...
		    lastIdx = dependentDllStack.Count() - 1;
	    	continue;
	    	}

		curDll = imageData.iFileName;
	 	tempPtr.Set((unsigned char*)curDll, strlen(curDll));
		dependentfileName.Copy(tempPtr);
		TParsePtr pParser(dependentfileName);
		dependentfileName = pParser.NameAndExt();
	  	//Search in this dll's symbol table	
		handleToDll = Emulator::GetModuleHandle(dependentfileName.PtrZ());
		symAddr = Emulator::GetProcAddress(handleToDll,aName);
		//Check if Symbol is found
		if( symAddr )
			{
			dependentDllStack.Close();
			aSymAddress = symAddr;
			return KErrNone;
			}
		//Add this dll to list of searched dlls
		err = aSearchedDlls.Append(symInfoHeader);
		if ( KErrNone != err )
			{
			dependentDllStack.Close();
			return KErrNoMemory;
			}
		dependentDll = (char*) symInfoHeader + sizeof(TE32ExpSymInfoHdr);
	
		//if this image is an exe - the these would be some symbols
		if ( symInfoHeader->iSymCount )
			{//skip all symbol addresse
			char* curSymbolStr = dependentDll + (KFourByteOffset * symInfoHeader->iSymCount);
			//skip all symbol names
			for (TInt i = 0; i < symInfoHeader->iSymCount; ++i )
				{
				curSymbolStr += strlen(curSymbolStr) + 1;
				}
			//initialise first dependent dll name	
			dependentDll = curSymbolStr;
			}
		//Store names of all dependent dlls of current dll/exe store there name in 
		//curDependentDll. This is done to maintain load ordering.
		for (TInt i = 0; i < symInfoHeader->iDllCount; ++i)
			{
			tempPtr.Set((unsigned char*)dependentDll, strlen(dependentDll));
			dependentfileName.Copy(tempPtr);
			handleToDll = Emulator::GetModuleHandle(dependentfileName.PtrZ());
			Emulator::TModule aModule((HINSTANCE)handleToDll);
			tempSymInfoHeader  = (TE32ExpSymInfoHdr*)aModule.Section(KWin32SectionName_NmdExpData);
			if ( tempSymInfoHeader && aSearchedDlls.Find(tempSymInfoHeader) == KErrNotFound )
				{
				tempImageData.iFileName = dependentDll;
				tempImageData.iSymInfoHeader = tempSymInfoHeader;
				err = curDependentDll.Append(tempImageData);
				if ( KErrNone != err )
					{
					dependentDllStack.Close();
					curDependentDll.Close();
					return KErrNoMemory;
					}
				}
			//advance to next dependent dll	
			dependentDll += tempPtr.Length() + 1;
			}
		//add in load order to dependentDllStack
		for (TInt i = curDependentDll.Count() - 1; i >= 0; --i)
			{
			err = dependentDllStack.Append(curDependentDll[i]);
			if ( KErrNone != err )
				{
				dependentDllStack.Close();
				curDependentDll.Close();
				return KErrNoMemory;
				}
			}
		curDependentDll.Close();	
		lastIdx = dependentDllStack.Count() - 1;	
		}
	dependentDllStack.Close();
	return KErrNotFound;
	}
	
//-----------------------------------------------------------------------------
//Function Name : void* DlSymByDependencyOrdering(const void* aHandle,
//													const char* aName)
//Description   : To search for symbol named name in statically dependent dll 
//				  on aSymInfoHeader, is dependencies, also dependencies of 
//				  dependencies using Dependency-ordering. 
//Return Value  : Valid address if name found otherwise NULL
//-----------------------------------------------------------------------------
void* DlSymByDependencyOrdering(const void* aHandle,const char* aName)
	{
	//Get 0th ordinal datastructure of dll denoted by handle.
	LoadedDlls()->Lock();
	TInt idx = LoadedDlls()->Find(aHandle);
	//check if handle not found	
	if ( KErrNotFound == idx )
		{
		LoadedDlls()->UnLock();
		SetError(KDlSymErrBadHandle);
		return NULL;
		}
	TDllEntry& dllEntry = LoadedDlls()->At(idx);
	
	// The below 'if' condition prevents the symbol lookup on non-STDDLL
	// Check for the symbol information, if not found
	// return with KDlSymErrNoSupport 
	if( !dllEntry.iSymbolHeader )
		{
		LoadedDlls()->UnLock();
		SetError(KDlSymErrNoSupport);
		return NULL;
		}

	TBuf8<KMaxFileName> fileName;
	if(CnvUtfConverter::ConvertFromUnicodeToUtf8(fileName,dllEntry.iLibrary.FileName()) != 0)
		{
		LoadedDlls()->UnLock();
		return NULL;
		}
	TE32ExpSymInfoHdr* symInfoHeader = dllEntry.iSymbolHeader;
	//Queue for using Breadthfirst search / Dependency ordering.
	RArray<TImageData> dependentDllQue;
	TImageData imageData((char*)fileName.PtrZ(), symInfoHeader);
	//add first item in queue
	TInt err = dependentDllQue.Append(imageData);
	if ( KErrNone != err )
		{
		LoadedDlls()->UnLock();
		dependentDllQue.Close();
		SetError(KDlSymErrNoMemory);	
		return NULL;		
		}
	char* curDll = NULL;
	char* dependentDll = NULL;
	HMODULE handleToDll = NULL;
	TBuf<KMaxFileName> dependentfileName;	
	TE32ExpSymInfoHdr* tempSymInfoHeader = NULL;
	void* symAddr = NULL;
	TPtrC8 tempPtr;
	//Array of searched dlls. Used to check circular dependency
	//this is array of pointer to name of dll in image
	RPointerArray<TE32ExpSymInfoHdr> searchedDlls;
	//Breath First search for Dependancy ordering.
	while(dependentDllQue.Count())
		{
		imageData = dependentDllQue[0];
		curDll = imageData.iFileName;
		symInfoHeader = imageData.iSymInfoHeader;
		dependentDllQue.Remove(0);
		
		// The below 'if' condition prevents the symbol lookup on dependent non-STDDLL
		if(!symInfoHeader)
			{
			continue;
			}

		tempPtr.Set((unsigned char*)curDll, strlen(curDll));
		dependentfileName.Copy(tempPtr);
		TParsePtr pParser(dependentfileName);
		dependentfileName = pParser.NameAndExt();
	  	//Search in this dll's symbol table	
		handleToDll = Emulator::GetModuleHandle(dependentfileName.PtrZ());
		symAddr = Emulator::GetProcAddress(handleToDll,aName);
		//check if symbol is found
		if( symAddr )
			{
			LoadedDlls()->UnLock();
			dependentDllQue.Close();
			searchedDlls.Close();
			return symAddr;
			}
		//Insert this to searched list
		err = searchedDlls.Append(symInfoHeader);
		if ( KErrNone != err )
			{
			LoadedDlls()->UnLock();
			dependentDllQue.Close();
			searchedDlls.Close();	
			SetError(KDlSymErrNoMemory);	
			return NULL;		
			}
		//Add list of dependencies
		dependentDll = (char*) symInfoHeader + sizeof(TE32ExpSymInfoHdr);

		//Add at last to make it Queue, needed for Dependency ordering
		for (TInt i = 0; i <  symInfoHeader->iDllCount; i++)
			{
			tempPtr.Set((unsigned char*)dependentDll, strlen(dependentDll));
			dependentfileName.Copy(tempPtr);
			//get i'th dependency
			handleToDll = Emulator::GetModuleHandle(dependentfileName.PtrZ());
			Emulator::TModule aModule((HINSTANCE)handleToDll);
			tempSymInfoHeader  = (TE32ExpSymInfoHdr*)aModule.Section(KWin32SectionName_NmdExpData);
			//check i'th dependency is OE dll e.i. equal to zero or not
			//and also its not already searched
			if ( tempSymInfoHeader && searchedDlls.Find(tempSymInfoHeader) == KErrNotFound )
				{
				imageData.iFileName = dependentDll;
				imageData.iSymInfoHeader = tempSymInfoHeader;
				err = dependentDllQue.Append(imageData);
				if ( KErrNone  != err )
					{
					LoadedDlls()->UnLock();
					dependentDllQue.Close();
					searchedDlls.Close();
					SetError(KDlSymErrNoMemory);	
					return NULL;		
					}
				}
			//advance to next dependent dll	
			dependentDll += tempPtr.Length() + 1;
			}
		}
	LoadedDlls()->UnLock();	
	dependentDllQue.Close();
	searchedDlls.Close();
	//Symbol not found return NULL
	SetError(KDlSymErrNotFound);
	return NULL;	
	}

//-----------------------------------------------------------------------------
//Function Name : TInt SearchNameInExe(const TE32ExpSymInfoHdr* aSymInfoHeader,
//										 const char* aName, void*& aSymAddress)
//Description   : To search for symbol named name in EXE. If symbol is found
//				  address is set to aSymAddress.
//Return Value  : KErrNone if symbol found otherwise system wide error codes.
//-----------------------------------------------------------------------------
TInt SearchNameInExe(const TE32ExpSymInfoHdr* aSymInfoHeader,
					 const char* aName, void*& aSymAddress)
	{
	//no of symbols in this exe
	TInt symCount = aSymInfoHeader->iSymCount;
	//descriptor for symbol name
	TPtrC8 tempName;
	TPtrC8 pName((unsigned char*)aName);
	//calculate first string address i.e. skip symbol addresses and structure itself
	char* tempstring = (char*)aSymInfoHeader + sizeof(TE32ExpSymInfoHdr) + (KFourByteOffset * symCount);
	//symbol are continuous set of NULL terminated strings
	//so search one by one
	for (TInt i = 0; i < symCount; ++i)
		{
		tempName.Set((unsigned char*)tempstring,  strlen(tempstring));
		//if symbol found
		if ( !pName.Compare(tempName) )
			{
			aSymAddress = *( (void**)( (TUint8*)aSymInfoHeader + sizeof(TE32ExpSymInfoHdr) + (KFourByteOffset * i)));
			return KErrNone;
			}
		//advance to next symbol
		tempstring += tempName.Length() + 1;	
		}
	return KErrNotFound;		
	}
	
#else // ARMCC

//-----------------------------------------------------------------------------
//Function Name : TInt SearchSymbolByLoadordering(TE32ExpSymInfoHdr* symInfoHeader
//							, char* aFileName, const char* aName, 
//							RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
//							void*& aSymAddress)
//Description   : To search for Symbol named name in statically dependent dll 
//				  on aSymInfoHeader, it may be belonging to EXE or a DLL, also 
//				  dependencies of dependencies using Load-ordering. It also sets
//				  valid address in aSymAddress if name found, and all sll it searched in 
//				  aSearchedDlls param.	
//Return Value  : KErrNone if symbol found otherwise system wide error codes.
//-----------------------------------------------------------------------------
TInt SearchSymbolByLoadordering(const TE32ExpSymInfoHdr* aSymInfoHeader,
								const char* aName, 
								RPointerArray<TE32ExpSymInfoHdr>& aSearchedDlls,
								void*& aSymAddress)
	{
	//This is used as stack for Load-ordering or Depthfirst search.
	RPointerArray<TE32ExpSymInfoHdr> dependentDllStack;
	aSymAddress = NULL;
	//Add exe on the stack
	TInt err = dependentDllStack.Append(aSymInfoHeader);
	if ( KErrNone != err )
		{
		dependentDllStack.Close();
		return KErrNoMemory;
		}
	TE32ExpSymInfoHdr* symInfoHeader;
	//temporary pointer to 
	TUint8* dependencyTableOffset;
	TE32ExpSymInfoHdr* tempSymInfoHeader = NULL;
	TInt lastIdx = 0;
	//Retunn address would be stored in this
	void* symAddr = NULL;
	//Depth First search for Load-ordering. Intentional use of "="
	while ( lastIdx >= 0 )
		{
		//Take first dll on the stack i.e. topmost
	    symInfoHeader = dependentDllStack[lastIdx];
	    //Remove from stack
	    dependentDllStack.Remove(lastIdx);
	    
	    if(!symInfoHeader)
	    	{
	    	//skip non-std binaries...
	    	lastIdx = dependentDllStack.Count() - 1;
	    	continue;
	    	}
	    
		//Search in current dll Symbol table.	
	    symAddr = SearchName(symInfoHeader, aName);
		//Check if Symbol is found
		if( symAddr )
			{
			dependentDllStack.Close();
			aSymAddress = symAddr;
			return KErrNone;
			}
		//Add this dll to list of searched dlls
		err = aSearchedDlls.Append(symInfoHeader);
		if ( KErrNone != err )
			{
			dependentDllStack.Close();
			return KErrNoMemory;
			}
		
		dependencyTableOffset = (TUint8*) symInfoHeader + symInfoHeader->iDepDllZeroOrdTableOffset;
		//All all the dependent dll for current dll/exe
		for (TInt i = symInfoHeader->iDllCount - 1; i >= 0; --i)
			{
			tempSymInfoHeader = *((TE32ExpSymInfoHdr**) (dependencyTableOffset + (KFourByteOffset * i)));
			if ( tempSymInfoHeader && aSearchedDlls.Find(tempSymInfoHeader) == KErrNotFound )
				{
				err = dependentDllStack.Append(tempSymInfoHeader);
				if ( KErrNone != err )
					{
					dependentDllStack.Close();
					return KErrNoMemory;
					}
				}
			}
		lastIdx = dependentDllStack.Count() - 1;	
		}
	dependentDllStack.Close();
	return KErrNotFound;
	}

//-----------------------------------------------------------------------------
//Function Name : void* SearchName(const TE32ExpSymInfoHdr* aSymInfoHeader,
//										 const char* aName)
//Description   : To search symbol name in symbol table of the dll.
//Return Value  : Valid address if name found otherwise NULL
//-----------------------------------------------------------------------------
void* SearchName(const TE32ExpSymInfoHdr* aSymInfoHeader, const char* aName)
	{
	//Size of Offset used by Symbol Table, it depends upon iFlags of E32EpocExpSymInfoHdr
	TInt sizeOfStringTableOffset = KTwoByteOffset;    
	// Check size of symbol table offset
	if ( aSymInfoHeader->iFlags )		
		{
		sizeOfStringTableOffset =  KFourByteOffset;
		}
	//Total no of Symbol in a Symbol table
	TInt noOfSymbols = aSymInfoHeader->iSymCount;       
	TInt start = 0;
	TInt mid;
	TInt end = noOfSymbols-1;
	TInt temp = 0 ;
	TInt offsetToString = 0;
	TPtrC8 tempName;
	TPtrC8 pName;
	pName.Set((unsigned char*)aName, strlen(aName));
	TUint8* tempSymbolOffset = (TUint8*) aSymInfoHeader+ aSymInfoHeader->iSymbolTblOffset + noOfSymbols * sizeof(TInt);
	TUint8* tempStringTableOffset = (TUint8*) aSymInfoHeader + aSymInfoHeader->iStringTableOffset;
	//Binary Search for searching name in symbol table
	while (start <= end) 
		{
		mid = (start + end) / 2;
		if ( sizeOfStringTableOffset == KTwoByteOffset )		
			{
			offsetToString = *(unsigned short int*)(tempSymbolOffset + (sizeOfStringTableOffset * mid));
			}
		else
			{
			offsetToString = *(unsigned int*)(tempSymbolOffset + (sizeOfStringTableOffset * mid));
			}
		//need to shift 2 bit to left for alignment reasons
		offsetToString <<= 2;
		tempName.Set((unsigned char*)(tempStringTableOffset + offsetToString),  strlen((char*)tempStringTableOffset + offsetToString));
		temp = pName.Compare(tempName);
		//if found return address of symbol
		if( !temp )
			{
			return *( (void**)( (TUint8*)aSymInfoHeader+ aSymInfoHeader->iSymbolTblOffset + mid * sizeof(TInt)));
			}
		//update start/end	
		if ( temp < 0 )
			{
			end = mid -1;
			}
		else
			{
			start = mid +1;
			}
		}
	return NULL;
	}

//-----------------------------------------------------------------------------
//Function Name : void* DlSymByDependencyOrdering(const void* aHandle,
//													const char* aName)
//Description   : To search for Symbol named name in statically dependent dll 
//				  on aSymInfoHeader, is dependencies, also dependencies of 
//				  dependencies using Dependency-ordering. 
//Return Value  : Valid address if name found otherwise NULL
//-----------------------------------------------------------------------------
void* DlSymByDependencyOrdering(const void* aHandle,const char* aName)
	{
	
	//Get 0th ordinal datastructure of dll denoted by handle.
	LoadedDlls()->Lock();
	TInt idx = LoadedDlls()->Find(aHandle);
	//check if handle not found	
	if ( KErrNotFound == idx )
		{
		LoadedDlls()->UnLock();
		SetError(KDlSymErrBadHandle);
		return NULL;
		}
	TDllEntry& dllEntry = LoadedDlls()->At(idx);
	TE32ExpSymInfoHdr* symInfoHeader = dllEntry.iSymbolHeader;
	
	// The below 'if' condition prevents the symbol lookup on non-STDDLL
	// Check for the symbol information, if not found
	// return with KDlSymErrNoSupport 
	if( !dllEntry.iSymbolHeader )
		{
		LoadedDlls()->UnLock();
		SetError(KDlSymErrNoSupport);
		return NULL;
		}

	
	TBuf8<KMaxFileName> fileName;
	fileName.Copy(dllEntry.iLibrary.FileName());
	//Queue for using Breadthfirst search / Dependency ordering.
	RPointerArray<TE32ExpSymInfoHdr> dependentDllQue;
	//add first item in queue
	TInt err = dependentDllQue.Append(symInfoHeader);
	if ( KErrNone != err )
		{
		LoadedDlls()->UnLock();
		dependentDllQue.Close();
		SetError(KDlSymErrNoMemory);	
		return NULL;		
		}
	TUint8* dependencyTableOffset;
	TE32ExpSymInfoHdr* tempSymInfoHeader = NULL;
	//Array of searched dlls. Used to check circular dependency
	RPointerArray<TE32ExpSymInfoHdr> searchedDlls;
	void* symAddr = NULL;
	//Breath First search for Dependancy ordering.
	while(dependentDllQue.Count())
		{
		symInfoHeader = dependentDllQue[0];
		dependentDllQue.Remove(0);
		
		// The below 'if' condition prevents the symbol lookup on dependent non-STDDLL
		if(!symInfoHeader)
			{
			continue;
			}
		
	  	//Search in this dll's symbol table	
	    symAddr = SearchName(symInfoHeader, aName);
		if( symAddr )
			{
			LoadedDlls()->UnLock();
			dependentDllQue.Close();
			searchedDlls.Close();
			return symAddr;
			}
		//Insert this to searched list
		err = searchedDlls.Append(symInfoHeader);
		if ( KErrNone != err )
			{
			LoadedDlls()->UnLock();
			dependentDllQue.Close();
			searchedDlls.Close();	
			SetError(KDlSymErrNoMemory);	
			return NULL;		
			}
		//Add list of dependencies
	    dependencyTableOffset = (TUint8*) symInfoHeader + symInfoHeader->iDepDllZeroOrdTableOffset;
		//Add at last to make it Queue, needed for Dependency ordering
		for (TInt i = 0; i <  symInfoHeader->iDllCount; i++)
			{
			//get i'th dependency
			tempSymInfoHeader = *( (TE32ExpSymInfoHdr**) (dependencyTableOffset + (KFourByteOffset*i)));
			//check i'th dependency is OE dll e.i. equal to zero or not
			//and also its not already searched
			if ( tempSymInfoHeader && searchedDlls.Find(tempSymInfoHeader) == KErrNotFound )
				{
				err = dependentDllQue.Append(tempSymInfoHeader);
				if ( KErrNone  != err )
					{
					LoadedDlls()->UnLock();
					dependentDllQue.Close();
					searchedDlls.Close();
					SetError(KDlSymErrNoMemory);	
					return NULL;		
					}
				}
			}
		}
	LoadedDlls()->UnLock();	
	dependentDllQue.Close();
	searchedDlls.Close();
	//Symbol not found return NULL
	SetError(KDlSymErrNotFound);
	return NULL;
	}
	
#endif//defined(__WINSCW__) || defined(__WINS__)

//-----------------------------------------------------------------------------
//Function Name : void SetError(const TInt aError)
//Description   : To set error char* in TLD.
//Return Value  : void
//-----------------------------------------------------------------------------
void SetError(const char* aErrString)
	{
	//set string pointer correspoinding to aError in Tls.
	Dll::SetTls((void *)aErrString);
	}
