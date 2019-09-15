//================================================================================================================
// Done by abdollah.mohammadi@cern.ch and Si.Xie@cern.ch
//================================================================================================================
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

#include "Tofhir.h"
#include "Tracker.h"

#define MAX_TOFPET_CHANNEL 400
#define SEARCHWINDOWSIZE 15000 //in ps units  (5000 was changed to 15000 after an optimization study)
#define MATCH_THRESHOLD 0.000005 // in seconds
#define DEBUG 0

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3) {
        cerr << "Please give 3 arguments " << "inputTOFHIRFileName " << " inputTrackerFileName " << "outputFileName" <<endl;
        return -1;
    }
    
    //=====================Open TOFHIR input files=========================
    const char *inputTofhirFileName = argv[1];
    string singleFile= "/home/daq/2019_04_April_CMSTiming/TOFHIR/RecoData/v1/RecoWithTracks/run15706_singles.root";
    TFile *tofhirFile=new TFile(inputTofhirFileName,"read");
    TTree *tofhirTree = (TTree*)tofhirFile->Get("data");
    if( tofhirTree != NULL ) cout << "\n>>> got TOFHIR tree from file " << inputTofhirFileName << endl;
    else exit(-1);
    
    
    TOFHIR TOFTOF(tofhirTree);
    
    TOFHIR::SpillInfo  Ev1stSpill1st = TOFTOF.get1stEvent1stSplill();
    TOFHIR::SpillInfo  EvLastSpill1st =  TOFTOF.getLastEvent1stSplill(Ev1stSpill1st.Index );
    TOFHIR::SpillInfo  Ev1stSpill2nd =  TOFTOF.get1stEvent2ndSplill(EvLastSpill1st.Index);
    TOFHIR::SpillInfo  EvLastSpill2nd =  TOFTOF.getLastEvent2ndSplill(Ev1stSpill2nd.Index);
    TOFHIR::SpillInfo  Ev1stSpill3rd =  TOFTOF.get1stEvent3rdSplill(EvLastSpill2nd.Index);
    
    
    cout << "TOFHIR: first event's index and time in spill 1: " << Ev1stSpill1st.Index << " : " << Ev1stSpill1st.Time << "\n";
    cout << "TOFHIR: last event's index and time in spill 1:  " << EvLastSpill1st.Index << " : " << EvLastSpill1st.Time << "\n";
    cout << "TOFHIR: first event's index and time in spill 2: " << Ev1stSpill2nd.Index << " : " << Ev1stSpill2nd.Time << "\n";
    cout << "TOFHIR: last event's index and time in spill 2:  " << EvLastSpill2nd.Index << " : " << EvLastSpill2nd.Time << "\n";
    cout << "TOFHIR: first event's index and time in spill 3: " << Ev1stSpill3rd.Index << " : " << Ev1stSpill3rd.Time << "\n";

    
    
    //=====================Open Tracker input files=========================
    const char *inputTrackerFileName = argv[2];
    string trackingFile = "/home/daq/2019_04_April_CMSTiming/Tracks/Run14592_CMSTiming_FastTriggerStream_converted.root";
    TFile *trackerFile = new TFile(inputTrackerFileName,"READ");
    TTree *trackerTree = (TTree*)trackerFile->Get("CMSTiming");
    
    if( trackerTree != NULL ) cout << ">>> got track tree from file " << inputTrackerFileName << endl;
    else exit(-1);



    TRACKER TKTK(trackerTree);
    

    TRACKER::SpillInfo  Ev1stSpill1st_trk = TKTK.get1stEvent1stSplill(trackerTree, 0);
    TRACKER::SpillInfo  EvLastSpill1st_trk =  TKTK.getLastEvent1stSplill(trackerTree, Ev1stSpill1st_trk.Index );
    TRACKER::SpillInfo  Ev1stSpill2nd_trk =  TKTK.get1stEvent2ndSplill(trackerTree, EvLastSpill1st_trk.Index);
    TRACKER::SpillInfo  EvLastSpill2nd_trk =  TKTK.getLastEvent2ndSplill(trackerTree, Ev1stSpill2nd_trk.Index);
    TRACKER::SpillInfo  Ev1stSpill3rd_trk =  TKTK.get1stEvent3rdSplill(trackerTree, EvLastSpill2nd_trk.Index);


    cout << "TRACKER: first event's index and time in spill 1: " << Ev1stSpill1st_trk.Index << " : " << Ev1stSpill1st_trk.Time << "\n";
    cout << "TRACKER: last event's index and time in spill 1:  " << EvLastSpill1st_trk.Index << " : " << EvLastSpill1st_trk.Time << "\n";
    cout << "TRACKER: first event's index and time in spill 2: " << Ev1stSpill2nd_trk.Index << " : " << Ev1stSpill2nd_trk.Time << "\n";
    cout << "TRACKER: last event's index and time in spill 2:  " << EvLastSpill2nd_trk.Index << " : " << EvLastSpill2nd_trk.Time << "\n";
    cout << "TRACKER: first event's index and time in spill 3: " << Ev1stSpill3rd_trk.Index << " : " << Ev1stSpill3rd_trk.Time << "\n";


    //sanity check
