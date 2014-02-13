#!/usr/bin/python
from ROOT import TFile,TTree,TH2F, TCanvas
import time
 
start_time = time.time()

def GetDecayType(gen_tree):
   num_ele=0
   num_mu=0
   num_tau=0
   for obj in xrange(100):
      if abs(gen_tree.gen_ID[obj]) == 11:
         num_ele += 1
      if abs(gen_tree.gen_ID[obj]) == 13:
         num_mu += 1
      if abs(gen_tree.gen_ID[obj]) == 15:
         num_tau += 1

   if num_ele == 2:
      decay_type = 1
   elif num_mu == 2:
      decay_type = 2
   elif num_tau == 2:
      decay_type = 3
   elif num_ele==1 and num_mu==1:
      decay_type = 4
   elif num_ele==1 and num_tau==1:
      decay_type = 5
   elif num_mu==1 and num_tau==1:
      decay_type = 6
   elif num_ele==1 and (num_mu+num_tau)==0:
      decay_type = 7
   elif num_mu==1 and (num_ele+num_tau)==0:
      decay_type = 8
   elif num_tau==1 and (num_mu+num_ele)==0:
      decay_type = 9
   elif num_mu+num_ele+num_tau==0:
      decay_type = 10

   return decay_type

#def main():
if __name__=='__main__':
   
   rel0 = '700pre11'
   rel1 = '700pre12'
   output_log = open(rel0+'_'+rel1+'_compare_output.log',"w")
   output_table = open(rel0+'_'+rel1+'_compare_output_table.csv',"w")
   #output_event_changes = open('event_changes.csv',"w")
   file0 = '/afs/cern.ch/user/m/muell149/workb/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/'+rel0+'.root'
   file1 = '/afs/cern.ch/user/m/muell149/workb/HLTONLINE/CMSSW_7_0_0_pre1/src/DQMOffline/Trigger/test/'+rel1+'.root'
   file_rel0 = TFile(file0)
   file_rel1 = TFile(file1)
   hlt_tree0 = file_rel0.Get('genparticles/the_HLT_tree')
   hlt_tree1 = file_rel1.Get('genparticles/the_HLT_tree')
   gen_tree0 = file_rel0.Get('genparticles/the_gen_tree')
   gen_tree1 = file_rel1.Get('genparticles/the_gen_tree')
      
   entries0 = hlt_tree0.GetEntriesFast()
   entries1 = hlt_tree1.GetEntriesFast()

   gen_entries0 = gen_tree0.GetEntriesFast()
   gen_entries1 = gen_tree1.GetEntriesFast()
   
   output_log.write("Entries in tree0: %s\n"%entries0)
   output_log.write("Entries in tree1: %s\n"%entries1)
   #output_event_changes.write("Decay Type, Dataset, Path, 0not1,1not0\n")
   
   #initialize some counters
   evt0 = 0
   evt1 = 0
   pidx0 = 0
   pidx1 = 0
   idx0 = 0
   idx1 = 0
   diff0 = 0
   diff1 = 0
   proceed = True
   #initialize dicts
   accepted_paths = {'path':0}
   datasetIDX_v_path = {'path': 0}
   get_gen_entries0_from_event = {0:0} #event number : entry number
   get_gen_entries1_from_event = {0:0}
   #rel0not1_ds_dict = {'dataset':0}
   #rel1not0_ds_dict ={'dataset':0}
   #rel0not1_dt_dict = {'decay type':0}
   #rel1not0_dt_dict ={'decay type':0}
   
   path_num_dict = {}#{'path':['total_hits0','total_hits1','fired 0not1','fired 1not0',]}
   path_str_dict = {}#{'path':['dataset','module']}
   event_dict = {}#{'event':['changed','fired 0not1','fired 1not0']}

   #big_dict_list = [{} for i in range(10)]

   #make hists
   fired_rel0_not1_decayType_vs_dataset = TH2F("fired_rel0_didntfire_rel1_decay_type_vs_datasets","fired_rel0_didntfire_rel1_decay_type_vs_datasets",44,0,44,11,0,11)
   #fired_rel1_not0_decayType_vs_dataset= TH2F("fired_rel1_didntfire_rel0_decay_type_vs_datasets","fired_rel1_didntfire_rel0_decay_type_vs_datasets",44,0,44,11,0,11)
   #set some bin labels
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(1,"Double Ele")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(2,"Double Mu")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(3,"Double Tau")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(4,"Ele + Mu")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(5,"Ele + Tau")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(6,"Mu + Tau")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(7,"Single Ele")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(8,"Single Mu")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(9,"Single Tau")
   fired_rel0_not1_decayType_vs_dataset.GetYaxis().SetBinLabel(10,"All Had")
   fired_rel0_not1_decayType_vs_dataset.SetTitle("lost counts going from " + rel0 + " to " + rel1 + ", ttbar decay vs dataset")
   fired_rel0_not1_decayType_vs_dataset.GetXaxis().SetLabelSize(0.03)
   fired_rel0_not1_decayType_vs_dataset.SetStats(0)
      
   #apply gen matching
   for genentry in xrange(gen_entries0):
      gen_tree0.GetEntry(genentry)
      get_gen_entries0_from_event[gen_tree0.event] = genentry

   #stuff for primary datasets
   dsmap = file_rel0.Get("newHLTOffline/map_of_trig_to_trig_types")
   num_PD = 43 #number of primary datsets - 1
   fired_rel0_not1_decayType_vs_dataset.GetXaxis().SetBinLabel(num_PD,"NONE")#need to add these since some paths don't have a dataset
   
   for ds in xrange(1,num_PD):
      fired_rel0_not1_decayType_vs_dataset.GetXaxis().SetBinLabel(ds,dsmap.GetYaxis().GetBinLabel(ds))

   #clone histogram for gained counts
   fired_rel1_not0_decayType_vs_dataset = fired_rel0_not1_decayType_vs_dataset.Clone()
   fired_rel1_not0_decayType_vs_dataset.SetName("fired_rel1_didntfire_rel0_decay_type_vs_datasets")
   fired_rel1_not0_decayType_vs_dataset.SetTitle("gained counts going from " + rel0 + " to " + rel1 + ", ttbar decay vs dataset")

