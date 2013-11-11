# t_stringdictionary00tagtable.st
#
# Copyright (c) Symbian Software Ltd 2003 - 2007. All rights reserved.
#
# NOTE: For WBXML - Direct correlation on ordering of tag entries to the tag values 
#       the parser defines
# No empty lines allowed either
#
# Short String table
#
stringtable t_StringDictionary00TagTable
# //////////////////////////////////////////////////////////////////////
!// Element names
!// CodePage 00
EAdd 						Add
EAlert 						Alert
EArchive					Archive
EAtomic 					Atomic
EChal 						Chal
ECmd 						Cmd
ECmdId 						CmdID
ECmdRef 					CmdRef
ECopy 						Copy
ECred 						Cred
EData 						Data
EDelete 					Delete
EExec 						Exec
EFinal 						Final
EGet 						Get
EItem						Item
ELang						Lang
ELocName 					LocName
ELocUri 					LocURI
EMap 						Map
EMapItem 					MapItem
EMeta 						Meta
EMsgId 						MsgID
EMsgRef 					MsgRef
ENoResp 					NoResp
ENoResults 					NoResults
EPut 						Put
EReplace 					Replace
ERespUri 					RespURI
EResults 					Results
ESearch 					Search
ESequence 					Sequence
ESessionId 					SessionID
ESftDel 					SftDel
ESource 					Source
ESourceRef 					SourceRef
EStatus 					Status
ESync 						Sync
ESyncBody 					SyncBody
ESyncHdr 					SyncHdr
ESyncMl 					SyncML
ETarget 					Target
ETargetRef 					TargetRef
!// We should never parse this but just incase we do the client
!// or test code will see it.
ETokenValueReservedForFutureUse 	ReservedForFutureUse
EVerDtd 					VerDTD
EVerProto 					VerProto
ETokenValueNumberOfChanges 		NumberOfChanges
ETokenValueMoreData 			MoreData
# //////////////////////////////////////////////////////////////////////
!// For XML
EUrn						TestStringDictionary:TestStringDictionary1.1
!// For WBXML
EPublicId					FD1~0
EFormalPublicId				-//TestStringDictionary//DTD TestStringDictionary 1.1//EN~0
