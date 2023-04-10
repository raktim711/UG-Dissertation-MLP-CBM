#include <cstdlib>
// the best cut is currently 0.4886 

void hist_eval(double cut)
{
    double v,s=0,b=0;
    // s: no. of signal pts obtained after cut
    // b: no. of background pts obtained after cut 

    TH1F *bgn = new TH1F("Histogram"," ",50,0.496,0.505);
    // bgn->SetMaximum(7000);
    TCanvas *c1 = new TCanvas("Canvas","Output of NN");
   
    fstream file;

    file.open("bg_w_evaluated_test.txt",ios::in);

    while(1)
    {
        file>>v;
        if(file.eof()) break;
        if (v>cut) b++;     
        bgn->Fill(v);
    }
    file.close();
    bgn->SetLineColor(kBlue);
    bgn->SetFillStyle(3008);   bgn->SetFillColor(kBlue);
    bgn->Draw();

    TH1F *sign = new TH1F("Histogram"," ",50,0.496,0.505);
    // sign->SetMaximum(7000);
    file.open("sig_w_evaluated_test.txt",ios::in);
    while(1)
    {
        file>>v;
        if(file.eof()) break;
        if (v>cut) s++;
        sign->Fill(v);
        
    }
    file.close();

    sign->SetLineColor(kRed);
    sign->SetFillStyle(3003); sign->SetFillColor(kRed);
    sign->Draw("same");

    TLegend *legend = new TLegend();
    legend->AddEntry(bgn, "Background");
    legend->AddEntry(sign, "Signal");
    legend->Draw();

    // Analysis
    double sno = sign->GetEntries(); // The total no. of signal points from the file
    double bno = bgn->GetEntries(); // The total no. of background points from the file
    double pts = sno + bno;         // The total no. of points
    double efficiency = s*100/sno;
    double purity = s/(s+b);

    cout<<"\nNo. of bg points is "<<bno<<"\n";
    cout<<"No. of sig points is "<<sno<<"\n";
    cout<<"Total no. of points is "<<pts<<"\n";
    cout<<"\nCut placed at "<<cut<<"\n";
    cout<<"\nNo. of bg points above cut is "<<b<<"\n";
    cout<<"No. of sig points above cut is "<<s<<"\n";
    cout<<"\nEfficiency: "<<efficiency<<"% \n";
    cout<<"Purity: "<<purity*100<<"% \n";

}


