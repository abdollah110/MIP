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



int main(int argc, char* argv[]){

const char *inputFileName = argv[1];

TFile *AnaFile=new TFile(inputFileName,"read");
TTree *tr = (TTree*)AnaFile->Get("data");


TCanvas * can = new TCanvas();

    int array=[0];
    for (int i=128;i<144;i++){
            array[i]=i;
    }
    

    
    for (int i=0)
    tr->Draw("(chtot[143]>0):y_dut:x_dut>>h(100,0,50,100,00,50,2,-0.5,1.5)","ntracks==1 ","profcolz");
    
    can->SaveAs("out.pdf");
}



#!/bin/bash
for i in `seq 15643 15646`;
do
echo $i

./ConvertTOFPETSinglesToEvents ~/2019_04_April_CMSTiming/TOFHIR/RecoData/v1/RecoWithTracks/run$i_singles.root /home/daq/2019_04_April_CMSTiming/Tracks/Run$i_CMSTiming_FastTriggerStream_converted.root out_$i.root
done

.root



