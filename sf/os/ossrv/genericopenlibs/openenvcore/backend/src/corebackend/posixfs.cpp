/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <utime.h>
#include "sysif.h"
#include "fdesc.h"
#include "ltime.h"
#include "lposix.h"
#include "systemspecialfilercg.h"
#include "link.h"


wchar_t * PosixFilesystem::getcwd (RFs& aFs, wchar_t* buf, unsigned long len, int& anErrno)
	{
	TFullName name;
	TInt err = aFs.SessionPath(name);
	if (!err)
		{
		TPtr16 pathdes((TText16 *)buf, len);
		if (pathdes.MaxLength() >= (name.Length() + 1))	//+1 to allow for the null terminator
			{
			pathdes.Copy(name);
			pathdes.ZeroTerminate();
			return buf;
			}
		else
			{
			err = ERANGE;		//out of range
			}	
		}
	MapError(err, anErrno);
	return 0;
	}

int PosixFilesystem::chdir (RFs& aFs, const wchar_t* aPath, int& anErrno)
	{
	TParse name;
	TInt err=GetFullPath(name, (const TText16 *)aPath, NULL);
	if (!err)
		{
		TPtrC path=name.DriveAndPath();
		TUint att=0;
		if (path.Length()==3)	// Bug in F32 - the root directory has no attributes
			att=KEntryAttDir;
		else
			err=aFs.Att(path, att);
			
		if(err == KErrPathNotFound)	
			{
			anErrno = Find_Error(aPath, aFs);
			return -1;
			}
			
		// KErrNotReady, here typically means filesystem not mounted or the mentioned
		// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
		// posix error codes, ENOENT would be the appropriate error to return in this scenario.
		if (err == KErrNotReady)
			{
			anErrno = ENOENT;
			return -1;
			}	
			
		if (!err)
			{
			if (att&KEntryAttDir)
				{
				err = aFs.SetSessionPath(path);
				//Cache the new session path in the backend
				if(err == KErrNone)
					{
					err = Backend()->SaveSessionPath(path);
					}
				}
			else
				err=ENOTDIR;
			}
		}
	return MapError(err,anErrno);
	}

int PosixFilesystem::rmdir (RFs& aFs, const wchar_t* aPath, int& anErrno)
	{
	TParse name;
	TInt err=GetFullPath(name, (const TText16 *)aPath, NULL);
	if (!err)
		{
		TPtrC path=name.DriveAndPath();
		TUint att=0;
		if (path.Length()==3)
			err=EPERM;	// no, you may not remove the root directory
		else
			err=aFs.Att(path, att);
		if(err == KErrPathNotFound)
			{
			TFileName val = name.DriveAndPath();
			val.ZeroTerminate();
			// find_error() with parameter TFileName is called only because to avoid getfullfile()
			anErrno = Find_Error(val , aFs) ;
			return  -1 ;
			}
			
		// KErrNotReady, here typically means filesystem not mounted or the mentioned
		// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
		// posix error codes, ENOENT would be the appropriate error to return in this scenario.
		if (err == KErrNotReady)
			{
			anErrno = ENOENT;
			return -1;
			}	
							
		if (!err)
			if (att&KEntryAttDir)
				{
				err=aFs.RmDir(path);
				if (err==KErrInUse)
					err=ENOTEMPTY;	// i.e. directory not empty
				}
			else
				err=ENOTDIR; 
		}
		
	return MapError(err,anErrno);
	}

