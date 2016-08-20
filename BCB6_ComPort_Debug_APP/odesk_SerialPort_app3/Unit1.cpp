//=============================================================================
//................................. О программе ...............................
//.............................................................................
//....... Программа предназначена для демонстрации работы с COM-портом ........
//....... с помощью потоков с использованием асинхронных операций .............
//.............................................................................
//.. Используются потоки WINAPI с функциями ResumeThread() и SuspendThread() ..
//.............................................................................
//=============================================================================

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <io.h>         //для работы с файлами
#include <fcntl.h>      //для работы с файлами
#include <sys\stat.h>   //для работы с файлами

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
//..................... объявления глобальных переменных ......................
//=============================================================================

#define BUFSIZE 255     //ёмкость буфера

unsigned char bufrd[BUFSIZE], bufwr[BUFSIZE]; //приёмный и передающий буферы
String print_str = "";

//---------------------------------------------------------------------------

HANDLE COMport;		//дескриптор порта

//структура OVERLAPPED необходима для асинхронных операций, при этом для операции чтения и записи нужно объявить разные структуры
//эти структуры необходимо объявить глобально, иначе программа не будет работать правильно
OVERLAPPED overlapped;		//будем использовать для операций чтения (см. поток ReadThread)
OVERLAPPED overlappedwr;       	//будем использовать для операций записи (см. поток WriteThread)

//---------------------------------------------------------------------------

//int handle;             	//дескриптор для работы с файлом с помощью библиотеки <io.h>

//---------------------------------------------------------------------------

bool fl=0;	//флаг, указывающий на успешность операций записи (1 - успешно, 0 - не успешно)

unsigned long counter;	//счётчик принятых байтов, обнуляется при каждом открытии порта


//=============================================================================
//.............................. объявления функций ...........................
//=============================================================================

void COMOpen(void);             //открыть порт
void COMClose(void);            //закрыть порт


//=============================================================================
//.............................. объявления потоков ...........................
//=============================================================================

HANDLE reader;	//дескриптор потока чтения из порта
HANDLE writer;	//дескриптор потока записи в порт

DWORD WINAPI ReadThread(LPVOID);
DWORD WINAPI WriteThread(LPVOID);


//=============================================================================
//.............................. реализация потоков ...........................
//=============================================================================

//-----------------------------------------------------------------------------
//............................... поток ReadThead .............................
//-----------------------------------------------------------------------------

void ReadPrinting(void);

//---------------------------------------------------------------------------

//главная функция потока, реализует приём байтов из COM-порта
DWORD WINAPI ReadThread(LPVOID)
{
 COMSTAT comstat;		//структура текущего состояния порта, в данной программе используется для определения количества принятых в порт байтов
 DWORD btr, temp, mask, signal;	//переменная temp используется в качестве заглушки

 overlapped.hEvent = CreateEvent(NULL, true, true, NULL);	//создать сигнальный объект-событие для асинхронных операций
 SetCommMask(COMport, EV_RXCHAR);                   	        //установить маску на срабатывание по событию приёма байта в порт
 while(1)						//пока поток не будет прерван, выполняем цикл
  {
   WaitCommEvent(COMport, &mask, &overlapped);               	//ожидать события приёма байта (это и есть перекрываемая операция)
   signal = WaitForSingleObject(overlapped.hEvent, INFINITE);	//приостановить поток до прихода байта
   if(signal == WAIT_OBJECT_0)				        //если событие прихода байта произошло
    {
     if(GetOverlappedResult(COMport, &overlapped, &temp, true)) //проверяем, успешно ли завершилась перекрываемая операция WaitCommEvent
      if((mask & EV_RXCHAR)!=0)					//если произошло именно событие прихода байта
       {
        ClearCommError(COMport, &temp, &comstat);		//нужно заполнить структуру COMSTAT
        btr = comstat.cbInQue;                          	//и получить из неё количество принятых байтов
        if(btr)                         			//если действительно есть байты для чтения
        {
         ReadFile(COMport, bufrd, btr, &temp, &overlapped);     //прочитать байты из порта в буфер программы
         counter+=btr;                                          //увеличиваем счётчик байтов
         ReadPrinting();                      		//вызываем функцию для вывода данных на экран и в файл
        }
       }
    }
  }
}

//---------------------------------------------------------------------------

//выводим принятые байты на экран и в файл (если включено)
void ReadPrinting()
{
 //Form1->Memo1->Lines->Add((char*)bufrd);	 //выводим принятую строку в Memo
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

 Form1->StatusBar1->Panels->Items[2]->Text = "Всего принято " + IntToStr(counter) + " байт";	//выводим счётчик в строке состояния

// if(Form1->CheckBox3->Checked == true)  //если включен режим вывода в файл
 // {
 //  write(handle, bufrd, strlen(bufrd)); //записать в файл данные из приёмного буфера
 // }
 memset(bufrd, 0, BUFSIZE);   	        //очистить буфер (чтобы данные не накладывались друг на друга)
}

