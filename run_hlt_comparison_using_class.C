#include "Compare_GenHLTOffline.cc"
#include "TString.h"

void run_hlt_comparison_using_class()
{
  TString sample1 = "/data/ndpc0/c/slaunwhj/ONLINE/CMSSW_5_3_11_patch3/src/DQMOffline/Trigger/test/53X_menu2011.root";
  TString sample2 = "/data/ndpc0/c/slaunwhj/ONLINE/CMSSW_4_4_2_patch8/src/DQMOffline/Trigger/test/442_menu2011.root";
	
  //TString sample1 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/620_pre7.root";
  //TString sample2 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/620_pre6.root";
  TString label1 = "620_pre5_3_11_p3";
  TString label2 = "620_pre4_4_2_p8";
  
  TString new_histo_file = "JasonCompare.root";
  

  Compare_GenHLTOffline *relval_HLT = new Compare_GenHLTOffline();
  
  relval_HLT->load_files(sample1,sample2,label1,label2);
  relval_HLT->new_file(new_histo_file);
  relval_HLT->compare_genhltoffline();
}
