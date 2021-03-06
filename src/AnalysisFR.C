#define AnalysisFR_cxx
#include "../include/AnalysisFR.h"

float _event_weight_loose; 
float _event_weight_tight;
float _event_weight_tight_2l;
float inputJetEt;

int   _Zlepton1type;
int   _Zlepton2type;
int   _Zdecayflavour;

const float _luminosity = 2.263;  // fb

const int pTbin = 8;
const Double_t pTbins[pTbin+1] = { 10., 15., 20., 25., 30., 35.,40., 45., 50.};

const int etabin = 5;
const Double_t etabins[etabin+1] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5};

//------------------------------------------------------------------------------
// AnalysisFR
//------------------------------------------------------------------------------
AnalysisFR::AnalysisFR(TTree* tree, TString systematic) : AnalysisCMS(tree, systematic)
{
  SetSaveMinitree(false);
}


//------------------------------------------------------------------------------
// Loop
//------------------------------------------------------------------------------
void AnalysisFR::Loop(TString analysis, TString filename, float luminosity)
{

  if (fChain == 0) return;

  Setup(analysis, filename, luminosity);

  // Define histograms
  //----------------------------------------------------------------------------

  for (int j = 0; j < ncut; j++) {

    TString directory = scut[j];

    root_output->cd();
    gDirectory->mkdir(directory);
    root_output->cd(directory);

    h_Muon_loose_pt_eta_bin[j] = new TH2D("h_Muon_loose_pt_eta_bin", "h_Muon_loose_pt_eta_bin", pTbin, pTbins, etabin, etabins);
    h_Muon_tight_pt_eta_bin[j] = new TH2D("h_Muon_tight_pt_eta_bin", "h_Muon_tight_pt_eta_bin", pTbin, pTbins, etabin, etabins);
  
    h_Muon_loose_pt_bin[j] = new TH1D("h_Muon_loose_pt_bin", "h_Muon_loose_pt_bin", pTbin, pTbins);
    h_Muon_tight_pt_bin[j] = new TH1D("h_Muon_tight_pt_bin", "h_Muon_tight_pt_bin", pTbin, pTbins);
    
    h_Muon_loose_eta_bin[j] = new TH1D("h_Muon_loose_eta_bin", "h_Muon_loose_eta_bin", etabin, etabins);
    h_Muon_tight_eta_bin[j] = new TH1D("h_Muon_tight_eta_bin", "h_Muon_tight_eta_bin", etabin, etabins);

    h_Ele_loose_pt_eta_bin[j] = new TH2D("h_Ele_loose_pt_eta_bin", "h_Ele_loose_pt_eta_bin", pTbin, pTbins, etabin, etabins);
    h_Ele_tight_pt_eta_bin[j] = new TH2D("h_Ele_tight_pt_eta_bin", "h_Ele_tight_pt_eta_bin", pTbin, pTbins, etabin, etabins);

    h_Ele_loose_pt_bin[j] = new TH1D("h_Ele_loose_pt_bin", "h_Ele_loose_pt_bin", pTbin, pTbins);
    h_Ele_tight_pt_bin[j] = new TH1D("h_Ele_tight_pt_bin", "h_Ele_tight_pt_bin", pTbin, pTbins);

    h_Ele_loose_eta_bin[j] = new TH1D("h_Ele_loose_eta_bin", "h_Ele_loose_eta_bin", etabin, etabins);
    h_Ele_tight_eta_bin[j] = new TH1D("h_Ele_tight_eta_bin", "h_Ele_tight_eta_bin", etabin, etabins);

    h_Muon_loose_pt[j] = new TH1D("h_Muon_loose_pt","h_Muon_loose_pt", 1000, 0, 200);
    h_Muon_tight_pt[j] = new TH1D("h_Muon_tight_pt","h_Muon_tight_pt", 1000, 0, 200);
    h_Ele_loose_pt[j] = new TH1D("h_Ele_loose_pt","h_Ele_loose_pt", 1000, 0, 200);
    h_Ele_tight_pt[j] = new TH1D("h_Ele_tight_pt","h_Ele_tight_pt", 1000, 0, 200);

    h_Muon_loose_mtw[j] = new TH1D("h_Muon_loose_mtw","h_Muon_loose_mtw", 1000, 0, 200);
    h_Muon_tight_mtw[j] = new TH1D("h_Muon_tight_mtw","h_Muon_tight_mtw", 1000, 0, 200);
    h_Ele_loose_mtw[j] = new TH1D("h_Ele_loose_mtw","h_Ele_loose_mtw", 1000, 0, 200);
    h_Ele_tight_mtw[j] = new TH1D("h_Ele_tight_mtw","h_Ele_tight_mtw", 1000, 0, 200);

    h_Muon_loose_m2l[j] = new TH1D("h_Muon_loose_m2l","h_Muon_loose_m2l", 1000, 0, 200);
    h_Muon_tight_m2l[j] = new TH1D("h_Muon_tight_m2l","h_Muon_tight_m2l", 1000, 0, 200);
    h_Ele_loose_m2l[j] = new TH1D("h_Ele_loose_m2l","h_Ele_loose_m2l", 1000, 0, 200);
    h_Ele_tight_m2l[j] = new TH1D("h_Ele_tight_m2l","h_Ele_tight_m2l", 1000, 0, 200);

  }

  root_output -> cd();

  // Loop over events
  //--------------------------------------------------------------------------

  for (Long64_t jentry=0; jentry<_nentries;jentry++) {

    Long64_t ientry = LoadTree(jentry);

    if (ientry < 0) break;

    fChain->GetEntry(jentry);

    PrintProgress(jentry, _nentries);

    EventSetup();

    _m2l = -999;
    _Zlepton1type = Loose;
    _Zlepton2type = Loose;
    _Zdecayflavour = 0;

    _channel = (abs(Lepton1.flavour) == ELECTRON_FLAVOUR) ? e : m;

    if (_channel == e && Lepton1.v.Pt() <= 13) continue;
    if (_channel == m && Lepton1.v.Pt() <= 10) continue;
    if (_channel == e && Lepton1.v.Eta() >= 2.5 ) continue;
    if (_channel == m && Lepton1.v.Eta() >= 2.4 ) continue;

    if (AnalysisLeptons.size() >= 2) { 

      for (int iLep1 = 0; iLep1 < AnalysisLeptons.size(); iLep1++) {
	
	if (AnalysisLeptons[iLep1].v.Pt() < 10.) continue;

	for (int iLep2 = iLep1+1; iLep2 < AnalysisLeptons.size(); iLep2++) {
	  
	  if (AnalysisLeptons[iLep2].v.Pt() < 10.) continue;
	  if ((AnalysisLeptons[iLep1].flavour + AnalysisLeptons[iLep2].flavour) != 0.) continue;
	  
	  _m2l = (AnalysisLeptons[iLep1].v + AnalysisLeptons[iLep2].v).M();

	  _Zlepton1type = AnalysisLeptons[iLep1].type;  
	  _Zlepton2type = AnalysisLeptons[iLep2].type;

	  _Zdecayflavour = AnalysisLeptons[iLep1].flavour; 
	  
	}
      }
    }

    bool passTrigger = true;

    if (_ismc) {

      Double_t genWeight = GEN_weight_SM/abs(GEN_weight_SM);

      _event_weight_loose = puW * baseW * genWeight;
      _event_weight_tight = puW * baseW * genWeight;
      _event_weight_tight_2l = puW * baseW * genWeight;

      /*
      if (filename.Contains("WJetsToLNu")) {

	if (_channel == m) {

	  _event_weight_loose *= 0.081;

	  if (Lepton1.type == Tight) {

	    _event_weight_tight *= 0.075;
	    	  
	  }

	  if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	    _event_weight_tight_2l *= 0.075;
	    
	  }

	} else if (_channel == e) {

	  _event_weight_loose *= 0.0057;
	  
	  if (Lepton1.type == Tight) {

	    _event_weight_tight *= 0.0018;
	    
	  }

	  if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	    _event_weight_tight_2l *= 0.0018;
	    
	  }

	}
	
      } else if (filename.Contains("DYJetsToLL")) {

	if (_channel == m) {
	    
	  _event_weight_loose *= 0.085;
	  
	  if (Lepton1.type == Tight) {

	    _event_weight_tight *= 0.084;
	    
	  }

	  if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	    _event_weight_tight_2l *= 0.084;

	  }

	} else if (_channel == e) {

	  _event_weight_loose *= 0.0015;
	  
	  if (Lepton1.type == Tight) {

	    _event_weight_tight *= 0.0014;
	    
	  }

	  if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	    _event_weight_tight_2l *= 0.0014;
	    
	  }

	}
      }
      */
    } else  {

      _event_weight_loose = 1.0;
      _event_weight_tight = 1.0;
      _event_weight_tight_2l = 1.0;

      passTrigger = false;

      if (_channel == m) {

	if (Lepton1.v.Pt() <= 20. && std_vector_trigger->at(22)) { //Lumi HLT_Mu8_TrkIsoVVL_v*: 1.386 pb

	  passTrigger= false; // WATCH!

	}
	else if (Lepton1.v.Pt() > 20. && std_vector_trigger->at(23)) { //Lumi Lumi HLT_Mu17_TrkIsoVVL_v*: 201.951 pb

	  passTrigger = true;

	}    
      }

      if (_channel == e) {
	
	if (Lepton1.v.Pt() <= 25. && std_vector_trigger->at(31)) { //Lumi HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v*: 11.204 pb;

	  passTrigger = false; // WATCH!

	} else if (Lepton1.v.Pt() > 25. && std_vector_trigger->at(33)) { //Lumi HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v*: 3.201

	  passTrigger = true;
	  
	}
      }
     
    }

    if (!passTrigger) continue;    
    if (!PassJetSelection()) continue;
    if (AnalysisJets.size() < 1 ) continue;

    if (_channel == e) {
      inputJetEt = 35.;
    } else if (_channel == m) {
      inputJetEt = 20.;
    }

    if (AnalysisJets[0].v.Pt() < inputJetEt) continue; 

    // Selection
    //--------------------------------------------------------------------------
    
    GetMt(Lepton1, _mtw);
     
    bool pass = true;
  
    pass &= (MET.Et() < 20.);
    pass &= (_mtw < 20.);
    pass &= (_nlepton == 1);

    FillLevelHistograms(FR_00_QCD, pass);

    pass = (MET.Et() > 20.);
    pass &= (_mtw > 20.);
    pass &= (_nlepton == 1);
    pass &= (Lepton1.v.Pt() > 35.);

    FillLevelHistograms(FR_01_WRegion, pass);

    pass = (MET.Et() < 20.);
    pass &= (_mtw > 20.);
    pass &= (_nlepton == 1);
    pass &= (Lepton1.v.Pt() > 35.);

    FillLevelHistograms(FR_02_WRegionQCD, pass);

    pass = (MET.Et() > 20.);
    pass &= (_mtw > 20.);
    pass &= (_nlepton >= 2);
    //    pass &= (Lepton1.v.Pt() > 35.);
    pass &= _m2l > 0;

    if (pass && fabs(_Zdecayflavour) == ELECTRON_FLAVOUR) {
 
      h_Ele_loose_m2l[FR_03_ZRegion] -> Fill(_m2l, _event_weight_loose);     
     
      if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	h_Ele_tight_m2l[FR_03_ZRegion] -> Fill(_m2l, _event_weight_tight_2l);
	
      }

    } else if (pass && fabs(_Zdecayflavour) == MUON_FLAVOUR) {

      h_Muon_loose_m2l[FR_03_ZRegion] -> Fill(_m2l, _event_weight_loose);

      if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	h_Muon_tight_m2l[FR_03_ZRegion] -> Fill(_m2l, _event_weight_tight_2l);

      } 

    }

    pass = (MET.Et() < 20.);
    pass &= (_mtw < 20.);
    pass &= (_nlepton >= 2);
    pass &= (Lepton1.v.Pt() > 35.);
    pass &= _m2l > 0;

    if (pass && fabs(_Zdecayflavour) == ELECTRON_FLAVOUR) {
 
      h_Ele_loose_m2l[FR_04_ZRegionQCD] -> Fill(_m2l, _event_weight_loose);     
     
      if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	h_Ele_tight_m2l[FR_04_ZRegionQCD] -> Fill(_m2l, _event_weight_tight_2l);
	
      }

    } else if (pass && fabs(_Zdecayflavour) == MUON_FLAVOUR) {

      h_Muon_loose_m2l[FR_04_ZRegionQCD] -> Fill(_m2l, _event_weight_loose);

      if (_Zlepton1type == Tight && _Zlepton2type == Tight) {

	h_Muon_tight_m2l[FR_04_ZRegionQCD] -> Fill(_m2l, _event_weight_tight_2l);

      } 

    }}


  EndJob();

}

