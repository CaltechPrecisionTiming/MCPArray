#include <iostream>
#include <cmath>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TArrow.h>
#include <TF1.h>

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
#define rightMargin2D 0.17

//TLatex
#define font     42
#define fontSize 0.06
#define text_x   0.935
#define text_y   0.855
#define text_x2D 0.80

void TimeResolution( TString _rootFileName = "", bool weight = true, TString outName = "default" ) {
    
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
    TH1F *h;
    if (weight)
        h = (TH1F *) f->Get("Dt_Int_Weight");
    else
        h = (TH1F *)f->Get("Dt_HI_Int");
    // TH1F* h = (TH1F*)f->Get(Form("Dt_%d%d", ));
    // TH1F* h = (TH1F*)f->Get(Form("DtI_%d%d", ));

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
    // c->SaveAs( outName + ".C" );
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
    if( f->IsOpen() )
        std::cout << "[INFO]: opening file: " << _rootFileName << std::endl;
    else {
        std::cerr << "[ERROR]: could not open file: " << _rootFileName << std::endl;
        std::cerr << "[ERROR]: exiting!" << std::endl;
        return;
    }

    //-------------------------------------------
    // g e t t i n g   c o r r e c t   h i s t o
    //-------------------------------------------
    TH2F *h = (TH2F *) f->Get("Int_Center");

    //-----------------------------------------
    // s e t   h i s t o   p a r a m e t e r s
    //-----------------------------------------
    h->SetStats(0);
    h->SetTitle("");
    h->GetYaxis()->SetTitle("Y Axis [mm]");
    h->GetXaxis()->SetTitle("X Axis [mm]");
    h->GetZaxis()->SetTitle(Form("Entries / (%1.1f mm #times %1.1f mm)",
            h->GetXaxis()->GetBinWidth(1), h->GetYaxis()->GetBinWidth(1)));

    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->GetZaxis()->SetTitleSize(axisTitleSize);
    h->GetZaxis()->SetTitleOffset(axisTitleOffset);
    h->Draw("colz");
/*
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
*/
    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    // c->SaveAs( outName + ".C" );
    return;
}

void IntDt( unsigned int chnly, unsigned int chnlx, TString _rootFileName = "",
            bool correction = true, TString outName = "default" ) {
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
    if( f->IsOpen() )
        std::cout << "[INFO]: opening file: " << _rootFileName << std::endl;
    else {
        std::cerr << "[ERROR]: could not open file: " << _rootFileName << std::endl;
        std::cerr << "[ERROR]: exiting!" << std::endl;
        return;
    }

    //-------------------------------------------
    // g e t t i n g   c o r r e c t   h i s t o
    //-------------------------------------------
    TH2F *h0;
    if (correction)
        h0 = (TH2F *) f->Get(Form("t_int_corr_%u%u", chnly, chnlx));
    else
        h0 = (TH2F *) f->Get(Form("t_int_%u%u", chnly, chnlx));
    // Convert x-axis
    std::cout << "new histogram" << std::endl;
    TH2F *h = new TH2F("h", "",
        h0->GetNbinsX(), 0, 0.02 * h0->GetNbinsX() * h0->GetXaxis()->GetBinWidth(1),
        h0->GetNbinsY(), h0->GetYaxis()->GetXmin(), h0->GetYaxis()->GetXmax()
        );
    std::cout << "copying" << std::endl;
    for (int x = 1; x <= h0->GetNbinsX(); x++)
        for (int y = 1; y <= h0->GetNbinsY(); y++)
            h->SetBinContent(x, y, h0->GetBinContent(x, y));

    //-----------------------------------------
    // s e t   h i s t o   p a r a m e t e r s
    //-----------------------------------------
    h->SetStats(0);
    h->SetTitle("");
    h->GetYaxis()->SetTitle("#Deltat [ns]");
    h->GetXaxis()->SetTitle("Charge [pC]");
    h->GetZaxis()->SetTitle(Form("Entries / (%2.2f pC #times %2.2f ns)",
            h->GetXaxis()->GetBinWidth(1), h->GetYaxis()->GetBinWidth(1)));

    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->GetZaxis()->SetTitleSize(axisTitleSize);
    h->GetZaxis()->SetTitleOffset(axisTitleOffset);
    h->Draw("colz");
/*
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
*/
    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    // c->SaveAs( outName + ".C" );
    return;
}


