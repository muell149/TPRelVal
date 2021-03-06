// compare_genhltoffline_script.C 
// created: Oct. 7, 2012 by Geoffrey Smith
// initial function: compares the "cppath" histos between two releases that were produced using HLTGeneralOffline code.
// 
// Modification history:
// 10/8/12 (G.S.) -- cleaned up print statements, added section that lists paths with zero hits
// 10/16-18 (G.S.) -- major mods -- compares hlt_count_hist histos from HLTGeneralOffline, added plots
// 11/2 (G.S.) -- v4. major mods (completely new): takes trees as input.
// 11/5 (G.S.) -- v5. mods to v4
// 11/9 (G.S.) -- v6. mods to v5
// 11/19 (G.S.) -- v7. mods to v6
// 1/13-4/13 -- up to v.11: many new plots, variables. text dump output option.
// 4/29/13 (G.S.) -- v12.: migration to modular implemenation (Compare_GenHLTOffline.cc)
// 
// Usage:
//   Compare_GenHLTOffline *relval_HLT = new Compare_GenHLTOffline();
//   relval_HLT->load_files(sample1,sample2,label1,label2);  // <-- sample1, sample2 both contain "the_gen_tree" and "the_HLT_tree" from QuickGenInfo and GeneralHLTOffline2, respectively
//   relval_HLT->new_file(new_histo_file); // <-- open new histo file
//   relval_HLT->compare_genhltoffline(); // <-- actually run the comparison code
//  
//  
//  

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream> 
#include <string>

#include "TH1.h"
// #include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
// #include "TObjString.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLegend.h"
#include "TSystem.h"
#include "TClass.h"


using namespace std;


class Compare_GenHLTOffline
{
	private:
		int verbosity;
		
		TString file1;
		TString file2;
		TString rel0_lbl;
		TString rel1_lbl;
		
		TFile *fn1;
		TFile *fn2;
		TFile *histfile;
		
		TTree *tree[2];
		TTree *gentree[2];
		
		TH2D *dataset_map;
		
		TString new_file_name;
		
		
		// --- HLT tree: ---
		char tree_path_name[2][750];

		int tree_event[2];
		int tree_path_index[2];
		int tree_path_was_run[2];
		int tree_path_error[2];
		int tree_path_accept[2];	

		char tree_last_module_run_label[2][750];
		int tree_last_module_run_index[2];

		char tree_last_module_with_saved_tags_label[2][750];
		int tree_last_module_with_saved_tags_index[2];

		char tree_L1_module_with_saved_tags_label[2][750];
		int tree_L1_module_with_saved_tags_index[2];

		char tree_last_module_run_type[2][750];

		int tree_L1_module_with_saved_tags_obj_ID[2][100];
		float tree_L1_module_with_saved_tags_obj_pt[2][100];
		float tree_L1_module_with_saved_tags_obj_eta[2][100];
		float tree_L1_module_with_saved_tags_obj_phi[2][100];

		int tree_last_module_with_saved_tags_obj_ID[2][100];
		float tree_last_module_with_saved_tags_obj_pt[2][100];
		float tree_last_module_with_saved_tags_obj_eta[2][100];
		float tree_last_module_with_saved_tags_obj_phi[2][100];
		

		// --- tree with gen info: ---
		int gen_tree_event[2];
		int gen_tree_gen_ID[2][100];
		int gen_tree_gen_parent_ID[2][100];
		int gen_tree_gen_grandparent_ID[2][100];
		float gen_tree_gen_px[2][100];
		float gen_tree_gen_py[2][100];
		float gen_tree_gen_pz[2][100]; 
		float gen_tree_gen_pt[2][100]; 
		float gen_tree_gen_energy[2][100];
		float gen_tree_gen_eta[2][100];
		float gen_tree_gen_phi[2][100];
		float gen_tree_gen_charge[2][100];
		
		
		
		long int get_entry_from_evt_trig_tree0[10000][750];
		long int get_entry_from_evt_trig_tree1[10000][750];
		long int get_entry_from_evt_gen_tree0[10000];
                long int get_entry_from_evt_gen_tree1[10000];

		std::vector<int> event_subset_lost_hits;
		std::vector<int> event_subset_gained_hits;
		
		int events0;
		int events1;
		
		int sanity_ck1;
		int sanity_ck2;
		
		int decay_type_study;
		int dataset_study;
		
		bool dump_evt_info;
		bool use_prescale;
		bool use_last_mod_run_type;
		
		
		// histos
		TH1F *path_accept0;
		TH1F *path_accept1;
		TH1F *abs_path_accept_diff;

		TH1F *fired_rel0_didntfire_rel1;
		TH1F *fired_rel1_didntfire_rel0;
		TH1F *trigs_all_diffs;

		TH1F *fired_rel0_didntfire_rel1_L1_ID;
		TH1F *fired_rel1_didntfire_rel0_L1_ID;

		TH1F *prescales_hist;
		
		TH1F *fired_rel0_didntfire_rel1_prescale_value;
		TH1F *fired_rel1_didntfire_rel0_prescale_value;

		TH1F *events_where_fired_rel0_didntfire_rel1;
		TH1F *events_where_fired_rel1_didntfire_rel0;
		TH1F *events_all_diffs;

		TH1F *gen_ID_objs_where_fired_rel0_didntfire_rel1;
		TH1F *gen_ID_objs_where_fired_rel1_didntfire_rel0;

		TH2F *event_path0;
		TH2F *event_path1;	

		//TH2F *last_mod_index0;
		//TH2F *last_mod_index1;

		TH1F *howmany;

		TH1F *fired_rel0_didntfire_rel1_wasL1_fail;
		TH1F *fired_rel1_didntfire_rel0_wasL1_fail;

		TH1F *one_event_datasets;

		TH1F *path_accept0_datasets;
		TH1F *path_accept1_datasets;	    
		TH1F *fired_rel0_didntfire_rel1_datasets;
		TH1F *fired_rel1_didntfire_rel0_datasets;
		
		TH2F *path_accept0_decay_type_vs_datasets;
		TH2F *path_accept1_decay_type_vs_datasets;
		TH2F *fired_rel0_didntfire_rel1_decay_type_vs_datasets;
		TH2F *fired_rel1_didntfire_rel0_decay_type_vs_datasets;

		TH1F *last_mod_type_fired_rel0_didntfire_rel1;
		TH1F *last_mod_type_fired_rel1_didntfire_rel0;
		
		TH2F *diff;
		//TH2F *diff2;
		
		
		
	public:
		
		Compare_GenHLTOffline();
		~Compare_GenHLTOffline();
		
		void compare_genhltoffline();
		void load_files(TString file1_arg = "file1.root", TString file2_arg = "file2.root", TString rel0_lbl_arg = "label for file 1", TString rel1_lbl_arg = "label for file 2");
		
		void getprescales(TH1F *prescales);  // <- temp until better way found
		
		void book_histograms();
		void table_dump();
		void new_file(TString save_file_name = "new_file.root");
		void end_job();
		
		
};

Compare_GenHLTOffline::~Compare_GenHLTOffline() {}	

Compare_GenHLTOffline::Compare_GenHLTOffline()
{

  TString makeshared(gSystem->GetMakeSharedLib());
  TString dummy = makeshared.ReplaceAll("-W ", "");
  gSystem->SetMakeSharedLib(makeshared);
  TString dummy = makeshared.ReplaceAll("-Wshadow ", "-std=c++0x ");
  gSystem->SetMakeSharedLib(makeshared);
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libDataFormatsCommon.so");
  
  TString path = gSystem->GetIncludePath();
  path.Append(" -I$ROOTSYS/include/root -I./include  ");
  gSystem->SetIncludePath(path);
  







	
	verbosity = 1; // 0 = silent, 1 = moderate, 2 = debug
	
	
	// look at particular mc truth decay:
	
	decay_type_study = 6;
	
	// double ele: 0
	// double mu: 1
	// double tau: 2
	// ele + mu: 3
	// ele + tau: 4
	// mu + tau: 5
	// single ele:6 
	// single mu: 7
	// single tau: 8
	// all had: 9
	// other: 10
	
	
	// look at losses/gains from a particular dataset (this number = bin-1 from y-axis of dataset_map):
	dataset_study = 33;
	
	
	// additional options:
       	dump_evt_info = false;		// dump table of information for selected events
       	use_prescale = false;		// only look at prescaled triggers
       	use_last_mod_run_type = false;	// turn off if not present in trees (historical)
	
}

