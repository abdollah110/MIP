import os
import ROOT
from ROOT import *

def _FIT_(x, p):
#    Land = p[2] * TMath.Landau(x[0], p[3], p[4])
#    Pol0 = p[0]+p[1]*x[0]
    return TMath.Gaus(x[0])


In=TFile('outMean.root','open')

can = TCanvas();
gStyle.SetOptStat(0)

OutFile=TFile('DistMean.root','RECREATE')


for ch in range(128,144):
    OutHist=TH2F('out%s'%str(ch),'',25,0,50,25,0,50)

#    mean=In.Get('mean2D')
#    multiplicity=In.Get('multiplicity')
    mean=In.Get('chtot_Hist%s'%str(ch))
    multiplicity=In.Get('mulchtot_Hist%s'%str(ch))

    print mean.GetNbinsX()
    for x in range(mean.GetNbinsX()):
        for y in range(mean.GetNbinsY()):
            if mean.GetBinContent(x+1,y+1)==0: continue
            
            OutHist.SetBinContent(x+1,y+1,mean.GetBinContent(x+1,y+1)/multiplicity.GetBinContent(x+1,y+1))

    OutHist.GetXaxis().SetTitle("x [mm]")
    OutHist.GetYaxis().SetTitle("y [mm]")
#    OutHist.SetMaximum(OutHist.GetMaximum()*1.3)
    OutHist.Draw("COLZ")
#    XXX=OutHist.Fit("gaus","S")
#    func = TF1('func', 'gaus', 95, 105)
#    fit = OutHist.Fit('func', 'S')

    text=TText(5,45,"Combination of runs")
    text.Draw()
    text0=TText(5,40,"Channel %d"%ch)
    text0.Draw()
#    text1=TText(95.2,30,"Mean=%0.2f \n sigma=%0.2f"%(func.GetParameters()[1],func.GetParameters()[2]))
#    text1.SetTextFont (   41 )
#    text1.Draw()
#    print fit
#    print XXX.GetCorrelationMatrix()
#    M=TText(96,10,N)
#    M.Add(fit)
#    M.Draw()



#    OutHist.Draw("AC*")
#    print fit.GetParameters.Print()

#    theFit=TF1("theFit", "gaus")
#    FitParam=theFit.GetParameters()
#    print FitParam[1]
#    nPar =3
#    theFit=TF1("theFit", _FIT_, 95, 105,nPar)
#    OutHist.Fit("theFit", "R0")

#    print XXX

#    print "===>", XXX.GetCorrelationMatrix()

    can.SaveAs("_chtot%s.pdf"%str(ch))
                    

#OutFile.cd()
#OutHist.Write()
#OutFile.Close()


