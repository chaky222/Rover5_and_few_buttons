object DBForm: TDBForm
  Left = 440
  Top = 284
  Width = 517
  Height = 322
  Caption = 'DBForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object sButton1: TsButton
    Left = 124
    Top = 172
    Width = 75
    Height = 25
    Caption = 'sButton1'
    TabOrder = 0
    OnClick = sButton1Click
    SkinData.SkinSection = 'BUTTON'
  end
  object sPanel1: TsPanel
    Left = 0
    Top = 244
    Width = 509
    Height = 51
    Align = alBottom
    Caption = 'sPanel1'
    TabOrder = 1
    SkinData.SkinSection = 'PANEL'
    object DBNavigator1: TDBNavigator
      Left = 16
      Top = 20
      Width = 240
      Height = 25
      DataSource = DataSource1
      TabOrder = 0
    end
  end
  object DBGrid1: TDBGrid
    Left = 0
    Top = 0
    Width = 509
    Height = 244
    Align = alClient
    DataSource = DataSource1
    TabOrder = 2
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
  end
  object ZConnection1: TZConnection
    Protocol = 'sqlite-3'
    Database = 'D:\programing\bcb6\odesk_SerialPort_app\Serial_port_bd.sqlite'
    Left = 24
    Top = 16
  end
  object DataSource1: TDataSource
    DataSet = ZTable1
    Left = 100
    Top = 16
  end
  object ZReadOnlyQuery1: TZReadOnlyQuery
    Connection = ZConnection1
    SQL.Strings = (
      'select id, code,default_cmd from codes_tb ORDER BY code')
    Params = <>
    Left = 140
    Top = 16
  end
  object ZReadOnlyQuery2: TZReadOnlyQuery
    Connection = ZConnection1
    Params = <>
    Left = 172
    Top = 16
  end
  object ZTable1: TZTable
    Connection = ZConnection1
    TableName = 'codes_tb'
    Left = 60
    Top = 16
  end
  object DataSource2: TDataSource
    DataSet = ZReadOnlyQuery1
    Left = 100
    Top = 48
  end
end
