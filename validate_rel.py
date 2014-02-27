#!/usr/bin/python
from ROOT import TFile,TTree,TH2F, TCanvas
import sys, time, array, subprocess, commands
 
start_time = time.time()

def writeTable(file,dict_num,dict_str):
   file.write("HLT PATH,dataset,module,total counts in rel0,total counts in rel1,change, absolute change, events firing rel0 not rel1, events firing rel1 not rel0, sum of changed events, change of event change, absolute event change\n")
   for key in dict_num.keys(): 
      if abs(dict_num[key][0]-dict_num[key][1]) > 0 or dict_num[key][3]+dict_num[key][2] > 0:

         file.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s, %(6)s, %(7)s, %(8)s, %(9)s, %(10)s, %(11)s, %(12)s\n"%{"1":key, "2":dict_str[key][0],"3":dict_str[key][1],"4":dict_num[key][0],"5":dict_num[key][1],"6":dict_num[key][1]-dict_num[key][0], "7":abs(dict_num[key][1]-dict_num[key][0]),"8":dict_num[key][2],"9":dict_num[key][3],"10":dict_num[key][2]+dict_num[key][3],"11":dict_num[key][3]-dict_num[key][2],"12":abs(dict_num[key][3]-dict_num[key][2])})
   file.close()
   return file

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
   rel1 = '700pre13'
   output_file_dir = '/afs/cern.ch/user/m/muell149/steam_validation/'+rel0+'_'+rel1+'/'
   #output_file_dir = ''

   
#   if commands.getstatusoutput("mkdir "+output_file_dir)[0] != 0:
#      ow = raw_input('output directory '+output_file_dir+' already exists, overwrite? (y/n):')
#      if ow == 'y':
#         subprocess.call("mkdir "+output_file_dir,shell=True)
#      else:
#         sys.exit()

   subprocess.call("mkdir "+output_file_dir,shell=True)
   output_log = open(output_file_dir+rel0+'_'+rel1+'_compare_output.log',"w")

   output_table = open(output_file_dir+rel0+'_'+rel1+'_compare_output_table.csv',"w")
   output_DoubleEle = open(output_file_dir+'DoubleEle.csv',"w")
   output_DoubleMu = open(output_file_dir+'DoubleMu.csv',"w")
   output_DoubleTau = open(output_file_dir+'DoubleTau.csv',"w")
   output_EleMu = open(output_file_dir+'EleMu.csv',"w")
   output_EleTau = open(output_file_dir+'EleTau.csv',"w")
   output_MuTau = open(output_file_dir+'EleTau.csv',"w")
   output_SingleEle = open(output_file_dir+'SingleEle.csv',"w")
   output_SingleMu = open(output_file_dir+'SingleMu.csv',"w")
   output_SingleTau = open(output_file_dir+'SingleTau.csv',"w")
   output_AllHad = open(output_file_dir+'AllHad.csv',"w")
   file_list = [output_table,output_DoubleEle,output_DoubleMu,output_DoubleTau,output_EleMu,output_EleTau,output_MuTau,output_SingleEle,output_SingleMu,output_SingleTau,output_AllHad]

   #output_event_changes = open('event_changes.csv',"w")
   file0 = '/afs/cern.ch/user/m/muell149/work/HLTONLINE/root_input_files/'+rel0+'.root'
   file1 = '/afs/cern.ch/user/m/muell149/work/HLTONLINE/root_input_files/'+rel1+'.root'
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
   rel0not1_dt_dict = {}#{'decay type':0}
   rel1not0_dt_dict = {}#{'decay type':0}
   for k in xrange(10):
      rel0not1_dt_dict[k+1] = 0
      rel1not0_dt_dict[k+1] = 0
   
   path_str_dict = {}#{'path':['dataset','module']}
   event_dict = {}#{'event':['changed','fired 0not1','fired 1not0']}

   output_list = [{} for i in xrange(11)]
   
   
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

   fired_rel1 = fired_rel1_not0_decayType_vs_dataset.Clone()
   fired_rel0 = fired_rel0_not1_decayType_vs_dataset.Clone()
   
   #set some branch addresses
   hlt_event0 = array.array("i",[0])
   gen_event0 = array.array("i",[0])
   hlt_path_accept0 = array.array("i",[0])
   hlt_path_index0 = array.array("i",[0])
   hlt_path_name0 = array.array("c",[" "]*100)
   hlt_module0 = array.array("c",[" "]*150)

   hlt_event1= array.array("i",[0])
   gen_event1= array.array("i",[0])
   hlt_path_accept1= array.array("i",[0])
   hlt_path_index1= array.array("i",[0])
   hlt_path_name1= array.array("c",[" "]*100)
   hlt_module1= array.array("c",[" "]*150)

   hlt_tree0.SetBranchAddress("event",hlt_event0)
   gen_tree0.SetBranchAddress("event",gen_event0)
   hlt_tree0.SetBranchAddress("path_accept",hlt_path_accept0)
   hlt_tree0.SetBranchAddress("path_index",hlt_path_index0)
   hlt_tree0.SetBranchAddress("path_name",hlt_path_name0)
   hlt_tree0.SetBranchAddress("last_module_with_saved_tags_label",hlt_module0)

   hlt_tree1.SetBranchAddress("event",hlt_event1)
   gen_tree1.SetBranchAddress("event",gen_event1)
   hlt_tree1.SetBranchAddress("path_accept",hlt_path_accept1)
   hlt_tree1.SetBranchAddress("path_index",hlt_path_index1)
   hlt_tree1.SetBranchAddress("path_name",hlt_path_name1)
   hlt_tree1.SetBranchAddress("last_module_with_saved_tags_label",hlt_module1)
