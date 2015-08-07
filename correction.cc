#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"

#define NOOUTPUT

#define NROWS 4
#define NCOLS 4
#define PIXLEN 6

#define CHERENKOVy 3
#define CHERENKOVx 3

#define STARTx 1
#define STARTy 0
#define ENDx 4
#define ENDy 3

#define NBINS 25
#define AMPMAX 0.5 /*V*/
#define INTMAX 500 /*psV*/

/*
 * This application takes an analyzed root file with and uses the values
 * to do a self correction of the time resolution.
 * Outputs graphs: corrected time resolution for individual pixels,
 * a distribution of time vs amp and int for each pixel, both original and corrected,
 * and a weighted time distribution using the corrected time stamps.
 * Also a root file containing these plots.
 */


int findi(float intensity, float max) {
    for (int i = 0; i < NBINS; i++) {
        float value1 = (i * max) / NBINS;
        float value2 = ((i+1) * max) / NBINS;
        if (value1 <= intensity && intensity < value2)
            return i+1;
    }
}

int main (int argc, char **argv) {
    // Read parameters and open input file
    TFile *inputfile;
    if (argc == 3) {
            inputfile = TFile::Open(argv[1], "READ");
            std::cout << ">> Opening file " << argv[1] << std::endl;
            // terminate if the file can't be opened
            if (!inputfile->IsOpen()) {
                std::cerr << "File open error:" << argv[1] << std::endl;
                return 0;
            }
    }
    // terminate if arguments are not correct
    else {
        std::cout << "usage: correction inputfile.root outputname.root" << std::endl;
        return 0;
    }

    std::string outputname(argv[2]);
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

    // Create histograms for amplitude/integral vs time stamp
    TFile *outputfile = TFile::Open(argv[2], "RECREATE");
    TH2F *amps[NROWS][NCOLS];
    TH2F *ints[NROWS][NCOLS];
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            amps[a][b] = new TH2F(("t_amp_" + std::to_string(a) + std::to_string(b)).c_str(),
                                   "; Amplitude [V]; #Delta t [ns]", NBINS, 0, AMPMAX, 2*NBINS, -4.5, -3);

            ints[a][b] = new TH2F(("t_int_" + std::to_string(a) + std::to_string(b)).c_str(),
                                   "; Integral [psV]; #Delta t [ns]", NBINS, 0, INTMAX, 2*NBINS, -4.5, -3);
        }

    // Weighted Delta t
    TH1F *DtWA = new TH1F("Dt_Amp_Weight", "; #Delta t [ns]; Number of Events", 45, -0.75, 0.75);
    TH1F *DtWI = new TH1F("Dt_Int_Weight", "; #Delta t [ns]; Number of Events", 45, -0.75, 0.75);

    // Individual pixel corrected Delta t
    TH1F *TresA[NROWS][NCOLS];
    TH1F *TresI[NROWS][NCOLS];
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            TresA[a][b] = new TH1F( ("DtA_" + std::to_string(a) + std::to_string(b)).c_str(),
                                    "; #Delta t [ns]; Number of Events", 30, -0.75, 0.75);
            TresI[a][b] = new TH1F( ("DtI_" + std::to_string(a) + std::to_string(b)).c_str(),
                                    "; #Delta t [ns]; Number of Events", 30, -0.75, 0.75);
    }

    // Fill 2d histogram
    long nentries = tree->GetEntries();
    for (long iEntry = 0; iEntry < nentries; iEntry++) {
        tree->GetEntry(iEntry);

        // Cherenkov filter
        if (!(QualityBit[CHERENKOVy][CHERENKOVx] & 0b0111001))
            for (int a = STARTy; a < ENDy; a++)
                // Reference filter
                if (!(QualityBit[a][0]))
                    for (int b = STARTx; b < ENDx; b++)
                        if (!(QualityBit[a][b])) {
                            float dt = time_gausfit[a][b] - time_gausfit[a][0];
                            amps[a][b]->Fill(amplitude[a][b], dt);
                            ints[a][b]->Fill(200*integral[a][b], dt);
            }
    }

    // Obtain a mean for each slice
    float tmeanA[NROWS][NCOLS][NBINS];
    float tmeanI[NROWS][NCOLS][NBINS];
    TF1 *gaussian = new TF1("gauss", "[0] * TMath::Gaus(x, [1], [2])", -4, -3.5);
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++)
            for (int i = 0; i < NBINS; i++) {
                TH1D *sliceA = amps[a][b]->ProjectionY((std::string(amps[a][b]->GetName()) + "_py").c_str(),
                                                      i, i );
                TH1D *sliceI = ints[a][b]->ProjectionY((std::string(ints[a][b]->GetName()) + "_py").c_str(),
                                                      i, i );
                // tmeanA[a][b][i] = sliceA->GetMean();
                // tmeanI[a][b][i] = sliceI->GetMean();

                gaussian->SetParameters(sliceA->GetEntries()/3, sliceA->GetMean(), sliceA->GetRMS());
                sliceA->Fit(gaussian, "LMNQR");
                tmeanA[a][b][i] = gaussian->GetParameter(1);
                gaussian->SetParameters(sliceI->GetEntries()/3, sliceI->GetMean(), sliceI->GetRMS());
                sliceI->Fit(gaussian, "LMNQR");
                tmeanI[a][b][i] = gaussian->GetParameter(1);
                delete sliceA, sliceI;
            }
    delete gaussian;

    // Get a new set of histograms
    TH2F *amps2[NROWS][NCOLS];
    TH2F *ints2[NROWS][NCOLS];
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            amps2[a][b] = new TH2F(("t_amp_corr_" + std::to_string(a) + std::to_string(b)).c_str(),
                                   "; Amplitude [V]; #Delta t [ns]", 2*NBINS, 0, AMPMAX, 2*NBINS, -0.75, 0.75);

            ints2[a][b] = new TH2F(("t_int_corr_" + std::to_string(a) + std::to_string(b)).c_str(),
                                   "; Integral [psV]; #Delta t [ns]", 2*NBINS, 0, INTMAX, 2*NBINS, -0.75, 0.75);
        }

    // Fill new histogram with corrected mean
    for (long iEntry = 0; iEntry < nentries; iEntry++) {
        tree->GetEntry(iEntry);
        // For weighte Delta t calculation
        float sumA = 0, tot_dtA = 0;
        float sumI = 0, tot_dtI = 0;

        // Cherenkov filter
        if (!(QualityBit[CHERENKOVy][CHERENKOVx] & 0b0111001))
            for (int a = STARTy; a < ENDy; a++)
                // Reference filter
                if (!(QualityBit[a][0]))
                    for (int b = STARTx; b < ENDx; b++)
                        if (!(QualityBit[a][b])) {
                            float dt = time_gausfit[a][b] - time_gausfit[a][0], dtA, dtI;
                            amps2[a][b]->Fill(amplitude[a][b], dtA = (dt - tmeanA[a][b][findi(amplitude[a][b], AMPMAX)]) );
                            ints2[a][b]->Fill(200*integral[a][b], dtI = (dt - tmeanI[a][b][findi(200*integral[a][b], INTMAX)]) );
                            // Weighted Delta t calculation
                            sumA += amplitude[a][b];
                            sumI += integral[a][b];
                            tot_dtA += amplitude[a][b] * dtA;
                            tot_dtI += integral[a][b] * dtI;
                            // Individual pixel
                            TresA[a][b]->Fill(dtA);
                            TresI[a][b]->Fill(dtI);
            }
        // Fill weighted Delta t measurement
        if (sumA) { 
            DtWA->Fill(tot_dtA / sumA);
            DtWI->Fill(tot_dtI / sumI);
        }
    }

    inputfile->Close();

