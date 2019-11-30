/****************************************************************************
 * Amiex 2 FAME Converter written by SieGeL/tRSi (/X & F.A.M.E. iNNOVATiON) *
 *                                                                          *
 *                 USAGE: UCONVERT <AMIEX-DIR> <FAMEDIR>                    *
 ****************************************************************************/
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <strings.h>
#include <proto/utility.h>
#include "cwork:uconvert/hauptstructs.h"	/* Amiex-User-Structs */
#include <fame/famedefine.h>
#include <fame/FAMEuserstructs.h>					/* FAME -Structs */

#define	VER	"0.5"
#define Print(s) FPuts(Output(),s)

char	amiexdir[256];
char	famedir[256];

extern struct ExecBase *SysBase;
extern char *_ProgramName;

struct AUser 	 		*au;						/* Amiex user.data */
struct AUserKeys 	*ak;						/*  -"-  user.keys */
struct AMiscData 	*am;						/*  -"-  user.misc */
struct FAMEUser	 			*fu;						/* FAME  user.data */
struct FAMEUserKeys   *fk;   /*   FAME   user.keys   */
struct  Library		*UtilityBase=NULL;
BOOL get_all(void);
void free_all(void);
BOOL convert_them(void);										/* Konvertierung ! */
void copy_udata(void);											/* User-Datas kopieren */
void SPrintf(char *Buffer,char *ctl, ...);
void CheckPW(void);													/* AutoPW Database verwenden */

static const STRPTR Ver="$VER: UConvert "VER" "__AMIGADATE__"\0";

char 	*VerStr="FAMEUSDA$VER:FAME v0.75 UConvert V0.5 (16.09.96)            ";
char  *VersUk="FAMEUSKE$VER:FAME v0.75 UConvert V0.5 (16.09.96)            ";

int main(void)
	{
	int			retcode=RETURN_OK;
	struct	RDArgs *rda = NULL;
	long		ArgArray[2]={0,0};			/* Array fuer die Parameter */

	au=NULL;ak=NULL;am=NULL;fu=NULL;fk=NULL;
	if(SysBase->LibNode.lib_Version<37) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);return 20;}
	if(!(UtilityBase=OpenLibrary("utility.library",37))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY); return 20;}
	if(rda=ReadArgs("AMIEXDIR/A,FAMEDIR/A",ArgArray,rda))
		{
		SPrintf(amiexdir,"%s",ArgArray[0]);
		SPrintf(famedir,"%s",ArgArray[1]);
		FreeArgs(rda);rda=NULL;
		Printf("\f\n[33m[12CUConvert [32mv%s [31mwritten by SieGeL (tRSi/X-iNNOVATiON)\n\n",VER);
		Print("This Tool will convert the Amiex files 'USER.DATA, USER.KEYS & USER.MISC'\n");
		Print("\ninto the FAME-Format, which needs only the 'USER.DATA & USER.KEYS' Files !\n\n");
		Printf("   [1mAmiex-Directory:[m %s\n    [1mFAME-Directory:[m %s\n\n",amiexdir,famedir);
		Print("If you want to start now press <ENTER> to start or <Q> to abort: ");
		Flush(Output());
		if(FGetC(Input())!=10) { Print("\nABORT...\n"); CloseLibrary(UtilityBase); exit(0);}
		if(convert_them()==FALSE)
			{
			Print("[33m                Something went wrong during the converting...\n\n");
			PrintFault(IoErr(),_ProgramName);
			retcode=RETURN_FAIL;
			}
		else
			{
			Print(" [m- [33mConverting successful !\n\n[mDo you have installed AutoPW from ByteAndi/tRSi (y/N) ? ");
    	Flush(Output());
			if(ToUpper((char)FGetC(Input()))=='Y') CheckPW();
			Print("\n\n[31m               All done, have fun with the great FAME BBS System :)\n");
			retcode=RETURN_OK;
			}
		Print("\n");
		}
	else { PrintFault(IoErr(),_ProgramName); retcode = RETURN_FAIL; }
  CloseLibrary(UtilityBase);
	exit(retcode);
	}

