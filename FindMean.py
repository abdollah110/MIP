import os
import ROOT
from ROOT import *
import numpy as np
from array import array


In=TFile('outMean.root','open')

can = TCanvas();
#gStyle.SetOptStat(0)


OverVolt=[2,4,6,8]
vThreshhold=[0,20]

#ar=np.array(4)
#
##ar[1].fill(0)
#print ar

#
def makePlot(ch):
    
    outF=TFile('out%s.root'%str(ch),'RECREATE')
    tDirectory= outF.mkdir(str(ch))

    for i,vth in enumerate(vThreshhold):
        hist=TH2F('out%s%s'%(str(ch),str(vth)),'',10,-.5,9.5,150,0,300)
        n = 4
        x, y = array( 'd' ), array( 'd' )

        for ov in OverVolt:
        

            histoName='chtime_Hist_%s_%s_%s'%(str(ch),str(ov),str(vth))
            mean=In.Get(histoName)


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
            text0.Draw()
            text1=TText(-1900,mean.GetMaximum()*0.85,"Mean=%0.0f ps \n sigma=%0.0f ps"%(func.GetParameters()[1],func.GetParameters()[2]))
            sigma=func.GetParameters()[2]
            text1.SetTextFont (   62 )
            text1.Draw()
            text=TText(-1900,mean.GetMaximum()*0.75,"th=%d, V_ov= %d"%(vth,ov))
            text1.SetTextFont (   62 )
            text.Draw()
            can.SaveAs("_mean%s.pdf"%histoName)
            
            x.append(ov)
            y.append(sigma)
        
        gr = TGraph( n, x, y )
        gr.SetLineColor( 2 )
        gr.SetLineWidth( 4 )
        gr.SetMarkerColor( 4 )
        gr.SetMarkerStyle( 21 )
        gr.SetTitle( 'a simple graph' )
        gr.GetXaxis().SetTitle( 'X title' )
        gr.GetYaxis().SetTitle( 'Y title' )
        gr.Draw( 'ACP' )
        
        
        tDirectory.cd()
#        hist.Draw('AC*')
        tDirectory.WriteObject(gr,'mean_%s_%s'%(str(ch),str(vth)))

    outF.Close()



channel=[128,132,134,136,138,140,142]
for ch in channel:
    makePlot(ch)



