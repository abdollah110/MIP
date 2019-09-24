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
#include <utility>

#include "Tracker.h"

#define SEARCHWINDOWSIZE 15000 //in ps units  (5000 was changed to 15000 after an optimization study)
#define MATCH_THRESHOLD 0.000005 // in seconds
#define ClockScaleFactor 1.04617
#define DEBUG_Tofhir 1

using namespace std;
//std::cout << std::setprecision(10);

class TOFHIR {
public:
    
    struct SpillInfo {
        int Index;
        Long64_t Time;
    };
    
    struct chTimeInfo {
        Long64_t chTime_[400];
        float chtot_[400];
    };
    
    
    
    explicit TOFHIR (TTree *);
    
    const UInt_t triggerChannelID = 384;
    vector<SpillInfo> triggeredTofhirEv;
    vector<chTimeInfo> TimeVar;
    
    
    Long64_t *  makeTimeTot(TTree * , int );
    
    SpillInfo get1stEvent1stSplill(Int_t);
    SpillInfo getLastEvent1stSplill(Int_t);
    SpillInfo get1stEvent2ndSplill(Int_t);
    SpillInfo getLastEvent2ndSplill(Int_t);
    SpillInfo get1stEvent3rdSplill(Int_t);
    
    int doMatching(TTree *, TRACKER , SlowTrigTracker , TTree *,TTree *, TTree *, int  );
    int find1stSpill(TRACKER ,TTree *, TTree *, int , int , int );
    void MatchAndFill(TTree * ,   TRACKER , SlowTrigTracker ,TTree *, TTree *, TTree * ,TTree *, vector< pair <int,int> > );
    
    UInt_t channelID;
    Long64_t time;
    float tot;
    float step1_;
    float step2_;
    
    vector< pair <int,int> > SPILLIndex ;
    
private:
    
    
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
        }
    }
    
    chTimeInfo tInfo;
    for (Int_t q=0;q<triggeredTofhirEv.size(); q++) {
        
        tofhirTree->GetEntry(triggeredTofhirEv[q].Index);
        
        for(UInt_t pp=0;pp<400;pp++){
            tInfo.chTime_[pp]=-9999;
            tInfo.chtot_[pp]=-9999;
        }
        
        tInfo.chTime_[triggerChannelID] = time;
        tInfo.chtot_[triggerChannelID] = tot;
        
        Long64_t tTrigger = time;
        
        int j_start = triggeredTofhirEv[q].Index-100;
        if (j_start < 0) j_start = 0;
        int j_end = triggeredTofhirEv[q].Index+100;
        
        for (Int_t j=j_start;j<j_end; j++) {
            tofhirTree->GetEntry(j);
            
            Long64_t tdiff = tTrigger - time;
            
            //If channel falls within our search window, then populate the data for that channel
            if (tdiff >= 170000 - SEARCHWINDOWSIZE &&  tdiff <= 170000 + SEARCHWINDOWSIZE) {
                tInfo.chTime_[channelID] = time;
                tInfo.chtot_[channelID] = tot;
            }
        }
        TimeVar.push_back(tInfo);
    }
}


//================================================================================================================
//////// First Event in 1st spill  in TOFHIR data
//================================================================================================================

TOFHIR::SpillInfo TOFHIR::get1stEvent1stSplill(Int_t startEvent){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 2ms of each other then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 2e-3 && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 < 2e-2  ) {
            spl.Index = i;
            spl.Time = timeThisEvent;
            if (DEBUG_Tofhir) cout << "TOFHIR: first event's index and time in spill 1: " << i << " : " << timeThisEvent << "\n";
            break;
        }
    }
    return  spl;
}

//================================================================================================================
//////// Last Event in 1st spill  in TOFHIR data
//================================================================================================================

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
            cout << "TOFHIR: last event's index and time in spill 1:  " << i << " : " << timeThisEvent << "\n";
            break;
        }
        
        //if next event is more than  10ms and the next-to-next is more than 50 ms then we have left the spill
        if ( abs(timeNextEvent-timeThisEvent) * 1e-12 > 1e-2  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 > 5e-2 ) {
            spl.Index = i;
            spl.Time = timeThisEvent;
            if (DEBUG_Tofhir) cout << "TOFHIR: last event's index and time in spill 1:  " << i << " : " << timeThisEvent << "\n";
            break;
        }
    }
    return  spl;
}

