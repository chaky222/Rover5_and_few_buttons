//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZConnection"
#pragma link "sButton"
#pragma link "ZAbstractDataset"
#pragma link "ZAbstractRODataset"
#pragma link "ZDataset"
#pragma link "sPanel"
#pragma link "ZAbstractTable"
#pragma resource "*.dfm"
 TDBForm *DBForm;
//---------------------------------------------------------------------------
__fastcall TDBForm::TDBForm(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDBForm::sButton1Click(TObject *Sender)
{
ShowMessage("asd");
}
//---------------------------------------------------------------------------

 String TDBForm::get_SQL_value(String SQL){
    //
    ZReadOnlyQuery2->Active = false;
    ZReadOnlyQuery2->SQL->Text = SQL;
    //ZReadOnlyQuery2->SQL->Clear();
    //ZReadOnlyQuery2->SQL->Add(SQL);
    ZReadOnlyQuery2->Active = true;
    String result = "";
    while (!ZReadOnlyQuery2->Eof){
        result=ZReadOnlyQuery2->Fields->FieldByNumber(1)->AsString;
        break;
    }
    return  result;
 }