int PosixFilesystem::mkdir (RFs& aFs, const wchar_t* aPath, int perms, int& anErrno)
	{
	TParse name;
	TInt err = 0;
	
		
	switch(perms & (S_IWUSR | S_IRUSR )) 
		{
		case S_IRUSR | S_IWUSR :
		case S_IWUSR :
		case S_IRUSR :
			{
			break ;	
			}
		default :
			{
			err = KErrArgument ;
			return MapError(err,anErrno);
			}
		}

		err = GetFullPath(name, (const TText16 *)aPath, NULL);
		
		if (!err)
			{
			TFileName pvtPath;
			TPtrC path=name.DriveAndPath();	
			TPtrC pth=name.Path();
			aFs.PrivatePath(pvtPath);

			//Checks for the presence of Private Path, if present then creates only the private path
			if (pth.FindF(pvtPath) == 0)
				{
				TEntry ent; 
				TInt ret;
				//returns in the format Drive:
		 		TPtrC ptr = name.Drive();
		 		TFileName drvandpath;
		 		drvandpath.Copy(ptr);
		 		drvandpath.Append(pvtPath);
				ret = aFs.Entry(drvandpath , ent);
				if ( ret == KErrNotFound) 
 					{	
					TInt driveNumber;
					aFs.CharToDrive((TUint)ptr[0], driveNumber);
					//creates the private path
					TInt val;
					val = aFs.CreatePrivatePath(driveNumber);
					if(val != KErrNone)
						{
						if (val == KErrNotReady)
							{
							anErrno = ENOENT;
							return -1;
							}
						return MapError(val,anErrno);
						}
					}
      			else
      				{
      				if(ret == KErrNone)
      					{
      					if(pvtPath.Length() == pth.Length())
		      				{
		      				anErrno = EEXIST ;
				 			return  -1  ;
		      				}
      					}
      				else
      					{
      					if (ret == KErrNotReady)
							{
							anErrno = ENOENT;
							return -1;
							}
      					return  MapError ( ret , anErrno) ;
      					}
      				}
      			//Creates the directory inside the private path
      			if(pvtPath.Length() != pth.Length())
      				{
      				err=aFs.MkDir(path);
      				}
      			}
			else
      			{
      			err=aFs.MkDir(path);
      			}

			//if entry  is a existing file and is present then symbian returns KErrAccessDenied
			if(err == KErrAccessDenied) 
				{
				TEntry Ent ;
				if((aFs.Entry(path , Ent )) != KErrNone) 
					{
					return  MapError ( err , anErrno) ;
					}
				 anErrno = EEXIST ;
				 return  -1  ;
				 } 
				 
			if (err == KErrPathNotFound)
				{	   
				anErrno = Find_Error(aPath , aFs) ;
				return -1 ;
				} 
				
			// KErrNotReady, here typically means filesystem not mounted or the mentioned
			// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
			// posix error codes, ENOENT would be the appropriate error to return in this scenario.
			if (err == KErrNotReady)
				{
				anErrno = ENOENT;
				return -1;
				}
					  
			if (!err)
				{
				if ((perms&S_IWUSR)==0)
				err=aFs.SetAtt(path,KEntryAttReadOnly,0);
				}
			}
			
		return MapError(err,anErrno);
			}

int PosixFilesystem::statbackend (RFs& aFs, const wchar_t* name, struct stat *st, int& anErrno)
	{
	TFullName fullName;
	TInt err=0;
	if(!name || *name == L'\0') //st is NULL check can also be added
		{
		err = KErrArgument;
		}
	else
		{
		err = GetFullFile(fullName,(const TText16*)name,aFs);	
		}
	
	if (!err)
		{
		TEntry entry;
	    if (fullName.Length()==3)
			{
			TDriveList aList;
			aFs.DriveList(aList);
			
			if(0 != aList[(dev_t)TDriveUnit(fullName)])
				{
				entry.iAtt=KEntryAttDir;
				entry.iModified=TTime(0);
				TDriveInfo info;
				if( (err = aFs.Drive(info,(dev_t)TDriveUnit(fullName))) == KErrNone)
				    {
				    if(info.iType == EMediaNotPresent)
                    			{
			                err = KErrNotReady;
			                }
				    }
				}
			else
				{
				err = KErrNotReady;
				}
			}
		else
			{
			err=aFs.Entry(fullName,entry);
			}
			
		if (!err)
			{
			memset(st,0,sizeof(struct stat));
			
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
			st->st_size = entry.FileSize();
#else
			st->st_size = entry.iSize;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
			
			st->st_dev = st->st_rdev = (dev_t)TDriveUnit(fullName);
			if( ( (entry.iAtt & (KEntryAttHidden | KEntryAttSystem)) != (KEntryAttHidden | KEntryAttSystem) )
			        || (entry.iAtt & KEntryAttDir))
			    {
			    CFileDesc::MapStat(*st, entry.iModified, entry.iAtt);
                }
            else
                {
                mode_t fileMode = S_IFREG;
                TSpecialFileType fileType;
                fileType = _SystemSpecialFileBasedFilePath(name, err, aFs);
                switch(fileType)
                    {
                    case EFileTypeSymLink:
                        fileMode = S_IFLNK;
                        break;
                    case EFileTypeMkFifo:
                        fileMode = S_IFIFO;
                        break;
                    case EFileTypeGeneral:
                        break;
                    default:
                        return MapError(err, anErrno);
                    }
                    CFileDesc::MapStat(*st, entry.iModified, entry.iAtt, fileMode);
                }
			return 0;
			}
		}
	
	if(err == KErrPathNotFound )
		{
	 	anErrno = ENOENT;
	 	return -1 ;
	 	}
	 	
	// KErrNotReady, here typically means filesystem not mounted or the mentioned
	// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
	// posix error codes, ENOENT would be the appropriate error to return in this scenario.
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}
			 	
	return MapError(err, anErrno);
	}
	