############### event loop ################
   for ientry in xrange(entries0):
      
      hlt_tree0.GetEntry(ientry-diff0)
      hlt_tree1.GetEntry(ientry-diff1)

      if ientry>1000:#100000:
         break

      if hlt_tree0.event > evt0:
         evt0 = hlt_tree0.event
      if hlt_tree1.event > evt1:
         evt1 = hlt_tree1.event
      if hlt_tree0.path_index > pidx0:
         pidx0 = hlt_tree0.path_index
      if hlt_tree1.path_index > pidx1:
         pidx1 = hlt_tree1.path_index
      #####################################################################################
      ######################in case trees are not the same size############################
      #####################################################################################
      if hlt_tree0.path_index != hlt_tree1.path_index:
         proceed = False
         if hlt_tree0.path_index == 0:
            idx0 = 1
         elif hlt_tree1.path_index == 0:
            idx1 = 1
      if idx1 == 1 and idx0 ==1:
         proceed = True
         #reset these counters
         idx1 = 0
         idx0 = 0
         if hlt_tree0.path_index > hlt_tree1.path_index:
            diff0 = diff0 + hlt_tree0.path_index
         else:
            diff1 = diff1 + hlt_tree1.path_index
      #####################################################################################
      #####################################################################################    
      #####################################################################################
      
      if proceed and hlt_tree0.event == hlt_tree1.event and hlt_tree0.path_name == hlt_tree1.path_name:#make sure events are matching

         hlt_tree0.GetEntry(ientry-diff0)
         hlt_tree1.GetEntry(ientry-diff1)
         gen_tree0.GetEntry(ientry-diff0)
         gen_tree1.GetEntry(ientry-diff1)

         event = hlt_tree0.event
         name = hlt_tree0.path_name[:hlt_tree0.path_name.find('\x00')]

         gen_tree0.GetEntry(get_gen_entries0_from_event[event])
         gen_tree1.GetEntry(get_gen_entries0_from_event[event])

         module = hlt_tree0.last_module_with_saved_tags_label[:hlt_tree0.last_module_with_saved_tags_label.find('\x00')]
                  
         if hlt_tree0.event != gen_tree0.event:
            print "Gen level mismatch!!!"
            break
            
###################### stuff for primary datasets ################
         for ds in xrange(1,num_PD):
            if dsmap.GetBinContent(hlt_tree0.path_index,ds):
               path_str_dict[name] = [dsmap.GetYaxis().GetBinLabel(ds),'']
               datasetIDX_v_path[name] = ds
        
         if not path_str_dict.has_key(name):#check to see if path has a PD, if not, add "NONE" 
            path_str_dict[name] = ["NONE",'']
            datasetIDX_v_path[name] = num_PD