void Compare_GenHLTOffline::compare_genhltoffline()
{

	// first: load input files (done from script)

        ofstream outputDiff;
        outputDiff.open("trigs_all_diffs.csv");

        histfile = new TFile(new_file_name,"RECREATE","testfile");
	
	book_histograms();

	getprescales(prescales_hist);

	
	std::string path_index_to_name[750];
	
	int num_double_ele_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_double_mu_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_double_tau_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_ele_mu_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_ele_tau_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_mu_tau_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_single_ele_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_single_mu_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_single_tau_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_all_had_evts_where_fired_rel0_didntfire_rel1 = 0;
	int num_other_evts_where_fired_rel0_didntfire_rel1 = 0;
	
	
	int num_double_ele_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_double_mu_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_double_tau_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_ele_mu_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_ele_tau_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_mu_tau_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_single_ele_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_single_mu_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_single_tau_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_all_had_evts_where_fired_rel1_didntfire_rel0 = 0;
	int num_other_evts_where_fired_rel1_didntfire_rel0 = 0;
	map <string,int> name_idx_map;

	event_subset_lost_hits.push_back(-1);
	event_subset_gained_hits.push_back(-1);
	
	for (int evt=1;evt<=events0; evt++){
	  for (int init_it2=0; init_it2<sanity_ck1+1; init_it2++)
	    {
	      path_index_to_name[init_it2] = "init";
	      tree[1]->GetEntry(get_entry_from_evt_trig_tree1[evt][init_it2]);
	      string pn = string(tree_path_name[1]);
	      pn = pn.substr(0,pn.find_last_of("\\_v"));
	      name_idx_map[pn] = init_it2;
	      //cout << "TREE0 = " << tree_path_name[0] << " TREE1 = " << tree_path_name[1] << endl;
	    }
	}

	if (verbosity>1) cout << "Before event loop" << endl;
	
	for (int Event=1; Event<=events0; Event++)
	{		
		bool happened_already_this_evt = false;
		bool happened_already_this_evt_a = false;
		bool happened_already_this_evt_b = false;
		int yaxis_bin_decay_type_vs_datasets = -1;
		
		for (int trigger=0; trigger<(sanity_ck1+1); trigger++) // num_trigs
		{
			
			if ((Event%1000==0)&&(trigger==50)&&(verbosity))
			{
				cout << "Second loop. On Event " << Event << " out of " << events0 << endl;
			}			
						
			
			if (get_entry_from_evt_trig_tree0[Event][trigger]==-1) continue;
			
			tree[0]->GetEntry(get_entry_from_evt_trig_tree0[Event][trigger]);
			string pn0 = string(tree_path_name[0]);
			pn0 = pn0.substr(0,pn0.find_last_of("\\_v"));
			tree[1]->GetEntry(get_entry_from_evt_trig_tree1[Event][name_idx_map[pn0]]); //anywhere first tree is referenced, replace trigger with name_idx_map[pno]
			//now the paths matchup 
			gentree[0]->GetEntry(get_entry_from_evt_gen_tree0[Event]);
			gentree[1]->GetEntry(get_entry_from_evt_gen_tree0[Event]);
			//cout << "PATH NAMES " << tree_path_name[0] << "|||||" << tree_path_name[1] <<endl;

			
			if (Event==50&&verbosity)
			{
				cout << "Trigger index: " << trigger << ", trigger path name, rel0: " << tree_path_name[0] << ", trigger path name, rel1: " << tree_path_name[1] << endl;				
				path_index_to_name[trigger] = string(tree_path_name[0]);
			}
			
			if (verbosity>1) cout << "Inside event loop, event " << Event << ", path index " << trigger << endl;
			
			
			int prsc = prescales_hist->GetBinContent(prescales_hist->GetXaxis()->FindBin(tree_path_name[0])); // <- Gets prescale by trigger name
			float weight = 1.;

			if (use_prescale)
			{
				/*
				if (prsc==0) weight = 0.;
				if (prsc>0) weight = 1. / (float)prsc;
				*/
				// jason:
				if (prsc==1) weight = 1.;
				if (prsc!=1) weight = 0.;
			}
			
			
			if (!happened_already_this_evt)
			  {
			    gentree[0]->GetEntry(get_entry_from_evt_gen_tree0[Event]);
			    gentree[1]->GetEntry(get_entry_from_evt_gen_tree0[Event]);
			    //if (fabs(gen_tree_gen_pt[0][6]) != fabs(gen_tree_gen_pt[1][6]))                                                                                           
			    //{
				//cout << "ID0 = " << gen_tree_gen_ID[0][6] << "  ID1 = " << gen_tree_gen_ID[1][6] << endl; 
				//cout << "top pt 0 = " << gen_tree_gen_pt[0][6] << " top pt 1 = " << gen_tree_gen_pt[1][6] << endl;
				//continue;
			    //}
			    
			    //DOUBLE CHECK LOOP                                                                                                                                                                                                                                   
			    cout << "ID, pt, eta, phi of gen particles (status 3) in this event:" << endl;
			    for (int genobjs=0; genobjs<100; genobjs++)
			      {
				if ((gen_tree_gen_ID[0][genobjs]!=-99)&&fabs(gen_tree_gen_ID[0][genobjs])>0)
				  {
				    cout << "ID0: " << gen_tree_gen_ID[0][genobjs] << ", pt: " << gen_tree_gen_pt[0][genobjs] << ", eta: " << gen_tree_gen_eta[0][genobjs] << ", phi: " << gen_tree_gen_phi[0][genobjs] << endl;
				    cout << "ID1: " << gen_tree_gen_ID[1][genobjs] << ", pt: " << gen_tree_gen_pt[1][genobjs] << ", eta: " << gen_tree_gen_eta[1][genobjs] << ", phi: " << gen_tree_gen_phi[1][genobjs] << endl;
				  }
			      }
			    ///END DOUBLE CHECK LOOP              			    
			    


				int num_muons_ = 0;
				int num_electrons_ = 0;
				int num_taus_ = 0;


				for (int genobjs=0; genobjs<100; genobjs++)
				{						
					if (fabs(gen_tree_gen_ID[0][genobjs])==11) num_electrons_++;
					if (fabs(gen_tree_gen_ID[0][genobjs])==13) num_muons_++;
					if (fabs(gen_tree_gen_ID[0][genobjs])==15) num_taus_++;
				}

				happened_already_this_evt = true;

				if (num_electrons_==2) { yaxis_bin_decay_type_vs_datasets = 0; }
				else if (num_muons_==2) { yaxis_bin_decay_type_vs_datasets = 1; }
				else if (num_taus_==2) { yaxis_bin_decay_type_vs_datasets = 2; }
				else if (num_electrons_==1&&num_muons_==1) {yaxis_bin_decay_type_vs_datasets = 3; }
				else if (num_electrons_==1&&num_taus_==1) { yaxis_bin_decay_type_vs_datasets = 4; }
				else if (num_muons_==1&&num_taus_==1) { yaxis_bin_decay_type_vs_datasets = 5; }
				else if (num_electrons_==1&&((num_muons_+num_taus_)==0)) { yaxis_bin_decay_type_vs_datasets = 6; }
				else if (num_muons_==1&&((num_electrons_+num_taus_)==0)) { yaxis_bin_decay_type_vs_datasets = 7; }
				else if (num_taus_==1&&((num_muons_+num_electrons_)==0)) { yaxis_bin_decay_type_vs_datasets = 8; }
				else if ((num_muons_+num_electrons_+num_taus_)==0) { yaxis_bin_decay_type_vs_datasets = 9; }
				else
				{
					yaxis_bin_decay_type_vs_datasets = -1;
				}

			}
			
			if (tree_path_accept[0])
			{
				event_path0->Fill(trigger,Event);
				path_accept0->Fill(trigger,weight);
				
				for (int datasets=0; datasets<50; datasets++) 
				{
					if (dataset_map->GetBinContent(trigger+1,datasets+1))
					{
						path_accept0_datasets->Fill(datasets,weight);
						path_accept0_decay_type_vs_datasets->Fill(datasets,yaxis_bin_decay_type_vs_datasets,weight);
					}
				}
							
				
				if (!tree_path_accept[1])
				{
					if (verbosity>1) cout << "path " << trigger << " fired in first release but not second." << endl;
					
					/*
					int prsc = prescales_hist->GetBinContent(prescales_hist->GetXaxis()->FindBin(tree_path_name[0])); // <- Gets prescale by trigger name
					float weight = 1.;
					
					if (use_prescale)
					{
						
						if (prsc==0) weight = 0.;
						if (prsc>0) weight = 1. / (float)prsc;
						
						// jason:
						if (prsc==1) weight = 1.;
						if (prsc!=1) weight = 0.;
					}
					*/
					
					fired_rel0_didntfire_rel1->Fill(trigger,weight);
					trigs_all_diffs->Fill(trigger,weight);
					events_where_fired_rel0_didntfire_rel1->Fill(Event,weight);
					events_all_diffs->Fill(Event,weight);
					
					
					fired_rel0_didntfire_rel1_prescale_value->Fill(prsc);
					
					
					if (!happened_already_this_evt_a)
					{
						gentree[0]->GetEntry(get_entry_from_evt_gen_tree0[Event]);

						int num_muons = 0;
						int num_electrons = 0;
						int num_taus = 0;


						for (int genobjs=0; genobjs<100; genobjs++)
						{						
							gen_ID_objs_where_fired_rel0_didntfire_rel1->Fill(gen_tree_gen_ID[0][genobjs]);
							
							if (fabs(gen_tree_gen_ID[0][genobjs])==11) num_electrons++;
							if (fabs(gen_tree_gen_ID[0][genobjs])==13) num_muons++;
							if (fabs(gen_tree_gen_ID[0][genobjs])==15) num_taus++;
							
						}
						
						happened_already_this_evt_a = true;
						
						if (num_electrons==2) { num_double_ele_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 0; }
						else if (num_muons==2) { num_double_mu_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 1; }
						else if (num_taus==2) { num_double_tau_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 2; }
						else if (num_electrons==1&&num_muons==1) { num_ele_mu_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 3; }
						else if (num_electrons==1&&num_taus==1) { num_ele_tau_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 4; }
						else if (num_muons==1&&num_taus==1) { num_mu_tau_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 5; }
						else if (num_electrons==1&&((num_muons+num_taus)==0)) { num_single_ele_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 6; }
						else if (num_muons==1&&((num_electrons+num_taus)==0)) { num_single_mu_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 7; }
						else if (num_taus==1&&((num_muons+num_electrons)==0)) { num_single_tau_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 8; }
						else if ((num_muons+num_electrons+num_taus)==0) { num_all_had_evts_where_fired_rel0_didntfire_rel1++; yaxis_bin_decay_type_vs_datasets = 9; }
						else
						{
							num_other_evts_where_fired_rel0_didntfire_rel1++;
							yaxis_bin_decay_type_vs_datasets = -1;
							if (verbosity>1) cout << "num electrons: " << num_electrons << ",  num muons: " << num_muons << ",  num taus: " << num_taus << endl;
						}
						
					}
					
					
					for (int datasets=0; datasets<50; datasets++)
					{
						if (dataset_map->GetBinContent(trigger+1,datasets+1))
						{
							fired_rel0_didntfire_rel1_datasets->Fill(datasets,weight);
							fired_rel0_didntfire_rel1_decay_type_vs_datasets->Fill(datasets,yaxis_bin_decay_type_vs_datasets,weight);
							if (datasets==dataset_study&&yaxis_bin_decay_type_vs_datasets==decay_type_study) if (event_subset_lost_hits[event_subset_lost_hits.size()-1]!=Event) event_subset_lost_hits.push_back(Event);
						}
					}
					
					
					if (use_last_mod_run_type)
					{
						int thebin = last_mod_type_fired_rel0_didntfire_rel1->GetXaxis()->FindBin(tree_last_module_run_type[1]);
						last_mod_type_fired_rel0_didntfire_rel1->Fill(thebin-1,weight);
					}
					
					//if (tree_last_module_run_index[1]==tree_L1_module_with_saved_tags_index[1]) fired_rel0_didntfire_rel1_wasL1_fail->Fill(trigger);
					
					int thing1a = tree_last_module_run_index[1];
					int thing2a = tree_L1_module_with_saved_tags_index[1];
					
					if (thing1a==thing2a) fired_rel0_didntfire_rel1_wasL1_fail->Fill(trigger);
					
					for (int id_int=0; id_int<100; id_int++)
					{
						fired_rel0_didntfire_rel1_L1_ID->Fill(tree_L1_module_with_saved_tags_obj_ID[0][id_int]);
					}
				
				}
			}
			if (tree_path_accept[1])
			  {
			    event_path1->Fill(name_idx_map[pn0],Event);
			    path_accept1->Fill(name_idx_map[pn0],weight);
			    
			    for (int datasets=0; datasets<50; datasets++) 
			      {
				if (dataset_map->GetBinContent(name_idx_map[pn0]+1,datasets+1))
				  {
				    path_accept1_datasets->Fill(datasets,weight);
				    path_accept1_decay_type_vs_datasets->Fill(datasets,yaxis_bin_decay_type_vs_datasets,weight);
				  }
			      }
			    
			    if (!tree_path_accept[0])
			      {
					if (verbosity>1) cout << "path " << trigger << " fired in second release but not first." << endl;
					/*
					int prsc = prescales_hist->GetBinContent(prescales_hist->GetXaxis()->FindBin(tree_path_name[0])); // <- Gets prescale by trigger name
					float weight = 1.;
					
					if (use_prescale)
					{
						//if (prsc==0) weight = 0;
						if (prsc==1) weight = 1.;
						if (prsc!=1) weight = 0.;  // <-- jason    1. / (float)prsc;
					}
					*/

					//if (tree_last_module_run_index[0]==tree_L1_module_with_saved_tags_index[0]) fired_rel1_didntfire_rel0_wasL1_fail->Fill(trigger);
					int thing1b = tree_last_module_run_index[0];
					int thing2b = tree_L1_module_with_saved_tags_index[0];
					
					if (thing1b==thing2b) fired_rel1_didntfire_rel0_wasL1_fail->Fill(name_idx_map[pn0]);
					
					         
					fired_rel1_didntfire_rel0->Fill(name_idx_map[pn0],weight);
					trigs_all_diffs->Fill(name_idx_map[pn0],weight);
					events_where_fired_rel1_didntfire_rel0->Fill(Event,weight);
					events_all_diffs->Fill(Event,weight);
					
					fired_rel1_didntfire_rel0_prescale_value->Fill(prsc);
					
					
					if (!happened_already_this_evt_b)
					{
					
						gentree[0]->GetEntry(get_entry_from_evt_gen_tree0[Event]);

						int num_muons = 0;
						int num_electrons = 0;
						int num_taus = 0;

						for (int genobjs=0; genobjs<100; genobjs++)
						{						
							gen_ID_objs_where_fired_rel1_didntfire_rel0->Fill(gen_tree_gen_ID[0][genobjs]);
							
							if (fabs(gen_tree_gen_ID[0][genobjs])==11) num_electrons++;
							if (fabs(gen_tree_gen_ID[0][genobjs])==13) num_muons++;
							if (fabs(gen_tree_gen_ID[0][genobjs])==15) num_taus++;
							
						}
					
						happened_already_this_evt_b = true;
						
						if (num_electrons==2) { num_double_ele_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 0; }
						else if (num_muons==2) { num_double_mu_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 1; }
						else if (num_taus==2) { num_double_tau_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 2; }
						else if (num_electrons==1&&num_muons==1) { num_ele_mu_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 3; }
						else if (num_electrons==1&&num_taus==1) { num_ele_tau_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 4; }
						else if (num_muons==1&&num_taus==1) { num_mu_tau_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 5; }
						else if (num_electrons==1&&((num_muons+num_taus)==0)) { num_single_ele_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 6; }
						else if (num_muons==1&&((num_electrons+num_taus)==0)) { num_single_mu_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 7; }
						else if (num_taus==1&&((num_muons+num_electrons)==0)) { num_single_tau_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 8; }
						else if ((num_muons+num_electrons+num_taus)==0) { num_all_had_evts_where_fired_rel1_didntfire_rel0++; yaxis_bin_decay_type_vs_datasets = 9; }
						else 
						{
							num_other_evts_where_fired_rel1_didntfire_rel0++;
							yaxis_bin_decay_type_vs_datasets = -1;
							if (verbosity>1) cout << "num electrons: " << num_electrons << ",  num muons: " << num_muons << ",  num taus: " << num_taus << endl;
						}

					}
					
					for (int datasets=0; datasets<50; datasets++)
					{
						if (dataset_map->GetBinContent(name_idx_map[pn0]+1,datasets+1))
						{
							fired_rel1_didntfire_rel0_datasets->Fill(datasets,weight);
							fired_rel1_didntfire_rel0_decay_type_vs_datasets->Fill(datasets,yaxis_bin_decay_type_vs_datasets,weight);
							if (datasets==dataset_study&&yaxis_bin_decay_type_vs_datasets==decay_type_study) if (event_subset_gained_hits[event_subset_gained_hits.size()-1]!=Event) event_subset_gained_hits.push_back(Event);
						}
					}
					
					if (use_last_mod_run_type)
					{
						int thebin = last_mod_type_fired_rel1_didntfire_rel0->GetXaxis()->FindBin(tree_last_module_run_type[0]);
						last_mod_type_fired_rel1_didntfire_rel0->Fill(thebin-1,weight);
					}
				}
			}			
			
			//last_mod_index0->Fill(trigger,Event,tree_last_module_run_index[0]);			
			//last_mod_index1->Fill(trigger,Event,tree_last_module_run_index[1]);
			
	
			
			
			
			bool one_has_obj = true;
			
			int count = 0;
			
			while (one_has_obj)
			{
				double pt0 = tree_last_module_with_saved_tags_obj_pt[0][count];
				double pt1 = tree_last_module_with_saved_tags_obj_pt[1][count];
				
				/*
				if (pt0!=pt1)
				{
					cout << "Found object difference: Event " << Event << ", Trigger " << trigger << ", object " << count << ". " << endl;
					cout << "----> object from first release has pt " << pt0 << ", object from second release has pt " << pt1 << endl;
				}
				*/
				
				count++;
				
				if (pt0==-99&&pt1==-99) one_has_obj = false;
				
				if (count>99) break;
			}			
			
			howmany->Fill(count);	
	
		}
	}
	
	
	diff = (TH2F*)event_path1->Clone("diff");	
	diff->Add(event_path0,-1);
	diff->SetTitle("Event vs. Path: " + rel1_lbl + " - " + rel0_lbl);
	
	abs_path_accept_diff = (TH1F*)path_accept1->Clone("abs_path_accept_diff");
	abs_path_accept_diff->Add(path_accept0,-1);
	abs_path_accept_diff->GetXaxis()->SetTitle("path index");
	abs_path_accept_diff->GetYaxis()->SetTitle("count difference");	
	abs_path_accept_diff->SetTitle("total counts (" + rel1_lbl + ") - total counts (" + rel0_lbl + ")");
	
	
	
	if (verbosity)
	{	
		
		
		
		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of : path_accept0" << endl;
		cout << "  " << endl;

		int path_accept0_count = 0;

		for (int bin=0; bin<750; bin++)
		{
			if (fabs(path_accept0->GetBinContent(bin+1))>0)
			{
				cout << path_index_to_name[bin] << ":  " << path_accept0->GetBinContent(bin+1) << endl;
				path_accept0_count++;
			}

		}
	
		
		cout << "total number of paths in path_accept0: " << path_accept0_count << endl;
		
		
		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of fired_rel0_didntfire_rel1: " << endl;
		cout << "  " << endl;

		int rel0rel1_trigcount = 0;

		for (int bin=0; bin<750; bin++)
		{
			if (fabs(fired_rel0_didntfire_rel1->GetBinContent(bin+1))>0)
			{
				cout << path_index_to_name[bin] << ":  " << fired_rel0_didntfire_rel1->GetBinContent(bin+1) << endl;
				rel0rel1_trigcount++;
			}

		}

		cout << "total number of paths in fired_rel0_didntfire_rel1: " << rel0rel1_trigcount << endl;

		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of fired_rel1_didntfire_rel0: " << endl;
		cout << "  " << endl;

		int rel1rel0_trigcount = 0;

		for (int bin=0; bin<750; bin++)
		{
			if (fabs(fired_rel1_didntfire_rel0->GetBinContent(bin+1))>0)
			{
				cout << path_index_to_name[bin] << ":  " << fired_rel1_didntfire_rel0->GetBinContent(bin+1) << endl;
				rel1rel0_trigcount++;
			}


		}

		cout << "total number of paths in fired_rel1_didntfire_rel0: " << rel1rel0_trigcount << endl;


		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of trigs_all_diffs: " << endl;
		cout << "  " << endl;

		int all_trigcount = 0;


		for (int bin=0; bin<750; bin++)
		{
			if (fabs(trigs_all_diffs->GetBinContent(bin+1))>0)
			{
			  outputDiff << path_index_to_name[bin] << "," << trigs_all_diffs->GetBinContent(bin+1) << endl;
			  cout << path_index_to_name[bin] << ":  " << trigs_all_diffs->GetBinContent(bin+1) << endl;
			  all_trigcount++;
			}
			
			
		}

		cout << "total number of paths in trigs_all_diffs: " << all_trigcount << endl;




		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of abs_path_accept_diff: " << endl;
		cout << "  " << endl;


		for (int bin=0; bin<750; bin++)
		{
			if (fabs(abs_path_accept_diff->GetBinContent(bin+1))>0) cout << path_index_to_name[bin] << ":  " << abs_path_accept_diff->GetBinContent(bin+1) << endl;

		}


		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of events_where_fired_rel0_didntfire_rel1: " << endl;
		cout << "  " << endl;

		int rel0rel1_evtcount = 0;

		for (int bin=0; bin<10000; bin++)
		{
			if (fabs(events_where_fired_rel0_didntfire_rel1->GetBinContent(bin+1))>0)
			{
				cout << "event " << bin << " paths: " <<  events_where_fired_rel0_didntfire_rel1->GetBinContent(bin+1) << endl;
				rel0rel1_evtcount++;
			}
		}

		cout << "total num of events with content in events_where_fired_rel0_didntfire_rel1: " << rel0rel1_evtcount << endl;


		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of events_where_fired_rel1_didntfire_rel0: " << endl;
		cout << "  " << endl;

		int rel1rel0_evtcount = 0;

		for (int bin=0; bin<10000; bin++)
		{

			if (fabs(events_where_fired_rel1_didntfire_rel0->GetBinContent(bin+1))>0)
			{
				cout << "event " << bin << " paths: " <<  events_where_fired_rel1_didntfire_rel0->GetBinContent(bin+1) << endl;
				rel1rel0_evtcount++;
			}
		}

		cout << "total num of events with content in events_where_fired_rel1_didntfire_rel0: " << rel1rel0_evtcount << endl;



		cout << "  " << endl;
		cout << "  " << endl;
		cout << "contents of events_all_diffs: " << endl;
		cout << "  " << endl;

		int tot_evtcount = 0;

		for (int bin=0; bin<10000; bin++)
		{

			if (fabs(events_all_diffs->GetBinContent(bin+1))>0)
			{
				cout << "event " << bin << " paths: " <<  events_all_diffs->GetBinContent(bin+1) << endl;
				tot_evtcount++;
			}
		}

		cout << "total num of events with content in events_all_diffs: " << tot_evtcount << endl;


		cout << "  " << endl;
		cout << "events that lost hits; what type of event was it: " << endl;
		cout << "double ele: " << num_double_ele_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "double mu: " << num_double_mu_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "double tau: " << num_double_tau_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "ele + mu: " << num_ele_mu_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "ele + tau: " << num_ele_tau_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "mu + tau: " << num_mu_tau_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "single ele: " << num_single_ele_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "single mu: " << num_single_mu_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "single tau: " << num_single_tau_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "all had: " << num_all_had_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "other: " << num_other_evts_where_fired_rel0_didntfire_rel1 << endl;
		cout << "  " << endl;


		cout << "  " << endl;
		cout << "events that gained hits; what type of event was it: " << endl;
		cout << "double ele: " << num_double_ele_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "double mu: " << num_double_mu_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "double tau: " << num_double_tau_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "ele + mu: " << num_ele_mu_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "ele + tau: " << num_ele_tau_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "mu + tau: " << num_mu_tau_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "single ele: " << num_single_ele_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "single mu: " << num_single_mu_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "single tau: " << num_single_tau_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "all had: " << num_all_had_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "other: " << num_other_evts_where_fired_rel1_didntfire_rel0 << endl;
		cout << "  " << endl;


	}
	
		
	
	//print out table of formatted info about specific triggers, events:
	if (dump_evt_info) table_dump();
		
	//close output histo file:	
	end_job();

}




