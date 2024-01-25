#include "TFile.h"
#include "TString.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"

#include "../Utilities/Dataset.cc"
#include "../Utilities/Configs.cc"
#include "Utilities/CMSStyle.cc"
#include "../Utilities/Permutations.cc"
#include "Utilities/Regions.cc"
#include "Utilities/Uncertainties.cc"

void DrawPlotAuxHist(int iter = 0) {
  int isampleyear = 3;
  Configs *conf = new Configs(isampleyear); // Maybe install more functions later. Now only need to use some variables.
  string outputpath = "AuxPlots/";
  setTDRStyle();

  if (iter == 0) { // bTagging efficiency
    string sampletype = "Merged";
    string filename = "bTagEff_" + conf->SampleYear + "_" + sampletype + ".root";
    // gStyle->SetPadRightMargin(0.10);
    TString FileName = conf->AuxHistBasePath + filename;
    TFile* f_eff = new TFile(FileName,"READ");
    cout << "Reading from bTagEff file " << FileName << endl;
    if (f_eff->IsZombie()) {
      cout << "Faild to read bTagEff file." <<endl;
      return;
    }
    vector<TH2F*> h_eff;
    h_eff.resize(6);
    h_eff[0] = (TH2F*) f_eff->Get("BtagPass_L");
    h_eff[1] = (TH2F*) f_eff->Get("BtagPass_M");
    h_eff[2] = (TH2F*) f_eff->Get("BtagPass_T");
    TH2F* TotalEvts = (TH2F*) f_eff->Get("TotalEvts");
    h_eff[0]->Divide(TotalEvts);
    h_eff[1]->Divide(TotalEvts);
    h_eff[2]->Divide(TotalEvts);
    h_eff[0]->SetDirectory(0);
    h_eff[1]->SetDirectory(0);
    h_eff[2]->SetDirectory(0);
    TotalEvts->SetDirectory(0);
    f_eff->Close();
    cout << "Done reading bTagEff file. Testing tight bTagging eff. at pT = 100 is ";
    cout << h_eff[2]->GetBinContent(h_eff[2]->FindBin(5,100)) <<endl;

    TH2F* h = h_eff[2];
    for (unsigned i = 0; i < h->GetNbinsY(); ++i) {
      h->SetBinContent(h->FindBin(3.,h->GetYaxis()->GetBinCenter(i + 1)), h->GetBinContent(h->FindBin(0.,h->GetYaxis()->GetBinCenter(i + 1))));
    }
    h->GetXaxis()->SetRangeUser(3 , 5);
    h->GetYaxis()->SetRangeUser(20,3000);
    h->SetTitle(";b-tagging efficiencies;Jet p_{T}");

    h->GetXaxis()->CenterTitle();
    h->GetXaxis()->SetBinLabel(h->FindBin(3.), "u/d/s/g");
    h->GetXaxis()->SetBinLabel(h->FindBin(4.), "c");
    h->GetXaxis()->SetBinLabel(h->FindBin(5.), "b");

    // h->GetYaxis()->CenterTitle();

    TCanvas *c1 = new TCanvas("c1","c1",800,800);
    c1->UseCurrentStyle();
    c1->SetRightMargin(0.12);
    h->GetZaxis()->SetRangeUser(0.0005,1.);
    h->Draw("colz");
    c1->SetLogz();
    CMSFrame(c1, isampleyear);
    TString outputname = outputpath + "bTagEff.pdf";
    c1->SaveAs(outputname);
    delete c1;
  }

  if (iter == 1) { // Permutations
    string sampletype = "Merged";
    string filename = "Permutations_" + conf->SampleYear + "_" + sampletype + ".root";
    TString FileName = conf->AuxHistBasePath + filename;
    TFile *PermFile = new TFile(FileName,"READ");
    vector<string> PermSamples;
    vector<TString> LegName;
    Permutations PM;
    vector<int> PtPerms = PM.PtPerms;
    vector<int> bTagPerms = {0,1,10,11,100,101,110,111};
    // tdrGrid(1);
    for (unsigned i = 0; i < 9; ++i) {
      PermSamples.push_back(Form("FL%i", (i + 3) * 100));
      LegName.push_back(Form("m(W_{h}) = %i", (i + 3) * 100));
    }
    for (unsigned i = 0; i < 9; ++i) {
      PermSamples.push_back(Form("LL%i", (i + 3) * 100));
      LegName.push_back(Form("m(W_{l}) = %i", (i + 3) * 100));
    }
    for (unsigned imass = 0; imass < 9; ++imass) {
      for (unsigned ih = 0; ih < 2; ++ih) {
        string prefix = "PtPerm_";
        if (ih == 1) prefix = "bTagPerm_";
        TString HistNameFL = prefix + conf->SampleYear + "_" + PermSamples[imass];
        TString HistNameLL = prefix + conf->SampleYear + "_" + PermSamples[imass+9];
        TH1F* h1 = (TH1F*) PermFile->Get(HistNameFL);
        TH1F* h2 = (TH1F*) PermFile->Get(HistNameLL);
        h1->SetDirectory(0);
        h2->SetDirectory(0);
        h1->SetLineColor(2);
        h2->SetLineColor(4);
        h2->SetLineStyle(2);
        h1->Scale(1. / h1->GetMaximum());
        h2->Scale(1. / h2->GetMaximum());
        if (ih == 1) {
          h1->GetXaxis()->SetRange(1,8);
          h2->GetXaxis()->SetRange(1,8);
        }
        TCanvas *c1 = new TCanvas("c1","c1",800,800);
        c1->cd();
        if (ih == 0) {
          for (unsigned ib = 0; ib < PtPerms.size(); ++ib) {
            h1->GetXaxis()->SetBinLabel(ib+1, Form("%05i",PtPerms[ib]));
          }
          h1->GetXaxis()->SetLabelSize(0.02);
          h1->SetTitle(";p_{T} Permutations Distribution; Likelihood Weight");
          h1->GetXaxis()->SetTitleOffset(1.3);
        }
        else {
          for (unsigned ib = 0; ib < bTagPerms.size(); ++ib) {
            h1->GetXaxis()->SetBinLabel(ib+1, Form("%05i",bTagPerms[ib]));
            h1->SetTitle(";bTagging Permutations Distribution; Likelihood Weight");
          }
        }
        h1->GetXaxis()->CenterTitle();
        h1->Draw("hist");
        h2->Draw("samehist");
        TLegend *leg1 = new TLegend(0.6,0.65,0.85,0.9,"","NDC");
        leg1->SetBorderSize(1);
        leg1->AddEntry(h1,LegName[imass],"l");
        leg1->AddEntry(h2,LegName[imass+9],"l");
        leg1->Draw();
        CMSFrame(c1,isampleyear);
        TString outputname = outputpath + prefix + Form("M%i.pdf",(imass+3)*100);
        c1->SaveAs(outputname);
        delete c1;
      }
    }
    TString WPrimedRHistName = "WPrimedR_" + conf->SampleYear + "_FL500";
    TH1F* WPrimedRHist = (TH1F*) PermFile->Get(WPrimedRHistName);
    WPrimedRHist->SetDirectory(0);
    WPrimedRHist->Scale(1./WPrimedRHist->GetMaximum());
    TCanvas *c1 = new TCanvas("c1","c1",800,800);
    c1->cd();
    WPrimedRHist->SetTitle(";#Delta R(t,b) of m(W'_{h}) = 500 GeV; Likelihood Weight");
    WPrimedRHist->GetXaxis()->CenterTitle();
    WPrimedRHist->GetXaxis()->SetTitleOffset(1.3);
    WPrimedRHist->Draw("hist");
    CMSFrame(c1,isampleyear);
    TString outputname = outputpath + "WPrimedR.pdf";
    c1->SaveAs(outputname);
    delete c1;
  }

  if (iter == 2) { // JetScales
    gStyle->SetOptFit(0);
    const vector<double> etabins{0., 1.3, 2.5, 3.0, 5.2}; // size 5, 4 bins, ieta top at 3;
    const vector<vector<double> > ptbins{
      {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,130., 150.,180.,220., 260., 300.,350.,400.,500.,1000.,10000.}, // 23 bins, 24 numbers
      {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260., 300.,10000.}, // 18 bins
      {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150., 180.,220.,260.,10000.}, // 17 bins
      {0.,30.,32.,34.,37.,40.,45.,50.,57.,65.,75.,90.,110.,150.,10000.} //14 bins
    };
    string sampletype = "Merged";
    string filename = "Scale_" + conf->SampleYear + "_" + sampletype + ".root";
    TString FileName = conf->AuxHistBasePath + filename;
    TFile *ScaleFile = new TFile(FileName,"READ");
    for (unsigned ieta = 0; ieta < etabins.size() - 1; ++ieta) {
      TString ProfName = Form("Eta%i",ieta);
      TString ProfTitle = Form(";p_{T} in %.1f<|#eta|<%.1f; Jet response", etabins[ieta], etabins[ieta + 1]);
      double xbins[24];
      for (unsigned ipt = 0; ipt < ptbins[ieta].size(); ++ipt) xbins[ipt] = ptbins[ieta][ipt];
      TH1F *prof = new TH1F(ProfName, ProfTitle, ptbins[ieta].size() - 1, xbins);
      for (unsigned ipt = 0; ipt < ptbins[ieta].size() - 1; ++ipt) {
        TString sn = Form("eta%d_pt%d", ieta, ipt);
        TH1F* h1 = (TH1F*) ScaleFile->Get(sn);
        h1->Scale(1./h1->GetMaximum());
        // jeseta.push_back(h1);
        double max = h1->GetMaximum();
        double mean = h1->GetMean();
        double stddev = h1->GetStdDev();
        TString fsn = Form("f_eta%d_pt%d", ieta, ipt);
        TF1* f1 = new TF1(fsn,"gaus",0., 2.);
        f1->SetParameters(max / 4.0, mean, stddev);
        h1->Fit(f1, "RMQ0", "", 0., 2.);
        TCanvas *c1 = new TCanvas("c1","c1",800,800);
        c1->cd();
        TString HistTitle = Form(";Jet Response on %.1f<|#eta|<%.1f , %.0f<p_{T}<%.0f;Likelihood Weight",etabins[ieta],etabins[ieta+1],ptbins[ieta][ipt],ptbins[ieta][ipt+1]);
        h1->SetTitle(HistTitle);
        h1->GetXaxis()->CenterTitle();
        h1->GetXaxis()->SetRangeUser(0,3);
        h1->Draw("hist");
        f1->Draw("same");
        TLegend *leg1 = new TLegend(0.6,0.65,0.85,0.9,"","NDC");
        leg1->SetBorderSize(1);
        leg1->AddEntry(h1,"Disribution","l");
        leg1->AddEntry(f1,"Fit","l");
        leg1->Draw();
        CMSFrame(c1,isampleyear);
        TString outputname = outputpath + Form("Scale_eta_%.1f_%.1f_pt_%.0f_%.0f.pdf",etabins[ieta], etabins[ieta+1],ptbins[ieta][ipt],ptbins[ieta][ipt+1]);
        c1->SaveAs(outputname);
        delete c1;
        prof->SetBinContent(ipt + 1, f1->GetParameter(1));
        prof->SetBinError(ipt + 1, f1->GetParameter(2));
      }
      TCanvas *c1 = new TCanvas("c1","c1",800,800);
      c1->cd();
      prof->GetXaxis()->CenterTitle();
      prof->GetXaxis()->SetRange(2,prof->GetNbinsX());
      prof->Draw("E0");
      CMSFrame(c1,isampleyear);
      c1->SetLogx();
      TString outputname = outputpath + Form("ScaleProfile_eta_%.1f_%.1f.pdf",etabins[ieta], etabins[ieta+1]);
      c1->SaveAs(outputname);
      delete c1;
    }
    vector<TString> MassNames = {"LeptMass","HadtMass","HadWMass"};
    for (unsigned im = 0; im < MassNames.size(); ++im) {
      TH1F* h1 = (TH1F*) ScaleFile->Get(MassNames[im]);
      h1->SetDirectory(0);
      h1->Scale(1./h1->GetMaximum());
      if (im == 0) h1->SetTitle(";m(t_{l}); Likelihood Weight");
      else if (im == 1) h1->SetTitle(";m(t_{h}); Likelihood Weight");
      else if (im == 2) h1->SetTitle(";m(W_{h}); Likelihood Weight");
      h1->GetXaxis()->CenterTitle();
      TCanvas *c1 = new TCanvas("c1","c1",800,800);
      c1->cd();
      h1->Draw("hist");
      CMSFrame(c1,isampleyear);
      TString outputname = outputpath + MassNames[im] + ".pdf";
      c1->SaveAs(outputname);
      delete c1;
    }
  }

  if (iter == 3) { // MCReweight
    gStyle->SetOptFit(0);
    TString HistPath = "/afs/cern.ch/user/s/siluo/EOS/WPrimeAnalysis/ValidationFitted/Hists/";
    TString HistName = conf->SampleYear + "_Validation_ReweightSF.root";
    TString FileName = HistPath + HistName;
    TFile *f = new TFile(FileName,"READ");
    vector<string> Regions = {"1151", "1161", "2151", "2161", "1152", "1162", "2152", "2162"};
    vector<string> RTitle = {"#font[12]{#mu} 5j1b", "#font[12]{#mu} 6j1b", "#font[12]{e} 5j1b", "#font[12]{e} 6j1b", "#font[12]{#mu} 5j2b", "#font[12]{#mu} 6j2b","#font[12]{e} 5j2b", "#font[12]{e} 6j2b"};
    vector<string> Variations = rm.Variations;
    for (unsigned ir = 0; ir < Regions.size(); ++ir) {
      Uncertainties *unc = new Uncertainties();
      TF1* f1;
      for (unsigned iv = 0; iv < Variations.size(); ++iv) {
        if (iv > 0 && iv % 2 == 0) continue;
        TString HistName1 = "ttbarReweightSF_ST_" + Regions[ir] + "_" + Variations[iv];
        TString HistName2 = "ttbarReweightSF_ST_" + Regions[ir] + "_" + Variations[iv + 1];
        if (iv == 0) {
          TString FuncName = "MCRFunc" + Regions[ir] + Variations[iv];
          TH1F* h1 = (TH1F*) f->Get(HistName1)->Clone();
          h1->SetDirectory(0);
          unc->AddCentral(h1);
          f1 = h1->GetFunction(FuncName);
          continue;
        }
        TH1F* hup = (TH1F*) f->Get(HistName1)->Clone();
        TH1F* hdown = (TH1F*) f->Get(HistName2)->Clone();
        hup->SetDirectory(0);
        hdown->SetDirectory(0);
        unc->AddVarianceSet(hup, hdown);
      }
      TGraph *gr = unc->CreateErrorGraph();
      gr->SetFillColor(1);
      TCanvas *c1 = new TCanvas("c1","c1",800,800);
      c1->cd();
      unc->hcentral->GetYaxis()->SetRangeUser(0.5,2.);
      TString HistTitle = "; ST ((Data - Other MC)/ttbar) in region    " + RTitle[ir] + "; Scale Factor";
      unc->hcentral->SetTitle(HistTitle);
      unc->hcentral->GetXaxis()->CenterTitle();
      unc->hcentral->Draw();
      f1->Draw("same");
      gr->Draw("samef");
      CMSFrame(c1,isampleyear);
      TString outputname = outputpath + "ReweightSF_" + Regions[ir] + ".pdf";
      c1->SaveAs(outputname);
      delete c1;
    }
  }
}