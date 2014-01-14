#!/usr/bin/python
from ROOT import TFile,TTree,TH1D, TLegend, TCanvas

def fillDict(tree,key,fillMany):
   if fillMany and tree.has_key(key):
      tree[key] += 1
   else:
      tree[key] = 1
   return tree

def main():
   rel0 = '700pre9'#620_pre1
   rel1 = '700pre10'
   output = open(rel0+'_'+rel1+'TEST_comparison.log',"w")
   outputA = open(rel0+'_'+rel1+'TEST_pathCounts.csv',"w")
   outputB = open(rel0+'_'+rel1+'objFilter_pathCounts.csv',"w")
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
   get_gen_entries0_from_event = {0:0} #event number : entry number                                     
   get_gen_entries1_from_event = {0:0}
   looked_at_this_event_b4 = {'event':0}
   path_per_event = {'path':'event'}
   #make histos
   single_mu_rel0 = TH1D("IsoMu24","rel0 Single Muon Pt",20,10,200)
   single_mu_rel1 = TH1D("IsoMu24","rel1 Single Muon Pt",20,10,200)
   single_ele_rel0 = TH1D("Ele36_WP80","rel0 Single Ele Pt",20,30,200)
   single_ele_rel1 = TH1D("Ele36_WP80","rel1 Single Ele Pt",20,30,200)
   single_jet_rel0 = TH1D("PFJet200","rel0 Single Jet Pt",25,180,500)
   single_jet_rel1 = TH1D("PFJet200","rel1 Single Jet Pt",25,180,500)
   
#apply gen matching (make sure events you're comparing are the same @ generator level
   for genentry in xrange(gen_entries0):
      gen_tree0.GetEntry(genentry)
      get_gen_entries0_from_event[gen_tree0.event] = genentry



   outputB.write("event,path,last module w saved tags,obj ID,pre7 pt,pre8 pt,pre7 phi,pre8 phi,pre7 eta,pre8 eta\n")
   #for ientry in xrange(1000000):
   for ientry in xrange(entries0):
      
      hlt_tree0.GetEntry(ientry-diff0)
      hlt_tree1.GetEntry(ientry-diff1)
      name = hlt_tree0.path_name[:hlt_tree0.path_name.find('\x00')]
      if name != "HLT_PFJet200_v10" and name != "HLT_Ele36_WP80_v1" and name != "HLT_IsoMu24_v18":
         continue
      
      #if ientry>1000000:
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
         idx1 = 0#reset counters
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
         event = hlt_tree0.event
         gen_tree0.GetEntry(get_gen_entries0_from_event[event])
         gen_tree1.GetEntry(get_gen_entries0_from_event[event])

       
