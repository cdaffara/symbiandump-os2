/**
 * Contacts model database schema (file version v8).
 *
 *
 */


-- CREATE SEQUENCE contacts_id_sequence INITIAL_VALUE=0 INCREMENT=1;

CREATE table contacts
	(
	cm_id					INTEGER NOT NULL, -- see (1): DEFAULT NEXT VALUE OF contacts_id_sequence
	cm_type					INTEGER NOT NULL,
	cm_preftemplaterefid	INTEGER NULL,
	cm_uidstring			VARCHAR(244) NULL,
	cm_last_modified		TIMESTAMP NULL,
	cm_contactcreationdate	TIMESTAMP NULL,
	cm_attributes			INTEGER NOT NULL, -- should be UNSIGNED
	cm_replicationcount		INTEGER NOT NULL, -- should be UNSIGNED
	cm_header				LONGBLOB NULL, -- BLOB
	cm_textblob				LONGBLOB NULL, -- BLOB
	cm_searchabletext		LONGTEXT NULL, -- CLOB

	PRIMARY KEY (cm_id), 
 
	FOREIGN KEY (cm_preftemplaterefid) REFERENCES contacts(cm_id) 
		ON DELETE SET NULL 
		ON UPDATE CASCADE
	);

CREATE table identitytable
	(
	parent_cmid			INTEGER NOT NULL,
	cm_firstname		VARCHAR(255) NULL,
	cm_lastname			VARCHAR(255) NULL,
	cm_companyname		VARCHAR(255) NULL,
	cm_type				INTEGER NULL,
	cm_attributes		INTEGER NULL, -- should be UNSIGNED
	cm_hintfield		BIT(8) NULL, -- binary(1)
	cm_exthintfield		BIT(16) NULL,-- binary(2)
	cm_firstnmprn		VARCHAR(255) NULL,
	cm_lastnmprn		VARCHAR(255) NULL,
	cm_companynmprn		VARCHAR(255) NULL,

	PRIMARY KEY (parent_cmid),

	FOREIGN KEY (parent_cmid) REFERENCES contacts(cm_id)
	);

-- CREATE SEQUENCE emailtable_fieldid_sequence INITIAL_VALUE=0 INCREMENT=1;

CREATE table emailtable
	(
	emailparent_cmid	INTEGER NOT NULL,
	email_fieldid		INTEGER NOT NULL, -- see (2) DEFAULT NEXT VALUE OF emailtable_fieldid_sequence
	emailaddress		VARCHAR(255) NULL,
	
	PRIMARY KEY (email_fieldid, emailparent_cmid),

	FOREIGN KEY (emailparent_cmid) REFERENCES contacts(cm_id)
	);

CREATE table phone
	(
	cm_id						INTEGER NOT NULL,
	cm_phonematching			INTEGER NOT NULL,
	cm_extendedphonematching	INTEGER NOT NULL,

	PRIMARY KEY (cm_phonematching, cm_id),

	FOREIGN KEY (cm_id) REFERENCES contacts(cm_id)
	);

CREATE table groups
	(
	cm_id		INTEGER NOT NULL,
	cm_members	INTEGER NOT NULL, -- this should really be cm_member, but it is called cm_members in the current database

	PRIMARY KEY (cm_members, cm_id),

	FOREIGN KEY (cm_id) REFERENCES contacts(cm_id),

	FOREIGN KEY (cm_members) REFERENCES contacts(cm_id)
	);

CREATE table groups2
	(
	cm_id			INTEGER NOT NULL,
	cm_groupmembers	LONGBLOB NULL, -- BLOB
	
	PRIMARY KEY (cm_id),

	FOREIGN KEY (cm_id) REFERENCES contacts(cm_id)
	);

CREATE table sync
	(
	cm_id			INTEGER NOT NULL,	-- why is this UNSIGNED in ?
	cm_lastsyncdate	TIMESTAMP NULL,

	PRIMARY KEY (cm_id),

	FOREIGN KEY (cm_id) REFERENCES contacts(cm_id) -- does this reference contacts?
	);

CREATE table preferences
	(
	cm_preffileid				SMALLINT NULL,
	cm_preftemplateid			INTEGER NOT NULL,
	cm_prefowncardid			INTEGER NULL,
	cm_prefcardtemplateprefid	INTEGER NULL,
	cm_preffilever				INTEGER NOT NULL,
	cm_prefcardtemplateid		LONGBLOB NULL, -- BLOB
	cm_prefgroupidlist			LONGBLOB NULL, -- BLOB
	cm_creationdate				TIMESTAMP NULL,
	cm_machineuid				BIGINT NULL,
	cm_prefsortorder			LONGBLOB NULL, -- BLOB

	FOREIGN KEY (cm_preftemplateid) REFERENCES contacts(cm_id),

	FOREIGN KEY (cm_prefowncardid) REFERENCES contacts(cm_id),
	
	FOREIGN KEY (cm_prefcardtemplateprefid) REFERENCES contacts(cm_id)
	);
