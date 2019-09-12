import os
import ROOT
from ROOT import *
import numpy as np
from array import array



def add_CMS():
    lowX=0.10
    lowY=0.82
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.15, lowY+0.16, "NDC")
    lumi.SetTextFont(61)
    lumi.SetTextSize(0.035)
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.AddText("FNAL June 2019 BTL Test Beam         (SiPM arrays)")
    return lumi

def make_legend():
    output = ROOT.TLegend(0.5, 0.10, 0.85, 0.45, "", "brNDC")
    output.SetLineWidth(0)
    output.SetLineStyle(0)
    output.SetFillStyle(0)
    output.SetBorderSize(0)
    output.SetTextFont(62)
#    output.SetNColumns(2)
    return output


def run_adjust(mean):

    mean.GetXaxis().SetTitleSize(0.04)
    mean.GetXaxis().SetNdivisions(505)
    mean.GetYaxis().SetLabelFont(42)
    mean.GetYaxis().SetLabelOffset(0.01)
    mean.GetYaxis().SetLabelSize(0.03)
    mean.GetYaxis().SetTitleSize(0.035)
    mean.GetYaxis().SetTitleOffset(1.54)
    mean.GetYaxis().SetTitle("Events")
    

    return mean



######################################################################
#   Input root file
######################################################################
InputFile=TFile('outMean.root','open')

## channels
channel=[128,132,134,136,138,140,142]
AllChannel=[128,129,131,132,133,134,135,136,137,138,139,140,141,142,143]
AllChannel_Even=[128,132,134,136,138,140,142]
AllChannel_Odd=[129,131,133,135,137,139,141,143]
OverVolt=[2,4,6,8]
vThreshhold=[0,20]


######################################################################
#   CHTime
######################################################################


outF=TFile('outTGraph_ChTime.root','RECREATE')
can=ROOT.TCanvas("canvas","",0,0,600,600)

adapt=ROOT.gROOT.GetColor(12)
new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
ROOT.gStyle.SetFrameLineWidth(3)
ROOT.gStyle.SetLineWidth(3)

gStyle.SetOptStat(0)

for ch in channel:
    tDirectory= outF.mkdir(str(ch))
    
    for i,vth in enumerate(vThreshhold):
        hist=TH2F('out%s%s'%(str(ch),str(vth)),'',10,-.5,9.5,150,0,300)
        n = 4
        x, y = array( 'd' ), array( 'd' )

        for ov in OverVolt:
        
            histoName='chtime_Hist_%s_%s_%s'%(str(ch),str(ov),str(vth))
            print "HistoName is", histoName
            mean=InputFile.Get(histoName)
            mean.SetTitle("")
            can.SetGridy()
            mean=run_adjust(mean)
#            mean.GetXaxis().SetTitleSize(0.04)
#            mean.GetXaxis().SetNdivisions(505)
#            mean.GetYaxis().SetLabelFont(42)
#            mean.GetYaxis().SetLabelOffset(0.01)
#            mean.GetYaxis().SetLabelSize(0.03)
#            mean.GetYaxis().SetTitleSize(0.035)
#            mean.GetYaxis().SetTitleOffset(1.54)
#            mean.GetYaxis().SetTitle("Events")

            mean.GetXaxis().SetTitle("Time [ps] Channel %d - Channel %d"%(ch, ch+1))
            mean.GetYaxis().SetTitle("# of events")
            mean.SetMaximum(mean.GetMaximum()*1.3)
            mean.Draw()
            fit_low= mean.GetXaxis().GetBinCenter(int(mean.GetMaximumBin()))-300
            fit_high= mean.GetXaxis().GetBinCenter(int(mean.GetMaximumBin()))+300
            print "fit_low ", fit_low, " fit_high ", fit_high
            #    XXX=OutHist.Fit("gaus","S")
            func = TF1('func', 'gaus', -1000,1000)
            fit = mean.Fit('func', 'S','',fit_low,fit_high)


            text0=TText(-1900,mean.GetMaximum()*0.95,"Channel %d - Channel %d"%(ch, ch+1))
            text0.SetTextSize(0.04)
            text0.Draw()
            text1=TText(-1900,mean.GetMaximum()*0.85,"Mean=%0.0f ps \n sigma=%0.0f ps"%(func.GetParameters()[1],func.GetParameters()[2]))
            sigma=func.GetParameters()[2]
            text1.SetTextSize(0.04)
#            text1.SetTextFont (   62 )
            text1.Draw()
            text=TText(-1900,mean.GetMaximum()*0.75,"th=%d, V_ov= %d"%(vth,ov))
            text.SetTextSize(0.04)
#            text1.SetTextFont (   62 )
            text.Draw()

            l2=add_CMS()
            l2.Draw("same")

            can.SaveAs("plots/Time/_mean%s.pdf"%histoName)

            x.append(ov)
            y.append(sigma)

        gr = TGraph( n, x, y )
        gr.SetLineColor( 2 )
        gr.SetLineWidth( 4 )
        gr.SetMarkerColor( 4 )
        gr.SetMarkerStyle( 21 )
        gr.GetXaxis().SetTitle( 'X title' )
        gr.GetYaxis().SetTitle( 'Y title' )
        gr.Draw( 'ACP' )


        tDirectory.cd()
