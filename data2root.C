#include <vector>
#include <utility> // std::pair
#include <pair>

bool sortbysecond(const pair<int,double> &a, const pair<int,double> &b){
  return (a.second < b.second);
}

void data2root(){
  using namespace std;
  gSystem->Load("liblegolas");
  FILE *runlist = fopen("runlist.txt","r");
  vector<int> run_num;
  int num;
  while(feof(runlist)!=1){
    fscanf(runlist,"%d\n",&num);
    run_num.push_back(num);
  }
  fclose(runlist);

  int nrun = run_num.size();
  TString evtfile_name;
  int ie =0 ; // event number
  int evt_type;
  TH1D *h_wfm = new TH1D("h_wfw","waveform",27,-0.5,26.5);;


  TTree *tree = new TTree("T","T");
  const int ngem = 4;
  const int napv = 4;
  const int nch = 128;
  double integral[ngem][napv][nch];
  
  TString branch_txt = Form("integral[%d][%d][%d]/D",ngem,napv,nch);
  TBranch *branch_integral = tree->Branch("integral",integral,branch_txt);
  TBranch *branch_ev = tree->Branch("ev",&ie,"ev/I");

  
  for(int irun=0;irun<nrun;irun++){
    evtfile_name = Form("/home/yetao/workarea/GEM_test/data/scint_000000%d-0000.evt",
		    run_num[irun]);
    poncsopen(evtfile_name);
    evt_type = 0;
    while(evt_type!=12){
      prun(1);
      evt_type = eoe->GetBinContent(1);
      ie++;
      for(int igem=0; igem<ngem;igem++){
	for(int iapv=0;iapv<napv;iapv++){
	  for(int ich=0;ich<nch;ich++){
	    h_wfm = (TH1D*)gDirectory->FindObject(Form("h_%d_%d_%d",igem,iapv,ich));
	    integral[igem][iapv][ich] = h_wfm->Integral(1,25);
	  }
	}
      }
      tree->Fill();
    }
    pclose();
  }
  
  double pedestal[ngem][napv][nch];
  TH1D *h_ped = new TH1D("h_ped","pedestal",1e5,-0.5,1e5-0.5);
  for(int igem=0;igem<ngem;igem++){
    for(int iapv=0;iapv<napv;iapv++){
      for(int ich=0;ich<nch;ich++){
	tree->Draw(Form("integral[%d][%d][%d]>>h_ped",igem,iapv,ich),"","goff");
	pedestal[igem][iapv][ich] = getPedestal(h_ped);
      }
    }
  }

  double common_mode[ngem][napv];
  branch_txt = Form("cmn[%d][%d]/D",ngem,napv);
  TBranch *branch_cmn =  tree->Branch("cmn",common_mode,branch_txt);
  TH1D *h_cmn = new TH1D("h_cmn","h_cmn",1e4,-2e4,2e4);
  
  double charge[ngem][napv][nch];
  branch_txt = Form("charge[%d][%d][%d]/D",ngem,napv,nch);
  TBranch *branch_charge = tree->Branch("charge",charge,branch_txt);

  int nevt = tree->GetEntries();
  tree->SetBranchAddress("integral",integral);

  for(int ievt =0;ievt<nevt;ievt++){
    tree->GetEntry(ievt+1);

    for(int igem=0;igem<ngem;igem++){
      for(int iapv=0;iapv<napv;iapv++){
	//collect raw integral from each channel ,then find most probable value
	for(int ich=0; ich<nch;ich++){
	  h_cmn->Fill(pedestal[igem][iapv][ich]-integral[igem][iapv][ich]);
	}
	int max_bin = h_cmn->GetMaximumBin();
	common_mode[igem][iapv] = h_cmn->GetBinCenter(max_bin);
	h_cmn->Reset();
	for(int ich=0;ich<nch;ich++){
	  charge[igem][iapv][ich] = pedestal[igem][iapv][ich]-integral[igem][iapv][ich]-common_mode[igem][iapv];
	}
      }
    }
    branch_cmn->Fill();
    branch_charge->Fill();
  }

  TFile *rootfile = TFile::Open("test.root","RECREATE");
  tree->Write();
  rootfile->Close();
  
}


double getPedestal(TH1D *h){

  int bin = h->GetMaximumBin();
  double pedestal = h->GetBinCenter(bin);
  return pedestal;

}
