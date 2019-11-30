struct AUser {
 char   Name[31],
	Pass[9],
	Location[30],
	PhoneNumber[13];
 USHORT	Slot_Number;
 USHORT Sec_Status,
	Sec_Board,                   /* File or Byte Ratio */
    	Sec_Library,                 /* Ratio              */
    	Sec_Bulletin,                /* Computer Type      */
    	Messages_Posted;
 ULONG 	NewSinceDate,
	CRCPassword,
	ConfRead2,
	ConfRead3;
 UWORD 	ZoomAscii;
 UWORD  xx;
 UWORD 	xxx;
 UWORD 	Area;
 UWORD 	XferProtocol,
	Filler2;
 UWORD 	Lcfiles,
	BadFiles; 
 ULONG 	AccountDate;
 UWORD 	ScreenType,
	EditorType;
 char   Conference_Access[10];
 USHORT Uploads,
	Downloads,
	ConfRJoin,
	Times_Called;
 long   Time_Last_On,
	Time_Used,
	Time_Limit,
	Time_Total;
 ULONG  Bytes_Download,
	Bytes_Upload,
	Daily_Bytes_Limit,
	Daily_Bytes_Dld;
 char   Expert;
 ULONG  ChatRemaining,
	ChatLimit,
	CreditDays,
	CreditAmount,
	CreditStartDate,
	CreditTotal,
	CreditTotalDate,
        CreditBits,
	ConfYM9;
 long   BeginLogCall;
 UBYTE  Protocol,
	UUCPA,
	LineLength,
	New_User;
 	}Auser;

struct AUserKeys 
	{                 /* changes to UserKeys (RTS) */
 	char    UserName[31];
 	long    Number;
 	UBYTE   New_User;
 	UWORD   UPcps;              /* highest upload cps rate */
 	UWORD   DNcps;              /* highest dnload cps rate */
 	UWORD   Userflags;          /*                         */
 	UWORD   baud_rate;          /* last online baud rate   */
 	char    Pad[9];             /* ?? should be 15         */
	}auserkeys;

struct AMiscData
	{
  	char 	username[10];
  	char 	realname[30];
  	ULONG 	Flags[10];
  	ULONG 	ConfAccess[32];
  	ULONG 	SecurityAccess[10];
	}amiscdata;
