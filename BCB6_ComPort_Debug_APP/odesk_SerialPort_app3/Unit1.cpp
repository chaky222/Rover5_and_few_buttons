//=============================================================================
//................................. � ��������� ...............................
//.............................................................................
//....... ��������� ������������� ��� ������������ ������ � COM-������ ........
//....... � ������� ������� � �������������� ����������� �������� .............
//.............................................................................
//.. ������������ ������ WINAPI � ��������� ResumeThread() � SuspendThread() ..
//.............................................................................
//=============================================================================

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <io.h>         //��� ������ � �������
#include <fcntl.h>      //��� ������ � �������
#include <sys\stat.h>   //��� ������ � �������

#include "Unit1.h"
#include "Unit2.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sPanel"
#pragma link "ZConnection"
#pragma link "sTrackBar"
#pragma resource "*.dfm"
TForm1 *Form1;
//TDBForm *DBForm1;

USEFORM("Unit2.cpp", DBForm);
//=============================================================================
//..................... ���������� ���������� ���������� ......................
//=============================================================================

#define BUFSIZE 255     //������� ������

unsigned char bufrd[BUFSIZE], bufwr[BUFSIZE]; //������� � ���������� ������
String print_str = "";

//---------------------------------------------------------------------------

HANDLE COMport;		//���������� �����

//��������� OVERLAPPED ���������� ��� ����������� ��������, ��� ���� ��� �������� ������ � ������ ����� �������� ������ ���������
//��� ��������� ���������� �������� ���������, ����� ��������� �� ����� �������� ���������
OVERLAPPED overlapped;		//����� ������������ ��� �������� ������ (��. ����� ReadThread)
OVERLAPPED overlappedwr;       	//����� ������������ ��� �������� ������ (��. ����� WriteThread)

//---------------------------------------------------------------------------

//int handle;             	//���������� ��� ������ � ������ � ������� ���������� <io.h>

//---------------------------------------------------------------------------

bool fl=0;	//����, ����������� �� ���������� �������� ������ (1 - �������, 0 - �� �������)

unsigned long counter;	//������� �������� ������, ���������� ��� ������ �������� �����


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

void COMOpen(void);             //������� ����
void COMClose(void);            //������� ����


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

HANDLE reader;	//���������� ������ ������ �� �����
HANDLE writer;	//���������� ������ ������ � ����

DWORD WINAPI ReadThread(LPVOID);
DWORD WINAPI WriteThread(LPVOID);


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

//-----------------------------------------------------------------------------
//............................... ����� ReadThead .............................
//-----------------------------------------------------------------------------

void ReadPrinting(void);

//---------------------------------------------------------------------------

