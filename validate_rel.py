from ROOT import TFile,TTree,TH2D


def fillDictMany(has_key_already,tree,key):
   if has_key_already:
      tree[key] += 1
   else:
      tree[key] = 1
   return tree

def fillDictOnce(has_key_already,tree,key):
   if not has_key_already:
      tree[key] = 1
   return tree


rel0 = '620_pre8'
rel1 = '620'
output = open(rel0+'_'+rel1+'_compare_output.log',"wb")
file0 = '/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/'+rel0+'.root'
file1 = '/afs/cern.ch/user/m/muell149/work/muell149/HLTONLINE/CMSSW_6_2_0_pre6/src/DQMOffline/Trigger/test/'+rel1+'.root'
file_rel0 = TFile(file0)
file_rel1 = TFile(file1)
hlt_tree0 = file_rel0.Get('genparticles/the_HLT_tree')
hlt_tree1 = file_rel1.Get('genparticles/the_HLT_tree')
gen_tree0 = file_rel0.Get('genparticles/the_gen_tree')
gen_tree1 = file_rel1.Get('genparticles/the_gen_tree')

entries0 = hlt_tree0.GetEntriesFast()
entries1 = hlt_tree1.GetEntriesFast()
print >> output, "Entries in tree0: ", entries0
print >> output, "Entries in tree1: ", entries1

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
      name = hlt_tree0.path_name
            
      dsmap = file_rel0.Get("newHLTOffline/map_of_trig_to_trig_types")
      for ds in xrange(45):
         if dsmap.GetBinContent(hlt_tree0.path_index,ds):
            dataset_v_path[name] = dsmap.GetYaxis().GetBinLabel(ds)

      if hlt_tree0.path_accept:

         fillDictMany(path_total_hits0.has_key(name),path_total_hits0,name)
         fillDictOnce(path_accept0.has_key(name),path_accept0,name)
                       
         if not hlt_tree1.path_accept:
            fillDictOnce(event_fired0_not1.has_key(event),event_fired0_not1,event)
            fillDictOnce(path_fired0_not1.has_key(name),path_fired0_not1,name)               
            fillDictOnce(events_that_changed.has_key(event),events_that_changed,event)

      if hlt_tree1.path_accept:

         fillDictMany(path_total_hits1.has_key(name),path_total_hits1,name)
         fillDictOnce(path_accept1.has_key(name),path_accept1,name)
                                      
         if not hlt_tree0.path_accept:
            fillDictOnce(event_fired1_not0.has_key(event),event_fired1_not0,event)
            fillDictOnce(path_fired1_not0.has_key(name),path_fired1_not0,name)
            fillDictOnce(events_that_changed.has_key(event),events_that_changed,event)
         


      
#    if hlt_tree0.event > evt0:
#       evt0 = hlt_tree0.event
#    if hlt_tree1.event > evt1:
#       evt1 = hlt_tree1.event
#    if hlt_tree0.path_index > pidx0:
#       pidx0 = hlt_tree0.path_index
#    if hlt_tree1.path_index > pidx1:
#       pidx1 = hlt_tree1.path_index

sum_path_fired0_not1 = sum(path_fired0_not1.values())
sum_path_fired1_not0 = sum(path_fired1_not0.values())
sum_event_fired0_not1 = sum(event_fired0_not1.values())
sum_event_fired1_not0 = sum(event_fired1_not0.values())
sum_path_accpt0 = sum(path_accept0.values())
sum_path_accpt1 = sum(path_accept1.values())
sum_changes = sum(events_that_changed.values())


print >> output, "Events in rel0: ", evt0
print >> output, "Events in rel1: ", evt1
print >> output, "Paths in rel0: ", pidx0
print >> output, "Paths in rel1: ", pidx1    
print >> output, "events with paths that fired in "+rel0+" and didn't fire in "+rel1, sum_event_fired0_not1
print >> output, "events with paths that fired in "+rel1+" and didn't fire in "+rel0, sum_event_fired1_not0
print >> output, "# of paths accepted in "+rel0+":", sum_path_accpt0
print >> output, "# of paths accepted in "+rel1+":", sum_path_accpt1
print >> output, "# of paths that fired in "+rel0+" and didn't fire in "+rel1+":", sum_path_fired0_not1
print >> output, "# of paths that fired in "+rel1+" and didn't fire in "+rel0+":", sum_path_fired1_not0
print >> output, "# of events that changed from "+rel0+" to "+rel1+":", sum_changes
print >> output, "total # of events in "+rel0+":", entries0/(pidx0+1)
print >> output, "total # of events in "+rel1+":", entries1/(pidx1+1)
print >> output, " "
print >> output, "Fractional change in events from "+rel0+" to "+rel1+":", 100.00* sum_changes*(pidx0+1)/entries0,"%"
print >> output, " "

print >> output, "Paths that have at least 10 hits in "+rel0+" and no hits in "+rel1+":"
for keyA in path_total_hits0.iterkeys():
   if path_total_hits0[keyA] >= 5 and not path_total_hits1.has_key(keyA):
      print >> output, "PATH: ",keyA," COUNTS: ", path_total_hits0[keyA]
print >> output, " "
print >> output, "Paths that have at least 10 hits in "+rel1+" and no hits in "+rel0+":"
for keyB in path_total_hits1.iterkeys():
   if path_total_hits1[keyB] >= 5 and not path_total_hits0.has_key(keyB):
      print >> output, "PATH: ",keyB," COUNTS: ", path_total_hits1[keyB]

print >> output, " "
#print >> output, "Path | #counts in "+rel0+" |  counts in "+rel1+" | dataset"
#for keyC in path_total_hits0.iterkeys():
#   if dataset_v_path.has_key(keyC):
#      print keyC, path_total_hits0[keyC], path_total_hits1[keyC], dataset_v_path[keyC]
#   else:
#      print keyC, path_total_hits0[keyC], path_total_hits1[keyC],"????"


#for keyC in dataset_v_path.iterkeys():
#   print >> output, keyC, dataset_v_path[keyC]
