// include std libraries
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <string.h>
#include <sstream>
// include ROOT libraries 
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TProfile.h"
#include <thread>
#include <chrono>
#include "makeHisto.h"

using namespace std;


int main(int argc, char* argv[]){
    if(argc < 1) {
        cerr << "Please give 1 arguments " << "inputRECOFileName " <<endl;
        return -1;
    }
    
    
    myMap1 = new std::map<std::string, TH1F*>();
    
    int event;
    Float_t step1;
    Float_t step2;
    long long  chTime[400];
    float chtot[400];
    Float_t xIntercept;
    Float_t yIntercept;
    Float_t xSlope;
    Float_t ySlope;
    Float_t x_dut;
    Float_t y_dut;
    float chi2;
    int ntracks;
    int nplanes;
    
    //=====================Open Tracker input files=========================
    const char *inputFileName = argv[1];
    TFile *inFile = new TFile(inputFileName,"READ");
    TTree *inTree = (TTree*)inFile->Get("data");
    
    inTree->SetBranchAddress("event",&event);
    inTree->SetBranchAddress("step1", &step1);
    inTree->SetBranchAddress("step2", &step2);
    inTree->SetBranchAddress("chTime",&chTime);
    inTree->SetBranchAddress("chtot",&chtot);
    inTree->SetBranchAddress("xIntercept", &xIntercept);
    inTree->SetBranchAddress("yIntercept", &yIntercept);
    inTree->SetBranchAddress("xSlope", &xSlope);
    inTree->SetBranchAddress("ySlope", &ySlope);
    inTree->SetBranchAddress("x_dut", &x_dut);
    inTree->SetBranchAddress("y_dut", &y_dut);
    inTree->SetBranchAddress("chi2", &chi2);
    inTree->SetBranchAddress("ntracks", &ntracks);
    inTree->SetBranchAddress("nplanes", &nplanes);
    
    
    TFile * outFile =TFile::Open("outMean.root","RECREATE");
    
    int vth[2] = {0,20};
    int ov[4] = {2,4,6,8};
    
    
    for(int iev=0; iev<inTree->GetEntries(); iev++) {
        
        inTree->GetEntry( iev);
        
        if (iev % 10000 == 0) fprintf(stdout, "\r  Processed events: %d  ", iev);
        fflush(stdout);
        
        for (int chl=128; chl<144; chl++){
            for (int i=0; i < 2;i++){
                for (int j=0; j < 4;j++){
                    
                    // This is to unsure that the runs/data have the same Ov and vth settings
                    if (step1 != ov[j] || step2 != vth[i]) continue;
                    
                    // Time difference between 2 channels
                    if (chTime[chl] > 0 && chTime[chl+1] >0 ){
                        if (chl%2==0){
                            std::string Name_chTime= "chtime_Hist_"+std::to_string(chl)+"_"+std::to_string(ov[j])+"_"+std::to_string(vth[i]);
                            plotFill(Name_chTime.c_str(),(chTime[chl]  - chTime[chl+1]) ,100,-2000,2000);
                        }
                    }
                    
                    // chTot of each channel
                    if (chTime[chl] > 0){
                        std::string Name_chtot= "chTOT_Hist_"+std::to_string(chl)+"_"+std::to_string(ov[j])+"_"+std::to_string(vth[i]);
                        plotFill(Name_chtot.c_str(),(chtot[chl])*1E-3 ,250,0,500);
                    }
                    
                }
            }
        }
    }

outFile->cd();



map<string, TH1F*>::const_iterator iMap1 = myMap1->begin();
map<string, TH1F*>::const_iterator jMap1 = myMap1->end();

for (; iMap1 != jMap1; ++iMap1)
nplot1(iMap1->first)->Write();

outFile->Close();
}