void Compare_GenHLTOffline::table_dump()
{


  /*
  for (int evt=1;evt<=events0; evt++){
    for (int init_it2=0; init_it2<sanity_ck1+1; init_it2++)
      {
	path_index_to_name[init_it2] = "init";
	tree[1]->GetEntry(get_entry_from_evt_trig_tree1[evt][init_it2]);
	string pn = string(tree_path_name[1]);
	pn = pn.substr(0,pn.find_last_of("\\_v"));
	name_idx_map[pn] = init_it2;
	//cout << "TREE0 = " << tree_path_name[0] << " TREE1 = " << tree_path_name[1] << endl;                                                                                                                                                                          
      }
  }
  
  */

//	int Event = 8153; // interesting event	
	
	int evt_subset_size = event_subset_lost_hits.size();	// now get handfull of events
	
	if (evt_subset_size>=2)
	{
		for (int Event_i=1; Event_i<evt_subset_size; Event_i++)
		{
			
			int Event = event_subset_lost_hits[Event_i];
			
			cout << "  " << endl;
			cout << "  " << endl;
			cout << "  " << endl;
			cout << "  " << endl;
			cout << "|| ------------------------------------------- ||" << endl;
			cout << "|| dumping info for event " << Event << "                 ||" << endl;   
			cout << "|| ------------------------------------------- ||" << endl;
			cout << "  " << endl;

			gentree[0]->GetEntry(get_entry_from_evt_gen_tree0[Event]);

			cout << "ID, pt, eta, phi of gen particles (status 3) in this event:" << endl;
			
			for (int genobjs=0; genobjs<100; genobjs++)
			{	
				if ((gen_tree_gen_ID[0][genobjs]!=-99)&&fabs(gen_tree_gen_ID[0][genobjs])>0)
				{
					
					cout << "ID: " << gen_tree_gen_ID[0][genobjs] << ", pt: " << gen_tree_gen_pt[0][genobjs] << ", eta: " << gen_tree_gen_eta[0][genobjs] << ", phi: " << gen_tree_gen_phi[0][genobjs] << endl;
				}
			}
				
			cout << "  " << endl;
			cout << "  " << endl;

			printf("%-10s %-100s %-5s %-5s %-130s %-110s %-130s %-10s %-30s %-30s %-30s %-10s %-30s %-30s %-30s \n", "path index", "name", "pass", "error", "label of last run module", "label of the last module in the path that had saved tags", "label of the module that had L1 saved tags", "L1 obj ID", "L1 object pt", "L1 object eta", "L1 object phi", "last saved tags ojbect ID", "last saved tags object pt", "last saved tags object eta", "last saved tags object phi");

			cout << "  " << endl;


			// dump some selected info, for both releases (1 event)



			string dashes = "";
			for (int str_it=0; str_it<690; str_it++)
			{
				dashes.append("-");
			}

			for (int trigger=0; trigger<(sanity_ck1+1); trigger++) //
			{
			  
			  tree[0]->GetEntry(get_entry_from_evt_trig_tree0[Event][trigger]);
			  /*string pn0 = string(tree_path_name[0]);
			    pn0 = pn0.substr(0,pn0.find_last_of("\\_v"));
			    tree[1]->GetEntry(get_entry_from_evt_trig_tree1[Event][name_idx_map[pn0]]);*/
			  tree[1]->GetEntry(get_entry_from_evt_trig_tree1[Event][trigger]);
			  
			  /*
				if (tree_path_accept[0])
				{

					for (int datasets=0; datasets<50; datasets++)
					{
						if (dataset_map->GetBinContent(trigger+1,datasets+1)) one_event_datasets->Fill(datasets);					

					}

				}
				*/

				if (dataset_map->GetBinContent(trigger+1,dataset_study+1))				// tree_path_accept[0]&&(!tree_path_accept[1]))  // tree_path_accept[0]&&(!tree_path_accept[1])
				{

					int prsc = prescales_hist->GetBinContent(prescales_hist->GetXaxis()->FindBin(tree_path_name[0])); // <- Gets prescale by trigger name
					float weight = 1.;
					
					if (use_prescale)
					{
						//if (prsc==0) weight = 0;
						if (prsc==1) weight = 1.;
						if (prsc!=1) weight = 0.; // <-- jason   // weight = 1. / (float)prsc;
					}
					
					for (int datasets=0; datasets<50; datasets++)
					{
						if (dataset_map->GetBinContent(trigger+1,datasets+1)) one_event_datasets->Fill(datasets,weight);					

					}


					cout << " " << endl;
					cout << dashes << endl;
					cout << " " << endl;

					for (int sample=0; sample<2; sample++)
					{

						int obj_it = 0;

						bool at_least_one_obj = false;

						if (!((tree_last_module_with_saved_tags_obj_pt[sample][0]==-99.)&&(tree_L1_module_with_saved_tags_obj_pt[sample][0]==-99.)))
						{
							at_least_one_obj = true;
						}

						string name(tree_path_name[sample]);
						string last_mod_run_lbl(tree_last_module_run_label[sample]);	
						string last_mod_with_saved_tags_lbl(tree_last_module_with_saved_tags_label[sample]);
						string L1_mod_with_saved_tags_lbl(tree_L1_module_with_saved_tags_label[sample]);


						if (at_least_one_obj)
						{
							printf("%-10i %-100s %-5i %-5i %-130s %-110s %-130s %-10i %-30f %-30f %-30f %-10i %-30f %-30f %-30f \n",tree_path_index[sample],name.c_str(),tree_path_accept[sample],tree_path_error[sample],last_mod_run_lbl.c_str(),last_mod_with_saved_tags_lbl.c_str(),L1_mod_with_saved_tags_lbl.c_str(),tree_L1_module_with_saved_tags_obj_ID[sample][0],tree_L1_module_with_saved_tags_obj_pt[sample][0],tree_L1_module_with_saved_tags_obj_eta[sample][0],tree_L1_module_with_saved_tags_obj_phi[sample][0],tree_last_module_with_saved_tags_obj_ID[sample][0],tree_last_module_with_saved_tags_obj_pt[sample][0],tree_last_module_with_saved_tags_obj_eta[sample][0],tree_last_module_with_saved_tags_obj_phi[sample][0]);

						}



						if (!at_least_one_obj)
						{
							printf("%-10i %-100s %-5i %-5i %-130s %-110s %-130s %-10s %-30s %-30s %-30s %-10s %-30s %-30s %-30s \n",tree_path_index[sample],name.c_str(),tree_path_accept[sample],tree_path_error[sample],last_mod_run_lbl.c_str(),last_mod_with_saved_tags_lbl.c_str(),L1_mod_with_saved_tags_lbl.c_str(),"--","--","--","--","--","--","--","--");
						}

						obj_it++;


						// cout << "not in while loop: " << obj_it << endl;

						while (obj_it<100)
						{
							if (!((tree_last_module_with_saved_tags_obj_pt[sample][obj_it]==-99.)&&(tree_L1_module_with_saved_tags_obj_pt[sample][obj_it]==-99.)))
							{

								printf("%-10s %-100s %-5s %-5s %-130s %-110s %-130s %-10i %-30f %-30f %-30f %-10i %-30f %-30f %-30f \n"," "," "," "," "," "," "," ",tree_L1_module_with_saved_tags_obj_ID[sample][obj_it],tree_L1_module_with_saved_tags_obj_pt[sample][obj_it],tree_L1_module_with_saved_tags_obj_eta[sample][obj_it],tree_L1_module_with_saved_tags_obj_phi[sample][obj_it],tree_last_module_with_saved_tags_obj_ID[sample][obj_it],tree_last_module_with_saved_tags_obj_pt[sample][obj_it],tree_last_module_with_saved_tags_obj_eta[sample][obj_it],tree_last_module_with_saved_tags_obj_phi[sample][obj_it]);

							}

							obj_it++;

							// cout << "in while loop: " << obj_it << endl;
						}

						cout << " " << endl;

					}

				}


			}

		}

	
	}



}
void Compare_GenHLTOffline::load_files(TString file1_arg, TString file2_arg, TString rel0_lbl_arg, TString rel1_lbl_arg)
{
	
	file1 = file1_arg;
	file2 = file2_arg;
	rel0_lbl = rel0_lbl_arg;
	rel1_lbl = rel1_lbl_arg;
	
	fn1 = new TFile(file1);
	fn2 = new TFile(file2);

	
	//tree[0] = (TTree*)fn1->Get("genparticles/the_tree");
	//tree[1] = (TTree*)fn2->Get("genparticles/the_tree");
	
	
	tree[0] = (TTree*)fn1->Get("genparticles/the_HLT_tree");
	tree[1] = (TTree*)fn2->Get("genparticles/the_HLT_tree");
	
	
	
	
	dataset_map = (TH2D*)fn1->Get("newHLTOffline/map_of_trig_to_trig_types");
	//dataset_map[1] = (TH2D*)fn2->Get("newHLTOffline/map_of_trig_to_trig_types");
	
	
	
	gentree[0] = (TTree*)fn1->Get("genparticles/the_gen_tree");
	gentree[1] = (TTree*)fn2->Get("genparticles/the_gen_tree");
	
	
	for (int i=0; i<2; i++) // comparing 2 samples
	{

//	int i=0;
				
		tree[i]->SetBranchAddress("path_name",tree_path_name[i]);

		tree[i]->SetBranchAddress("event",&tree_event[i]);
		tree[i]->SetBranchAddress("path_index",&tree_path_index[i]);
		tree[i]->SetBranchAddress("path_was_run",&tree_path_was_run[i]);
		tree[i]->SetBranchAddress("path_error",&tree_path_error[i]);
		tree[i]->SetBranchAddress("path_accept",&tree_path_accept[i]);


		tree[i]->SetBranchAddress("last_module_run_label",tree_last_module_run_label[i]);
		tree[i]->SetBranchAddress("last_module_run_index",&tree_last_module_run_index[i]);

		tree[i]->SetBranchAddress("last_module_with_saved_tags_label",tree_last_module_with_saved_tags_label[i]);
		tree[i]->SetBranchAddress("last_module_with_saved_tags_index",&tree_last_module_with_saved_tags_index[i]);

		tree[i]->SetBranchAddress("L1_module_with_saved_tags_label",tree_L1_module_with_saved_tags_label[i]);
		tree[i]->SetBranchAddress("L1_module_with_saved_tags_index",&tree_L1_module_with_saved_tags_index[i]);
		
		if (use_last_mod_run_type) tree[i]->SetBranchAddress("last_module_run_type",tree_last_module_run_type[i]);

		tree[i]->SetBranchAddress("L1_module_with_saved_tags_object_obj_ID",&tree_L1_module_with_saved_tags_obj_ID[i]);
		tree[i]->SetBranchAddress("L1_module_with_saved_tags_object_obj_pt",&tree_L1_module_with_saved_tags_obj_pt[i]);
		tree[i]->SetBranchAddress("L1_module_with_saved_tags_object_obj_eta",&tree_L1_module_with_saved_tags_obj_eta[i]);
		tree[i]->SetBranchAddress("L1_module_with_saved_tags_object_obj_phi",&tree_L1_module_with_saved_tags_obj_phi[i]);

		tree[i]->SetBranchAddress("last_module_with_saved_tags_object_obj_ID",&tree_last_module_with_saved_tags_obj_ID[i]);
		tree[i]->SetBranchAddress("last_module_with_saved_tags_object_obj_pt",&tree_last_module_with_saved_tags_obj_pt[i]);
		tree[i]->SetBranchAddress("last_module_with_saved_tags_object_obj_eta",&tree_last_module_with_saved_tags_obj_eta[i]);
		tree[i]->SetBranchAddress("last_module_with_saved_tags_object_obj_phi",&tree_last_module_with_saved_tags_obj_phi[i]);
			
						
	}

	for (int i=0; i<2; i++) // comparing 2 samples
	{
				
		gentree[i]->SetBranchAddress("event",&gen_tree_event[i]);
		gentree[i]->SetBranchAddress("gen_ID",&gen_tree_gen_ID[i]);
		gentree[i]->SetBranchAddress("gen_parent_ID",&gen_tree_gen_parent_ID[i]);
		gentree[i]->SetBranchAddress("gen_grandparent_ID",&gen_tree_gen_grandparent_ID[i]);
		gentree[i]->SetBranchAddress("gen_px",&gen_tree_gen_px[i]);
		gentree[i]->SetBranchAddress("gen_py",&gen_tree_gen_py[i]);
		gentree[i]->SetBranchAddress("gen_pz",&gen_tree_gen_pz[i]); 
		gentree[i]->SetBranchAddress("gen_pt",&gen_tree_gen_pt[i]); 
		gentree[i]->SetBranchAddress("gen_energy",&gen_tree_gen_energy[i]);
		gentree[i]->SetBranchAddress("gen_eta",&gen_tree_gen_eta[i]);
		gentree[i]->SetBranchAddress("gen_phi",&gen_tree_gen_phi[i]);
		gentree[i]->SetBranchAddress("gen_charge",&gen_tree_gen_charge[i]);
			
						
	}

	
	for (int ent0=0; ent0<10000; ent0++)
	{
		get_entry_from_evt_gen_tree0[ent0] = -1;
		
		for (int ent1=0; ent1<750; ent1++)
		{
			get_entry_from_evt_trig_tree0[ent0][ent1] = -1;
			get_entry_from_evt_trig_tree1[ent0][ent1] = -1;
		}
	}
		
	int tree1_entries = tree[0]->GetEntries();
	int tree2_entries = tree[1]->GetEntries();
	
	if (verbosity)
	{
		cout << "  " << endl;
		cout << "checking to see if the trees have the same number of entries: " << endl;
		cout << "tree 1: " << tree1_entries << endl;
		cout << "tree 2: " << tree2_entries << endl;
		cout << "  " << endl;
	}
	
	events0 = 0;
	events1 = 0;
	
	long int min_event0 = 1000000;
	long int min_event1 = 1000000;
	
	sanity_ck1 = 0;
	sanity_ck2 = 0;
	
	for (Long_t ientry=0; ientry<tree[0]->GetEntries(); ientry++)
	{
		
		tree[0]->GetEntry(ientry);
		tree[1]->GetEntry(ientry);
		
		if ((ientry%100000==0)&&verbosity)
		{
			cout << "First loop. On entry " << ientry << " out of " << tree1_entries << ", " << 100*((float)ientry/(float)tree1_entries) << " percent done." << endl;
		}
				
		get_entry_from_evt_trig_tree0[tree_event[0]][tree_path_index[0]] = ientry;
		get_entry_from_evt_trig_tree1[tree_event[1]][tree_path_index[1]] = ientry;
		
		if (tree_path_index[0]>sanity_ck1) sanity_ck1 = tree_path_index[0];
		if (tree_path_index[1]>sanity_ck2) sanity_ck2 = tree_path_index[1];
		
		if (tree_event[0]>events0) events0 = tree_event[0];
		if (tree_event[1]>events1) events1 = tree_event[1];
		
		if (tree_event[0]<min_event0) min_event0 = tree_event[0];
		if (tree_event[1]<min_event1) min_event1 = tree_event[1];
				
		
	}
	
	if (verbosity)
	{
		cout << "  " << endl;
		cout << "last event in rel0: " << events0 << endl;
		cout << "last event in rel1: " << events1 << endl;
		cout << "first event in rel0 (may not be 1): " << min_event0 << endl;
		cout << "first event in rel1 (may not be 1): " << min_event1 << endl;
		cout << "total events in rel0, calculated by total entries/paths (should be whole number): " << (double)tree1_entries / (double)(sanity_ck1 + 1) << endl;
		cout << "total events in rel1, calculated by total entries/paths (should be whole number): " << (double)tree2_entries / (double)(sanity_ck2 + 1) << endl;
		cout << "paths in rel0: " << sanity_ck1 + 1 << endl;
		cout << "paths in rel1: " << sanity_ck2 + 1 << endl;
		cout << "  " << endl;
	}
	
	for (Long_t ientry=0; ientry<gentree[0]->GetEntries(); ientry++)
	{
		gentree[0]->GetEntry(ientry);
		get_entry_from_evt_gen_tree0[gen_tree_event[0]] = ientry;
	}
	

}