BOOL convert_them(void)
	{
	long	usize,count,err=0;
	BPTR	up=NULL,ukp=NULL,ump=NULL,fup=NULL,fkp=NULL;
	char	str[256];
	struct	FileInfoBlock __aligned myfib;

	*str='\0';
	usize=count=0L;
	Printf("\nCounting Number of Users from Amiex USER.DATA...[33m",amiexdir);
	strcpy(str,amiexdir);
	AddPart(str,"USER.DATA",255);
	if(up=Open(str,MODE_OLDFILE))
		{
    ExamineFH(up,&myfib);
		usize=myfib.fib_Size/sizeof(struct AUser);
		strcpy(str,amiexdir);
		AddPart(str,"USER.KEYS",255);
		if(ukp=Open(str,MODE_OLDFILE))
			{
			strcpy(str,amiexdir);
			AddPart(str,"USER.MISC",255);
			if(ump=Open(str,MODE_OLDFILE))
				{
				strcpy(str,famedir);
				AddPart(str,"USER.DATA",255);
				if(fup=Open(str,MODE_NEWFILE))
					{
					Seek(fup,0L,OFFSET_BEGINNING);
  				Write(fup,VerStr,61);
          strcpy(str,famedir);
					AddPart(str,"USER.KEYS",255);
					if(fkp=Open(str,MODE_NEWFILE))
						{
						Seek(fkp,0L,OFFSET_BEGINNING);
						Write(fkp,VersUk,61);
						Printf("done !\n\n[31mFound %ld User(s) in the user.datas ![33m\n\n",usize);
						if(get_all()==TRUE)
							{
							while(count<usize)
								{
								Seek(up,(count*(sizeof(struct AUser))),OFFSET_BEGINNING);
								Read(up,au,sizeof(struct AUser));
								Seek(ukp,(count*(sizeof(struct AUserKeys))),OFFSET_BEGINNING);
								Read(ukp,ak,sizeof(struct AUserKeys));
								Seek(ump,(count*(sizeof(struct AMiscData))),OFFSET_BEGINNING);
								Read(ump,am,sizeof(struct AMiscData));
								copy_udata();
								Seek(fup,61+(count*(sizeof(struct FAMEUser))),OFFSET_BEGINNING);
								Write(fup,fu,sizeof(struct FAMEUser));
								Seek(fkp,61+(count*(sizeof(struct FAMEUserKeys))),OFFSET_BEGINNING);
								Write(fkp,fk,sizeof(struct FAMEUserKeys));
								count++;
								Printf("[K\r [31mNow converting User : [32m%ld",count);
								}
							free_all();
							}
						else err=6;
						Close(fkp);fkp=NULL;
						}
					else err=5;
					Close(fup);fup=NULL;
					}
				else err=4;
				Close(ump);ump=NULL;
				}
			else err=3;
			Close(ukp);ukp=NULL;
			}
		else err=2;
		Close(up);up=NULL;
		}
	else err=1;
	free_all();
	if(up) 	Close(up);
	if(ukp)	Close(ukp);
	if(ump)	Close(ump);
	if(fup) Close(fup);
	if(fkp) Close(fkp);
  switch(err)
		{
  	case	0:	return(TRUE);
		case	1:	Printf("ERROR![m\n\nUnable to open Amiex user.datas for reading !\n\n");
							break;
		case	2:	Printf("ERROR![m\n\nUnable to open Amiex user.keys for reading !\n\n");
							break;
		case	3:	Printf("ERROR![m\n\nUnable to open Amiex user.misc for reading !\n\n");
							break;
		case	4:	Printf("ERROR![m\n\nUnable to open FAME user.datas for writing !\n\n");
							break;
		case	5:	Printf("ERROR![m\n\nUnable to open FAME user.keys for writing !\n\n");
							break;
		case	6:	Printf("ERROR![m\n\nUnable to allocate memory for converting !\n\n");
							break;
		}
	return(FALSE);
	}