//    Int_t totalNumEve=TOFHIR_TriggerIndices.size();
//    if (trackerTree->GetEntries()/TOFHIR_TriggerIndices.size() < 1.0 or trackerTree->GetEntries()/TOFHIR_TriggerIndices.size() > 1.5 ){
//        cout <<"Tracker has much less or much more events than TOFHIR !!!\n";
//        exit(-1);
//    }

    //=====================RECREATE outputFile=========================
    const char *outFileName   = argv[3];
    TFile *outFile = new TFile(outFileName,"recreate");
    TTree *outTree = new TTree("data","data");
    outTree->SetAutoSave();
//
//    //=====================Config Variables=========================
    const UInt_t triggerChannelID = 384;
    const double  ClockScaleFactor=1.04617;

//    //=====================Initialize input tree variables=========================
    UInt_t channelID;
    Long64_t time;
    float tot;
    float xIntercept;
    float yIntercept;
    float xSlope;
    float ySlope;
    float x_dut;
    float y_dut;
    float chi2;
    int ntracks;
    int nplanes;
    float step1_;
    float step2_;
    int matchEff;

    channelID=-9999;
    time=-9999;
    tot=-9999;
    xIntercept=-9999;
    yIntercept=-9999;
    xSlope=-9999;
    ySlope=-9999;
    x_dut=-9999;
    y_dut=-9999;
    chi2 = -9999;
    ntracks = -1;
    nplanes = -1;
    step1_=-99;
    step2_=-99;
    matchEff=0;

    int totalNumEve=1;
    int totalNumEveMatched=1;

    int numSpill=0;
//    //==============set Branch addresses for all the input variables================
//    tofhirTree->SetBranchAddress("channelID",&channelID);
//    tofhirTree->SetBranchAddress("time",&time);
//    tofhirTree->SetBranchAddress("tot",&tot);
//    tofhirTree->SetBranchAddress("step1",&step1_);
//    tofhirTree->SetBranchAddress("step2",&step2_);
//    //=====================Initialize output tree variables=========================
    Long64_t chTime[400];
    float chtot[400];
    Int_t event;
    float step1;
    float step2;
    //initialize for event 1
    event=1;
    for(int k=0;k<400;k++){
        chTime[k]=-9999;
        chtot[k]=-9999;
    }
    Long64_t TofhirTimeZero=0;
    Long64_t TrackerTimeZero=0;

    //==============set Branch addresses for all the output variables================
    outTree->Branch("event",&event,"event/I");
    outTree->Branch("step1_n", &step1, "step1_n/F");
    outTree->Branch("step2_n", &step2, "step2_n/F");
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



