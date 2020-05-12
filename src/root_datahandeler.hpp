/************************************************************************/
/*  Created by Nick Tyler*/
/*	University Of South Carolina*/
/************************************************************************/

#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD
#include "TChain.h"
#include "TF1.h"
#include "colors.hpp"
#include "filehandeler.hpp"
#include <TFile.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <fstream>
#include <iostream>
#include <vector>

void datahandeler(std::string fin, std::string fout) {
  //  float energy = CLAS12_E;
  // if (getenv("CLAS12_E") != NULL)
  //  energy = atof(getenv("CLAS12_E"));

  TFile* out = new TFile(fout.c_str(), "RECREATE");

  // Load chain from branch h10
  TChain* chain = filehandeler::addFiles(fin);
  filehandeler::getBranches(chain);
  int num_of_events = (int)chain->GetEntries();

  //  Then make these histograms with this exact binning
  TH1F* h1 = new TH1F("h1", "Hipo PIDs", 8000, -4000, 4000);
  TH1F* h2 = new TH1F("h2", "Electron Px", 500, -5, 5);
  TH1F* h3 = new TH1F("h3", "FTOF 1B Energy", 250, 0, 50);
  for (int current_event = 0; current_event < num_of_events; current_event++) {
    chain->GetEntry(current_event);
    if (current_event % 1000)
      std::cerr << "  " << std::floor((100 * (double)current_event / (double)num_of_events))
                << "%\r\r" << std::flush;

    if (pid->size() > 0) { // cut # 1  (gpart > 0).

      // Then to fill
      // If particle id at 0 is 11 fill in the histogram h2 with px.

      if (pid->at(0) == 11 && !std::isnan(px->at(0)))
        h2->Fill(px->at(0));

      // Then looping over particles 1 to the number of particles :
      // Fill the PID of every particle into h1
      // Fill the energy component of FTOF 1b into h3.
      for (int part = 1; part < pid->size(); part++) {
        h1->Fill(pid->at(part));
        if (!std::isnan(sc_ftof_1b_energy->at(part)))
          h3->Fill(sc_ftof_1b_energy->at(part));
      }
    }
  }
  out->cd();

  TDirectory* output_hist = out->mkdir("output_hist");
  output_hist->cd();
  h1->Write();
  h2->Write();
  h3->Write();

  out->Close();
  chain->Reset();
}
#endif