void copy_udata(void)
	{
	char	str[6];

	memset(fu,0,sizeof(struct FAMEUser));
	memset(fk,0,sizeof(struct FAMEUserKeys));

	fu->UserFlags1=NULL;
	fu->UserFlags2=NULL;
	fu->NToNComFlag1=NULL;
	fu->NToNComFlag1 |= NTN_DEFAULT;				// Defaults to NToN Comm set to ON!
	fu->NToNComFlag2=NULL;
	fu->UserFlags1 |= UD_MSGROOM;
	fu->UserFlags1 |= UD_STRED;
	fu->UserFlags1 |= UD_SCRPAUSE;
	fu->UserFlags1 |= UD_MSGCLS;
	fu->UserFlags1 |= UD_DFLAG;
	fu->UserFlags1 |= UD_AUTOFORCEONWHO;
	fu->UserFlags1 |= UD_BACKGROUNDPOST;
	strcpy(fu->UserName,au->Name);
	strcpy(fk->UserName,ak->UserName);
	strcpy(fu->Password,au->Name);						/* Bis auf weiteres */
	strcpy(fu->UserLocation,au->Location);
	strcpy(fu->UserFrom,au->Location);
	strcpy(fu->UserPhone,au->PhoneNumber);
	strcpy(fu->ConfAccess,au->Conference_Access);
	fu->Calls					=(ULONG)au->Times_Called;

	fu->Uploads				=(ULONG)au->Uploads;
	fu->BytesUpload		=(ULONG)au->Bytes_Upload;
	fu->Downloads			=(ULONG)au->Downloads;
	fu->BytesDownload	=(ULONG)au->Bytes_Download;
	fu->MessageWrite	=(ULONG)au->Messages_Posted;
	fu->MessageRead   =(ULONG)0L;
	fu->DailyByteLimit=(ULONG)au->Daily_Bytes_Limit;
	fu->DailyFileLimit=(ULONG)0L;
	fu->DailyByteDl		=(ULONG)au->Daily_Bytes_Dld;
	fu->CnfReJoin			=(ULONG)au->ConfRJoin;
	fu->LastConf			=(ULONG)0;
	fu->LastTime			=(long)au->Time_Last_On;
	fu->TimeLimit			=(long)au->Time_Limit;
	fu->TimeUsed			=(long)au->Time_Used;
	fu->FirstCall			=(long)au->AccountDate;
	fu->HighCPSDown		=(long)ak->DNcps;
	fu->HighCPSUP			=(long)ak->UPcps;
	fu->BAUD					=(long)ak->baud_rate;
	fu->ChatTime			=(long)au->ChatLimit;
	fu->ChatTimeUsed	=(long)0L;
	if(!(Stricmp(au->Name,ak->UserName)))
		{
		fu->Deleted_Or_Not=(short int)0;
		fk->Deleted_Or_Not=(int)0;
		}
	else
		{
		fu->Deleted_Or_Not=(short int)2;	/* Inaktiv */
		fk->Deleted_Or_Not=(int)2;
		}
	fu->Userlevel			=(short int)au->Sec_Status;
	fu->Ratio					=(short int)au->Sec_Library;
	fu->RatioType			=(short int)au->Sec_Board;
	fu->Ansi_On_Off		=(short int)2;								// Prompt for Ansi
	fu->NumLines			=(short int)au->LineLength;
	fu->CompuType			=(short int)au->Sec_Bulletin;
	fu->Extension			=(short int)au->ScreenType;
	fu->UserNumber		=(USHORT)au->Slot_Number;
	fk->UserNumber    =(USHORT)au->Slot_Number;
	if(Stricmp(str,"X")) fu->UserFlags1|=UD_EXPERT;
	if(ak->New_User==1)
		{
		fu->UserFlags1|=UD_NEWUSER;
		fk->NewUser=TRUE;
		}
	else
		{
		fk->NewUser=FALSE;
		}
	if(ak->Userflags) 
		{
		fu->UserFlags1|=UD_MSGCLS;
		fu->UserFlags1|=UD_FILECLS;
		}
	fu->NewScan				=(short int)4;							// Prompt for Scanning
	fu->Editor        =(USHORT)3;									// Prompt for Editor
	fu->DayRelogins   =(ULONG)3;									// 3 Relogins per Day
	fu->ShellNumLines =(ULONG)au->LineLength/2;		// Lines for Shell
	fu->UserBlockSize =(short int) 1024;					// Defaults to 1024 Bytes/Block
	fu->FileIdDiz     =(short int) 2;							// Ask for FileID
	fu->DoMsgCrypt    =(short int) 2;             // Ask for Crypting
	}

BOOL get_all(void)
	{
	if(!(au=AllocVec((sizeof(struct AUser)),MEMF_CLEAR|MEMF_PUBLIC))) return(FALSE);
	if(!(ak=AllocVec((sizeof(struct AUserKeys)),MEMF_CLEAR|MEMF_PUBLIC))) return(FALSE);
	if(!(am=AllocVec((sizeof(struct AMiscData)),MEMF_CLEAR|MEMF_PUBLIC))) return(FALSE);
	if(!(fu=AllocVec((sizeof(struct FAMEUser)),MEMF_CLEAR|MEMF_PUBLIC))) return(FALSE);
	if(!(fk=AllocVec((sizeof(struct FAMEUserKeys)),MEMF_CLEAR|MEMF_PUBLIC))) return(FALSE);
	return(TRUE);
	}

void free_all(void)
	{
	if(au) FreeVec(au);au=NULL;
	if(ak) FreeVec(ak);ak=NULL;
	if(am) FreeVec(am);am=NULL;
	if(fu) FreeVec(fu);fu=NULL;
	if(fk) FreeVec(fk);fk=NULL;
	}

