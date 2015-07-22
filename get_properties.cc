#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TF2.h"
#include "TMath.h"

using namespace std;

#define STDSx 3.
#define STDSy 2.
#define LOWBOUNDX 3.
#define LOWBOUNDY 3.
#define UPBOUNDX 15.
#define UPBOUNDY 15.

#define PIXLEN 6 /*mm*/
#define NCOLS 3
#define NROWS 3

/*
 * Iterate over root files, fit gaussian to find mean and std.
 * Output data as csv.
 */
int main (int argc, char **argv) {
    if (argc < 3)
        cout << "usage: get_properties outfilename {infiles.root}" << endl;

    // CSV file spreadsheet containing all info
    ofstream outfile;
    // Create header for CSV
    cout << "Creating new csv file and root file" << endl;
    outfile.open((string(argv[1]) + ".csv").c_str());
    outfile << "Filename, "
               "AmpEntries, AmpMeanX, AmpSigmaX, AmpMeanY, AmpSigmaY, "
               "IntEntries, IntMeanX, IntSigmaX, IntMeanY, IntSigmaY, "
               "DMeanX, DMeanY, DSigX, DSigY" << endl;

    // Create a tree for containing all info, create root file at the end
    TTree *outtree = new TTree("tree", "tree");

    // tree ntuple variables
    unsigned int AmpEntries;
    outtree->Branch("AmpEntries", &AmpEntries, "AmpEntries/i");
    float AmpMeanX;    outtree->Branch("AmpMeanX",  &AmpMeanX,  "AmpMeanX/F");
    float AmpSigmaX;   outtree->Branch("AmpSigmaX", &AmpSigmaX, "AmpSigmaX/F");
    float AmpMeanY;    outtree->Branch("AmpMeanY",  &AmpMeanY,  "AmpMeanY/F");
    float AmpSigmaY;   outtree->Branch("AmpSigmaY", &AmpSigmaY, "AmpSigmaY/F");
    unsigned int IntEntries;
    outtree->Branch("IntEntries", &IntEntries, "IntEntries/i");
    float IntMeanX;    outtree->Branch("IntMeanX",  &IntMeanX,  "IntMeanX/F");
    float IntSigmaX;   outtree->Branch("IntSigmaX", &IntSigmaX, "IntSigmaX/F");
    float IntMeanY;    outtree->Branch("IntMeanY",  &IntMeanY,  "IntMeanY/F");
    float IntSigmaY;   outtree->Branch("IntSigmaY", &IntSigmaY, "IntSigmaY/F");

    // Loop over all files and fit to their histograms
    for (int i = 2; i < argc; i++) {

        cout << "Reading " << argv[i] << endl;
        TFile *infile = TFile::Open(argv[i], "READ");

        TH2F *Amp = (TH2F *) infile->Get("Amp_Center");
        TH2F *Int = (TH2F *) infile->Get("Int_Center");
        double xmin, xmax, ymin, ymax;

        /* Amplitude */
        // Get Histogram values and use to set bounds of fitting function
        double mAx = Amp->GetMean(1);
        double mAy = Amp->GetMean(2);
        double sAx = Amp->GetRMS(1);
        double sAy = Amp->GetRMS(2);
        xmin = (mAx - STDSx*sAx > LOWBOUNDX ? mAx - STDSx*sAx : LOWBOUNDX);
        xmax = (mAx + STDSx*sAx < UPBOUNDX ? mAx + STDSx*sAx : UPBOUNDX);
        ymin = (mAy - STDSy*sAy > LOWBOUNDY ? mAy - STDSy*sAy : LOWBOUNDY);
        ymax = (mAy + STDSy*sAy < UPBOUNDY ? mAy + STDSy*sAy : UPBOUNDY);

        TF2 *gaussianA = new TF2("gaussA", "[0] * TMath::Gaus(x, [1], [2])"
                                               "* TMath::Gaus(y, [3], [4])",
                                               xmin, xmax, ymin, ymax);

        // Initialize fit parameters and set a limit
        gaussianA->SetParameters(200, mAx, sAx, mAy, sAy);
        gaussianA->SetParLimits(1, 0, PIXLEN * NCOLS);
        gaussianA->SetParLimits(3, 0, PIXLEN * NROWS);
        gaussianA->SetParLimits(2, 0, PIXLEN);
        gaussianA->SetParLimits(4, 0, PIXLEN);
        Amp->Fit(gaussianA, "LMNQR");

        /* Integral */
        // Get Histogram values and use to set bounds of fitting function
        double sIx = Int->GetRMS(1);
        double sIy = Int->GetRMS(2);
        double mIx = Int->GetMean(1);
        double mIy = Int->GetMean(2);
        xmin = (mIx - STDSx*sIx > LOWBOUNDX ? mIx - STDSx*sIx : LOWBOUNDX);
        xmax = (mIx + STDSx*sIx < UPBOUNDX ? mIx + STDSx*sIx : UPBOUNDX);
        ymin = (mIy - STDSy*sIy > LOWBOUNDY ? mIy - STDSy*sIy : LOWBOUNDY);
        ymax = (mIy + STDSy*sIy < UPBOUNDY ? mIy + STDSy*sIy : UPBOUNDY);

        TF2 *gaussianI = new TF2("gaussI", "[0] * TMath::Gaus(x, [1], [2])"
                                               "* TMath::Gaus(y, [3], [4])",
                                               xmin, xmax, ymin, ymax);

        // Initialize fit parameters and set a limit
        gaussianI->SetParameters(200, mIx, sIx, mIy, sIy);
        gaussianI->SetParLimits(1, 0, PIXLEN * NCOLS);
        gaussianI->SetParLimits(3, 0, PIXLEN * NROWS);
        gaussianI->SetParLimits(2, 0, PIXLEN);
        gaussianI->SetParLimits(4, 0, PIXLEN);
        Int->Fit(gaussianI, "LMNQR");
        
        // Output everything to the CSV file
        std::string strname(argv[i]);
        int beg = strname.rfind("/") + 1;

        outfile << strname.substr(beg) << ", "
                << (AmpEntries = Amp->GetEntries()) << ", "
                << (AmpMeanX   = gaussianA->GetParameter(1)) << ", "
                << (AmpSigmaX  = gaussianA->GetParameter(2)) << ", "
                << (AmpMeanY   = gaussianA->GetParameter(3)) << ", "
                << (AmpSigmaY  = gaussianA->GetParameter(4)) << ", "
                << (IntEntries = Int->GetEntries()) << ", "
                << (IntMeanX   = gaussianI->GetParameter(1))  << ", "
                << (IntSigmaX  = gaussianI->GetParameter(2)) << ", "
                << (IntMeanY   = gaussianI->GetParameter(3))  << ", "
                << (IntSigmaY  = gaussianI->GetParameter(4)) << ", "
                << fabs(AmpMeanX - IntMeanX) << ", "
                << fabs(AmpMeanY - IntMeanY) << ", "
                << fabs(AmpSigmaX - IntSigmaX) << ", "
                << fabs(AmpSigmaY - IntSigmaY) << endl;

        // Output to tree
        outtree->Fill();

        delete gaussianA, gaussianI;
        delete Amp, Int;
        infile->Close();
        delete infile;
    }


    outfile.close();

    TFile *rootfile = TFile::Open((string(argv[1]) + ".root").c_str(), "RECREATE");
    outtree->Write();
    rootfile->Close();
    delete outtree;
}