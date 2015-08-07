#include <iostream>
#include <fstream>
#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"

#define NOOUTPUT

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
        std::cout << "usage: pixel_analysis inputfile.root outputname" << std::endl;
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
    float amplitude[NROWS][NCOLS];
    float integral[NROWS][NCOLS];
    unsigned int QualityBit[NROWS][NCOLS];

    tree->SetBranchAddress("tgausroot", &time_gausfit);
    tree->SetBranchAddress("Amplitude", &amplitude);
    tree->SetBranchAddress("Int", &integral);
    tree->SetBranchAddress("QualityBit", &QualityBit);

    /******************************* HISTOGRAMS *******************************/
    // only fit parameters
    //gStyle->SetOptFit(10);
    //gStyle->SetOptStat(11);
    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);

    // Make a histogram for each pixel to calculate above variables
    TH1F *Amp[NROWS][NCOLS];
    TH1F *Int[NROWS][NCOLS];
    TH1F *Tres[NROWS][NCOLS];
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            Amp[a][b] = new TH1F( ("Amp_" + std::to_string(a) + std::to_string(b)).c_str(),
                                    "; Amplitude [V]; Number of Events", 100, 0, 0.75 );
            Int[a][b] = new TH1F( ("Int_" + std::to_string(a) + std::to_string(b)).c_str(),
                                    "; Integral [psV]; Number of Events", 100, 0, 3.5 );
            Tres[a][b] = new TH1F( ("Dt_" + std::to_string(a) + std::to_string(b)).c_str(),
                                    "; #Delta t [ns]; Number of Events", 30, -4.5, -3 );
        }

    // 2D histogram for center of beam
    TH2F *AmplitudeCenter = new TH2F("Amp_Center","; X Axis [mm]; Y Axis [mm]",\
                                     (NROWS-1) * 2 * PIXLEN, 0, PIXLEN * (NROWS - 1),
                                     (NCOLS-1) * 2 * PIXLEN, 0, PIXLEN * (NCOLS - 1));
    TH2F *IntegralCenter  = new TH2F("Int_Center","; X Axis [mm]; Y Axis [mm]",\
                                     (NROWS-1) * 2 * PIXLEN, 0, PIXLEN * (NROWS - 1),
                                     (NCOLS-1) * 2 * PIXLEN, 0, PIXLEN * (NCOLS - 1));

    // Weighted Delta t
    TH1F *DtWA = new TH1F("Dt_Amp_Weight", "; #Delta t [ns]; Number of Events", 45, -4.5, -3);
    TH1F *DtWI = new TH1F("Dt_Int_Weight", "; #Delta t [ns]; Number of Events", 45, -4.5, -3);
    // Highest amplitude/integral Delta t
    TH1F *DtHA = new TH1F("Dt_HI_Amp", "; #Delta t [ns]; Number of Events", 45, -4.5, -3);
    TH1F *DtHI = new TH1F("Dt_HI_Int", "; #Delta t [ns]; Number of Events", 45, -4.5, -3);

    /**************************** LOOP OVER EVENTS ****************************/

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
    
        float sumA = 0, tot_dtA = 0;
        float sumI = 0, tot_dtI = 0;
        float highA = 0, timeA = 0;
        float highI = 0, timeI = 0;

        for (int a = STARTy; a < ENDy; a++)
            for (int b = STARTx; b < ENDx; b++) {
                // If the pulse and reference pulse are good, save everything
                if (!(QualityBit[a][b] || QualityBit[a][REFERENCEx])) {
                    float dt = time_gausfit[a][b] - time_gausfit[a][REFERENCEx];

                    Amp[a][b]->Fill(amplitude[a][b]);
                    Int[a][b]->Fill(integral[a][b]);
                    Tres[a][b]->Fill(dt);
                    // Weighted Delta t calculation
                    sumA += amplitude[a][b];
                    sumI += integral[a][b];
                    tot_dtA += amplitude[a][b] * dt;
                    tot_dtI += integral[a][b] * dt;
                    // High intensity Delta t calculation
                    if (highA < amplitude[a][b]) {highA = amplitude[a][b]; timeA = dt;}
                    if (highI < integral[a][b]) {highI = integral[a][b]; timeI = dt;}
                }
                // If there is a pulse but timing is compromised save only amp and int
                else if (!(QualityBit[a][b] & 0b0010000)) {
                    Amp[a][b]->Fill(amplitude[a][b]);
                    Int[a][b]->Fill(integral[a][b]);
                }
                // Otherwise just consider zero Amp and Int, and skip Delta t measurement
                else {
                    Amp[a][b]->Fill(0);
                    Int[a][b]->Fill(0);
                }
            }

        // Fill the beam-center-coordinate histograms
        float coords[2] = {0, 0};
        center(amplitude, coords, QualityBit);
        if (coords[0] != 9. && coords[1] != 15.)
        AmplitudeCenter->Fill(coords[0], coords[1]);
        center(integral, coords, QualityBit);
        if (coords[0] != 9. && coords[1] != 15.)
        IntegralCenter->Fill(coords[0], coords[1]);

        // Fill weighted Delta t measurement
        if (sumA) {
            DtWA->Fill(tot_dtA / sumA);
            DtWI->Fill(tot_dtI / sumI);
        }
        // Fill highest intensity Delta t measurement
        if (highA) {
            DtHA->Fill(timeA);
            DtHI->Fill(timeI);
        }
    }

    /****************************** SAVE RESULTS ******************************/

    // Arrays to store average amplitude, average integral and time resolution
    float av_amp[NROWS][NCOLS];
    float av_int[NROWS][NCOLS];
    float sigmat[NROWS][NCOLS];

    // Gaussian for finding time resolution
    TF1 *gaussian = new TF1("timeres", "gaus", -4., -3.5);

    // Save results in root file
    TFile *out = new TFile(outputname.c_str(), "RECREATE");
    AmplitudeCenter->Write();
    IntegralCenter->Write();

    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            // Average Amplitude
            av_amp[a][b] = Amp[a][b]->GetMean();
            //Amp[a][b]->Write();

            // Average Integral
            av_int[a][b] = 200 * Int[a][b]->GetMean();
            //Int[a][b]->Write();

            // Time Resolution Calculation - fit gaussian to distribution
            gaussian->SetParameters(Tres[a][b]->GetEntries()/3, Tres[a][b]->GetMean(), Tres[a][b]->GetRMS());
            Tres[a][b]->Fit(gaussian, "LMQR");
            sigmat[a][b] = gaussian->GetParameter(2);
            // sigmat[a][b] = Tres[a][b]->GetRMS();
            Tres[a][b]->Write();

            delete Amp[a][b], Int[a][b], Tres[a][b];
        }
    // Fit weighted and high intensity Delta t
    gaussian->SetParameters(DtWA->GetEntries()/3, DtWA->GetMean(), DtWA->GetRMS());
    DtWA->Fit(gaussian, "LMQR");
    DtWA->Write();
    gaussian->SetParameters(DtWI->GetEntries()/3, DtWI->GetMean(), DtWI->GetRMS());
    DtWI->Fit(gaussian, "LMQR");
    DtWI->Write();
    gaussian->SetParameters(DtHA->GetEntries()/3, DtHA->GetMean(), DtHA->GetRMS());
    DtHA->Fit(gaussian, "LMQR");
    DtHA->Write();
    gaussian->SetParameters(DtHI->GetEntries()/3, DtHI->GetMean(), DtHI->GetRMS());
    DtHI->Fit(gaussian, "LMQR");
    DtHI->Write();

    delete gaussian;