void SPrintf(char *Buffer,char *ctl, ...)
	{
	RawDoFmt(ctl, (long *)(&ctl + 1), (void (*))"\x16\xc0\x4e\x75",Buffer);
	}

void CheckPW(void)
	{
	static const STRPTR byti="BBS:USER.PW";
  long		apos=NULL,counta=NULL;
	BPTR		pwfh=NULL,udata=NULL,ukeys=NULL;
	char		dest[256];
	struct	FileInfoBlock __aligned myfib;
	struct	BytiPW
		{
		char	PW[60];
		}pw;

	*dest='\0';fu=NULL;fk=NULL;
  Printf("\n[mChecking stored passwords in %s...",byti);
	Flush(Output());
	if(!(pwfh=Open(byti,MODE_OLDFILE))) { Printf("[33mFAILED !\n\n[mFile %s not found !\n",byti);return;}
	ExamineFH(pwfh,&myfib);
	apos=myfib.fib_Size/sizeof(struct BytiPW);
	if(!apos) { Close(pwfh); Printf("[33mSTOPPED !\n\nNo passwords stored in file %s !\n",byti); return;};
	Print("[33mdone !\n\n[mNow writing the existing passwords to FAME user.data...\n\n");
	if(!(fu=AllocVec(sizeof(struct FAMEUser),MEMF_CLEAR|MEMF_PUBLIC))) { Close(pwfh);Print("Unable to allocate FAME User-struct !!!\n\n");exit(0);}
	if(!(fk=AllocVec(sizeof(struct FAMEUserKeys),MEMF_CLEAR|MEMF_PUBLIC))) { Close(pwfh);FreeVec(fu);Print("Unable to allocate FAME UserKeys-struct !!!\n\n");exit(0);};
	strcpy(dest,famedir);
	AddPart(dest,"USER.DATA",255);
	if(!(udata=Open(dest,MODE_READWRITE))) { FreeVec(fu);fu=NULL;FreeVec(fk);fk=NULL;Close(pwfh);Printf("Can't open %s for writing !\n\n",dest);exit(0);};
	strcpy(dest,famedir);
	AddPart(dest,"USER.KEYS",255);
	if(!(ukeys=Open(dest,MODE_READWRITE))) { FreeVec(fu);fu=NULL;FreeVec(fk);fk=NULL;Close(pwfh);Close(udata);Printf("Can't open %s for writing !\n\n",dest);exit(0);};
  while(counta<apos)
		{
		Seek(pwfh,counta*sizeof(struct BytiPW),OFFSET_BEGINNING);
		Read(pwfh,&pw,sizeof(struct BytiPW));
		if(*pw.PW)
			{
			Seek(udata,61+(counta*sizeof(struct FAMEUser)),OFFSET_BEGINNING);
			Read(udata,fu,sizeof(struct FAMEUser));
			Printf("Found Password for User %s (Slotnr.: %ld)\n",fu->UserName,(counta+1));
			strncpy(fu->Password,pw.PW,22);
			fu->Password[21]='\0';
  	  Seek(udata,61+(counta*sizeof(struct FAMEUser)),OFFSET_BEGINNING);
			Write(udata,fu,sizeof(struct FAMEUser));
			}
		else
      {
			Seek(udata,61+(counta*sizeof(struct FAMEUser)),OFFSET_BEGINNING);
			Read(udata,fu,sizeof(struct FAMEUser));
    	Printf("%s has no PW, auto-set him to inactive state !\n",fu->UserName);
      fu->Deleted_Or_Not=2;		// Inactive State!
			Seek(udata,61+(counta*sizeof(struct FAMEUser)),OFFSET_BEGINNING);
			Write(udata,fu,sizeof(struct FAMEUser));
			Seek(ukeys,61+(counta*sizeof(struct FAMEUserKeys)),OFFSET_BEGINNING);
			Read(ukeys,fk,sizeof(struct FAMEUserKeys));
			fk->Deleted_Or_Not=2;
      Seek(ukeys,61+(counta*sizeof(struct FAMEUserKeys)),OFFSET_BEGINNING);
			Write(ukeys,fk,sizeof(struct FAMEUserKeys));
      }
		counta++;
		}
  Close(pwfh);pwfh=NULL;
	Close(udata);udata=NULL;
  FreeVec(fu);fu=NULL;
	Close(ukeys);ukeys=NULL;
  FreeVec(fk);fk=NULL;
	}