//================================================================================================================
//////// First Event in 2nd spill  in TOFHIR data
//================================================================================================================

TOFHIR::SpillInfo TOFHIR::get1stEvent2ndSplill(Int_t startEvent){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 5ms of each other and the next-to-next is less than 10 ms then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 2e-3  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 < 1e-2) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            
            if (DEBUG_Tofhir)  cout << "TOFHIR: first event's index and time in spill 2: " << i << " : " << timeThisEvent << "\n";
            break;
        }
    }
    return  spl;
}

//================================================================================================================
//////// Last Event in 2nd spill  in TOFHIR data
//================================================================================================================

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
            if (DEBUG_Tofhir)   cout << "TOFHIR: last event's index and time in spill 2:  " << i << " : " << timeThisEvent << "\n";
            break;
        }
        
        //if next event is more than  10ms and the next-to-next is more than 50 ms then we have left the spill
        if ( abs(timeNextEvent-timeThisEvent) * 1e-12 > 1e-2  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 > 5e-2 ) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            
            if (DEBUG_Tofhir)  cout << "TOFHIR: last event's index and time in spill 2:  " << i << " : " << timeThisEvent << "\n";
            
            break;
        }
    }
    return  spl;
}

//================================================================================================================
//////// Last Event in 3rd spill  in TOFHIR data
//================================================================================================================


TOFHIR::SpillInfo TOFHIR::get1stEvent3rdSplill(Int_t startEvent){
    
    TOFHIR::SpillInfo spl;
    for (Int_t i=startEvent;i<triggeredTofhirEv.size(); i++) {
        
        Long64_t timeNextToNextEvent = triggeredTofhirEv[i+2].Time;
        Long64_t timeNextEvent = triggeredTofhirEv[i+1].Time;
        Long64_t timeThisEvent = triggeredTofhirEv[i].Time;
        
        //if events are within 5ms of each other and the next-to-next is less than 10 ms then we have entered the spill
        if ( abs(timeNextEvent-timeThisEvent) *1e-12 < 2e-3  && abs(timeNextToNextEvent-timeNextEvent) * 1e-12 < 1e-2) {
            
            spl.Index = i;
            spl.Time = timeThisEvent;
            if (DEBUG_Tofhir)   cout << "TOFHIR: first event's index and time in spill 3: " << i << " : " << timeThisEvent << "\n";
            
            break;
        }
    }
    return  spl;
}


//*********************************************************************************************************************
//*********************************************************************************************************************
//                                         Finding Begining of Spill precisely
//*********************************************************************************************************************
//*********************************************************************************************************************