//------------------------------------------------------------------------------                                                                           
// FillLevelHistograms
//------------------------------------------------------------------------------
                    
void AnalysisFR::FillLevelHistograms(int icut, bool pass) {

  if (!pass) return;

  FillAnalysisHistograms(icut);
 
}

//------------------------------------------------------------------------------   
// FillanalysisHistograms
//------------------------------------------------------------------------------                                                                        

void AnalysisFR::FillAnalysisHistograms(int icut) {

  if (Lepton1.type == Loose || Lepton1.type == Tight) {

    if (_channel == m) {

      h_Muon_loose_pt_eta_bin[icut] -> Fill(Lepton1.v.Pt(), Lepton1.v.Eta(), _event_weight_loose);
      h_Muon_loose_pt_bin[icut] -> Fill(Lepton1.v.Pt(), _event_weight_loose);
      h_Muon_loose_eta_bin[icut] -> Fill(Lepton1.v.Eta(), _event_weight_loose);
     
      h_Muon_loose_pt[icut]  -> Fill(Lepton1.v.Pt(), _event_weight_loose);
      h_Muon_loose_mtw[icut] -> Fill(_mtw,           _event_weight_loose);

    } else if (_channel == e) {

      h_Ele_loose_pt_eta_bin[icut] -> Fill(Lepton1.v.Pt(), Lepton1.v.Eta(), _event_weight_loose);
      h_Ele_loose_pt_bin[icut] -> Fill(Lepton1.v.Pt(), _event_weight_loose);
      h_Ele_loose_eta_bin[icut] -> Fill(Lepton1.v.Eta(), _event_weight_loose);
      
      h_Ele_loose_pt[icut]  -> Fill(Lepton1.v.Pt(), _event_weight_loose);
      h_Ele_loose_mtw[icut] -> Fill(_mtw,           _event_weight_loose);

    }
 
  }
    
  if (Lepton1.type == Tight) {
   
    if (_channel == m) {

      h_Muon_tight_pt_eta_bin[icut] -> Fill(Lepton1.v.Pt(), Lepton1.v.Eta(), _event_weight_tight);
      h_Muon_tight_pt_bin[icut] -> Fill(Lepton1.v.Pt(), _event_weight_tight);
      h_Muon_tight_eta_bin[icut] -> Fill(Lepton1.v.Eta(), _event_weight_tight);
      
      h_Muon_tight_pt[icut]  -> Fill(Lepton1.v.Pt(), _event_weight_tight);
      h_Muon_tight_mtw[icut] -> Fill(_mtw,           _event_weight_tight);	

    } else if (_channel == e) {

      h_Ele_tight_pt_eta_bin[icut] -> Fill(Lepton1.v.Pt(), Lepton1.v.Eta(), _event_weight_tight);
      h_Ele_tight_pt_bin[icut] -> Fill(Lepton1.v.Pt(), _event_weight_tight);
      h_Ele_tight_eta_bin[icut] -> Fill(Lepton1.v.Eta(), _event_weight_tight);
	
      h_Ele_tight_pt[icut]  -> Fill(Lepton1.v.Pt(), _event_weight_tight);
      h_Ele_tight_mtw[icut] -> Fill(_mtw,           _event_weight_tight);

    }
  }
  
}

