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

#define MAX_TOFPET_CHANNEL 400
#define SEARCHWINDOWSIZE 5000 //in ps units
#define MATCH_THRESHOLD 0.000001 // in seconds
#define DEBUG 0

using namespace std;


int main(int argc, char* argv[]){
    if(argc < 1) {
        cerr << "Please give 1 arguments " << "inputTOFHIRFileName " <<endl;
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
    
//    vector <TH1F *>  vecHist_chtime;
////    vector <TH2F *>  vecMult_chtime;
////    vector <TH2F *>  vecHist_chtot;
////    vector <TH2F *>  vecMult_chtot;
//
//
//    vecHist_chtime.clear();
////    vecMult_chtime.clear();
////    vecHist_chtot.clear();
////    vecMult_chtot.clear();
//
    
//    for (int ch=0; ch<8; ch++){
//
//
//        TH1F * chtime_Hist = new TH1F (("chtime_Hist"+std::to_string(ch)).c_str(),"", 200,-2000,2000);
////        TH2F * mulchtime_Hist = new TH2F (("mulchtime_Hist"+std::to_string(ch)).c_str(),"", 25,0,50,25,0,50);
////
////        TH2F * chtot_Hist = new TH2F (("chtot_Hist"+std::to_string(ch)).c_str(),"", 25,0,50,25,0,50);
////        TH2F * mulchtot_Hist = new TH2F (("mulchtot_Hist"+std::to_string(ch)).c_str(),"", 25,0,50,25,0,50);
//
//        vecHist_chtime.push_back(chtime_Hist);
////        vecMult_chtime.push_back(mulchtime_Hist);
////
////        vecHist_chtot.push_back(chtot_Hist);
////        vecMult_chtot.push_back(mulchtot_Hist);
//    }
    
    int vth[2] = {0,20};
    int ov[4] = {2,4,6,8};
    
    for(int iev=0; iev<inTree->GetEntries(); iev++) {
        
        inTree->GetEntry( iev);
        

        
        for (int chl=128; chl<144; chl=chl+2){
//            for (int chl=128; chl<130; chl=chl+2){
            for (int i=0; i < 2;i++){
            for (int j=0; j < 4;j++){
                
                if (step1 != ov[j] || step2 != vth[i]) continue;
                
//                cout<<step1 <<" "<<step2<<"\n";
                
            if (chTime[chl] > 0 && chTime[chl+1] >0 ){
                
                std::string OutName= "chtime_Hist_"+std::to_string(chl)+"_"+std::to_string(ov[j])+"_"+std::to_string(vth[i]);
                
               plotFill(OutName.c_str(),(chTime[chl]  - chTime[chl+1]) ,100,-2000,2000);
                
                
//                vecHist_chtime[(chl-128)/2]->Fill(chTime[chl]  - chTime[chl+1]);
            }
            }
                
            }
            
            
            
//            cout<<chTime[132] << "   "<< chTime[133] << "  diff= "<<chTime[132]  - chTime[133]  <<"\n";
            
            
            
//            if (chTime[chl] > 0 && x_dut > 0 && y_dut > 0) {
//
//                vecHist_chtime[chl-128]->Fill(x_dut,y_dut,chTime[chl]*1e-12);
//                vecMult_chtime[chl-128]->Fill(x_dut,y_dut,1);
//
//                vecHist_chtot[chl-128]->Fill(x_dut,y_dut,chtot[chl]);
//                vecMult_chtot[chl-128]->Fill(x_dut,y_dut,1);
//            }
        }
        
    }
    
    outFile->cd();
    
    
    
    map<string, TH1F*>::const_iterator iMap1 = myMap1->begin();
    map<string, TH1F*>::const_iterator jMap1 = myMap1->end();
    
    for (; iMap1 != jMap1; ++iMap1)
    nplot1(iMap1->first)->Write();

    
    
//    for (int j=0 ; j<vecHist_chtime.size();j++ ){
//        vecHist_chtime[j]->Write();
////        vecMult_chtime[j]->Write();
////        vecHist_chtot[j]->Write();
////        vecMult_chtot[j]->Write();
//        //        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    }
    outFile->Close();
}



