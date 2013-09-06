{
  
  gStyle->SetTitleW(0.8);

  //openFile("hist_file_pre1_pre2_jason_again.root");
  TFile *f1 = new TFile("scratch.root");
  f1.cd();


  TCanvas *can7 = new TCanvas("can7","canvas 7", 150,10,990,660);
  can7->SetGridx();
  can7->SetGridy();
  can7->SetBottomMargin(0.3);
  fired_rel0_didntfire_rel1_decay_type_vs_datasets->Add(fired_rel1_didntfire_rel0_decay_type_vs_datasets,1);
  fired_rel0_didntfire_rel1_decay_type_vs_datasets->SetTitle("Decay Type vs. Dataset Pre7 Pre8 Total Diffs");
  fired_rel0_didntfire_rel1_decay_type_vs_datasets->Draw("COLZ");
  can7->SaveAs("total_diffs.png");



  TCanvas *can1 = new TCanvas("can1","canvas1",150,10,990,660);
  can1->SetBottomMargin(0.3);
  can1->SetGridx();
  can1->SetGridy();
  fired_rel0_didntfire_rel1_decay_type_vs_datasets->Draw("COLZ");
  can1->SaveAs("fired_rel0_didntfire_rel1_decay_type_vs_datasets.png");
  
  TCanvas *can2 = new TCanvas("can2","canvas2",150,10,990,660);
  can2->SetBottomMargin(0.3);
  can2->SetGridx();
  can2->SetGridy();
  fired_rel1_didntfire_rel0_decay_type_vs_datasets->Draw("COLZ");
  can2->SaveAs("fired_rel1_didntfire_rel0_decay_type_vs_datasets.png");
  
  //TCanvas *can3 = new TCanvas("can3","canvas3",150,10,990,660);
  //can3->SetBottomMargin(0.3);
  //can2->SetGridx();
  //can2->SetGridy();
  //path_accept0_datasets->SetStats(0);
  //path_accept0->Draw();
  //can3->SaveAs("path_accept0.png");
  
  //TCanvas *can4 = new TCanvas("can4","canvas4",150,10,990,660);
  //can4->SetBottomMargin(0.3);
  //can4->SetGridx();
  //can4->SetGridy();
  //path_accept0_decay_type_vs_datasets->SetStats(0);
  //path_accept0_decay_type_vs_datasets->Draw("COLZ");
  //can4->SaveAs("path_accept0_decay_type_vs_datasets.png");

  //TCanvas *can6 = new TCanvas("can6","canvas6",150,10,990,660);
  //last_mod_type_fired_rel0_didntfire_rel1->Draw();
  //can6->SaveAs("last_mod_type_fired_rel0_didntfire_rel1.png");
  

  /*
    
  TCanvas *can5 = new TCanvas("can5","canvas5",150,10,990,660);
  can5->SetBottomMargin(0.3);
  can5->SetGridx();
  can5->SetGridy();
  
  TH2F *newthing = (TH2F*)fired_rel0_didntfire_rel1_decay_type_vs_datasets->Clone("newthing");
  newthing->SetBinContent(20,7,0.0);
  newthing->Divide(path_accept0_decay_type_vs_datasets);
  newthing->SetTitle("fraction of counts lost going from 620_pre1 to 620_pre2, ttbar decay vs. dataset");
  newthing->SetStats(0);
  newthing->Draw("COLZ");
  */
  
  
}
