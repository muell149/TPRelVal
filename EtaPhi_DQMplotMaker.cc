#include<TH1F.h>
#include<TFile.h>
#include<TCanvas.h> 

vector<double> comp(TH1F* h1, TH1F* h2, char* r1, char* r2){
  char* hist_name1 = h1->GetName();
  TAxis *x1 = h1->GetXaxis();
  TAxis *x2 = h1->GetXaxis();
  Int_t nbin1 = x1->GetNbins();
  Int_t nbin2 = x2->GetNbins();
  Float_t xmin = x1->GetXmin();
  Float_t xmax = x1->GetXmax();
  int bin1;
  int bin2;
  int clost;
  //printf("\n %d bins in h1, %d bins in h2 \n",nbin1,nbin2);                                                                                                                                                
  if (nbin1 == nbin2){
    char lostitle[256];
    sprintf(lostitle,"Counts Lost %s",hist_name1);
    TH1F* hlost = new TH1F(lostitle,hist_name1,nbin1,xmin,xmax);
    TH1F* hgain = new TH1F("Counts Gained",hist_name1,nbin1,xmin,xmax);
    for (ibin=1; ibin<=nbin1; ibin++){
      bin1=h1->GetBinContent(ibin);
      bin2=h2->GetBinContent(ibin);
      clost = bin1-bin2;
      if (clost>0){
        hlost->SetBinContent(ibin,clost);
      }
      else if(clost<0){
        hgain->SetBinContent(ibin,-clost);
      }
    }
    else
      {
	//printf("\n nbin1 != nbin2 \n");                                                                                                                                                                        
      }
  }

  hlost->SetLineColor(1);
  hgain->SetLineColor(2);
  h1->SetLineColor(1);
  h2->SetLineColor(2);

  char title[256];                                                                                                                                                                                           
  sprintf(title,"can1 %s",h1->GetName());                                                                                                                                                                    
  TCanvas* can1 = new TCanvas(title,title);                                                                                                                                                                  
  h1->Draw("hist");                                                                                                                                                                                          
  h2->Draw("samehist");                                                                                                                                                                                      
  TLegend *leg1 = new TLegend(0.83,0.66,0.99,0.77,NULL,"brNDC");                                                                                                                                              
  if(h1->Integral()>0) leg1->AddEntry(h1,r1);                                                                                                                                                            
  if(h2->Integral()>0) leg1->AddEntry(h2,r2);                                                                                                                                                            
  leg1->Draw("same");                                                                                                                                                                                        
  can1->SaveAs(title);


  char labell[256];
  char labelg[256];
  sprintf(labell,"Counts lost from %s to %s",r1,r2);
  sprintf(labelg,"Counts gained from %s to %s",r1,r2);
  char title[256];                                                                                                                                                                                           
  sprintf(title,"can3 %s",hlost->GetName());                                                                                                                                                                 
  TCanvas* can3 = new TCanvas(title,title);                                                                                                                                                                  
  hlost->Draw("hist");                                                                                                                                                                                       
  hgain->Draw("samehist");                                                                                                                                                                                   
  TLegend *leg3 = new TLegend(0.73,0.61,0.99,0.75,NULL,"brNDC");                                                                                                                                              
  if(hlost->Integral()>0) leg3->AddEntry(hlost,labell);                                                                                                                                               
  if(hgain->Integral()>0) leg3->AddEntry(hgain,labelg);                                                                                                                                             
  leg3->Draw("same");  
  can3->SaveAs(title);
}


void EtaPhi_DQMplotMaker(void){
  TFile* f1 = new TFile("/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre1DQM.root");///afs/cern.ch/user/m/muell149/work/muell149/CMSSW_6_2_0_pre5/src/DQMOffline/Trigger/test/pre5DQM.root");
  TFile* f2 = new TFile("/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre2DQM.root");
  char* r1="700_pre1";
  char* r2="700_pre2";

  vector<double> drawIt;
  char path[256];
  char path1[256];
  sprintf(path1,"DQMData/Run\ 1/HLT/Run\ summary/GeneralHLTOffline");
   
  f1->cd(path1);
  TDirectoryFile *dir = (TDirectoryFile*)f1->Get(path1);
  
  TIter nextkey(dir->GetListOfKeys());
  TKey *key;
  
  while( key = (TKey*)nextkey()){
    TObject *obj = key->ReadObj();
    //printf(obj->GetName());
    if(obj->IsA()->InheritsFrom("TH1F")){
      TH1F* h1 = (TH1F*)obj;
      printf(h1->GetName());
      if (h1!=NULL){
	char* hist_name = h1->GetName();
	sprintf(path,"DQMData/Run\ 1/HLT/Run\ summary/GeneralHLTOffline/%s",hist_name);
	TH1F* h2 = (TH1F*)f2->Get(path);
	if (h2!=NULL){
	  drawIt = comp(h1,h2,r1,r2);
	}
      }
    }
  }
  
}
