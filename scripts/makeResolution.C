#include <iostream>
#include <TFile.h>
#include <TH1F.h>
<<<<<<< HEAD
#include <TH2F.h>
=======
#include <TH2F.H>
>>>>>>> a708634f165e24fc6dd682ed99ca24540f8c9800
#include <TString.h>
#include <TCanvas.h>
#include <TLatex.h>

<<<<<<< HEAD
#define FNAME "timeres"

//Axis
#define axisTitleSize 0.06
#define axisTitleOffset .8

#define axisTitleSizeRatioX   0.18
#define axisLabelSizeRatioX   0.12
#define axisTitleOffsetRatioX 0.84

#define axisTitleSizeRatioY   0.15
#define axisLabelSizeRatioY   0.108
#define axisTitleOffsetRatioY 0.32

//Margins
#define leftMargin    0.12
#define rightMargin   0.05
#define topMargin     0.07
#define bottomMargin  0.12
#define rightMargin2D 0.15

//TLatex
#define font     42
#define fontSize 0.06
#define text_x   0.935
#define text_y   0.855
#define text_x2D 0.835

void TimeResolution( TString _rootFileName = "", bool _isTime = true, TString outName = "default" ) {
    
    TFile *f = new TFile( _rootFileName );
    if( f->IsOpen() ) {
        std::cout << "[INFO]: opening file: " << _rootFileName << std::endl;
    }
    else {
        std::cerr << "[ERROR]: could not open file: " << _rootFileName << std::endl;
        std::cerr << "[ERROR]: exiting!" << std::endl;
        return;
    }
    TCanvas *c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
    //------------------------------------------
    // s e t  c a n v a s   p a r a m e t e r s
    //------------------------------------------
    c->SetHighLightColor(2);
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetLeftMargin(leftMargin);
    c->SetRightMargin(rightMargin);
    c->SetTopMargin(topMargin);
    c->SetBottomMargin(bottomMargin);
    c->SetFrameBorderMode(0);
    c->SetFrameBorderMode(0);

    //-------------------------------------------
    // g e t t i n g   c o r r e c t   h i s t o
    //-------------------------------------------
    TH1F *h = (TH1F*) f->Get("Dt_Int_Weight");
    // TH1F* h = (TH1F*)f->Get("Dt_HI_Int");
    // TH1F* h = (TH1F*)f->Get(Form("Dt_%d%d", ));
    // TH1F* h = (TH1F*)f->Get(Form("DtI_%d%d", ));

    int nEntries = h->GetEntries();

    //-----------------------------------------
    // s e t   h i s t o   p a r a m e t e r s
    //-----------------------------------------
    h->SetStats(0);
    h->SetTitle("");
    h->GetYaxis()->SetTitle(Form("Entries / %2.2f ns", h->GetBinWidth(1)));
    h->GetXaxis()->SetTitle("#Deltat [ns]");
    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->Draw();

    //---------------------------
    // e x t r a   t e x t 
    //---------------------------
    TString extraText = Form("#sigma = %d ps", (int) (1000*h->GetFunction(FNAME)->GetParameter(2)));
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);
    latex.SetTextFont(font);
    latex.SetTextAlign(31);
    latex.SetTextSize(fontSize);
    latex.DrawLatex(text_x, text_y, extraText);

    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    c->SaveAs( outName + ".C" );
    return;
}

