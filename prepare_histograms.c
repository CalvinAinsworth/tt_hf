#include <TH2.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TPad.h>
#include <TMath.h>

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;



// ##################################
// ## Split string into components ##
// ##################################
vector<TString> split(TString split_string, char delimiter)
{
  stringstream ss;
  ss << split_string;
  string component;

  vector<TString> container;
  while(getline(ss, component, delimiter))
    {
      container.push_back(component);
    }

  return container;
}



// #################################################
// ## Make a list of files in the given directory ##
// #################################################
vector<TString> get_list_of_files(TString dirname, vector<TString> container = {})
{
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if (files)
    {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next()))
        {
          fname = file->GetName();
          if (fname != "." && fname != "..")
            {
              if (fname.EndsWith(".root")) { container.push_back(dirname + fname); }
              else { container.push_back(dirname + fname + "/"); }
            }
        }
    }
  return container;
}



// ###########################
// ## Draw a few histograms ##
// ###########################
int draw_n_histos(vector<TH1*> h_vec, vector<TString> h_title, TString x_axis_title, TString title, bool normalize=false)
{
  // Draws N histogram on one canvas (not stacked)
  TCanvas *c = new TCanvas(h_title[0], h_title[0], 1600, 900);
  gStyle->SetOptStat(0);
  gPad->SetGrid();
  if (normalize==false) gPad->SetLogy();
  double legend_height = 0.09*h_vec.size();
  double legend_y1 = 0.90 - legend_height;
  TLegend *legend = new TLegend(0.80, legend_y1, 0.90, 0.90);

  for (int i=0; i<h_vec.size(); i++) {
    double sf = 1/h_vec[i]->Integral(0, h_vec[i]->GetNbinsX());
    
    h_vec[i]->SetMarkerStyle(20);
    h_vec[i]->SetMarkerSize(2);
    h_vec[i]->SetMarkerColor(i+1);
    h_vec[i]->SetLineColor(i+1);
    h_vec[i]->SetLineWidth(2);
    if (normalize==true) h_vec[i]->Scale(sf);
    
    if (i==0) {
      h_vec[i]->Draw("C");
      h_vec[i]->SetTitle(title);
      
      if (normalize==false) {
	h_vec[i]->GetYaxis()->SetRangeUser(1, 10000);
	h_vec[i]->GetYaxis()->SetTitle("#bf{Events}"); }
      
      else {
	h_vec[i]->GetYaxis()->SetRangeUser(0, 1.1);
	h_vec[i]->GetYaxis()->SetTitle("#bf{Events norm to 1}"); }
      
      h_vec[i]->GetXaxis()->SetTitle(x_axis_title); }
    
    else { h_vec[i]->Draw("same C"); }
    legend->AddEntry(h_vec[i], h_title[i]);
  }
  legend->Draw("same");

  c->Print("Plots/" + title + ".png");

  return 0;
}



