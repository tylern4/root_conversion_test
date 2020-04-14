/************************************************************************/
/*  Created by Nick Tyler*/
/*	University Of South Carolina*/
/************************************************************************/

#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD
#include "TChain.h"
#include "TF1.h"
#include "colors.hpp"
#include "reader.h"
#include <TFile.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <fstream>
#include <iostream>
#include <vector>

void datahandeler(std::string fin, std::string fout) {
  TFile* out = new TFile(fout.c_str(), "RECREATE");

  // Load chain from branch h10
  auto reader = std::make_shared<hipo::reader>();
  reader->open(fin.c_str());
  auto dict = std::make_shared<hipo::dictionary>();
  reader->readDictionary(*dict);
  auto hipo_event       = std::make_shared<hipo::event>();
  auto rec_Particle     = std::make_shared<hipo::bank>(dict->getSchema("REC::Particle"));
  auto rec_Scintillator = std::make_shared<hipo::bank>(dict->getSchema("REC::Scintillator"));

  //  Then make these histograms with this exact binning
  TH1F*  h1         = new TH1F("h1", "Hipo PIDs", 8000, -4000, 4000);
  TH1F*  h2         = new TH1F("h2", "Electron Px", 500, -5, 5);
  TH1F*  h3         = new TH1F("h3", "FTOF 1B Energy", 250, 0, 50);
  size_t num_events = 0;
  while (reader->next()) {

    reader->read(*hipo_event);
    hipo_event->getStructure(*rec_Particle);
    hipo_event->getStructure(*rec_Scintillator);

    if (rec_Particle->getRows() > 0) { // cut # 1  (gpart > 0).
      num_events++;

      // Then to fill
      // If particle id at 0 is 11 fill in the histogram h2 with px.
      if (rec_Particle->getInt("pid", 0) == 11) {
        h2->Fill(rec_Particle->getFloat("px", 0));
      }

      // Then looping over particles 1 to the number of particles :
      for (int part = 1; part < rec_Particle->getRows(); part++) {
        // Fill the PID of every particle into h1
        h1->Fill(rec_Particle->getInt("pid", part));

        // Fill the energy component of FTOF 1b into h3.
        for (int k = 0; k < rec_Scintillator->getRows(); k++) {
          int pindex   = rec_Scintillator->getShort("pindex", k);
          int detector = rec_Scintillator->getInt("detector", k);
          int layer    = rec_Scintillator->getInt("layer", k);
          if (pindex == part && detector == 12 && layer == 2)
            h3->Fill(rec_Scintillator->getFloat("energy", k));
        }
      }
    }
  }
  out->cd();

  TDirectory* output_hist = out->mkdir("hipo_hists");
  output_hist->cd();
  h1->Write();
  h2->Write();
  h3->Write();
  out->Close();
}
#endif