############### event loop ################
   for ientry in xrange(entries0):
      
      hlt_tree0.GetEntry(ientry-diff0)
      hlt_tree1.GetEntry(ientry-diff1)
            
      #if ientry>500:#100000:
      #   break

      if hlt_event0[0] > evt0:
         evt0 = hlt_event0[0]
      if hlt_event1[0] > evt1:
         evt1 = hlt_event1[0]
      if hlt_path_index0[0] > pidx0:
         pidx0 = hlt_path_index0[0]
      if hlt_path_index1[0] > pidx1:
         pidx1 = hlt_path_index1[0]
      #####################################################################################
      ######################in case trees are not the same size############################
      #####################################################################################
      if hlt_path_index0[0] != hlt_path_index1[0]:
         proceed = False
         if hlt_path_index0[0] == 0:
            idx0 = 1
         elif hlt_path_index1[0] == 0:
            idx1 = 1
      if idx1 == 1 and idx0 ==1:
         proceed = True
         #reset these counters
         idx1 = 0
         idx0 = 0
         if hlt_path_index0[0] > hlt_path_index1[0]:
            diff0 = diff0 + hlt_path_index0[0]
         else:
            diff1 = diff1 + hlt_path_index1[0]
      #####################################################################################
      #####################################################################################    
      #####################################################################################
      
      if proceed and hlt_event0[0] == hlt_event1[0] and hlt_path_name0.tostring() == hlt_path_name1.tostring():#make sure events are matching

         hlt_tree0.GetEntry(ientry-diff0)
         hlt_tree1.GetEntry(ientry-diff1)
         gen_tree0.GetEntry(ientry-diff0)
         gen_tree1.GetEntry(ientry-diff1)

         #print diff0, diff1
         
         event = hlt_event0[0]
         name = hlt_path_name0.tostring()[:hlt_path_name0.tostring().find('\x00')]

         gen_tree0.GetEntry(get_gen_entries0_from_event[event])
         gen_tree1.GetEntry(get_gen_entries0_from_event[event])

         module = hlt_module0.tostring()[:hlt_module0.tostring().find('\x00')]
                  
         if hlt_event0[0] != gen_event0[0]:
            print "Gen level mismatch!!!"
            break
            
###################### stuff for primary datasets ################
         for ds in xrange(1,num_PD):
            if dsmap.GetBinContent(hlt_path_index0[0],ds):
               path_str_dict[name] = [dsmap.GetYaxis().GetBinLabel(ds),'']
               datasetIDX_v_path[name] = ds
        
         if not path_str_dict.has_key(name):#check to see if path has a PD, if not, add "NONE" 
            path_str_dict[name] = ["NONE",'']
            datasetIDX_v_path[name] = num_PD
         