void MakeProjection(TString outName = "default") {

    TFile *file0 = TFile::Open("outputs/run30.root");
    TFile *file1 = TFile::Open("outputs/run32.root");
    TFile *file2 = TFile::Open("outputs/run34.root");
    if (!file0->IsOpen() || !file1->IsOpen() || !file2->IsOpen()) {
        std::cerr << "[ERROR]: could not find one or more file" << std::endl;
        return;
    }

    TH2F *h0 = (TH2F *) file0->Get("Int_Center");
    TH2F *h1 = (TH2F *) file1->Get("Int_Center");
    TH2F *h2 = (TH2F *) file2->Get("Int_Center");
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

    hp0->SetLineColor(kOrange+7);
    hp0->SetFillStyle(3005);
    hp0->SetFillColor(kOrange+7);
    hp1->SetLineColor(kAzure-3);
    hp1->SetFillStyle(3005);
    hp1->SetFillColor(kAzure-3);
    hp2->SetLineColor(kViolet-2);
    hp2->SetFillStyle(3005);
    hp2->SetFillColor(kViolet-2);
    

    hp0->SetLineWidth(2);
    hp1->SetLineWidth(2);
    hp2->SetLineWidth(2);
    //hp0->GetXaxis()->SetRangeUser(3, 15);

    hp0->Scale(1./hp0->GetEntries());
    hp1->Scale(1./hp1->GetEntries());
    hp2->Scale(1./hp2->GetEntries());

    hp2->GetYaxis()->SetTitle(Form("Fraction of Entries / %1.1f mm", hp1->GetBinWidth(1)));
    hp2->GetXaxis()->SetTitle("Y Axis [mm]");
    hp2->GetXaxis()->SetTitleSize(axisTitleSize);
    hp2->GetXaxis()->SetTitleOffset(axisTitleOffset);
    hp2->GetYaxis()->SetTitleSize(axisTitleSize);
    hp2->GetYaxis()->SetTitleOffset(axisTitleOffset+.1);

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
    hp2->Draw("same");
    gPad->Update();
    hp1->Draw("same");
    gPad->Update();
    hp0->Draw("same");
    gPad->Update();

    //---------------------------
    // a r r o w s 
    //---------------------------
    TArrow *ar1 = new TArrow(7.77052, .15, 9.63089, .15, 0.05, "<>");
    TArrow *ar2 = new TArrow(6.12369, .15, 7.77052, .15, 0.05, "<>");
    ar1->SetAngle(25);
    ar2->SetAngle(25);
    ar1->SetLineWidth(2);
    ar2->SetLineWidth(2);
    ar1->SetLineColor(kGreen+1);
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
    tex1->SetTextSize(0.045);    tex2->SetTextSize(0.045);    tex3->SetTextSize(0.045);

    tex3->DrawLatexNDC(.45, .81, "Beam displacement:");
    tex2->SetTextColor(kRed);
    tex2->DrawLatexNDC(.50, .73, "#splitline{Measured: 1.65 mm}{Known:      2.00 mm}");
    tex1->SetTextColor(kGreen+1);
    tex1->DrawLatexNDC(.60, .63, "#splitline{Measured: 1.86 mm}{Known:      2.00 mm}");

    //-------------------------------------
    // s a v e   p l o t,  a s   pdf and C
    //-------------------------------------
    c->SaveAs( outName + ".pdf" );
    // c->SaveAs( outName + ".C" );

    file0->Close();
    file1->Close();
    file2->Close();
    return;
}

void ExIntegral(TString outName = "default") {
  
    //------------------------------------------
    // s e t  c a n v a s   p a r a m e t e r s
    //------------------------------------------
    TCanvas *c1 = new TCanvas( "c1", "c1", 2119, 33, 800, 700 );
    c1->SetHighLightColor(2);
    c1->SetFillColor(0);
    c1->SetBorderMode(0);
    // c1->SetBorderSize(2);
    // c1->SetLeftMargin(leftMargin);
    c1->SetRightMargin(rightMargin2D+.01);
    // c1->SetTopMargin(topMargin);
    // c1->SetBottomMargin(bottomMargin);
    c1->SetFrameBorderMode(0);
    c1->SetFrameBorderMode(0);

    TH2F *h = new TH2F("IntegralVsPixel","Mean Charge Distribution;;",\
                                          3, 0, 3, 3, 0, 3);
    for (int i = 1; i <= 3; i++)
        h->GetYaxis()->SetBinLabel(i, "");//Form("#scale[2.]{Row %d}", i));
    for (int i = 1; i <= 3; i++)
        h->GetXaxis()->SetBinLabel(i, "");//Form("#scale[2.]{Col %d}", i));

    h->SetMarkerSize(3);
    h->SetMarkerColor(kWhite);

    h->SetStats(0);
    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);
    h->GetZaxis()->SetTitle("Charge [fC]");
    h->GetZaxis()->SetTitleSize(axisTitleSize);
    h->GetZaxis()->SetTitleOffset(axisTitleOffset+.2);

    h->SetBinContent(1, 3, round(43.7555 * 20.));
    h->SetBinContent(2, 3, round(129.888 * 20.));
    h->SetBinContent(3, 3, round(44.3214 * 20.));
    h->SetBinContent(1, 2, 0);//round(1.14947 * 20.));
    h->SetBinContent(2, 2, round(241.857 * 20.));
    h->SetBinContent(3, 2, round(61.356  * 20.));
    h->SetBinContent(1, 1, round(42.402  * 20.));
    h->SetBinContent(2, 1, round(70.9921 * 20.));
    h->SetBinContent(3, 1, round(31.8931 * 20.));

    h->Draw("colztext");

    TBox *b = new TBox();
    b->SetFillColor(12);
    b->SetFillStyle(3144);
    b->DrawBox(h->GetXaxis()->GetBinCenter(1)-.5,
               h->GetXaxis()->GetBinCenter(2)-.5,
               h->GetXaxis()->GetBinCenter(1)+.5,
               h->GetXaxis()->GetBinCenter(2)+.5
               );
    
    TLatex *tex = new TLatex();
    tex->SetTextColor(kWhite);
    tex->SetTextFont(font);
    tex->SetTextSize(0.045);
    for (int a = 1; a < 4; a++)
        for (int b = 1; b < 4; b++)
            tex->DrawLatex(h->GetXaxis()->GetBinCenter(a)-.3,
                           h->GetXaxis()->GetBinCenter(b)-.4,
                           Form("#it{Pixel %d%d}", a+3, b+2));

    gPad->Update();
    c1->SaveAs( outName + ".pdf" );
    // c1->SaveAs( outName + ".C" );
    return;
}

