#include <TFile.h>
#include <TTree.h>
#include <iostream>

#define NSAMPLES 1024

/*
 * TODO : Get it to work with any number of channels
 */

int main (int argc, char **argv) {
    if (argc != 2)
        std::cout << "usage: pulse_convert filename.root" << std::endl
                  << "where file has tree named 'pulse'" << std::endl;

    TFile *infile = TFile::Open(argv[1]);
    if (!infile) {
        std::cout << "File not found: " << argv[1] << std::endl;
        return 0;
    }
    TTree *intree = (TTree *) infile->Get("pulse");
    if (!intree) {
        std::cout << "Error: could not find tree \"pulse\"" << std::endl;
        return 0;
    }

    Int_t event, tc1, tc2, tc3, tc4;
    float b1_t[1024], b1_c[4096], b2_t[1024], b2_c[4096], b3_t[1024], b3_c[4096], b4_t[1024], b4_c[4096];

    float *b1_c1 = b1_c;
    float *b1_c2 = b1_c + 1024;
    float *b1_c3 = b1_c + 2048;
    float *b1_c4 = b1_c + 3072;
    float *b2_c1 = b2_c;
    float *b2_c2 = b2_c + 1024;
    float *b2_c3 = b2_c + 2048;
    float *b2_c4 = b2_c + 3072;
    float *b3_c1 = b3_c;
    float *b3_c2 = b3_c + 1024;
    float *b3_c3 = b3_c + 2048;
    float *b3_c4 = b3_c + 3072;
    float *b4_c1 = b4_c;
    float *b4_c2 = b4_c + 1024;
    float *b4_c3 = b4_c + 2048;
    float *b4_c4 = b4_c + 3072;

    intree->SetBranchAddress("event", &event);
    intree->SetBranchAddress("tc1", &tc1);
    intree->SetBranchAddress("tc2", &tc2);
    intree->SetBranchAddress("tc3", &tc3);
    intree->SetBranchAddress("tc4", &tc4);
    intree->SetBranchAddress("b1_t", &b1_t);
    intree->SetBranchAddress("b2_t", &b2_t);
    intree->SetBranchAddress("b3_t", &b3_t);
    intree->SetBranchAddress("b4_t", &b4_t);
    intree->SetBranchAddress("b1_c", &b1_c);
    intree->SetBranchAddress("b2_c", &b2_c);
    intree->SetBranchAddress("b3_c", &b3_c);
    intree->SetBranchAddress("b4_c", &b4_c);

    std::string prefix(argv[1]);
    int pf = prefix.find(".root");
    TFile *outfile = TFile::Open((prefix.substr(0, pf) + "_pulse.root").c_str(), "recreate");

    // TTree *outtree = new TTree("pulse", "old pulse tree");
    TTree *outtree = new TTree("p", "new pulse tree");
    outtree->SetMarkerStyle(6);
    outtree->SetMarkerColor(2);
    outtree->SetLineColor(2);

    outtree->Branch("event", &event, "event/I");

    outtree->Branch("tc1", &tc1,  "tc1/I");
    outtree->Branch("t1",  b1_t,  TString::Format("t1[%d]/F", NSAMPLES) );
    outtree->Branch("c1",  b1_c1, TString::Format("c1[%d]/F", NSAMPLES) );
    outtree->Branch("c2",  b1_c2, TString::Format("c2[%d]/F", NSAMPLES) );
    outtree->Branch("c3",  b1_c3, TString::Format("c3[%d]/F", NSAMPLES) );
    outtree->Branch("c4",  b1_c4, TString::Format("c4[%d]/F", NSAMPLES) );
    outtree->Branch("tc2", &tc2,  "tc2/I");
    outtree->Branch("t2",  b2_t,  TString::Format("t2[%d]/F", NSAMPLES) );
    outtree->Branch("c5",  b2_c1, TString::Format("c5[%d]/F", NSAMPLES) );
    outtree->Branch("c6",  b2_c2, TString::Format("c6[%d]/F", NSAMPLES) );
    outtree->Branch("c7",  b2_c3, TString::Format("c7[%d]/F", NSAMPLES) );
    outtree->Branch("c8",  b2_c4, TString::Format("c8[%d]/F", NSAMPLES) );
    outtree->Branch("tc3", &tc3,  "tc3/I");
    outtree->Branch("t3",  b3_t,  TString::Format("t3[%d]/F",  NSAMPLES) );
    outtree->Branch("c9",  b3_c1, TString::Format("c9[%d]/F",  NSAMPLES) );
    outtree->Branch("c10", b3_c2, TString::Format("c10[%d]/F", NSAMPLES) );
    outtree->Branch("c11", b3_c3, TString::Format("c11[%d]/F", NSAMPLES) );
    outtree->Branch("c12", b3_c4, TString::Format("c12[%d]/F", NSAMPLES) );
    outtree->Branch("tc4", &tc4,  "tc4/I");
    outtree->Branch("t4",  b4_t,  TString::Format("t4[%d]/F",  NSAMPLES) );
    outtree->Branch("c13", b4_c1, TString::Format("c13[%d]/F", NSAMPLES) );
    outtree->Branch("c14", b4_c2, TString::Format("c14[%d]/F", NSAMPLES) );
    outtree->Branch("c15", b4_c3, TString::Format("c15[%d]/F", NSAMPLES) );
    outtree->Branch("c16", b4_c4, TString::Format("c16[%d]/F", NSAMPLES) );

    for (Long64_t jentry = 0; jentry < intree->GetEntries(); jentry++) {
/*
        if (jentry % 1000 == 0)
            std::cout<< "jentry = " << jentry << std::endl;
*/
        intree->LoadTree(jentry);
        intree->GetEntry(jentry);
        outtree->Fill();
    }

    std::cout << "Write " << outtree->GetEntries() << " events into file " << outfile->GetName() << std::endl;
    outfile->Write();
}
