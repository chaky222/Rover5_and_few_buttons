//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZConnection.hpp"
#include "sButton.hpp"
#include "ZAbstractDataset.hpp"
#include "ZAbstractRODataset.hpp"
#include "ZDataset.hpp"
#include <DB.hpp>
#include <DBGrids.hpp>
#include <Grids.hpp>
#include "sPanel.hpp"
#include <ExtCtrls.hpp>
#include <DBCtrls.hpp>
#include "ZAbstractTable.hpp"
//---------------------------------------------------------------------------
class TDBForm : public TForm
{
__published:	// IDE-managed Components
        TZConnection *ZConnection1;
  TsButton *sButton1;
  TDataSource *DataSource1;
  TZReadOnlyQuery *ZReadOnlyQuery1;
  TZReadOnlyQuery *ZReadOnlyQuery2;
  TsPanel *sPanel1;
  TDBNavigator *DBNavigator1;
  TZTable *ZTable1;
  TDBGrid *DBGrid1;
  TDataSource *DataSource2;
  void __fastcall sButton1Click(TObject *Sender);

private:	// User declarations
public:		// User declarations
        __fastcall TDBForm(TComponent* Owner);
        String get_SQL_value(String SQL);

};
//---------------------------------------------------------------------------
extern PACKAGE TDBForm *DBForm;
//---------------------------------------------------------------------------
#endif
