//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "sPanel.hpp"
#include <DBGrids.hpp>
#include <Grids.hpp>
#include "sTrackBar.hpp"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TComboBox *ComboBox1;
        TLabel *Label2;
        TComboBox *ComboBox2;
        TGroupBox *GroupBox2;
        TMemo *Memo1;
        TSpeedButton *SpeedButton1;
        TStatusBar *StatusBar1;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox2;
        TButton *Button3;
        TEdit *Edit1;
        TButton *Button1;
        TsPanel *sPanel1;
        TsPanel *sPanel2;
        TsPanel *sPanel3;
  TButton *DBForm_button;
  TDBGrid *DBGrid1;
  TsPanel *sPanel4;
  TLabel *Label3;
  TMemo *Memo2;
  TButton *Button2;
  TGroupBox *GroupBox3;
  TSpeedButton *SpeedButton2;
  TGroupBox *GroupBox4;
  TLabel *curr1;
  TLabel *curr2;
  TLabel *curr3;
  TLabel *curr4;
  TTimer *Timer1;
  TGroupBox *GroupBox5;
  TLabel *pos1;
  TLabel *pos2;
  TLabel *pos3;
  TLabel *pos4;
        TSpeedButton *SpeedButton3;
        TsTrackBar *sTrackBar1;
        TLabel *l_speed;
        TGroupBox *GroupBox6;
        TCheckBox *Motor0;
        TCheckBox *Motor1;
        TCheckBox *Motor2;
        TCheckBox *Motor3;
  TLabel *Label5;
  TCheckBox *CheckBox3;
        TButton *Button4;
        TCheckBox *CheckBox4;
        TGroupBox *GroupBox7;
        TLabel *Label4;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TGroupBox *GroupBox8;
        TGroupBox *GroupBox9;
        TGroupBox *GroupBox10;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TTrackBar *TrackBar1;
        TButton *Button5;
        TButton *Button6;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
        void __fastcall DBForm_buttonClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall DBGrid1KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall DBGrid1CellClick(TColumn *Column);
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall DBGrid1DblClick(TObject *Sender);
  void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Motor1Click(TObject *Sender);
  void __fastcall sTrackBar2Change(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall CheckBox4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        void __fastcall run_cmd(String s, bool print_error = true);
        void __fastcall received_cmd(String s, bool print_error = true);
        void __fastcall do_cmd(String cmd,String p1,String p2,String p3,String p4,String p5);
        void __fastcall refresh_description();
        bool DBcreated;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