void Compare_GenHLTOffline::book_histograms()
{
	if (verbosity>1)
	{
		cout << " " << endl; 
		cout << "booking histos .." << endl;
		cout << " " << endl;
	}
	
	
	path_accept0 = new TH1F("path_accept0","path_accept0",750,0,750);
	path_accept1 = new TH1F("path_accept1","path_accept1",750,0,750);

	fired_rel0_didntfire_rel1 = new TH1F("fired_rel0_didntfire_rel1","fired_rel0_didntfire_rel1",750,0,750);
	fired_rel1_didntfire_rel0 = new TH1F("fired_rel1_didntfire_rel0","fired_rel1_didntfire_rel0",750,0,750);
	trigs_all_diffs = new TH1F("trigs_all_diffs","trigs_all_diffs",750,0,750);

	fired_rel0_didntfire_rel1_L1_ID = new TH1F("fired_rel0_didntfire_rel1_L1_ID","fired_rel0_didntfire_rel1_L1_ID",1000,-100,100);
	fired_rel1_didntfire_rel0_L1_ID = new TH1F("fired_rel1_didntfire_rel0_L1_ID","fired_rel1_didntfire_rel0_L1_ID",1000,-100,100);

	prescales_hist = new TH1F("prescales_hist","prescales_hist",750,0,750);

	fired_rel0_didntfire_rel1_prescale_value = new TH1F("fired_rel0_didntfire_rel1_prescale_value","fired_rel0_didntfire_rel1_prescale_value",1000,0,1000);
	fired_rel1_didntfire_rel0_prescale_value = new TH1F("fired_rel1_didntfire_rel0_prescale_value","fired_rel1_didntfire_rel0_prescale_value",1000,0,1000);

	events_where_fired_rel0_didntfire_rel1 = new TH1F("events_where_fired_rel0_didntfire_rel1","events_where_fired_rel0_didntfire_rel1",10000,0,10000);
	events_where_fired_rel1_didntfire_rel0 = new TH1F("events_where_fired_rel1_didntfire_rel0","events_where_fired_rel1_didntfire_rel0",10000,0,10000);
	events_all_diffs = new TH1F("events_all_diffs","events_all_diffs",10000,0,10000);

	gen_ID_objs_where_fired_rel0_didntfire_rel1 = new TH1F("gen_ID_objs_where_fired_rel0_didntfire_rel1","gen_ID_objs_where_fired_rel0_didntfire_rel1",200,-100,100);
	gen_ID_objs_where_fired_rel1_didntfire_rel0 = new TH1F("gen_ID_objs_where_fired_rel1_didntfire_rel0","gen_ID_objs_where_fired_rel1_didntfire_rel0",200,-100,100);

	event_path0 = new TH2F("event_path0","event_path0",750,0,750,10000,0,10000);
	event_path1 = new TH2F("event_path1","event_path1",750,0,750,10000,0,10000);	  

	//last_mod_index0 = new TH2F("last_mod_index0","last_mod_index0",750,0,750,10000,0,10000);
	//last_mod_index1 = new TH2F("last_mod_index1","last_mod_index1",750,0,750,10000,0,10000);

	howmany = new TH1F("howmany","howmany",100,0,100);

	fired_rel0_didntfire_rel1_wasL1_fail = new TH1F("fired_rel0_didntfire_rel1_wasL1_fail","fired_rel0_didntfire_rel1_wasL1_fail",750,0,750);
	fired_rel1_didntfire_rel0_wasL1_fail = new TH1F("fired_rel1_didntfire_rel0_wasL1_fail","fired_rel1_didntfire_rel0_wasL1_fail",750,0,750);

	one_event_datasets =  new TH1F("one_event_datasets","one_event_datasets",50,0,50);

	path_accept0_datasets = new TH1F("path_accept0_datasets","path_accept0_datasets",50,0,50);
	path_accept1_datasets = new TH1F("path_accept1_datasets","path_accept1_datasets",50,0,50);	  
	fired_rel0_didntfire_rel1_datasets = new TH1F("fired_rel0_didntfire_rel1_datasets","fired_rel0_didntfire_rel1_datasets",50,0,50);
	fired_rel1_didntfire_rel0_datasets = new TH1F("fired_rel1_didntfire_rel0_datasets","fired_rel1_didntfire_rel0_datasets",50,0,50);
	
	path_accept0_decay_type_vs_datasets = new TH2F("path_accept0_decay_type_vs_datasets","path_accept0_decay_type_vs_datasets",50,0,50,10,0,10);
	path_accept1_decay_type_vs_datasets = new TH2F("path_accept1_decay_type_vs_datasets","path_accept1_decay_type_vs_datasets",50,0,50,10,0,10);	
	fired_rel0_didntfire_rel1_decay_type_vs_datasets = new TH2F("fired_rel0_didntfire_rel1_decay_type_vs_datasets","fired_rel0_didntfire_rel1_decay_type_vs_datasets",50,0,50,10,0,10);
	fired_rel1_didntfire_rel0_decay_type_vs_datasets = new TH2F("fired_rel1_didntfire_rel0_decay_type_vs_datasets","fired_rel1_didntfire_rel0_decay_type_vs_datasets",50,0,50,10,0,10);

	last_mod_type_fired_rel0_didntfire_rel1 = new TH1F("last_mod_type_fired_rel0_didntfire_rel1","last_mod_type_fired_rel0_didntfire_rel1",10,0,10);
	last_mod_type_fired_rel1_didntfire_rel0 = new TH1F("last_mod_type_fired_rel1_didntfire_rel0","last_mod_type_fired_rel1_didntfire_rel0",10,0,10);


	for (int get_dataset_labels=0; get_dataset_labels<50; get_dataset_labels++)
	{
		one_event_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		path_accept0_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		path_accept1_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		fired_rel0_didntfire_rel1_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		fired_rel1_didntfire_rel0_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		path_accept0_decay_type_vs_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		path_accept1_decay_type_vs_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
		fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetXaxis()->SetBinLabel(get_dataset_labels+1,dataset_map->GetYaxis()->GetBinLabel(get_dataset_labels+1));
	}


	fired_rel0_didntfire_rel1->GetXaxis()->SetTitle("path index");
	fired_rel0_didntfire_rel1->GetYaxis()->SetTitle("events");
	fired_rel0_didntfire_rel1->SetTitle("fired " + rel0_lbl + ", didn't fire " + rel1_lbl);	
	
	// dataset version:
	fired_rel0_didntfire_rel1_datasets->GetXaxis()->SetTitle("dataset");
	fired_rel0_didntfire_rel1_datasets->GetXaxis()->LabelsOption("v");
	fired_rel0_didntfire_rel1_datasets->GetYaxis()->SetTitle("events");
	fired_rel0_didntfire_rel1_datasets->SetTitle("fired " + rel0_lbl + ", didn't fire " + rel1_lbl);
	
	fired_rel1_didntfire_rel0->GetXaxis()->SetTitle("path index");
	fired_rel1_didntfire_rel0->GetYaxis()->SetTitle("events");	
	fired_rel1_didntfire_rel0->SetTitle("fired " + rel1_lbl + ", didn't fire " + rel0_lbl);
	
	// dataset version:	
	fired_rel1_didntfire_rel0_datasets->GetXaxis()->SetTitle("dataset");
	fired_rel1_didntfire_rel0_datasets->GetXaxis()->LabelsOption("v");
	fired_rel1_didntfire_rel0_datasets->GetYaxis()->SetTitle("events");	
	fired_rel1_didntfire_rel0_datasets->SetTitle("fired " + rel1_lbl + ", didn't fire " + rel0_lbl);

	path_accept0->GetXaxis()->SetTitle("path index");
	path_accept0->GetYaxis()->SetTitle("counts");	
	path_accept0->SetTitle("total counts (" + rel0_lbl + ")");
	
	path_accept1->GetXaxis()->SetTitle("path index");
	path_accept1->GetYaxis()->SetTitle("counts");	
	path_accept1->SetTitle("total counts (" + rel1_lbl + ")");
	
	// dataset version:
	path_accept0_datasets->GetXaxis()->SetTitle("dataset");
	path_accept0_datasets->GetXaxis()->LabelsOption("v");
	path_accept0_datasets->GetYaxis()->SetTitle("counts");	
	path_accept0_datasets->SetTitle("total counts in stream A datasets (" + rel0_lbl + ")");
	
	path_accept1_datasets->GetXaxis()->SetTitle("dataset");
	path_accept1_datasets->GetXaxis()->LabelsOption("v");
	path_accept1_datasets->GetYaxis()->SetTitle("counts");	
	path_accept1_datasets->SetTitle("total counts in stream A datasets (" + rel1_lbl + ")");
	
	
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(1,"Double Ele");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(2,"Double Mu");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(3,"Double Tau");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(4,"Ele + Mu");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(5,"Ele + Tau");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(6,"Mu + Tau");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(7,"Single Ele");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(8,"Single Mu");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(9,"Single Tau");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(10,"All Had");

	fired_rel0_didntfire_rel1_decay_type_vs_datasets->SetTitle("lost counts going from " + rel0_lbl + " to " + rel1_lbl + ", ttbar decay vs dataset");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetXaxis()->LabelsOption("v");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->GetXaxis()->SetTitle("dataset");
	fired_rel0_didntfire_rel1_decay_type_vs_datasets->SetStats(0);
	
	
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(1,"Double Ele");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(2,"Double Mu");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(3,"Double Tau");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(4,"Ele + Mu");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(5,"Ele + Tau");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(6,"Mu + Tau");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(7,"Single Ele");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(8,"Single Mu");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(9,"Single Tau");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(10,"All Had");

	fired_rel1_didntfire_rel0_decay_type_vs_datasets->SetTitle("gained counts going from " + rel0_lbl + " to " + rel1_lbl + ", ttbar decay vs dataset");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetXaxis()->LabelsOption("v");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->GetXaxis()->SetTitle("dataset");
	fired_rel1_didntfire_rel0_decay_type_vs_datasets->SetStats(0);
	
	
	
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(1,"Double Ele");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(2,"Double Mu");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(3,"Double Tau");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(4,"Ele + Mu");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(5,"Ele + Tau");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(6,"Mu + Tau");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(7,"Single Ele");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(8,"Single Mu");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(9,"Single Tau");
	path_accept0_decay_type_vs_datasets->GetYaxis()->SetBinLabel(10,"All Had");
	path_accept0_decay_type_vs_datasets->SetTitle("ttbar decay vs dataset, " + rel0_lbl);
	path_accept0_decay_type_vs_datasets->GetXaxis()->LabelsOption("v");
	path_accept0_decay_type_vs_datasets->GetXaxis()->SetTitle("dataset");
	path_accept0_decay_type_vs_datasets->SetStats(0);
	
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(1,"Double Ele");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(2,"Double Mu");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(3,"Double Tau");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(4,"Ele + Mu");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(5,"Ele + Tau");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(6,"Mu + Tau");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(7,"Single Ele");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(8,"Single Mu");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(9,"Single Tau");
	path_accept1_decay_type_vs_datasets->GetYaxis()->SetBinLabel(10,"All Had");
	path_accept1_decay_type_vs_datasets->SetTitle("ttbar decay vs dataset, " + rel1_lbl);
	path_accept1_decay_type_vs_datasets->GetXaxis()->LabelsOption("v");
	path_accept1_decay_type_vs_datasets->GetXaxis()->SetTitle("dataset");
	path_accept1_decay_type_vs_datasets->SetStats(0);
	
	

	
	
	last_mod_type_fired_rel0_didntfire_rel1->SetTitle("EDFilter of last run module (fired " + rel0_lbl + ", didn't fire " + rel1_lbl + ")");
	//last_mod_type_fired_rel0_didntfire_rel1->GetXaxis()->LabelsOption("v");
	last_mod_type_fired_rel1_didntfire_rel0->SetTitle("EDFilter of last run module (fired " + rel1_lbl + ", didn't fire " + rel0_lbl + ")");
	//last_mod_type_fired_rel1_didntfire_rel0->GetXaxis()->LabelsOption("v");



}