###################### stuff for primary datasets ################

         path_str_dict[name][1] = module
         decaytype1 = GetDecayType(gen_tree0)
         decaytype2 = GetDecayType(gen_tree1)
         if decaytype1 != decaytype2:
            print "THE DECAY TYPES DO NOT MATCH UGHHHHHHH"
         
         
         if hlt_tree0.path_accept:
            try:
               path_num_dict[name][0] += 1
            except KeyError:
               path_num_dict[name] = [1,0,0,0]
                           
            if not hlt_tree1.path_accept:
               path_num_dict[name][2] += 1
               try:
                  event_dict[event][0] += 1
                  event_dict[event][1] += 1
               except KeyError:
                  event_dict[event] = [1,1,0]
               decaytype = GetDecayType(gen_tree0)
               fired_rel0_not1_decayType_vs_dataset.Fill(datasetIDX_v_path[name]-1,decaytype-1)
               #output_event_changes.write("%(1)s,%(2)s,%(3)s,1,0\n"%{"1":fired_rel0_not1_decayType_vs_dataset.GetYaxis().GetBinLabel(decaytype),"2":dataset_v_path[name],"3":name})
               
         if hlt_tree1.path_accept:
            path_num_dict[name][1] += 1
                        
            if not hlt_tree0.path_accept:
               path_num_dict[name][3] += 1
               try:
                  event_dict[event][0] += 1
                  event_dict[event][2] += 1
               except KeyError:
                  event_dict[event] = [1,0,1]
               fired_rel1_not0_decayType_vs_dataset.Fill(datasetIDX_v_path[name]-1,decaytype-1)
               #output_event_changes.write("%(1)s,%(2)s,%(3)s,0,1\n"%{"1":fired_rel1_not0_decayType_vs_dataset.GetYaxis().GetBinLabel(decaytype),"2":dataset_v_path[name],"3":name})

                              

   #draw hists
   title = fired_rel0_not1_decayType_vs_dataset.GetName()
   can1 = TCanvas(title,title,1)
   fired_rel0_not1_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel0_not1_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel0_not1_decayType_vs_dataset.LabelsOption("v")
   fired_rel0_not1_decayType_vs_dataset.Draw("COLZ")
   can1.SetBottomMargin(0.19)
   can1.SetGrid()
   can1.SaveAs("decayType_vs_dataset_0not1.png")
   can1.SaveAs("decayType_vs_dataset_0not1.root")

   title = fired_rel1_not0_decayType_vs_dataset.GetName()
   can2 = TCanvas(title,title,1)
   fired_rel1_not0_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel1_not0_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel1_not0_decayType_vs_dataset.LabelsOption("v")
   fired_rel1_not0_decayType_vs_dataset.Draw("COLZ")
   can2.SetBottomMargin(0.19)
   can2.SetGrid()
   can2.SaveAs("decayType_vs_dataset_1not0.png")
   can2.SaveAs("decayType_vs_dataset_1not0.root")
   
   sum_path_fired0_not1 = len([arr[2] for arr in path_num_dict.values() if arr[2]!=0])
   sum_path_fired1_not0 = len([arr[3] for arr in path_num_dict.values() if arr[3]!=0])
   sum_event_fired0_not1 = len([arr[1] for arr in event_dict.values() if arr[1]!=0])
   sum_event_fired1_not0 = len([arr[2] for arr in event_dict.values() if arr[2]!=0])
   sum_path_accpt0 = len([arr[0] for arr in path_num_dict.values() if arr[0]!=0])
   sum_path_accpt1 = len([arr[1] for arr in path_num_dict.values() if arr[1]!=0])
   sum_changes = len([arr[0] for arr in event_dict.values() if arr[0]!=0])

   output_log.write("Events in rel0: %s\n"%evt0)
   output_log.write("Events in rel1: %s\n"%evt1)
   output_log.write("Paths in rel0: %s\n"%pidx0)
   output_log.write("Paths in rel1: %s\n"%pidx1)
   output_log.write("events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_event_fired0_not1}) 
   output_log.write("events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel1,"2":rel0,"3":sum_event_fired1_not0})
   output_log.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel0,"2":sum_path_accpt0})
   output_log.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel1,"2":sum_path_accpt1})
   output_log.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3": sum_path_fired0_not1})
   output_log.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel1,"2":rel0,"3": sum_path_fired1_not0})
   output_log.write("# of events that changed from %(1)s to %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_changes})
   output_log.write("total # of events in %(1)s: %(2)s\n"%{"1":rel0,"2":entries0/(pidx0+1)})
   output_log.write("total # of events in %(1)s: %(2)s\n"%{"1":rel1,"2":entries1/(pidx1+1)})
   output_log.write(" \n")
   output_log.write("Fractional change in events from %(1)s to %(2)s: %(3)s  percent\n"%{"1":rel0,"2":rel1,"3":100.00*sum_changes*(pidx0+1)/entries0})
   exectime = time.time()-start_time
   output_log.write("Exectution time = %f\n"%exectime)
   output_log.close()
   output_table.write("HLT PATH,dataset,module,total counts in rel0,total counts in rel1,change, absolute change, relative change,events firing rel0 not rel1, events firing rel1 not rel0, sum of changed events, change of event change, absolute event change\n")


   
   for key in path_num_dict.keys(): 
      if abs(path_num_dict[key][0]-path_num_dict[key][1]) > 0 or path_num_dict[key][3]+path_num_dict[key][2] > 0:
         output_table.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s, %(6)s, %(7)s, %(8)s, %(9)s, %(10)s, %(11)s, %(12)s, %(13)s\n"%{"1":key, "2":path_str_dict[key][0],"3":path_str_dict[key][1],"4":path_num_dict[key][0],"5":path_num_dict[key][1],"6":path_num_dict[key][1]-path_num_dict[key][0], "7":abs(path_num_dict[key][1]-path_num_dict[key][0]),"8":abs(path_num_dict[key][1]-path_num_dict[key][0])/path_num_dict[key][0],"9":path_num_dict[key][2],"10":path_num_dict[key][3],"11":path_num_dict[key][2]+path_num_dict[key][3],"12":path_num_dict[key][3]-path_num_dict[key][2],"13":abs(path_num_dict[key][3]-path_num_dict[key][2])})

   output_table.close()
   #output_event_changes.close()
   print "Execution time = ", time.time()-start_time," sec"
            

#if __name__=='__main__':
#   main()