#        hist.Draw('AC*')
        tDirectory.WriteObject(gr,'mean_%s_%s'%(str(ch),str(vth)))


outF.Close()


######################################################################
#   CHTime   plotting
######################################################################
plotInput_chTime=TFile('outTGraph_ChTime.root','open')

for vth in vThreshhold:

    can=ROOT.TCanvas("canvas","",0,0,600,600)
    adapt=ROOT.gROOT.GetColor(12)
    new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
    ROOT.gStyle.SetFrameLineWidth(3)
    ROOT.gStyle.SetLineWidth(3)
    gStyle.SetOptStat(0)
    gStyle.SetTitle("")
    can.SetGridy()

    TG_0=[]
    for ch in channel:
        TG_0.append(plotInput_chTime.Get('%s/mean_%s_%s'%(ch,ch,vth)))


    tg1=TG_0[0]
    tg1.SetTitle("")
    tg1.SetLineColor(4)
    tg1.GetYaxis().SetRangeUser(30,250)
    tg1.GetYaxis().SetTitle("#sigma_t [ps]")
    tg1.GetXaxis().SetTitle("V_ov [V]")

    for i,th0ch in enumerate(TG_0):

        th0ch.SetLineColor(i+1)
        th0ch.SetMarkerSize(0)
        if i==0 : th0ch.Draw()
        else: th0ch.Draw('same')



    #tg1.Draw()
    #tg2.Draw('same')

    text0=TText(2,220,"Threshold %s [ADC]"%vth)
    text0.SetTextSize(0.04)
    text0.Draw()

    legende=make_legend()
    for i,th0ch in enumerate(TG_0):
        legende.AddEntry(th0ch,'Channel %s-%s'%(str(channel[i]),str(channel[i]+1)),"l")

    legende.Draw()

    l2=add_CMS()
    l2.Draw("same")


    can.SaveAs('plots/Time/_allcombined_CHTime_%s.pdf'%vth)



for ch in channel:

    can=ROOT.TCanvas("canvas","",0,0,600,600)
    adapt=ROOT.gROOT.GetColor(12)
    new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
    ROOT.gStyle.SetFrameLineWidth(3)
    ROOT.gStyle.SetLineWidth(3)
    gStyle.SetOptStat(0)
    gStyle.SetTitle("")
    can.SetGridy()

    tg1=plotInput_chTime.Get('%s/mean_%s_0'%(ch,ch))
    tg2=plotInput_chTime.Get('%s/mean_%s_20'%(ch,ch))
    tg1.SetTitle("")
    tg1.SetLineColor(4)
    tg1.GetYaxis().SetRangeUser(50,250)
    tg1.GetYaxis().SetTitle("#sigma_t [ps]")
    tg1.GetXaxis().SetTitle("V_ov [V]")
    



    tg1.Draw()
    tg2.Draw('same')
    
    text0=TText(2,220,"Channel %d "%ch)
    text0.SetTextSize(0.04)
    text0.Draw()

    legende=make_legend()
    legende.AddEntry(tg1,"vth=0 ","l")
    legende.AddEntry(tg2,"vth=20 ","l")
    legende.Draw()

    l2=add_CMS()
    l2.Draw("same")


    can.SaveAs('plots/Time/_combined_CHTime_%s.pdf'%ch)




######################################################################
#   CHTOT
######################################################################

outF=TFile('outTGraph_ChTot.root','RECREATE')
can=ROOT.TCanvas("canvas","",0,0,600,600)
adapt=ROOT.gROOT.GetColor(12)
new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
ROOT.gStyle.SetFrameLineWidth(3)
ROOT.gStyle.SetLineWidth(3)
gStyle.SetOptStat(0)
gStyle.SetTitle("")