void Compare_GenHLTOffline::getprescales(TH1F *prescales)
{

	// by hand for now
	
	// expects that prescales has >= 454 bins
	
	std::vector<std::string> labels;

	labels.push_back("AlCa_EcalEtaEBonly_v6");										
	labels.push_back("AlCa_EcalEtaEEonly_v6");										
	labels.push_back("AlCa_EcalPhiSym_v13");										
	labels.push_back("AlCa_EcalPi0EBonly_v6");										
	labels.push_back("AlCa_EcalPi0EEonly_v6");										
	labels.push_back("AlCa_LumiPixels_Random_v1");									
	labels.push_back("AlCa_LumiPixels_v8");										
	labels.push_back("AlCa_LumiPixels_ZeroBias_v4");									
	labels.push_back("AlCa_RPCMuonNoHits_v9");										
	labels.push_back("AlCa_RPCMuonNormalisation_v9");									
	labels.push_back("AlCa_RPCMuonNoTriggers_v9");									
	labels.push_back("DQM_FEDIntegrity_v11");										
	labels.push_back("DST_Ele8_CaloIdL_CaloIsoVL_TrkIdVL_TrkIsoVL_HT250_v5");						
	labels.push_back("DST_HT250_v4");											
	labels.push_back("DST_L1HTT_Or_L1MultiJet_v4");									
	labels.push_back("DST_Mu5_HT250_v4");										
	labels.push_back("DST_Physics_v5");  										
	labels.push_back("HLT_Activity_Ecal_SC7_v13");									
	labels.push_back("HLT_BeamGas_HF_Beam1_v5"); 									
	labels.push_back("HLT_BeamGas_HF_Beam2_v5"); 									
	labels.push_back("HLT_BeamHalo_v13");										
	labels.push_back("HLT_BTagMu_DiJet110_Mu5_v6");									
	labels.push_back("HLT_BTagMu_DiJet20_Mu5_v6");									
	labels.push_back("HLT_BTagMu_DiJet40_Mu5_v6");									
	labels.push_back("HLT_BTagMu_DiJet70_Mu5_v6");									
	labels.push_back("HLT_BTagMu_Jet20_Mu4_v2"); 									
	labels.push_back("HLT_BTagMu_Jet300_Mu5_v6");									
	labels.push_back("HLT_BTagMu_Jet60_Mu4_v2"); 									
	labels.push_back("HLT_CleanPFNoPUHT300_Ele15_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET45_v3"); 			
	labels.push_back("HLT_CleanPFNoPUHT300_Ele15_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET50_v3"); 			
	labels.push_back("HLT_CleanPFNoPUHT300_Ele40_CaloIdVT_TrkIdT_v3");							
	labels.push_back("HLT_CleanPFNoPUHT300_Ele60_CaloIdVT_TrkIdT_v3");							
	labels.push_back("HLT_CleanPFNoPUHT350_Ele5_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET45_v3");  			
	labels.push_back("HLT_CleanPFNoPUHT350_Ele5_CaloIdT_CaloIsoVL_TrkIdT_TrkIsoVL_PFMET50_v3");  			
	labels.push_back("HLT_DiCentralJetSumpT100_dPhi05_DiCentralPFJet60_25_PFMET100_HBHENoiseCleaned_v5");		
	labels.push_back("HLT_DiCentralPFJet30_PFMET80_BTagCSV07_v5");							
	labels.push_back("HLT_DiCentralPFJet30_PFMET80_v6"); 								
	labels.push_back("HLT_DiCentralPFNoPUJet50_PFMETORPFMETNoMu80_v4");  						
	labels.push_back("HLT_DiJet20_MJJ650_AllJets_DEta3p5_HT120_VBF_v1"); 						
	labels.push_back("HLT_DiJet30_MJJ700_AllJets_DEta3p5_VBF_v1");							
	labels.push_back("HLT_DiJet35_MJJ650_AllJets_DEta3p5_VBF_v5");							
	labels.push_back("HLT_DiJet35_MJJ700_AllJets_DEta3p5_VBF_v5");							
	labels.push_back("HLT_DiJet35_MJJ750_AllJets_DEta3p5_VBF_v5");							
	labels.push_back("HLT_DiJet40Eta2p6_BTagIP3DFastPV_v7");								
	labels.push_back("HLT_DiJet80_DiJet60_DiJet20_v6");  								
	labels.push_back("HLT_DiJet80Eta2p6_BTagIP3DFastPVLoose_v7");							
	labels.push_back("HLT_Dimuon0_Jpsi_Muon_v18");									
	labels.push_back("HLT_Dimuon0_Jpsi_NoVertexing_v14");								
	labels.push_back("HLT_Dimuon0_Jpsi_v17");										
	labels.push_back("HLT_Dimuon0_PsiPrime_v6"); 									
	labels.push_back("HLT_Dimuon0_Upsilon_Muon_v18");									
	labels.push_back("HLT_Dimuon0_Upsilon_v17"); 									
	labels.push_back("HLT_Dimuon10_Jpsi_v6");										
	labels.push_back("HLT_Dimuon11_Upsilon_v6"); 									
	labels.push_back("HLT_Dimuon3p5_SameSign_v6");									
	labels.push_back("HLT_Dimuon5_PsiPrime_v6"); 									
	labels.push_back("HLT_Dimuon5_Upsilon_v6");  									
	labels.push_back("HLT_Dimuon7_PsiPrime_v3"); 									
	labels.push_back("HLT_Dimuon7_Upsilon_v7");  									
	labels.push_back("HLT_Dimuon8_Jpsi_v7");										
	labels.push_back("HLT_Dimuon8_Upsilon_v6");  									
	labels.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ600VBF_LeadingJets_v9");						
	labels.push_back("HLT_DiPFJet40_PFMETnoMu65_MJJ800VBF_AllJets_v9");  						
	labels.push_back("HLT_DiPFJet80_DiPFJet30_BTagCSVd07d05_v5");							
	labels.push_back("HLT_DiPFJet80_DiPFJet30_BTagCSVd07d05d03_v5");							
	labels.push_back("HLT_DiPFJet80_DiPFJet30_BTagCSVd07d05d05_v5");							
	labels.push_back("HLT_DiPFJetAve140_v10");										
	labels.push_back("HLT_DiPFJetAve200_v10");										
	labels.push_back("HLT_DiPFJetAve260_v10");										
	labels.push_back("HLT_DiPFJetAve320_v10");										
	labels.push_back("HLT_DiPFJetAve40_v9");										
	labels.push_back("HLT_DiPFJetAve400_v10");									
	labels.push_back("HLT_DiPFJetAve80_v10");										
	labels.push_back("HLT_DisplacedPhoton65_CaloIdVL_IsoL_PFMET25_v4");  						
	labels.push_back("HLT_DisplacedPhoton65EBOnly_CaloIdVL_IsoL_PFMET30_v4");						
	labels.push_back("HLT_DoubleDisplacedMu4_DiPFJet40Neutral_v8");							
	labels.push_back("HLT_DoubleEle10_CaloIdL_TrkIdVL_Ele10_CaloIdT_TrkIdVL_v12");					
	labels.push_back("HLT_DoubleEle14_CaloIdT_TrkIdVL_Mass8_PFMET40_v8");						
	labels.push_back("HLT_DoubleEle14_CaloIdT_TrkIdVL_Mass8_PFMET50_v8");						
	labels.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v7");								
	labels.push_back("HLT_DoubleEle33_CaloIdL_v14");									
	labels.push_back("HLT_DoubleEle33_CaloIdT_v10");									
	labels.push_back("HLT_DoubleEle8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT175_v4");						
	labels.push_back("HLT_DoubleEle8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT225_v4");						
	labels.push_back("HLT_DoubleEle8_CaloIdT_TrkIdVL_v12");								
	labels.push_back("HLT_DoubleIsoL2Tau30_eta2p1_v1");  								
	labels.push_back("HLT_DoubleJet20_ForwardBackward_v4");								
	labels.push_back("HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Jet30_v5"); 						
	labels.push_back("HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Reg_Jet30_v1");						
	labels.push_back("HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Reg_v1");							
	labels.push_back("HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_v4");							
	labels.push_back("HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Prong1_Reg_v1");						
	labels.push_back("HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Prong1_v4");						
	labels.push_back("HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_Reg_v1");							
	labels.push_back("HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_v4");							
	labels.push_back("HLT_DoubleMu11_Acoplanarity03_v5");								
	labels.push_back("HLT_DoubleMu14_Mass8_PFMET40_v8"); 								
	labels.push_back("HLT_DoubleMu14_Mass8_PFMET50_v8"); 								
	labels.push_back("HLT_DoubleMu3_4_Dimuon5_Bs_Central_v5");								
	labels.push_back("HLT_DoubleMu3p5_4_Dimuon5_Bs_Central_v5"); 							
	labels.push_back("HLT_DoubleMu3p5_LowMass_Displaced_v6");								
	labels.push_back("HLT_DoubleMu3p5_LowMassNonResonant_Displaced_v6"); 						
	labels.push_back("HLT_DoubleMu4_Acoplanarity03_v5"); 								
	labels.push_back("HLT_DoubleMu4_Dimuon7_Bs_Forward_v5");								
	labels.push_back("HLT_DoubleMu4_Jpsi_Displaced_v12");								
	labels.push_back("HLT_DoubleMu4_JpsiTk_Displaced_v7");								
	labels.push_back("HLT_DoubleMu5_Ele8_CaloIdT_TrkIdVL_v16");  							
	labels.push_back("HLT_DoubleMu5_IsoMu5_v20");									
	labels.push_back("HLT_DoubleMu8_Ele8_CaloIdT_TrkIdVL_v5");								
	labels.push_back("HLT_DoubleMu8_Mass8_PFNoPUHT175_v4");								
	labels.push_back("HLT_DoubleMu8_Mass8_PFNoPUHT225_v4");								
	labels.push_back("HLT_DoublePhoton40_CaloIdL_Rsq0p035_v6");  							
	labels.push_back("HLT_DoublePhoton40_CaloIdL_Rsq0p06_v6");								
	labels.push_back("HLT_DoublePhoton48_HEVT_v9");									
	labels.push_back("HLT_DoublePhoton53_HEVT_v3");									
	labels.push_back("HLT_DoublePhoton70_v6");										
	labels.push_back("HLT_DoublePhoton80_v7");										
	labels.push_back("HLT_DoubleRelIso1p0Mu5_Mass8_PFNoPUHT175_v4");							
	labels.push_back("HLT_DoubleRelIso1p0Mu5_Mass8_PFNoPUHT225_v4");							
	labels.push_back("HLT_DTCalibration_v2");										
	labels.push_back("HLT_DTErrors_v3"); 										
	labels.push_back("HLT_EcalCalibration_v3");  									
	labels.push_back("HLT_EightJet30_eta3p0_v5");									
	labels.push_back("HLT_EightJet35_eta3p0_v5");									
	labels.push_back("HLT_Ele12_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_DoubleCentralJet65_v4");				
	labels.push_back("HLT_Ele12_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_RsqMR30_Rsq0p04_MR200_v4");				
	labels.push_back("HLT_Ele12_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_RsqMR40_Rsq0p04_MR200_v4");				
	labels.push_back("HLT_Ele13_eta2p1_WP90NoIso_LooseIsoPFTau20_L1ETM36_v1");						
	labels.push_back("HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_L1ETM36_v1");						
	labels.push_back("HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_v1");							
	labels.push_back("HLT_Ele15_Ele8_Ele5_CaloIdL_TrkIdVL_v6");  							
	labels.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_v17"); 								
	labels.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v19");	
	labels.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Jet30_v7");						
	labels.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v6"); 						
	labels.push_back("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_Ele8_Mass50_v6");					
	labels.push_back("HLT_Ele20_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC4_Mass50_v7");					
	labels.push_back("HLT_Ele22_CaloIdL_CaloIsoVL_v6");  								
	labels.push_back("HLT_Ele22_eta2p1_WP90NoIso_LooseIsoPFTau20_v7");							
	labels.push_back("HLT_Ele22_eta2p1_WP90Rho_LooseIsoPFTau20_v7");							
	labels.push_back("HLT_Ele23_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_HFT30_v8");						
	labels.push_back("HLT_Ele24_WP80_CentralPFJet35_CentralPFJet25_PFMET20_v2"); 					
	labels.push_back("HLT_Ele24_WP80_CentralPFJet35_CentralPFJet25_v2"); 						
	labels.push_back("HLT_Ele24_WP80_PFJet30_PFJet25_Deta3_CentralPFJet30_v2");  					
	labels.push_back("HLT_Ele24_WP80_PFJet30_PFJet25_Deta3_v2"); 							
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralPFNoPUJet30_BTagIPIter_v10");			
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralPFNoPUJet30_v9");				
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_DiCentralPFNoPUJet30_v3");				
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet30_v5");				
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet45_35_25_v3");			
	labels.push_back("HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet50_40_30_v5");			
	labels.push_back("HLT_Ele27_CaloIdL_CaloIsoVL_TrkIdVL_TrkIsoVL_v11");						
	labels.push_back("HLT_Ele27_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele15_CaloIdT_CaloIsoVL_trackless_v8");		
	labels.push_back("HLT_Ele27_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_HFT15_v8");						
	labels.push_back("HLT_Ele27_WP80_CentralPFJet80_v10");								
	labels.push_back("HLT_Ele27_WP80_PFMET_MT50_v8");									
	labels.push_back("HLT_Ele27_WP80_v12");										
	labels.push_back("HLT_Ele27_WP80_WCandPt80_v10");									
	labels.push_back("HLT_Ele30_CaloIdVT_TrkIdT_PFNoPUJet100_PFNoPUJet25_v8");						
	labels.push_back("HLT_Ele30_CaloIdVT_TrkIdT_PFNoPUJet150_PFNoPUJet25_v8");						
	labels.push_back("HLT_Ele30_CaloIdVT_TrkIdT_v6");									
	labels.push_back("HLT_Ele32_CaloIdL_CaloIsoVL_TrkIdVL_TrkIsoVL_v11");						
	labels.push_back("HLT_Ele32_CaloIdT_CaloIsoT_TrkIdT_TrkIsoT_SC17_Mass50_v6");					
	labels.push_back("HLT_Ele5_SC5_Jpsi_Mass2to15_v5");  								
	labels.push_back("HLT_Ele8_CaloIdL_CaloIsoVL_v17");  								
	labels.push_back("HLT_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Jet30_v7");						
	labels.push_back("HLT_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v15"); 						
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdT_DiJet30_v18");								
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdT_QuadJet30_v18");								
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdT_TriJet30_v18");								
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdVL_EG7_v2"); 								
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdVL_Jet30_v7");								
	labels.push_back("HLT_Ele8_CaloIdT_TrkIdVL_v5");									
	labels.push_back("HLT_Ele80_CaloIdVT_GsfTrkIdT_v2"); 								
	labels.push_back("HLT_Ele90_CaloIdVT_GsfTrkIdT_v2"); 								
	labels.push_back("HLT_ExclDiJet35_HFAND_v4");									
	labels.push_back("HLT_ExclDiJet35_HFOR_v4"); 									
	labels.push_back("HLT_ExclDiJet80_HFAND_v4");									
	labels.push_back("HLT_FatDiPFJetMass750_DR1p1_Deta1p5_v10"); 							
	labels.push_back("HLT_GlobalRunHPDNoise_v8");									
	labels.push_back("HLT_HcalCalibration_v3");  									
	labels.push_back("HLT_HcalNZS_v10"); 										
	labels.push_back("HLT_HcalPhiSym_v11");										
	labels.push_back("HLT_HcalUTCA_v1"); 										
	labels.push_back("HLT_HT200_AlphaT0p57_v8"); 									
	labels.push_back("HLT_HT200_v6");											
	labels.push_back("HLT_HT250_AlphaT0p55_v8"); 									
	labels.push_back("HLT_HT250_AlphaT0p57_v8"); 									
	labels.push_back("HLT_HT250_v7");											
	labels.push_back("HLT_HT300_AlphaT0p53_v8"); 									
	labels.push_back("HLT_HT300_AlphaT0p54_v14");									
	labels.push_back("HLT_HT300_DoubleDisplacedPFJet60_ChgFraction10_v10");						
	labels.push_back("HLT_HT300_DoubleDisplacedPFJet60_v10");								
	labels.push_back("HLT_HT300_SingleDisplacedPFJet60_ChgFraction10_v10");						
	labels.push_back("HLT_HT300_SingleDisplacedPFJet60_v10");								
	labels.push_back("HLT_HT300_v7");											
	labels.push_back("HLT_HT350_AlphaT0p52_v8"); 									
	labels.push_back("HLT_HT350_AlphaT0p53_v19");									
	labels.push_back("HLT_HT350_v7");											
	labels.push_back("HLT_HT400_AlphaT0p51_v19");									
	labels.push_back("HLT_HT400_AlphaT0p52_v14");									
	labels.push_back("HLT_HT400_v7");											
	labels.push_back("HLT_HT450_AlphaT0p51_v14");									
	labels.push_back("HLT_HT450_v7");											
	labels.push_back("HLT_HT500_v7");											
	labels.push_back("HLT_HT550_v7");											
	labels.push_back("HLT_HT650_Track50_dEdx3p6_v10");									
	labels.push_back("HLT_HT650_Track60_dEdx3p7_v10");									
	labels.push_back("HLT_HT650_v7");											
	labels.push_back("HLT_HT750_v7");											
	labels.push_back("HLT_IsoMu12_DoubleCentralJet65_v4");								
	labels.push_back("HLT_IsoMu12_RsqMR30_Rsq0p04_MR200_v4");								
	labels.push_back("HLT_IsoMu12_RsqMR40_Rsq0p04_MR200_v4");								
	labels.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v7");									
	labels.push_back("HLT_IsoMu15_eta2p1_LooseIsoPFTau35_Trk20_Prong1_L1ETM20_v10");					
	labels.push_back("HLT_IsoMu17_eta2p1_CentralPFNoPUJet30_BTagIPIter_v4");						
	labels.push_back("HLT_IsoMu17_eta2p1_CentralPFNoPUJet30_v4");							
	labels.push_back("HLT_IsoMu17_eta2p1_DiCentralPFNoPUJet30_PFNoPUHT350_PFMHT40_v3");  				
	labels.push_back("HLT_IsoMu17_eta2p1_DiCentralPFNoPUJet30_v4");							
	labels.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v7");								
	labels.push_back("HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet30_v4");							
	labels.push_back("HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet45_35_25_v2");						
	labels.push_back("HLT_IsoMu18_CentralPFJet30_CentralPFJet25_PFMET20_v1");						
	labels.push_back("HLT_IsoMu18_CentralPFJet30_CentralPFJet25_v1");							
	labels.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_Reg_v1");  					
	labels.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v4");						
	labels.push_back("HLT_IsoMu18_PFJet30_PFJet25_Deta3_CentralPFJet25_v1");						
	labels.push_back("HLT_IsoMu18_PFJet30_PFJet25_Deta3_v1");								
	labels.push_back("HLT_IsoMu20_eta2p1_CentralPFJet80_v9");								
	labels.push_back("HLT_IsoMu20_eta2p1_v7");										
	labels.push_back("HLT_IsoMu20_WCandPt80_v4");									
	labels.push_back("HLT_IsoMu24_eta2p1_v15");  									
	labels.push_back("HLT_IsoMu24_v17"); 										
	labels.push_back("HLT_IsoMu30_eta2p1_v15");  									
	labels.push_back("HLT_IsoMu30_v11"); 										
	labels.push_back("HLT_IsoMu34_eta2p1_v13");  									
	labels.push_back("HLT_IsoMu40_eta2p1_v10"); 									
	labels.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1");							
	labels.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1");								
	labels.push_back("HLT_IsoTrackHB_v14");										
	labels.push_back("HLT_IsoTrackHE_v15");										
	labels.push_back("HLT_Jet160Eta2p4_Jet120Eta2p4_DiBTagIP3DFastPVLoose_v7");  					
	labels.push_back("HLT_Jet370_NoJetID_v15"); 									
	labels.push_back("HLT_Jet60Eta1p7_Jet53Eta1p7_DiBTagIP3DFastPV_v7"); 						
	labels.push_back("HLT_Jet80Eta1p7_Jet70Eta1p7_DiBTagIP3DFastPV_v7"); 						
	labels.push_back("HLT_JetE30_NoBPTX_v14");										
	labels.push_back("HLT_JetE30_NoBPTX3BX_NoHalo_v16"); 								
	labels.push_back("HLT_JetE50_NoBPTX3BX_NoHalo_v13"); 								
	labels.push_back("HLT_JetE70_NoBPTX3BX_NoHalo_v5");  								
	labels.push_back("HLT_L1DoubleEG3_FwdVeto_v2");									
	labels.push_back("HLT_L1DoubleJet36Central_v7");									
	labels.push_back("HLT_L1ETM100_v2"); 										
	labels.push_back("HLT_L1ETM30_v2");  										
	labels.push_back("HLT_L1ETM40_v2");  										
	labels.push_back("HLT_L1ETM70_v2");  										
	labels.push_back("HLT_L1SingleEG12_v6");										
	labels.push_back("HLT_L1SingleEG5_v6");										
	labels.push_back("HLT_L1SingleJet16_v7");										
	labels.push_back("HLT_L1SingleJet36_v7");										
	labels.push_back("HLT_L1SingleMu12_v2");										
	labels.push_back("HLT_L1SingleMuOpen_AntiBPTX_v7");  								
	labels.push_back("HLT_L1SingleMuOpen_v7");										
	labels.push_back("HLT_L1Tech_CASTOR_HaloMuon_v4");									
	labels.push_back("HLT_L1Tech_DT_GlobalOR_v4");									
	labels.push_back("HLT_L1Tech_HBHEHO_totalOR_v6");									
	labels.push_back("HLT_L1Tech_HCAL_HF_single_channel_v4");								
	labels.push_back("HLT_L1TrackerCosmics_v7"); 									
	labels.push_back("HLT_L2DoubleMu23_NoVertex_2Cha_Angle2p5_v3");							
	labels.push_back("HLT_L2DoubleMu23_NoVertex_v11");									
	labels.push_back("HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_v3");							
	labels.push_back("HLT_L2Mu10_NoVertex_NoBPTX3BX_NoHalo_v4"); 							
	labels.push_back("HLT_L2Mu20_eta2p1_NoVertex_v2");									
	labels.push_back("HLT_L2Mu20_NoVertex_2Cha_NoBPTX3BX_NoHalo_v1");							
	labels.push_back("HLT_L2Mu30_NoVertex_2Cha_NoBPTX3BX_NoHalo_v1");							
	labels.push_back("HLT_L2Mu70_2Cha_eta2p1_PFMET55_v2");								
	labels.push_back("HLT_L2Mu70_2Cha_eta2p1_PFMET60_v2");								
	labels.push_back("HLT_L2TripleMu10_0_0_NoVertex_PFJet40Neutral_v8");						
	labels.push_back("HLT_LogMonitor_v4");										
	labels.push_back("HLT_LooseIsoPFTau35_Trk20_Prong1_MET70_v10");							
	labels.push_back("HLT_LooseIsoPFTau35_Trk20_Prong1_MET75_v10");							
	labels.push_back("HLT_LooseIsoPFTau35_Trk20_Prong1_v10");								
	labels.push_back("HLT_MET100_HBHENoiseCleaned_v1");  								
	labels.push_back("HLT_MET120_HBHENoiseCleaned_v6");  								
	labels.push_back("HLT_MET120_v13");  										
	labels.push_back("HLT_MET200_HBHENoiseCleaned_v5");  								
	labels.push_back("HLT_MET200_v12");  										
	labels.push_back("HLT_MET300_HBHENoiseCleaned_v5");  								
	labels.push_back("HLT_MET300_v4");											
	labels.push_back("HLT_MET400_HBHENoiseCleaned_v5");  								
	labels.push_back("HLT_MET400_v7");											
	labels.push_back("HLT_MET80_Parked_v5");										
	labels.push_back("HLT_MET80_Track50_dEdx3p6_v6");									
	labels.push_back("HLT_MET80_Track60_dEdx3p7_v6");									
	labels.push_back("HLT_MET80_v5");											
	labels.push_back("HLT_MonoCentralPFJet80_PFMETnoMu105_NHEF0p95_v4"); 						
	labels.push_back("HLT_Mu12_eta2p1_DiCentral_20_v8"); 								
	labels.push_back("HLT_Mu12_eta2p1_DiCentral_40_20_BTagIP3D1stTrack_v8");						
	labels.push_back("HLT_Mu12_eta2p1_DiCentral_40_20_DiBTagIP3D1stTrack_v8");						
	labels.push_back("HLT_Mu12_eta2p1_DiCentral_40_20_v8");								
	labels.push_back("HLT_Mu12_eta2p1_L1Mu10erJetC12WdEtaPhi1DiJetsC_v7");						
	labels.push_back("HLT_Mu12_v18");											
	labels.push_back("HLT_Mu13_Mu8_NoDZ_v1");										
	labels.push_back("HLT_Mu13_Mu8_v22");										
	labels.push_back("HLT_Mu14_Ele14_CaloIdT_TrkIdVL_Mass8_PFMET40_v8");						
	labels.push_back("HLT_Mu14_Ele14_CaloIdT_TrkIdVL_Mass8_PFMET50_v8"); 						
	labels.push_back("HLT_Mu15_eta2p1_DiCentral_20_v1"); 								
	labels.push_back("HLT_Mu15_eta2p1_DiCentral_40_20_v1");								
	labels.push_back("HLT_Mu15_eta2p1_L1ETM20_v5");									
	labels.push_back("HLT_Mu15_eta2p1_L1Mu10erJetC12WdEtaPhi1DiJetsC_v3");						
	labels.push_back("HLT_Mu15_eta2p1_TriCentral_40_20_20_BTagIP3D1stTrack_v8"); 					
	labels.push_back("HLT_Mu15_eta2p1_TriCentral_40_20_20_DiBTagIP3D1stTrack_v8");					
	labels.push_back("HLT_Mu15_eta2p1_TriCentral_40_20_20_v8");  							
	labels.push_back("HLT_Mu15_eta2p1_v5");										
	labels.push_back("HLT_Mu15_TkMu5_Onia_v1");  									
	labels.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v9");						
	labels.push_back("HLT_Mu17_eta2p1_CentralPFNoPUJet30_BTagIPIter_v4");						
	labels.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v7");								
	labels.push_back("HLT_Mu17_eta2p1_TriCentralPFNoPUJet45_35_25_v2");  						
	labels.push_back("HLT_Mu17_Mu8_v22");										
	labels.push_back("HLT_Mu17_TkMu8_NoDZ_v1");  									
	labels.push_back("HLT_Mu17_TkMu8_v14");										
	labels.push_back("HLT_Mu17_v5");											
	labels.push_back("HLT_Mu18_CentralPFJet30_CentralPFJet25_v1");							
	labels.push_back("HLT_Mu18_PFJet30_PFJet25_Deta3_CentralPFJet25_v1");						
	labels.push_back("HLT_Mu22_Photon22_CaloIdL_v7");									
	labels.push_back("HLT_Mu22_TkMu22_v9");										
	labels.push_back("HLT_Mu22_TkMu8_v9");										
	labels.push_back("HLT_Mu24_eta2p1_v5");										
	labels.push_back("HLT_Mu24_v16");											
	labels.push_back("HLT_Mu30_Ele30_CaloIdL_v8");									
	labels.push_back("HLT_Mu30_eta2p1_v5");										
	labels.push_back("HLT_Mu30_v16");											
	labels.push_back("HLT_Mu40_eta2p1_Track50_dEdx3p6_v5");								
	labels.push_back("HLT_Mu40_eta2p1_Track60_dEdx3p7_v5");								
	labels.push_back("HLT_Mu40_eta2p1_v11");										
	labels.push_back("HLT_Mu40_PFNoPUHT350_v4"); 									
	labels.push_back("HLT_Mu40_v14");											
	labels.push_back("HLT_Mu5_L2Mu3_Jpsi_v8");										
	labels.push_back("HLT_Mu5_Track2_Jpsi_v21"); 									
	labels.push_back("HLT_Mu5_Track3p5_Jpsi_v7");									
	labels.push_back("HLT_Mu5_v20");											
	labels.push_back("HLT_Mu50_eta2p1_v8");										
	labels.push_back("HLT_Mu60_PFNoPUHT350_v4"); 									
	labels.push_back("HLT_Mu7_Ele7_CaloIdT_CaloIsoVL_v7");								
	labels.push_back("HLT_Mu7_Track7_Jpsi_v20");									
	labels.push_back("HLT_Mu8_DiJet30_v7");										
	labels.push_back("HLT_Mu8_DoubleEle8_CaloIdT_TrkIdVL_v7");								
	labels.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v9");						
	labels.push_back("HLT_Mu8_Ele8_CaloIdT_TrkIdVL_Ele8_CaloIdL_TrkIdVL_v7");						
	labels.push_back("HLT_Mu8_Ele8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT175_v5");						
	labels.push_back("HLT_Mu8_Ele8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT225_v5");						
	labels.push_back("HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1");							
	labels.push_back("HLT_Mu8_QuadJet30_v7");										
	labels.push_back("HLT_Mu8_TriJet30_v7");										
	labels.push_back("HLT_Mu8_v18");											
	labels.push_back("HLT_PFJet140_v9"); 										
	labels.push_back("HLT_PFJet200_v9 ");										
	labels.push_back("HLT_PFJet260_v9 ");										
	labels.push_back("HLT_PFJet320_v9"); 										
	labels.push_back("HLT_PFJet40_v8");  										
	labels.push_back("HLT_PFJet400_v9"); 										
	labels.push_back("HLT_PFJet80_v9");  										
	labels.push_back("HLT_PFMET150_v7"); 										
	labels.push_back("HLT_PFMET180_v7"); 										
	labels.push_back("HLT_PFNoPUHT350_Mu15_PFMET45_v4"); 								
	labels.push_back("HLT_PFNoPUHT350_Mu15_PFMET50_v4"); 								
	labels.push_back("HLT_PFNoPUHT350_PFMET100_v4");									
	labels.push_back("HLT_PFNoPUHT350_v4");										
	labels.push_back("HLT_PFNoPUHT400_Mu5_PFMET45_v4");  								
	labels.push_back("HLT_PFNoPUHT400_Mu5_PFMET50_v4");  								
	labels.push_back("HLT_PFNoPUHT400_PFMET100_v4");									
	labels.push_back("HLT_PFNoPUHT650_DiCentralPFNoPUJet80_CenPFNoPUJet40_v4");  					
	labels.push_back("HLT_PFNoPUHT650_v4");										
	labels.push_back("HLT_PFNoPUHT700_v4");										
	labels.push_back("HLT_PFNoPUHT750_v4");										
	labels.push_back("HLT_Photon135_v7");										
	labels.push_back("HLT_Photon150_v4");										
	labels.push_back("HLT_Photon160_v4");										
	labels.push_back("HLT_Photon20_CaloIdVL_IsoL_v16");  								
	labels.push_back("HLT_Photon20_CaloIdVL_v4");									
	labels.push_back("HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_v5");							
	labels.push_back("HLT_Photon26_Photon18_v12");									
	labels.push_back("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_R9Id85_OR_CaloId10_Iso50_Mass70_v2");		
	labels.push_back("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_v5");						
	labels.push_back("HLT_Photon30_CaloIdVL_v14");									
	labels.push_back("HLT_Photon30_R9Id90_CaloId_HE10_Iso40_EBOnly_Met25_HBHENoiseCleaned_v1");  			
	labels.push_back("HLT_Photon30_R9Id90_CaloId_HE10_Iso40_EBOnly_v1"); 						
	labels.push_back("HLT_Photon30_v1"); 										
	labels.push_back("HLT_Photon300_NoHE_v5");										
	labels.push_back("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50_v6");  					
	labels.push_back("HLT_Photon36_CaloId10_Iso50_Photon22_R9Id85_v6");  						
	labels.push_back("HLT_Photon36_Photon22_v6");									
	labels.push_back("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon10_R9Id85_OR_CaloId10_Iso50_Mass80_v1");		
	labels.push_back("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v6");			
	labels.push_back("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_v5");						
	labels.push_back("HLT_Photon36_R9Id85_Photon22_CaloId10_Iso50_v6");  						
	labels.push_back("HLT_Photon36_R9Id85_Photon22_R9Id85_v4");  							
	labels.push_back("HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_v5");							
	labels.push_back("HLT_Photon40_CaloIdL_RsqMR40_Rsq0p09_MR150_v6");							
	labels.push_back("HLT_Photon40_CaloIdL_RsqMR45_Rsq0p09_MR150_v6");							
	labels.push_back("HLT_Photon40_CaloIdL_RsqMR50_Rsq0p09_MR150_v6");							
	labels.push_back("HLT_Photon50_CaloIdVL_IsoL_v17");  								
	labels.push_back("HLT_Photon50_CaloIdVL_v10");									
	labels.push_back("HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_v5");							
	labels.push_back("HLT_Photon60_CaloIdL_HT300_v4");									
	labels.push_back("HLT_Photon60_CaloIdL_MHT70_v11");  								
	labels.push_back("HLT_Photon70_CaloIdXL_PFMET100_v7");								
	labels.push_back("HLT_Photon70_CaloIdXL_PFNoPUHT400_v4");								
	labels.push_back("HLT_Photon70_CaloIdXL_PFNoPUHT500_v4");								
	labels.push_back("HLT_Photon75_CaloIdVL_v13");									
	labels.push_back("HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_v5");							
	labels.push_back("HLT_Photon90_CaloIdVL_v10");									
	labels.push_back("HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_v5");							
	labels.push_back("HLT_Physics_Parked_v1");										
	labels.push_back("HLT_Physics_v5");  										
	labels.push_back("HLT_PixelTracks_Multiplicity70_v3");								
	labels.push_back("HLT_PixelTracks_Multiplicity80_v12");								
	labels.push_back("HLT_PixelTracks_Multiplicity90_v3");								
	labels.push_back("HLT_QuadJet45_v1");										
	labels.push_back("HLT_QuadJet50_v5");										
	labels.push_back("HLT_QuadJet60_DiJet20_v6");									
	labels.push_back("HLT_QuadJet70_v6");										
	labels.push_back("HLT_QuadJet75_55_35_20_BTagIP_VBF_v8");							
	labels.push_back("HLT_QuadJet75_55_35_20_VBF_v2");									
	labels.push_back("HLT_QuadJet75_55_38_20_BTagIP_VBF_v8");							
	labels.push_back("HLT_QuadJet80_v6");										
	labels.push_back("HLT_QuadJet90_v6");										
	labels.push_back("HLT_QuadPFJet78_61_44_31_BTagCSV_VBF_v6"); 							
	labels.push_back("HLT_QuadPFJet78_61_44_31_VBF_v1"); 								
	labels.push_back("HLT_QuadPFJet82_65_48_35_BTagCSV_VBF_v6"); 							
	labels.push_back("HLT_Random_v2");										
	labels.push_back("HLT_RelIso1p0Mu20_v3");										
	labels.push_back("HLT_RelIso1p0Mu5_Ele8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT175_v5");					
	labels.push_back("HLT_RelIso1p0Mu5_Ele8_CaloIdT_TrkIdVL_Mass8_PFNoPUHT225_v5");					
	labels.push_back("HLT_RelIso1p0Mu5_v6");										
	labels.push_back("HLT_RsqMR40_Rsq0p04_v6");  									
	labels.push_back("HLT_RsqMR45_Rsq0p09_v5");  									
	labels.push_back("HLT_RsqMR55_Rsq0p09_MR150_v6");									
	labels.push_back("HLT_RsqMR60_Rsq0p09_MR150_v6");									
	labels.push_back("HLT_RsqMR65_Rsq0p09_MR150_v5");									
	labels.push_back("HLT_SingleForJet15_v4");										
	labels.push_back("HLT_SingleForJet25_v4");										
	labels.push_back("HLT_SixJet35_v6"); 										
	labels.push_back("HLT_SixJet45_v6"); 										
	labels.push_back("HLT_SixJet50_v6"); 										
	labels.push_back("HLT_Tau2Mu_ItTrack_v7");										
	labels.push_back("HLT_TrackerCalibration_v3");									
	labels.push_back("HLT_TripleEle10_CaloIdL_TrkIdVL_v18");								
	labels.push_back("HLT_TripleMu5_v19");										
	labels.push_back("HLT_ZeroBias_Parked_v1");  									
	labels.push_back("HLT_ZeroBias_v7"); 										
	labels.push_back("HLT_ZeroBiasPixel_DoubleTrack_v2");								
	labels.push_back("HLTriggerFirstPath");										
	labels.push_back("HLTriggerFinalPath");										


	std::vector<int> prescale_values;


	/// column "3" (7e33):

	prescale_values.push_back(2);
	prescale_values.push_back(1); // 278
	prescale_values.push_back(1);
	prescale_values.push_back(3);
	prescale_values.push_back(1);
	prescale_values.push_back(30);
	prescale_values.push_back(1);
	prescale_values.push_back(18);
	prescale_values.push_back(2);
	prescale_values.push_back(10);
	prescale_values.push_back(2);
	prescale_values.push_back(10);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(100);
	prescale_values.push_back(1);
	prescale_values.push_back(10);
	prescale_values.push_back(280);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(2);
	prescale_values.push_back(7);
	prescale_values.push_back(2);
	prescale_values.push_back(8);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(3);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(60);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(5);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(200);
	prescale_values.push_back(200);
	prescale_values.push_back(200);
	prescale_values.push_back(1);
	prescale_values.push_back(200);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(7);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(15);
	prescale_values.push_back(5);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(40);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(2);
	prescale_values.push_back(500);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(70);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1); // 200
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(200);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(8);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(17);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(5);
	prescale_values.push_back(10);
	prescale_values.push_back(0);
	prescale_values.push_back(55);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(125);
	prescale_values.push_back(100);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(0);
	prescale_values.push_back(7);
	prescale_values.push_back(0);
	prescale_values.push_back(40);
	prescale_values.push_back(10);
	prescale_values.push_back(10);
	prescale_values.push_back(2);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(40);
	prescale_values.push_back(1);
	prescale_values.push_back(40);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1500);
	prescale_values.push_back(1);
	prescale_values.push_back(15);
	prescale_values.push_back(15);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(4800);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(2400);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(100);
	prescale_values.push_back(150);
	prescale_values.push_back(1200);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(600);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(1);
	prescale_values.push_back(150);
	prescale_values.push_back(100);
	prescale_values.push_back(70);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(25);
	prescale_values.push_back(1);
	prescale_values.push_back(10);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(16);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(60);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(730);
	prescale_values.push_back(95);
	prescale_values.push_back(1);
	prescale_values.push_back(1750);
	prescale_values.push_back(150);
	prescale_values.push_back(34);
	prescale_values.push_back(1800);
	prescale_values.push_back(55);
	prescale_values.push_back(200);
	prescale_values.push_back(25);
	prescale_values.push_back(1);
	prescale_values.push_back(25);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(500);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(100);
	prescale_values.push_back(5000);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(10);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(8);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(100);
	prescale_values.push_back(1);
	prescale_values.push_back(1050);
	prescale_values.push_back(160);
	prescale_values.push_back(1);
	prescale_values.push_back(570);
	prescale_values.push_back(1000);
	prescale_values.push_back(30);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1050);
	prescale_values.push_back(570);
	prescale_values.push_back(85);
	prescale_values.push_back(1000);
	prescale_values.push_back(70);
	prescale_values.push_back(1);
	prescale_values.push_back(180);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(96);
	prescale_values.push_back(64);
	prescale_values.push_back(32);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(50);
	prescale_values.push_back(5);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(200);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(200);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(150);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(100);
	prescale_values.push_back(1);
	prescale_values.push_back(3);
	prescale_values.push_back(4);
	prescale_values.push_back(2);
	prescale_values.push_back(2);
	prescale_values.push_back(30);
	prescale_values.push_back(1);
	prescale_values.push_back(5);
	prescale_values.push_back(1);
	prescale_values.push_back(2);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1000);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(17);
	prescale_values.push_back(0);
	prescale_values.push_back(270);
	prescale_values.push_back(1600);
	prescale_values.push_back(1);
	prescale_values.push_back(1400);
	prescale_values.push_back(7000);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(500);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(800);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(300);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(90);
	prescale_values.push_back(10);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(330);
	prescale_values.push_back(900);
	prescale_values.push_back(30);
	prescale_values.push_back(60);
	prescale_values.push_back(20);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(150);
	prescale_values.push_back(10);
	prescale_values.push_back(100);
	prescale_values.push_back(5);
	prescale_values.push_back(1200);
	prescale_values.push_back(8000);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(35);
	prescale_values.push_back(0);
	prescale_values.push_back(1000);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(1000);
	prescale_values.push_back(1);
	prescale_values.push_back(200);
	prescale_values.push_back(550);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(20);
	prescale_values.push_back(150);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(0);
	prescale_values.push_back(0);
	prescale_values.push_back(15);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(1);
	prescale_values.push_back(30);
	prescale_values.push_back(150);
	prescale_values.push_back(0);
	prescale_values.push_back(1);
	prescale_values.push_back(1);


	int lblsize = labels.size();
	
	for (int setlbl=0; setlbl<lblsize; setlbl++)
	{
		prescales->GetXaxis()->SetBinLabel(setlbl+1,labels[setlbl].c_str());
		prescales->SetBinContent(setlbl+1,prescale_values[setlbl]);
	}
	
	
	
}	
	
void Compare_GenHLTOffline::new_file(TString save_file_name)
{
	new_file_name = save_file_name;
}

	
void Compare_GenHLTOffline::end_job()
{
	std::cout << " Writing and Closing histo file " << std::endl;

	// Now close the histogram file
	//histFile->cd();

	histfile->Write(); // "", TObject::kOverwrite);
	histfile->Close();
	std::cout << " Done! " << std::endl;
	
	  
}
