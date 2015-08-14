#include <iostream>
#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TMath.h"
#include "TLatex.h"

// #define NOOUTPUT

#define NSAMPLES 1024
#define NROWS 4
#define NCOLS 4
#define PIXLEN 6 /*mm*/

#define CHERENKOVy 3
#define CHERENKOVx 3
//#define REFERENCEy 3
#define REFERENCEx 0

#define STARTx 1
#define STARTy 0
#define ENDx 4
#define ENDy 3

#define MAXPIXELS 8

/*
 * Find the center of the beam from a corner point
 */
void center(const float (&arr)[NROWS][NCOLS], float (&result)[2],
            const unsigned int (&quality)[NROWS][NCOLS]) {
    float cx = 0., cy = 0., tot = 0.;
    for (int y = STARTy; y < ENDy; y++)
        for (int x = STARTx; x < ENDx; x++)
            if (!(quality[y][x] & 0b0010000)) {
                cx += (0.5 + x - STARTx) * PIXLEN * arr[y][x];
                cy += (0.5 + y - STARTy) * PIXLEN * arr[y][x];
                tot += arr[y][x];
            }
    cx /= tot;
    cy /= tot;
    result[0] = cx;
    result[1] = cy;
}

/*
 * Recursive quicksort algorithm to sort two arrays in descending order of
 * the "weight" array
 */
void sort(float *weight, float *values,
          int llim = 0, int rlim = NROWS * NCOLS - 1) {
    if (rlim <= llim) return;
    
    float pivot = weight[(rlim + llim) / 2];
    int right = rlim;
    int left = llim;

    while (left <= right) {
        while (weight[left] > pivot)
            left++;
        while (weight[right] < pivot)
            right--;
        if (left <= right) {
            if (left != right) {
                float tmp = weight[right];
                weight[right] = weight[left];
                weight[left] = tmp;
                
                tmp = values[right];
                values[right] = values[left];
                values[left] = tmp;
            }
            left++;
            right--;
        }
    }
    sort(weight, values, llim, right);
    sort(weight, values, left, rlim);
}