#ifndef NOOUTPUT
    TCanvas *c = new TCanvas("c", "c", 800, 600);
    c->SetRightMargin(0.15);
    c->Range(-10, -1, 10, 1);

    // gStyle->SetOptFit(10);
    // gStyle->SetOptStat(11);
    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
#endif

    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            amps[a][b]->Write();
            ints[a][b]->Write();
            amps2[a][b]->Write();
            ints2[a][b]->Write();

#ifndef NOOUTPUT
            amps[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_orig-amp_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->SaveAs((outprefix + "_orig-amp_" + std::to_string(a) + std::to_string(b) + ".pdf").c_str());
            c->Clear();
            ints[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_orig-int_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->SaveAs((outprefix + "_orig-int_" + std::to_string(a) + std::to_string(b) + ".pdf").c_str());
            c->SaveAs();
            c->Clear();
            amps2[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_corr-amp_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->SaveAs((outprefix + "_corr-amp_" + std::to_string(a) + std::to_string(b) + ".pdf").c_str());
            c->Clear();
            ints2[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_corr-int_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->SaveAs((outprefix + "_corr-int_" + std::to_string(a) + std::to_string(b) + ".pdf").c_str());
            c->SaveAs();
            c->Clear();
#endif

            delete amps[a][b], ints[a][b];
    }

    // Fit and draw weighted Delta t
    gaussian = new TF1("timeres", "gaus", -0.1, 0.1);

    gaussian->SetParameters(DtWA->GetEntries()/3, DtWA->GetMean(), DtWA->GetRMS());
    DtWA->Fit(gaussian, "LMQR");
    DtWA->Write();

    gaussian->SetParameters(DtWI->GetEntries()/3, DtWI->GetMean(), DtWI->GetRMS());
    DtWI->Fit(gaussian, "LMQR");
    DtWI->Write();

#ifndef NOOUTPUT
    DtWA->Draw();
    c->SaveAs((outprefix + "_DtWA.gif").c_str());
    c->SaveAs((outprefix + "_DtWA.pdf").c_str());
    DtWI->Draw();
    c->SaveAs((outprefix + "_DtWI.gif").c_str());
    c->SaveAs((outprefix + "_DtWI.pdf").c_str());
    delete DtWA, DtWI, gaussian;
    c->Clear();
#endif

    // Individual pixel time resolution
    gaussian = new TF1("timeres", "gaus", -0.15, 0.15);
    float sigmatA[NROWS][NCOLS];
    float sigmatI[NROWS][NCOLS];
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            gaussian->SetParameters(TresA[a][b]->GetEntries()/3, TresA[a][b]->GetMean(), TresA[a][b]->GetRMS());
            TresA[a][b]->Fit(gaussian, "LMNQR");
            sigmatA[a][b] = gaussian->GetParameter(2);
            gaussian->SetParameters(TresI[a][b]->GetEntries()/3, TresI[a][b]->GetMean(), TresI[a][b]->GetRMS());
            TresI[a][b]->Fit(gaussian, "LMNQR");
            sigmatI[a][b] = gaussian->GetParameter(2);
            TresA[a][b]->Write();
            TresI[a][b]->Write();
            delete TresA[a][b], TresI[a][b];
    }

#ifndef NOOUTPUT
    TH2F *SigmaTVsPixelGraph = new TH2F("TimeResolutionVsPixel","; X Axis [mm]; Y Axis [mm]",\
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
            SigmaTVsPixelGraph->SetBinContent(b+1, a+1, sigmatA[a][b]);
    SigmaTVsPixelGraph->Draw("colztext");
    c->SaveAs((outprefix + "_st-Px-CA.gif").c_str());
    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++)
            SigmaTVsPixelGraph->SetBinContent(b+1, a+1, sigmatI[a][b]);
    SigmaTVsPixelGraph->Draw("colztext");
    c->SaveAs((outprefix + "_st-Px-CI.gif").c_str());
    delete SigmaTVsPixelGraph;
    delete c;
#endif

    delete gaussian;

    outputfile->Close();
}
