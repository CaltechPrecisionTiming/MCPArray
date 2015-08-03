#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.H>
#include <TString.h>
#include <TCanvas.h>
#include <TLatex.h>

//Axis
const float axisTitleSize = 0.06;
const float axisTitleOffset = .8;

const float axisTitleSizeRatioX   = 0.18;
const float axisLabelSizeRatioX   = 0.12;
const float axisTitleOffsetRatioX = 0.84;

const float axisTitleSizeRatioY   = 0.15;
const float axisLabelSizeRatioY   = 0.108;
const float axisTitleOffsetRatioY = 0.32;

//Margins
const float leftMargin   = 0.12;
const float rightMargin  = 0.05;
const float topMargin    = 0.07;
const float bottomMargin = 0.12;

//TLatex
const int font = 42;
const float fontSize = 0.06;
const float text_x = 0.935;
const float text_y = 0.855;

void makeResolution( TString _rootFileName = "", bool _isTime = true, TString outName = "default" )
{
  TCanvas* c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
  //----------------------------------------
  //s e t  c a n v a s   p a r a m e t e r s
  //----------------------------------------
  c->SetHighLightColor(2);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetBorderSize(2);
  c->SetLeftMargin( leftMargin );
  c->SetRightMargin( rightMargin );
  c->SetTopMargin( topMargin );
  c->SetBottomMargin( bottomMargin );
  c->SetFrameBorderMode(0);
  c->SetFrameBorderMode(0);
  
  TFile* f  = new TFile( _rootFileName );
  if( f->IsOpen() )
    {
      std::cout << "[INFO]: opening file: " << _rootFileName << std::endl;
    }
  else
    {
      std::cerr << "[ERROR]: could not open file: " << _rootFileName << std::endl;
      std::cerr << "[ERROR]: exiting!" << std::endl;
      return;
    }

  //------------------------------------------
  // g e t t i n g   c o r r e c t   h i s t o
  //------------------------------------------
  TH1F* h = (TH1F*)f->Get("Dt_Int_Weight");
  int nEntries = h->Integral();

  //--------------------------------------
  //S e t   h i s t o   p a r a m e t e r s
  //--------------------------------------
  h->SetStats(0);
  h->SetTitle("");
  h->GetYaxis()->SetTitle("entries / xx ns");
  h->GetXaxis()->SetTitle("#Deltat (ns)");
  h->GetXaxis()->SetTitleSize( axisTitleSize );
  h->GetXaxis()->SetTitleOffset( axisTitleOffset );
  h->GetYaxis()->SetTitleSize( axisTitleSize );
  h->GetYaxis()->SetTitleOffset( axisTitleOffset );
  h->Draw();

  //---------------------------
  //E x t r a   t e x t 
  //----------------------------
  TString extraText = "#sigma = 79 ps";
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    
  latex.SetTextFont( font );
  latex.SetTextAlign(31); 
  latex.SetTextSize( fontSize );    
  latex.DrawLatex( text_x, text_y, extraText);

  //-----------------------------------
  //S a v e   p l o t,  a s   pdf and C
  //-----------------------------------
  c->SaveAs( outName + ".pdf" );
  c->SaveAs( outName + ".C" );
  return;
}
