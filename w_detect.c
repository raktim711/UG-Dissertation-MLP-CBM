void w_detect() 
{
   // Loading the data file and trees
       TFile *input = 0;
       input = TFile::Open("w_30k.root","read");
       
       Int_t ntrain=51;
    
       TTree *sig_filtered = (TTree *) input->Get("omega"); 
       TTree *bg_filtered = (TTree *) input->Get("centr");
       TTree *simu = new TTree("MonteCarlo", "Filtered Monte Carlo Events");
      
       Double_t STS_Hits, MUCH_Hits, chi_STS, chi_MUCH;
       Int_t type;
       // Setting up the trees

       sig_filtered->SetBranchAddress("STS_Hits", &STS_Hits);
       sig_filtered->SetBranchAddress("MUCH_Hits", &MUCH_Hits);
       sig_filtered->SetBranchAddress("chi_STS", &chi_STS);
       sig_filtered->SetBranchAddress("chi_MUCH", &chi_MUCH);

       bg_filtered->SetBranchAddress("STS_Hits", &STS_Hits);
       bg_filtered->SetBranchAddress("MUCH_Hits", &MUCH_Hits);
       bg_filtered->SetBranchAddress("chi_STS", &chi_STS);
       bg_filtered->SetBranchAddress("chi_MUCH", &chi_MUCH);
       
       simu->Branch("STS_Hits", &STS_Hits,"STS_Hits/D");
       simu->Branch("MUCH_Hits", &MUCH_Hits,"MUCH_Hits/D");
       simu->Branch("chi_STS", &chi_STS,"chi_STS/D");
       simu->Branch("chi_MUCH", &chi_MUCH,"chi_MUCH/D");
       simu->Branch("type",   &type,   "type/I");
       // filling up a tree with both signal and noise
       type = 1;
       Int_t i;
       for (i = 0; i < sig_filtered->GetEntries(); i++) {
          sig_filtered->GetEntry(i);         
          simu->Fill();
       }
       type = 0;
       for (i = 0; i < bg_filtered->GetEntries(); i++) {
          bg_filtered->GetEntry(i);
          simu->Fill();
       }

      // The ANN layout    
       TMultiLayerPerceptron *mlp = new TMultiLayerPerceptron("@STS_Hits,@MUCH_Hits,@chi_STS,@chi_MUCH:6:7:3:type",
       simu,"Entry$%2","(Entry$+1)%2"); 

       mlp->SetLearningMethod(TMultiLayerPerceptron::kBatch);
       TStopwatch t; 
       mlp->Train(ntrain, "text,graph,update=10,minErrorTrain");
       cout << "Training time -> ";
       t.Print();
       mlp->DumpWeights("mu_weights.txt"); // print the weights of the network into a file
       int count=1;

       cout<<"\nSignal:\n";
       while(count<10) // running the loop simply to get the parameter for 3 different entries
       {
         Double_t params[4];
        sig_filtered->GetEntry(count+2498);
          params[0] = STS_Hits;
          params[1] = MUCH_Hits;
          params[2] = chi_STS;
          params[3] = chi_MUCH;
          cout<< mlp->Evaluate(0, params)<<" "<<count<<"\n";
          //cout<<mlp->Result(sig_filtered->GetEntry(count+1478),0)<<" "<<count <<"\n\n";
          count++;
       }
       count=1;
       cout<<"\nNoise:\n";
       while(count<5) // running the loop simply to get the parameter for 3 different entries
       {
         Double_t params[4];
         bg_filtered->GetEntry(count+14768);
          params[0] = STS_Hits;
          params[1] = MUCH_Hits;
          params[2] = chi_STS;
          params[3] = chi_MUCH;
          cout<< mlp->Evaluate(0, params)<<" "<<count<<"\n";
          //cout<<mlp->Result(sig_filtered->GetEntry(count+1478),0)<<" "<<count <<"\n\n";
          count++;
       }
       TCanvas* c1 = new TCanvas("Canvas 1","Neural network");
       TMLPAnalyzer ana(mlp);
       // Initialisation
       ana.GatherInformations();
       // output to the console
       ana.CheckNetwork();       
       // shows how each variable influences the network
       // shows the network structure
       mlp->Draw();
      TCanvas* c2 = new TCanvas("Canvas 2","Input at 1st layer");
       // draws the resulting network
       ana.DrawNetwork(0,"type==1","type==0");
       //Writting the evaluated output into a file
       TH1F *bgn = new TH1F("bgh", "NN output", 50, 0.499,0.503); 
       TH1F *sign = new TH1F("sigh", "NN output", 50, 0.499,0.503);
       bgn->SetDirectory(0);
       sign->SetDirectory(0);
       Double_t params[4];
       fstream file;
       file.open("bg_w_evaluated.txt",ios::out);

       for (i = 0; i < bg_filtered->GetEntries(); i++) {
          bg_filtered->GetEntry(i);
          params[0] = STS_Hits;
          params[1] = MUCH_Hits;
          params[2] = chi_STS;
          params[3] = chi_MUCH;
          file<< mlp->Evaluate(0, params)<<"\n";          
          bgn->Fill(mlp->Evaluate(0, params));
         
       }
      
      file.close();

      file.open("sig_w_evaluated.txt",ios::out);
       for (i = 0; i < sig_filtered->GetEntries(); i++) {
          sig_filtered->GetEntry(i);
          params[0] = STS_Hits;
          params[1] = MUCH_Hits;
          params[2] = chi_STS;
          params[3] = chi_MUCH;
          file<< mlp->Evaluate(0, params)<<"\n";
          sign->Fill(mlp->Evaluate(0,params));
         
         }
      file.close();

      delete input;
   

   
}