int PosixFilesystem::chmod (RFs& aFs, const wchar_t* name, int perms, int& anErrno)
	{
	TFullName fullName;
	TInt err = 0;
	if(!name || *name == L'\0')
		{
		return MapError(KErrArgument,anErrno);
		}
	
	err = GetFullFile(fullName,(const TText16*)name,aFs);	
	TParse path;
	path.Set(fullName,NULL,NULL);		
	TPtrC pth = path.Path();
	TFileName pvtPath;
	//the below code is removed from getfullfile and explicitly added here so that creation of private path unnecessarily could be avoided while calling getfullfile()
	aFs.PrivatePath(pvtPath);
	if (!(pth.CompareF(pvtPath)))
		{
		TEntry ent; 
		TFileName Pvt;
		//returns in the format Drive:
		TPtrC ptr = path.Drive();
		Pvt.Copy(ptr);
		Pvt.Append(pvtPath);	
		if (aFs.Entry(Pvt , ent) == KErrNotFound) 
			{
	     	TInt ret = 0;
	     	TInt driveNumber;
			aFs.CharToDrive((TUint)ptr[0], driveNumber);
			//creates the private path
			ret = aFs.CreatePrivatePath(driveNumber);
      		if ((ret == KErrAlreadyExists) || (ret == KErrNone))
      			{
      			err = 0;
      			}
      		else
	      		{
	      		err = ret;
	      		}
			}
		}	
	if(!err) 
		{
		switch(perms & (S_IWUSR | S_IRUSR )) 
			{
			case S_IRUSR | S_IWUSR :
			case S_IWUSR :
				{
				err = aFs.SetAtt(fullName , 0 , KEntryAttReadOnly) ;
				break ;
				}
			case S_IRUSR :
				{
				err = aFs.SetAtt(fullName , KEntryAttReadOnly , 0 ) ;
				break ;	
				}
			default :
				{
				err = KErrArgument ;
				break ;
				}
			}
		}

	// KErrNotReady, here typically means filesystem not mounted or the mentioned
	// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
	// posix error codes, ENOENT would be the appropriate error to return in this scenario.
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}
		
	return MapError(err, anErrno);
	}

int PosixFilesystem::utime (RFs& aFs, const wchar_t* name, const struct utimbuf* filetimes, int& anErrno)
	{
	TFullName fullName;
	TTime iModified(MAKE_TINT64(0x00dcddb3,0x0f2f8000)) ;   // 00:00, Jan 1st 1970
	TInt err=GetFullFile(fullName,(const TText16*)name,aFs);
	if (!err)
		{
		if (filetimes==NULL)
			{
			iModified.UniversalTime();
			err=aFs.SetModified(fullName,iModified);
			}
		else
			{
			iModified+=(TTimeIntervalSeconds)filetimes->modtime;
			err=aFs.SetModified(fullName,iModified);
			}
		}
		
	if(err == KErrPathNotFound)
		{
		anErrno = Find_Error(fullName , aFs) ;
		return -1 ;
		}	
			
	// KErrNotReady, here typically means filesystem not mounted or the mentioned
	// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
	// posix error codes, ENOENT would be the appropriate error to return in this scenario.
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}			
		
	return MapError(err, anErrno);
	}

int PosixFilesystem::reg_unlink (RFs& aFs, const wchar_t* name, int& anErrno)
	{
	TFullName fullName;
	TInt err=GetFullFile(fullName, (TText16*)name, aFs);
	if (!err)
		{
		TUint att=0;
		err=aFs.Att(fullName, att);
	    
	    if(!err) 
	    	{
    		if(att & KEntryAttReadOnly) 
    	 		{
    	 		err = aFs.SetAtt(fullName , 0 , KEntryAttReadOnly) ;
    	 		}
    	 		
    	 	else if (att & KEntryAttDir)
    	 		{
    	 		err = EPERM ;
    	 		}
	    	}
        if(!err)
        	{
        	err = aFs.Delete(fullName) ;
        	}
		}
		
	// KErrNotReady, here typically means filesystem not mounted or the mentioned
	// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
	// posix error codes, ENOENT would be the appropriate error to return in this scenario.
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}
				
	return MapError(err, anErrno);
	}