//------------------------------------------------------------------------------                                            
// PassJetSelection
//------------------------------------------------------------------------------  

bool AnalysisFR::PassJetSelection()
{
  bool pass = false;

  AnalysisJets.clear();
   
  int jetsize = std_vector_jet_pt->size();
        
  for ( int j=0; j<jetsize; j++) {
      
    Jet jet_; 
      
    jet_.index = j; 
      
    float pt = std_vector_jet_pt->at(j);
    float eta = std_vector_jet_eta->at(j);
    float phi = std_vector_jet_phi->at(j);
    float btag = std_vector_jet_csvv2ivf->at(j);     

    if ( pt <= 10.0) continue;

    TLorentzVector jetvt;
    jetvt.SetPtEtaPhiM(pt, eta, phi, 0);
      
    jet_.v = jetvt;

    float dR =  jetvt.DeltaR(AnalysisLeptons[0].v);

    if ( dR > 1 )   {       
      AnalysisJets.push_back(jet_);
    }

  }

  pass = true;

  return pass;
}

/*

We have checked with brillcalc that the luminosity for the triggers used are correct on the 12th of May 2016.
This test was performed with brillcalc version 10.7 (running the command export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.0.3/bin:$PATH  brilcalc --version).

Then, for each trigger, the following line was executed : 
"brilcalc lumi --hltpath HLT_Mu8_TrkIsoVVL_v* -u /pb -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Reprocessing/Cert_13TeV_16Dec2015ReReco_Collisions15_25ns_JSON_v2.txt "

The triggers names for the electrons have also been updated.

*/
