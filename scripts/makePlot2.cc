#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

using namespace std;
#define axisTitleSize 0.06
#define axisTitleOffset .8

#define leftMargin    0.12
#define rightMargin   0.05
#define topMargin     0.07
#define bottomMargin  0.12
#define FNNAME "timeres"
#define HNAME "Dt_Int_Weight"

/*
 * Iterate over root files containing histograms of time resolution.
 * Grab TR value and uncertainty in fit and plot. 
 * For example, run: ./makePlot graph.pdf datadir/t1065_run_*.root
 * Use wildcard * to include all files in argv list
 */
int main (int argc, char **argv) {
    if (argc < 3)
        cout << "usage: get_properties outfilename {infiles.root}" << endl;

    int nentries = (argc - 2) / 2;
    float *run    = new float[nentries];
    float *tresC  = new float[nentries]; // Calibrated by 1 run
    float *tresS  = new float[nentries]; // Self Calibrated
    float *unctyS = new float[nentries];
    float *unctX  = new float[nentries];
    float *unctyC = new float[nentries];

    // Loop over all files and fit to their histograms
    int lim = nentries + 2;
    for (int i = 2; i < lim; i++) {

        cout << "Reading " << argv[i] << endl;
        TFile *infile = TFile::Open(argv[i], "READ");
        TH1F *hist = (TH1F *) infile->Get(HNAME);

        cout <<
        (run[i-2] = i-1) << " " <<
        (tresC[i-2] = 1000 * hist->GetFunction(FNNAME)->GetParameter(2)) << " " <<
        (unctyC[i-2] = 1000 * hist->GetFunction(FNNAME)->GetParError(2)) << endl;
        unctX[i-2] = 0.0;
        
        infile->Close();
        delete infile, hist;
    }
    for (int i = lim; i < argc; i++) {

        cout << "Reading " << argv[i] << endl;
        TFile *infile = TFile::Open(argv[i], "READ");
        TH1F *hist = (TH1F *) infile->Get(HNAME);

        cout <<
        (i-1-lim) << " " <<
        (tresS[i-lim] = 1000 * hist->GetFunction(FNNAME)->GetParameter(2)) << " " <<
        (unctyS[i-lim] = 1000 * hist->GetFunction(FNNAME)->GetParError(2)) << endl;
        
        infile->Close();
        delete infile, hist;
    }

    TGraphErrors *g1 = new TGraphErrors(nentries, run, tresS, unctX, unctyS);
    TGraphErrors *g2 = new TGraphErrors(nentries, run, tresC, unctX, unctyC);

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

    g2->SetTitle("");
    g2->SetMinimum(0);
    g2->GetXaxis()->SetTitle("Run Number");
    g2->GetYaxis()->SetTitle("Time Resolution [ps]");
    g2->GetXaxis()->SetTitleSize(axisTitleSize);
    g2->GetXaxis()->SetTitleOffset(axisTitleOffset);
    g2->GetYaxis()->SetTitleSize(axisTitleSize);
    g2->GetYaxis()->SetTitleOffset(axisTitleOffset);

    g2->SetLineColor(kOrange-3);
    g2->SetMarkerColor(kOrange-3);
    g1->SetLineColor(kAzure);
    g1->SetMarkerColor(kAzure);
    g2->SetMarkerStyle(kFullSquare);
    g1->SetMarkerStyle(kFullCircle);
    g2->SetMarkerSize(1.5);
    g1->SetMarkerSize(1.5);
    g1->SetLineWidth(2.);
    g2->SetLineWidth(2.);
    
    TLegend *leg = new TLegend(0.25, 0.2, 0.75, 0.4, NULL, "brNDC");
    leg->SetLineColor(kWhite);
    leg->SetFillColor(kWhite);
    leg->SetTextSize(0.04);
    leg->AddEntry(g1, "Calibrated", "lep");
    leg->AddEntry(g2, "Self-Calibrated", "lep");

    g2->Draw("AP");
    g1->Draw("P same");
    leg->Draw();

    c->SaveAs(argv[1]);

    delete run, tresS, tresC, unctX, unctyS, unctyC, g1, g2, c, leg;
}


/*
c23-21.root c23-22.root c23-23.root c23-24.root c23-25.root c23-26.root c23-27.root c23-28.root c23-29.root c23-30.root c23-31.root c23-32.root c23-33.root c23-34.root c23-35.root c23-36.root c23-37.root c23-38.root c23-39.root c23-40.root c23-41.root c23-42.root c21-21.root c22-22.root c23-23.root c24-24.root c25-25.root c26-26.root c27-27.root c28-28.root c29-29.root c30-30.root c31-31.root c32-32.root c33-33.root c34-34.root c35-35.root c36-36.root c37-37.root c38-38.root c39-39.root c40-40.root c41-41.root c42-42.root
*/