int main (int argc, char **argv) {
    // Read parameters and open input file
    TFile *inputfile;
    if (argc == 3) {
            inputfile = new TFile(argv[1], "READ");
            std::cout << ">> Opening file " << argv[1] << std::endl;
            // terminate if the file can't be opened
            if (!inputfile->IsOpen()) {
                std::cerr << "File open error:" << argv[1] << std::endl;
                return 0;
            }
    }
    // terminate if arguments are not correct
    else {
        std::cout << "usage: weighting inputfile.root outputname" << std::endl;
        return 0;
    }
    std::string outputname;
    if (strncmp(argv[2], "same", 5) == 0) {
        std::string input(argv[1]);
        int pi = input.rfind("/") + 1;
        int pf = input.find("_anal.root");
        outputname = input.substr(pi, pf-pi) + ".root";
    }
    else
        outputname = argv[2];
    int pf = outputname.find(".root");
    std::string outprefix = outputname.substr(0, pf);


    // Get the tree from input file
    TTree *tree = (TTree *) inputfile->Get("tree");

    // Get the variables from the tree's ntuple
    float time_gausfit[NROWS][NCOLS];
    float integral[NROWS][NCOLS];
    unsigned int QualityBit[NROWS][NCOLS];

    float deltat[NROWS][NCOLS];

    tree->SetBranchAddress("tgausroot", &time_gausfit);
    tree->SetBranchAddress("Int", &integral);
    tree->SetBranchAddress("QualityBit", &QualityBit);

    // Weighted Delta t
    TH1F *DtWI[MAXPIXELS];
    for (int a = 0; a < MAXPIXELS; a++)
        DtWI[a] = new TH1F(("Dt_Int_Weight" + std::to_string(a)).c_str(),
                              "; #Delta t [ns]; Number of Events", 45, -4.5, -3);


    /* Lenient Configuration - event considered as long as no pulse channel saturates */
    Long64_t nentries = tree->GetEntries();
    for (Long64_t iEntry = 0; iEntry < nentries; iEntry++) {
        tree->GetEntry(iEntry);

        // if not a good event, skip
        bool skip = false;
        for (int a = 0; a < NROWS; a++)
            for (int b = 0; b < NCOLS; b++)
                if ( ((a == CHERENKOVy && b == CHERENKOVx) && QualityBit[a][b] & 0b0111001) ||
                    QualityBit[a][b] & 0b1000000)
                    skip = true;
        if (skip)
            continue;
    
        bool eventful = false;
        for (int a = STARTy; a < ENDy; a++)
            for (int b = STARTx; b < ENDx; b++) {
                // If the pulse and reference pulse are good, save everything
                if (!(QualityBit[a][b] || QualityBit[a][REFERENCEx])) {
                    deltat[a][b] = time_gausfit[a][b] - time_gausfit[a][REFERENCEx];
                    eventful = true;
                }
                // Else if not a good pulse set weight to 0 to avoid using it
                else
                    integral[a][b] = 0;
            }

        // Find center point (to omit noisy pixel)
        float coords[2] = {0, 0};
        center(integral, coords, QualityBit);

        if (eventful && (coords[0] != 9. && coords[1] != 15.)) {
            // Sort delta T array by integral weighting
            float *int1D = (float *) &integral, *dt1D = (float *) &deltat;
            sort(int1D, dt1D);
            // Fill weighted Delta t measurements, do calculation for n=[1,MAXPIXELS] pixels
            float sumI = 0, tot_dtI = 0;
            for (int pixels = 0; pixels < MAXPIXELS; pixels++) {
                sumI += int1D[pixels];
                tot_dtI += int1D[pixels] * dt1D[pixels];
                DtWI[pixels]->Fill(tot_dtI / sumI);
            }
        }

    }

    /****************************** SAVE RESULTS ******************************/

    // Gaussian for finding time resolution
    TF1 *gaussian = new TF1("timeres", "gaus", -4., -3.5);

    // Save results in root file
    TFile *out = new TFile(outputname.c_str(), "RECREATE");

    float sigmat[MAXPIXELS];
    float num[MAXPIXELS];
    float errors[MAXPIXELS];

    // Fit weighted and high intensity Delta t
    for (int a = 0; a < MAXPIXELS; a++) {
        gaussian->SetParameters(DtWI[a]->GetEntries()/3, DtWI[a]->GetMean(), DtWI[a]->GetRMS());
        DtWI[a]->Fit(gaussian, "LMQR");
        DtWI[a]->Write();
        sigmat[a] = gaussian->GetParameter(2);
        errors[a] = gaussian->GetParError(2);
        num[a] = a + 1;
        // Set smaller range
        if (a == 1)
            gaussian->SetRange(-3.9, -3.6);
    }

    // Make time resolution by number of pixels graph and fit it
    // with an inverse root function
    TH1F *graph = new TH1F("Graph", ";Number of Pixels (N); Time Resolution [ps]", MAXPIXELS, 0.5, MAXPIXELS+.5);
    for (int a = 0; a < MAXPIXELS; a++) {
        graph->SetBinContent(a+1, 1000*sigmat[a]);
        graph->SetBinError(a+1, 1000*errors[a]);
    }

    // TGraphErrors *graph = new TGraphErrors(MAXPIXELS, num, sigmat, 0, errors);
    
    // TF1 *invroot = new TF1("invroot", "[0]/TMath::Sqrt(x-[1])+[2]", 1, MAXPIXELS);
    TF1 *invroot = new TF1("invroot", "[0]/TMath::Sqrt(x)+[1]", 1, MAXPIXELS);
    invroot->SetParLimits(1, 0, 100);
    // TF1 *power = new TF1("power", "[0]*TMath::Power(x, [1])+[2]", 1, MAXPIXELS);

    graph->Fit(invroot, "MNQR");
    // graph->Fit(power, "MNQR");

    graph->Write();
    invroot->Write();
    // power->Write();

// Omit Saving with NOOUTPUT FLAG
#ifndef NOOUTPUT

    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    // Set up canvas
    // TCanvas *c = new TCanvas("c", "c", 800, 600);
    TCanvas *c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
    c->SetTopMargin(0.06);
    c->SetBottomMargin(0.12);

    graph->GetXaxis()->SetTitleSize(.06);
    graph->GetXaxis()->SetTitleOffset(.8);
    graph->GetYaxis()->SetTitleSize(.06);
    graph->GetYaxis()->SetTitleOffset(.75);
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.);
    graph->SetLineWidth(2.);
    graph->SetLineColor(kBlack);

    graph->Draw();
    // graph2->Draw("APE*");
    
    invroot->SetLineColor(kRed);
    // power->SetLineColor(kGreen);
    invroot->Draw("same");
    // power->Draw("same");

    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.06);

    tex->DrawLatex(.35, .65,
        Form("#frac{(%2.1f#pm%2.1f)}{#sqrt{N}} + (%2.1f#pm%2.1f)", invroot->GetParameter(0),
             invroot->GetParError(0), invroot->GetParameter(1), invroot->GetParError(1)
             ));
    tex->DrawLatex(.35, .80, Form("#chi^{2}/2:  %2.2f", invroot->GetChisquare()));

    c->Update();

    c->SaveAs((outprefix + "_Dt_IWP.pdf").c_str());
    // c->SaveAs((outprefix + "_Dt_IWP.png").c_str());

    delete c, tex;

#endif

    delete gaussian, invroot;
    for (int a = 0; a < MAXPIXELS; a++)
        delete DtWI[a];
    delete graph;

    out->Close();
    inputfile->Close();
    delete out, inputfile;
}