for ch in AllChannel:

    tDirectory= outF.mkdir(str(ch))
    for i,vth in enumerate(vThreshhold):
        hist=TH2F('out%s%s'%(str(ch),str(vth)),'',10,-.5,9.5,150,0,300)
        n = 4
        x, y = array( 'd' ), array( 'd' )

        for ov in OverVolt:
            
            histoName='chTOT_Hist_%s_%s_%s'%(str(ch),str(ov),str(vth))
            mean=InputFile.Get(histoName)
            MIPmax= mean.GetXaxis().GetBinCenter(int(mean.GetMaximumBin()))
            mean.Draw()
            
            l2=add_CMS()
            l2.Draw("same")
            
            text0=TText(250,mean.GetMaximum()*0.95,"Channel %d "%ch)
            text0.SetTextSize(0.04)
            text0.Draw()
            text1=TText(250,mean.GetMaximum()*0.85,"Threshod=%d, V_ov=%d"%(vth,ov))
            text1.SetTextSize(0.04)
            text1.Draw()
            
            mean.GetXaxis().SetTitle("Tot [ns]")
            mean.GetYaxis().SetTitle("# of events")
            mean.SetTitle("")
            mean.GetXaxis().SetTitleSize(0.04)
            mean.GetXaxis().SetNdivisions(505)
            mean.GetYaxis().SetLabelFont(42)
            mean.GetYaxis().SetLabelOffset(0.01)
            mean.GetYaxis().SetLabelSize(0.03)
            mean.GetYaxis().SetTitleSize(0.035)
            mean.GetYaxis().SetTitleOffset(1.54)

            
            can.SaveAs("plots/ToT/_mip%s.pdf"%histoName)
            

            x.append(ov)
            y.append(MIPmax)

        gr = TGraph( n, x, y )
        gr.SetLineColor( 2 )
        gr.SetLineWidth( 4 )
        gr.SetMarkerColor( 4 )
        gr.SetMarkerStyle( 21 )
        gr.GetXaxis().SetTitle( 'X title' )
        gr.GetYaxis().SetTitle( 'Y title' )
        gr.Draw( 'ACP' )


        tDirectory.cd()
        tDirectory.WriteObject(gr,'totMax_%s_%s'%(str(ch),str(vth)))


outF.Close()




######################################################################
#   CHTOT   plotting
######################################################################
plotInput_chTime=TFile('outTGraph_ChTot.root','open')

for vth in vThreshhold:

    can=ROOT.TCanvas("canvas","",0,0,600,600)
    adapt=ROOT.gROOT.GetColor(12)
    new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
    ROOT.gStyle.SetFrameLineWidth(3)
    ROOT.gStyle.SetLineWidth(3)
    gStyle.SetOptStat(0)
    gStyle.SetTitle("")
    can.SetGridy()

    TG=[]
    TG_even=[]
    TG_odd=[]
    for ch in AllChannel:
        TG.append(plotInput_chTime.Get('%s/totMax_%s_%s'%(ch,ch,vth)))
    for ch in AllChannel_Even:
        TG_even.append(plotInput_chTime.Get('%s/totMax_%s_%s'%(ch,ch,vth)))
    for ch in AllChannel_Odd:
        TG_odd.append(plotInput_chTime.Get('%s/totMax_%s_%s'%(ch,ch,vth)))


#    TOTCases=[TG,TG_even,TG_odd]
#    TotChannel=[AllChannel,AllChannel_Even,AllChannel_Odd]
    TOTCases=[TG_odd]
    TotChannel=[AllChannel_Odd]


    for rank, TG_0 in enumerate(TOTCases):
    
        print "\n\n\n\n  len(TG_0)= ", len(TG_0),"\n\n\n\n"
        tg1=TG_0[0]
        tg1.SetTitle("")
        tg1.SetLineColor(4)
        tg1.GetYaxis().SetRangeUser(30,250)
        tg1.GetYaxis().SetTitle("tot [ns]")
        tg1.GetXaxis().SetTitle("V_ov [V]")

        for i,th0ch in enumerate(TG_0):
        
            print "th0ch= ",th0ch

            th0ch.SetLineColor(i+1)
            th0ch.SetMarkerSize(0)
            if i==0 : th0ch.Draw()
            else: th0ch.Draw('same')


        text0=TText(2,220,"Threshold %s [ADC]"%vth)
        text0.SetTextSize(0.04)
        text0.Draw()

        legende=make_legend()
        for i,th0ch in enumerate(TG_0):
            legende.AddEntry(th0ch,'Channel %s'%str(TotChannel[rank][i]),"l")

        legende.Draw()

        l2=add_CMS()
        l2.Draw("same")


        can.SaveAs('plots/ToT/_allcombined_CHTOT_%s_%s.pdf'%(vth,rank))



for ch in AllChannel:

    can=ROOT.TCanvas("canvas","",0,0,600,600)
    adapt=ROOT.gROOT.GetColor(12)
    new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
    ROOT.gStyle.SetFrameLineWidth(3)
    ROOT.gStyle.SetLineWidth(3)
    gStyle.SetOptStat(0)
    gStyle.SetTitle("")
    can.SetGridy()

    tg1=plotInput_chTime.Get('%s/totMax_%s_0'%(ch,ch))
    tg2=plotInput_chTime.Get('%s/totMax_%s_20'%(ch,ch))
    tg1.SetTitle("")
    tg1.SetLineColor(4)
    tg1.GetYaxis().SetRangeUser(50,250)
    tg1.GetYaxis().SetTitle("tot [ns]")
    tg1.GetXaxis().SetTitle("V_ov [V]")
    



    tg1.Draw()
    tg2.Draw('same')
    
    text0=TText(2,220,"Channel %d "%ch)
    text0.SetTextSize(0.04)
    text0.Draw()

    legende=make_legend()
    legende.AddEntry(tg1,"vth=0 [ADC]","l")
    legende.AddEntry(tg2,"vth=20 [ADC]","l")
    legende.Draw()

    l2=add_CMS()
    l2.Draw("same")


    can.SaveAs('plots/ToT/_combined_CHTOT_%s.pdf'%ch)
