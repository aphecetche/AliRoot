#ifndef ALITRDARRAYDICTIONARY_H
#define ALITRDARRAYDICTIONARY_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. * 
 * See cxx source for full Copyright notice */ 

/* $Id: AliTRDarrayDictionary.h 23387 2008-01-17 17:25:16Z cblume $ */

///////////////////////////////////////////////////
//                                               //
// Container Class for Dictionary Info           //
//                                               //
///////////////////////////////////////////////////

#include <TObject.h>

class AliTRDarrayDictionary: public TObject
{

 public:

  AliTRDarrayDictionary();
  AliTRDarrayDictionary(Int_t nrow, Int_t ncol, Int_t ntime);
  AliTRDarrayDictionary(const AliTRDarrayDictionary &a);
  ~AliTRDarrayDictionary();
  AliTRDarrayDictionary &operator=(const AliTRDarrayDictionary &a);

  void  Allocate(Int_t nrow, Int_t ncol, Int_t ntime);
  void  SetNdet(Int_t ndet) {fNdet=ndet;};  
  Int_t GetNdet()  const {return fNdet;};
  void  SetDataByAdcCol(Int_t nrow, Int_t ncol, Int_t ntime, Int_t value)
                       {fDictionary[(nrow*fNumberOfChannels+ncol)*fNtime+ntime]=value;};
  Int_t GetDataByAdcCol(Int_t nrow, Int_t ncol, Int_t ntime) const
               {return fDictionary[(nrow*fNumberOfChannels+ncol)*fNtime+ntime];};
  Int_t GetDim() const {return fNDdim;};
  void  Compress();
  void  Expand();
  void  Reset();
  Int_t GetData(Int_t nrow, Int_t ncol, Int_t ntime) const;
  void  SetData(Int_t nrow, Int_t ncol, Int_t ntime, Int_t value);
  static  void    CreateLut();
  Bool_t WasExpandCalled() const {return fFlag;};

 protected:

  Int_t   fNdet;        //ID number of the chamber
  Int_t   fNrow;        //Number of rows
  Int_t   fNcol;        //Number of columns
  Int_t   fNumberOfChannels;  //  Number of MCM channels per row
  Int_t   fNtime;       //Number of time bins
  Int_t   fNDdim;       //Dimension of the Dictionary array
  Int_t*  fDictionary;  //[fNDdim]  //Pointer to integers array
  Bool_t  fFlag;        //! Has Expand() being called before?
  static Short_t *fgLutPadNumbering;   //  [fNcol] Look Up Table

  ClassDef(AliTRDarrayDictionary,7) //Dictionary container class
    
};

// inline definitions

//________________________________________________________________________________
inline Int_t AliTRDarrayDictionary::GetData(Int_t nrow, Int_t ncol, Int_t ntime) const
{
  //
  // Get the data using the pad numbering.
  // To access data using the mcm scheme use instead
  // the method GetDataByAdcCol
  //

  Int_t corrcolumn = fgLutPadNumbering[ncol];

  return fDictionary[(nrow*fNumberOfChannels+corrcolumn)*fNtime+ntime];

}
//________________________________________________________________________________
inline void AliTRDarrayDictionary::SetData(Int_t nrow, Int_t ncol, Int_t ntime, Int_t value)
{
  //
  // Set the data using the pad numbering.
  // To write data using the mcm scheme use instead
  // the method SetDataByAdcCol
  //

  Int_t colnumb = fgLutPadNumbering[ncol];

  fDictionary[(nrow*fNumberOfChannels+colnumb)*fNtime+ntime]=value;

}

#endif