// Omit Saving with NOOUTPUT FLAG
#ifndef NOOUTPUT

    // Set up canvas
    TCanvas *c = new TCanvas("c", "c", 800, 600);
    c->SetRightMargin(0.15);
    c->Range(-10, -1, 10, 1);

    /************************************ Beam Center **********************************/
    AmplitudeCenter->Draw("colztext");
    c->SaveAs((outprefix + "_AmpCenter.gif").c_str());
    c->SaveAs((outprefix + "_AmpCenter.pdf").c_str());
    IntegralCenter->Draw("colztext");
    c->SaveAs((outprefix + "_IntCenter.gif").c_str());
    c->SaveAs((outprefix + "_IntCenter.pdf").c_str());
    delete AmplitudeCenter, IntegralCenter;
    c->Clear();

    /********************************** Weighted Time **********************************/
    DtWA->Draw();
    c->SaveAs((outprefix + "_DtWA.gif").c_str());
    c->SaveAs((outprefix + "_DtWA.pdf").c_str());
    DtWI->Draw();
    c->SaveAs((outprefix + "_DtWI.gif").c_str());
    c->SaveAs((outprefix + "_DtWI.pdf").c_str());
    delete DtWA, DtWI;
    c->Clear();

    /********************************** High Int Time **********************************/
    DtHA->Draw();
    c->SaveAs((outprefix + "_DtHA.gif").c_str());
    c->SaveAs((outprefix + "_DtHA.pdf").c_str());
    DtHI->Draw();
    c->SaveAs((outprefix + "_DtHI.gif").c_str());
    c->SaveAs((outprefix + "_DtHI.pdf").c_str());
    delete DtHA, DtHI;
    c->Clear();

    /************************************ Amplitude ************************************/
    TH2F *AmplitudeVsPixelGraph = new TH2F("AmplitudeVsPixel","; X Axis [mm]; Y Axis [mm]",\
                                           NROWS, 0, NROWS*PIXLEN, NCOLS, 0, NCOLS*PIXLEN);
    for (int i = 1; i <= NROWS; i++)
        AmplitudeVsPixelGraph->GetYaxis()->SetBinLabel(i, "");
    for (int i = 1; i <= NCOLS; i++)
        AmplitudeVsPixelGraph->GetXaxis()->SetBinLabel(i, "");

    AmplitudeVsPixelGraph->SetStats(0);
    AmplitudeVsPixelGraph->GetXaxis()->SetTitle("X Axis");
    AmplitudeVsPixelGraph->GetYaxis()->SetTitle("Y Axis");
    AmplitudeVsPixelGraph->GetYaxis()->SetTitleOffset(1.0);
    AmplitudeVsPixelGraph->GetZaxis()->SetTitle("Amplitude [V]");
    AmplitudeVsPixelGraph->GetZaxis()->SetTitleOffset(1.25);
    AmplitudeVsPixelGraph->SetMarkerSize(2.0);

    // Fill histogram
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++)
            AmplitudeVsPixelGraph->SetBinContent(b+1, a+1, av_amp[a][b]);

    AmplitudeVsPixelGraph->Draw("colztext");
    c->SaveAs((outprefix + "_Amp-Px.gif").c_str());
    c->SaveAs((outprefix + "_Amp-Px.pdf").c_str());

    delete AmplitudeVsPixelGraph;

    /*********************************** Integration ***********************************/
    TH2F *IntegralVsPixelGraph = new TH2F("IntegralVsPixel","; X Axis [mm]; Y Axis [mm]",\
                                          NROWS, 0, NROWS*PIXLEN, NCOLS, 0, NCOLS*PIXLEN);
    for (int i = 1; i <= NROWS; i++)
        IntegralVsPixelGraph->GetYaxis()->SetBinLabel(i, "");
    for (int i = 1; i <= NCOLS; i++)
        IntegralVsPixelGraph->GetXaxis()->SetBinLabel(i, "");

    IntegralVsPixelGraph->SetStats(0);
    IntegralVsPixelGraph->GetXaxis()->SetTitle("X Axis");
    IntegralVsPixelGraph->GetYaxis()->SetTitle("Y Axis");
    IntegralVsPixelGraph->GetYaxis()->SetTitleOffset(1.0);
    IntegralVsPixelGraph->GetZaxis()->SetTitle("Integral [psV]");
    IntegralVsPixelGraph->GetZaxis()->SetTitleOffset(1.25);
    IntegralVsPixelGraph->SetMarkerSize(2.0);

    // Fill histogram
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++)
            IntegralVsPixelGraph->SetBinContent(b+1, a+1, av_int[a][b]);

    IntegralVsPixelGraph->Draw("colztext");
    c->SaveAs((outprefix + "_Int-Px.gif").c_str());
    c->SaveAs((outprefix + "_Int-Px.pdf").c_str());

    delete IntegralVsPixelGraph;

    /********************************** Sigma Delta t **********************************/
    TH2F *SigmaTVsPixelGraph = new TH2F("Time ResolutionVsPixel","; X Axis [mm]; Y Axis [mm]",\
                                        NROWS, 0, NROWS*PIXLEN, NCOLS, 0, NCOLS*PIXLEN);
    for (int i = 1; i <= NROWS; i++)
        SigmaTVsPixelGraph->GetYaxis()->SetBinLabel(i, "");
    for (int i = 1; i <= NCOLS; i++)
        SigmaTVsPixelGraph->GetXaxis()->SetBinLabel(i, "");

    SigmaTVsPixelGraph->SetStats(0);
    SigmaTVsPixelGraph->GetXaxis()->SetTitle("X Axis");
    SigmaTVsPixelGraph->GetYaxis()->SetTitle("Y Axis");
    SigmaTVsPixelGraph->GetYaxis()->SetTitleOffset(1.0);
    SigmaTVsPixelGraph->GetZaxis()->SetTitle("#sigma #Delta t [ns]");
    SigmaTVsPixelGraph->GetZaxis()->SetTitleOffset(1.25);
    SigmaTVsPixelGraph->SetMarkerSize(2.0);

    // Fill histogram
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++)
            SigmaTVsPixelGraph->SetBinContent(b+1, a+1, sigmat[a][b]);

    SigmaTVsPixelGraph->Draw("colztext");
    c->SaveAs((outprefix + "_st-Px.gif").c_str());
    c->SaveAs((outprefix + "_st-Px.pdf").c_str());

    /***********************************************************************************/
    
    delete c;

#else

    delete AmplitudeCenter, IntegralCenter;
    delete DtWA, DtWI;
    delete DtHA, DtHI;

#endif

    out->Close();
    inputfile->Close();
    delete out, inputfile;
}
