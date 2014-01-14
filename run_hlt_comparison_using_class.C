#include "Compare_GenHLTOffline.cc"
#include "TString.h"

void run_hlt_comparison_using_class()
{
  TString sample1 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre2.root";
  TString sample2 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/700pre4.root";
	
  //TString sample1 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/620_pre7.root";
  //TString sample2 = "/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/620_pre6.root";
  TString label1 = "700pre2";
  TString label2 = "700pre4";
  
  TString new_histo_file = "JasonCompare.root";
  

  Compare_GenHLTOffline *relval_HLT = new Compare_GenHLTOffline();
  
  relval_HLT->load_files(sample1,sample2,label1,label2);
  relval_HLT->new_file(new_histo_file);
  relval_HLT->compare_genhltoffline();
}