###################### stuff for primary datasets ################
 
         path_str_dict[name][1] = module
         decaytype = GetDecayType(gen_tree0)
                   
         if hlt_path_accept0[0]:
            fired_rel0.Fill(datasetIDX_v_path[name]-1,decaytype-1)
            try:
               output_list[0][name][0] += 1
            except KeyError:
               output_list[0][name] = [1,0,0,0]
            try:
               output_list[decaytype][name][0] += 1
            except KeyError:
               output_list[decaytype][name] = [1,0,0,0]
              
            if not hlt_path_accept1[0]:
               output_list[0][name][2] += 1
               output_list[decaytype][name][2] += 1
               try:
                  event_dict[event][0] += 1
                  event_dict[event][1] += 1
               except KeyError:
                  event_dict[event] = [1,1,0]

               #try:
               #   rel0not1_ds_dict[path_str_dict[name]] += 1
               #except KeyError:
               #   rel0not1_ds_dict[path_str_dict[name]] = 1
               
               rel0not1_dt_dict[decaytype] += 1
               
               fired_rel0_not1_decayType_vs_dataset.Fill(datasetIDX_v_path[name]-1,decaytype-1)
               #output_event_changes.write("%(1)s,%(2)s,%(3)s,1,0\n"%{"1":fired_rel0_not1_decayType_vs_dataset.GetYaxis().GetBinLabel(decaytype),"2":dataset_v_path[name],"3":name})

         if hlt_path_accept1[0]:
            fired_rel1.Fill(datasetIDX_v_path[name]-1,decaytype-1)
            
            try:
               output_list[0][name][1] += 1
            except KeyError:
               output_list[0][name] = [0,1,0,0]
            try:
               output_list[decaytype][name][1] += 1
            except KeyError:
               output_list[decaytype][name] = [0,1,0,0]
               
            if not hlt_path_accept0[0]:
               output_list[0][name][3] += 1
               output_list[decaytype][name][3] += 1
               try:
                  event_dict[event][0] += 1
                  event_dict[event][2] += 1
               except KeyError:
                  event_dict[event] = [1,0,1]

               #try:
               #   rel1not0_ds_dict[path_str_dict[name]] += 1
               #except KeyError:
               #   rel1not0_ds_dict[path_str_dict[name]]= 1
               rel1not0_dt_dict[decaytype] += 1
                                 
               fired_rel1_not0_decayType_vs_dataset.Fill(datasetIDX_v_path[name]-1,decaytype-1)
               #output_event_changes.write("%(1)s,%(2)s,%(3)s,0,1\n"%{"1":fired_rel1_not0_decayType_vs_dataset.GetYaxis().GetBinLabel(decaytype),"2":dataset_v_path[name],"3":name})

                              

               
   #draw hists
   hist_file = TFile(output_file_dir+"hist_file_"+rel0+"_"+rel1+".root","recreate")
   title = fired_rel0_not1_decayType_vs_dataset.GetName()
   can1 = TCanvas(title,title,1)
   fired_rel0_not1_relative_decayType_vs_dataset = fired_rel0_not1_decayType_vs_dataset.Clone()
   fired_rel1_not0_relative_decayType_vs_dataset = fired_rel1_not0_decayType_vs_dataset.Clone()
   
   fired_rel0_not1_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel0_not1_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel0_not1_decayType_vs_dataset.LabelsOption("v")
   fired_rel0_not1_decayType_vs_dataset.Draw("COLZ")
   can1.SetBottomMargin(0.19)
   can1.SetGrid()
   can1.SaveAs(output_file_dir+"decayType_vs_dataset_0not1.png")
   fired_rel0_not1_decayType_vs_dataset.SetOption("COLZ")
   fired_rel0_not1_decayType_vs_dataset.Write()
   #can1.SaveAs("decayType_vs_dataset_0not1.root")

   title = fired_rel1_not0_decayType_vs_dataset.GetName()
   can2 = TCanvas(title,title,1)
   fired_rel1_not0_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel1_not0_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel1_not0_decayType_vs_dataset.LabelsOption("v")
   fired_rel1_not0_decayType_vs_dataset.Draw("COLZ")
   can2.SetBottomMargin(0.19)
   can2.SetGrid()
   can2.SaveAs(output_file_dir+"decayType_vs_dataset_1not0.png")
   fired_rel1_not0_decayType_vs_dataset.SetOption("COLZ")
   fired_rel1_not0_decayType_vs_dataset.Write()
   #can2.SaveAs("decayType_vs_dataset_1not0.root")

   fired_rel0_not1_relative_decayType_vs_dataset.Divide(fired_rel0)
   fired_rel1_not0_relative_decayType_vs_dataset.Divide(fired_rel1)

   title = fired_rel0_not1_relative_decayType_vs_dataset.GetName()
   can3 = TCanvas(title,title,1)
   fired_rel0_not1_relative_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel0_not1_relative_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel0_not1_relative_decayType_vs_dataset.LabelsOption("v")
   fired_rel0_not1_relative_decayType_vs_dataset.Draw("COLZ")
   can3.SetBottomMargin(0.19)
   can3.SetGrid()
   can3.SaveAs(output_file_dir+"decayType_vs_dataset_0not1_relative.png")
   fired_rel0_not1_relative_decayType_vs_dataset.SetOption("COLZ")
   fired_rel0_not1_relative_decayType_vs_dataset.Write()

   title = fired_rel1_not0_relative_decayType_vs_dataset.GetName()
   can4 = TCanvas(title,title,1)
   fired_rel1_not0_relative_decayType_vs_dataset.LabelsDeflate("X")
   fired_rel1_not0_relative_decayType_vs_dataset.LabelsDeflate("Y")
   fired_rel1_not0_relative_decayType_vs_dataset.LabelsOption("v")
   fired_rel1_not0_relative_decayType_vs_dataset.Draw("COLZ")
   can4.SetBottomMargin(0.19)
   can4.SetGrid()
   can4.SaveAs(output_file_dir+"decayType_vs_dataset_1not0_relative.png")
   fired_rel1_not0_relative_decayType_vs_dataset.SetOption("COLZ")
   fired_rel1_not0_relative_decayType_vs_dataset.Write()

   #####
   sum_path_fired0_not1 = len([arr[2] for arr in output_list[0].values() if arr[2]!=0])
   sum_path_fired1_not0 = len([arr[3] for arr in output_list[0].values() if arr[3]!=0])
   sum_event_fired0_not1 = len([arr[1] for arr in event_dict.values() if arr[1]!=0])
   sum_event_fired1_not0 = len([arr[2] for arr in event_dict.values() if arr[2]!=0])
   sum_path_accpt0 = len([arr[0] for arr in output_list[0].values() if arr[0]!=0])
   sum_path_accpt1 = len([arr[1] for arr in output_list[0].values() if arr[1]!=0])
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
   output_log.write("Events that gained counts by decay type\n")
   output_log.write("Double Ele: %s\n"%rel1not0_dt_dict[1])
   output_log.write("Double Mu: %s\n"%rel1not0_dt_dict[2])
   output_log.write("Double Tau: %s\n"%rel1not0_dt_dict[3])
   output_log.write("Ele + Mu: %s\n"%rel1not0_dt_dict[4])
   output_log.write("Ele + Tau: %s\n"%rel1not0_dt_dict[5])
   output_log.write("Mu + Tau: %s\n"%rel1not0_dt_dict[6])
   output_log.write("Single Ele: %s\n"%rel1not0_dt_dict[7])
   output_log.write("Single Mu: %s\n"%rel1not0_dt_dict[8])
   output_log.write("Single Tau: %s\n"%rel1not0_dt_dict[9])
   output_log.write("All Had: %s\n"%rel1not0_dt_dict[10])
   output_log.write(" \n")
   output_log.write("Events that lost counts by decay type\n")
   output_log.write("Double Ele: %s\n"%rel0not1_dt_dict[1])
   output_log.write("Double Mu: %s\n"%rel0not1_dt_dict[2])
   output_log.write("Double Tau: %s\n"%rel0not1_dt_dict[3])
   output_log.write("Ele + Mu: %s\n"%rel0not1_dt_dict[4])
   output_log.write("Ele + Tau: %s\n"%rel0not1_dt_dict[5])
   output_log.write("Mu + Tau: %s\n"%rel0not1_dt_dict[6])
   output_log.write("Single Ele: %s\n"%rel0not1_dt_dict[7])
   output_log.write("Single Mu: %s\n"%rel0not1_dt_dict[8])
   output_log.write("Single Tau: %s\n"%rel0not1_dt_dict[9])
   output_log.write("All Had: %s\n"%rel0not1_dt_dict[10])
   output_log.write(" \n")
   output_log.write("Fractional change in events from %(1)s to %(2)s: %(3)s  percent\n"%{"1":rel0,"2":rel1,"3":100.00*sum_changes*(pidx0+1)/entries0})
   exectime = time.time()-start_time
   output_log.write("Exectution time = %f\n"%exectime)
   output_log.close()

   for i in xrange(11):
      writeTable(file_list[i],output_list[i],path_str_dict)

   #output_event_changes.close()
   print "Execution time = ", time.time()-start_time," sec"

   
   

#if __name__=='__main__':
#   main()