//
//    //================================================================================================================
//    //================================================================================================================
//    //======================== Look for coincidences =================================================================
//    //================================================================================================================
//    //================================================================================================================
//
    int PreviousMatchIndex = Ev1stSpill1st_trk.Index;
    cout << std::setprecision(10);

    for (Int_t q=Ev1stSpill1st.Index;q<TOFTOF.triggeredTofhirEv.size(); q++) {
//
        tofhirTree->GetEntry(TOFTOF.triggeredTofhirEv[q-1].Index);
        Long64_t tTrigger_previous = TOFTOF.triggeredTofhirEv[q-1].Time;  // This is the time for channelID 384
//
//
        tofhirTree->GetEntry(TOFTOF.triggeredTofhirEv[q].Index);
//
        
//
        //================================================================================================================
        //======================== reset the clock for 2nd and 3rd spill =================================================
        //================================================================================================================
        TofhirTimeZero=Ev1stSpill1st.Time;
            if ( q >= Ev1stSpill2nd.Index){
                TofhirTimeZero=Ev1stSpill2nd.Time;
            }
            if ( q >= Ev1stSpill3rd.Index){
                TofhirTimeZero=Ev1stSpill3rd.Time;
            }
        //================================================================================================================
        if (1) cout<<"q= "<<q<<"   time is= "<<TOFTOF.triggeredTofhirEv[q].Time*1e-12 <<"  dif=" <<(TOFTOF.triggeredTofhirEv[q].Time-TofhirTimeZero)*1e-12<<"\n";
//
//
        //initialize at beginning of each event
        for(int pp=0;pp<400;pp++){
            chTime[pp]=-9999;
            chtot[pp]=-9999;
        }

        //populate trigger data
        chTime[triggerChannelID] = time;
        chtot[triggerChannelID] = tot;
//
//
        //find signal hits that correspond to the given trigger
        // We find that sipms are typically around 170ns EARLIER than the trigger timestamp.
        // So look within a 10ns window around that.
        Long64_t tTrigger = time;  // This is the time for channelID 384

        // Just look at the hits that are 100 before the trigger or 100 after the trigger. If their time matches with trigger time then write them donw in the chTime/chtot vectors, otherwsie they will be filled with default -999; Now it is insured that the time and tot of all hits from the the single events are the same
        int j_start = TOFTOF.triggeredTofhirEv[q].Index-100;
        if (j_start < 0) j_start = 0;
        int j_end = TOFTOF.triggeredTofhirEv[q].Index+100;

        for (Int_t j=j_start;j<j_end; j++) {
            tofhirTree->GetEntry(j);

            Long64_t tdiff = tTrigger - time;

cout <<"Long64_t tdiff = tTrigger - time; "<<tdiff <<"  "<< tTrigger <<"  "<< time<<" \n ";
            //If channel falls within our search window, then populate the data for that channel
            if (tdiff >= 170000 - SEARCHWINDOWSIZE &&  tdiff <= 170000 + SEARCHWINDOWSIZE) {
                chTime[channelID] = time;
                chtot[channelID] = tot;
            }
        }

        double TOFHIRTriggerTimestamp = (tTrigger - TofhirTimeZero)*1e-12;
//
        int NMatchedTracks = 0;
        matchEff=0;
        for(int k=PreviousMatchIndex; k<trackerTree->GetEntries(); k++) {

            cout<<"trackerTree->GetEntries()= "<<trackerTree->GetEntries()<<"   PreviousMatchIndex="<<PreviousMatchIndex<<"\n";

            trackerTree->GetEntry( k );
            Long64_t TrackerBCOThisEvent = TKTK.trackerEvent->bco;

            if (TKTK.trackerEvent->bco== 4294967295) continue;


        //================================================================================================================
        //======================== reset the clock for 2nd and 3rd spill =================================================
        //================================================================================================================
            TrackerTimeZero=Ev1stSpill1st_trk.Time;
            if ( k >= Ev1stSpill2nd_trk.Index){
                TrackerTimeZero=Ev1stSpill2nd_trk.Time;
            }
            if ( k >= Ev1stSpill3rd_trk.Index){
                TrackerTimeZero=Ev1stSpill3rd_trk.Time;
            }


            double trackerTime = (TrackerBCOThisEvent - TrackerTimeZero) * 144e-9 * ClockScaleFactor;
//
            cout<<"trackerTree->GetEntries()= "<<trackerTree->GetEntries()<<"   PreviousMatchIndex="<<PreviousMatchIndex<<"\n";
            if (1) cout<<"k = "<<k<<"  time is= "<<TrackerBCOThisEvent<< " - "<<TrackerTimeZero<<" =>   dif= "<<trackerTime<<"\n";
//
//            //================================================================================================================
//            //now find a match with TOFHIR trigger time
            if (fabs (TOFHIRTriggerTimestamp - trackerTime ) < MATCH_THRESHOLD  ){
                cout <<"Event with index of Tofhir= "<<q <<"  MATCHED w/ event w/ index of Tracker= " << k << " : " << TOFHIRTriggerTimestamp << "   | "<< trackerTime<<"\n";
                totalNumEveMatched++;
                NMatchedTracks++;
                PreviousMatchIndex = k;

                //populate tracking data
                xIntercept=TKTK.trackerEvent->xIntercept * 1e-3;
                yIntercept=TKTK.trackerEvent->yIntercept * 1e-3;
                xSlope=TKTK.trackerEvent->xSlope * 1e-3;
                ySlope=TKTK.trackerEvent->ySlope * 1e-3;
                x_dut= (TKTK.trackerEvent->xIntercept + TKTK.trackerEvent->xSlope * 2.0e5) * 1e-3;
                y_dut= (TKTK.trackerEvent->yIntercept + TKTK.trackerEvent->ySlope * 2.0e5) * 1e-3;
                chi2 = TKTK.trackerEvent->chi2;
                ntracks = NMatchedTracks;
                nplanes = TKTK.trackerEvent->nPlanes;
                step1=step1_;
                step2=step2_;
                matchEff=1;

            }
            else if (TOFHIRTriggerTimestamp - trackerTime < -0.001 || (TOFHIRTriggerTimestamp==0 && trackerTime==0)) {
            
                break;
            }

        }

        outTree->Fill();
        event++;
    }

    outFile->Write();
    outFile->Close();
    tofhirFile->Close();
    cout<<"\n============>  Matching efficiency is "<<totalNumEveMatched <<"/" <<totalNumEve <<" = "<< 1.0*totalNumEveMatched/totalNumEve<<"\n";
    cout<<"====================================================================================================================================\n\n";
}
