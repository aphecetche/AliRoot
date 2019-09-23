/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/// \ingroup macros
/// \file runSimulation.C
/// \brief Macro for running simulation
///
/// Macro extracted from the MUON test script
///
/// \author Laurent Aphecetche

#include "AliCDBManager.h"
#include "AliHeader.h"
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliSimulation.h"
#include <Riostream.h>
#include <TRandom.h>

void runSimulation(int seed, int nevents, const char *config,
                   const char *embedwith, int runnumber) {
  // Uncoment following lines to run simulation with local residual
  // mis-alignment (generated via MUONGenerateGeometryData.C macro)
  // AliCDBManager* man = AliCDBManager::Instance();
  // man->SetDefaultStorage("local://$ALIROOT_OCDB_ROOT/OCDB");
  // man->SetSpecificStorage("MUON/Align/Data","local://$ALIROOT_OCDB_ROOT/OCDB/MUON/ResMisAlignCDB");

  AliSimulation MuonSim(config);

  if (strlen(embedwith) > 0) {
    // setups specific to embedding

    gAlice->SetConfigFunction(
        "Config(\"\", \"param\", \"AliMUONDigitStoreV2S\",kTRUE);");

    // get the run number from real data

    AliRunLoader *runLoader = AliRunLoader::Open(embedwith, "titi");
    if (runLoader == 0x0) {
      std::cerr << Form("Cannot open file %s", embedwith) << std::endl;
      return;
    }

    runLoader->LoadHeader();

    if (!runLoader->GetHeader()) {
      std::cerr << "Cannot load header." << std::endl;
      return;
    } else {
      Int_t runNumber = runLoader->GetHeader()->GetRun();
      MuonSim.SetRunNumber(runNumber);
      std::cout << Form("***** RUN NUMBER SET TO %09d ACCORDING TO %s ",
                        runNumber, embedwith)
                << std::endl;
    }
    runLoader->UnloadHeader();
    delete runLoader;

    std::cout << "***** EMBEDDING MODE : USING RAW OCDB" << std::endl;
    AliCDBManager::Instance()->SetDefaultStorage("raw://");
    AliCDBManager::Instance()->SetSpecificStorage(
        "local://$ALIROOT_OCDB_ROOT/OCDB", "MUON/Align/Data");

  } else if (runnumber > 0) {
    // simulation with anchor run

    std::cout << "***** ANCHOR RUN MODE : USING RAW OCDB AS MUCH AS POSSIBLE"
              << std::endl;
    std::cout
        << "*****                   AND TAKING VERTEX FROM OCDB IF AVAILABLE"
        << std::endl;

    // Last parameter of Config.C indicates we're doing realistic simulations,
    // so we NEED the ITS in the geometry
    gAlice->SetConfigFunction(
        "Config(\"\", \"param\", \"AliMUONDigitStoreV2S\",kFALSE,kTRUE);");

    AliCDBManager::Instance()->SetDefaultStorage("raw://");
    // use something like :
    // "alien://folder=/alice/data/2011/OCDB?cacheFold=/Users/laurent/OCDBcache"
    // instead of "raw://" if getting slow/problematic accesses to OCDB...

    AliCDBManager::Instance()->SetSpecificStorage(
        "MUON/Align/Data",
        "alien://folder=/alice/cern.ch/user/j/jcastill/LHC10hMisAlignCDB");

    MuonSim.SetRunNumber(runnumber);

    MuonSim.UseVertexFromCDB();
  } else {
    std::cout << "HERE !!!" << std::endl;
    AliCDBManager::Instance()->SetDefaultStorage(
        "local://$ALIROOT_OCDB_ROOT/OCDB");
    gAlice->SetConfigFunction(
        "Config(\"\", \"param\", \"AliMUONDigitStoreV2S\",kFALSE);");
  }

  MuonSim.SetSeed(seed);
  MuonSim.SetTriggerConfig("MUON");
  MuonSim.SetWriteRawData("MUON ", "raw.root", kTRUE);

  MuonSim.SetMakeSDigits("MUON");
  MuonSim.SetMakeDigits(
      "MUON ITS"); // ITS needed to propagate the simulated vertex
  MuonSim.SetMakeDigitsFromHits(
      "ITS"); // ITS needed to propagate the simulated vertex

  MuonSim.SetRunHLT("libAliHLTMUON.so chains=dHLT-sim");

  MuonSim.SetRunQA("MUON:ALL");

  if (strlen(embedwith) > 0) {
    MuonSim.MergeWith(embedwith);
  }

  MuonSim.Run(nevents);
  // gObjectTable->Print();
}