//������� ������� ������, ��������� ���� ������ �� COM-�����
DWORD WINAPI ReadThread(LPVOID)
{
 COMSTAT comstat;		//��������� �������� ��������� �����, � ������ ��������� ������������ ��� ����������� ���������� �������� � ���� ������
 DWORD btr, temp, mask, signal;	//���������� temp ������������ � �������� ��������

 overlapped.hEvent = CreateEvent(NULL, true, true, NULL);	//������� ���������� ������-������� ��� ����������� ��������
 SetCommMask(COMport, EV_RXCHAR);                   	        //���������� ����� �� ������������ �� ������� ����� ����� � ����
 while(1)						//���� ����� �� ����� �������, ��������� ����
  {
   WaitCommEvent(COMport, &mask, &overlapped);               	//������� ������� ����� ����� (��� � ���� ������������� ��������)
   signal = WaitForSingleObject(overlapped.hEvent, INFINITE);	//������������� ����� �� ������� �����
   if(signal == WAIT_OBJECT_0)				        //���� ������� ������� ����� ���������
    {
     if(GetOverlappedResult(COMport, &overlapped, &temp, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
      if((mask & EV_RXCHAR)!=0)					//���� ��������� ������ ������� ������� �����
       {
        ClearCommError(COMport, &temp, &comstat);		//����� ��������� ��������� COMSTAT
        btr = comstat.cbInQue;                          	//� �������� �� �� ���������� �������� ������
        if(btr)                         			//���� ������������� ���� ����� ��� ������
        {
         ReadFile(COMport, bufrd, btr, &temp, &overlapped);     //��������� ����� �� ����� � ����� ���������
         counter+=btr;                                          //����������� ������� ������
         ReadPrinting();                      		//�������� ������� ��� ������ ������ �� ����� � � ����
        }
       }
    }
  }
}

//---------------------------------------------------------------------------

//������� �������� ����� �� ����� � � ���� (���� ��������)
void ReadPrinting()
{
 //Form1->Memo1->Lines->Add((char*)bufrd);	 //������� �������� ������ � Memo
 for (int i=0;i<BUFSIZE;i++){
    byte c = bufrd[i];
    if (c==0){
        break;
    }
    //Form1->Memo1->Lines->Add(IntToStr(c));
    if ((c==10)||(c==13)){
       // print_str = print_str+(AnsiString)(char)13+(AnsiString)(char)10;
       if (print_str.Length() >0){
         Form1->received_cmd(print_str);
       }            
        print_str="";
    }else{
        print_str = print_str+(char)c;
    }
 }

 Form1->StatusBar1->Panels->Items[2]->Text = "����� ������� " + IntToStr(counter) + " ����";	//������� ������� � ������ ���������

// if(Form1->CheckBox3->Checked == true)  //���� ������� ����� ������ � ����
 // {
 //  write(handle, bufrd, strlen(bufrd)); //�������� � ���� ������ �� �������� ������
 // }
 memset(bufrd, 0, BUFSIZE);   	        //�������� ����� (����� ������ �� ������������� ���� �� �����)
}

//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//............................... ����� WriteThead ............................
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------

//������� ������� ������, ��������� �������� ������ �� ������ � COM-����
DWORD WINAPI WriteThread(LPVOID)
{
 DWORD temp, signal;

 overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  //������� �������
 while(1)
  {WriteFile(COMport, bufwr, strlen(bufwr), &temp, &overlappedwr);  //�������� ����� � ���� (������������� ��������!)
   signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE);	  //������������� �����, ���� �� ���������� ������������� �������� WriteFile

   if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//���� �������� ����������� �������
     {
      Form1->StatusBar1->Panels->Items[0]->Text  = "Sended success";    //������� ��������� �� ���� � ������ ���������
     }
   else {Form1->StatusBar1->Panels->Items[0]->Text  = "Error while sending";} 	//����� ������� � ������ ��������� ��������� �� ������

   SuspendThread(writer);
   
  }
}

//---------------------------------------------------------------------------


//=============================================================================
//............................. �������� ����� ................................
//=============================================================================

//---------------------------------------------------------------------------

//����������� �����, ������ � �� ����������� ������������� ��������� �����
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
 //������������� ��������� ����� ��� ������� ���������

 Form1->Button1->Enabled = false;
 Form1->CheckBox1->Enabled = false;
 Form1->CheckBox2->Enabled = false;
}

//---------------------------------------------------------------------------

//���������� ������� �� ������ "������� ����"
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
 if(SpeedButton1->Down)
  {
   COMOpen();                   //���� ������ ������ - ������� ����

   //��������/�������� �������� �� �����
   Form1->ComboBox1->Enabled = false;
   Form1->ComboBox2->Enabled = false;
   Form1->Button1->Enabled = true;
   Form1->CheckBox1->Enabled = true;
   Form1->CheckBox2->Enabled = true;

   Form1->SpeedButton1->Caption = "Close Port";	//������� ������� �� ������

   counter = 0;	//���������� ������� ������

   //���� ���� �������� ������ DTR � RTS, ���������� ��� ����� � �������
   Form1->CheckBox1Click(Sender);
   Form1->CheckBox2Click(Sender);
  }

 else
  {
   COMClose();                  //���� ������ ������ - ������� ����

   Form1->SpeedButton1->Caption = "Open serialPort";	//������� ������� �� ������
   Form1->StatusBar1->Panels->Items[0]->Text = "";	//�������� ������ ������� ������ ���������

   //��������/�������� �������� �� �����
   Form1->ComboBox1->Enabled = true;
   Form1->ComboBox2->Enabled = true;
   Form1->Button1->Enabled = false;
   Form1->CheckBox1->Enabled = false;
   Form1->CheckBox2->Enabled = false;
  }
}