int PosixFilesystem::rename (RFs& aFs, const wchar_t* oldname, const wchar_t* newname, int& anErrno)
	{
	TFileName oldFullName , NewFullName;
	TInt path ;
	TInt err ;
	
	err = GetFullFile(oldFullName,(const TText16 *)oldname,aFs);

	if(err != KErrNone)
		{
		return MapError(err , anErrno) ;
		}
		
	err = GetFullFile(NewFullName ,(const TText16 *)newname , aFs) ;
	if(err != KErrNone)
		{
		return MapError(err , anErrno) ;
		}
	
	
	err=aFs.Rename(oldFullName,NewFullName);
	
	if(err)
		{
		
		if(err == KErrPermissionDenied)
			{
			return MapError(err , anErrno) ;
			}
		if(err == KErrAccessDenied)
			{
			return MapError(err , anErrno) ;
			}
		
		 path = Get_PathCombinations(oldFullName, NewFullName  , aFs) ;
		 
		 switch(path)
		 	{
		 	case Permission_Denied:
		 		{
		 		anErrno = EACCES;
		 		return -1;
		 		}
		 	case DirNotEmpty_DirEmpty :
		 	case DirEmpty_DirEmpty:
		 		{
		 	   if(oldFullName.Find(NewFullName) == 0 ) //Renaming parent directory in parent directoy...
			     {
			     anErrno = EINVAL ;
			     return -1 ;
			     }
	
		    if(NewFullName.Find(oldFullName) == 0 ) //Renaming parent directory in parent directoy...
			    {
			    anErrno = EINVAL ;
			     return -1 ;
			    }	
		 		TParse ParseObj ;
		 		
		 		err = GetFullPath(ParseObj, (const TText16 *)newname, NULL) ;
		 		
		 		if(err)
		 			{
		 			return MapError(err , anErrno) ;
		 			}
		 			
		 		TPtrC NewPrs = ParseObj.DriveAndPath() ;
                err= aFs.RmDir(NewPrs) ;
		 		
		 		if(err )  
		 			{
					if (err == KErrNotReady)
						{
						anErrno = ENOENT;
						return -1;
						}
		 		  
		 		  	return MapError(err , anErrno) ;
		 		  	} 
		 		break ;
		 		} 
		 		
		 	
		 	case DirEmpty_DirNotEmpty:
   		 		{
   		 		anErrno = ENOTEMPTY ;
		 		return -1 ;
		 		}
		 	/*
		 	case DirEmpty_File :
		 		{
		 		anErrno = ENOTDIR ;
		 		return -1 ;
		 		}
		 	*/	
		 	
		 	case DirNotEmpty_DirNotEmpty :	 		
		 		{
		 		 anErrno = ENOTEMPTY ;
		 		 return -1 ;
		 		}
		 	/*	
		 	case DirNotEmpty_File :
		 		{
		 		anErrno = ENOTDIR ;
		 		return -1 ;
		 	    }
		 	    
		 	 case File_DirEmpty :
		 	    {
		 	     anErrno = EISDIR ;
		 	     return -1 ;
		 	    }
		 	    
		 	 case File_DirNotEmpty :
		 	 	{
		 	 	anErrno = EISDIR ;
		 	 	return -1 ;
		 	 	}
		 	 	*/
		 	 case File_File :
		 	    {
		 	    // Renaming a link file...?
				TSpecialFileType fileType = _SystemSpecialFileBasedFilePath(oldname, err,Backend()->FileSession());
				if(fileType == EFileTypeSymLink)
				{
 		 	    	struct SLinkInfo enBuf;
 					//CFileDescBase *f = NULL;
 				    
 				    err = _ReadSysSplFile(oldname, (char*)&enBuf, sizeof(struct SLinkInfo), anErrno, aFs);
 					
					if(err == KErrNone)
						{
 						TFullName Link_File_Ptr;
 						err = GetFullFile(Link_File_Ptr,(const TText16*)enBuf.iParentPath,aFs);
 						
 						if( err != KErrNone)
 							{
 							if(KErrPathNotFound == err || KErrNotFound == err)
 								{
 									//If the parent file does not exist, remove the link file also
 								err = _DeleteSystemSpecialFileBasedFilePath(oldname, aFs);
 								MapError(err, anErrno);
 								}
 		 	    			}
 		 	    		
 		 	    		if(Link_File_Ptr == NewFullName)   
 		 	    			{//Renaming same files 
 		 	    			 return 0;
 		 	    			}
 		 	    		}
 		 	    		
 		 	    	}
 		 	    else if(_SystemSpecialFileBasedFilePath(newname, err,Backend()->FileSession()) == EFileTypeSymLink)
 		 	    	{
 		 	    	struct SLinkInfo enBuf;
 					//CFileDescBase *f = NULL;
 				    
 				    err = _ReadSysSplFile(newname, (char*)&enBuf, sizeof(struct SLinkInfo), anErrno, aFs);
					
 					if(err == KErrNone)
 						{
 						TFullName Link_File_Ptr;
 						err = GetFullFile(Link_File_Ptr,(const TText16*)enBuf.iParentPath,aFs);
 						
 						if( err != KErrNone)
 							{
 							if(KErrPathNotFound == err || KErrNotFound == err)
 								{
 									//If the parent file does not exist, remove the link file also
								err = _DeleteSystemSpecialFileBasedFilePath(oldname, aFs);
 								MapError(err, anErrno);
 								}
 		 	    			}
		 	    		if(Link_File_Ptr == oldFullName)   
 		 	    			{//Renaming same files 
		 	    			 return 0;
 		 	    			}
 		 	    		}
 		 	    	}	
		 	    TEntry Ent ;
				err = aFs.Entry(NewFullName , Ent) ;
				
				 if(!err) 
				 	{
				 	if(Ent.IsReadOnly()) 
				 		{
				 		aFs.SetAtt(NewFullName , 0 , KEntryAttReadOnly) ;
				 		}
				 	err = aFs.Delete(NewFullName);
				 	
				 	if(err)
				 		{
						if (err == KErrNotReady)
							{
							anErrno = ENOENT;
							return -1;
							}
					 		
				 		return MapError(err , anErrno) ;
				 		}
		 	 	    }
		 	 	    
				if (err == KErrNotReady)
					{
					anErrno = ENOENT;
					return -1;
					}		 	 	    
					
		 	 	    break ;
		 	 	   } 
		 	   
		 	 case File_Dir :
		 	 	{
		 	 	anErrno = EISDIR ;
		 	 	return -1 ;
		 	 	}
		 	 	
		 	 case Dir_File :
		 	 	{
		 	 	anErrno = ENOTDIR ;
		 	 	return -1 ;
		 	    }	
		 	 		  
		 	  case Invalid_Path_EnotDir :
		 	     {
		 	     anErrno = ENOTDIR ;
		 	     return -1 ;
		 	     }  
		 	  
		 	  case Invalid_Path_Eent :
		 	  	{
		 	  	anErrno = ENOENT ;
		 	  	return -1 ;
		 	  	} 
		 	     	
		 	 default :
		 	    {
		 	     anErrno = EINVAL ;
		 	     return -1 ;
		 	     }
		 	      
			} 	 //End of switch 	    
		 	err=aFs.Rename(oldFullName,NewFullName);  
			if (err == KErrNotReady)
				{
				anErrno = ENOENT;
				return -1;
				}
		 	 
		 	 return MapError(err , anErrno) ;
		}  //End of if 	   	
		
	return KErrNone;
	}

TInt PosixFilesystem::ResolvePath(TParse& aResult, const wchar_t* path, TDes* aFilename)
	{
	return GetFullPath(aResult, (const TText16*)path, aFilename);
	}


#ifdef __WINS__
TInt PosixFilesystem::SetDefaultDir (RFs& /*aFs*/)
    {
    // NB. don't do this on WINS because the executable is
    // something like w:\epoc32\release\wins\deb\mytest.exe (or just mytest.exe or
    // even ./mytest !!)
    return KErrNone;
    }
#else
TInt PosixFilesystem::SetDefaultDir (RFs& aFs)
    {
    TParsePtrC parse(RProcess().FileName());
    TInt drv = TDriveUnit(parse.Drive());
    // Create the pvt path in the app launch drive
    // If this fails, fret not, it could be on the rom
    TInt err = aFs.CreatePrivatePath(drv);
    // Try to create the pvt path in the system drive also
    // Create in the default system drive

    aFs.CreatePrivatePath(aFs.GetSystemDrive());

    // Set the session path to the launch drive pvt path
    return aFs.SetSessionToPrivate(drv);
    }
#endif