//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//............................... поток WriteThead ............................
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------

//главная функция потока, выполняет передачу байтов из буфера в COM-порт
DWORD WINAPI WriteThread(LPVOID)
{
 DWORD temp, signal;

 overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  //создать событие
 while(1)
  {WriteFile(COMport, bufwr, strlen(bufwr), &temp, &overlappedwr);  //записать байты в порт (перекрываемая операция!)
   signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE);	  //приостановить поток, пока не завершится перекрываемая операция WriteFile

   if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//если операция завершилась успешно
     {
      Form1->StatusBar1->Panels->Items[0]->Text  = "Sended success";    //вывести сообщение об этом в строке состояния
     }
   else {Form1->StatusBar1->Panels->Items[0]->Text  = "Error while sending";} 	//иначе вывести в строке состояния сообщение об ошибке

   SuspendThread(writer);
   
  }
}

//---------------------------------------------------------------------------


//=============================================================================
//............................. элементы формы ................................
//=============================================================================

//---------------------------------------------------------------------------

//конструктор формы, обычно в нём выполняется инициализация элементов формы
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
 //инициализация элементов формы при запуске программы

 Form1->Button1->Enabled = false;
 Form1->CheckBox1->Enabled = false;
 Form1->CheckBox2->Enabled = false;
}

//---------------------------------------------------------------------------

//обработчик нажатия на кнопку "Открыть порт"
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
 if(SpeedButton1->Down)
  {
   COMOpen();                   //если кнопка нажата - открыть порт

   //показать/спрятать элементы на форме
   Form1->ComboBox1->Enabled = false;
   Form1->ComboBox2->Enabled = false;
   Form1->Button1->Enabled = true;
   Form1->CheckBox1->Enabled = true;
   Form1->CheckBox2->Enabled = true;

   Form1->SpeedButton1->Caption = "Close Port";	//сменить надпись на кнопке

   counter = 0;	//сбрасываем счётчик байтов

   //если были включены флажки DTR и RTS, установить эти линии в единицу
   Form1->CheckBox1Click(Sender);
   Form1->CheckBox2Click(Sender);
  }

 else
  {
   COMClose();                  //если кнопка отжата - закрыть порт

   Form1->SpeedButton1->Caption = "Open serialPort";	//сменить надпись на кнопке
   Form1->StatusBar1->Panels->Items[0]->Text = "";	//очистить первую колонку строки состояния

   //показать/спрятать элементы на форме
   Form1->ComboBox1->Enabled = true;
   Form1->ComboBox2->Enabled = true;
   Form1->Button1->Enabled = false;
   Form1->CheckBox1->Enabled = false;
   Form1->CheckBox2->Enabled = false;
  }
}

//---------------------------------------------------------------------------

//обработчик закрытия формы
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
 COMClose();
}
//---------------------------------------------------------------------------


//кнопка "Передать"
void __fastcall TForm1::Button1Click(TObject *Sender)
{
    run_cmd(Form1->Edit1->Text);
}

//---------------------------------------------------------------------------

//кнопка "Очистить поле"
void __fastcall TForm1::Button3Click(TObject *Sender)
{
 Form1->Memo1->Clear();	//очистить Memo1
}

//---------------------------------------------------------------------------

//галочка "DTR"
void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
 //если установлена - установить линию DTR в единицу, иначе - в ноль
 if(Form1->CheckBox1->Checked) EscapeCommFunction(COMport, SETDTR);
 else EscapeCommFunction(COMport, CLRDTR);
}

//---------------------------------------------------------------------------

//галочка "RTS"
void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
 //если установлена - установить линию RTS в единицу, иначе - в ноль
 if(Form1->CheckBox2->Checked) EscapeCommFunction(COMport, SETRTS);
 else EscapeCommFunction(COMport, CLRRTS);
}

//---------------------------------------------------------------------------

//=============================================================================
//........................... реализации функций ..............................
//=============================================================================

//---------------------------------------------------------------------------

