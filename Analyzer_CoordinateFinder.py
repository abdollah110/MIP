import ROOT
from ROOT import *

def main(args):

    InputFile=TFile(args.run,"read");
    tr =InputFile.Get("data");

    can = TCanvas();
    gStyle.SetOptStat(0)

    ########################################################
    # Plots for arrays SiPMs
    ########################################################
    for ch in range(128,144):
        tr.Draw("(chtot[%d]>0):y_dut:x_dut>>h(35,0,70,25,0,50,2,-0.5,1.5)"%ch,"ntracks==1 ","profcolz");


        L_array=[]
        for ll in range(128,144):
            xx= ll-128
            L=TLine(38+xx*2,25,38+xx*2,35)
            L_array.append(L)

        for ll in range(len(L_array)):
            L_array[ll].SetLineWidth(15)
            L_array[ll].SetLineColor(18)
            if ll+128==ch: L_array[ll].SetLineColor(5)
            L_array[ll].Draw()

        Tex=TText(40,15,"Channel Number=%d"%ch)
        Tex.Draw()

        can.SaveAs("Out_arrays%d.pdf"%ch);


    SingleCh=[0,1,14,15]


    
    ########################################################
    # Plots for arrays SiPMs
    ########################################################
    for ch in range(128,144):
        tr.Draw("(chtot[%d]>0):y_dut:x_dut>>h(35,0,70,25,0,50,2,-0.5,1.5)"%ch,"ntracks==1 && x_dut >(%d-128) && x_dut <(%d-122) "%(ch,ch),"profcolz");
        
        
        L_array=[]
        for ll in range(128,144):
            xx= ll-128
            L=TLine(38+xx*2,25,38+xx*2,35)
            L_array.append(L)
        
        for ll in range(len(L_array)):
            L_array[ll].SetLineWidth(15)
            L_array[ll].SetLineColor(18)
            if ll+128==ch: L_array[ll].SetLineColor(5)
            L_array[ll].Draw()
    
        Tex=TText(40,15,"Channel Number=%d"%ch)
        Tex.Draw()
        
        can.SaveAs("Out_arrays_cutted%d.pdf"%ch);


    SingleCh=[0,1,14,15]
    
    ########################################################
    # Plots for single SiPMs
    ########################################################

    
    for chNum,ch in enumerate(SingleCh):
        tr.Draw("(chtot[%d]>0):y_dut:x_dut>>h(35,0,70,25,0,50,2,-0.5,1.5)"%ch,"ntracks==1 ","profcolz");
    
    
        L_array=[]
        for ll in range(0,4):
                L=TLine(50,25+ll*2,70,25+ll*2)
                L_array.append(L)

        for ll in range(len(L_array)):
            L_array[ll].SetLineWidth(15)
            L_array[ll].SetLineColor(18)
            if ll==chNum: L_array[ll].SetLineColor(5)
            L_array[ll].Draw()

        Tex=TText(40,15,"Channel Number=%d"%ch)
        Tex.Draw()
        
        can.SaveAs("Out_single%d.pdf"%ch);




if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--runNumber', '-r', action='store', dest='run', default='~/2019_04_April_CMSTiming/TOFHIR/RecoData/v1/RecoWithTracks/run15572_events.root', help='run')
    
    main(parser.parse_args())




