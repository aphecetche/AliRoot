/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Svein Lindal <slindal@fys.uio.no   >                  *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/// @file   AliHLTEvePhos.cxx
/// @author Svein Lindal <slindal@fys.uio.no>
/// @brief  TPC processor for the HLT EVE display

#include "AliHLTEveTRD.h"
#include "AliHLTHOMERBlockDesc.h"
#include "TCanvas.h"
#include "AliHLTEveBase.h"
#include "AliEveHOMERManager.h"
#include "TEveManager.h"
#include "TEvePointSet.h"
#include "TColor.h"
#include "TMath.h"
#include "TH1F.h"
#include "AliHLTTRDCluster.h"
#include "AliTRDcluster.h"

ClassImp(AliHLTEveTRD)

AliHLTEveTRD::AliHLTEveTRD() : 
  AliHLTEveBase(), 
  fEveClusters(NULL),
  fEveColClusters(NULL),
  fNColorBins(15)
{
  // Constructor.

}

AliHLTEveTRD::~AliHLTEveTRD()
{
  //Destructor, not implemented
  if(fEveColClusters)
    delete fEveColClusters;
  fEveColClusters = NULL;
  
  if(fEveClusters)
    delete fEveClusters;
  fEveClusters = NULL;
}


void AliHLTEveTRD::ProcessBlock(AliHLTHOMERBlockDesc * block) {
  //See header file for documentation

  if ( ! block->GetDataType().CompareTo("CLUSTERS") ) {
    
    if(!fEveColClusters){
      fEveColClusters = CreatePointSetArray();
      fEventManager->GetEveManager()->AddElement(fEveColClusters);
    } 
    
    ProcessClusters(block, fEveClusters, fEveColClusters);
   
  } else if ( ! block->GetDataType().CompareTo("ROOTHIST") ) {
  
    if(!fCanvas) fCanvas = CreateCanvas("TRD QA", "TRD QA");
    fCanvas->Divide(3, 2);
    AddHistogramsToCanvas(block, fCanvas, fHistoCount);
		   
  }
  
}


void AliHLTEveTRD::AddHistogramsToCanvas(AliHLTHOMERBlockDesc* block, TCanvas * canvas, Int_t &cdCount ) {
  //See header file for documentation
  if ( ! block->GetClassName().CompareTo("TH1F")) {
    TH1F* histo = reinterpret_cast<TH1F*>(block->GetTObject());
    ++cdCount;
  
    TVirtualPad* pad = canvas->cd(cdCount);
    histo->Draw();
    pad->SetGridy();
    pad->SetGridx();

    if ( ! strcmp(histo->GetName(), "nscls") ) {
      histo->GetXaxis()->SetRangeUser(0.,15.);
    }

    if ( ! strcmp(histo->GetName(),"sclsdist") ||
	 ! strcmp(histo->GetName(),"evSize") )
      pad->SetLogy();
  }

}



TEvePointSet * AliHLTEveTRD::CreatePointSet() {
  //See header file for documentation
  TEvePointSet * ps = new TEvePointSet("TRD Clusters");
  ps->SetMainColor(kBlue);
  ps->SetMarkerStyle((Style_t)kFullDotSmall);
 
  return ps;

}

TEvePointSetArray * AliHLTEveTRD::CreatePointSetArray(){
  //See header file for documentation
  TEvePointSetArray * cc = new TEvePointSetArray("TRD Clusters Colorized");
  cc->SetMainColor(kRed);
  cc->SetMarkerStyle(4); // antialiased circle
  cc->SetMarkerSize(0.4);
  cc->InitBins("Cluster Charge", fNColorBins, 0., fNColorBins*100.);
  
  const Int_t nCol = TColor::GetNumberOfColors();
  for (Int_t ii = 0; ii < fNColorBins + 1; ++ii) {
    cc->GetBin(ii)->SetMainColor(TColor::GetColorPalette(ii * nCol / (fNColorBins+2)));
  }

  return cc;
     
}


void AliHLTEveTRD::UpdateElements() {
  //See header file for documentation
  if(fCanvas) fCanvas->Update();
  if(fEveClusters) fEveClusters->ResetBBox();

  //   for (Int_t ib = 0; ib <= fNColorBins + 1; ++ib) {
  //     fEveColClusters->GetBin(ib)->ResetBBox();
  //   }

}

void AliHLTEveTRD::ResetElements(){
  //See header file for documentation
  if(fEveClusters) fEveClusters->Reset();
 
  if(fEveColClusters){
    for (Int_t ib = 0; ib <= fNColorBins + 1; ++ib) {
      fEveColClusters->GetBin(ib)->Reset();
    }
  }

}

Int_t AliHLTEveTRD::ProcessClusters( AliHLTHOMERBlockDesc * block, TEvePointSet * cont, TEvePointSetArray * contCol ){
  //See header file for documentation
  
  Int_t iResult = 0;

  Int_t sm = block->GetSubDetector();
  if ( sm == 6 ) sm = 7;
  
  Float_t phi   = ( sm + 0.5 ) * TMath::Pi() / 9.0;  
  Float_t cos   = TMath::Cos( phi );
  Float_t sin   = TMath::Sin( phi );
  
  Byte_t* ptrData = reinterpret_cast<Byte_t*>(block->GetData());
  UInt_t ptrSize = block->GetSize();

  for (UInt_t size = 0; size+sizeof(AliHLTTRDCluster) <= ptrSize; size+=sizeof(AliHLTTRDCluster) ) {
    AliHLTTRDCluster *cluster = reinterpret_cast<AliHLTTRDCluster*>(&(ptrData[size]));
   
    AliTRDcluster *trdCluster = new AliTRDcluster;
    cluster->ExportTRDCluster( trdCluster );
   
    contCol->Fill(cos*trdCluster->GetX() - sin*trdCluster->GetY(), 
		   sin*trdCluster->GetX() + cos*trdCluster->GetY(), 
		   trdCluster->GetZ(),
		   trdCluster->GetQ() );    
     
    cont->SetNextPoint(cos*trdCluster->GetX() - sin*trdCluster->GetY(), 
    		       sin*trdCluster->GetX() + cos*trdCluster->GetY(), trdCluster->GetZ());
  }
  
  return iResult;





 
  return 0;  


}


