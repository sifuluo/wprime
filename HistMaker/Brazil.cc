{


  vector<vector<double>> vlimits;
  vector<double> masspoints{450.,550.};
  // double x = 244.7 / 0.33;
  vector<double> xsecs;
  xsecs.push_back(574.1 + 574.6);
  vlimits.push_back(vector<double>{0.0187,0.0252,0.0347,0.0482,0.0643});

  int n = vlimits.size();
  if (n == 1) {
    vlimits.push_back(vlimits[0]);
    xsecs.push_back(xsecs[0]);
    n = 2;
  }
  if (n != masspoints.size()) cout << "Mass point wrong" <<endl;
  for (unsigned i = 0; i < n; ++i) {
    cout << "For M = " << masspoints[i] << ", limits are:" << endl;
    for (unsigned j = 0; j < vlimits[i].size(); ++j) {
      cout << "r = " << vlimits[i][j] << ", ";
      vlimits[i][j] *= xsecs[i];
      cout << "x-section = " << vlimits[i][j] <<endl;
    }
    cout <<endl;
  }


  TGraph *gryellow = new TGraph(2*n);
  TGraph *grgreen = new TGraph(2*n);
  TGraph *grmedian = new TGraph(n);
  double frameuplimit = 0;

  for (unsigned i = 0; i< n; ++i) {
    double mp = masspoints[i];
    gryellow->SetPoint(i, mp, vlimits[i][4]);
    grgreen->SetPoint(i, mp, vlimits[i][3]);
    grmedian->SetPoint(i, mp, vlimits[i][2]);
    grgreen->SetPoint(2*n-1-i, mp, vlimits[i][1]);
    gryellow->SetPoint(2*n-1-i, mp, vlimits[i][0]);
    if (vlimits[i][4] > frameuplimit) frameuplimit = vlimits[i][4];
  }
  double h1 = 600;
  double w1 = 800;
  double t1 = 0.08*h1;
  double b1 = 0.12*h1;
  double l1 = 0.12*w1;
  double r1 = 0.04*w1;
  TCanvas *c = new TCanvas("c","c",100,100,w1,h1);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetFrameFillStyle(0);
  c->SetFrameBorderMode(0);
  c->SetLeftMargin( l1/w1 );
  c->SetRightMargin( r1/w1 );
  c->SetTopMargin( t1/h1 );
  c->SetBottomMargin( b1/h1 );
  c->SetTickx(0);
  c->SetTicky(0);
  c->SetGrid();
  c->cd();

  TH1F * fr = c->DrawFrame(100,0,1000,1000);
  fr->GetYaxis()->CenterTitle();
  fr->GetYaxis()->SetTitleSize(0.05);
  fr->GetXaxis()->SetTitleSize(0.05);
  fr->GetYaxis()->SetLabelSize(0.04);
  fr->GetXaxis()->SetLabelSize(0.04);
  fr->GetYaxis()->SetTitleOffset(0.9);
  fr->GetXaxis()->SetNdivisions(512);
  fr->GetYaxis()->CenterTitle(true);
  fr->GetYaxis()->SetTitle("95% CL_{s} limit on #sigma_{signals}");
  fr->GetXaxis()->SetTitle("Simulated W' mass");
  fr->SetMinimum(0);
  fr->SetMaximum(frameuplimit*1.05);
  fr->GetXaxis()->SetLimits(0,1000);

  gryellow->SetFillColor(kOrange);
  gryellow->SetLineColor(kOrange);
  gryellow->SetFillStyle(1001);
  gryellow->Draw("F");

  grgreen->SetFillColor(kGreen+1);
  grgreen->SetLineColor(kGreen+1);
  grgreen->SetFillStyle(1001);
  grgreen->Draw("Fsame");

  grmedian->SetLineColor(1);
  grmedian->SetLineWidth(2);
  grmedian->SetLineStyle(2);
  grmedian->Draw("Lsame");

  fr->Draw("sameaxis");

  TLegend *leg = new TLegend(0.15,0.6,0.4,0.76);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.041);
  leg->SetTextFont(42);
  leg->AddEntry(grmedian, "Asymptotic CL_{s} expected","L");
  leg->AddEntry(grgreen, "#pm 1 std. dev.","f");
  leg->AddEntry(gryellow, "#pm 2 std. dev.","f");
  leg->Draw();

  c->SaveAs("UpperLimit.pdf");

}