//---------------------------------------------------------------------------

//���������� �������� �����
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
 COMClose();
}
//---------------------------------------------------------------------------


//������ "��������"
void __fastcall TForm1::Button1Click(TObject *Sender)
{
    run_cmd(Form1->Edit1->Text);
}

//---------------------------------------------------------------------------

//������ "�������� ����"
void __fastcall TForm1::Button3Click(TObject *Sender)
{
 Form1->Memo1->Clear();	//�������� Memo1
}

//---------------------------------------------------------------------------

//������� "DTR"
void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
 //���� ����������� - ���������� ����� DTR � �������, ����� - � ����
 if(Form1->CheckBox1->Checked) EscapeCommFunction(COMport, SETDTR);
 else EscapeCommFunction(COMport, CLRDTR);
}

//---------------------------------------------------------------------------

//������� "RTS"
void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
 //���� ����������� - ���������� ����� RTS � �������, ����� - � ����
 if(Form1->CheckBox2->Checked) EscapeCommFunction(COMport, SETRTS);
 else EscapeCommFunction(COMport, CLRRTS);
}

//---------------------------------------------------------------------------

//=============================================================================
//........................... ���������� ������� ..............................
//=============================================================================

//---------------------------------------------------------------------------

//������� �������� � ������������� �����
void COMOpen()
{
 String portname;     	 //��� ����� (��������, "COM1", "COM2" � �.�.)
 DCB dcb;                //��������� ��� ����� ������������� ����� DCB
 COMMTIMEOUTS timeouts;  //��������� ��� ��������� ���������
 
 portname = Form1->ComboBox1->Text;	//�������� ��� ���������� �����

 //������� ����, ��� ����������� �������� ����������� ����� ������� ���� FILE_FLAG_OVERLAPPED
 COMport = CreateFile(portname.c_str(),GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
 //�����:
 // - portname.c_str() - ��� ����� � �������� ����� �����, c_str() ����������� ������ ���� String � ������ � ���� ������� ���� char, ����� ������� �� ������
 // - GENERIC_READ | GENERIC_WRITE - ������ � ����� �� ������/�������
 // - 0 - ���� �� ����� ���� ������������� (shared)
 // - NULL - ���������� ����� �� �����������, ������������ ���������� ������������ �� ���������
 // - OPEN_EXISTING - ���� ������ ����������� ��� ��� ������������ ����
 // - FILE_FLAG_OVERLAPPED - ���� ���� ��������� �� ������������� ����������� ��������
 // - NULL - ��������� �� ���� ������� �� ������������ ��� ������ � �������

 if(COMport == INVALID_HANDLE_VALUE)            //���� ������ �������� �����
  {
   Form1->SpeedButton1->Down = false;           //������ ������
   Form1->StatusBar1->Panels->Items[0]->Text = "Could not open port";       //������� ��������� � ������ ���������
   return;
  }

 //������������� �����

 dcb.DCBlength = sizeof(DCB); 	//� ������ ���� ��������� DCB ���������� ������� � �����, ��� ����� �������������� ��������� ��������� ����� ��� �������� ������������ ���������

 //������� ��������� DCB �� �����
 if(!GetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "�� ������� ������� DCB";
   return;
  }

 //������������� ��������� DCB
 dcb.BaudRate = StrToInt(Form1->ComboBox2->Text);       //����� �������� �������� 115200 ���
 dcb.fBinary = TRUE;                                    //�������� �������� ����� ������
 dcb.fOutxCtsFlow = FALSE;                              //��������� ����� �������� �� �������� CTS
 dcb.fOutxDsrFlow = FALSE;                              //��������� ����� �������� �� �������� DSR
 dcb.fDtrControl = DTR_CONTROL_DISABLE;                 //��������� ������������� ����� DTR
 dcb.fDsrSensitivity = FALSE;                           //��������� ��������������� �������� � ��������� ����� DSR
 dcb.fNull = FALSE;                                     //��������� ���� ������� ������
 dcb.fRtsControl = RTS_CONTROL_DISABLE;                 //��������� ������������� ����� RTS
 dcb.fAbortOnError = FALSE;                             //��������� ��������� ���� �������� ������/������ ��� ������
 dcb.ByteSize = 8;                                      //����� 8 ��� � �����
 dcb.Parity = 0;                                        //��������� �������� ��������
 dcb.StopBits = 0;                                      //����� ���� ����-���

 //��������� ��������� DCB � ����
 if(!SetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Failed to set DCB";
   return;
  }

 //���������� ��������
 timeouts.ReadIntervalTimeout = 0;	 	//������� ����� ����� ���������
 timeouts.ReadTotalTimeoutMultiplier = 0;	//����� ������� �������� ������
 timeouts.ReadTotalTimeoutConstant = 0;         //��������� ��� ������ �������� �������� ������
 timeouts.WriteTotalTimeoutMultiplier = 0;      //����� ������� �������� ������
 timeouts.WriteTotalTimeoutConstant = 0;        //��������� ��� ������ �������� �������� ������

 //�������� ��������� ��������� � ����
 if(!SetCommTimeouts(COMport, &timeouts))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "�� ������� ���������� ����-����";
   return;
  }

 //���������� ������� �������� ����� � ��������
 SetupComm(COMport,2000,2000);
  
 //������� ��� ������� ������������ ���� ��� ������ ����������� ������



 PurgeComm(COMport, PURGE_RXCLEAR);	//�������� ����������� ����� �����

 reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//������ ����� ������, ������� ����� ����� ����������� (������������� �������� = 0)
 writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//������ ����� ������ � ������������� ��������� (������������� �������� = CREATE_SUSPENDED)

}

