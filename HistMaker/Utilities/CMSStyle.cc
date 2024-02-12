#ifndef CMSSTYLE_CC
#define CMSSTYLE_CC

#include "TVirtualPad.h"
#include "TLatex.h"
#include "TString.h"
#include "TStyle.h"
#include "../../Utilities/Dataset.cc"

void CMSFrame(TVirtualPad* pad, int year, bool NoData = false, bool NoLumi = false) {
	// text sizes and text offsets with respect to the top frame
	// in unit of the top margin size
  TString cmsText     = "CMS";
  float cmsTextFont   = 61;  // default is helvetic-bold
	float cmsTextSize   = 0.75;
	float cmsTextOffset = 0.1;

  TString extraText   = "Work In Progress";
  if (NoData) extraText = "Simulation Work In Progress";
  float extraTextFont = 52;  // default is helvetica-italics
	float extraOverCmsTextSize  = 0.8; // ratio of extra text size and cmsTextSize

	vector<double> CMSLumiYears = dlib.CMSLumiYears;
  double lumi = 0;
  if (year < 4) lumi = CMSLumiYears[year];
  else lumi = CMSLumiYears[0] + CMSLumiYears[1] + CMSLumiYears[2] + CMSLumiYears[3];
  TString lumiText = Form("%.1f fb^{-1} (13TeV)", lumi);
  if (NoData && NoLumi) lumiText = dlib.SampleYears[year] + " Simulation (13TeV)";
	float lumiTextFont     = 42;
  float lumiTextSize     = 0.6;
  float lumiTextOffset   = 0.1;


  float H = float(pad->GetWh()) * pad->GetHNDC();
	float W = float(pad->GetWw()) * pad->GetWNDC();
	float l = pad->GetLeftMargin();
	float t = pad->GetTopMargin();
	float r = pad->GetRightMargin();
	// float b = pad->GetBottomMargin();
	// cout << "Pad H = " << H << ", Pad W = " << W <<endl;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(1);

	pad->cd();
	// CMS
	latex.SetTextAlign(11);// (1.left, 2.center, 3.right) (1.bottom, 2.center, 3.top)
	latex.SetTextFont(cmsTextFont);
	latex.SetTextSize(cmsTextSize*t);
	latex.DrawLatex(l,1-t+cmsTextOffset*t,cmsText);

	// extraText
	latex.SetTextAlign(11);
	latex.SetTextFont(extraTextFont);
	latex.SetTextSize(cmsTextSize * extraOverCmsTextSize*t);
	latex.DrawLatex(l+(cmsTextSize*t*2.2*H/W), 1-t+cmsTextOffset*t,extraText);

	// lumiText
  latex.SetTextAlign(31); // (1.left, 2.center, 3.right) (1.bottom, 2.center, 3.top)
  latex.SetTextFont(lumiTextFont);
	latex.SetTextSize(lumiTextSize*t);
	latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);
}

// tdrGrid: Turns the grid lines on (true) or off (false)

void tdrGrid(bool gridOn) {
  gStyle->SetPadGridX(gridOn);
  gStyle->SetPadGridY(gridOn);
}

// fixOverlay: Redraws the axis

void fixOverlay() {
  gPad->RedrawAxis();
}

void setTDRStyle() {
  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

// For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
  tdrStyle->SetCanvasDefY(0);

// For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

// For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);

// For the histo:
  // tdrStyle->SetHistFillColor(1);
  // tdrStyle->SetHistFillStyle(0);
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  // tdrStyle->SetLegoInnerR(Float_t rad = 0.5);
  // tdrStyle->SetNumberContours(Int_t number = 20);

  tdrStyle->SetEndErrorSize(2);
  // tdrStyle->SetErrorMarker(20);
  //tdrStyle->SetErrorX(0.);

  tdrStyle->SetMarkerStyle(20);

//For the fit/function:
  tdrStyle->SetOptFit(1);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);

//For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);

// For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  // tdrStyle->SetStatStyle(Style_t style = 1001);
  // tdrStyle->SetStatX(Float_t x = 0);
  // tdrStyle->SetStatY(Float_t y = 0);

// Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.02);

// For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);

// For the axis titles:

  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  // tdrStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // tdrStyle->SetTitleYSize(Float_t size = 0.02);
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

// For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.05, "XYZ");

// For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

// Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

// Postscript options:
  tdrStyle->SetPaperSize(20.,20.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->SetHatchesLineWidth(5);
  tdrStyle->SetHatchesSpacing(0.05);

  tdrStyle->cd();

}

#endif
