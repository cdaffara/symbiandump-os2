BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=389621146
SELECT * FROM Artist WHERE UniqueId=1594187644
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1594187644,'Weezer',1)
SELECT * FROM Album WHERE UniqueId=1622929049
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1622929049,'Weezer - Blue Album',1,1594187644,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(2039225812,'',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2307661268,'',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(389621146,0,'2008-01-01 09:11:07',2415919104,204207,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\01-My name is Jonas.mp3',0,'audio/mpeg',1,'My name is Jonas',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=389621146
SELECT VolumeId FROM Music WHERE UniqueId=412706499
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(412706499,0,'2008-01-01 09:11:07',2415919104,184295,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\02-No one else.mp3',0,'audio/mpeg',2,'No one else',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=412706499
SELECT VolumeId FROM Music WHERE UniqueId=360042055
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(360042055,0,'2008-01-01 09:11:07',2415919104,258730,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\03-The world has turned nd left me h.mp3',0,'audio/mpeg',3,'The world has turned nd left me here',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=360042055
SELECT VolumeId FROM Music WHERE UniqueId=469250361
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(469250361,0,'2008-01-01 09:11:08',2415919104,158963,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\04-Buddy Holly.mp3',0,'audio/mpeg',4,'Buddy Holly',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=469250361
SELECT VolumeId FROM Music WHERE UniqueId=308931138
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(308931138,0,'2008-01-01 09:11:09',2415919104,304808,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\05-Undone - the Sweater song.mp3',0,'audio/mpeg',5,'Undone - the Sweater song',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=308931138
SELECT VolumeId FROM Music WHERE UniqueId=444712692
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(444712692,0,'2008-01-01 09:11:09',2415919104,186094,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\06-Surf Wax America.mp3',0,'audio/mpeg',6,'Surf Wax America',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=444712692
SELECT VolumeId FROM Music WHERE UniqueId=366724258
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(366724258,0,'2008-01-01 09:11:09',2415919104,258365,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\07-Say it ain''t so.mp3',0,'audio/mpeg',7,'Say it ain''t so',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=366724258
SELECT VolumeId FROM Music WHERE UniqueId=320679045
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(320679045,0,'2008-01-01 09:11:10',2415919104,235143,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\08-In the garage.mp3',0,'audio/mpeg',8,'In the garage',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=320679045
SELECT VolumeId FROM Music WHERE UniqueId=400494112
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(400494112,0,'2008-01-01 09:11:10',2415919104,204546,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\09-Holiday.mp3',0,'audio/mpeg',9,'Holiday',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=400494112
SELECT VolumeId FROM Music WHERE UniqueId=478934275
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1622929049
SELECT * FROM Album WHERE UniqueId=162292904
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1622929049
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(478934275,0,'2008-01-01 09:11:10',2415919104,478645,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer Blue\\10-Only in dreams.mp3',0,'audio/mpeg',10,'Only in dreams',1594187644,1622929049,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=478934275
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=495942959
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1854331675,'Weezer (Green Album)',1,1594187644,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(1970654874,'Alternative & Punk',1)
SELECT * FROM Composer WHERE UniqueId=2216232813
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2216232813,'Rivers Cuomo',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(495942959,0,'2008-01-01 09:11:23',2415919104,179583,44100,127395,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\01 Don''t Let Go.m4a',0,'audio/mp4','2001-01-02 00:00:00',1,'Don''t Let Go',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=495942959
SELECT VolumeId FROM Music WHERE UniqueId=305640870
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(2114860941,'Rock',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(305640870,0,'2008-01-01 09:11:23',2415919104,179161,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\01-Don''t Let Go.mp3',0,'audio/mpeg','2001-01-02 00:00:00',1,'Don''t Let Go',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=305640870
SELECT VolumeId FROM Music WHERE UniqueId=274163266
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(274163266,0,'2008-01-01 09:11:24',2415919104,139157,44100,127518,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\02 Photograph.m4a',0,'audio/mp4','2001-01-02 00:00:00',2,'Photograph',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=274163266
SELECT VolumeId FROM Music WHERE UniqueId=312204517
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(312204517,0,'2008-01-01 09:11:24',2415919104,138816,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\02-Photograph.mp3',0,'audio/mpeg','2001-01-02 00:00:00',2,'Photograph',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=312204517
SELECT VolumeId FROM Music WHERE UniqueId=463189439
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(463189439,0,'2008-01-01 09:11:25',2415919104,186595,44100,127619,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\03 Hash Pipe.m4a',0,'audio/mp4','2001-01-02 00:00:00',3,'Hash Pipe',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=463189439
SELECT VolumeId FROM Music WHERE UniqueId=428867300
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(428867300,0,'2008-01-01 09:11:25',2415919104,186146,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\03-Hash Pipe.mp3',0,'audio/mpeg','2001-01-02 00:00:00',3,'Hash Pipe',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=428867300
SELECT VolumeId FROM Music WHERE UniqueId=277540470
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(277540470,0,'2008-01-01 09:11:26',2415919104,200364,44100,127651,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\04 Island In The Sun.m4a',0,'audio/mp4','2001-01-02 00:00:00',4,'Island In The Sun',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=277540470
SELECT VolumeId FROM Music WHERE UniqueId=336278711
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(336278711,0,'2008-01-01 09:11:26',2415919104,199881,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\04-Island in the Sun.mp3',0,'audio/mpeg','2001-01-02 00:00:00',4,'Island in the Sun',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=336278711
SELECT VolumeId FROM Music WHERE UniqueId=379749923
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(379749923,0,'2008-01-01 09:11:26',2415919104,154621,44100,127575,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\05 Crab.m4a',0,'audio/mp4','2001-01-02 00:00:00',5,'Crab',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=379749923
SELECT VolumeId FROM Music WHERE UniqueId=451963536
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(451963536,0,'2008-01-01 09:11:27',2415919104,154271,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\05-Crab.mp3',0,'audio/mpeg','2001-01-02 00:00:00',5,'Crab',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=451963536
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=413596867
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(413596867,0,'2008-01-01 09:11:41',2415919104,128383,44100,127936,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\06 Knock-Down Drag-Out.m4a',0,'audio/mp4','2001-01-02 00:00:00',6,'Knock-Down Drag-Out',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=413596867
SELECT VolumeId FROM Music WHERE UniqueId=279738192
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(279738192,0,'2008-01-01 09:11:41',2415919104,128079,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\06-Knock-down Drag-out.mp3',0,'audio/mpeg','2001-01-02 00:00:00',6,'Knock-down Drag-out',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=279738192
SELECT VolumeId FROM Music WHERE UniqueId=441250939
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(441250939,0,'2008-01-01 09:11:42',2415919104,158731,44100,128058,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\07 Smile.m4a',0,'audio/mp4','2001-01-02 00:00:00',7,'Smile',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=441250939
SELECT VolumeId FROM Music WHERE UniqueId=526785200
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(526785200,0,'2008-01-01 09:11:42',2415919104,158363,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\07-Smile.mp3',0,'audio/mpeg','2001-01-02 00:00:00',7,'Smile',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=526785200
SELECT VolumeId FROM Music WHERE UniqueId=437430128
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(437430128,0,'2008-01-01 09:11:42',2415919104,176773,44100,127605,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\08 Simple Pages.m4a',0,'audio/mp4','2001-01-02 00:00:00',8,'Simple Pages',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=437430128
SELECT VolumeId FROM Music WHERE UniqueId=527178592
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(527178592,0,'2008-01-01 09:11:43',2415919104,176346,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\08-Simple Pages.mp3',0,'audio/mpeg','2001-01-02 00:00:00',8,'Simple Pages',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=527178592
SELECT VolumeId FROM Music WHERE UniqueId=413161026
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(413161026,0,'2008-01-01 09:11:43',2415919104,160891,44100,127762,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\09 Glorious Day.m4a',0,'audio/mp4','2001-01-02 00:00:00',9,'Glorious Day',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=413161026
SELECT VolumeId FROM Music WHERE UniqueId=290104802
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(290104802,0,'2008-01-01 09:11:44',2415919104,160500,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\09-Glorious Day.mp3',0,'audio/mpeg','2001-01-02 00:00:00',9,'Glorious Day',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=290104802
SELECT VolumeId FROM Music WHERE UniqueId=473054890
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2216232813
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2216232813
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(473054890,0,'2008-01-01 09:11:44',2415919104,229784,44100,127890,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\10 O Girlfriend.m4a',0,'audio/mp4','2001-01-02 00:00:00',10,'O Girlfriend',1594187644,1854331675,1970654874,2216232813)
SELECT * FROM PlaylistSongInfo WHERE SongId=473054890
SELECT VolumeId FROM Music WHERE UniqueId=469513987
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1854331675
SELECT * FROM Album WHERE UniqueId=185433167
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1854331675
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(469513987,0,'2008-01-01 09:11:44',2415919104,229253,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Weezer (Green Album)\\10-O Girlfriend.mp3',0,'audio/mpeg','2001-01-02 00:00:00',10,'O Girlfriend',1594187644,1854331675,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=469513987
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=513322424
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1873505437,'Maladroit',1,1594187644,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(513322424,0,'2008-01-01 09:11:54',2415919104,161673,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\01-American gigolo.mp3',0,'audio/mpeg',1,'American gigolo',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=513322424
SELECT VolumeId FROM Music WHERE UniqueId=282676407
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(282676407,0,'2008-01-01 09:11:54',2415919104,137148,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\02-Dope nose.mp3',0,'audio/mpeg',2,'Dope nose',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=282676407
SELECT VolumeId FROM Music WHERE UniqueId=519087359
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(519087359,0,'2008-01-01 09:11:54',2415919104,171916,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\03-Keep fishin''.mp3',0,'audio/mpeg',3,'Keep fishin''',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=519087359
SELECT VolumeId FROM Music WHERE UniqueId=304247902
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(304247902,0,'2008-01-01 09:11:55',2415919104,184191,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\04-Take control.mp3',0,'audio/mpeg',4,'Take control',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=304247902
SELECT VolumeId FROM Music WHERE UniqueId=308933177
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(308933177,0,'2008-01-01 09:11:55',2415919104,158233,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\05-Death and destruction.mp3',0,'audio/mpeg',5,'Death and destruction',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=308933177
SELECT VolumeId FROM Music WHERE UniqueId=481448814
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(481448814,0,'2008-01-01 09:11:55',2415919104,188439,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\06-Slob.mp3',0,'audio/mpeg',6,'Slob',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=481448814
SELECT VolumeId FROM Music WHERE UniqueId=383783068
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(383783068,0,'2008-01-01 09:11:56',2415919104,158311,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\07-Burndt jamb.mp3',0,'audio/mpeg',7,'Burndt jamb',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=383783068
SELECT VolumeId FROM Music WHERE UniqueId=273857644
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(273857644,0,'2008-01-01 09:11:56',2415919104,112988,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\08-Space rock.mp3',0,'audio/mpeg',8,'Space rock',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=273857644
SELECT VolumeId FROM Music WHERE UniqueId=406437264
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(406437264,0,'2008-01-01 09:11:56',2415919104,173010,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\09-Slave.mp3',0,'audio/mpeg',9,'Slave',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=406437264
SELECT VolumeId FROM Music WHERE UniqueId=370145367
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(370145367,0,'2008-01-01 09:11:57',2415919104,120416,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\10-Fall together.mp3',0,'audio/mpeg',10,'Fall together',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=370145367
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=436556466
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(436556466,0,'2008-01-01 09:12:08',2415919104,119634,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\11-Possibilities.mp3',0,'audio/mpeg',11,'Possibilities',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=436556466
SELECT VolumeId FROM Music WHERE UniqueId=388370661
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(388370661,0,'2008-01-01 09:12:08',2415919104,154558,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\12-Love explosion.mp3',0,'audio/mpeg',12,'Love explosion',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=388370661
SELECT VolumeId FROM Music WHERE UniqueId=533553163
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(533553163,0,'2008-01-01 09:12:09',2415919104,180959,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\13-December.mp3',0,'audio/mpeg',13,'December',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=533553163
SELECT VolumeId FROM Music WHERE UniqueId=380179277
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1873505437
SELECT * FROM Album WHERE UniqueId=187350543
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873505437
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(380179277,0,'2008-01-01 09:12:09',2415919104,199568,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Maladroit\\14-Island in the sun.mp3',0,'audio/mpeg',14,'Island in the sun',1594187644,1873505437,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=380179277
SELECT VolumeId FROM Music WHERE UniqueId=374522909
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1654232607,'Make believe',1,1594187644,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(374522909,0,'2008-01-01 09:12:10',2415919104,195685,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\01-Beverly hills.mp3',0,'audio/mpeg',1,'Beverly hills',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=374522909
SELECT VolumeId FROM Music WHERE UniqueId=462044770
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(462044770,0,'2008-01-01 09:12:11',2415919104,254795,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\02-Perfect situation.mp3',0,'audio/mpeg',2,'Perfect situation',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=462044770
SELECT VolumeId FROM Music WHERE UniqueId=389237882
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(389237882,0,'2008-01-01 09:12:11',2415919104,204025,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\03-This is such a pity.mp3',0,'audio/mpeg',3,'This is such a pity',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=389237882
SELECT VolumeId FROM Music WHERE UniqueId=526369876
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(526369876,0,'2008-01-01 09:12:12',2415919104,262014,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\04-Hold me.mp3',0,'audio/mpeg',4,'Hold me',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=526369876
SELECT VolumeId FROM Music WHERE UniqueId=407799528
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(407799528,0,'2008-01-01 09:12:12',2415919104,233084,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\05-Peace.mp3',0,'audio/mpeg',5,'Peace',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=407799528
SELECT VolumeId FROM Music WHERE UniqueId=360374994
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(360374994,0,'2008-01-01 09:12:12',2415919104,214997,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\06-We are all on drugs.mp3',0,'audio/mpeg',6,'We are all on drugs',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=360374994
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=376934233
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(376934233,0,'2008-01-01 09:12:24',2415919104,242467,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\07-The damage in your heart.mp3',0,'audio/mpeg',7,'The damage in your heart',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=376934233
SELECT VolumeId FROM Music WHERE UniqueId=365391710
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(365391710,0,'2008-01-01 09:12:24',2415919104,255446,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\08-Pardon me.mp3',0,'audio/mpeg',8,'Pardon me',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=365391710
SELECT VolumeId FROM Music WHERE UniqueId=482210651
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(482210651,0,'2008-01-01 09:12:24',2415919104,167485,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\09-My best friend.mp3',0,'audio/mpeg',9,'My best friend',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=482210651
SELECT VolumeId FROM Music WHERE UniqueId=458986905
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(458986905,0,'2008-01-01 09:12:25',2415919104,196519,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\10-The other way.mp3',0,'audio/mpeg',10,'The other way',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=458986905
SELECT VolumeId FROM Music WHERE UniqueId=305909282
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(305909282,0,'2008-01-01 09:12:25',2415919104,206032,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\11-Freak me out.mp3',0,'audio/mpeg',11,'Freak me out',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=305909282
SELECT VolumeId FROM Music WHERE UniqueId=418023315
SELECT * FROM Artist WHERE UniqueId=1594187644
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1594187644
SELECT * FROM Album WHERE UniqueId=1654232607
SELECT * FROM Album WHERE UniqueId=165423260
SELECT * FROM Artist WHERE UniqueId=159418764
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654232607
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(418023315,0,'2008-01-01 09:12:26',2415919104,276479,44100,128000,0,2048765375,'\\Music\\Digital\\Weezer\\Make Believe\\12-Haunt you every day.mp3',0,'audio/mpeg',12,'Haunt you every day',1594187644,1654232607,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=418023315
SELECT VolumeId FROM Music WHERE UniqueId=387847271
SELECT * FROM Artist WHERE UniqueId=1388188135
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1388188135,'Travis',1)
SELECT * FROM Album WHERE UniqueId=1822430726
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1822430726,'The Man Who',1,1388188135,'')
SELECT * FROM Genre WHERE UniqueId=2010866722
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(2010866722,'Unknown',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(387847271,0,'2008-01-01 09:12:26',2415919104,222850,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\01 Travis _ Writing To Reach You.mp3',0,'audio/mpeg',1,'Travis / Writing To Reach You',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=387847271
SELECT VolumeId FROM Music WHERE UniqueId=346398418
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(346398418,0,'2008-01-01 09:12:26',2415919104,254537,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\02 Travis _ The Fear.mp3',0,'audio/mpeg',2,'Travis / The Fear',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=346398418
SELECT VolumeId FROM Music WHERE UniqueId=341378226
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(341378226,0,'2008-01-01 09:12:27',2415919104,255947,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\03 Travis _ As You Are.mp3',0,'audio/mpeg',3,'Travis / As You Are',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=341378226
SELECT VolumeId FROM Music WHERE UniqueId=304935751
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(304935751,0,'2008-01-01 09:12:27',2415919104,215745,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\04 Travis _ Driftwood.mp3',0,'audio/mpeg',4,'Travis / Driftwood',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=304935751
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=373511799
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(373511799,0,'2008-01-01 09:12:38',2415919104,262243,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\05 Travis _ The Last Laugh Of The La.mp3',0,'audio/mpeg',5,'Travis / The Last Laugh Of The Laughter',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=373511799
SELECT VolumeId FROM Music WHERE UniqueId=524226934
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(524226934,0,'2008-01-01 09:12:38',2415919104,265926,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\06 Travis _ Turn.mp3',0,'audio/mpeg',6,'Travis / Turn',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=524226934
SELECT VolumeId FROM Music WHERE UniqueId=441680759
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(441680759,0,'2008-01-01 09:12:38',2415919104,267075,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\07 Travis _ Why Does It Always Rain.mp3',0,'audio/mpeg',7,'Travis / Why Does It Always Rain On Me?',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=441680759
SELECT VolumeId FROM Music WHERE UniqueId=522983578
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(522983578,0,'2008-01-01 09:12:39',2415919104,297325,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\08 Travis _ Luv.mp3',0,'audio/mpeg',8,'Travis / Luv',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=522983578
SELECT VolumeId FROM Music WHERE UniqueId=512806921
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(512806921,0,'2008-01-01 09:12:39',2415919104,197537,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\09 Travis _ She''s So Strange.mp3',0,'audio/mpeg',9,'Travis / She''s So Strange',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=512806921
SELECT VolumeId FROM Music WHERE UniqueId=389030744
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1822430726
SELECT * FROM Album WHERE UniqueId=182243072
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1822430726
SELECT * FROM Genre WHERE UniqueId=2010866722
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2010866722
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(389030744,0,'2008-01-01 09:12:40',2415919104,631092,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Man Who\\10 Travis _ Slide Show.mp3',0,'audio/mpeg',10,'Travis / Slide Show',1388188135,1822430726,2010866722,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=389030744
SELECT VolumeId FROM Music WHERE UniqueId=441333535
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1696288631,'The Invisible Band',1,1388188135,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(441333535,0,'2008-01-01 09:12:40',2415919104,228315,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\01-Sing.mp3',0,'audio/mpeg',1,'Sing',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=441333535
SELECT VolumeId FROM Music WHERE UniqueId=396094767
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(396094767,0,'2008-01-01 09:12:40',2415919104,176737,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\02-Dear Diary.mp3',0,'audio/mpeg',2,'Dear Diary',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=396094767
SELECT VolumeId FROM Music WHERE UniqueId=497845055
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(497845055,0,'2008-01-01 09:12:41',2415919104,238740,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\03-Side.mp3',0,'audio/mpeg',3,'Side',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=497845055
SELECT VolumeId FROM Music WHERE UniqueId=419870374
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(419870374,0,'2008-01-01 09:12:41',2415919104,245308,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\04-Pipe Dreams.mp3',0,'audio/mpeg',4,'Pipe Dreams',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=419870374
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=329351375
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(329351375,0,'2008-01-01 09:12:55',2415919104,221226,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\05-Flowers In The Window.mp3',0,'audio/mpeg',5,'Flowers In The Window',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=329351375
SELECT VolumeId FROM Music WHERE UniqueId=477140561
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(477140561,0,'2008-01-01 09:12:55',2415919104,185416,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\06-The Cage.mp3',0,'audio/mpeg',6,'The Cage',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=477140561
SELECT VolumeId FROM Music WHERE UniqueId=313193985
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(313193985,0,'2008-01-01 09:12:55',2415919104,262744,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\07-Safe.mp3',0,'audio/mpeg',7,'Safe',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=313193985
SELECT VolumeId FROM Music WHERE UniqueId=533622161
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(533622161,0,'2008-01-01 09:12:56',2415919104,188518,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\08-Follow The Light.mp3',0,'audio/mpeg',8,'Follow The Light',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=533622161
SELECT VolumeId FROM Music WHERE UniqueId=396382477
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(396382477,0,'2008-01-01 09:12:56',2415919104,195607,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\09-Last Train.mp3',0,'audio/mpeg',9,'Last Train',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=396382477
SELECT VolumeId FROM Music WHERE UniqueId=476726781
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(476726781,0,'2008-01-01 09:12:57',2415919104,245412,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\10-Afterglow.mp3',0,'audio/mpeg',10,'Afterglow',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=476726781
SELECT VolumeId FROM Music WHERE UniqueId=477602362
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(477602362,0,'2008-01-01 09:12:57',2415919104,232303,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\11-Indefinitely.mp3',0,'audio/mpeg',11,'Indefinitely',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=477602362
SELECT VolumeId FROM Music WHERE UniqueId=395362219
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1696288631
SELECT * FROM Album WHERE UniqueId=169628863
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1696288631
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(395362219,0,'2008-01-01 09:12:58',2415919104,301368,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\The Invisible Band\\12-The Humpty Dumpty Love Song.mp3',0,'audio/mpeg',12,'The Humpty Dumpty Love Song',1388188135,1696288631,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=395362219
SELECT VolumeId FROM Music WHERE UniqueId=354295538
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1614440346,'Singles',1,1388188135,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(354295538,0,'2008-01-01 09:12:58',2415919104,229435,44100,192149,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\01 Sing.m4a',0,'audio/mp4','2004-01-02 00:00:00',1,'Sing',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=354295538
SELECT VolumeId FROM Music WHERE UniqueId=519548558
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(519548558,0,'2008-01-01 09:12:58',2415919104,214343,44100,191579,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\02 Driftwood.m4a',0,'audio/mp4','2004-01-02 00:00:00',2,'Driftwood',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=519548558
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=497618078
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(497618078,0,'2008-01-01 09:13:15',2415919104,222561,44100,190820,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\03 Writing To Reach You.m4a',0,'audio/mp4','2004-01-02 00:00:00',3,'Writing To Reach You',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=497618078
SELECT VolumeId FROM Music WHERE UniqueId=423029273
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(423029273,0,'2008-01-01 09:13:15',2415919104,264870,44100,191631,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\04 Why Does It Always Rain On Me_.m4a',0,'audio/mp4','2004-01-02 00:00:00',4,'Why Does It Always Rain On Me?',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=423029273
SELECT VolumeId FROM Music WHERE UniqueId=288150358
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(288150358,0,'2008-01-01 09:13:16',2415919104,227508,44100,191112,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\05 Re-Offender.m4a',0,'audio/mp4','2004-01-02 00:00:00',5,'Re-Offender',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=288150358
SELECT VolumeId FROM Music WHERE UniqueId=414966602
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(414966602,0,'2008-01-01 09:13:16',2415919104,178305,44100,191562,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\06 Walking In The Sun.m4a',0,'audio/mp4','2004-01-02 00:00:00',6,'Walking In The Sun',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=414966602
SELECT VolumeId FROM Music WHERE UniqueId=523895017
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(523895017,0,'2008-01-01 09:13:17',2415919104,186153,44100,191653,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\07 Tied To The 90''s.m4a',0,'audio/mp4','2004-01-02 00:00:00',7,'Tied To The 90''s',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=523895017
SELECT VolumeId FROM Music WHERE UniqueId=399195426
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(399195426,0,'2008-01-01 09:13:18',2415919104,187500,44100,191730,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\08 Coming Around.m4a',0,'audio/mp4','2004-01-02 00:00:00',8,'Coming Around',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=399195426
SELECT VolumeId FROM Music WHERE UniqueId=474764844
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(474764844,0,'2008-01-01 09:13:18',2415919104,208560,44100,190254,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\09 Flowers In The Window.m4a',0,'audio/mp4','2004-01-02 00:00:00',9,'Flowers In The Window',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=474764844
SELECT VolumeId FROM Music WHERE UniqueId=397664526
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(397664526,0,'2008-01-01 09:13:18',2415919104,218196,44100,191912,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\10 Love Will Come Through.m4a',0,'audio/mp4','2004-01-02 00:00:00',10,'Love Will Come Through',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=397664526
SELECT VolumeId FROM Music WHERE UniqueId=406051716
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(406051716,0,'2008-01-01 09:13:19',2415919104,237956,44100,190922,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\11 More Than Us.m4a',0,'audio/mp4','2004-01-02 00:00:00',11,'More Than Us',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=406051716
SELECT VolumeId FROM Music WHERE UniqueId=370648070
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(370648070,0,'2008-01-01 09:13:19',2415919104,235565,44100,191561,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\12 Side.m4a',0,'audio/mp4','2004-01-02 00:00:00',12,'Side',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=370648070
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=536166828
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(536166828,0,'2008-01-01 09:13:35',2415919104,241951,44100,191062,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\13 U16 Girls.m4a',0,'audio/mp4','2004-01-02 00:00:00',13,'U16 Girls',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=536166828
SELECT VolumeId FROM Music WHERE UniqueId=455703090
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(455703090,0,'2008-01-01 09:13:35',2415919104,255860,44100,190821,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\14 Happy.m4a',0,'audio/mp4','2004-01-02 00:00:00',14,'Happy',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=455703090
SELECT VolumeId FROM Music WHERE UniqueId=364174166
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(364174166,0,'2008-01-01 09:13:35',2415919104,233150,44100,191978,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\15 All I Want To Do Is Rock.m4a',0,'audio/mp4','2004-01-02 00:00:00',15,'All I Want To Do Is Rock',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=364174166
SELECT VolumeId FROM Music WHERE UniqueId=431908023
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(431908023,0,'2008-01-01 09:13:36',2415919104,226370,44100,191729,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\16 The Beautiful Occupation.m4a',0,'audio/mp4','2004-01-02 00:00:00',16,'The Beautiful Occupation',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=431908023
SELECT VolumeId FROM Music WHERE UniqueId=268801800
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(268801800,0,'2008-01-01 09:13:37',2415919104,264428,44100,191143,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\17 Turn.m4a',0,'audio/mp4','2004-01-02 00:00:00',17,'Turn',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=268801800
SELECT VolumeId FROM Music WHERE UniqueId=357570559
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1614440346
SELECT * FROM Album WHERE UniqueId=161444034
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1614440346
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(357570559,0,'2008-01-01 09:13:37',2415919104,227253,44100,192075,0,2048765375,'\\Music\\Digital\\Travis\\Singles\\18 The Distance.m4a',0,'audio/mp4','2004-01-02 00:00:00',18,'The Distance',1388188135,1614440346,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=357570559
SELECT VolumeId FROM Music WHERE UniqueId=331719202
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1749506334,'Good Feeling',1,1388188135,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2413952949,'Fran Healy',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(331719202,0,'2008-01-01 09:13:38',2415919104,233360,44100,128106,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\01 All I Want To Do Is Rock.m4a',0,'audio/mp4','1997-01-02 00:00:00',1,'All I Want To Do Is Rock',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=331719202
SELECT VolumeId FROM Music WHERE UniqueId=292813194
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(292813194,0,'2008-01-01 09:13:38',2415919104,240651,44100,128109,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\02 U 16 Girls.m4a',0,'audio/mp4','1997-01-02 00:00:00',2,'U 16 Girls',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=292813194
SELECT VolumeId FROM Music WHERE UniqueId=431214501
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(431214501,0,'2008-01-01 09:13:39',2415919104,244970,44100,128100,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\03 The Line Is Fine.m4a',0,'audio/mp4','1997-01-02 00:00:00',3,'The Line Is Fine',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=431214501
SELECT VolumeId FROM Music WHERE UniqueId=418252635
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(418252635,0,'2008-01-01 09:13:39',2415919104,197392,44100,128075,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\04 Good Day To Die.m4a',0,'audio/mp4','1997-01-02 00:00:00',4,'Good Day To Die',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=418252635
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=468065725
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(468065725,0,'2008-01-01 09:14:24',2415919104,204823,44100,128078,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\05 Good Feeling.m4a',0,'audio/mp4','1997-01-02 00:00:00',5,'Good Feeling',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=468065725
SELECT VolumeId FROM Music WHERE UniqueId=350703332
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(350703332,0,'2008-01-01 09:14:25',2415919104,234800,44100,128116,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\06 Midsummer Nights Dreamin''.m4a',0,'audio/mp4','1997-01-02 00:00:00',6,'Midsummer Nights Dreamin''',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=350703332
SELECT VolumeId FROM Music WHERE UniqueId=508296750
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(508296750,0,'2008-01-01 09:14:28',2415919104,188662,44100,128057,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\07 Tied To The 90''s.m4a',0,'audio/mp4','1997-01-02 00:00:00',7,'Tied To The 90''s',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=508296750
SELECT VolumeId FROM Music WHERE UniqueId=389281082
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(389281082,0,'2008-01-01 09:14:29',2415919104,330930,44100,128096,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\08 I Love You Anyways.m4a',0,'audio/mp4','1997-01-02 00:00:00',8,'I Love You Anyways',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=389281082
SELECT VolumeId FROM Music WHERE UniqueId=370271205
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(370271205,0,'2008-01-01 09:14:31',2415919104,256023,44100,128084,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\09 Happy.m4a',0,'audio/mp4','1997-01-02 00:00:00',9,'Happy',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=370271205
SELECT VolumeId FROM Music WHERE UniqueId=378998027
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(378998027,0,'2008-01-01 09:14:33',2415919104,236495,44100,128080,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\10 More Than Us.m4a',0,'audio/mp4','1997-01-02 00:00:00',10,'More Than Us',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=378998027
SELECT VolumeId FROM Music WHERE UniqueId=485289979
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(485289979,0,'2008-01-01 09:14:35',2415919104,257857,44100,128088,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\11 Falling Down.m4a',0,'audio/mp4','1997-01-02 00:00:00',11,'Falling Down',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=485289979
SELECT VolumeId FROM Music WHERE UniqueId=295436947
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2413952949
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2413952949
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(295436947,0,'2008-01-01 09:14:37',2415919104,320040,44100,128091,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\12 Funny Thing.m4a',0,'audio/mp4','1997-01-02 00:00:00',12,'Funny Thing',1388188135,1749506334,1970654874,2413952949)
SELECT * FROM PlaylistSongInfo WHERE SongId=295436947
SELECT VolumeId FROM Music WHERE UniqueId=411121072
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(411121072,0,'2008-01-01 09:14:39',2415919104,232798,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-01.mp3',0,'audio/mpeg',1,'Track-01',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=411121072
SELECT VolumeId FROM Music WHERE UniqueId=449138264
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(449138264,0,'2008-01-01 09:14:41',2415919104,240095,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-02.mp3',0,'audio/mpeg',2,'Track-02',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=449138264
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=275871962
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(275871962,0,'2008-01-01 09:15:08',2415919104,244396,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-03.mp3',0,'audio/mpeg',3,'Track-03',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=275871962
SELECT VolumeId FROM Music WHERE UniqueId=445379040
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(445379040,0,'2008-01-01 09:15:08',2415919104,196936,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-04.mp3',0,'audio/mpeg',4,'Track-04',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=445379040
SELECT VolumeId FROM Music WHERE UniqueId=376640643
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(376640643,0,'2008-01-01 09:15:09',2415919104,204338,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-05.mp3',0,'audio/mpeg',5,'Track-05',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=376640643
SELECT VolumeId FROM Music WHERE UniqueId=475568724
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(475568724,0,'2008-01-01 09:15:09',2415919104,234231,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-06.mp3',0,'audio/mpeg',6,'Track-06',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=475568724
SELECT VolumeId FROM Music WHERE UniqueId=445172493
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(445172493,0,'2008-01-01 09:15:09',2415919104,188205,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-07.mp3',0,'audio/mpeg',7,'Track-07',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=445172493
SELECT VolumeId FROM Music WHERE UniqueId=319389127
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(319389127,0,'2008-01-01 09:15:10',2415919104,330141,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-08.mp3',0,'audio/mpeg',8,'Track-08',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=319389127
SELECT VolumeId FROM Music WHERE UniqueId=271663133
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(271663133,0,'2008-01-01 09:15:10',2415919104,255420,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-09.mp3',0,'audio/mpeg',9,'Track-09',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=271663133
SELECT VolumeId FROM Music WHERE UniqueId=348608169
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(348608169,0,'2008-01-01 09:15:10',2415919104,235925,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-10.mp3',0,'audio/mpeg',10,'Track-10',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=348608169
SELECT VolumeId FROM Music WHERE UniqueId=442607584
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(442607584,0,'2008-01-01 09:15:11',2415919104,257244,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-11.mp3',0,'audio/mpeg',11,'Track-11',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=442607584
SELECT VolumeId FROM Music WHERE UniqueId=345408907
SELECT * FROM Artist WHERE UniqueId=1388188135
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1388188135
SELECT * FROM Album WHERE UniqueId=1749506334
SELECT * FROM Album WHERE UniqueId=174950633
SELECT * FROM Artist WHERE UniqueId=138818813
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1749506334
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(345408907,0,'2008-01-01 09:15:11',2415919104,319273,44100,128000,0,2048765375,'\\Music\\Digital\\Travis\\Good Feeling\\Track-12.mp3',0,'audio/mpeg',12,'Track-12',1388188135,1749506334,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=345408907
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=410205177
SELECT * FROM Artist WHERE UniqueId=1489875735
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1489875735,'Third Eye Blind',1)
SELECT * FROM Album WHERE UniqueId=1758311191
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1758311191,'Third Eye Blind',1,1489875735,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2254005734,'Jenkins, Cadogan',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(410205177,0,'2008-01-01 09:15:27',2415919104,200690,44100,128063,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\01 Losing A Whole Year.m4a',0,'audio/mp4','1997-01-02 00:00:00',1,'Losing A Whole Year',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=410205177
SELECT VolumeId FROM Music WHERE UniqueId=403439157
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(403439157,0,'2008-01-01 09:15:27',2415919104,228623,44100,128077,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\02 Narcolepsy.m4a',0,'audio/mp4','1997-01-02 00:00:00',2,'Narcolepsy',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=403439157
SELECT VolumeId FROM Music WHERE UniqueId=306632878
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2321276795
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2321276795,'Jenkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(306632878,0,'2008-01-01 09:15:28',2415919104,268794,44100,128066,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\03 Semi-Charmed Life.m4a',0,'audio/mp4','1997-01-02 00:00:00',3,'Semi-Charmed Life',1489875735,1758311191,1970654874,2321276795)
SELECT * FROM PlaylistSongInfo WHERE SongId=306632878
SELECT VolumeId FROM Music WHERE UniqueId=364792521
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2321276795
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2321276795
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(364792521,0,'2008-01-01 09:15:29',2415919104,273043,44100,128087,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\04 Jumper.m4a',0,'audio/mp4','1997-01-02 00:00:00',4,'Jumper',1489875735,1758311191,1970654874,2321276795)
SELECT * FROM PlaylistSongInfo WHERE SongId=364792521
SELECT VolumeId FROM Music WHERE UniqueId=361695099
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2150615559
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2150615559,'Kevin Cadogan/Stephan Jenkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(361695099,0,'2008-01-01 09:15:29',2415919104,189799,44100,128068,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\05 Graduate.m4a',0,'audio/mp4','1997-01-02 00:00:00',5,'Graduate',1489875735,1758311191,1970654874,2150615559)
SELECT * FROM PlaylistSongInfo WHERE SongId=361695099
SELECT VolumeId FROM Music WHERE UniqueId=346755812
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(346755812,0,'2008-01-01 09:15:30',2415919104,253724,44100,128075,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\06 How''s It Going To Be.m4a',0,'audio/mp4','1997-01-02 00:00:00',6,'How''s It Going To Be',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=346755812
SELECT VolumeId FROM Music WHERE UniqueId=312366759
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(312366759,0,'2008-01-01 09:15:30',2415919104,297679,44100,128101,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\07 Thanks A Lot.m4a',0,'audio/mp4','1997-01-02 00:00:00',7,'Thanks A Lot',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=312366759
SELECT VolumeId FROM Music WHERE UniqueId=447515066
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(447515066,0,'2008-01-01 09:15:30',2415919104,179792,44100,128077,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\08 Burning Man.m4a',0,'audio/mp4','1997-01-02 00:00:00',8,'Burning Man',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=447515066
SELECT VolumeId FROM Music WHERE UniqueId=341466956
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(341466956,0,'2008-01-01 09:15:31',2415919104,232060,44100,128105,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\09 Good For You.m4a',0,'audio/mp4','1997-01-02 00:00:00',9,'Good For You',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=341466956
SELECT VolumeId FROM Music WHERE UniqueId=425577842
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(425577842,0,'2008-01-01 09:15:31',2415919104,187059,44100,128070,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\10 London.m4a',0,'audio/mp4','1997-01-02 00:00:00',10,'London',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=425577842
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=501795329
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2321276795
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2321276795
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(501795329,0,'2008-01-01 09:15:47',2415919104,269235,44100,128081,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\11 I Want You.m4a',0,'audio/mp4','1997-01-02 00:00:00',11,'I Want You',1489875735,1758311191,1970654874,2321276795)
SELECT * FROM PlaylistSongInfo WHERE SongId=501795329
SELECT VolumeId FROM Music WHERE UniqueId=366420612
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(366420612,0,'2008-01-01 09:15:47',2415919104,296797,44100,128112,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\12 The Background.m4a',0,'audio/mp4','1997-01-02 00:00:00',12,'The Background',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=366420612
SELECT VolumeId FROM Music WHERE UniqueId=301804158
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2321276795
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2321276795
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(301804158,0,'2008-01-01 09:15:48',2415919104,262687,44100,128089,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\13 Motorcycle Drive By.m4a',0,'audio/mp4','1997-01-02 00:00:00',13,'Motorcycle Drive By',1489875735,1758311191,1970654874,2321276795)
SELECT * FROM PlaylistSongInfo WHERE SongId=301804158
SELECT VolumeId FROM Music WHERE UniqueId=501626017
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1758311191
SELECT * FROM Album WHERE UniqueId=175831119
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1758311191
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2254005734
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2254005734
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(501626017,0,'2008-01-01 09:15:49',2415919104,317556,44100,128094,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Third Eye Blind\\14 God Of Wine.m4a',0,'audio/mp4','1997-01-02 00:00:00',14,'God Of Wine',1489875735,1758311191,1970654874,2254005734)
SELECT * FROM PlaylistSongInfo WHERE SongId=501626017
SELECT VolumeId FROM Music WHERE UniqueId=318892648
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1802028219,'Blue',1,1489875735,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2361029706
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2361029706,'Stephan Jenkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(318892648,0,'2008-01-01 09:15:49',2415919104,119629,44100,128054,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\01 Anything.m4a',0,'audio/mp4','1999-01-02 00:00:00',1,'Anything',1489875735,1802028219,1970654874,2361029706)
SELECT * FROM PlaylistSongInfo WHERE SongId=318892648
SELECT VolumeId FROM Music WHERE UniqueId=280231455
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2342982727,'Stephan Jenkins & Kevin Cadogan',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(280231455,0,'2008-01-01 09:15:49',2415919104,291596,44100,128090,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\02 Wounded.m4a',0,'audio/mp4','1999-01-02 00:00:00',2,'Wounded',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=280231455
SELECT VolumeId FROM Music WHERE UniqueId=308946289
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2311745873
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2311745873,'Stephan Jenkins & Arion Salazar',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(308946289,0,'2008-01-01 09:15:50',2415919104,185504,44100,128085,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\03 10 Days Late.m4a',0,'audio/mp4','1999-01-02 00:00:00',3,'10 Days Late',1489875735,1802028219,1970654874,2311745873)
SELECT * FROM PlaylistSongInfo WHERE SongId=308946289
SELECT VolumeId FROM Music WHERE UniqueId=381173748
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2361029706
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2361029706
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(381173748,0,'2008-01-01 09:15:50',2415919104,237377,44100,128091,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\04 Never Let You Go.m4a',0,'audio/mp4','1999-01-02 00:00:00',4,'Never Let You Go',1489875735,1802028219,1970654874,2361029706)
SELECT * FROM PlaylistSongInfo WHERE SongId=381173748
SELECT VolumeId FROM Music WHERE UniqueId=402665620
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2361029706
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2361029706
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(402665620,0,'2008-01-01 09:15:51',2415919104,250821,44100,128102,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\05 Deep Inside Of You.m4a',0,'audio/mp4','1999-01-02 00:00:00',5,'Deep Inside Of You',1489875735,1802028219,1970654874,2361029706)
SELECT * FROM PlaylistSongInfo WHERE SongId=402665620
SELECT VolumeId FROM Music WHERE UniqueId=429197352
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2342982727
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(429197352,0,'2008-01-01 09:15:52',2415919104,233848,44100,128087,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\06 1000 Julys.m4a',0,'audio/mp4','1999-01-02 00:00:00',6,'1000 Julys',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=429197352
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=269901682
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2342982727
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(269901682,0,'2008-01-01 09:16:13',2415919104,155968,44100,128067,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\07 An Ode To Maybe.m4a',0,'audio/mp4','1999-01-02 00:00:00',7,'An Ode To Maybe',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=269901682
SELECT VolumeId FROM Music WHERE UniqueId=346952708
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2342982727
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(346952708,0,'2008-01-01 09:16:14',2415919104,325590,44100,128104,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\08 The Red Summer Sun.m4a',0,'audio/mp4','1999-01-02 00:00:00',8,'The Red Summer Sun',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=346952708
SELECT VolumeId FROM Music WHERE UniqueId=313088095
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2342982727
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(313088095,0,'2008-01-01 09:16:15',2415919104,285675,44100,128074,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\09 Camouflage.m4a',0,'audio/mp4','1999-01-02 00:00:00',9,'Camouflage',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=313088095
SELECT VolumeId FROM Music WHERE UniqueId=346307699
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2361029706
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2361029706
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(346307699,0,'2008-01-01 09:16:15',2415919104,225999,44100,128075,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\10 Farther.m4a',0,'audio/mp4','1999-01-02 00:00:00',10,'Farther',1489875735,1802028219,1970654874,2361029706)
SELECT * FROM PlaylistSongInfo WHERE SongId=346307699
SELECT VolumeId FROM Music WHERE UniqueId=453137304
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2361029706
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2361029706
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(453137304,0,'2008-01-01 09:16:16',2415919104,274065,44100,128068,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\11 Slow Motion (Instrumental).m4a',0,'audio/mp4','1999-01-02 00:00:00',11,'Slow Motion (Instrumental)',1489875735,1802028219,1970654874,2361029706)
SELECT * FROM PlaylistSongInfo WHERE SongId=453137304
SELECT VolumeId FROM Music WHERE UniqueId=377076726
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2342982727
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2342982727
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(377076726,0,'2008-01-01 09:16:16',2415919104,308686,44100,128095,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\12 Darkness.m4a',0,'audio/mp4','1999-01-02 00:00:00',12,'Darkness',1489875735,1802028219,1970654874,2342982727)
SELECT * FROM PlaylistSongInfo WHERE SongId=377076726
SELECT VolumeId FROM Music WHERE UniqueId=309726544
SELECT * FROM Artist WHERE UniqueId=1489875735
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1489875735
SELECT * FROM Album WHERE UniqueId=1802028219
SELECT * FROM Album WHERE UniqueId=180202821
SELECT * FROM Artist WHERE UniqueId=148987573
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1802028219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2311745873
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2311745873
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(309726544,0,'2008-01-01 09:16:17',2415919104,1220046,44100,46684,0,2048765375,'\\Music\\Digital\\Third Eye Blind\\Blue\\13 Darwin.m4a',0,'audio/mp4','1999-01-02 00:00:00',13,'Darwin',1489875735,1802028219,1970654874,2311745873)
SELECT * FROM PlaylistSongInfo WHERE SongId=309726544
SELECT VolumeId FROM Music WHERE UniqueId=274589732
SELECT * FROM Artist WHERE UniqueId=1377891040
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1377891040,'The White Stripes',1)
SELECT * FROM Album WHERE UniqueId=1675829327
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1675829327,'White Blood Cells',1,1377891040,'')
SELECT * FROM Genre WHERE UniqueId=1974481792
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(1974481792,'AlternRock',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(274589732,0,'2008-01-01 09:16:17',2415919104,185411,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\01-Dead Leaves And The Dirty Ground.mp3',0,'audio/mpeg','2001-01-02 00:00:00',1,'Dead Leaves And The Dirty Ground',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=274589732
SELECT VolumeId FROM Music WHERE UniqueId=504764472
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(504764472,0,'2008-01-01 09:16:18',2415919104,130397,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\02-Hotel Yorba.mp3',0,'audio/mpeg','2001-01-02 00:00:00',2,'Hotel Yorba',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=504764472
SELECT VolumeId FROM Music WHERE UniqueId=486414058
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(486414058,0,'2008-01-01 09:16:18',2415919104,174335,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\03-I''m Finding It Harder To Be A Gentlemen.mp3',0,'audio/mpeg','2001-01-02 00:00:00',3,'I''m Finding It Harder To Be A Gentlemen',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=486414058
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=483815211
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(483815211,0,'2008-01-01 09:16:27',2415919104,110361,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\04-Fell In Love With A Girl.mp3',0,'audio/mpeg','2001-01-02 00:00:00',4,'Fell In Love With A Girl',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=483815211
SELECT VolumeId FROM Music WHERE UniqueId=276481087
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(276481087,0,'2008-01-01 09:16:27',2415919104,123265,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\05-Expecting.mp3',0,'audio/mpeg','2001-01-02 00:00:00',5,'Expecting',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=276481087
SELECT VolumeId FROM Music WHERE UniqueId=352974755
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(352974755,0,'2008-01-01 09:16:28',2415919104,50436,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\06-Little Room.mp3',0,'audio/mpeg','2001-01-02 00:00:00',6,'Little Room',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=352974755
SELECT VolumeId FROM Music WHERE UniqueId=432700623
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(432700623,0,'2008-01-01 09:16:28',2415919104,205969,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\07-The Union Forever.mp3',0,'audio/mpeg','2001-01-02 00:00:00',7,'The Union Forever',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=432700623
SELECT VolumeId FROM Music WHERE UniqueId=422603320
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(422603320,0,'2008-01-01 09:16:29',2415919104,190243,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\08-The Same Boy You''ve Always Known.mp3',0,'audio/mpeg','2001-01-02 00:00:00',8,'The Same Boy You''ve Always Known',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=422603320
SELECT VolumeId FROM Music WHERE UniqueId=457856017
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(457856017,0,'2008-01-01 09:16:29',2415919104,142309,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\09-We''re Going To Be Friends.mp3',0,'audio/mpeg','2001-01-02 00:00:00',9,'We''re Going To Be Friends',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=457856017
SELECT VolumeId FROM Music WHERE UniqueId=532030663
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(532030663,0,'2008-01-01 09:16:30',2415919104,186273,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\10-Offend In Every Way.mp3',0,'audio/mpeg','2001-01-02 00:00:00',10,'Offend In Every Way',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=532030663
SELECT VolumeId FROM Music WHERE UniqueId=405909327
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(405909327,0,'2008-01-01 09:16:30',2415919104,124336,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\11-I Think I Smell A Rat.mp3',0,'audio/mpeg','2001-01-02 00:00:00',11,'I Think I Smell A Rat',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=405909327
SELECT VolumeId FROM Music WHERE UniqueId=464552920
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(464552920,0,'2008-01-01 09:16:31',2415919104,139461,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\12-Aluminum.mp3',0,'audio/mpeg','2001-01-02 00:00:00',12,'Aluminum',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=464552920
SELECT VolumeId FROM Music WHERE UniqueId=475250113
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(475250113,0,'2008-01-01 09:16:31',2415919104,218508,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\13-I Can''t Wait.mp3',0,'audio/mpeg','2001-01-02 00:00:00',13,'I Can''t Wait',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=475250113
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=430040876
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(430040876,0,'2008-01-01 09:16:44',2415919104,107618,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\14-Now Mary.mp3',0,'audio/mpeg','2001-01-02 00:00:00',14,'Now Mary',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=430040876
SELECT VolumeId FROM Music WHERE UniqueId=444647756
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(444647756,0,'2008-01-01 09:16:44',2415919104,211716,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\15-I Can Learn.mp3',0,'audio/mpeg','2001-01-02 00:00:00',15,'I Can Learn',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=444647756
SELECT VolumeId FROM Music WHERE UniqueId=322733208
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1675829327
SELECT * FROM Album WHERE UniqueId=167582932
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1675829327
SELECT * FROM Genre WHERE UniqueId=1974481792
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1974481792
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(322733208,0,'2008-01-01 09:16:44',2415919104,131416,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\White Blood Cells\\16-This Protector.mp3',0,'audio/mpeg','2001-01-02 00:00:00',16,'This Protector',1377891040,1675829327,1974481792,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=322733208
SELECT VolumeId FROM Music WHERE UniqueId=347471362
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1763558972,'Elephant',1,1377891040,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2414755838,'Jack White',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(347471362,0,'2008-01-01 09:16:45',2415919104,232083,44100,127927,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\01 Seven Nation Army.m4a',0,'audio/mp4','2003-01-02 00:00:00',1,'Seven Nation Army',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=347471362
SELECT VolumeId FROM Music WHERE UniqueId=291299816
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(291299816,0,'2008-01-01 09:16:45',2415919104,183948,44100,128086,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\02 Black Math.m4a',0,'audio/mp4','2003-01-02 00:00:00',2,'Black Math',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=291299816
SELECT VolumeId FROM Music WHERE UniqueId=400380565
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(400380565,0,'2008-01-01 09:16:46',2415919104,223747,44100,128103,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\03 There''s No Home For You Here.m4a',0,'audio/mp4','2003-01-02 00:00:00',3,'There''s No Home For You Here',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=400380565
SELECT VolumeId FROM Music WHERE UniqueId=350954889
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(350954889,0,'2008-01-01 09:16:46',2415919104,166533,44100,128087,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\04 I Just Don''t Know What To Do With.m4a',0,'audio/mp4','2003-01-02 00:00:00',4,'I Just Don''t Know What To Do With Myself',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=350954889
SELECT VolumeId FROM Music WHERE UniqueId=374708694
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(374708694,0,'2008-01-01 09:16:46',2415919104,178491,44100,128008,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\05 In The Cold Cold Night.m4a',0,'audio/mp4','2003-01-02 00:00:00',5,'In The Cold Cold Night',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=374708694
SELECT VolumeId FROM Music WHERE UniqueId=323774195
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(323774195,0,'2008-01-01 09:16:47',2415919104,200550,44100,128098,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\06 I Want To Be The Boy.m4a',0,'audio/mp4','2003-01-02 00:00:00',6,'I Want To Be The Boy',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=323774195
SELECT VolumeId FROM Music WHERE UniqueId=513104630
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(513104630,0,'2008-01-01 09:16:47',2415919104,219892,44100,128094,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\07 You''ve Got Her In Your Pocket.m4a',0,'audio/mp4','2003-01-02 00:00:00',7,'You''ve Got Her In Your Pocket',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=513104630
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=399134770
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(399134770,0,'2008-01-01 09:17:01',2415919104,438996,44100,128114,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\08 Ball & Biscuit.m4a',0,'audio/mp4','2003-01-02 00:00:00',8,'Ball & Biscuit',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=399134770
SELECT VolumeId FROM Music WHERE UniqueId=378139657
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(378139657,0,'2008-01-01 09:17:01',2415919104,212369,44100,128101,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\09 The Hardest Button To Button.m4a',0,'audio/mp4','2003-01-02 00:00:00',9,'The Hardest Button To Button',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=378139657
SELECT VolumeId FROM Music WHERE UniqueId=338593528
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(338593528,0,'2008-01-01 09:17:02',2415919104,249289,44100,128118,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\10 Little Acorns.m4a',0,'audio/mp4','2003-01-02 00:00:00',10,'Little Acorns',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=338593528
SELECT VolumeId FROM Music WHERE UniqueId=495144515
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(495144515,0,'2008-01-01 09:17:02',2415919104,108483,0,0,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\11 Hypnotise.m4a',0,'audio/mp4','2003-01-02 00:00:00',11,'Hypnotise',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=495144515
SELECT VolumeId FROM Music WHERE UniqueId=395958888
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(395958888,0,'2008-01-01 09:17:03',2415919104,220427,44100,128101,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\12 The Air Near My Fingers.m4a',0,'audio/mp4','2003-01-02 00:00:00',12,'The Air Near My Fingers',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=395958888
SELECT VolumeId FROM Music WHERE UniqueId=422945479
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(422945479,0,'2008-01-01 09:17:04',2415919104,197810,44100,128097,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\13 Girl You Have No Faith In Medicin.m4a',0,'audio/mp4','2003-01-02 00:00:00',13,'Girl You Have No Faith In Medicine',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=422945479
SELECT VolumeId FROM Music WHERE UniqueId=459347041
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1763558972
SELECT * FROM Album WHERE UniqueId=176355897
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1763558972
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2414755838
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2414755838
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(459347041,0,'2008-01-01 09:17:04',2415919104,160565,44100,126553,0,2048765375,'\\Music\\Digital\\The White Stripes\\Elephant\\14 It''s True That We Love One Anothe.m4a',0,'audio/mp4','2003-01-02 00:00:00',14,'It''s True That We Love One Another',1377891040,1763558972,1970654874,2414755838)
SELECT * FROM PlaylistSongInfo WHERE SongId=459347041
SELECT VolumeId FROM Music WHERE UniqueId=445579849
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1840395811,'De Stijl',1,1377891040,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(445579849,0,'2008-01-01 09:17:05',2415919104,109786,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\01-You''re Pretty Good Looking.mp3',0,'audio/mpeg','2000-01-02 00:00:00',1,'You''re Pretty Good Looking',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=445579849
SELECT VolumeId FROM Music WHERE UniqueId=456740214
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(456740214,0,'2008-01-01 09:17:05',2415919104,156336,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\02-Hello Operator.mp3',0,'audio/mpeg','2000-01-02 00:00:00',2,'Hello Operator',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=456740214
SELECT VolumeId FROM Music WHERE UniqueId=329221268
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(329221268,0,'2008-01-01 09:17:06',2415919104,186874,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\03-Little Bird.mp3',0,'audio/mpeg','2000-01-02 00:00:00',3,'Little Bird',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=329221268
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=389281714
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(389281714,0,'2008-01-01 09:17:16',2415919104,133375,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\04-Apple Blossom.mp3',0,'audio/mpeg','2000-01-02 00:00:00',4,'Apple Blossom',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=389281714
SELECT VolumeId FROM Music WHERE UniqueId=522271777
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(522271777,0,'2008-01-01 09:17:16',2415919104,190034,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\05-I''m Bound to Pack It Up.mp3',0,'audio/mpeg','2000-01-02 00:00:00',5,'I''m Bound to Pack It Up',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=522271777
SELECT VolumeId FROM Music WHERE UniqueId=505713420
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(505713420,0,'2008-01-01 09:17:17',2415919104,270021,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\06-Death Letter.mp3',0,'audio/mpeg','2000-01-02 00:00:00',6,'Death Letter',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=505713420
SELECT VolumeId FROM Music WHERE UniqueId=351066040
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(351066040,0,'2008-01-01 09:17:17',2415919104,171905,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\07-Sister, Do You Know My Name_.mp3',0,'audio/mpeg','2000-01-02 00:00:00',7,'Sister, Do You Know My Name?',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=351066040
SELECT VolumeId FROM Music WHERE UniqueId=461094883
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(461094883,0,'2008-01-01 09:17:17',2415919104,194606,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\08-Truth Doesn''t Make a Noise.mp3',0,'audio/mpeg','2000-01-02 00:00:00',8,'Truth Doesn''t Make a Noise',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=461094883
SELECT VolumeId FROM Music WHERE UniqueId=468496579
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(468496579,0,'2008-01-01 09:17:18',2415919104,262158,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\09-A Boy''s Best Friend.mp3',0,'audio/mpeg','2000-01-02 00:00:00',9,'A Boy''s Best Friend',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=468496579
SELECT VolumeId FROM Music WHERE UniqueId=524300687
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(524300687,0,'2008-01-01 09:17:18',2415919104,118459,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\10-Let''s Build a Home.mp3',0,'audio/mpeg','2000-01-02 00:00:00',10,'Let''s Build a Home',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=524300687
SELECT VolumeId FROM Music WHERE UniqueId=480500715
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(480500715,0,'2008-01-01 09:17:19',2415919104,113678,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\11-Jumble, Jumble.mp3',0,'audio/mpeg','2000-01-02 00:00:00',11,'Jumble, Jumble',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=480500715
SELECT VolumeId FROM Music WHERE UniqueId=299422943
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(299422943,0,'2008-01-01 09:17:19',2415919104,202312,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\12-Why Can''t You Be Nicer to Me_.mp3',0,'audio/mpeg','2000-01-02 00:00:00',12,'Why Can''t You Be Nicer to Me?',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=299422943
SELECT VolumeId FROM Music WHERE UniqueId=464030048
SELECT * FROM Artist WHERE UniqueId=1377891040
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1377891040
SELECT * FROM Album WHERE UniqueId=1840395811
SELECT * FROM Album WHERE UniqueId=184039581
SELECT * FROM Artist WHERE UniqueId=137789104
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840395811
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(464030048,0,'2008-01-01 09:17:19',2415919104,149100,44100,128000,0,2048765375,'\\Music\\Digital\\The White Stripes\\De Stijl\\13-Your Southern Can Is Mine.mp3',0,'audio/mpeg','2000-01-02 00:00:00',13,'Your Southern Can Is Mine',1377891040,1840395811,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=464030048
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=499478267
SELECT * FROM Artist WHERE UniqueId=1435621465
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1435621465,'The View',1)
SELECT * FROM Album WHERE UniqueId=1809168219
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1809168219,'Hats Off To The Buskers',1,1435621465,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2238863746,'Kieren Webster/Kyle Falconer',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(499478267,0,'2008-01-01 09:17:33',2415919104,179443,44100,126695,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\01 Comin'' Down.m4a',0,'audio/mp4','2007-01-02 00:00:00',1,'Comin'' Down',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=499478267
SELECT VolumeId FROM Music WHERE UniqueId=372141419
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(372141419,0,'2008-01-01 09:17:33',2415919104,195628,44100,126874,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\02 Superstar Tradesman.m4a',0,'audio/mp4','2007-01-02 00:00:00',2,'Superstar Tradesman',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=372141419
SELECT VolumeId FROM Music WHERE UniqueId=401158678
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(401158678,0,'2008-01-01 09:17:33',2415919104,215156,44100,126933,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\03 Same Jeans.m4a',0,'audio/mp4','2007-01-02 00:00:00',3,'Same Jeans',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=401158678
SELECT VolumeId FROM Music WHERE UniqueId=339109690
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(339109690,0,'2008-01-01 09:17:34',2415919104,204637,44100,126870,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\04 Don''t Tell Me.m4a',0,'audio/mp4','2007-01-02 00:00:00',4,'Don''t Tell Me',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=339109690
SELECT VolumeId FROM Music WHERE UniqueId=330627461
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(330627461,0,'2008-01-01 09:17:34',2415919104,182996,44100,126721,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\05 Skag Trendy.m4a',0,'audio/mp4','2007-01-02 00:00:00',5,'Skag Trendy',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=330627461
SELECT VolumeId FROM Music WHERE UniqueId=496504875
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(496504875,0,'2008-01-01 09:17:35',2415919104,191448,44100,126749,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\06 The Don.m4a',0,'audio/mp4','2007-01-02 00:00:00',6,'The Don',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=496504875
SELECT VolumeId FROM Music WHERE UniqueId=286805325
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(286805325,0,'2008-01-01 09:17:36',2415919104,200434,44100,126842,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\07 Face For The Radio.m4a',0,'audio/mp4','2007-01-02 00:00:00',7,'Face For The Radio',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=286805325
SELECT VolumeId FROM Music WHERE UniqueId=279592361
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(279592361,0,'2008-01-01 09:17:36',2415919104,239211,44100,127097,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\08 Wasted Little DJ''s.m4a',0,'audio/mp4','2007-01-02 00:00:00',8,'Wasted Little DJ''s',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=279592361
SELECT VolumeId FROM Music WHERE UniqueId=483036778
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(483036778,0,'2008-01-01 09:17:37',2415919104,155829,44100,126486,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\09 Gran''s For Tea.m4a',0,'audio/mp4','2007-01-02 00:00:00',9,'Gran''s For Tea',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=483036778
SELECT VolumeId FROM Music WHERE UniqueId=333377476
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(333377476,0,'2008-01-01 09:17:39',2415919104,187083,44100,126816,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\10 Dance Into The Night.m4a',0,'audio/mp4','2007-01-02 00:00:00',10,'Dance Into The Night',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=333377476
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=436227456
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(436227456,0,'2008-01-01 09:18:24',2415919104,162771,44100,126523,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\11 Claudia.m4a',0,'audio/mp4','2007-01-02 00:00:00',11,'Claudia',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=436227456
SELECT VolumeId FROM Music WHERE UniqueId=377901523
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(377901523,0,'2008-01-01 09:18:25',2415919104,179258,44100,126684,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\12 Street Lights.m4a',0,'audio/mp4','2007-01-02 00:00:00',12,'Street Lights',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=377901523
SELECT VolumeId FROM Music WHERE UniqueId=522167618
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(522167618,0,'2008-01-01 09:18:28',2415919104,148375,44100,126380,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\13 Wasteland.m4a',0,'audio/mp4','2007-01-02 00:00:00',13,'Wasteland',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=522167618
SELECT VolumeId FROM Music WHERE UniqueId=363995610
SELECT * FROM Artist WHERE UniqueId=1435621465
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1435621465
SELECT * FROM Album WHERE UniqueId=1809168219
SELECT * FROM Album WHERE UniqueId=180916821
SELECT * FROM Artist WHERE UniqueId=143562146
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1809168219
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2238863746
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2238863746
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(363995610,0,'2008-01-01 09:18:30',2415919104,35062,44100,127812,0,2048765375,'\\Music\\Digital\\The View\\Hats Off To The Buskers\\14 Typical Time.m4a',0,'audio/mp4','2007-01-02 00:00:00',14,'Typical Time',1435621465,1809168219,1970654874,2238863746)
SELECT * FROM PlaylistSongInfo WHERE SongId=363995610
SELECT VolumeId FROM Music WHERE UniqueId=278583047
SELECT * FROM Artist WHERE UniqueId=1502354900
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1502354900,'',1)
SELECT * FROM Album WHERE UniqueId=1770790356
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1770790356,'',1,1502354900,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(278583047,0,'2008-01-01 09:18:31',2415919104,357689,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\01-Bitter sweet symphony.mp3',0,'audio/mpeg',1,'Bitter sweet symphony',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=278583047
SELECT VolumeId FROM Music WHERE UniqueId=400739347
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(400739347,0,'2008-01-01 09:18:32',2415919104,260893,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\02-Sonnet.mp3',0,'audio/mpeg',2,'Sonnet',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=400739347
SELECT VolumeId FROM Music WHERE UniqueId=295599115
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(295599115,0,'2008-01-01 09:18:32',2415919104,421073,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\03-The rolling people.mp3',0,'audio/mpeg',3,'The rolling people',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=295599115
SELECT VolumeId FROM Music WHERE UniqueId=443157889
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(443157889,0,'2008-01-01 09:18:33',2415919104,304444,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\04-The drugs don''t work.mp3',0,'audio/mpeg',4,'The drugs don''t work',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=443157889
SELECT VolumeId FROM Music WHERE UniqueId=396411770
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(396411770,0,'2008-01-01 09:18:33',2415919104,385915,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\05-Catching the butterfly.mp3',0,'audio/mpeg',5,'Catching the butterfly',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=396411770
SELECT VolumeId FROM Music WHERE UniqueId=341757281
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(341757281,0,'2008-01-01 09:18:34',2415919104,157451,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\06-Neon wilderness.mp3',0,'audio/mpeg',6,'Neon wilderness',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=341757281
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=519891761
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(519891761,0,'2008-01-01 09:18:57',2415919104,335927,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\07-Space and time.mp3',0,'audio/mpeg',7,'Space and time',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=519891761
SELECT VolumeId FROM Music WHERE UniqueId=453469954
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(453469954,0,'2008-01-01 09:18:58',2415919104,289327,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\08-Weeping willow.mp3',0,'audio/mpeg',8,'Weeping willow',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=453469954
SELECT VolumeId FROM Music WHERE UniqueId=463657993
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(463657993,0,'2008-01-01 09:18:59',2415919104,292872,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\09-Lucky man.mp3',0,'audio/mpeg',9,'Lucky man',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=463657993
SELECT VolumeId FROM Music WHERE UniqueId=326179961
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(326179961,0,'2008-01-01 09:19:00',2415919104,302359,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\10-One day.mp3',0,'audio/mpeg',10,'One day',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=326179961
SELECT VolumeId FROM Music WHERE UniqueId=358489384
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(358489384,0,'2008-01-01 09:19:01',2415919104,230426,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\11-This time.mp3',0,'audio/mpeg',11,'This time',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=358489384
SELECT VolumeId FROM Music WHERE UniqueId=501224877
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(501224877,0,'2008-01-01 09:19:02',2415919104,296729,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\12-Velvet morning.mp3',0,'audio/mpeg',12,'Velvet morning',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=501224877
SELECT VolumeId FROM Music WHERE UniqueId=395959806
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1770790356
SELECT * FROM Album WHERE UniqueId=177079035
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1770790356
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(395959806,0,'2008-01-01 09:19:03',2415919104,913290,44100,128000,0,2048765375,'\\Music\\Digital\\The Verve\\Urban Hymns\\13-Come on.mp3',0,'audio/mpeg',13,'Come on',1502354900,1770790356,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=395959806
SELECT VolumeId FROM Music WHERE UniqueId=436988274
SELECT * FROM Artist WHERE UniqueId=1445550852
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1445550852,'The Strokes',1)
SELECT * FROM Album WHERE UniqueId=1703521124
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1703521124,'Room on Fire',1,1445550852,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(436988274,0,'2008-01-01 09:19:04',2415919104,166609,44100,128000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\01 What Ever Happened_.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',1,'What Ever Happened?',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=436988274
SELECT VolumeId FROM Music WHERE UniqueId=413148520
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(413148520,0,'2008-01-01 09:19:05',2415919104,213786,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\02 Reptilia.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',2,'Reptilia',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=413148520
SELECT VolumeId FROM Music WHERE UniqueId=379336981
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2167930792
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2167930792,'Albert Hammond Jr.',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(379336981,0,'2008-01-01 09:19:06',2415919104,199549,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\03 Automatic.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',3,'Automatic',1445550852,1703521124,2114860941,2167930792)
SELECT * FROM PlaylistSongInfo WHERE SongId=379336981
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=524369029
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2196002761
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2196002761,'J. Casablancas',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(524369029,0,'2008-01-01 09:19:21',2415919104,145606,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\04 12_51.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',4,'12:51',1445550852,1703521124,2114860941,2196002761)
SELECT * FROM PlaylistSongInfo WHERE SongId=524369029
SELECT VolumeId FROM Music WHERE UniqueId=421557165
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(421557165,0,'2008-01-01 09:19:21',2415919104,177528,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\05 You Talk Way Too Much.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',5,'You Talk Way Too Much',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=421557165
SELECT VolumeId FROM Music WHERE UniqueId=382349883
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(382349883,0,'2008-01-01 09:19:22',2415919104,187533,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\06 Between Love and Hate.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',6,'Between Love and Hate',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=382349883
SELECT VolumeId FROM Music WHERE UniqueId=479685479
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(479685479,0,'2008-01-01 09:19:22',2415919104,170004,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\07 Meet me in the Bathroom.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',7,'Meet me in the Bathroom',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=479685479
SELECT VolumeId FROM Music WHERE UniqueId=391665005
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(391665005,0,'2008-01-01 09:19:23',2415919104,179748,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\08 Under Control.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',8,'Under Control',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=391665005
SELECT VolumeId FROM Music WHERE UniqueId=373149816
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(373149816,0,'2008-01-01 09:19:23',2415919104,178729,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\09 The End Has No End.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',9,'The End Has No End',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=373149816
SELECT VolumeId FROM Music WHERE UniqueId=387968555
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(387968555,0,'2008-01-01 09:19:23',2415919104,136437,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\10 The Way It is.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',10,'The Way It is',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=387968555
SELECT VolumeId FROM Music WHERE UniqueId=369445053
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1703521124
SELECT * FROM Album WHERE UniqueId=170352112
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1703521124
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(369445053,0,'2008-01-01 09:19:24',2415919104,148767,44100,160000,0,2048765375,'\\Music\\Digital\\The Strokes\\Room on Fire\\11 I Cant Win.mp3',0,'audio/mpeg','11+CD9251FC9F04C71F4B9A43662F935278+4845110','2003-01-02 00:00:00',11,'I Cant Win',1445550852,1703521124,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=369445053
SELECT VolumeId FROM Music WHERE UniqueId=461622292
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1873999532,'Is This It?',1,1445550852,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(461622292,0,'2008-01-01 09:19:24',2415919104,155573,44100,127968,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\01 Is This It.m4a',0,'audio/mp4','2002-01-02 00:00:00',1,'Is This It',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=461622292
SELECT VolumeId FROM Music WHERE UniqueId=433624098
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(433624098,0,'2008-01-01 09:19:24',2415919104,213041,44100,125594,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\02 The Modern Age.m4a',0,'audio/mp4','2002-01-02 00:00:00',2,'The Modern Age',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=433624098
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=367470978
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(367470978,0,'2008-01-01 09:19:37',2415919104,157663,44100,125279,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\03 Soma.m4a',0,'audio/mp4','2002-01-02 00:00:00',3,'Soma',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=367470978
SELECT VolumeId FROM Music WHERE UniqueId=459564030
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(459564030,0,'2008-01-01 09:19:38',2415919104,238491,44100,126337,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\04 Barely Legal.m4a',0,'audio/mp4','2002-01-02 00:00:00',4,'Barely Legal',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=459564030
SELECT VolumeId FROM Music WHERE UniqueId=354107668
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(354107668,0,'2008-01-01 09:19:38',2415919104,187058,44100,128012,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\05 Someday.m4a',0,'audio/mp4','2002-01-02 00:00:00',5,'Someday',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=354107668
SELECT VolumeId FROM Music WHERE UniqueId=315671797
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(315671797,0,'2008-01-01 09:19:39',2415919104,192353,44100,125943,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\06 Alone, Together.m4a',0,'audio/mp4','2002-01-02 00:00:00',6,'Alone, Together',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=315671797
SELECT VolumeId FROM Music WHERE UniqueId=320316553
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(320316553,0,'2008-01-01 09:19:39',2415919104,197903,44100,125394,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\07 Last Nite.m4a',0,'audio/mp4','2002-01-02 00:00:00',7,'Last Nite',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=320316553
SELECT VolumeId FROM Music WHERE UniqueId=345303783
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(345303783,0,'2008-01-01 09:19:40',2415919104,227786,44100,126208,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\08 Hard To Explain.m4a',0,'audio/mp4','2002-01-02 00:00:00',8,'Hard To Explain',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=345303783
SELECT VolumeId FROM Music WHERE UniqueId=285162103
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(285162103,0,'2008-01-01 09:19:40',2415919104,177121,44100,126944,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\09 When It Started.m4a',0,'audio/mp4','2002-01-02 00:00:00',9,'When It Started',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=285162103
SELECT VolumeId FROM Music WHERE UniqueId=394561823
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(394561823,0,'2008-01-01 09:19:41',2415919104,207771,44100,125403,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\10 Trying Your Luck.m4a',0,'audio/mp4','2002-01-02 00:00:00',10,'Trying Your Luck',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=394561823
SELECT VolumeId FROM Music WHERE UniqueId=456218393
SELECT * FROM Artist WHERE UniqueId=1445550852
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1445550852
SELECT * FROM Album WHERE UniqueId=1873999532
SELECT * FROM Album WHERE UniqueId=187399953
SELECT * FROM Artist WHERE UniqueId=144555085
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1873999532
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(456218393,0,'2008-01-01 09:19:41',2415919104,196580,44100,127948,0,2048765375,'\\Music\\Digital\\The Strokes\\Is This It_\\11 Take It Or Leave It.m4a',0,'audio/mp4','2002-01-02 00:00:00',11,'Take It Or Leave It',1445550852,1873999532,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=456218393
SELECT VolumeId FROM Music WHERE UniqueId=493016057
SELECT * FROM Artist WHERE UniqueId=1604998888
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1604998888,'The Shins',1)
SELECT * FROM Album WHERE UniqueId=1649648454
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1649648454,'Chutes Too Narrow',1,1604998888,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(493016057,0,'2008-01-01 09:19:42',2415919104,199073,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\01-Kissing The Lipless.mp3',0,'audio/mpeg',1,'Kissing The Lipless',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=493016057
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=524865904
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(524865904,0,'2008-01-01 09:19:53',2415919104,199829,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\02-Mine''s Not  A High Horse.mp3',0,'audio/mpeg',2,'Mine''s Not  A High Horse',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=524865904
SELECT VolumeId FROM Music WHERE UniqueId=445990101
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(445990101,0,'2008-01-01 09:19:53',2415919104,168163,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\03-So Says I.mp3',0,'audio/mpeg',3,'So Says I',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=445990101
SELECT VolumeId FROM Music WHERE UniqueId=382253771
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(382253771,0,'2008-01-01 09:19:54',2415919104,167225,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\04-Young Pligrams.mp3',0,'audio/mpeg',4,'Young Pligrams',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=382253771
SELECT VolumeId FROM Music WHERE UniqueId=419800846
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(419800846,0,'2008-01-01 09:19:55',2415919104,264907,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\05-Saint Simon.mp3',0,'audio/mpeg',5,'Saint Simon',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=419800846
SELECT VolumeId FROM Music WHERE UniqueId=523789105
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(523789105,0,'2008-01-01 09:19:55',2415919104,146401,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\06-Fighting In A Sack.mp3',0,'audio/mpeg',6,'Fighting In A Sack',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=523789105
SELECT VolumeId FROM Music WHERE UniqueId=392793596
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(392793596,0,'2008-01-01 09:19:56',2415919104,232824,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\07-Pink Bullets.mp3',0,'audio/mpeg',7,'Pink Bullets',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=392793596
SELECT VolumeId FROM Music WHERE UniqueId=435280305
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(435280305,0,'2008-01-01 09:19:57',2415919104,190681,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\08-Turn A Square.mp3',0,'audio/mpeg',8,'Turn A Square',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=435280305
SELECT VolumeId FROM Music WHERE UniqueId=278292786
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(278292786,0,'2008-01-01 09:19:57',2415919104,193105,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\09-Gone For Good.mp3',0,'audio/mpeg',9,'Gone For Good',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=278292786
SELECT VolumeId FROM Music WHERE UniqueId=435732783
SELECT * FROM Artist WHERE UniqueId=1604998888
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1604998888
SELECT * FROM Album WHERE UniqueId=1649648454
SELECT * FROM Album WHERE UniqueId=164964845
SELECT * FROM Artist WHERE UniqueId=160499888
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1649648454
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(435732783,0,'2008-01-01 09:19:58',2415919104,264255,44100,128000,0,2048765375,'\\Music\\Digital\\The Shins\\10-Those To Come.mp3',0,'audio/mpeg',10,'Those To Come',1604998888,1649648454,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=435732783
SELECT VolumeId FROM Music WHERE UniqueId=307228182
SELECT * FROM Artist WHERE UniqueId=1549739650
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1549739650,'The Pigeon Detectives',1)
SELECT * FROM Album WHERE UniqueId=1743818356
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1743818356,'Wait For Me',1,1549739650,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2306229224,'Bowman & Main',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(307228182,0,'2008-01-01 09:19:58',2415919104,159126,44100,126260,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\01 Romantic Type.m4a',0,'audio/mp4','2007-01-02 00:00:00',1,'Romantic Type',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=307228182
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=386392130
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(386392130,0,'2008-01-01 09:20:11',2415919104,129451,44100,126160,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\02 I Found Out.m4a',0,'audio/mp4','2007-01-02 00:00:00',2,'I Found Out',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=386392130
SELECT VolumeId FROM Music WHERE UniqueId=503554373
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(503554373,0,'2008-01-01 09:20:11',2415919104,118770,44100,125946,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\03 Don''t Know How To Say Goodbye.m4a',0,'audio/mp4','2007-01-02 00:00:00',3,'Don''t Know How To Say Goodbye',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=503554373
SELECT VolumeId FROM Music WHERE UniqueId=401006556
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2280530588
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2280530588,'Main',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(401006556,0,'2008-01-01 09:20:12',2415919104,125341,44100,126038,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\04 Caught In Your Trap.m4a',0,'audio/mp4','2007-01-02 00:00:00',4,'Caught In Your Trap',1549739650,1743818356,2114860941,2280530588)
SELECT * FROM PlaylistSongInfo WHERE SongId=401006556
SELECT VolumeId FROM Music WHERE UniqueId=289777651
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(289777651,0,'2008-01-01 09:20:12',2415919104,168205,44100,126617,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\05 I Can''t Control Myself.m4a',0,'audio/mp4','2007-01-02 00:00:00',5,'I Can''t Control Myself',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=289777651
SELECT VolumeId FROM Music WHERE UniqueId=328568343
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(328568343,0,'2008-01-01 09:20:13',2415919104,224165,44100,127009,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\06 I''m Not Sorry.m4a',0,'audio/mp4','2007-01-02 00:00:00',6,'I''m Not Sorry',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=328568343
SELECT VolumeId FROM Music WHERE UniqueId=501132254
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(501132254,0,'2008-01-01 09:20:13',2415919104,185782,44100,126723,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\07 You Know I Love You.m4a',0,'audio/mp4','2007-01-02 00:00:00',7,'You Know I Love You',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=501132254
SELECT VolumeId FROM Music WHERE UniqueId=278338777
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(278338777,0,'2008-01-01 09:20:14',2415919104,194211,44100,126807,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\08 Stop Or Go 1.m4a',0,'audio/mp4','2007-01-02 00:00:00',8,'Stop Or Go',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=278338777
SELECT VolumeId FROM Music WHERE UniqueId=501298388
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(501298388,0,'2008-01-01 09:20:14',2415919104,268608,44100,127207,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\09 You Better Not Look My Way 1.m4a',0,'audio/mp4','2007-01-02 00:00:00',9,'You Better Not Look My Way',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=501298388
SELECT VolumeId FROM Music WHERE UniqueId=490095428
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2228547535
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2228547535,'Best, Bowman, Main, Naylor & Wilson',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(490095428,0,'2008-01-01 09:20:15',2415919104,199203,44100,126855,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\10 Take Her Back 1.m4a',0,'audio/mp4','2007-01-02 00:00:00',10,'Take Her Back',1549739650,1743818356,2114860941,2228547535)
SELECT * FROM PlaylistSongInfo WHERE SongId=490095428
SELECT VolumeId FROM Music WHERE UniqueId=346547623
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2306229224
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2306229224
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(346547623,0,'2008-01-01 09:20:15',2415919104,134675,44100,126207,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\11 Wait For Me 1.m4a',0,'audio/mp4','2007-01-02 00:00:00',11,'Wait For Me',1549739650,1743818356,2114860941,2306229224)
SELECT * FROM PlaylistSongInfo WHERE SongId=346547623
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=500148111
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1743818356
SELECT * FROM Album WHERE UniqueId=174381835
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1743818356
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2280530588
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2280530588
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(500148111,0,'2008-01-01 09:20:30',2415919104,181463,44100,126685,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Wait For Me\\12 I''m Always Right 1.m4a',0,'audio/mp4','2007-01-02 00:00:00',12,'I''m Always Right',1549739650,1743818356,2114860941,2280530588)
SELECT * FROM PlaylistSongInfo WHERE SongId=500148111
SELECT VolumeId FROM Music WHERE UniqueId=286723377
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1611567053,'Emergency (Bonus Track Version)',1,1549739650,'')
SELECT * FROM Genre WHERE UniqueId=1915387647
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(1915387647,'Pop',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(286723377,0,'2008-01-01 09:20:30',2415919104,165535,44100,125710,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Don''t You Wanna Find Out.m4a',0,'audio/mp4','2008-01-02 00:00:00',4,'Don''t You Wanna Find Out',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=286723377
SELECT VolumeId FROM Music WHERE UniqueId=478827171
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(478827171,0,'2008-01-01 09:20:31',2415919104,315187,44100,125151,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Everybody Wants Me.m4a',0,'audio/mp4','2008-01-02 00:00:00',13,'Everybody Wants Me',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=478827171
SELECT VolumeId FROM Music WHERE UniqueId=324482761
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(324482761,0,'2008-01-01 09:20:31',2415919104,200550,44100,127472,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\I''ll Be Waiting.m4a',0,'audio/mp4','2008-01-02 00:00:00',5,'I''ll Be Waiting',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=324482761
SELECT VolumeId FROM Music WHERE UniqueId=349470285
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(349470285,0,'2008-01-01 09:20:32',2415919104,198275,44100,127481,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\I''m A Liar.m4a',0,'audio/mp4','2008-01-02 00:00:00',8,'I''m A Liar',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=349470285
SELECT VolumeId FROM Music WHERE UniqueId=404383345
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(404383345,0,'2008-01-01 09:20:32',2415919104,173035,44100,129970,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\I''m Not Gonna Take This (Live at Lon.m4a',0,'audio/mp4','2008-01-02 00:00:00',14,'I''m Not Gonna Take This (Live at London Astoria)',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=404383345
SELECT VolumeId FROM Music WHERE UniqueId=434915599
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(434915599,0,'2008-01-01 09:20:33',2415919104,165465,44100,124515,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\I''m Not Gonna Take This.m4a',0,'audio/mp4','2008-01-02 00:00:00',2,'I''m Not Gonna Take This',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=434915599
SELECT VolumeId FROM Music WHERE UniqueId=471109267
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(471109267,0,'2008-01-01 09:20:34',2415919104,293593,44100,128786,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Keep On Your Dress.m4a',0,'audio/mp4','2008-01-02 00:00:00',3,'Keep On Your Dress',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=471109267
SELECT VolumeId FROM Music WHERE UniqueId=327053210
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(327053210,0,'2008-01-01 09:20:34',2415919104,204056,44100,124089,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Love You For A Day (Hate You For A W.m4a',0,'audio/mp4','2008-01-02 00:00:00',11,'Love You For A Day (Hate You For A Week)',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=327053210
SELECT VolumeId FROM Music WHERE UniqueId=525224846
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(525224846,0,'2008-01-01 09:20:35',2415919104,209049,44100,124726,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Making Up Numbers.m4a',0,'audio/mp4','2008-01-02 00:00:00',12,'Making Up Numbers',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=525224846
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=398660025
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(398660025,0,'2008-01-01 09:20:50',2415919104,111502,44100,121757,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Nothing To Do With You.m4a',0,'audio/mp4','2008-01-02 00:00:00',7,'Nothing To Do With You',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=398660025
SELECT VolumeId FROM Music WHERE UniqueId=372702238
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(372702238,0,'2008-01-01 09:20:51',2415919104,165233,44100,127301,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\Say It Like You Mean It.m4a',0,'audio/mp4','2008-01-02 00:00:00',10,'Say It Like You Mean It',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=372702238
SELECT VolumeId FROM Music WHERE UniqueId=383170638
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(383170638,0,'2008-01-01 09:20:51',2415919104,129404,44100,123873,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\She''s Gone.m4a',0,'audio/mp4','2008-01-02 00:00:00',6,'She''s Gone',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=383170638
SELECT VolumeId FROM Music WHERE UniqueId=440308691
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(440308691,0,'2008-01-01 09:20:51',2415919104,193050,44100,127085,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\This Is An Emergency.m4a',0,'audio/mp4','2008-01-02 00:00:00',1,'This Is An Emergency',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=440308691
SELECT VolumeId FROM Music WHERE UniqueId=284641402
SELECT * FROM Artist WHERE UniqueId=1549739650
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1549739650
SELECT * FROM Album WHERE UniqueId=1611567053
SELECT * FROM Album WHERE UniqueId=161156705
SELECT * FROM Artist WHERE UniqueId=154973965
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1611567053
SELECT * FROM Genre WHERE UniqueId=1915387647
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1915387647
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(284641402,0,'2008-01-01 09:20:52',2415919104,169412,44100,124927,0,2048765375,'\\Music\\Digital\\The Pigeon Detectives\\Emergency (Bonus Track Version)\\You Don''t Need It.m4a',0,'audio/mp4','2008-01-02 00:00:00',9,'You Don''t Need It',1549739650,1611567053,1915387647,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=284641402
SELECT VolumeId FROM Music WHERE UniqueId=348354197
SELECT * FROM Artist WHERE UniqueId=1358309945
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1358309945,'The Last Shadow Puppets',1)
SELECT * FROM Album WHERE UniqueId=1764999021
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1764999021,'The Age Of The Understatement',1,1358309945,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2230935138,'Alex Turner & Miles Kane',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(348354197,0,'2008-01-01 09:20:52',2415919104,241719,44100,126457,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Black Plant.m4a',0,'audio/mp4','2008-01-02 00:00:00',8,'Black Plant',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=348354197
SELECT VolumeId FROM Music WHERE UniqueId=503263343
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(503263343,0,'2008-01-01 09:20:53',2415919104,148329,44100,123611,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Calm Like You.m4a',0,'audio/mp4','2008-01-02 00:00:00',3,'Calm Like You',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=503263343
SELECT VolumeId FROM Music WHERE UniqueId=299721334
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(299721334,0,'2008-01-01 09:20:53',2415919104,187152,44100,123596,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\I Don''t Like You Anymore.m4a',0,'audio/mp4','2008-01-02 00:00:00',9,'I Don''t Like You Anymore',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=299721334
SELECT VolumeId FROM Music WHERE UniqueId=273015646
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(273015646,0,'2008-01-01 09:20:54',2415919104,151463,44100,121985,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\In My Room.m4a',0,'audio/mp4','2008-01-02 00:00:00',10,'In My Room',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=273015646
SELECT VolumeId FROM Music WHERE UniqueId=352166155
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(352166155,0,'2008-01-01 09:20:54',2415919104,238027,44100,125192,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Meeting Place.m4a',0,'audio/mp4','2008-01-02 00:00:00',11,'Meeting Place',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=352166155
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=362548246
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(362548246,0,'2008-01-01 09:21:08',2415919104,186595,44100,123497,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\My Mistakes Were Made For You.m4a',0,'audio/mp4','2008-01-02 00:00:00',7,'My Mistakes Were Made For You',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=362548246
SELECT VolumeId FROM Music WHERE UniqueId=298052670
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(298052670,0,'2008-01-01 09:21:09',2415919104,166138,44100,120354,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Only The Truth.m4a',0,'audio/mp4','2008-01-02 00:00:00',6,'Only The Truth',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=298052670
SELECT VolumeId FROM Music WHERE UniqueId=332899230
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(332899230,0,'2008-01-01 09:21:09',2415919104,160473,44100,122121,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Separate And Ever Deadly.m4a',0,'audio/mp4','2008-01-02 00:00:00',4,'Separate And Ever Deadly',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=332899230
SELECT VolumeId FROM Music WHERE UniqueId=387996622
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(387996622,0,'2008-01-01 09:21:10',2415919104,140480,44100,123554,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Standing Next To Me.m4a',0,'audio/mp4','2008-01-02 00:00:00',2,'Standing Next To Me',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=387996622
SELECT VolumeId FROM Music WHERE UniqueId=483687967
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(483687967,0,'2008-01-01 09:21:11',2415919104,189614,44100,124263,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\The Age Of The Understatement.m4a',0,'audio/mp4','2008-01-02 00:00:00',1,'The Age Of The Understatement',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=483687967
SELECT VolumeId FROM Music WHERE UniqueId=421248075
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(421248075,0,'2008-01-01 09:21:11',2415919104,159196,44100,125263,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\The Chamber.m4a',0,'audio/mp4','2008-01-02 00:00:00',5,'The Chamber',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=421248075
SELECT VolumeId FROM Music WHERE UniqueId=303439702
SELECT * FROM Artist WHERE UniqueId=1358309945
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358309945
SELECT * FROM Album WHERE UniqueId=1764999021
SELECT * FROM Album WHERE UniqueId=176499902
SELECT * FROM Artist WHERE UniqueId=135830994
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1764999021
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2230935138
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2230935138
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(303439702,0,'2008-01-01 09:21:12',2415919104,142245,44100,127159,0,2048765375,'\\Music\\Digital\\The Last Shadow Puppets\\The Age Of The Understatement\\Time Has Come Again.m4a',0,'audio/mp4','2008-01-02 00:00:00',12,'Time Has Come Again',1358309945,1764999021,1970654874,2230935138)
SELECT * FROM PlaylistSongInfo WHERE SongId=303439702
SELECT VolumeId FROM Music WHERE UniqueId=350572701
SELECT * FROM Artist WHERE UniqueId=1550310333
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1550310333,'The Kooks',1)
SELECT * FROM Album WHERE UniqueId=1845086585
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1845086585,'Konk',1,1550310333,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2355616701,'The Kooks',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(350572701,0,'2008-01-01 09:21:13',2415919104,164257,44100,126083,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Always Where I Need To Be.m4a',0,'audio/mp4','2008-01-02 00:00:00',2,'Always Where I Need To Be',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=350572701
SELECT VolumeId FROM Music WHERE UniqueId=455226534
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(455226534,0,'2008-01-01 09:21:13',2415919104,248476,44100,127375,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Do You Wanna.m4a',0,'audio/mp4','2008-01-02 00:00:00',4,'Do You Wanna',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=455226534
SELECT VolumeId FROM Music WHERE UniqueId=468439367
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(468439367,0,'2008-01-01 09:21:13',2415919104,149722,44100,126005,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Down To The Market.m4a',0,'audio/mp4','2008-01-02 00:00:00',10,'Down To The Market',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=468439367
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=328326404
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(328326404,0,'2008-01-01 09:21:33',2415919104,242555,44100,127656,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Gap.m4a',0,'audio/mp4','2008-01-02 00:00:00',5,'Gap',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=328326404
SELECT VolumeId FROM Music WHERE UniqueId=454895617
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(454895617,0,'2008-01-01 09:21:34',2415919104,174103,44100,127429,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Love It All.m4a',0,'audio/mp4','2008-01-02 00:00:00',6,'Love It All',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=454895617
SELECT VolumeId FROM Music WHERE UniqueId=397138943
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(397138943,0,'2008-01-01 09:21:35',2415919104,182253,44100,123121,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Mr. Maker.m4a',0,'audio/mp4','2008-01-02 00:00:00',3,'Mr. Maker',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=397138943
SELECT VolumeId FROM Music WHERE UniqueId=289148204
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(289148204,0,'2008-01-01 09:21:36',2415919104,160867,44100,124249,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\One Last Time.m4a',0,'audio/mp4','2008-01-02 00:00:00',11,'One Last Time',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=289148204
SELECT VolumeId FROM Music WHERE UniqueId=527121392
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(527121392,0,'2008-01-01 09:21:38',2415919104,218987,44100,127404,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\See The Sun.m4a',0,'audio/mp4','2008-01-02 00:00:00',1,'See The Sun',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=527121392
SELECT VolumeId FROM Music WHERE UniqueId=296842228
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(296842228,0,'2008-01-01 09:21:39',2415919104,197601,44100,127093,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Shine On.m4a',0,'audio/mp4','2008-01-02 00:00:00',9,'Shine On',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=296842228
SELECT VolumeId FROM Music WHERE UniqueId=488746534
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(488746534,0,'2008-01-01 09:21:40',2415919104,244320,44100,126416,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Stormy Weather.m4a',0,'audio/mp4','2008-01-02 00:00:00',7,'Stormy Weather',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=488746534
SELECT VolumeId FROM Music WHERE UniqueId=417995263
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(417995263,0,'2008-01-01 09:21:41',2415919104,219057,44100,124925,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Sway.m4a',0,'audio/mp4','2008-01-02 00:00:00',8,'Sway',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=417995263
SELECT VolumeId FROM Music WHERE UniqueId=365802501
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1845086585
SELECT * FROM Album WHERE UniqueId=184508658
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1845086585
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2355616701
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2355616701
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(365802501,0,'2008-01-01 09:21:43',2415919104,461984,44100,121443,0,2048765375,'\\Music\\Digital\\The Kooks\\Konk\\Tick Of Time.m4a',0,'audio/mp4','2008-01-02 00:00:00',12,'Tick Of Time',1550310333,1845086585,1970654874,2355616701)
SELECT * FROM PlaylistSongInfo WHERE SongId=365802501
SELECT VolumeId FROM Music WHERE UniqueId=312656414
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1866956088,'Inside In/Inside Out Acoustic',1,1550310333,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2164845545,'Luke Pritchard, Hugh Harris, Paul Garred, Max Rafferty',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(312656414,0,'2008-01-01 09:21:43',2415919104,133863,44100,123192,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\California [Live in Osaka].m4a',0,'audio/mp4','2006-01-02 00:00:00',15,'California [Live in Osaka]',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=312656414
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=527064021
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(527064021,0,'2008-01-01 09:22:39',2415919104,130054,44100,124136,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Eddie''s Gun [Live in Osaka].m4a',0,'audio/mp4','2006-01-02 00:00:00',12,'Eddie''s Gun [Live in Osaka]',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=527064021
SELECT VolumeId FROM Music WHERE UniqueId=301127497
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(301127497,0,'2008-01-01 09:22:41',2415919104,146192,44100,128680,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Eddie''s Gun.m4a',0,'audio/mp4','2006-01-02 00:00:00',4,'Eddie''s Gun',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=301127497
SELECT VolumeId FROM Music WHERE UniqueId=454063727
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(454063727,0,'2008-01-01 09:22:43',2415919104,155132,44100,128518,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Jackie Big Tits.m4a',0,'audio/mp4','2006-01-02 00:00:00',10,'Jackie Big Tits',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=454063727
SELECT VolumeId FROM Music WHERE UniqueId=348917749
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(348917749,0,'2008-01-01 09:22:45',2415919104,134768,44100,127118,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Matchbox.m4a',0,'audio/mp4','2006-01-02 00:00:00',8,'Matchbox',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=348917749
SELECT VolumeId FROM Music WHERE UniqueId=391241539
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(391241539,0,'2008-01-01 09:22:47',2415919104,227509,44100,124943,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Naive [Live in Osaka].m4a',0,'audio/mp4','2006-01-02 00:00:00',13,'Naive [Live in Osaka]',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=391241539
SELECT VolumeId FROM Music WHERE UniqueId=402550888
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(402550888,0,'2008-01-01 09:22:49',2415919104,228879,44100,128712,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Naive.m4a',0,'audio/mp4','2006-01-02 00:00:00',9,'Naive',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=402550888
SELECT VolumeId FROM Music WHERE UniqueId=302541756
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(302541756,0,'2008-01-01 09:22:51',2415919104,205589,44100,128580,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Ooh La.m4a',0,'audio/mp4','2006-01-02 00:00:00',5,'Ooh La',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=302541756
SELECT VolumeId FROM Music WHERE UniqueId=272777988
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(272777988,0,'2008-01-01 09:22:53',2415919104,112152,44100,125975,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Seaside.m4a',0,'audio/mp4','2006-01-02 00:00:00',1,'Seaside',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=272777988
SELECT VolumeId FROM Music WHERE UniqueId=456048810
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(456048810,0,'2008-01-01 09:22:55',2415919104,148329,44100,127972,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\See The World.m4a',0,'audio/mp4','2006-01-02 00:00:00',2,'See The World',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=456048810
SELECT VolumeId FROM Music WHERE UniqueId=461439657
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(461439657,0,'2008-01-01 09:22:57',2415919104,178932,44100,124288,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\She Moves In Her Own Way [Live in Os.m4a',0,'audio/mp4','2006-01-02 00:00:00',14,'She Moves In Her Own Way [Live in Osaka]',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=461439657
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=369012821
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(369012821,0,'2008-01-01 09:23:11',2415919104,184668,44100,128815,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\She Moves In Her Own Way.m4a',0,'audio/mp4','2006-01-02 00:00:00',7,'She Moves In Her Own Way',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=369012821
SELECT VolumeId FROM Music WHERE UniqueId=390818142
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(390818142,0,'2008-01-01 09:23:11',2415919104,160031,44100,128490,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Sofa Song.m4a',0,'audio/mp4','2006-01-02 00:00:00',3,'Sofa Song',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=390818142
SELECT VolumeId FROM Music WHERE UniqueId=302502634
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(302502634,0,'2008-01-01 09:23:12',2415919104,296333,44100,125915,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\Time Awaits.m4a',0,'audio/mp4','2006-01-02 00:00:00',11,'Time Awaits',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=302502634
SELECT VolumeId FROM Music WHERE UniqueId=305710741
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1866956088
SELECT * FROM Album WHERE UniqueId=186695608
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1866956088
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2164845545
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2164845545
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(305710741,0,'2008-01-01 09:23:12',2415919104,188453,44100,128817,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside In_Inside Out Acoustic\\You Don''t Love Me.m4a',0,'audio/mp4','2006-01-02 00:00:00',6,'You Don''t Love Me',1550310333,1866956088,1970654874,2164845545)
SELECT * FROM PlaylistSongInfo WHERE SongId=305710741
SELECT VolumeId FROM Music WHERE UniqueId=449298919
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1720982438,'Inside in Inside Out',1,1550310333,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2263808999
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2263808999,'Hugh Harris/Luke Pritchard/Max Rafferty/Paul Carred',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(449298919,0,'2008-01-01 09:23:13',2415919104,126566,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Eddie''s Gun.mp3',0,'audio/mpeg','2006-01-02 00:00:00',4,'Eddie''s Gun',1550310333,1720982438,2114860941,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=449298919
SELECT VolumeId FROM Music WHERE UniqueId=430307071
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=2060465507
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(2060465507,'Other',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(430307071,0,'2008-01-01 09:23:13',2415919104,215544,44100,320000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Got No Love.mp3',0,'audio/mpeg',6,'Got No Love',1550310333,1720982438,2060465507,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=430307071
SELECT VolumeId FROM Music WHERE UniqueId=395921263
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1906990162
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(1906990162,'(131)',1)
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(395921263,0,'2008-01-01 09:23:14',2415919104,206889,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\I want you back.mp3',0,'audio/mpeg','No comments.','2006-01-02 00:00:00',10,'I Want You Back',1550310333,1720982438,1906990162,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=395921263
SELECT VolumeId FROM Music WHERE UniqueId=498890055
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1906990162
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1906990162
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,comment,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(498890055,0,'2008-01-01 09:23:15',2415919104,121521,44100,218152,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\If only.mp3',0,'audio/mpeg','No comments.','2006-01-02 00:00:00',11,'If Only',1550310333,1720982438,1906990162,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=498890055
SELECT VolumeId FROM Music WHERE UniqueId=475572874
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1902754327
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(1902754327,'Indie',1)
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(475572874,0,'2008-01-01 09:23:15',2415919104,148853,44100,218152,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Jackie Big Tits.mp3',0,'audio/mpeg','2006-01-02 00:00:00',12,'Jackie Big Tits',1550310333,1720982438,1902754327,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=475572874
SELECT VolumeId FROM Music WHERE UniqueId=334162142
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1902754327
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1902754327
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(334162142,0,'2008-01-01 09:23:16',2415919104,129653,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Matchbox.mp3',0,'audio/mpeg',8,'Matchbox',1550310333,1720982438,1902754327,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=334162142
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=347362340
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=2102849972
INSERT INTO Genre(UniqueId,Name,SongCount) VALUES(2102849972,'Blues',1)
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(347362340,0,'2008-01-01 09:23:27',2415919104,206950,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Naive.mp3',0,'audio/mpeg','2006-01-02 00:00:00',9,'Naive',1550310333,1720982438,2102849972,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=347362340
SELECT VolumeId FROM Music WHERE UniqueId=452036423
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=2102849972
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2102849972
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(452036423,0,'2008-01-01 09:23:28',2415919104,214264,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Ooh La.mp3',0,'audio/mpeg','2006-01-02 00:00:00',5,'Ooh La',1550310333,1720982438,2102849972,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=452036423
SELECT VolumeId FROM Music WHERE UniqueId=526158435
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1902754327
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1902754327
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(526158435,0,'2008-01-01 09:23:28',2415919104,100728,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Seaside.mp3',0,'audio/mpeg','2006-01-02 00:00:00',1,'Seaside',1550310333,1720982438,1902754327,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=526158435
SELECT VolumeId FROM Music WHERE UniqueId=324659660
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=2060465507
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2060465507
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(324659660,0,'2008-01-01 09:23:29',2415919104,172154,44100,183096,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\She Moves In Her Own Way.mp3',0,'audio/mpeg','2006-01-02 00:00:00',7,'She Moves In Her Own Way',1550310333,1720982438,2060465507,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=324659660
SELECT VolumeId FROM Music WHERE UniqueId=517551818
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=1902754327
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1902754327
SELECT * FROM Composer WHERE UniqueId=2263808999
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2263808999
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,comment,albumtrack,title,artist,album,genre,composer) VALUES(517551818,0,'2008-01-01 09:23:30',2415919104,130332,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\Sofa Song.mp3',0,'audio/mpeg','2006-01-02 00:00:00',' 00001CFD 00000000 00005E91 00000000 0001D4D7 00000000 00008000 00000000 00009C40 00000000',3,'Sofa Song',1550310333,1720982438,1902754327,2263808999)
SELECT * FROM PlaylistSongInfo WHERE SongId=517551818
SELECT VolumeId FROM Music WHERE UniqueId=448022645
SELECT * FROM Artist WHERE UniqueId=1550310333
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1550310333
SELECT * FROM Album WHERE UniqueId=1720982438
SELECT * FROM Album WHERE UniqueId=172098243
SELECT * FROM Artist WHERE UniqueId=155031033
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1720982438
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(448022645,0,'2008-01-01 09:23:30',2415919104,158675,44100,128000,0,2048765375,'\\Music\\Digital\\The Kooks\\Inside in Inside Out\\You Don''t Love Me.mp3',0,'audio/mpeg',13,'You Don''t Love Me',1550310333,1720982438,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=448022645
SELECT VolumeId FROM Music WHERE UniqueId=377795069
SELECT * FROM Artist WHERE UniqueId=1358464173
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1358464173,'The Fratellis',1)
SELECT * FROM Album WHERE UniqueId=1848043093
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1848043093,'Here We Stand',1,1358464173,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(377795069,0,'2008-01-01 09:23:31',2415919104,296751,44100,124331,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\A Heady Tale.m4a',0,'audio/mp4','2008-01-02 00:00:00',2,'A Heady Tale',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=377795069
SELECT VolumeId FROM Music WHERE UniqueId=431801687
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(431801687,0,'2008-01-01 09:23:31',2415919104,265636,44100,124447,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Acid Jazz Singer.m4a',0,'audio/mp4','2008-01-02 00:00:00',10,'Acid Jazz Singer',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=431801687
SELECT VolumeId FROM Music WHERE UniqueId=447614028
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(447614028,0,'2008-01-01 09:23:32',2415919104,286255,44100,124079,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Baby Doll.m4a',0,'audio/mp4','2008-01-02 00:00:00',8,'Baby Doll',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=447614028
SELECT VolumeId FROM Music WHERE UniqueId=529847994
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(529847994,0,'2008-01-01 09:23:32',2415919104,266843,44100,123633,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\George Stole My Baby.m4a',0,'audio/mp4','2008-01-02 00:00:00',7,'George Stole My Baby',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=529847994
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=300532120
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(300532120,0,'2008-01-01 09:23:45',2415919104,237215,44100,123785,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Look Out Sunshine.m4a',0,'audio/mp4','2008-01-02 00:00:00',4,'Look Out Sunshine',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=300532120
SELECT VolumeId FROM Music WHERE UniqueId=422624944
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(422624944,0,'2008-01-01 09:23:45',2415919104,331000,44100,124405,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Lupe Brown.m4a',0,'audio/mp4','2008-01-02 00:00:00',11,'Lupe Brown',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=422624944
SELECT VolumeId FROM Music WHERE UniqueId=351534991
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(351534991,0,'2008-01-01 09:23:46',2415919104,285814,44100,125104,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Milk & Money.m4a',0,'audio/mp4','2008-01-02 00:00:00',12,'Milk & Money',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=351534991
SELECT VolumeId FROM Music WHERE UniqueId=468643445
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(468643445,0,'2008-01-01 09:23:47',2415919104,271534,44100,123067,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Mistress Mabel.m4a',0,'audio/mp4','2008-01-02 00:00:00',6,'Mistress Mabel',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=468643445
SELECT VolumeId FROM Music WHERE UniqueId=385917860
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(385917860,0,'2008-01-01 09:23:48',2415919104,186479,44100,123522,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\My Friend John.m4a',0,'audio/mp4','2008-01-02 00:00:00',1,'My Friend John',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=385917860
SELECT VolumeId FROM Music WHERE UniqueId=349257652
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(349257652,0,'2008-01-01 09:23:48',2415919104,239746,44100,125573,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Nobody''s Favourite Actor.m4a',0,'audio/mp4','2008-01-02 00:00:00',13,'Nobody''s Favourite Actor',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=349257652
SELECT VolumeId FROM Music WHERE UniqueId=504182543
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(504182543,0,'2008-01-01 09:23:48',2415919104,240907,44100,123905,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Shameless.m4a',0,'audio/mp4','2008-01-02 00:00:00',3,'Shameless',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=504182543
SELECT VolumeId FROM Music WHERE UniqueId=394704321
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(394704321,0,'2008-01-01 09:23:49',2415919104,275388,44100,123870,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Stranggler Moon.m4a',0,'audio/mp4','2008-01-02 00:00:00',5,'Stranggler Moon',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=394704321
SELECT VolumeId FROM Music WHERE UniqueId=281609211
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1848043093
SELECT * FROM Album WHERE UniqueId=184804309
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1848043093
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(281609211,0,'2008-01-01 09:23:49',2415919104,242996,44100,123700,0,2048765375,'\\Music\\Digital\\The Fratellis\\Here We Stand\\Tell Me A Lie.m4a',0,'audio/mp4','2008-01-02 00:00:00',9,'Tell Me A Lie',1358464173,1848043093,2114860941,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=281609211
SELECT VolumeId FROM Music WHERE UniqueId=400960745
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1619477033,'Costello Music',1,1358464173,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2248058378,'Barry Wallace, Gordon McRory & John Lawler',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(400960745,0,'2008-01-01 09:23:50',2415919104,214552,44100,126534,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\01 Henrietta.m4a',0,'audio/mp4','2006-01-02 00:00:00',1,'Henrietta',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=400960745
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=410014272
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(410014272,0,'2008-01-01 09:24:12',2415919104,199691,44100,126835,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\02 Flathead.m4a',0,'audio/mp4','2006-01-02 00:00:00',2,'Flathead',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=410014272
SELECT VolumeId FROM Music WHERE UniqueId=496642647
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(496642647,0,'2008-01-01 09:24:13',2415919104,213252,44100,126870,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\03 Country Boys & City Girls.m4a',0,'audio/mp4','2006-01-02 00:00:00',3,'Country Boys & City Girls',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=496642647
SELECT VolumeId FROM Music WHERE UniqueId=401108771
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(401108771,0,'2008-01-01 09:24:14',2415919104,217872,44100,126900,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\04 Whistle For The Choir.m4a',0,'audio/mp4','2006-01-02 00:00:00',4,'Whistle For The Choir',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=401108771
SELECT VolumeId FROM Music WHERE UniqueId=303566300
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(303566300,0,'2008-01-01 09:24:14',2415919104,217361,44100,126450,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\05 Chelsea Dagger.m4a',0,'audio/mp4','2006-01-02 00:00:00',5,'Chelsea Dagger',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=303566300
SELECT VolumeId FROM Music WHERE UniqueId=531040591
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(531040591,0,'2008-01-01 09:24:15',2415919104,170411,44100,125671,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\06 For The Girl.m4a',0,'audio/mp4','2006-01-02 00:00:00',6,'For The Girl',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=531040591
SELECT VolumeId FROM Music WHERE UniqueId=332901476
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(332901476,0,'2008-01-01 09:24:15',2415919104,202942,44100,126915,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\07 Doginabag.m4a',0,'audio/mp4','2006-01-02 00:00:00',7,'Doginabag',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=332901476
SELECT VolumeId FROM Music WHERE UniqueId=325975349
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(325975349,0,'2008-01-01 09:24:16',2415919104,189219,44100,125959,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\08 Creeping Up The Backstairs.m4a',0,'audio/mp4','2006-01-02 00:00:00',8,'Creeping Up The Backstairs',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=325975349
SELECT VolumeId FROM Music WHERE UniqueId=517811959
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(517811959,0,'2008-01-01 09:24:16',2415919104,196185,44100,125830,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\09 Vince The Loveable Stoner.m4a',0,'audio/mp4','2006-01-02 00:00:00',9,'Vince The Loveable Stoner',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=517811959
SELECT VolumeId FROM Music WHERE UniqueId=509957277
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2248058378
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2248058378
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(509957277,0,'2008-01-01 09:24:17',2415919104,236472,44100,127038,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\10 Everybody Knows You Cried Last Ni.m4a',0,'audio/mp4','2006-01-02 00:00:00',10,'Everybody Knows You Cried Last Night',1358464173,1619477033,1970654874,2248058378)
SELECT * FROM PlaylistSongInfo WHERE SongId=509957277
SELECT VolumeId FROM Music WHERE UniqueId=270533386
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2348877656
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2348877656,'The Fratellis/Barry Wallace, Gordon McRory & John Lawler',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(270533386,0,'2008-01-01 09:24:18',2415919104,238770,44100,126774,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\11 Baby Fratelli.m4a',0,'audio/mp4','2006-01-02 00:00:00',11,'Baby Fratelli',1358464173,1619477033,1970654874,2348877656)
SELECT * FROM PlaylistSongInfo WHERE SongId=270533386
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=397687934
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2163770541
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2163770541,'The Fratellis',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(397687934,0,'2008-01-01 09:24:28',2415919104,193306,44100,126671,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\12 Got Ma Nuts From A Hippie.m4a',0,'audio/mp4','2006-01-02 00:00:00',12,'Got Ma Nuts From A Hippie',1358464173,1619477033,1970654874,2163770541)
SELECT * FROM PlaylistSongInfo WHERE SongId=397687934
SELECT VolumeId FROM Music WHERE UniqueId=483288106
SELECT * FROM Artist WHERE UniqueId=1358464173
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1358464173
SELECT * FROM Album WHERE UniqueId=1619477033
SELECT * FROM Album WHERE UniqueId=161947703
SELECT * FROM Artist WHERE UniqueId=135846417
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1619477033
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2169564143
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2169564143,'Barry Wallace, Gordon McRory & John Lawler/The Fratellis',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(483288106,0,'2008-01-01 09:24:29',2415919104,196301,44100,126431,0,2048765375,'\\Music\\Digital\\The Fratellis\\Costello Music\\13 Ole Black ''N'' Blue Eyes.m4a',0,'audio/mp4','2006-01-02 00:00:00',13,'Ole Black ''N'' Blue Eyes',1358464173,1619477033,1970654874,2169564143)
SELECT * FROM PlaylistSongInfo WHERE SongId=483288106
SELECT VolumeId FROM Music WHERE UniqueId=426902356
SELECT * FROM Artist WHERE UniqueId=1449313355
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1449313355,'The Flaming Lips',1)
SELECT * FROM Album WHERE UniqueId=1750428269
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1750428269,'Yoshimi Battles The Pink Robots',1,1449313355,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(426902356,0,'2008-01-01 09:24:29',2415919104,253717,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\01-Fight Test.mp3',0,'audio/mpeg','2002-01-02 00:00:00',1,'Fight Test',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=426902356
SELECT VolumeId FROM Music WHERE UniqueId=497575915
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(497575915,0,'2008-01-01 09:24:30',2415919104,299102,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\02-One More Robot.mp3',0,'audio/mpeg','2002-01-02 00:00:00',2,'One More Robot',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=497575915
SELECT VolumeId FROM Music WHERE UniqueId=482369105
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(482369105,0,'2008-01-01 09:24:31',2415919104,177450,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\04-Yoshimi Battles The Pink Robots (Part Two).mp3',0,'audio/mpeg','2002-01-02 00:00:00',4,'Yoshimi Battles The Pink Robots (Part Two)',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=482369105
SELECT VolumeId FROM Music WHERE UniqueId=408532046
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(408532046,0,'2008-01-01 09:24:31',2415919104,378109,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\05-In The Morning Of The Magicians.mp3',0,'audio/mpeg','2002-01-02 00:00:00',5,'In The Morning Of The Magicians',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=408532046
SELECT VolumeId FROM Music WHERE UniqueId=347729735
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(347729735,0,'2008-01-01 09:24:31',2415919104,274089,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\06-Ego Tripping At The Gates Of Hell.mp3',0,'audio/mpeg','2002-01-02 00:00:00',6,'Ego Tripping At The Gates Of Hell',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=347729735
SELECT VolumeId FROM Music WHERE UniqueId=519637781
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(519637781,0,'2008-01-01 09:24:31',2415919104,284183,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\07-Are You A Hypnotist.mp3',0,'audio/mpeg','2002-01-02 00:00:00',7,'Are You A Hypnotist',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=519637781
SELECT VolumeId FROM Music WHERE UniqueId=397299749
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(397299749,0,'2008-01-01 09:24:32',2415919104,259925,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\08-It''s Summertime.mp3',0,'audio/mpeg','2002-01-02 00:00:00',8,'It''s Summertime',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=397299749
SELECT VolumeId FROM Music WHERE UniqueId=363466883
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(363466883,0,'2008-01-01 09:24:32',2415919104,212401,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\09-Do You Realize.mp3',0,'audio/mpeg','2002-01-02 00:00:00',9,'Do You Realize',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=363466883
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=411362560
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(411362560,0,'2008-01-01 09:24:44',2415919104,232929,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\10-All We Have Is Now.mp3',0,'audio/mpeg','2002-01-02 00:00:00',10,'All We Have Is Now',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=411362560
SELECT VolumeId FROM Music WHERE UniqueId=332313671
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1750428269
SELECT * FROM Album WHERE UniqueId=175042826
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1750428269
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(332313671,0,'2008-01-01 09:24:44',2415919104,189057,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\Yoshimi Battles The Pink Robots\\11-Approaching Pavonis Mons By Balloon.mp3',0,'audio/mpeg','2002-01-02 00:00:00',11,'Approaching Pavonis Mons By Balloon',1449313355,1750428269,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=332313671
SELECT VolumeId FROM Music WHERE UniqueId=293357682
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1861992765,'The Soft Bulletin',1,1449313355,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(293357682,0,'2008-01-01 09:24:45',2415919104,258073,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\01-Race for the Prize.mp3',0,'audio/mpeg','1999-01-02 00:00:00',1,'Race for the Prize',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=293357682
SELECT VolumeId FROM Music WHERE UniqueId=518737380
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(518737380,0,'2008-01-01 09:24:45',2415919104,211932,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\02-A Spoonful Weighs a Ton.mp3',0,'audio/mpeg','1999-01-02 00:00:00',2,'A Spoonful Weighs a Ton',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=518737380
SELECT VolumeId FROM Music WHERE UniqueId=394603174
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(394603174,0,'2008-01-01 09:24:46',2415919104,354086,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\03-The Spark That Bled.mp3',0,'audio/mpeg','1999-01-02 00:00:00',3,'The Spark That Bled',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=394603174
SELECT VolumeId FROM Music WHERE UniqueId=284989945
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(284989945,0,'2008-01-01 09:24:47',2415919104,228469,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\04-Slow Motion.mp3',0,'audio/mpeg','1999-01-02 00:00:00',4,'Slow Motion',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=284989945
SELECT VolumeId FROM Music WHERE UniqueId=391423049
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(391423049,0,'2008-01-01 09:24:47',2415919104,244536,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\05-What is the Light.mp3',0,'audio/mpeg','1999-01-02 00:00:00',5,'What is the Light?',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=391423049
SELECT VolumeId FROM Music WHERE UniqueId=448596438
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(448596438,0,'2008-01-01 09:24:47',2415919104,250953,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\06-The Observer.mp3',0,'audio/mpeg','1999-01-02 00:00:00',6,'The Observer',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=448596438
SELECT VolumeId FROM Music WHERE UniqueId=526817479
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(526817479,0,'2008-01-01 09:24:47',2415919104,257056,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\07-Waitin'' for a Superman.mp3',0,'audio/mpeg','1999-01-02 00:00:00',7,'Waitin'' for a Superman',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=526817479
SELECT VolumeId FROM Music WHERE UniqueId=481880558
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(481880558,0,'2008-01-01 09:24:48',2415919104,233920,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\08-Suddenly Everything Has Changed.mp3',0,'audio/mpeg','1999-01-02 00:00:00',8,'Suddenly Everything Has Changed',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=481880558
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=514291105
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(514291105,0,'2008-01-01 09:24:58',2415919104,242136,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\09-The Gash.mp3',0,'audio/mpeg','1999-01-02 00:00:00',9,'The Gash',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=514291105
SELECT VolumeId FROM Music WHERE UniqueId=438825707
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(438825707,0,'2008-01-01 09:24:58',2415919104,316657,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\10-Feeling Yourself Disintegrate.mp3',0,'audio/mpeg','1999-01-02 00:00:00',10,'Feeling Yourself Disintegrate',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=438825707
SELECT VolumeId FROM Music WHERE UniqueId=504963249
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(504963249,0,'2008-01-01 09:24:59',2415919104,189239,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\11-Sleeping on the Roof.mp3',0,'audio/mpeg','1999-01-02 00:00:00',11,'Sleeping on the Roof',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=504963249
SELECT VolumeId FROM Music WHERE UniqueId=517362777
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(517362777,0,'2008-01-01 09:24:59',2415919104,248266,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\12-Race for the Prize.mp3',0,'audio/mpeg','1999-01-02 00:00:00',12,'Race for the Prize',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=517362777
SELECT VolumeId FROM Music WHERE UniqueId=359079503
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(359079503,0,'2008-01-01 09:25:00',2415919104,259899,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\13-Waitin'' for a Superman.mp3',0,'audio/mpeg','1999-01-02 00:00:00',13,'Waitin'' for a Superman',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=359079503
SELECT VolumeId FROM Music WHERE UniqueId=435126970
SELECT * FROM Artist WHERE UniqueId=1449313355
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1449313355
SELECT * FROM Album WHERE UniqueId=1861992765
SELECT * FROM Album WHERE UniqueId=186199276
SELECT * FROM Artist WHERE UniqueId=144931335
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1861992765
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(435126970,0,'2008-01-01 09:25:00',2415919104,196099,44100,192000,0,2048765375,'\\Music\\Digital\\The Flaming Lips\\The Soft Bulletin\\14-Buggin''.mp3',0,'audio/mpeg','1999-01-02 00:00:00',14,'Buggin''',1449313355,1861992765,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=435126970
SELECT VolumeId FROM Music WHERE UniqueId=462765573
SELECT * FROM Artist WHERE UniqueId=1537638303
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1537638303,'The Finn Brothers',1)
SELECT * FROM Album WHERE UniqueId=1811720982
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1811720982,'Everyone Is Here',1,1537638303,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2316246368,'Neil Finn/Tim Finn',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(462765573,0,'2008-01-01 09:25:00',2415919104,256775,44100,192000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\01-Won''t Give In.mp3',0,'audio/mpeg','2004-01-02 00:00:00',1,'Won''t Give In',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=462765573
SELECT VolumeId FROM Music WHERE UniqueId=349294941
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(349294941,0,'2008-01-01 09:25:01',2415919104,250077,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\02-Nothing Wrong With You.mp3',0,'audio/mpeg','2004-01-02 00:00:00',2,'Nothing Wrong With You',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=349294941
SELECT VolumeId FROM Music WHERE UniqueId=393451556
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(393451556,0,'2008-01-01 09:25:02',2415919104,184295,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\03-Anything Can Happen.mp3',0,'audio/mpeg','2004-01-02 00:00:00',3,'Anything Can Happen',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=393451556
SELECT VolumeId FROM Music WHERE UniqueId=404501833
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(404501833,0,'2008-01-01 09:25:02',2415919104,235665,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\04-Luckiest Man Alive.mp3',0,'audio/mpeg','2004-01-02 00:00:00',4,'Luckiest Man Alive',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=404501833
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=347585362
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(347585362,0,'2008-01-01 09:25:12',2415919104,228002,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\05-Homesick.mp3',0,'audio/mpeg','2004-01-02 00:00:00',5,'Homesick',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=347585362
SELECT VolumeId FROM Music WHERE UniqueId=350994035
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(350994035,0,'2008-01-01 09:25:12',2415919104,219584,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\06-Disembodied Voices.mp3',0,'audio/mpeg','2004-01-02 00:00:00',6,'Disembodied Voices',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=350994035
SELECT VolumeId FROM Music WHERE UniqueId=488346418
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(488346418,0,'2008-01-01 09:25:13',2415919104,233189,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\07-A Life Between Us.mp3',0,'audio/mpeg','2004-01-02 00:00:00',7,'A Life Between Us',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=488346418
SELECT VolumeId FROM Music WHERE UniqueId=461329939
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(461329939,0,'2008-01-01 09:25:13',2415919104,227246,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\08-All God''s Children.mp3',0,'audio/mpeg','2004-01-02 00:00:00',8,'All God''s Children',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=461329939
SELECT VolumeId FROM Music WHERE UniqueId=398087427
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(398087427,0,'2008-01-01 09:25:14',2415919104,291074,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\09-Edible Flowers.mp3',0,'audio/mpeg','2004-01-02 00:00:00',9,'Edible Flowers',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=398087427
SELECT VolumeId FROM Music WHERE UniqueId=308366377
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(308366377,0,'2008-01-01 09:25:14',2415919104,130815,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\10-All The Colours.mp3',0,'audio/mpeg','2004-01-02 00:00:00',10,'All The Colours',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=308366377
SELECT VolumeId FROM Music WHERE UniqueId=326811896
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(326811896,0,'2008-01-01 09:25:14',2415919104,208898,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\11-Part of Me Part of You.mp3',0,'audio/mpeg','2004-01-02 00:00:00',11,'Part of Me Part of You',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=326811896
SELECT VolumeId FROM Music WHERE UniqueId=406930113
SELECT * FROM Artist WHERE UniqueId=1537638303
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1537638303
SELECT * FROM Album WHERE UniqueId=1811720982
SELECT * FROM Album WHERE UniqueId=181172098
SELECT * FROM Artist WHERE UniqueId=153763830
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811720982
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2316246368
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2316246368
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(406930113,0,'2008-01-01 09:25:15',2415919104,275983,44100,128000,0,2048765375,'\\Music\\Digital\\The Finn Brothers\\Everyone Is Here\\12-Gentle Hum.mp3',0,'audio/mpeg','2004-01-02 00:00:00',12,'Gentle Hum',1537638303,1811720982,2114860941,2316246368)
SELECT * FROM PlaylistSongInfo WHERE SongId=406930113
SELECT VolumeId FROM Music WHERE UniqueId=470086935
SELECT * FROM Artist WHERE UniqueId=1430881187
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1430881187,'Tex Perkins'' Dark Horses',1)
SELECT * FROM Album WHERE UniqueId=1825171462
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1825171462,'Sweet Nothing',1,1430881187,'')
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2334299603
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2334299603,'Dark Horses',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(470086935,0,'2008-01-01 09:25:16',2415919104,183460,44100,128087,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\01 A Hair of the God.m4a',0,'audio/mp4','2000-01-02 00:00:00',1,'A Hair of the God',1430881187,1825171462,2114860941,2334299603)
SELECT * FROM PlaylistSongInfo WHERE SongId=470086935
SELECT VolumeId FROM Music WHERE UniqueId=400837022
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2287459443
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2287459443,'Paterson/Tex Perkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(400837022,0,'2008-01-01 09:25:16',2415919104,197857,44100,128057,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\02 Midnight Sunshine.m4a',0,'audio/mp4','2000-01-02 00:00:00',2,'Midnight Sunshine',1430881187,1825171462,2114860941,2287459443)
SELECT * FROM PlaylistSongInfo WHERE SongId=400837022
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=458630088
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2287459443
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2287459443
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(458630088,0,'2008-01-01 09:25:30',2415919104,266727,44100,127456,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\03 Lucid.m4a',0,'audio/mp4','2000-01-02 00:00:00',3,'Lucid',1430881187,1825171462,2114860941,2287459443)
SELECT * FROM PlaylistSongInfo WHERE SongId=458630088
SELECT VolumeId FROM Music WHERE UniqueId=401840759
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2392642042
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2392642042,'Joel Silbersher/Owen/Tex Perkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(401840759,0,'2008-01-01 09:25:30',2415919104,229018,44100,128102,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\04 Cold Feet.m4a',0,'audio/mp4','2000-01-02 00:00:00',4,'Cold Feet',1430881187,1825171462,2114860941,2392642042)
SELECT * FROM PlaylistSongInfo WHERE SongId=401840759
SELECT VolumeId FROM Music WHERE UniqueId=304807645
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2334299603
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2334299603
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(304807645,0,'2008-01-01 09:25:30',2415919104,283863,44100,128095,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\05 Great Apes.m4a',0,'audio/mp4','2000-01-02 00:00:00',5,'Great Apes',1430881187,1825171462,2114860941,2334299603)
SELECT * FROM PlaylistSongInfo WHERE SongId=304807645
SELECT VolumeId FROM Music WHERE UniqueId=358822978
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2284576186
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2284576186,'Owen/Paterson/Tex Perkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(358822978,0,'2008-01-01 09:25:31',2415919104,270767,44100,128078,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\06 Changelings.m4a',0,'audio/mp4','2000-01-02 00:00:00',6,'Changelings',1430881187,1825171462,2114860941,2284576186)
SELECT * FROM PlaylistSongInfo WHERE SongId=358822978
SELECT VolumeId FROM Music WHERE UniqueId=366860012
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2287459443
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2287459443
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(366860012,0,'2008-01-01 09:25:31',2415919104,181231,44100,127807,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\07 Days Like These.m4a',0,'audio/mp4','2000-01-02 00:00:00',7,'Days Like These',1430881187,1825171462,2114860941,2287459443)
SELECT * FROM PlaylistSongInfo WHERE SongId=366860012
SELECT VolumeId FROM Music WHERE UniqueId=322672044
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2334299603
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2334299603
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(322672044,0,'2008-01-01 09:25:32',2415919104,270675,44100,128097,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\08 Hang on to my Love.m4a',0,'audio/mp4','2000-01-02 00:00:00',8,'Hang on to my Love',1430881187,1825171462,2114860941,2334299603)
SELECT * FROM PlaylistSongInfo WHERE SongId=322672044
SELECT VolumeId FROM Music WHERE UniqueId=383988785
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2287459443
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2287459443
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(383988785,0,'2008-01-01 09:25:33',2415919104,243321,44100,128144,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\09 This Wind.m4a',0,'audio/mp4','2000-01-02 00:00:00',9,'This Wind',1430881187,1825171462,2114860941,2287459443)
SELECT * FROM PlaylistSongInfo WHERE SongId=383988785
SELECT VolumeId FROM Music WHERE UniqueId=427803648
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2284576186
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2284576186
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(427803648,0,'2008-01-01 09:25:34',2415919104,410459,44100,128122,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\10 A Name on Every One.m4a',0,'audio/mp4','2000-01-02 00:00:00',10,'A Name on Every One',1430881187,1825171462,2114860941,2284576186)
SELECT * FROM PlaylistSongInfo WHERE SongId=427803648
SELECT VolumeId FROM Music WHERE UniqueId=503309820
SELECT * FROM Artist WHERE UniqueId=1430881187
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1430881187
SELECT * FROM Album WHERE UniqueId=1825171462
SELECT * FROM Album WHERE UniqueId=182517146
SELECT * FROM Artist WHERE UniqueId=143088118
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1825171462
SELECT * FROM Genre WHERE UniqueId=2114860941
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2114860941
SELECT * FROM Composer WHERE UniqueId=2322576873
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2322576873,'Tex Perkins',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(503309820,0,'2008-01-01 09:25:36',2415919104,279684,44100,128102,0,2048765375,'\\Music\\Digital\\Tex Perkins'' Dark Horses\\Sweet Nothing\\11 Everything or Nothing.m4a',0,'audio/mp4','2000-01-02 00:00:00',11,'Everything or Nothing',1430881187,1825171462,2114860941,2322576873)
SELECT * FROM PlaylistSongInfo WHERE SongId=503309820
SELECT VolumeId FROM Music WHERE UniqueId=335472323
SELECT * FROM Artist WHERE UniqueId=1566199477
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1566199477,'Stereophonics',1)
SELECT * FROM Album WHERE UniqueId=1840710040
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1840710040,'You gotta go there to come back',1,1566199477,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(335472323,0,'2008-01-01 09:25:37',2415919104,414245,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\01-Help me.mp3',0,'audio/mpeg',1,'Help me',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=335472323
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=500650820
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(500650820,0,'2008-01-01 09:26:12',2415919104,272595,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\02-Maybe Tomorrow.mp3',0,'audio/mpeg',2,'Maybe Tomorrow',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=500650820
SELECT VolumeId FROM Music WHERE UniqueId=447264176
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(447264176,0,'2008-01-01 09:26:13',2415919104,234805,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\03-Madame Helga.mp3',0,'audio/mpeg',3,'Madame Helga',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=447264176
SELECT VolumeId FROM Music WHERE UniqueId=367636273
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(367636273,0,'2008-01-01 09:26:15',2415919104,257479,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\04-You stole my money honey.mp3',0,'audio/mpeg',4,'You stole my money honey',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=367636273
SELECT VolumeId FROM Music WHERE UniqueId=525081929
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(525081929,0,'2008-01-01 09:26:16',2415919104,248461,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\05-Getaway.mp3',0,'audio/mpeg',5,'Getaway',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=525081929
SELECT VolumeId FROM Music WHERE UniqueId=436586046
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(436586046,0,'2008-01-01 09:26:18',2415919104,294696,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\06-Climbing the wall.mp3',0,'audio/mpeg',6,'Climbing the wall',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=436586046
SELECT VolumeId FROM Music WHERE UniqueId=458981640
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(458981640,0,'2008-01-01 09:26:20',2415919104,259538,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\09-Nothing precious at all.mp3',0,'audio/mpeg',9,'Nothing precious at all',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=458981640
SELECT VolumeId FROM Music WHERE UniqueId=311799002
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(311799002,0,'2008-01-01 09:26:22',2415919104,250572,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\10-Rainbows and pots of gold.mp3',0,'audio/mpeg',10,'Rainbows and pots of gold',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=311799002
SELECT VolumeId FROM Music WHERE UniqueId=499801188
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(499801188,0,'2008-01-01 09:26:24',2415919104,290318,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\11-I miss you now.mp3',0,'audio/mpeg',11,'I miss you now',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=499801188
SELECT VolumeId FROM Music WHERE UniqueId=436813819
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(436813819,0,'2008-01-01 09:26:26',2415919104,199255,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\12-High as the ceiling.mp3',0,'audio/mpeg',12,'High as the ceiling',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=436813819
SELECT VolumeId FROM Music WHERE UniqueId=359101865
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(359101865,0,'2008-01-01 09:26:28',2415919104,282421,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\You Gotta Go There To Come Back\\13-Since i told you it''s over.mp3',0,'audio/mpeg',13,'Since i told you it''s over',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=359101865
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=466977282
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1778992962,'Word Gets Around',1,1566199477,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(466977282,0,'2008-01-01 09:26:43',2415919104,182914,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\01-A thousand trees.mp3',0,'audio/mpeg',1,'A thousand trees',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=466977282
SELECT VolumeId FROM Music WHERE UniqueId=337108463
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(337108463,0,'2008-01-01 09:26:44',2415919104,152213,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\02-Looks like Chaplin.mp3',0,'audio/mpeg',2,'Looks like Chaplin',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=337108463
SELECT VolumeId FROM Music WHERE UniqueId=322451538
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(322451538,0,'2008-01-01 09:26:45',2415919104,139598,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\03-More life in a tramps vest.mp3',0,'audio/mpeg',3,'More life in a tramps vest',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=322451538
SELECT VolumeId FROM Music WHERE UniqueId=447649960
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(447649960,0,'2008-01-01 09:26:46',2415919104,201575,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\04-Local boy in the photograph.mp3',0,'audio/mpeg',4,'Local boy in the photograph',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=447649960
SELECT VolumeId FROM Music WHERE UniqueId=362468860
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(362468860,0,'2008-01-01 09:26:47',2415919104,293680,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\05-Traffic.mp3',0,'audio/mpeg',5,'Traffic',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=362468860
SELECT VolumeId FROM Music WHERE UniqueId=487662389
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(487662389,0,'2008-01-01 09:26:48',2415919104,275671,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\06-Not up to you.mp3',0,'audio/mpeg',6,'Not up to you',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=487662389
SELECT VolumeId FROM Music WHERE UniqueId=335105482
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(335105482,0,'2008-01-01 09:26:48',2415919104,162664,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\07-Check my eyelids for holes.mp3',0,'audio/mpeg',7,'Check my eyelids for holes',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=335105482
SELECT VolumeId FROM Music WHERE UniqueId=528595740
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(528595740,0,'2008-01-01 09:26:49',2415919104,239678,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\08-Same size feet.mp3',0,'audio/mpeg',8,'Same size feet',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=528595740
SELECT VolumeId FROM Music WHERE UniqueId=279910805
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(279910805,0,'2008-01-01 09:26:50',2415919104,165166,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\09-Last of the big time drinkers.mp3',0,'audio/mpeg',9,'Last of the big time drinkers',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=279910805
SELECT VolumeId FROM Music WHERE UniqueId=298906877
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(298906877,0,'2008-01-01 09:26:51',2415919104,182315,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\10-Goldfish bowl.mp3',0,'audio/mpeg',10,'Goldfish bowl',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=298906877
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=451734050
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(451734050,0,'2008-01-01 09:27:24',2415919104,302072,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\11-Too many sandwiches.mp3',0,'audio/mpeg',11,'Too many sandwiches',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=451734050
SELECT VolumeId FROM Music WHERE UniqueId=405224007
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1778992962
SELECT * FROM Album WHERE UniqueId=177899296
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1778992962
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(405224007,0,'2008-01-01 09:27:24',2415919104,225344,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Word Gets Around\\12-Billy Daveys daughter.mp3',0,'audio/mpeg',12,'Billy Daveys daughter',1566199477,1778992962,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=405224007
SELECT VolumeId FROM Music WHERE UniqueId=488535841
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1654540847,'Performance And Cocktails',1,1566199477,'E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\01 Roll Up And Shine.m4a')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2406083123,'Kelly Jones, Stuart Cable, Richard Jones',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(488535841,0,'2008-01-01 09:27:24',2415919104,237586,44100,128093,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\01 Roll Up And Shine.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\01 Roll Up And Shine.m4a','1999-01-02 00:00:00',1,'Roll Up And Shine',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=488535841
SELECT VolumeId FROM Music WHERE UniqueId=518143062
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(518143062,0,'2008-01-01 09:27:25',2415919104,174544,0,0,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\02 The Bartender And The Thief.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\02 The Bartender And The Thief.m4a','1999-01-02 00:00:00',2,'The Bartender And The Thief',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=518143062
SELECT VolumeId FROM Music WHERE UniqueId=374270006
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(374270006,0,'2008-01-01 09:27:26',2415919104,280264,44100,128111,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\03 Hurry Up And Wait.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\03 Hurry Up And Wait.m4a','1999-01-02 00:00:00',3,'Hurry Up And Wait',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=374270006
SELECT VolumeId FROM Music WHERE UniqueId=371364145
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(371364145,0,'2008-01-01 09:27:26',2415919104,213693,44100,128044,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\04 Pick A Part That''s New.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\04 Pick A Part That''s New.m4a','1999-01-02 00:00:00',4,'Pick A Part That''s New',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=371364145
SELECT VolumeId FROM Music WHERE UniqueId=312953870
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(312953870,0,'2008-01-01 09:27:26',2415919104,253770,44100,128106,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\05 Just Looking.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\05 Just Looking.m4a','1999-01-02 00:00:00',5,'Just Looking',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=312953870
SELECT VolumeId FROM Music WHERE UniqueId=327618662
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(327618662,0,'2008-01-01 09:27:27',2415919104,175055,44100,128138,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\06 Half the Lies You Tell Ain''t True.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\06 Half the Lies You Tell Ain''t True.m4a','1999-01-02 00:00:00',6,'Half the Lies You Tell Ain''t True',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=327618662
SELECT VolumeId FROM Music WHERE UniqueId=401258013
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(401258013,0,'2008-01-01 09:27:27',2415919104,230434,44100,128071,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\07 I Wouldn''t Believe Your Radio.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\07 I Wouldn''t Believe Your Radio.m4a','1999-01-02 00:00:00',7,'I Wouldn''t Believe Your Radio',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=401258013
SELECT VolumeId FROM Music WHERE UniqueId=359862189
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(359862189,0,'2008-01-01 09:27:27',2415919104,244970,44100,128042,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\08 T-Shirt Sun Tan.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\08 T-Shirt Sun Tan.m4a','1999-01-02 00:00:00',8,'T-Shirt Sun Tan',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=359862189
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=471192003
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(471192003,0,'2008-01-01 09:27:52',2415919104,242485,44100,128042,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\09 Is Yesterday, Tomorrow, Today_.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\09 Is Yesterday, Tomorrow, Today_.m4a','1999-01-02 00:00:00',9,'Is Yesterday, Tomorrow, Today?',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=471192003
SELECT VolumeId FROM Music WHERE UniqueId=440455803
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(440455803,0,'2008-01-01 09:27:52',2415919104,226348,44100,128027,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\10 A Minute Longer.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\10 A Minute Longer.m4a','1999-01-02 00:00:00',10,'A Minute Longer',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=440455803
SELECT VolumeId FROM Music WHERE UniqueId=486826077
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(486826077,0,'2008-01-01 09:27:53',2415919104,235937,44100,128094,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\11 She Takes Her Clothes Off.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\11 She Takes Her Clothes Off.m4a','1999-01-02 00:00:00',11,'She Takes Her Clothes Off',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=486826077
SELECT VolumeId FROM Music WHERE UniqueId=464823056
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(464823056,0,'2008-01-01 09:27:53',2415919104,270442,44100,128055,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\12 Plastic California.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\12 Plastic California.m4a','1999-01-02 00:00:00',12,'Plastic California',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=464823056
SELECT VolumeId FROM Music WHERE UniqueId=419306328
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1654540847
SELECT * FROM Album WHERE UniqueId=165454084
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1654540847
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2406083123
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2406083123
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(419306328,0,'2008-01-01 09:27:54',2415919104,271441,44100,126469,0,2048765375,'\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\13 I Stopped To Fill My Car Up.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Performance And Cocktails\\13 I Stopped To Fill My Car Up.m4a','1999-01-02 00:00:00',13,'I Stopped To Fill My Car Up',1566199477,1654540847,1970654874,2406083123)
SELECT * FROM PlaylistSongInfo WHERE SongId=419306328
SELECT VolumeId FROM Music WHERE UniqueId=461266045
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1661723519,'Language. Sex. Violence. Other?',1,1502354900,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(461266045,0,'2008-01-01 09:27:56',2415919104,307284,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\01-Superman.mp3',0,'audio/mpeg',1,'Superman',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=461266045
SELECT VolumeId FROM Music WHERE UniqueId=293401984
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(293401984,0,'2008-01-01 09:27:59',2415919104,228550,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\02-Doorman.mp3',0,'audio/mpeg',2,'Doorman',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=293401984
SELECT VolumeId FROM Music WHERE UniqueId=491775480
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(491775480,0,'2008-01-01 09:28:00',2415919104,244396,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\03-Brother.mp3',0,'audio/mpeg',3,'Brother',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=491775480
SELECT VolumeId FROM Music WHERE UniqueId=470668956
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(470668956,0,'2008-01-01 09:28:00',2415919104,280258,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\04-Devil.mp3',0,'audio/mpeg',4,'Devil',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=470668956
SELECT VolumeId FROM Music WHERE UniqueId=358503419
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(358503419,0,'2008-01-01 09:28:01',2415919104,296781,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\05-Dakota.mp3',0,'audio/mpeg',5,'Dakota',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=358503419
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=280439838
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(280439838,0,'2008-01-01 09:28:14',2415919104,285653,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\06-Rewind.mp3',0,'audio/mpeg',6,'Rewind',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=280439838
SELECT VolumeId FROM Music WHERE UniqueId=333379267
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(333379267,0,'2008-01-01 09:28:14',2415919104,265610,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\07-Jealousy.mp3',0,'audio/mpeg',7,'Jealousy',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=333379267
SELECT VolumeId FROM Music WHERE UniqueId=359609526
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(359609526,0,'2008-01-01 09:28:15',2415919104,197848,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\07-Pedalpusher.mp3',0,'audio/mpeg',7,'Pedalpusher',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=359609526
SELECT VolumeId FROM Music WHERE UniqueId=319538870
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(319538870,0,'2008-01-01 09:28:15',2415919104,119660,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\08-Girl.mp3',0,'audio/mpeg',8,'Girl',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=319538870
SELECT VolumeId FROM Music WHERE UniqueId=433532827
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1840710040
SELECT * FROM Album WHERE UniqueId=184071004
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1840710040
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(433532827,0,'2008-01-01 09:28:16',2415919104,276088,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\08-I''m alright.mp3',0,'audio/mpeg',8,'I''m alright',1566199477,1840710040,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=433532827
SELECT VolumeId FROM Music WHERE UniqueId=419628609
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(419628609,0,'2008-01-01 09:28:16',2415919104,205719,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\09-Lolita.mp3',0,'audio/mpeg',9,'Lolita',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=419628609
SELECT VolumeId FROM Music WHERE UniqueId=394893310
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(394893310,0,'2008-01-01 09:28:16',2415919104,213694,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\10-Deadhead.mp3',0,'audio/mpeg',10,'Deadhead',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=394893310
SELECT VolumeId FROM Music WHERE UniqueId=421041983
SELECT * FROM Artist WHERE UniqueId=1502354900
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1502354900
SELECT * FROM Album WHERE UniqueId=1661723519
SELECT * FROM Album WHERE UniqueId=166172351
SELECT * FROM Artist WHERE UniqueId=150235490
SELECT * FROM Artist WHERE UniqueId=150235490
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1661723519
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(421041983,0,'2008-01-01 09:28:17',2415919104,224458,44100,128000,0,2048765375,'\\Music\\Digital\\Stereophonics\\Language Sex Violence Other_\\11-Reel.mp3',0,'audio/mpeg',11,'Reel',1502354900,1661723519,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=421041983
SELECT VolumeId FROM Music WHERE UniqueId=426702300
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1686221755,'Just Enough Education To Perform',1,1566199477,'E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\01 Vegas Two Times.m4a')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2156078283,'Kelly Jones',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(426702300,0,'2008-01-01 09:28:17',2415919104,268678,44100,128111,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\01 Vegas Two Times.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\01 Vegas Two Times.m4a','2001-01-02 00:00:00',1,'Vegas Two Times',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=426702300
SELECT VolumeId FROM Music WHERE UniqueId=475417165
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(475417165,0,'2008-01-01 09:28:17',2415919104,271348,44100,128102,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\02 Lying In The Sun.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\02 Lying In The Sun.m4a','2001-01-02 00:00:00',2,'Lying In The Sun',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=475417165
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=448979404
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2366573029
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2366573029,'Marshall Bird',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(448979404,0,'2008-01-01 09:28:44',2415919104,319367,44100,127556,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\03 Mr. Writer.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\03 Mr. Writer.m4a','2001-01-02 00:00:00',3,'Mr. Writer',1566199477,1686221755,1970654874,2366573029)
SELECT * FROM PlaylistSongInfo WHERE SongId=448979404
SELECT VolumeId FROM Music WHERE UniqueId=528226817
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(528226817,0,'2008-01-01 09:28:44',2415919104,240907,44100,127854,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\04 Step On My Old Size Nines.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\04 Step On My Old Size Nines.m4a','2001-01-02 00:00:00',4,'Step On My Old Size Nines',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=528226817
SELECT VolumeId FROM Music WHERE UniqueId=386649978
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(386649978,0,'2008-01-01 09:28:45',2415919104,205125,44100,128093,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\05 Have A Nice Day.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\05 Have A Nice Day.m4a','2001-01-02 00:00:00',5,'Have A Nice Day',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=386649978
SELECT VolumeId FROM Music WHERE UniqueId=278681125
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(278681125,0,'2008-01-01 09:28:45',2415919104,188963,0,0,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\06 Nice To Be Out.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\06 Nice To Be Out.m4a','2001-01-02 00:00:00',6,'Nice To Be Out',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=278681125
SELECT VolumeId FROM Music WHERE UniqueId=348791128
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(348791128,0,'2008-01-01 09:28:46',2415919104,209722,44100,128102,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\07 Watch Them Fly Sundays.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\07 Watch Them Fly Sundays.m4a','2001-01-02 00:00:00',7,'Watch Them Fly Sundays',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=348791128
SELECT VolumeId FROM Music WHERE UniqueId=371378724
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(371378724,0,'2008-01-01 09:28:47',2415919104,204637,44100,127461,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\08 Everyday I Think Of Money.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\08 Everyday I Think Of Money.m4a','2001-01-02 00:00:00',8,'Everyday I Think Of Money',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=371378724
SELECT VolumeId FROM Music WHERE UniqueId=317939077
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(317939077,0,'2008-01-01 09:28:47',2415919104,274622,44100,128097,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\09 Maybe.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\09 Maybe.m4a','2001-01-02 00:00:00',9,'Maybe',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=317939077
SELECT VolumeId FROM Music WHERE UniqueId=467475363
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(467475363,0,'2008-01-01 09:28:47',2415919104,192772,44100,127766,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\10 Caravan Holiday.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\10 Caravan Holiday.m4a','2001-01-02 00:00:00',10,'Caravan Holiday',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=467475363
SELECT VolumeId FROM Music WHERE UniqueId=506696785
SELECT * FROM Artist WHERE UniqueId=1566199477
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1566199477
SELECT * FROM Album WHERE UniqueId=1686221755
SELECT * FROM Album WHERE UniqueId=168622175
SELECT * FROM Artist WHERE UniqueId=156619947
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1686221755
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2156078283
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2156078283
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,Art,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(506696785,0,'2008-01-01 09:28:48',2415919104,375350,44100,128095,0,2048765375,'\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\11 Rooftop.m4a',0,'audio/mp4','E:\\Music\\Digital\\Stereophonics\\Just Enough Education To Perform\\11 Rooftop.m4a','2001-01-02 00:00:00',11,'Rooftop',1566199477,1686221755,1970654874,2156078283)
SELECT * FROM PlaylistSongInfo WHERE SongId=506696785
SELECT VolumeId FROM Music WHERE UniqueId=397348542
SELECT * FROM Artist WHERE UniqueId=1568132145
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1568132145,'Stereogram',1)
SELECT * FROM Album WHERE UniqueId=1756232144
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1756232144,'Schmack!',1,1568132145,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(397348542,0,'2008-01-01 09:28:50',2415919104,178874,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\01-Roadtrip.mp3',0,'audio/mpeg',1,'Roadtrip',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=397348542
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=532644136
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(532644136,0,'2008-01-01 09:29:07',2415919104,133135,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\02-Walkie talkie man.mp3',0,'audio/mpeg',2,'Walkie talkie man',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=532644136
SELECT VolumeId FROM Music WHERE UniqueId=517968676
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(517968676,0,'2008-01-01 09:29:14',2415919104,145228,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\03-Schmack!.mp3',0,'audio/mpeg',3,'Schmack!',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=517968676
SELECT VolumeId FROM Music WHERE UniqueId=383597612
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(383597612,0,'2008-01-01 09:29:15',2415919104,133604,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\04-Was the day.mp3',0,'audio/mpeg',4,'Was the day',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=383597612
SELECT VolumeId FROM Music WHERE UniqueId=469767822
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(469767822,0,'2008-01-01 09:29:17',2415919104,223989,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\05-White trash.mp3',0,'audio/mpeg',5,'White trash',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=469767822
SELECT VolumeId FROM Music WHERE UniqueId=426534169
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(426534169,0,'2008-01-01 09:29:18',2415919104,161569,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\06-In the city.mp3',0,'audio/mpeg',6,'In the city',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=426534169
SELECT VolumeId FROM Music WHERE UniqueId=352563566
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(352563566,0,'2008-01-01 09:29:20',2415919104,175721,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\07-Go.mp3',0,'audio/mpeg',7,'Go',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=352563566
SELECT VolumeId FROM Music WHERE UniqueId=398392582
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(398392582,0,'2008-01-01 09:29:23',2415919104,206918,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\08-Fat and proud.mp3',0,'audio/mpeg',8,'Fat and proud',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=398392582
SELECT VolumeId FROM Music WHERE UniqueId=530479354
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(530479354,0,'2008-01-01 09:29:25',2415919104,181429,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\09-Tsunami.mp3',0,'audio/mpeg',9,'Tsunami',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=530479354
SELECT VolumeId FROM Music WHERE UniqueId=280846910
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(280846910,0,'2008-01-01 09:29:28',2415919104,193965,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\10-Wind it up.mp3',0,'audio/mpeg',10,'Wind it up',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=280846910
SELECT VolumeId FROM Music WHERE UniqueId=407751195
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(407751195,0,'2008-01-01 09:29:35',2415919104,179083,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\11-Be good to me.mp3',0,'audio/mpeg',11,'Be good to me',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=407751195
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=532588183
SELECT * FROM Artist WHERE UniqueId=1568132145
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1568132145
SELECT * FROM Album WHERE UniqueId=1756232144
SELECT * FROM Album WHERE UniqueId=175623214
SELECT * FROM Artist WHERE UniqueId=156813214
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1756232144
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(532588183,0,'2008-01-01 09:30:14',2415919104,238714,44100,128000,0,2048765375,'\\Music\\Digital\\Stereogram\\Schmack!\\12-On and on.mp3',0,'audio/mpeg',12,'On and on',1568132145,1756232144,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=532588183
SELECT VolumeId FROM Music WHERE UniqueId=310083044
SELECT * FROM Artist WHERE UniqueId=1590177974
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1590177974,'Snow Patrol',1)
SELECT * FROM Album WHERE UniqueId=1811953358
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1811953358,'When It''s All Over We Still Have To Clear Up',1,1590177974,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
INSERT INTO Composer(UniqueId,Name,SongCount) VALUES(2395484342,'Snow Patrol',1)
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(310083044,0,'2008-01-01 09:30:17',2415919104,320365,44100,125417,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\An Olive Grove Facing The Sea.m4a',0,'audio/mp4','2001-01-02 00:00:00',11,'An Olive Grove Facing The Sea',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=310083044
SELECT VolumeId FROM Music WHERE UniqueId=344961109
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(344961109,0,'2008-01-01 09:30:28',2415919104,157013,44100,125980,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Ask Me How I Am.m4a',0,'audio/mp4','2001-01-02 00:00:00',2,'Ask Me How I Am',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=344961109
SELECT VolumeId FROM Music WHERE UniqueId=364594898
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(364594898,0,'2008-01-01 09:30:31',2415919104,212021,44100,125597,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Batten Down The Hatch.m4a',0,'audio/mp4','2001-01-02 00:00:00',7,'Batten Down The Hatch',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=364594898
SELECT VolumeId FROM Music WHERE UniqueId=437423036
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(437423036,0,'2008-01-01 09:30:37',2415919104,221982,44100,126823,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Black And Blue.m4a',0,'audio/mp4','2001-01-02 00:00:00',4,'Black And Blue',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=437423036
SELECT VolumeId FROM Music WHERE UniqueId=344070473
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(344070473,0,'2008-01-01 09:30:41',2415919104,162841,44100,126508,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Chased By... I Don''t Know What.m4a',0,'audio/mp4','2001-01-02 00:00:00',9,'Chased By... I Don''t Know What',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=344070473
SELECT VolumeId FROM Music WHERE UniqueId=417342557
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(417342557,0,'2008-01-01 09:30:43',2415919104,223375,44100,127696,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Firelight.m4a',0,'audio/mp4','2001-01-02 00:00:00',14,'Firelight',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=417342557
SELECT VolumeId FROM Music WHERE UniqueId=416587163
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(416587163,0,'2008-01-01 09:30:45',2415919104,289599,44100,127093,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\If I''d Found The Right Words To Say.m4a',0,'audio/mp4','2001-01-02 00:00:00',6,'If I''d Found The Right Words To Say',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=416587163
SELECT VolumeId FROM Music WHERE UniqueId=481946924
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(481946924,0,'2008-01-01 09:30:46',2415919104,165117,44100,124832,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Last Ever Lone Gunman.m4a',0,'audio/mp4','2001-01-02 00:00:00',5,'Last Ever Lone Gunman',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=481946924
SELECT VolumeId FROM Music WHERE UniqueId=393507624
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(393507624,0,'2008-01-01 09:30:49',2415919104,177446,44100,126637,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Make Love To Me Forever.m4a',0,'audio/mp4','2001-01-02 00:00:00',13,'Make Love To Me Forever',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=393507624
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=314653985
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(314653985,0,'2008-01-01 09:32:48',2415919104,261270,44100,126576,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Making Enemies.m4a',0,'audio/mp4','2001-01-02 00:00:00',3,'Making Enemies',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=314653985
SELECT VolumeId FROM Music WHERE UniqueId=411979380
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(411979380,0,'2008-01-01 09:32:50',2415919104,132841,44100,126116,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\Never Gonna Fall In Love Again.m4a',0,'audio/mp4','2001-01-02 00:00:00',1,'Never Gonna Fall In Love Again',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=411979380
SELECT VolumeId FROM Music WHERE UniqueId=468560478
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(468560478,0,'2008-01-01 09:32:53',2415919104,204962,44100,126837,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\One Night Is Not Enough.m4a',0,'audio/mp4','2001-01-02 00:00:00',8,'One Night Is Not Enough',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=468560478
SELECT VolumeId FROM Music WHERE UniqueId=529647186
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(529647186,0,'2008-01-01 09:32:54',2415919104,162144,44100,126470,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\On_Off.m4a',0,'audio/mp4','2001-01-02 00:00:00',10,'On/Off',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=529647186
SELECT VolumeId FROM Music WHERE UniqueId=308219337
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1811953358
SELECT * FROM Album WHERE UniqueId=181195335
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1811953358
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2395484342
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2395484342
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(308219337,0,'2008-01-01 09:32:57',2415919104,199877,44100,126786,0,2048765375,'\\Music\\Digital\\Snow Patrol\\When It''s All Over We Still Have To Clea\\When It''s All Over We Still Have To.m4a',0,'audio/mp4','2001-01-02 00:00:00',12,'When It''s All Over We Still Have To Clear Up',1590177974,1811953358,1970654874,2395484342)
SELECT * FROM PlaylistSongInfo WHERE SongId=308219337
SELECT VolumeId FROM Music WHERE UniqueId=307621787
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1712121801,'Final Straw',1,1590177974,'')
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(307621787,0,'2008-01-01 09:32:58',2415919104,204056,44100,126656,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\01 How To Be Dead.m4a',0,'audio/mp4','2004-01-02 00:00:00',1,'How To Be Dead',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=307621787
SELECT VolumeId FROM Music WHERE UniqueId=335650853
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(335650853,0,'2008-01-01 09:32:59',2415919104,244134,44100,127045,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\02 Wow.m4a',0,'audio/mp4','2004-01-02 00:00:00',2,'Wow',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=335650853
SELECT VolumeId FROM Music WHERE UniqueId=472546070
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(472546070,0,'2008-01-01 09:33:02',2415919104,126130,44100,126045,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\03 Gleaming Auction.m4a',0,'audio/mp4','2004-01-02 00:00:00',3,'Gleaming Auction',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=472546070
SELECT VolumeId FROM Music WHERE UniqueId=369822841
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(369822841,0,'2008-01-01 09:33:04',2415919104,161935,44100,126485,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\04 Whatever''s Left.m4a',0,'audio/mp4','2004-01-02 00:00:00',4,'Whatever''s Left',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=369822841
SELECT VolumeId FROM Music WHERE UniqueId=351113149
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(351113149,0,'2008-01-01 09:33:06',2415919104,228112,44100,126991,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\05 Spitting Games.m4a',0,'audio/mp4','2004-01-02 00:00:00',5,'Spitting Games',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=351113149
COMMIT TRANSACTION
BEGIN TRANSACTION
SELECT VolumeId FROM Music WHERE UniqueId=473243593
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(473243593,0,'2008-01-01 09:34:18',2415919104,184737,44100,125944,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\06 Chocolate.m4a',0,'audio/mp4','2004-01-02 00:00:00',6,'Chocolate',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=473243593
SELECT VolumeId FROM Music WHERE UniqueId=329691125
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(329691125,0,'2008-01-01 09:34:22',2415919104,177354,44100,126609,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Grazed Knees.m4a',0,'audio/mp4','2004-01-02 00:00:00',8,'Grazed Knees',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=329691125
SELECT VolumeId FROM Music WHERE UniqueId=519107291
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(519107291,0,'2008-01-01 09:34:25',2415919104,356704,44100,127398,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Run.m4a',0,'audio/mp4','2004-01-02 00:00:00',7,'Run',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=519107291
SELECT VolumeId FROM Music WHERE UniqueId=395847961
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(395847961,0,'2008-01-01 09:34:28',2415919104,416473,44100,73241,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Same.m4a',0,'audio/mp4','2004-01-02 00:00:00',12,'Same',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=395847961
SELECT VolumeId FROM Music WHERE UniqueId=526324343
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(526324343,0,'2008-01-01 09:34:31',2415919104,274692,44100,127171,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Somewhere A Clock Is Ticking.m4a',0,'audio/mp4','2004-01-02 00:00:00',11,'Somewhere A Clock Is Ticking',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=526324343
SELECT VolumeId FROM Music WHERE UniqueId=469759732
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(469759732,0,'2008-01-01 09:34:33',2415919104,150604,44100,125482,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Tiny Little Fractures.m4a',0,'audio/mp4','2004-01-02 00:00:00',10,'Tiny Little Fractures',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=469759732
SELECT VolumeId FROM Music WHERE UniqueId=400878295
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(400878295,0,'2008-01-01 09:34:37',2415919104,290063,44100,127262,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\Ways & Means.m4a',0,'audio/mp4','2004-01-02 00:00:00',9,'Ways & Means',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=400878295
SELECT VolumeId FROM Music WHERE UniqueId=399425739
SELECT * FROM Artist WHERE UniqueId=1590177974
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1590177974
SELECT * FROM Album WHERE UniqueId=1712121801
SELECT * FROM Album WHERE UniqueId=171212180
SELECT * FROM Artist WHERE UniqueId=159017797
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1712121801
SELECT * FROM Genre WHERE UniqueId=1970654874
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=1970654874
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,ReleaseDate,albumtrack,title,artist,album,genre,composer) VALUES(399425739,0,'2008-01-01 09:34:44',2415919104,195837,44100,127505,0,2048765375,'\\Music\\Digital\\Snow Patrol\\Final Straw\\We Can Run Away Now They''re All Dead.m4a',0,'audio/mp4','2004-01-02 00:00:00',13,'We Can Run Away Now They''re All Dead And Gone',1590177974,1712121801,1970654874,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=399425739
SELECT VolumeId FROM Music WHERE UniqueId=466753133
SELECT * FROM Artist WHERE UniqueId=1361845586
INSERT INTO Artist(UniqueId,Name,SongCount) VALUES(1361845586,'Shihad',1)
SELECT * FROM Album WHERE UniqueId=1630281042
INSERT INTO Album(UniqueId,Name,SongCount,Artist,Art) VALUES(1630281042,'Shihad',1,1361845586,'')
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(466753133,0,'2008-01-01 09:34:50',2415919104,215336,44100,128000,0,2048765375,'\\Music\\Digital\\Shihad\\Shihad\\01-Home Again.mp3',0,'audio/mpeg',1,'Home Again',1361845586,1630281042,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=466753133
SELECT VolumeId FROM Music WHERE UniqueId=355259680
SELECT * FROM Artist WHERE UniqueId=1361845586
UPDATE Artist SET SongCount=SongCount+1 WHERE UniqueId=1361845586
SELECT * FROM Album WHERE UniqueId=1630281042
SELECT * FROM Album WHERE UniqueId=163028104
SELECT * FROM Artist WHERE UniqueId=136184558
UPDATE Album SET SongCount=SongCount+1 WHERE UniqueId=1630281042
SELECT * FROM Genre WHERE UniqueId=2039225812
UPDATE Genre SET SongCount=SongCount+1 WHERE UniqueId=2039225812
SELECT * FROM Composer WHERE UniqueId=2307661268
UPDATE Composer SET SongCount=SongCount+1 WHERE UniqueId=2307661268
INSERT INTO Music(UniqueId,Deleted,TimeAdded,DbFlag,Duration,SampleRate,BitRate,MTPDrmStatus,VolumeId,Location,DRM,MimeType,albumtrack,title,artist,album,genre,composer) VALUES(355259680,0,'2008-01-01 09:34:52',2415919104,178353,44100,128000,0,2048765375,'\\Music\\Digital\\Shihad\\Shihad\\02-Ghost From The Past.mp3',0,'audio/mpeg',2,'Ghost From The Past',1361845586,1630281042,2039225812,2307661268)
SELECT * FROM PlaylistSongInfo WHERE SongId=355259680
COMMIT TRANSACTION