//---------------------------------------------------------------------------

//������� �������� �����
void COMClose()
{
//����������: ��� ��� ��� ���������� �������, ��������� � ������� ������� WinAPI, �������� TerminateThread
//	      ����� ����� ���� ������� �����, � ����� ����� ������ ����������, �� ����������� ����������
//	      ����������� �������-�������, ������������ � ��������� ���� OVERLAPPED, ��������� � �������,
//	      ������� �� ������ ���� ������, � ��������, ����� ������ ������� TerminateThread.
//	      ����� ���� ����� ���������� � ��� ���������� ������.

 if(writer)		//���� ����� ������ ��������, ��������� ���; �������� if(writer) �����������, ����� ��������� ������
  {TerminateThread(writer,0);
   CloseHandle(overlappedwr.hEvent);	//����� ������� ������-�������
   CloseHandle(writer);
  }
 if(reader)		   //���� ����� ������ ��������, ��������� ���; �������� if(reader) �����������, ����� ��������� ������
  {TerminateThread(reader,0);
   CloseHandle(overlapped.hEvent);	//����� ������� ������-�������
   CloseHandle(reader);
  }

 CloseHandle(COMport);                  //������� ����
 COMport=0;				//�������� ���������� ��� ����������� �����
}
void split(TStringList* lout, char* str, const char* separator) {       for(char* tok = strtok(str, separator); tok; tok = strtok(NULL, separator))          lout->Add(tok);}