// ##############
// ##   MAIN   ##
// ##############
void prepare_histograms()
{
  // Create a list of directories with ntuples
  TString path_to_ntuples = "/eos/user/e/eantipov/Files/tt_hf/";
  vector<TString> dir_paths = get_list_of_files(path_to_ntuples);

  
  // Declare histograms
  
  // dR_min between all bjets/btags and leptons
  TH1 *mc16_minDeltaR_lep0_bjets_from_top = new TH1F("minDeltaR_lep1_bjets_fromTop", "minDeltaR_lep1_bjets_fromTop", 10, 0, 5);
  TH1 *mc16_minDeltaR_lep0_bjets_not_from_top = new TH1F("minDeltaR_lep1_bjets_notFromTop", "minDeltaR_lep1_bjets_notFromTop", 10, 0, 5);
  TH1 *mc16_minDeltaR_lep1_bjets_from_top = new TH1F("minDeltaR_lep2_bjets_fromTop", "minDeltaR_lep2_bjets_fromTop", 10, 0, 5);
  TH1 *mc16_minDeltaR_lep1_bjets_not_from_top = new TH1F("minDeltaR_lep2_bjets_notFromTop", "minDeltaR_lep2_bjets_notFromTop", 10, 0, 5);
  
  // dR_min between the 1st/2nd/3rd bjets/btags and leptons
  TH1 *mc16_minDeltaR_lep0_bjets_from_top_tags[3];
  TH1 *mc16_minDeltaR_lep0_bjets_not_from_top_tags[3];
  TH1 *mc16_minDeltaR_lep1_bjets_from_top_tags[3];
  TH1 *mc16_minDeltaR_lep1_bjets_not_from_top_tags[3];
  for (int i=0; i<3; i++) {
      TString title1 = "minDeltaR_lep1_bjets_fromTop_" + to_string(i) + "_tag";
      TString title2 = "minDeltaR_lep1_bjets_notFromTop_" + to_string(i) + "_tag";
      TString title3 = "minDeltaR_lep2_bjets_fromTop" + to_string(i) + "_tag";
      TString title4 = "minDeltaR_lep2_bjets_notFromTop" + to_string(i) + "_tag";
      mc16_minDeltaR_lep0_bjets_from_top_tags[i] = new TH1F(title1, title1, 10, 0, 5);
      mc16_minDeltaR_lep0_bjets_not_from_top_tags[i] = new TH1F(title2, title2, 10, 0, 5);
      mc16_minDeltaR_lep1_bjets_from_top_tags[i] = new TH1F(title3, title3, 10, 0, 5);
      mc16_minDeltaR_lep1_bjets_not_from_top_tags[i] = new TH1F(title4, title4, 10, 0, 5); }
  
  // dR_min between bjets/btags
  TH1 *mc16_dR_b0_b1 = new TH1F("mc16_dR_b0_b1", "mc16_dR_b0_b1", 10, 0, 5);
  TH1 *mc16_dR_b0_b2 = new TH1F("mc16_dR_b0_b2", "mc16_dR_b0_b2", 10, 0, 5);
  TH1 *mc16_dR_b1_b2 = new TH1F("mc16_dR_b1_b2", "mc16_dR_b1_b2", 10, 0, 5);
  TH1 *mc16_minDeltaR_b01_b2_from_top = new TH1F("mc16_minDeltaR_b0_b2_from_top", "mc16_minDeltaR_b0_b2_from_top", 10, 0, 5);
  TH1 *mc16_minDeltaR_b01_b2_not_from_top = new TH1F("mc16_minDeltaR_b01_b2_not_from_top", "mc16_minDeltaR_b01_b2_not_from_top", 10, 0, 5);
  
  // pT of the first three bjets/btags (not)from top
  TH1 *mc16_jet_pT0_from_top = new TH1F("mc16_jet_pT0_from_top", "mc16_jet_pT0_from_top", 10, 0, 500);
  TH1 *mc16_jet_pT1_from_top = new TH1F("mc16_jet_pT1_from_top", "mc16_jet_pT1_from_top", 10, 0, 500);
  TH1 *mc16_jet_pT2_from_top = new TH1F("mc16_jet_pT2_from_top", "mc16_jet_pT2_from_top", 10, 0, 500);
  TH1 *mc16_jet_pT0_not_from_top = new TH1F("mc16_jet_pT0_not_from_top", "mc16_jet_pT0_not_from_top", 10, 0, 500);
  TH1 *mc16_jet_pT1_not_from_top = new TH1F("mc16_jet_pT1_not_from_top", "mc16_jet_pT1_not_from_top", 10, 0, 500);
  TH1 *mc16_jet_pT2_not_from_top = new TH1F("mc16_jet_pT2_not_from_top", "mc16_jet_pT2_not_from_top", 10, 0, 500);
 
  // the first three DL1r tags distributions for 2b1l / 4b / 3b / 2b1c
  TH1 *mc16_bjets_n_2b[4];
  TH1 *mc16_tag0_DL1r[4];
  TH1 *mc16_tag1_DL1r[4];
  TH1 *mc16_tag2_DL1r[4];
  for (int i=0; i<4; i++) {
      TString h_title0 = "mc16_tag0_DL1r_TopHFFF" + to_string(i);
      TString h_title1 = "mc16_tag1_DL1r_TopHFFF" + to_string(i);
      TString h_title2 = "mc16_tag2_DL1r_TopHFFF" + to_string(i);
      mc16_tag0_DL1r[i] = new TH1F(h_title0, h_title0, 30, -15, 15);
      mc16_tag1_DL1r[i] = new TH1F(h_title1, h_title1, 30, -15, 15);
      mc16_tag2_DL1r[i] = new TH1F(h_title2, h_title2, 30, -15, 15);

      TString h_bjn_title = "mc16_bjets_n_TopHFFF"+to_string(i);
      mc16_bjets_n_2b[i] = new TH1F(h_bjn_title, h_bjn_title, 5, 0, 5); }

  // MET
  TH1 *mc16_met = new TH1F("mc16_met", "mc16_met", 20, 0, 1000);
  TH1 *mc16_met_phi = new TH1F("mc16_met_phi", "mc16_met_phi", 40, -4, 4);

  // jet pT in high-low order
  TH1 *mc16_jet_pT0 = new TH1F("mc16_jet_pT0", "mc16_jet_pT0", 20, 0, 1000);
  TH1 *mc16_jet_pT1 = new TH1F("mc16_jet_pT1", "mc16_jet_pT1", 20, 0, 1000);
  TH1 *mc16_jet_pT2 = new TH1F("mc16_jet_pT2", "mc16_jet_pT2", 20, 0, 1000);
  TH1 *mc16_jet_pT3 = new TH1F("mc16_jet_pT3", "mc16_jet_pT3", 20, 0, 1000);
  TH1 *mc16_jet_pT4 = new TH1F("mc16_jet_pT4", "mc16_jet_pT4", 20, 0, 1000);
  TH1 *mc16_jet_pT5 = new TH1F("mc16_jet_pT5", "mc16_jet_pT5", 20, 0, 1000);
  
  // leptons
  TH1 *mc16_lep0_pt = new TH1F("mc16_lep0_pt", "mc16_lep0_pt", 20, 0, 1000);
  TH1 *mc16_lep1_pt = new TH1F("mc16_lep1_pt", "mc16_lep1_pt", 20, 0, 1000);
  TH1 *mc16_lep_pt = new TH1F("mc16_lep_pt", "mc16_lep_pt", 20, 0, 1000);
  TH1 *mc16_lep0_eta = new TH1F("mc16_lep0_eta", "mc16_let0_eta", 20, -5, 5);
  TH1 *mc16_lep1_eta = new TH1F("mc16_lep1_eta", "mc16_lep1_eta", 20, -5, 5);
  TH1 *mc16_lep_eta = new TH1F("mc16_lep_eta", "mc16_lep_eta", 20, -5, 5);
  TH1 *mc16_lep0_phi = new TH1F("mc16_lep0_phi", "mc16_lep0_phi", 40, -4, 4);
  TH1 *mc16_lep1_phi = new TH1F("mc16_lep1_phi", "mc16_lep1_phi", 40, -4, 4);
  TH1 *mc16_lep_phi = new TH1F("mc16_lep_phi", "mc16_lep_phi", 40, -4, 4);
  TH1 *mc16_dR_lep0_lep1 = new TH1F("mc16_dR_lep0_lep1", "mc16_dR_lep0_lep1", 10, 0, 5);


  // Loop over directories with ntuples collections
  for (int dir_counter=0; dir_counter<dir_paths.size(); dir_counter++)
    {
      // Announce current directory
      cout << "\n\n\n" << dir_paths[dir_counter] << endl;
      

      // Check for the content: data/mc? which campaign?
      vector<TString> dir_path_components = split(dir_paths[dir_counter], '/');
      int last_element_index = dir_path_components.size();
      vector<TString> dir_name_components = split(dir_path_components[last_element_index-1], '_');
      bool is_data = false;
      bool is_2015 = false;
      bool is_2016 = false;
      bool is_2017 = false;
      bool is_2018 = false;
      bool is_mc16a = false;
      bool is_mc16d = false;
      bool is_mc16e = false;
      for (int i=0; i<dir_name_components.size(); i++)
	{
	  if (dir_name_components[i] == "data") is_data = true;
          if (dir_name_components[i] == "2015") is_2015 = true;
          if (dir_name_components[i] == "2016") is_2016 = true;
          if (dir_name_components[i] == "2017") is_2017 = true;
          if (dir_name_components[i] == "2018") is_2018 = true;
          if (dir_name_components[i] == "mc16a") is_mc16a = true;
          if (dir_name_components[i] == "mc16d") is_mc16d = true;
          if (dir_name_components[i] == "mc16e") is_mc16e = true;
	}
      
      
      // We work with MC only
      if (is_data == true) continue;
      

      // Testing option: run over mc16a campaign only to save time
      //if (is_mc16a != true) continue;
      

      // Make a list of paths to jobs/DIDs outputs (pieces of a full ntuple)
      vector<TString> paths_to_jobs = get_list_of_files(dir_paths[dir_counter]);


      // Loop over jobs/DIDs
      for (int job_number=0; job_number<paths_to_jobs.size(); job_number++)
	{
	  // Get info about the job/DID from its name
	  vector<TString> path_to_jobs_components = split(paths_to_jobs[job_number], '/');
	  TString job_name = path_to_jobs_components[path_to_jobs_components.size() - 1];
	  vector<TString> job_name_components = split(job_name, '.');
	  TString job_DID = job_name_components[2];
	  vector<TString> campaign_info = split(job_name_components[5], '_');

	  
	  // Select only jobs/physics_processes of our interest: 
	  // (1) regular (not alternamtive) samples
	  // (2) tt+any, ttbb, ttb, ttc
	  if (campaign_info[1]!="s3126") continue;
	  if (job_DID!="410472" && job_DID!="411076" && job_DID!="411077" && job_DID!="411078") { continue; }
	  else { cout << "\n\nDID: " << job_DID << endl; }


	  // Testing option: keep only tt+all if true
	  bool only_410472 = false;
	  //if (job_DID=="410472") { only_410472=true; } else { continue; }


	  // Make a list of paths to ntuples of the given job/DID
	  vector<TString> paths_to_ntuples = get_list_of_files(paths_to_jobs[job_number]);


	  // Loop over ntuples of one job/DID
	  for (int ntuple_number=0; ntuple_number<paths_to_ntuples.size(); ntuple_number++)
	    {
	      // Open ntuple
	      cout << paths_to_ntuples[ntuple_number] << endl;
	      TFile *ntuple = new TFile (paths_to_ntuples[ntuple_number]);
	      TTree *tree_nominal = (TTree*)ntuple->Get("nominal");


	      // Set all the needed branches
	      vector<Float_t> *jet_pt, *jet_DL1r, *jet_eta, *jet_phi, *mu_pt, *mu_eta, *mu_phi, *mu_charge, *el_pt, *el_eta, *el_phi, *el_charge;
	      vector<int> *topHadronOriginFlag;
	      vector<char> *jet_DL1r_77;
	      jet_pt = jet_DL1r = jet_eta = jet_phi = mu_pt = mu_eta = mu_phi = mu_charge = el_pt = el_eta = el_phi = el_charge = 0;
	      topHadronOriginFlag = 0;
	      jet_DL1r_77 = 0;
	      Float_t met, met_phi;
	      tree_nominal->SetBranchAddress("jet_pt", &jet_pt);
              tree_nominal->SetBranchAddress("jet_eta", &jet_eta);
              tree_nominal->SetBranchAddress("jet_phi", &jet_phi);
              tree_nominal->SetBranchAddress("jet_DL1r", &jet_DL1r);
              tree_nominal->SetBranchAddress("jet_isbtagged_DL1r_77", &jet_DL1r_77);
              tree_nominal->SetBranchAddress("el_pt", &el_pt);
              tree_nominal->SetBranchAddress("el_eta", &el_eta);
              tree_nominal->SetBranchAddress("el_phi", &el_phi);
              tree_nominal->SetBranchAddress("el_charge", &el_charge);
              tree_nominal->SetBranchAddress("mu_pt", &mu_pt);
              tree_nominal->SetBranchAddress("mu_eta", &mu_eta);
              tree_nominal->SetBranchAddress("mu_phi", &mu_phi);
              tree_nominal->SetBranchAddress("mu_charge", &mu_charge);
              tree_nominal->SetBranchAddress("jet_GBHInit_topHadronOriginFlag", &topHadronOriginFlag); // https://gitlab.cern.ch/TTJ/Ntuple/-/blob/master/TTJNtuple/TTJNtuple/EventSaver.h#L55 
	      tree_nominal->SetBranchAddress("met_met", &met);
	      tree_nominal->SetBranchAddress("met_phi", &met_phi);
	      

	      // Weights
	      float w_mc, w_pu, w_leptonSF, w_DL1r_77, w_jvt;
              UInt_t runNumber;
              tree_nominal->SetBranchAddress("weight_mc", &w_mc);
              tree_nominal->SetBranchAddress("weight_pileup", &w_pu);
              tree_nominal->SetBranchAddress("weight_leptonSF", &w_leptonSF);
              tree_nominal->SetBranchAddress("weight_bTagSF_DL1r_77", &w_DL1r_77);
              tree_nominal->SetBranchAddress("weight_jvt", &w_jvt);
              tree_nominal->SetBranchAddress("runNumber", &runNumber);


	      // Top flavor filter flag
	      int topHFFF;
              tree_nominal->SetBranchAddress("topHeavyFlavorFilterFlag", &topHFFF);


	      // Ignore the "ReadStreamerInfo, class:string, illegal uid=-2" erro


	      // Loop over entries
	      Int_t nEntries = tree_nominal->GetEntries();
	      cout << "\tEntries = " << nEntries << endl;
	      for (int entry=0; entry<nEntries; entry++)
		{
		  // Show events counter
		  if (entry%1000==0) { cout << "\t" << entry << "\r"; cout.flush(); }
                  tree_nominal->GetEntry(entry);


		  // Compute weights
		  double weight_lumi = 1;
                  double sumWeights = 1;
                  double campaign_lumi = 1;
                  double campaign_xsection = 1;
                  double campaign_genFiltEff = 1;
                  double kFactor = 1;
                  double total_lumi = 3.21956 + 32.9881 + 44.3074 + 58.4501;
		  if (runNumber==284500)
                    {
                      campaign_lumi = 3.21956 + 32.9881;
                      if (job_DID=="411076")
                        {
                          sumWeights = 3.33006*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.008814;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="411077")
                        {
                          sumWeights = 3.61088*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.046655;
                          kFactor = 1.1398;
                        }
                      if (job_DID=="411078")
                        {
                          sumWeights = 3.61598*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.039503;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="410472")
                        {
                          sumWeights = 5.82869*pow(10, 10);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.10547;
                          kFactor = 1.13975636159;
                        }
                    }
		  if (runNumber==300000)
                    {
                      campaign_lumi = 44.3074;
                      if (job_DID=="411076")
                        {
                          sumWeights = 4.21891*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.008814;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="411077")
                        {
                          sumWeights = 4.49595*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.046655;
                          kFactor = 1.1398;
                        }
                      if (job_DID=="411078")
                        {
                          sumWeights = 4.49400*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.039503;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="410472")
                        {
                          sumWeights = 7.26510*pow(10, 10);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.10547;
                          kFactor = 1.13975636159;
                        }
                    }
		  if (runNumber==310000)
                    {
                      campaign_lumi = 58.4501;
                      if (job_DID=="411076")
                        {
                          sumWeights = 5.47811*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.008814;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="411077")
                        {
                          sumWeights = 5.94763*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.046655;
                          kFactor = 1.1398;
                        }
                      if (job_DID=="411078")
                        {
                          sumWeights = 5.94190*pow(10, 9);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.039503;
                          kFactor = 1.1397;
                        }
                      if (job_DID=="410472")
                        {
                          sumWeights = 1.01641*pow(10, 11);
                          campaign_xsection = 0.72977;
                          campaign_genFiltEff = 0.10547;
                          kFactor = 1.13975636159;
                        }
                    }
		  // Actual computation:
		  weight_lumi = campaign_lumi * campaign_xsection * pow(10,6) * campaign_genFiltEff * kFactor / sumWeights;
		  double weights = w_mc * w_pu * w_leptonSF * w_DL1r_77 * w_jvt * weight_lumi;
		  //double weights = w_mc * weight_lumi; // Particle level
		  //double weights = 1; // Data

		  
		  // Initiate cuts names
		  bool emu_cut = false;
                  bool OS_cut = false;
                  bool jets_n_cut = false;
                  bool bjets_n2_cut = false;
                  bool bjets_n3_cut = false;
                  bool lep_pt_cut = false;
                  bool jet_pt_cut = true;
                  bool all_cuts = false;
                  bool topHFFF_cut = false;
		  

		  // Define cuts themselves
		  if ((*el_pt).size()==1 && (*mu_pt).size()==1) emu_cut = true;
		  if ((*el_charge)[0]!=(*mu_charge)[0]) OS_cut = true;
		  int bjets_n = 0;
		  for (int i=0; i<(*jet_pt).size(); i++) { if ( int((*jet_DL1r_77)[i]==1) ) bjets_n++; }
		  if (bjets_n==3) bjets_n3_cut = true;
                  if (bjets_n>=2) bjets_n2_cut = true;
                  int jets_n = (*jet_pt).size();
                  if (jets_n >=3) jets_n_cut = true;
                  float pt_value_cut = 25000;
                  if ((*el_pt)[0]>=pt_value_cut && (*mu_pt)[0]>=pt_value_cut) lep_pt_cut = true;
                  for (int i=0; i<(*jet_pt).size(); i++) { if ((*jet_pt)[i]<pt_value_cut) jet_pt_cut = false; }
		  if ( only_410472==true || ( (topHFFF==1 && job_DID=="411076") || (topHFFF==2 && job_DID=="411077") || (topHFFF==3 && job_DID=="411078") || (topHFFF==0 && job_DID=="410472") ) ) topHFFF_cut = true;


		  // 3b, emu, OS channel: Draw min_dR
		  if (emu_cut*OS_cut*bjets_n3_cut*lep_pt_cut*jet_pt_cut*topHFFF_cut*jets_n_cut == true)
		    {
		      // Define min(dR) as some lagre value to begin with
                      double min_dR1_top = 999999.; // leading lepton
                      double min_dR2_top = 999999.; // subleading lepton
                      double min_dR1_not_top = 999999.; // leading lepton
                      double min_dR2_not_top = 999999.; // subleading lepton

		      
		      // Set iterator over b-jets
		      int bjet_iter = 0;
		      
		      
		      // Loop over all jets, and select only b-tagged in the loop
		      for (int jet_i=0; jet_i<(*jet_pt).size(); jet_i++)
			{
			  if ( int((*jet_DL1r_77)[jet_i]==1) )
			  //if ( (*topHadronOriginFlag)[i]!=-99 ) // Particle level
			    {
			      // Define initial dR and dPhi(lep, jet)
			      double dR1 = 0;
			      double dR2 = 0;
			      double delta_phi_mu = (*mu_phi)[0] - (*jet_phi)[jet_i];
                              double delta_phi_el = (*el_phi)[0] - (*jet_phi)[jet_i];

			      
			      // Adjust dPhi to be not greater than 180 degrees
			      if (delta_phi_mu > TMath::Pi()) delta_phi_mu = 2*TMath::Pi() - delta_phi_mu;
			      if (delta_phi_el > TMath::Pi()) delta_phi_el = 2*TMath::Pi() - delta_phi_el;

			      
			      // Assign dR1 to the leading lep and dR2 to the subleading
			      if ((*mu_pt)[0]>(*el_pt)[0]) {
				dR1 = sqrt( pow(delta_phi_mu, 2) + pow( (*mu_eta)[0] - (*jet_eta)[jet_i], 2) );
				dR2 = sqrt( pow(delta_phi_el, 2) + pow( (*el_eta)[0] - (*jet_eta)[jet_i], 2) ); }
                              else {
				dR1 = sqrt( pow(delta_phi_el, 2) + pow( (*el_eta)[0] - (*jet_eta)[jet_i], 2) );
				dR2 = sqrt( pow(delta_phi_mu, 2) + pow( (*mu_eta)[0] - (*jet_eta)[jet_i], 2) ); }


			      // Sort into two cases: from top and not from top
			      // Fill pT histograms
			      if ((*topHadronOriginFlag)[jet_i]==4) {
				min_dR1_top = min(min_dR1_top, dR1); min_dR2_top = min(min_dR2_top, dR2);
				if (bjet_iter==0) mc16_jet_pT0_from_top->Fill((*jet_pt)[jet_i]/1000, weights);
				if (bjet_iter==1) mc16_jet_pT1_from_top->Fill((*jet_pt)[jet_i]/1000, weights);
				if (bjet_iter==2) mc16_jet_pT2_from_top->Fill((*jet_pt)[jet_i]/1000, weights); }
			      else {
				min_dR1_not_top = min(min_dR1_not_top, dR1); min_dR2_not_top = min(min_dR2_not_top, dR2);
				if (bjet_iter==0) mc16_jet_pT0_not_from_top->Fill((*jet_pt)[jet_i]/1000, weights);
				if (bjet_iter==1) mc16_jet_pT1_not_from_top->Fill((*jet_pt)[jet_i]/1000, weights);
				if (bjet_iter==2) mc16_jet_pT2_not_from_top->Fill((*jet_pt)[jet_i]/1000, weights); }


			      // Increment number of bjets by one
			      bjet_iter++;

			    } // selection of b-jets from all jets
			  
			} // [jet_i] - loop over jets, sorted wrt pT


		      // Fill min_dR histograms
		      mc16_minDeltaR_lep0_bjets_from_top->Fill(min_dR1_top, weights);
                      mc16_minDeltaR_lep0_bjets_not_from_top->Fill(min_dR1_not_top, weights);
                      mc16_minDeltaR_lep1_bjets_from_top->Fill(min_dR2_top, weights);
                      mc16_minDeltaR_lep1_bjets_not_from_top->Fill(min_dR2_not_top, weights);
		      
		    } // 3b, emu, OS cuts



		  // 2+b, emu, OS channel
		  if (emu_cut*OS_cut*bjets_n2_cut*lep_pt_cut*jet_pt_cut*topHFFF_cut*jets_n_cut == true)
		    {
		      // MET plots:
		      mc16_met->Fill(met, 0.001);
		      mc16_met_phi->Fill(met_phi);


		      // jet pT plots in pT order:
		      mc16_jet_pT0->Fill((*jet_pt)[0], 0.001);
                      mc16_jet_pT1->Fill((*jet_pt)[1], 0.001);
                      mc16_jet_pT2->Fill((*jet_pt)[2], 0.001);
                      if ((*jet_pt).size()>=4) mc16_jet_pT3->Fill((*jet_pt)[3], 0.001);
                      if ((*jet_pt).size()>=5) mc16_jet_pT4->Fill((*jet_pt)[4], 0.001);
                      if ((*jet_pt).size()>=6) mc16_jet_pT5->Fill((*jet_pt)[5], 0.001);
		      

		      // leptons kinematic hists
		      if ( (*el_pt)[0] > (*mu_pt)[0] ) {		
			mc16_lep0_pt->Fill((*el_pt)[0], 0.001);
			mc16_lep1_pt->Fill((*mu_pt)[0], 0.001);
			mc16_lep0_eta->Fill((*el_eta)[0]);
			mc16_lep1_eta->Fill((*mu_eta)[0]);
			mc16_lep0_phi->Fill((*el_phi)[0]);
			mc16_lep1_phi->Fill((*mu_phi)[0]); }
		      else {
			mc16_lep0_pt->Fill((*mu_pt)[0], 0.001);
                        mc16_lep1_pt->Fill((*el_pt)[0], 0.001);
                        mc16_lep0_eta->Fill((*mu_eta)[0]);
                        mc16_lep1_eta->Fill((*el_eta)[0]);
                        mc16_lep0_phi->Fill((*mu_phi)[0]);
                        mc16_lep1_phi->Fill((*el_phi)[0]); }
		      
		      mc16_lep_pt->Fill((*el_pt)[0], 0.001);
		      mc16_lep_pt->Fill((*mu_pt)[0], 0.001);
		      mc16_lep_eta->Fill((*el_eta)[0]);
		      mc16_lep_eta->Fill((*mu_eta)[0]);
		      mc16_lep_phi->Fill((*el_phi)[0]);
		      mc16_lep_phi->Fill((*mu_phi)[0]);
		      
		      // Initiate variables for dR_lep0_lep1 computation:
		      double dR_lep0_lep1 = 0;
		      double delta_phi_lep0_lep1 = (*el_phi)[0] - (*mu_phi)[0];
		      double delta_eta_lep0_lep1 = (*el_eta)[0] - (*mu_eta)[0];
		      
		      // Adjust dPhi to be not greater than 180 degrees:
		      if (delta_phi_lep0_lep1 > TMath::Pi()) delta_phi_lep0_lep1 = 2*TMath::Pi() - delta_phi_lep0_lep1;
		      
		      // Compute dR_lep0_lep1:
		      dR_lep0_lep1 = sqrt( pow(delta_phi_lep0_lep1, 2) + pow(delta_eta_lep0_lep1, 2) );
		      mc16_dR_lep0_lep1->Fill(dR_lep0_lep1);


		      
		      // Sort jets wrt DL1r tag weights
		      sort (jet_DL1r->begin(), jet_DL1r->end(), greater<int>());


		      // Fill DL1r tag weight histos for the first three tags, also sort wrt topHFFF
		      mc16_tag0_DL1r[topHFFF]->Fill((*jet_DL1r)[0], weights);
                      mc16_tag1_DL1r[topHFFF]->Fill((*jet_DL1r)[1], weights);
                      mc16_tag2_DL1r[topHFFF]->Fill((*jet_DL1r)[2], weights);


		      // And fill the bjets_n histogram (need it for testing purposes)
		      mc16_bjets_n_2b[topHFFF]->Fill(bjets_n, weights);

		      
		      // min_dR for the first three tags
		      // Define min(dR) as some lagre value to begin with
		      double min_dR0_top = 999999.; // leading lepton 
		      double min_dR1_top = 999999.; // subleading lepton
		      double min_dR0_not_top = 999999.; // leading lepton
		      double min_dR1_not_top = 999999.; // subleading lepton
		      

		      // Indeces for the b_jets beign selected for min_dR value
		      int min_dR0_top_idx = 0;
		      int min_dR1_top_idx = 0;
		      int min_dR0_not_top_idx = 0;
		      int min_dR1_not_top_idx = 0;
		      
		      // Loop over jets to compute min_dR
		      for (int jet_i=0; jet_i<(*jet_pt).size(); jet_i++)
			{
			  // Define initial dR and dPhi(lep, jet)
			  double dR0 = 0;
			  double dR1 = 0;
			  double delta_phi_mu = (*mu_phi)[0] - (*jet_phi)[jet_i];
			  double delta_phi_el = (*el_phi)[0] - (*jet_phi)[jet_i];

			  
			  // Adjust dPhi to be not greater than 180 degrees
			  if (delta_phi_mu > TMath::Pi()) delta_phi_mu = 2*TMath::Pi() - delta_phi_mu;
			  if (delta_phi_el > TMath::Pi()) delta_phi_el = 2*TMath::Pi() - delta_phi_el;


			  // Assign dR1 to the leading lep and dR2 to the subleading
			  if ((*mu_pt)[0]>(*el_pt)[0]) {
			    dR0 = sqrt( pow(delta_phi_mu, 2) + pow( (*mu_eta)[0] - (*jet_eta)[jet_i], 2) );
			    dR1 = sqrt( pow(delta_phi_el, 2) + pow( (*el_eta)[0] - (*jet_eta)[jet_i], 2) ); }
			  else {
			    dR0 = sqrt( pow(delta_phi_el, 2) + pow( (*el_eta)[0] - (*jet_eta)[jet_i], 2) );
			    dR1 = sqrt( pow(delta_phi_mu, 2) + pow( (*mu_eta)[0] - (*jet_eta)[jet_i], 2) ); }


			  // Sort into two cases: from top and not from top
			  if ((*topHadronOriginFlag)[jet_i]==4) {
			    double min_dR0_top_old = min_dR0_top; double min_dR1_top_old = min_dR1_top;
			    min_dR0_top = min(min_dR0_top, dR0); min_dR1_top = min(min_dR1_top, dR1); 
			    if (min_dR0_top_old!=min_dR0_top) min_dR0_top_idx = jet_i;
			    if (min_dR1_top_old!=min_dR1_top) min_dR1_top_idx = jet_i; }
			  else {
			    double min_dR0_not_top_old = min_dR0_not_top; double min_dR1_not_top_old = min_dR1_not_top;
			    min_dR0_not_top = min(min_dR0_not_top, dR0); min_dR1_not_top = min(min_dR1_not_top, dR1); 
			    if (min_dR0_not_top_old!=min_dR0_not_top) min_dR0_not_top_idx = jet_i; 
			    if (min_dR1_not_top_old!=min_dR1_not_top) min_dR1_not_top_idx = jet_i; }
       
			} //[jet_i] - loop over jets t ocompute min_dR


		      // Fill the min_dR histos for the first three tag weights only
		      if (min_dR0_top_idx<3) mc16_minDeltaR_lep0_bjets_from_top_tags[min_dR0_top_idx]->Fill(min_dR0_top, weights);
		      if (min_dR0_not_top_idx<3) mc16_minDeltaR_lep0_bjets_not_from_top_tags[min_dR0_not_top_idx]->Fill(min_dR0_not_top, weights);
		      if (min_dR1_top_idx<3) mc16_minDeltaR_lep1_bjets_from_top_tags[min_dR1_top_idx]->Fill(min_dR1_top, weights);
		      if (min_dR1_not_top_idx<3) mc16_minDeltaR_lep1_bjets_not_from_top_tags[min_dR1_not_top_idx]->Fill(min_dR1_not_top, weights); 

		      
		      // Initiate variables to compute dR between bjets/btags:
		      double dR_b0_b1 = 0;
		      double dR_b0_b2 = 0;
		      double dR_b1_b2 = 0;
		      double min_dR_b01_b2_from_top = 0;
		      double min_dR_b01_b2_not_from_top = 0;
		      
		      double delta_eta_b0_b1 = (*jet_eta)[0] - (*jet_eta)[1];
                      double delta_eta_b0_b2 = (*jet_eta)[0] - (*jet_eta)[2];
                      double delta_eta_b1_b2 = (*jet_eta)[1] - (*jet_eta)[2];
		      double delta_phi_b0_b1 = (*jet_phi)[0] - (*jet_phi)[1];
		      double delta_phi_b0_b2 = (*jet_phi)[0] - (*jet_phi)[2];
		      double delta_phi_b1_b2 = (*jet_phi)[1] - (*jet_phi)[2];

		      // Adjust dPhi to be not greater than 180 degrees:
		      if (delta_phi_b0_b1 > TMath::Pi()) delta_phi_b0_b1 = 2*TMath::Pi() - delta_phi_b0_b1;
		      if (delta_phi_b0_b2 > TMath::Pi()) delta_phi_b0_b2 = 2*TMath::Pi() - delta_phi_b0_b2;
		      if (delta_phi_b1_b2 > TMath::Pi()) delta_phi_b1_b2 = 2*TMath::Pi() - delta_phi_b1_b2;

		      // Compute dR between bjets/btags and fill hists:
		      dR_b0_b1 = sqrt( pow(delta_phi_b0_b1, 2) + pow(delta_eta_b0_b1, 2) );
		      dR_b0_b2 = sqrt( pow(delta_phi_b0_b2, 2) + pow(delta_eta_b0_b2, 2) );
		      dR_b1_b2 = sqrt( pow(delta_phi_b1_b2, 2) + pow(delta_eta_b1_b2, 2) );
		      mc16_dR_b0_b1->Fill(dR_b0_b1);
		      mc16_dR_b0_b2->Fill(dR_b0_b2);
		      mc16_dR_b1_b2->Fill(dR_b1_b2);
		      // Assuming the 3rd btag is additional:
		      if ((*topHadronOriginFlag)[2]==4) { mc16_minDeltaR_b01_b2_from_top->Fill(min(dR_b0_b2, dR_b1_b2)); }
		      else if ((*topHadronOriginFlag)[2]!=-99) { mc16_minDeltaR_b01_b2_not_from_top->Fill(min(dR_b0_b2, dR_b1_b2)); }
			
		    } // 2+b, emu, OS cuts 

		} // [entry] - loop over entries
	      
	      // Close ntuple after we're done with it
	      ntuple->Close();
	      
	    } // [ntuple_number] - loop over ntuples of a particular job
	  
	} // [job_number] - loop over jobs (pieces) of a collection.

    } // [dir_counter] - loop over directories names with jobs folders: mc16a, mc16d, mc16e, data



  // Save histograms
  TFile *hists_file = new TFile("hists_mc.root", "RECREATE");
  for (int topHFFF_i=0; topHFFF_i<4; topHFFF_i++) {
    TString process = "";
    if (topHFFF_i==0) process = "2b1l";
    if (topHFFF_i==1) process = "4b";
    if (topHFFF_i==2) process = "3b";
    if (topHFFF_i==3) process = "2b1c";
    mc16_tag0_DL1r[topHFFF_i]->Write("DL1r_templates_"+process+"_1st_tag");
    mc16_tag1_DL1r[topHFFF_i]->Write("DL1r_templates_"+process+"_2nd_tag");
    mc16_tag2_DL1r[topHFFF_i]->Write("DL1r_templates_"+process+"_3rd_tag");
    mc16_bjets_n_2b[topHFFF_i]->Write("2b_emu_OS_"+process+"bjets_n"); }
  
  mc16_jet_pT0_from_top->Write("3b_emu_OS_jet_pT0_from_top");
  mc16_jet_pT1_from_top->Write("3b_emu_OS_jet_pT1_from_top");
  mc16_jet_pT2_from_top->Write("3b_emu_OS_jet_pT2_from_top");
  mc16_jet_pT0_not_from_top->Write("3b_emu_OS_jet_pT0_not_from_top");
  mc16_jet_pT1_not_from_top->Write("3b_emu_OS_jet_pT0_not_from_top");
  mc16_jet_pT2_not_from_top->Write("3b_emu_OS_jet_pT0_not_from_top");

  mc16_minDeltaR_lep0_bjets_from_top->Write("3b_emu_OS_min_dR_lep0_b_from_top");
  mc16_minDeltaR_lep1_bjets_from_top->Write("3b_emu_OS_min_dR_lep1_b_from_top");
  mc16_minDeltaR_lep0_bjets_not_from_top->Write("3b_emu_OS_min_dR_lep0_b_not_from_top");
  mc16_minDeltaR_lep1_bjets_not_from_top->Write("3b_emu_OS_min_dR_lep1_b_not_from_top");
      
  for (int i=0; i<3; i++) {
    TString savename1 = "2b_emu_OS_min_dR_lep0_b_from_top_" + to_string(i+1) + "tag";
    TString savename2 = "2b_emu_OS_min_dR_lep0_b_not_from_top_" + to_string(i+1) + "tag";
    TString savename3 = "2b_emu_OS_min_dR_lep1_b_from_top_" + to_string(i+1) + "tag";
    TString savename4 = "2b_emu_OS_min_dR_lep1_b_not_from_top_" + to_string(i+1) + "tag";
    mc16_minDeltaR_lep0_bjets_from_top_tags[i]->Write(savename1);
    mc16_minDeltaR_lep0_bjets_not_from_top_tags[i]->Write(savename2);
    mc16_minDeltaR_lep1_bjets_from_top_tags[i]->Write(savename3);
    mc16_minDeltaR_lep1_bjets_not_from_top_tags[i]->Write(savename4); }

  mc16_jet_pT0->Write("2b_emu_OS_jet_pt0");
  mc16_jet_pT1->Write("2b_emu_OS_jet_pt1");
  mc16_jet_pT2->Write("2b_emu_OS_jet_pt2");
  mc16_jet_pT3->Write("2b_emu_OS_jet_pt3");
  mc16_jet_pT4->Write("2b_emu_OS_jet_pt4");
  mc16_jet_pT5->Write("2b_emu_OS_jet_pt5");
  
  mc16_dR_b0_b1->Write("2b_emu_OS_dR_b0_b1");
  mc16_dR_b0_b2->Write("2b_emu_OS_dR_b0_b2");
  mc16_dR_b1_b2->Write("2b_emu_OS_dR_b1_b2");
  mc16_minDeltaR_b01_b2_from_top->Write("2b_emu_OS_min_dR_b01_b2_from_top");
  mc16_minDeltaR_b01_b2_not_from_top->Write("2b_emu_OS_min_dR_b01_b2_not_from_top");
  
  mc16_met->Write("2b_emu_OS_met");
  mc16_met_phi->Write("2b_emu_OS_met_phi");
  
  mc16_lep0_pt->Write("2b_emu_OS_lep0_pt");
  mc16_lep1_pt->Write("2b_emu_OS_lep1_pt");
  mc16_lep_pt->Write("2b_emu_OS_lep_pt");
  mc16_lep0_eta->Write("2b_emu_OS_lep0_eta");
  mc16_lep1_eta->Write("2b_emu_OS_lep1_eta");
  mc16_lep_eta->Write("2b_emu_OS_lep_eta");
  mc16_lep0_phi->Write("2b_emu_OS_lep0_phi");
  mc16_lep1_phi->Write("2b_emu_OS_lep1_phi");
  mc16_lep_phi->Write("2b_emu_OS_lep_phi");
  mc16_dR_lep0_lep1->Write("2b_emu_OS_dR_lep0_lep1");
  
  
  // Close the gists file
  hists_file->Close();
}