void MaximumDist( TString _rootFileName = "", TString outName = "default" ) {
    TCanvas *c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
    //------------------------------------------
    // s e t  c a n v a s   p a r a m e t e r s
    //------------------------------------------
    c->SetHighLightColor(2);
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetLeftMargin(leftMargin);
    c->SetRightMargin(rightMargin2D);
    c->SetTopMargin(topMargin);
    c->SetBottomMargin(bottomMargin);
    c->SetFrameBorderMode(0);
    c->SetFrameBorderMode(0);
    
    TFile *f = new TFile( _rootFileName );
    if( f->IsOpen() ) {
        std::cout << "[INFO]: opening file: " << _rootFileName << std::endl;
    }
    else {
        std::cerr << "[ERROR]: could not open file: " << _rootFileName << std::endl;
        std::cerr << "[ERROR]: exiting!" << std::endl;
        return;
    }

    //-------------------------------------------
    // g e t t i n g   c o r r e c t   h i s t o
    //-------------------------------------------
    TH2F *h = (TH2F *) f->Get("Int_Center");

    int nEntries = h->GetEntries();

    //-----------------------------------------
    // s e t   h i s t o   p a r a m e t e r s
    //-----------------------------------------
    h->SetStats(0);
    h->SetTitle("");
    h->GetYaxis()->SetTitle("Y Axis [mm]");
    h->GetXaxis()->SetTitle("X Axis [mm]");
    h->GetZaxis()->SetTitle(Form("Entries / %2.2f mm #times %2.2f mm",
            h->GetXaxis()->GetBinWidth(1), h->GetYaxis()->GetBinWidth(1)));

    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->GetZaxis()->SetTitleSize(axisTitleSize);
    h->GetZaxis()->SetTitleOffset(axisTitleOffset);
    h->Draw("colz");

    //---------------------------
    // e x t r a   t e x t 
    //---------------------------
    TString extraText = Form("Entries: %d", (int) h->GetEntries());
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);
    latex.SetTextFont(font);
    latex.SetTextAlign(31);
    latex.SetTextSize(fontSize);
    latex.DrawLatex(text_x2D, text_y, extraText);

    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    c->SaveAs( outName + ".C" );
    return;
}

void MakeProjection(TString outName = "default") {

    TFile *_file0 = TFile::Open("outputs/run30.root");
    TFile *_file1 = TFile::Open("outputs/run32.root");
    TFile *_file2 = TFile::Open("outputs/run27.root");
    if (!_file0->IsOpen() || !_file1->IsOpen() || !_file2->IsOpen())
        std::cerr << "[ERROR]: could not open one file" << std::endl;

    TH2F *h0 = (TH2F *) _file0->Get("Int_Center");
    TH2F *h1 = (TH2F *) _file1->Get("Int_Center");
    TH2F *h2 = (TH2F *) _file2->Get("Int_Center");
    //---------------------------
    // Y - P r o j e c t i o n
    //---------------------------
    TH1F *hp0 = (TH1F *) h0->ProjectionY("dwn");
    TH1F *hp1 = (TH1F *) h1->ProjectionY("mid");
    TH1F *hp2 = (TH1F *) h2->ProjectionY("up");

    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);

    //-----------------------------------------
    // s e t   h i s t o   p a r a m e t e r s
    //-----------------------------------------

    hp0->SetLineColor(kBlue);
    hp1->SetLineColor(kBlack);
    hp2->SetLineColor(kViolet);
    hp0->SetLineWidth(2);
    hp1->SetLineWidth(2);
    hp2->SetLineWidth(2);
    hp1->GetXaxis()->SetRangeUser(3,15);

    hp1->GetYaxis()->SetTitle(Form("Entries / %2.2f ns", hp1->GetBinWidth(1)));
    hp1->GetXaxis()->SetTitle("Y Axis [mm]");
    hp1->GetXaxis()->SetTitleSize(axisTitleSize);
    hp1->GetXaxis()->SetTitleOffset(axisTitleOffset);
    hp1->GetYaxis()->SetTitleSize(axisTitleSize);
    hp1->GetYaxis()->SetTitleOffset(axisTitleOffset);

    //------------------------------------------
    // s e t  c a n v a s   p a r a m e t e r s
    //------------------------------------------
    TCanvas *c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
    c->SetHighLightColor(2);
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetLeftMargin(leftMargin);
    c->SetRightMargin(rightMargin);
    c->SetTopMargin(topMargin);
    c->SetBottomMargin(bottomMargin);
    c->SetFrameBorderMode(0);
    c->SetFrameBorderMode(0);

    //---------------------------
    // d r a w
    //---------------------------
    hp1->Draw();
    gPad->Update();
    hp0->Draw("same");
    gPad->Update();
    hp2->Draw("same");
    gPad->Update();

    //---------------------------
    // a r r o w s 
    //---------------------------
    TArrow *ar1 = new TArrow(7.77052, 90, 9.63089, 90, 0.05, "<>");
    TArrow *ar2 = new TArrow(6.29648, 90, 7.77052, 90, 0.05, "<>");
    ar1->SetAngle(25);
    ar2->SetAngle(25);
    ar1->SetLineWidth(2);
    ar2->SetLineWidth(2);
    ar1->SetLineColor(kRed);
    ar2->SetLineColor(kRed);
    ar1->Draw();
    ar2->Draw();
    //---------------------------
    // e x t r a   t e x t 
    //---------------------------
    TLatex *tex1 = new TLatex();
    TLatex *tex2 = new TLatex();
    TLatex *tex3 = new TLatex();
    tex1->SetTextColor(kBlack);  tex2->SetTextColor(kBlack);  tex3->SetTextColor(kBlack);
    tex1->SetTextFont(font);     tex2->SetTextFont(font);     tex3->SetTextFont(font);
    tex1->SetTextSize(0.045);    tex2->SetTextSize(0.045);   tex3->SetTextSize(0.045);

    tex3->DrawLatexNDC(.62, .86, "Beam displacement:");
    tex2->DrawLatexNDC(.62, .78, "#splitline{Measured: 1.47 mm}{Known:      2.00 mm}");
    tex1->DrawLatexNDC(.62, .68, "#splitline{Measured: 1.86 mm}{Known:      2.00 mm}");

    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    c->SaveAs( outName + ".C" );
    return;
}