//функция открытия и инициализации порта
void COMOpen()
{
 String portname;     	 //имя порта (например, "COM1", "COM2" и т.д.)
 DCB dcb;                //структура для общей инициализации порта DCB
 COMMTIMEOUTS timeouts;  //структура для установки таймаутов
 
 portname = Form1->ComboBox1->Text;	//получить имя выбранного порта

 //открыть порт, для асинхронных операций обязательно нужно указать флаг FILE_FLAG_OVERLAPPED
 COMport = CreateFile(portname.c_str(),GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
 //здесь:
 // - portname.c_str() - имя порта в качестве имени файла, c_str() преобразует строку типа String в строку в виде массива типа char, иначе функция не примет
 // - GENERIC_READ | GENERIC_WRITE - доступ к порту на чтение/записть
 // - 0 - порт не может быть общедоступным (shared)
 // - NULL - дескриптор порта не наследуется, используется дескриптор безопасности по умолчанию
 // - OPEN_EXISTING - порт должен открываться как уже существующий файл
 // - FILE_FLAG_OVERLAPPED - этот флаг указывает на использование асинхронных операций
 // - NULL - указатель на файл шаблона не используется при работе с портами

 if(COMport == INVALID_HANDLE_VALUE)            //если ошибка открытия порта
  {
   Form1->SpeedButton1->Down = false;           //отжать кнопку
   Form1->StatusBar1->Panels->Items[0]->Text = "Could not open port";       //вывести сообщение в строке состояния
   return;
  }

 //инициализация порта

 dcb.DCBlength = sizeof(DCB); 	//в первое поле структуры DCB необходимо занести её длину, она будет использоваться функциями настройки порта для контроля корректности структуры

 //считать структуру DCB из порта
 if(!GetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Не удалось считать DCB";
   return;
  }

 //инициализация структуры DCB
 dcb.BaudRate = StrToInt(Form1->ComboBox2->Text);       //задаём скорость передачи 115200 бод
 dcb.fBinary = TRUE;                                    //включаем двоичный режим обмена
 dcb.fOutxCtsFlow = FALSE;                              //выключаем режим слежения за сигналом CTS
 dcb.fOutxDsrFlow = FALSE;                              //выключаем режим слежения за сигналом DSR
 dcb.fDtrControl = DTR_CONTROL_DISABLE;                 //отключаем использование линии DTR
 dcb.fDsrSensitivity = FALSE;                           //отключаем восприимчивость драйвера к состоянию линии DSR
 dcb.fNull = FALSE;                                     //разрешить приём нулевых байтов
 dcb.fRtsControl = RTS_CONTROL_DISABLE;                 //отключаем использование линии RTS
 dcb.fAbortOnError = FALSE;                             //отключаем остановку всех операций чтения/записи при ошибке
 dcb.ByteSize = 8;                                      //задаём 8 бит в байте
 dcb.Parity = 0;                                        //отключаем проверку чётности
 dcb.StopBits = 0;                                      //задаём один стоп-бит

 //загрузить структуру DCB в порт
 if(!SetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Failed to set DCB";
   return;
  }

 //установить таймауты
 timeouts.ReadIntervalTimeout = 0;	 	//таймаут между двумя символами
 timeouts.ReadTotalTimeoutMultiplier = 0;	//общий таймаут операции чтения
 timeouts.ReadTotalTimeoutConstant = 0;         //константа для общего таймаута операции чтения
 timeouts.WriteTotalTimeoutMultiplier = 0;      //общий таймаут операции записи
 timeouts.WriteTotalTimeoutConstant = 0;        //константа для общего таймаута операции записи

 //записать структуру таймаутов в порт
 if(!SetCommTimeouts(COMport, &timeouts))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Не удалось установить тайм-ауты";
   return;
  }

 //установить размеры очередей приёма и передачи
 SetupComm(COMport,2000,2000);
  
 //создать или открыть существующий файл для записи принимаемых данных



 PurgeComm(COMport, PURGE_RXCLEAR);	//очистить принимающий буфер порта

 reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//создаём поток чтения, который сразу начнёт выполняться (предпоследний параметр = 0)
 writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//создаём поток записи в остановленном состоянии (предпоследний параметр = CREATE_SUSPENDED)

}

//---------------------------------------------------------------------------

//функция закрытия порта
void COMClose()
{
//Примечание: так как при прерывании потоков, созданных с помощью функций WinAPI, функцией TerminateThread
//	      поток может быть прерван жёстко, в любом месте своего выполнения, то освобождать дескриптор
//	      сигнального объекта-события, находящегося в структуре типа OVERLAPPED, связанной с потоком,
//	      следует не внутри кода потока, а отдельно, после вызова функции TerminateThread.
//	      После чего нужно освободить и сам дескриптор потока.

 if(writer)		//если поток записи работает, завершить его; проверка if(writer) обязательна, иначе возникают ошибки
  {TerminateThread(writer,0);
   CloseHandle(overlappedwr.hEvent);	//нужно закрыть объект-событие
   CloseHandle(writer);
  }
 if(reader)		   //если поток чтения работает, завершить его; проверка if(reader) обязательна, иначе возникают ошибки
  {TerminateThread(reader,0);
   CloseHandle(overlapped.hEvent);	//нужно закрыть объект-событие
   CloseHandle(reader);
  }

 CloseHandle(COMport);                  //закрыть порт
 COMport=0;				//обнулить переменную для дескриптора порта
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
        memset(bufwr,0,BUFSIZE);			//очистить программный передающий буфер, чтобы данные не накладывались друг на друга
        PurgeComm(COMport, PURGE_TXCLEAR);             //очистить передающий буфер порта
        strcpy(bufwr,s1.c_str());      //занести в программный передающий буфер строку из Edit1

        ResumeThread(writer);               //активировать поток записи данных в порт
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