void MakePulses(unsigned int chnly, unsigned int chnlx, unsigned int event,
            TString filename = "",  TString outName = "default") {

    TFile *f = TFile::Open(filename);
    if( f->IsOpen() )
        std::cout << "[INFO]: opening file: " << filename << std::endl;
    else {
        std::cerr << "[ERROR]: could not open file: " << filename << std::endl;
        std::cerr << "[ERROR]: exiting!" << std::endl;
        return;
    }

    TTree *tree = (TTree *) f->Get("tree");

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
    gStyle->SetOptStat(0);
    
    // Get Values
    float chnls[4][4][1024];
    tree->SetBranchAddress("chnls", &chnls);
    tree->GetEntry(event+1);

    float t[1024], v[1024];
    for (Int_t i = 0; i < 1024 ; i++) {
        t[i] = 0.2 * i;
        v[i] = -chnls[chnly][chnlx][i];
    }

    gr = new TGraph(1024, t, v);
    gr->GetXaxis()->SetRangeUser(10, 26);
    
    gr->SetMarkerStyle(20);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("time [ns]");
    gr->GetYaxis()->SetTitle("Amplitude [V]");
    gr->GetXaxis()->SetTitleSize(axisTitleSize);
    gr->GetXaxis()->SetTitleOffset(axisTitleOffset);
    gr->GetYaxis()->SetTitleSize(axisTitleSize);
    gr->GetYaxis()->SetTitleOffset(axisTitleOffset+.2);
    gr->Draw("ALP");

    c->SaveAs(outName + ".pdf");
    // c->SaveAs(outName + ".C");

    f->Close();
    delete f;
    return;
}

void ConstTerm( TString outName = "default" ) {
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

    TH1F *h = new TH1F("const_term", "", 10, -8, 32);

    h->Fill(15.3);    h->Fill(16.5);    h->Fill(12.6);    h->Fill(10.9);
    h->Fill(12.4);    h->Fill(11.6);    h->Fill( 4.6);    h->Fill(10.9);
    h->Fill( 6.7);    h->Fill( 4.4);    h->Fill( 5.4);    h->Fill(11.3);
    h->Fill( 5.0);    h->Fill( 5.5);    h->Fill( 7.3);    h->Fill( 8.7);
    h->Fill( 9.6);    h->Fill(15.4);    h->Fill(11.6);    h->Fill( 3.8);
    h->Fill( 7.5);    h->Fill( 7.7);

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(10);
    //h->SetStats(0);
    h->GetYaxis()->SetTitle(Form("Entries / %2.2f ps", h->GetBinWidth(1)));
    h->GetXaxis()->SetTitle("Fit Constant Term [ps]");
    h->GetXaxis()->SetTitleSize(axisTitleSize);
    h->GetXaxis()->SetTitleOffset(axisTitleOffset);
    h->GetYaxis()->SetTitleSize(axisTitleSize);
    h->GetYaxis()->SetTitleOffset(axisTitleOffset);

    TF1 *gaussian = new TF1("const", "gaus", 2, 18);
    h->Fit(gaussian, "LMQR");
    h->Draw();

/*
    TString extraText = Form("#sigma = %d ps", (int) (1000*h->GetFunction(FNAME)->GetParameter(2)));
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);
    latex.SetTextFont(font);
    latex.SetTextAlign(31);
    latex.SetTextSize(fontSize);
    latex.DrawLatex(text_x, text_y, extraText);
*/
    c->SaveAs( outName + ".pdf" );
    // c->SaveAs( outName + ".C" );

    return;
}