void ExIntegral(TString outName = "default") {
    TH2F *h = new TH2F("IntegralVsPixel","; X Axis; Y Axis",\
                                          3, 0, 3, 3, 0, 3);
    for (int i = 1; i <= 3; i++)
        h->GetYaxis()->SetBinLabel(i, "");
    for (int i = 1; i <= 3; i++)
        h->GetXaxis()->SetBinLabel(i, "");

    // gStyle->SetPalette(57);
    // gStyle->SetPalette(63);
    // gStyle->SetPalette(70);
    // gStyle->SetPalette(100);

    h->SetStats(0);
    h->SetTitle("");
    h->GetYaxis()->SetTitle("Y Axis");
    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetXaxis()->SetTitle("X Axis");
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->GetZaxis()->SetTitle("Integral [psV]");
    h->GetZaxis()->SetTitleSize(axisTitleSize);
    h->GetZaxis()->SetTitleOffset(axisTitleOffset);
    h->SetMarkerSize(2.0);

    h->SetBinContent(1, 3, 43.8);
    h->SetBinContent(2, 3, 130.);
    h->SetBinContent(3, 3, 44.3);
    h->SetBinContent(1, 2, 1.15);
    h->SetBinContent(2, 2, 242.);
    h->SetBinContent(3, 2, 61.4);
    h->SetBinContent(1, 1, 42.4);
    h->SetBinContent(2, 1, 71.0);
    h->SetBinContent(3, 1, 31.9);

    //------------------------------------------
    // s e t  c a n v a s   p a r a m e t e r s
    //------------------------------------------
    TCanvas *c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
    c->SetHighLightColor(2);
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetLeftMargin(leftMargin);
    c->SetRightMargin(rightMargin2D);
    c->SetTopMargin(topMargin);
    c->SetBottomMargin(bottomMargin);
    c->SetFrameBorderMode(0);
    c->SetFrameBorderMode(0);

    h->Draw("colztext");

    c->SaveAs( outName + ".pdf" );
    c->SaveAs( outName + ".C" );
    return;
}
=======
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
>>>>>>> a708634f165e24fc6dd682ed99ca24540f8c9800
