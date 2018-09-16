typedef struct{
  int nhits;
  double hit_x;
  double hit_y;
  double charge_x;
  double charge_y;
  double charge_sum;
}GEM_HIT;
  

void Analyze(){

  TFile *raw_rf = TFile::Open("test.root");
  TTree *raw_tree = raw_rf->Get("T");

  TFile *analyzed_rf = TFile::Open("reconstructed.root","RECREATE");
  TTree *rcn_tree = new TTree("R","R");
  int nhits;
  double hit_x,hit_y;
  double charge_x, charge_y,charge_sum;

  GEM_HIT gem1_hits,gem2_hits,gem3_hits;
  
  TBranch *branch_gem1 = rcn_tree->Branch("gem1",&gem1_hits,
					  "nhits/I:hit_x/D:hit_y/D:charge_x/D:charge_y/D:charge_sum/D");
  TBranch *branch_gem1 = rcn_tree->Branch("gem2",&gem2_hits,
					  "nhits/I:hit_x/D:hit_y/D:charge_x/D:charge_y/D:charge_sum/D");
  TBranch *branch_gem1 = rcn_tree->Branch("gem3",&gem3_hits,
					  "nhits/I:hit_x/D:hit_y/D:charge_x/D:charge_y/D:charge_sum/D");
  
  int nentry = raw_tree->GetEntries();

  // TH1D *h_stripx = new TH1D("h_stripx","Charge vs strip X",256,-0.5,255.5);
  // TH1D *h_stripx_long = new TH1D("h_stripx_long","Charge vs strip X",512,-0.5,511.5);
  // TH1D *h_stripy = new TH1D("h_stripy","Charge vs strip Y",256,-0.5,255.5);
  int a; // dummy variable

  const int ngem = 4;
  const int napv = 4;
  const int nch = 128;
  
  // APV Layout Convention :See Elog
  int apv_slot[4][4]={{1, 2, 3, 4}, // Hemmick's GEM Quadrant
		      {2, 1, 1, 2},
		      {4, 3, 2, 1},
		      {1, 2, 0, 0}};  // 0: No APV plugged in
  // apv readout order respect to Axis direction
  // -1 : reverse order
  int apv_order[4][4]={{ 1, 1, 1, 1},
		       {-1,-1, 1, 1},  
		       { 1, 1, 1, 1},
		       { 1, 1, 0, 0}};
    
  TString apv_axis[4][4]={{"Q","Q","Q","Q"},
			  {"X","X","Y","Y"},
			  {"X","X","X","X"},
			  {"Y","Y","0","0"}};
  
  TString readout_type[4]={"H","S","L","S"};
  // H : Hemmick's 800 um pitch XY readout
  // L: 20 cm side of CERN 10x20 readout 
  // S: 10 cm side of CERN 10x10 GEM (or short side of 10x20 GEM)
  
  double charge[ngem][napv][nch];
  raw_tree->SetBranchAddress("charge",charge);
  TCanvas *c1 = new TCanvas("c1","c1",1200,600);
  c1->Divide(1,2);
  int max_x;
  int max_y;
  int igem =1;
  TH1D *h_stripy;
  TH1D *h_stripx;
  
  for(int ie=0;ie<nentry;ie++){
    raw_tree->GetEntry(ie+1);
    for(int igem=0;igem<ngem;igem++){
      // .....
    }

    rcn_tree->Fill();
    branch_charge_x->Fill();
    branch_charge_y->Fill();
    branch_charge_sum->Fill();
  }

  raw_rf->Close();
  rcn_tree->Write();
  analyzed_rf->Close();
  // gSystem->Exec("pdfunite qvsstrip*.pdf unite.pdf");
  // gSystem->Exec("rm qvsstrip*.pdf");
}


TH1D GetStripHistogram(double *charge, int *apv_slot, int *apv_order, TString *apv_axis, TString axis_type){
  TH1D h_strip;
  int ncount =0;
  if(apv_axis[0] == "Q"){
    
    TH1D *hbuff =new TH1D("hbuff","hbuff",120,-0.5,119.5);

    for(int iapv=0;iapv<4;iapv++){
	int istart = 0;
	if(axis_type="X")
	  istart= 9;
	if(axis_type="Y")
	  istart= 8;
	for(int ich =istart;ich<128;ich+=2){ // note: it must be 127 and ich+=2
	  hbuff->Fill(ich+128*(myslot%2-1),charge[iapv][ich]);
	}

    } // apv loop
  }
  else{
    for(int iapv=0;iapv<4;iapv++){
      if(apv_axis[iapv] == axis_type){
	ncount ++;
      }
    }
    
    TH1D *hbuff =new TH1D("hbuff","hbuff",ncount*128,-0.5,ncount*128-0.5);
    for(int iapv=0;iapv<4;iapv++){
      if(apv_axis[iapv] == axis_type){
	int my_slot = apv_slot[iapv];
	int my_order= apv_order[iapv];

	if(my_order== 1){
	  for(int ich=0;ich<128;ich++){
	    hbuff->Fill((my_slot-1)*128+ich,
			charge[iapv][ich]);
	  } // channel loop
	}
	if(my_order==-1){
	  for(int ich=0;ich<128;ich++){
	    hbuff->Fill(my_slot*128-ich,
			charge[iapv][ich]);
	  } // channel loop
	}
      }// axis condition
    }// apv loop
  }

  h_strips = *hbuff;
  return h_strips;
}


int GetHits(TH1D *h_strip, double &hit_pos){


  
}

double GetCharge(TH1D *h_strip, double hit_pos){
  double charge;
  charge = h_strip->Integral(hit_pos-5,hit_pos+5);

  return charge;
}
