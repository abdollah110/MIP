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

for ch in range(128,144):

    mean=In.Get('chtot_Hist%s'%str(ch))
    mean.Draw("Box")
    
    mean.GetXaxis().SetTitle("x [mm]")
    mean.GetYaxis().SetTitle("y [mm]")

    text=TText(5,45,"Combination of several runs")
    text.Draw()
    text0=TText(5,40,"Channel %d"%ch)
    text0.Draw()

    can.SaveAs("hotSpot%s.pdf"%str(ch))
