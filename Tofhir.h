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

using namespace std;


class TOFHIR {
public:
    
    struct SpillInfo {
        int Index;
        Long64_t Time;
    };
    
    
    explicit TOFHIR (TTree *);
    
    const UInt_t triggerChannelID = 384;
    vector<SpillInfo> triggeredTofhirEv;
    
    SpillInfo get1stEvent1stSplill();
    SpillInfo getLastEvent1stSplill(Int_t);
    SpillInfo get1stEvent2ndSplill(Int_t);
    SpillInfo getLastEvent2ndSplill(Int_t);
    SpillInfo get1stEvent3rdSplill(Int_t);
    
private:
    UInt_t channelID;
    Long64_t time;
    float tot;
    float step1_;
    float step2_;
    
    
};


TOFHIR::TOFHIR(TTree * tofhirTree):
//Member initialization in constructors
channelID(-9999), // functional form
time(-9999),
tot(-9999),
step1_(-99),
step2_(-99)
{
    tofhirTree->SetBranchAddress("channelID",&channelID);
    tofhirTree->SetBranchAddress("time",&time);
    tofhirTree->SetBranchAddress("tot",&tot);
    tofhirTree->SetBranchAddress("step1",&step1_);
    tofhirTree->SetBranchAddress("step2",&step2_);
    
    SpillInfo test;
    for (Int_t ix=0;ix<tofhirTree->GetEntries(); ix++) {
        
        tofhirTree->GetEntry(ix);
        
        if (channelID == triggerChannelID) {
            
            test.Time= time;
            test.Index= ix;
            
            triggeredTofhirEv.push_back(test);
            //            TOFHIR_TriggerIndices.push_back(ix);
        }
    }
}


TOFHIR::SpillInfo TOFHIR::get1stEvent1stSplill(){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=0;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 2ms of each other then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 2e-3 ) {
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
    }
    return  spl;
}



TOFHIR::SpillInfo TOFHIR::getLastEvent1stSplill(Int_t startEvent){
    
    
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //Check whether we have reached the end of spill
        if (i == triggeredTofhirEv.size()-1 ){
            cout << i<<" \t\t we reached to the end of spill \n";
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
        
        //if events are more than 0.01s of each other then we have left the spill
        if ( abs(timeNextEvent-timeThisEvent) * 1e-12 > 1e-2  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 > 5e-2 ) {
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
        
    }
    return  spl;
}




TOFHIR::SpillInfo TOFHIR::get1stEvent2ndSplill(Int_t startEvent){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 2ms of each other then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 5e-3  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 < 1e-2) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
    }
    return  spl;
}



TOFHIR::SpillInfo TOFHIR::getLastEvent2ndSplill(Int_t startEvent){
    
    
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //Check whether we have reached the end of spill
        if (i == triggeredTofhirEv.size()-1 ){
            cout << i<<" \t\t we reached to the end of spill \n";
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
        
        //if events are more than 0.01s of each other then we have left the spill
        if ( abs(timeNextEvent-timeThisEvent) * 1e-12 > 1e-2  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 > 5e-2 ) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
        
    }
    return  spl;
}


TOFHIR::SpillInfo TOFHIR::get1stEvent3rdSplill(Int_t startEvent){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 2ms of each other then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 5e-3  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 < 1e-2) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            break;
        }
    }
    return  spl;
}
