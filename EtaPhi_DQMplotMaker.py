#!/usr/bin/python
from ROOT import TLegend, TFile, TH1F, TCanvas, SetOwnership
import subprocess,os

#Usage: python EtaPhi_DQMplotMaker.py -b


def plot(h1,h2,r1,r2,savePlotDir):

    SetOwnership(h1,False)
    SetOwnership(h2,False)
    hist_name1 = h1.GetName()
    x1 = h1.GetXaxis()
    x2 = h1.GetXaxis()
    nbin1 = x1.GetNbins()
    nbin2 = x2.GetNbins()
    xmin = x1.GetXmin()
    xmax = x1.GetXmax()
    #printf("\n %d bins in h1, %d bins in h2 \n",nbin1,nbin2)                                                                                                                                                
    if nbin1 == nbin2:
        lostitle="Counts Lost "+hist_name1
        hlost = TH1F(lostitle,hist_name1,nbin1,xmin,xmax)
        hgain = TH1F("Counts Gained",hist_name1,nbin1,xmin,xmax)
        ibin=1
        while ibin<=nbin1:
            bin1=h1.GetBinContent(ibin)
            bin2=h2.GetBinContent(ibin)
            clost = bin1-bin2
            if clost>0:
                hlost.SetBinContent(ibin,clost)
            elif clost<0:
                hgain.SetBinContent(ibin,-clost)
            ibin=ibin+1
    else:
        print "nbin1 != nbin2"
        
    hlost.SetLineColor(1)
    hgain.SetLineColor(2)
    h1.SetLineColor(1)
    h2.SetLineColor(2)

    title=h1.GetName()                                                                                                                                                                    
    can1 = TCanvas(title,title)                                                                                                                                                                  
    h1.Draw("hist")                                                                                                                                                                                          
    h2.Draw("samehist")                                                                                                                                                                                      
    legend = TLegend(0.83,0.66,0.99,0.77,"","brNDC")                                                                                                                                              
    if h1.Integral()>0: legend.AddEntry(h1,r1)                                                                                                                                                            
    if h2.Integral()>0: legend.AddEntry(h2,r2)                                                                                                                                                            
    legend.Draw("same")
    title = savePlotDir+title+".png"
    can1.SaveAs(title)
    
    labell="Counts_lost_from_"+r1+"_to_"+r2
    labelg="Counts gained from"+r1+" to "+r2
    title=hlost.GetName()                                                                                                                                                                 
    can3 = TCanvas(title,title)                                                                                                                                                                  
    hlost.Draw("hist")                                                                                                                                                                                       
    hgain.Draw("samehist")                                                                                                                                                                                   
    legend = TLegend(0.73,0.61,0.99,0.75,"","brNDC")                                                                                                                                              
    if hlost.Integral()>0: legend.AddEntry(hlost,labell)                                                                                                                                               
    if hgain.Integral()>0: legend.AddEntry(hgain,labelg)                                                                                                                                             
    legend.Draw("same")
    title=savePlotDir+title+".png"
    can3.SaveAs(title)
    
    SetOwnership(legend,0)
    SetOwnership(can1,0)
    SetOwnership(can3,0)
    return can1,can3



def main():
    f1 = TFile("/afs/cern.ch/user/m/muell149/workb/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre1DQM.root")
    f2 = TFile("/afs/cern.ch/user/m/muell149/workb/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre2DQM.root")
    r1="700pre1"
    r2="700pre2"
    savePlotDir="DQM_plots/"+r1+"_"+r2+"/"
    cmd = "mkdir "+savePlotDir
    subprocess.call(cmd,shell=True)
    
    path1="DQMData/Run 1/HLT/Run summary/GeneralHLTOffline"
    f1.cd(path1)
    dir = f1.Get(path1)
    nextkey = dir.GetListOfKeys()
      
    for key in nextkey:
        h1 = key.ReadObj()
        #print "h1 before=",type(h1)
        if isinstance(h1,TH1F):
            print h1.GetName()
            if h1 is not None:
                hist_name = h1.GetName()
                path = "DQMData/Run 1/HLT/Run summary/GeneralHLTOffline/"+hist_name
                h2 = f2.Get(path)
                if h2 is not None:
                    #print "h2=",h2.GetName()
                    #print type(h1), type(h2)
                    #break
                    plot(h1,h2,r1,r2,savePlotDir)


if __name__=='__main__':
    main()