int TOFHIR::find1stSpill(TRACKER TRK_Fast,TTree *tofhirTree, TTree *trackerTree, int cut, int firstIndexTOF, int firstIndexTRK){
    
    int n_tof=cut%10-5;
    int n_trg=cut/10-5;
    
    int SpilBegin=firstIndexTOF+n_tof;
    if (SpilBegin < 0) SpilBegin=0;
    int PreviousMatchIndex = firstIndexTRK+n_trg;
    if (PreviousMatchIndex < 0) PreviousMatchIndex=0;
    
    Long64_t  TofhirTimeZero=triggeredTofhirEv[SpilBegin].Time;
    trackerTree->GetEntry(PreviousMatchIndex);
    Long64_t   TrackerTimeZero = TRK_Fast.trackerEvent->bco;
    
    int firstOfTrack=PreviousMatchIndex;
    int totalNumEveMatched=1;
    
    //==============set Branch addresses for all the input variables================
    UInt_t channelID=-9999;
    Long64_t time=-9999;
    
    tofhirTree->SetBranchAddress("channelID",&channelID);
    tofhirTree->SetBranchAddress("time",&time);
    
    //======================== Look for coincidences =================================================================
    for (Int_t q=SpilBegin;q<SpilBegin+20; q++) {
        
        
        if (q-SpilBegin-totalNumEveMatched > 10) {
            return 0;
        }
        
        tofhirTree->GetEntry(triggeredTofhirEv[q].Index);
        double TOFHIRTriggerTimestamp = (time - TofhirTimeZero)*1e-12;
        
        for(int k=PreviousMatchIndex; k<trackerTree->GetEntries(); k++) {
            
            trackerTree->GetEntry( k );
            Long64_t TrackerBCOThisEvent = TRK_Fast.trackerEvent->bco;
            if (TRK_Fast.trackerEvent->bco== 4294967295) continue;
            double trackerTime = (TrackerBCOThisEvent - TrackerTimeZero) * 144e-9 * ClockScaleFactor;
            
            if (fabs (TOFHIRTriggerTimestamp - trackerTime ) < MATCH_THRESHOLD  ){
                totalNumEveMatched++;
                PreviousMatchIndex = k;
                break;
            }
        }
    }
    
    SPILLIndex.push_back(make_pair(SpilBegin,firstOfTrack));
    return 1;
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//                                         Matching Function
//*********************************************************************************************************************
//*********************************************************************************************************************

void TOFHIR::MatchAndFill(TTree * outTree,   TRACKER TRK_Fast, SlowTrigTracker TRK_Slow,TTree *tofhirTree, TTree *trackerTree, TTree * trackerTreeSlow , TTree * VMETree, vector< pair <int,int> > SpillInterval){
    
    cout<<"\nIndex of first spill in TOFHIR= "<< SpillInterval[0].first << "\t and first spill in TRACKER = "<< SpillInterval[0].second<<"\n";
    cout<<"Index of second spill in TOFHIR= "<< SpillInterval[1].first << "\t and second spill in TRACKER = "<< SpillInterval[1].second<<"\n";
    cout<<"Index of third spill in TOFHIR= "<< SpillInterval[2].first << "\t and third spill in TRACKER = "<< SpillInterval[2].second<<"\n\n";
    
    
    int SpilBegin=SpillInterval[0].first;
    int PreviousMatchIndex= SpillInterval[0].second;
    
    Long64_t  TofhirTimeZero=triggeredTofhirEv[SpilBegin].Time;
    trackerTree->GetEntry(PreviousMatchIndex);
    Long64_t   TrackerTimeZero = TRK_Fast.trackerEvent->bco;
    
    //==============set Branch addresses TOFHIR================
    UInt_t channelID=-9999;
    Long64_t time=-9999;
    float tot=-9999;
    float step1_=-9999;
    float step2_=-9999;
    
    tofhirTree->SetBranchAddress("channelID",&channelID);
    tofhirTree->SetBranchAddress("time",&time);
    tofhirTree->SetBranchAddress("tot",&tot);
    tofhirTree->SetBranchAddress("step1",&step1_);
    tofhirTree->SetBranchAddress("step2",&step2_);
    
    
    //==============set Branch addresses VME================
    UInt_t  i_evt_            ;
    float channel_[36][1024];
    float time_[4][1024]    ;
    float baseline_[36]     ;
    float baseline_RMS_[36];
    float noise_[36]        ;
    float amp_[36]          ;
    float t_peak_[36]       ;
    float integral_[36]     ;
    float intfull_[36]      ;
    float risetime_[36]     ;
    float decaytime_[36]    ;
    float gaus_mean_[36]    ;
    float gaus_sigma_[36]  ;
    float gaus_chi2_[36]    ;
    int triggerNumber_ ;
    UShort_t corruption_      ;
    
    VMETree->SetBranchAddress("i_evt",&i_evt_);
    VMETree->SetBranchAddress("channel",&channel_);
    VMETree->SetBranchAddress("time",&time_);
    VMETree->SetBranchAddress("baseline",&baseline_);
    VMETree->SetBranchAddress("baseline_RMS",&baseline_RMS_);
    VMETree->SetBranchAddress("noise",&noise_);
    VMETree->SetBranchAddress("amp",&amp_);
    VMETree->SetBranchAddress("t_peak",&t_peak_);
    VMETree->SetBranchAddress("integral",&integral_);
    VMETree->SetBranchAddress("intfull",&intfull_);
    VMETree->SetBranchAddress("risetime",&risetime_);
    VMETree->SetBranchAddress("decaytime",&decaytime_);
    VMETree->SetBranchAddress("gaus_mean",&gaus_mean_);
    VMETree->SetBranchAddress("gaus_sigma",&gaus_sigma_);
    VMETree->SetBranchAddress("gaus_chi2",&gaus_chi2_);
    VMETree->SetBranchAddress("triggerNumber",&triggerNumber_);
    VMETree->SetBranchAddress("corruption",&corruption_);
    
    
    //==============Add Branch for all the output variables================
    Int_t event=1;
    float step1=-99;
    float step2=-99;
    Long64_t chTime[400];
    float chtot[400];
    float xIntercept=-9999;
    float yIntercept=-9999;
    float xSlope=-9999;
    float ySlope=-9999;
    float x_dut=-9999;
    float y_dut=-9999;
    float chi2=-9999;
    int ntracks=-1;
    int nplanes=-1;
    int matchEff=0;
    int SlowTriggerTag=0;
    
    // VME related branches
    UInt_t  i_evt            ;
    float channel[36][1024];
    float timeVME[4][1024]    ;
    float baseline[36]     ;
    float baseline_RMS[36];
    float noise[36]        ;
    float amp[36]          ;
    float t_peak[36]       ;
    float integral[36]     ;
    float intfull[36]      ;
    float risetime[36]     ;
    float decaytime[36]    ;
    float gaus_mean[36]    ;
    float gaus_sigma[36]  ;
    float gaus_chi2[36]    ;
    int triggerNumber ;
    UShort_t corruption      ;
    
    
    outTree->Branch("event",&event,"event/I");
    outTree->Branch("step1", &step1, "step1/F");
    outTree->Branch("step2", &step2, "step2/F");
    outTree->Branch("chTime",&chTime,"chTime[400]/L");
    outTree->Branch("chtot",&chtot,"chtot[400]/F");
    outTree->Branch("xIntercept", &xIntercept, "xIntercept/F");
    outTree->Branch("yIntercept", &yIntercept, "yIntercept/F");
    outTree->Branch("xSlope", &xSlope, "xSlope/F");
    outTree->Branch("ySlope", &ySlope, "ySlope/F");
    outTree->Branch("x_dut", &x_dut, "x_dut/F");
    outTree->Branch("y_dut", &y_dut, "y_dut/F");
    outTree->Branch("chi2", &chi2, "chi2/F");
    outTree->Branch("ntracks", &ntracks, "ntracks/I");
    outTree->Branch("nplanes", &nplanes, "nplanes/I");
    outTree->Branch("matchEff", &matchEff, "matchEff/I");
    outTree->Branch("SlowTriggerTag", &SlowTriggerTag, "SlowTriggerTag/I");
    
    // VME related branches
    outTree->Branch("i_evt",&i_evt ,"i_evt/i");
    outTree->Branch("channel",&channel   ,"channel[36][1024]/F");
    outTree->Branch("timeVME",&timeVME  ,"timeVME[4][1024]/F");
    outTree->Branch("baseline",&baseline  ,"baseline[36]/F");
    outTree->Branch("baseline_RMS",&baseline_RMS ,"baseline_RMS[36]/F");
    outTree->Branch("noise",&noise ,"noise[36]/F");
    outTree->Branch("amp",&amp   ,"amp[36]/F");
    outTree->Branch("t_peak",&t_peak,"t_peak[36]/F");
    outTree->Branch("integral",&integral  ,"integral[36]/F");
    outTree->Branch("intfull",&intfull   ,"intfull[36]/F");
    outTree->Branch("risetime",&risetime  ,"risetime[36]/F");
    outTree->Branch("decaytime",&decaytime ,"decaytime[36]/F");
    outTree->Branch("gaus_mean",&gaus_mean ,"gaus_mean[36]/F");
    outTree->Branch("gaus_sigma",&gaus_sigma ,"gaus_sigma[36]/F");
    outTree->Branch("gaus_chi2",&gaus_chi2 ,"gaus_chi2[36]/F");
    outTree->Branch("triggerNumber",&triggerNumber ,"triggerNumber/I");
    outTree->Branch("corruption",&corruption ,"corruption/s");
    
    
    //initialize for event 1
    for(int k=0;k<400;k++){
        chTime[k]=-9999;
        chtot[k]=-9999;
    }
    
    //==============Other variables================
    int totalNumEve=triggeredTofhirEv.size();
    int totalNumEveMatched=1;
    int bothTriggeredFired=0;
    int lastMatchedIndex=0;
    
    //    //================================================================================================================
    //    //================================================================================================================
    //    //======================== Look for coincidences =================================================================
    //    //================================================================================================================
    //    //================================================================================================================
    
    
    for (Int_t q=SpilBegin;q<triggeredTofhirEv.size(); q++) {
        
        tofhirTree->GetEntry(triggeredTofhirEv[q].Index);
        
        //================================================================================================================
        //======================== reset the clock for 2nd and 3rd spill =================================================
        //================================================================================================================
        
        if ( q >= SpillInterval[1].first && SpillInterval[1].first > 0){
            TofhirTimeZero=triggeredTofhirEv[SpillInterval[1].first].Time;
            trackerTree->GetEntry( SpillInterval[1].second );
            TrackerTimeZero=TRK_Fast.trackerEvent->bco;
        }
        
        if ( q >= SpillInterval[2].first && SpillInterval[2].first > 0){
            TofhirTimeZero=triggeredTofhirEv[SpillInterval[2].first].Time;
            trackerTree->GetEntry( SpillInterval[2].second );
            TrackerTimeZero=TRK_Fast.trackerEvent->bco;
        }
        //================================================================================================================
        //        if (DEBUG_Tofhir) cout<<"q= "<<q<<"   time is= "<<triggeredTofhirEv[q].Time*1e-12 <<"  dif=" <<(triggeredTofhirEv[q].Time-TofhirTimeZero)*1e-12<<"\n";
        
        
        for(int pp=0;pp<400;pp++){
            chTime[pp]=TimeVar[q].chTime_[pp];
            chtot[pp]=TimeVar[q].chtot_[pp];
        }
        
        double TOFHIRTriggerTimestamp = (time - TofhirTimeZero)*1e-12;
        int NMatchedTracks = 0;
        matchEff=0;
        
        for(int k=PreviousMatchIndex; k<trackerTree->GetEntries(); k++) {
            
            trackerTree->GetEntry( k+1 );
            Long64_t TrackerBCONextEvent = TRK_Fast.trackerEvent->bco;
            
            
            trackerTree->GetEntry( k );
            Long64_t TrackerBCOThisEvent = TRK_Fast.trackerEvent->bco;
            
            if (TRK_Fast.trackerEvent->bco== 4294967295) continue;
            
            double trackerTime = (TrackerBCOThisEvent - TrackerTimeZero) * 144e-9 * ClockScaleFactor;
            
            //            if (DEBUG_Tofhir) cout<<"k = "<<k<<"  time is= "<<TrackerBCOThisEvent<< " - "<<TrackerTimeZero<<" =>   dif= "<<trackerTime<<"\n";
            
            //            //================================================================================================================
            if (fabs (TOFHIRTriggerTimestamp - trackerTime ) < MATCH_THRESHOLD  ){
                if(q%10000==0 )    cout <<"Event with index of Tofhir= "<<q <<"  MATCHED w/ event w/ index of Tracker= " << k << " : " << TOFHIRTriggerTimestamp << "   | "<< trackerTime<<"\n";
                totalNumEveMatched++;
                NMatchedTracks++;
                PreviousMatchIndex = k;
                
                //================================================================================================================
                // Fill variables of the FastTriggerMode of Tracker
                //================================================================================================================
                
                xIntercept=TRK_Fast.trackerEvent->xIntercept * 1e-3;
                yIntercept=TRK_Fast.trackerEvent->yIntercept * 1e-3;
                xSlope=TRK_Fast.trackerEvent->xSlope * 1e-3;
                ySlope=TRK_Fast.trackerEvent->ySlope * 1e-3;
                x_dut= (TRK_Fast.trackerEvent->xIntercept + TRK_Fast.trackerEvent->xSlope * 2.0e5) * 1e-3;
                y_dut= (TRK_Fast.trackerEvent->yIntercept + TRK_Fast.trackerEvent->ySlope * 2.0e5) * 1e-3;
                chi2 = TRK_Fast.trackerEvent->chi2;
                ntracks = NMatchedTracks;
                nplanes = TRK_Fast.trackerEvent->nPlanes;
                step1=step1_;
                step2=step2_;
                matchEff=1;
                
                SlowTriggerTag=0;
                
                // Check weather the slow trigger is also fired for this particular event;
                if (TrackerBCONextEvent != TrackerBCOThisEvent){
                    
                    for(int s=lastMatchedIndex; s<lastMatchedIndex+10; s++) {
                        
                        trackerTreeSlow->GetEntry( s );
                        if (TRK_Fast.trackerEvent->chi2==TRK_Slow.trackerEventSlow->chi2){
                            SlowTriggerTag=1;
                            lastMatchedIndex = s;
                            bothTriggeredFired++;
                            
                            //================================================================================================================
                            // Fill variables of the VME
                            //================================================================================================================
                            
                            VMETree->GetEntry(TRK_Slow.trackerEventSlow->trigger);
                            
                            
                            for (int ix=0; ix < 1024;ix++){
                                
                                //                                for (int jx=0; jx < 36;jx++){
                                //                                    channel[jx][ix]=channel_[jx][ix];
                                //                                }
                                for (int jy=0; jy < 4;jy++){
                                    timeVME[jy][ix]=time_[jy][ix];
                                }
                            }
                            
                            for (int p=0;p<36;p++){
                                baseline[p]= baseline_[p]     ;
                                baseline_RMS[p]= baseline_RMS_[p];
                                noise[p]= noise_[p]        ;
                                amp[p]= amp_[p]          ;
                                t_peak[p]= t_peak_[p]       ;
                                integral[p]= integral_[p]     ;
                                intfull[p]= intfull_[p]      ;
                                risetime[p]= risetime_[p]     ;
                                decaytime[p]= decaytime_[p]    ;
                                gaus_mean[p]= gaus_mean_[p]    ;
                                gaus_sigma[p]= gaus_sigma_[p]  ;
                                gaus_chi2[p]= gaus_chi2_[p]    ;
                            }
                            
                            i_evt=i_evt_;
                            triggerNumber=triggerNumber_;
                            corruption=corruption_;
                            
                            //================================================================================================================
                            break;
                        }  // match slow mode trigger and fill vME info
                        else{
                            
                            for (int p=0;p<36;p++){
                                baseline[p]= -999;
                                baseline_RMS[p]= -999;
                                noise[p]= noise_[p] -999;
                                amp[p]= amp_[p]         -999;
                                t_peak[p]= t_peak_[p]       -999;
                                integral[p]= integral_[p]     -999;
                                intfull[p]= intfull_[p]     -999;
                                risetime[p]= risetime_[p]    -999;
                                decaytime[p]= decaytime_[p]   -999;
                                gaus_mean[p]= gaus_mean_[p]   -999;
                                gaus_sigma[p]= gaus_sigma_[p] -999;
                                gaus_chi2[p]= gaus_chi2_[p]   -999;
                            }
                            i_evt=i_evt_;
                            triggerNumber=-999;
                            corruption=-999;
                            
                            
                        } // else fill the branches with default values
                    }// loop over slow mode trigger
                }//check the next two events do not have same time values
            }// Check if Tofhir evet matched the trigger
            else if (TOFHIRTriggerTimestamp - trackerTime < -0.001 ) {
                break;
            }
            
        }
        
        outTree->Fill();
    }
    
    cout<<"\n=>  Matching efficiency is "<<totalNumEveMatched <<"/" <<totalNumEve <<" = "<< 1.0*totalNumEveMatched/totalNumEve<<"\n";
    cout<<"\n=>  Both Trigger Fired ratio is  "<<bothTriggeredFired <<"/" <<trackerTreeSlow->GetEntries() <<" = "<< 1.0*bothTriggeredFired/trackerTreeSlow->GetEntries()<<"\n";
    cout<<"======================================================================================================\n\n";
}