###################################### testing area ############################################################################# 
         if (hlt_tree0.path_accept and hlt_tree1.path_accept):
            if hlt_tree0.last_module_with_saved_tags_index==hlt_tree1.last_module_with_saved_tags_index:
               #print name
               if name=="HLT_PFJet200_v10":
                  for obj in xrange(100):
                     if hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] == hlt_tree1.last_module_with_saved_tags_object_obj_ID[obj] and hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] != -99:
                        #single_jet_rel0.Fill(hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj])
                        #single_jet_rel1.Fill(hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj])
                        #print hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        #print gen_tree0.gen_ID[obj]
                        #print gen_tree1.gen_ID[obj]
                        label=hlt_tree0.last_module_with_saved_tags_label[:hlt_tree0.last_module_with_saved_tags_label.find('\x00')]
                        objID=hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        objpt0=hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj]
                        objpt1=hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj]
                        objeta0=hlt_tree0.last_module_with_saved_tags_object_obj_eta[obj]
                        objeta1=hlt_tree1.last_module_with_saved_tags_object_obj_eta[obj]
                        objphi0=hlt_tree0.last_module_with_saved_tags_object_obj_phi[obj]
                        objphi1=hlt_tree1.last_module_with_saved_tags_object_obj_phi[obj]
                        outputB.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s, %(6)s, %(7)s, %(8)s, %(9)s, %(10)s\n"%{"1":event, "2":name,"3":label,"4":objID,"5":objpt0,"6":objpt1,"7":objphi0,"8":objphi1,"9":objeta0,"10":objeta1})


               if name=="HLT_Ele36_WP80_v1":
                  for obj in xrange(100):
                     if hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] == hlt_tree1.last_module_with_saved_tags_object_obj_ID[obj] and hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] != -99:
                        #single_ele_rel0.Fill(hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj])
                        #single_ele_rel1.Fill(hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj])
                        #print hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        #print gen_tree0.gen_ID[obj]
                        #print gen_tree1.gen_ID[obj]
                        label=hlt_tree0.last_module_with_saved_tags_label[:hlt_tree0.last_module_with_saved_tags_label.find('\x00')]
                        objID=hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        objpt0=hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj]
                        objpt1=hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj]
                        objeta0=hlt_tree0.last_module_with_saved_tags_object_obj_eta[obj]
                        objeta1=hlt_tree1.last_module_with_saved_tags_object_obj_eta[obj]
                        objphi0=hlt_tree0.last_module_with_saved_tags_object_obj_phi[obj]
                        objphi1=hlt_tree1.last_module_with_saved_tags_object_obj_phi[obj]
                        outputB.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s, %(6)s, %(7)s, %(8)s, %(9)s, %(10)s\n"%{"1":event, "2":name,"3":label,"4":objID,"5":objpt0,"6":objpt1,"7":objphi0,"8":objphi1,"9":objeta0,"10":objeta1})
                        
                        
               if name=="HLT_IsoMu24_v18":
                  for obj in xrange(100):
                     #if gen_tree0.gen_pt[obj] != -99.0:
                        #print "object ID===================",gen_tree0.gen_ID[obj]
                        #print "parent obj id===============", gen_tree0.gen_parent_ID[obj]
                        #print "grandparent obj id===============", gen_tree0.gen_grandparent_ID[obj]
                     if hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] == hlt_tree1.last_module_with_saved_tags_object_obj_ID[obj] and hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj] != -99:
                        #single_mu_rel0.Fill(hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj])
                        #single_mu_rel1.Fill(hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj])
                        #print hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        #print gen_tree0.gen_ID[obj]
                        #print gen_tree1.gen_ID[obj]
                        label=hlt_tree0.last_module_with_saved_tags_label[:hlt_tree0.last_module_with_saved_tags_label.find('\x00')]
                        objID=hlt_tree0.last_module_with_saved_tags_object_obj_ID[obj]
                        objpt0=hlt_tree0.last_module_with_saved_tags_object_obj_pt[obj]
                        objpt1=hlt_tree1.last_module_with_saved_tags_object_obj_pt[obj]
                        objeta0=hlt_tree0.last_module_with_saved_tags_object_obj_eta[obj]
                        objeta1=hlt_tree1.last_module_with_saved_tags_object_obj_eta[obj]
                        objphi0=hlt_tree0.last_module_with_saved_tags_object_obj_phi[obj]
                        objphi1=hlt_tree1.last_module_with_saved_tags_object_obj_phi[obj]
                        outputB.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s, %(6)s, %(7)s, %(8)s, %(9)s, %(10)s\n"%{"1":event, "2":name,"3":label,"4":objID,"5":objpt0,"6":objpt1,"7":objphi0,"8":objphi1,"9":objeta0,"10":objeta1})
                        
                        
                        
                        
##################################################################################################################################
         
         dsmap = file_rel0.Get("newHLTOffline/map_of_trig_to_trig_types")
         for ds in xrange(45):
            if dsmap.GetBinContent(hlt_tree0.path_index,ds):
               dataset_v_path[name] = dsmap.GetYaxis().GetBinLabel(ds)

         if hlt_tree0.path_accept:

            fillDict(path_total_hits0,name,1)
            fillDict(path_accept0,name,0)

            if not hlt_tree1.path_accept:
               fillDict(event_fired0_not1,event,1)
               fillDict(path_fired0_not1,name,0)               
               fillDict(events_that_changed,event,0)

         if hlt_tree1.path_accept:

            fillDict(path_total_hits1,name,1)
            fillDict(path_accept1,name,0)

            if not hlt_tree0.path_accept:
               fillDict(event_fired1_not0,event,1)
               fillDict(path_fired1_not0,name,0)
               fillDict(events_that_changed,event,0)



#make histos
#    single_mu_rel0.SetLineColor(1)
#    single_mu_rel1.SetLineColor(2)
#    title = single_mu_rel0.GetName()
#    can1 = TCanvas(title,title)
#    single_mu_rel0.Draw("hist")
#    single_mu_rel1.Draw("samehist")
#    legend = TLegend(0.83,0.66,0.99,0.77,"","brNDC")
#    legend.AddEntry(single_mu_rel0,rel0)
#    legend.AddEntry(single_mu_rel1,rel1)                          
#    legend.Draw("same")
#    title = title+".png"
#    can1.SaveAs(title)

