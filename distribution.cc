#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"

#define NROWS 4
#define NCOLS 4

#define CHERENKOVy 3
#define CHERENKOVx 3

#define STARTx 1
#define STARTy 0
#define ENDx 4
#define ENDy 3


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
        std::cout << "usage: pixel_analysis inputfile.root outputname" << std::endl;
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
                                   "; Amplitude [V]; #Delta t [ns]", 50, 0, .5, 50, -4.5, -3);

            ints[a][b] = new TH2F(("t_int_" + std::to_string(a) + std::to_string(b)).c_str(),
                                   "; Integral [psV]; #Delta t [ns]", 50, 0, 2.5, 50, -4.5, -3);
        }

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
                            amps[a][b]->Fill(amplitude[a][b], time_gausfit[a][b] - time_gausfit[a][0]);
                            ints[a][b]->Fill(integral[a][b], time_gausfit[a][b] - time_gausfit[a][0]);
            }

    }
    
    inputfile->Close();

    TCanvas *c = new TCanvas("c", "c", 800, 600);
    c->SetRightMargin(0.15);
    c->Range(-10, -1, 10, 1);

    for (int a = STARTy; a < ENDy; a++)
        for (int b = STARTx; b < ENDx; b++) {
            amps[a][b]->Write();
            ints[a][b]->Write();

            amps[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_amp_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->Clear();
            ints[a][b]->Draw("colztext");
            c->SaveAs((outprefix + "_int_" + std::to_string(a) + std::to_string(b) + ".gif").c_str());
            c->SaveAs();
            c->Clear();

            delete amps[a][b], ints[a][b];
    }

    outputfile->Close();
}