void __fastcall TForm1::received_cmd(String s, bool print_error){  //
  TStringList*  lst = new TStringList();
  String s2 = s+" ";
  //Form1->Memo1->Lines->Add("1["+s+"]");
  char *ch = new char[s.Length()];
  ch = s.c_str();
  split(lst, ch, " ");
  for(int i = lst->Count; i < 20; i++) {lst->Add("");}
  if (lst->Strings[0] == "[CMD]"){
    String s1 = "";
    int first_param = 1;
    if (lst->Strings[1] == "[HIDDEN]"){
      first_param=2;
    }else{
      first_param=1;
      for(int i = 1; i < lst->Count; i++) {s1 = s1+" "+lst->Strings[i];}
      Form1->Memo1->Lines->Add("["+s1+"]");
    }
    do_cmd(lst->Strings[first_param],lst->Strings[first_param+1],lst->Strings[first_param+2],lst->Strings[first_param+3],lst->Strings[first_param+4],lst->Strings[first_param+5]);
  }else{
    Form1->Memo1->Lines->Add("["+s2+"]");
  }
//  do_cmd();
  
}
void __fastcall TForm1::do_cmd(String cmd,String p1,String p2,String p3,String p4,String p5){//
  bool cmd_done = false;
  if (cmd == "G2"){
    curr1->Caption = p1;
    curr2->Caption = p2;
    curr3->Caption = p3;
    curr4->Caption = p4;
    cmd_done = true;
  }
  if (cmd == "G3"){
    pos1->Caption = p1;
    pos2->Caption = p2;
    pos3->Caption = p3;
    pos4->Caption = p4;
    cmd_done = true;
  }
  if (cmd == "G5"){//run motor
    //Memo1->Lines->Add("["+p1+"] ["+p2+"]");
    int ip1= StrToInt(p1);
    if ((ip1>-1) && (ip1<4)){
      TCheckBox * c[4] = {Motor0,Motor1,Motor2,Motor3};
      if (p2 == "1"){
         c[ip1]->Font->Color=clBlue;
      }else{
         c[ip1]->Font->Color=clGreen;
      }
      c[ip1]->Checked = true;
    }
    cmd_done = true;
  }
  if (cmd == "G7"){//stopped motor
    //Memo1->Lines->Add("["+p1+"]");
    int ip1= StrToInt(p1);
    if ((ip1>-1) && (ip1<4)){
      TCheckBox *c[4] = {Motor0,Motor1,Motor2,Motor3};
      c[ip1]->Font->Color=clRed;
      c[ip1]->Checked = false;
    }
    cmd_done = true;
  }
  if (cmd == "G10"){// run to dest in percent
    if (CheckBox4->Checked){
        TrackBar1->Position = StrToInt(p1);
    }
    cmd_done = true;                              
  }
  if (cmd == "G11"){// set global_motor_max_pwm_power
    if (CheckBox3->Checked){
        sTrackBar1->Position = StrToInt(p1);
    }
    cmd_done = true;
  }
  if (cmd == "G15"){//  set motor dest
    int motor_num =  StrToInt(p1);
    TLabel *ls[4] = {Label4,Label6,Label7,Label8};
    for (int u = 0;u<4;u++){
        if (ls[u]->Tag ==  motor_num){
             ls[u]->Caption = p2;
        }
    }
    cmd_done = true;
  }
  if (cmd == "G16"){//  set motor max_pos
    Label9->Caption  = p1;
    Label10->Caption = p2;
    Label11->Caption = p3;
    Label12->Caption = p4;
    cmd_done = true;
  }

  if (cmd == "G8"){//stop all motors
         Motor0->Checked = false;
         Motor1->Checked = false;
         Motor2->Checked = false;
         Motor3->Checked = false;
    cmd_done = true;
  }

  if (!cmd_done){
    Form1->Memo1->Lines->Add("["+cmd+"] "+p1+" "+p2+" "+p3+" "+p4+" "+p5+" ");
  }
}