#    single_ele_rel0.SetLineColor(1)
#    single_ele_rel1.SetLineColor(2)
#    title = single_ele_rel0.GetName()
#    can2 = TCanvas(title,title)
#    single_ele_rel0.Draw("hist")
#    single_ele_rel1.Draw("samehist")
#    legend2 = TLegend(0.83,0.66,0.99,0.77,"","brNDC")
#    legend2.AddEntry(single_ele_rel0,rel0)
#    legend2.AddEntry(single_ele_rel1,rel1)
#    legend2.Draw("same")
#    title = title+".png"
#    can2.SaveAs(title)
   
#    single_jet_rel0.SetLineColor(1)
#    single_jet_rel1.SetLineColor(2)
#    title = single_jet_rel0.GetName()
#    can3 = TCanvas(title,title)
#    single_jet_rel0.Draw("hist")
#    single_jet_rel1.Draw("samehist")
#    legend3 = TLegend(0.83,0.66,0.99,0.77,"","brNDC")
#    legend3.AddEntry(single_jet_rel0,rel0)
#    legend3.AddEntry(single_jet_rel1,rel1)
#    legend3.Draw("same")
#    title = title+".png"
#    can3.SaveAs(title)

   

   sum_path_fired0_not1 = sum(path_fired0_not1.values())
   sum_path_fired1_not0 = sum(path_fired1_not0.values())
   sum_event_fired0_not1 = len(event_fired0_not1)#sum(event_fired0_not1.values())
   sum_event_fired1_not0 = len(event_fired1_not0)#sum(event_fired1_not0.values())
   sum_path_accpt0 = sum(path_accept0.values())
   sum_path_accpt1 = sum(path_accept1.values())
   sum_changes = sum(events_that_changed.values())

#   print events_that_changed
   
   output.write("Events in rel0: %s\n"%evt0)
   output.write("Events in rel1: %s\n"%evt1)
   output.write("Paths in rel0: %s\n"%pidx0)
   output.write("Paths in rel1: %s\n"%pidx1)
   output.write(" \n event_fired0_not1\n")
   for (keyA, counts) in event_fired0_not1.items():
      output.write("event: %(1)s   | paths: %(2)s\n"%{"1":keyA, "2":counts})
   
   output.write(" \n event_fired1_not0\n")
   for (keyA, counts) in event_fired1_not0.items():
      output.write("event: %(1)s   | paths: %(2)s\n"%{"1":keyA, "2":counts})
      
   output.write("\n events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_event_fired0_not1}) 
   output.write("events with paths that fired in %(1)s and didnt fire in %(2)s : %(3)s\n"%{"1":rel1,"2":rel0,"3":sum_event_fired1_not0})
   output.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel0,"2":sum_path_accpt0})
   output.write("# of paths accepted in %(1)s: %(2)s\n"%{"1":rel1,"2":sum_path_accpt1})
   output.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3": sum_path_fired0_not1})
   output.write("# of paths that fired in %(1)s  and didn't fire in %(2)s: %(3)s\n"%{"1":rel1,"2":rel0,"3": sum_path_fired1_not0})
   output.write("# of events that changed from %(1)s to %(2)s: %(3)s\n"%{"1":rel0,"2":rel1,"3":sum_changes})
   output.write("total # of events in %(1)s: %(2)s\n"%{"1":rel0,"2":entries0/(pidx0+1)})
   output.write("total # of events in %(1)s: %(2)s\n"%{"1":rel1,"2":entries0/(pidx1+1)})
   output.write(" \n")
   output.write("Fractional change in events from %(1)s to %(2)s: %(3)s  percent\n"%{"1":rel0,"2":rel1,"3":100.00*sum_changes*(pidx0+1)/entries0})
   output.write(" \n")
   outputA.write("HLT PATH,dataset,counts in %(1)s,counts in %(2)s,difference in counts\n" %{"1":rel0,"2":rel1})

   for (keyA, counts) in path_total_hits0.items():
      if path_total_hits1.has_key(keyA):# and abs(counts-path_total_hits1[keyA]) >= 1: 
         if dataset_v_path.has_key(keyA):           
            #print keyA, path_total_hits1[keyA], dataset_v_path[keyA]
            outputA.write("%(1)s, %(2)s, %(3)s, %(4)s, %(5)s\n"%{"1":keyA, "2":dataset_v_path[keyA], "3":counts,"4":path_total_hits1[keyA],"5":counts-path_total_hits1[keyA]})
         else:
            outputA.write("%(1)s, none, %(2)s, %(3)s, %(4)s\n"%{"1":keyA, "2":counts,"3":path_total_hits1[keyA],"4":counts-path_total_hits1[keyA]})
   output.close()

            

if __name__=='__main__':
   main()
