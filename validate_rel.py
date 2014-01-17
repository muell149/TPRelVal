#!/usr/bin/python
from ROOT import TFile,TTree,TH2D

def fillDict(tree,key,fillMany):
   if fillMany and tree.has_key(key):
      tree[key] += 1
   else:
      tree[key] = 1
   return tree

def fillDecayType(gen_tree):
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
      yaxis_decay_type = 0
   elif num_mu == 2:
      yaxis_decay_type = 1
   elif num_tau == 2:
      yaxis_decay_type = 2
   elif num_ele==1 and num_mu==1:
      yaxis_decay_type = 3
   elif num_ele==1 and num_tau==1:
      yaxis_decay_type = 4
   elif num_mu==1 and num_tau==1:
      yaxis_decay_type = 5
   elif num_ele==1 and (num_mu+num_tau)==0:
      yaxis_decay_type = 6
   elif num_mu==1 and (num_ele+num_tau)==0:
      yaxis_decay_type = 7
   elif num_tau==1 and (num_mu+num_ele)==0:
      yaxis_decay_type = 8
   elif (num_mu+num_ele+num_tau)==0:
      yaxis_bin_decay_type_vs_datasets = 9; }

def main():
   rel0 = '700pre9'
   rel1 = '700pre10'
   output = open(rel0+'_'+rel1+'_compare_output.log',"w")
   outputA = open(rel0+'_'+rel1+'_compare_outputA.csv',"w")
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
   
   output.write("Entries in tree0: %s\n"%entries0)
   output.write("Entries in tree1: %s\n"%entries1)

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
   path_total_hits0 = {'paths' : 0}
   path_total_hits1 = {'paths' : 0}  
   path_fired0_not1 = {'paths' : 0}
   path_fired1_not0 = {'paths' : 0}
   event_fired0_not1 = {'event' : 0}
   event_fired1_not0 = {'event' : 0}
   path_accept0 = {'path' : 0}
   path_accept1 = {'path' : 0}
   events_that_changed = {'event' : 0}
   dataset_v_path = {'path': 'dataset'}

   #for ientry in xrange(1000000):
   for ientry in xrange(entries0):
      
      hlt_tree0.GetEntry(ientry-diff0)
      hlt_tree1.GetEntry(ientry-diff1)

      #if ientry>10000:
      #   break

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

      gen_tree0.GetEntry(ientry)
      gen_tree1.GetEntry(ientry)

      if proceed and hlt_tree0.event == hlt_tree1.event and hlt_tree0.path_name == hlt_tree1.path_name:#make sure events are matching

         hlt_tree0.GetEntry(ientry-diff0)
         hlt_tree1.GetEntry(ientry-diff1)

         event = hlt_tree0.event
         name = hlt_tree0.path_name[:hlt_tree0.path_name.find('\x00')]
         
         dsmap = file_rel0.Get("newHLTOffline/map_of_trig_to_trig_types")
         for ds in xrange(45):
            if dsmap.GetBinContent(hlt_tree0.path_index,ds):
               dataset_v_path[name] = dsmap.GetYaxis().GetBinLabel(ds)

         if hlt_tree0.path_accept:

            fillDict(path_total_hits0,name,1)
            fillDict(path_accept0,name,0)

            if not hlt_tree1.path_accept:
               fillDict(event_fired0_not1,event,0)
               fillDict(path_fired0_not1,name,0)               
               fillDict(events_that_changed,event,0)

         if hlt_tree1.path_accept:

            fillDict(path_total_hits1,name,1)
            fillDict(path_accept1,name,0)

            if not hlt_tree0.path_accept:
               fillDict(event_fired1_not0,event,0)
               fillDict(path_fired1_not0,name,0)
               fillDict(events_that_changed,event,0)

   sum_path_fired0_not1 = sum(path_fired0_not1.values())
   sum_path_fired1_not0 = sum(path_fired1_not0.values())
   sum_event_fired0_not1 = sum(event_fired0_not1.values())
   sum_event_fired1_not0 = sum(event_fired1_not0.values())
   sum_path_accpt0 = sum(path_accept0.values())
   sum_path_accpt1 = sum(path_accept1.values())
   sum_changes = sum(events_that_changed.values())

   output.write("Events in rel0: %s\n"%evt0)
   output.write("Events in rel1: %s\n"%evt1)
   output.write("Paths in rel0: %s\n"%pidx0)
   output.write("Paths in rel1: %s\n"%pidx1)
   output.write("events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_event_fired0_not1}) 
   output.write("events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel1,"2":rel0,"3":sum_event_fired1_not0})
   output.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel0,"2":sum_path_accpt0})
   output.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel1,"2":sum_path_accpt1})
   output.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3": sum_path_fired0_not1})
   output.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel1,"2":rel0,"3": sum_path_fired1_not0})
   output.write("# of events that changed from %(1)s to %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_changes})
   output.write("total # of events in %(1)s: %(2)s\n"%{"1":rel0,"2":entries0/(pidx0+1)})
   output.write("total # of events in %(1)s: %(2)s\n"%{"1":rel1,"2":entries1/(pidx1+1)})
   output.write(" \n")
   output.write("Fractional change in events from %(1)s to %(2)s: %(3)s  percent\n"%{"1":rel0,"2":rel1,"3":100.00*sum_changes*(pidx0+1)/entries0})
   output.write(" \n")
   outputA.write("HLT PATH,counts in rel0,counts in rel1,difference\n")

   for (keyA, counts) in path_total_hits0.items():
      if path_total_hits1.has_key(keyA) and abs(counts-path_total_hits1[keyA]) >= 1:
         outputA.write("%(1)s, %(2)s, %(3)s, %(4)s\n"%{"1":keyA, "2":counts,"3":path_total_hits1[keyA],"4":counts-path_total_hits1[keyA]})

   output.close()

            

if __name__=='__main__':
   main()
