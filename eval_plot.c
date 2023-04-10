#include <cstdlib>
#include <cmath>
double eval(double cut, int ch)
{
    double v,s=0,b=0,es,eb;
    // s: no. of signal pts obtained after cut
    // b: no. of background pts obtained after cut 

    TH1F *bgn = new TH1F("Histogram","Background",50,0,1);
    // TCanvas *c1 = new TCanvas("Canvas","Output of NN");
   
    fstream file;

    file.open("bg_w_evaluated_train.txt",ios::in);

    while(1)
    {
        file>>v;
        if(file.eof()) break;
        if (v>cut) b++;     
        bgn->Fill(v);
    }
    file.close();
    // bgn->SetLineColor(kBlue);
    // bgn->SetFillStyle(3008);   bgn->SetFillColor(kBlue);
    // bgn->Draw();

    TH1F *sign = new TH1F("Histogram","Signal",50,0,1);
    file.open("sig_w_evaluated_train.txt",ios::in);
    while(1)
    {
        file>>v;
        if(file.eof()) break;
        if (v>cut) s++;
        sign->Fill(v);
        
    }
    file.close();

    // sign->SetLineColor(kRed);
    // sign->SetFillStyle(3003); sign->SetFillColor(kRed);
    // sign->Draw("same");

    // TLegend *legend = new TLegend();
    // legend->AddEntry(bgn, "Background");
    // legend->AddEntry(sign, "Signal");
    // legend->Draw();

    // Analysis
    double sno = sign->GetEntries(); // The total no. of signal points from the file
    double bno = bgn->GetEntries(); // The total no. of background points from the file
    double pts = sno + bno;         // The total no. of points
    double efficiency = s*100/sno;
    double purity = s*100/(s+b);

    // cout<<"\nNo. of bg points is "<<bno<<"\n";
    // cout<<"No. of sig points is "<<sno<<"\n";
    // cout<<"Total no. of points is "<<pts<<"\n";
    // cout<<"\nCut placed at "<<cut<<"\n";
    // cout<<"\nNo. of bg points above cut is "<<b<<"\n";
    // cout<<"No. of sig points above cut is "<<s<<"\n";
    // cout<<"\nEfficiency: "<<efficiency<<"% \n";
    // cout<<"Purity: "<<purity<<"% \n";

    if (ch == 1) return efficiency;
    else return purity;
}

void eval_plot(double cut)
{
    int n = 20,j=0;
    double x, y1, y2, best_cut = 0.0;

    TGraph *ge = new TGraph();
    ge->SetMarkerStyle(kStar);
    ge->SetMarkerSize(0.5);
    ge->SetLineColor(kBlue);
    ge->SetTitle("Cut Performance");

    TGraph *gp = new TGraph();
    gp->SetMarkerStyle(kStar);
    gp->SetMarkerSize(0.5);
    gp->SetLineColor(kGreen);
    
    fstream file;
    file.open("cut_performance_data.txt",ios::out);

    TStopwatch t;
    for (double i=cut-0.01;i<cut+0.01;i+=0.001)
    {
        
        x = i;
        y1 = eval(i,1);
        y2 = eval(i,0);
        if (abs(y1-y2) < 0.01)
        {
            best_cut = x;
        }
        // if (fmod(x*100,1) == 0.0)
        // {
            ge->SetPoint(ge->GetN(),x,y1);
            gp->SetPoint(gp->GetN(),x,y2);
        // }
        
         file<<x<<" "<<y1<<" "<<y2<<"\n";
    }
     file.close();
    t.Print();

    cout<<"Best cut: "<<best_cut<<"\n";

    TCanvas *cut_perf = new TCanvas();

    ge->Draw();
    gp->Draw("Same");

    TLegend *legend = new TLegend(.75, .80, .95, .95);
      legend->AddEntry(ge, "Efficiency");
      legend->AddEntry(gp, "Purity");
      legend->Draw();

}