//---------------------------------------------------------------------------
void __fastcall TForm1::run_cmd(String s, bool print_error){  //
  if (Button1->Enabled){
        //Memo1->Lines->Add("["+s+"]");
        String s1 =  s + "\n\n";
        memset(bufwr,0,BUFSIZE);			//�������� ����������� ���������� �����, ����� ������ �� ������������� ���� �� �����
        PurgeComm(COMport, PURGE_TXCLEAR);             //�������� ���������� ����� �����
        strcpy(bufwr,s1.c_str());      //������� � ����������� ���������� ����� ������ �� Edit1

        ResumeThread(writer);               //������������ ����� ������ ������ � ����
        Sleep(1);//wait for send
  }else{
    if (print_error){
      ShowMessage("Connect to the arduino board in first! Press Open Port button!");
    }
  }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::DBForm_buttonClick(TObject *Sender)
{
//DBForm1 = new TDBForm(Form1);

  DBForm->Show();

}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
  DBcreated = false;  
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormShow(TObject *Sender)
{
  if(!DBcreated){
     Application->CreateForm(__classid(TDBForm), &DBForm);
     DBcreated = true;  
  }  
}
//---------------------------------------------------------------------------
void __fastcall TForm1::refresh_description(){
  String selected = DBGrid1->SelectedField->AsString;
  String code = "";
  String default_cmd = "";
  String description = "";
  if(DBcreated){
    code      = DBForm->get_SQL_value("SELECT code FROM codes_tb where id = "+selected);
    default_cmd = DBForm->get_SQL_value("SELECT default_cmd FROM codes_tb where id = "+selected);
    description = DBForm->get_SQL_value("SELECT description FROM codes_tb where id = "+selected);
  }
  Memo2->Text = "command "+code+" ("+default_cmd+") description: "+description;
//  Label3->Caption = selected + " " + s1;
}




void __fastcall TForm1::DBGrid1KeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
refresh_description();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DBGrid1CellClick(TColumn *Column)
{
refresh_description();  
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button2Click(TObject *Sender)
{
  String selected = DBGrid1->SelectedField->AsString;   
   if(DBcreated){
      Form1->StatusBar1->Panels->Items[0]->Text =  selected;
      String SQL =  "SELECT default_cmd FROM codes_tb where id = '"+selected+"'";
      Edit1->Text = DBForm->get_SQL_value(SQL);
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DBGrid1DblClick(TObject *Sender)
{
  Button2->Click();
  Button1->Click();  
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  run_cmd("G2",false);
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Motor1Click(TObject *Sender)
{
  l_speed->Caption = IntToStr(sTrackBar1->Position  );
 
 /* bool need_run[4] = {(Motor0->Checked),(Motor1->Checked),(Motor2->Checked),(Motor3->Checked)};
  int speed = sTrackBar1->Position ;
  l_speed->Caption = IntToStr(sTrackBar1->Position  );
  String dir = "D0";
  if (SpeedButton3->Down) dir = "D1";
  if (SpeedButton2->Down){
    for (int i = 0;i<4;i++){
      if (need_run[i]){
        run_cmd("G5 M"+IntToStr(i)+" "+dir+" P"+IntToStr(sTrackBar1->Position)+" ",false);
      }else{
        run_cmd("G7 M"+IntToStr(i)+" ",false);
      }
    }
  }else{
    if (Sender == SpeedButton2){
      for (int i = 0;i<4;i++){ run_cmd("G7 M"+IntToStr(i)+" ",false);}
    }
  }  */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::sTrackBar2Change(TObject *Sender)
{
 Label5->Caption = IntToStr(TrackBar1->Position  );
 
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button4Click(TObject *Sender)
{
run_cmd("G9 ",false);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
   int t = (CheckBox4->Checked ) ? 1 : 0;
   int s = (CheckBox3->Checked ) ? 1 : 0;
   run_cmd("G12 T"+IntToStr(t)+" S"+IntToStr(s)+" ",false);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
if (!CheckBox4->Checked){
   run_cmd("G10 P"+IntToStr(TrackBar1->Position),false);
 }        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
 if (!CheckBox3->Checked){
   run_cmd("G11 P"+IntToStr(sTrackBar1->Position),false);
 }        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
run_cmd("G5 ",false);        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
run_cmd("G6 ",false);          
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
run_cmd("G7 ",false);
}
//---------------------------------------------------------------------------

