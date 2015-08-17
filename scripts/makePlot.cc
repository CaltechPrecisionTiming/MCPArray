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

/*
 * Iterate over root files containing histograms of time resolution.
 * Grab TR value and uncertainty in fit and plot. 
 * For example, run: ./makePlot graph.pdf datadir/t1065_run_*.root
 * Use wildcard * to include all files in argv list
 */
int main (int argc, char **argv) {
    if (argc < 3)
        cout << "usage: makePlot outfilename {infiles.root}" << endl;

    int nentries = argc - 2;
    float *run    = new float[nentries];
    float *unctX  = new float[nentries];

    float *tresW  = new float[nentries];
    float *unctyW = new float[nentries];
    
    float *tresH  = new float[nentries];
    float *unctyH = new float[nentries];

    float *tresA  = new float[nentries];
    float *unctyA = new float[nentries];

    // Loop over all files and fit to their histograms
    for (int i = 2; i < argc; i++) {

        cout << "Reading " << argv[i] << endl;
        TFile *infile = TFile::Open(argv[i], "READ");
        TH1F *weight = (TH1F *) infile->Get("Dt_Int_Weight");
        TH1F *high = (TH1F *) infile->Get("Dt_HI_Int");
        TH1F *average = (TH1F *) infile->Get("Dt_Average");

        cout <<
        (run[i-2] = i-1) << " " <<
        (tresW[i-2] = 1000 * weight->GetFunction(FNNAME)->GetParameter(2)) << " " <<
        (unctyW[i-2] = 1000 * weight->GetFunction(FNNAME)->GetParError(2)) << " " <<
        (tresH[i-2] = 1000 * high->GetFunction(FNNAME)->GetParameter(2)) << " " <<
        (unctyH[i-2] = 1000 * high->GetFunction(FNNAME)->GetParError(2)) << " " <<
        (tresA[i-2] = 1000 * average->GetFunction(FNNAME)->GetParameter(2)) << " " <<
        (unctyA[i-2] = 1000 * average->GetFunction(FNNAME)->GetParError(2)) << endl;
        unctX[i-2] = 0.0;
        
        infile->Close();
        delete infile;
    }

    TGraphErrors *g1 = new TGraphErrors(nentries, run, tresW, unctX, unctyW);
    TGraphErrors *g2 = new TGraphErrors(nentries, run, tresH, unctX, unctyH);
    TGraphErrors *g3 = new TGraphErrors(nentries, run, tresA, unctX, unctyA);

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
    g3->SetLineColor(kGreen+2);
    g3->SetMarkerColor(kGreen+2);

    g1->SetMarkerStyle(kFullCircle);
    g2->SetMarkerStyle(kFullSquare);
    g3->SetMarkerStyle(kFullTriangleDown);
    g1->SetMarkerSize(1.5);
    g2->SetMarkerSize(1.5);
    g3->SetMarkerSize(1.5);
    g1->SetLineWidth(2.);
    g2->SetLineWidth(2.);
    g3->SetLineWidth(2.);
    
    TLegend *leg = new TLegend(0.25, 0.2, 0.75, 0.4, NULL, "brNDC");
    leg->SetLineColor(kWhite);
    leg->SetFillColor(kWhite);
    leg->SetTextSize(0.04);
    leg->AddEntry(g1, "Weighted Energy", "lep");
    leg->AddEntry(g2, "Highest Energy", "lep");
    leg->AddEntry(g3, "Unweighted Average", "lep");

    g2->Draw("AP");
    g1->Draw("P same");
    g3->Draw("P same");
    leg->Draw();

    c->SaveAs(argv[1]);

    delete run, tresW, tresH, tresA, unctX, unctyW, unctyH, unctyA, g1, g2, c, leg;
}
