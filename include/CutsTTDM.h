#if !defined(MYLIB_CUTSTTDM_H)
#define MYLIB_CUTSTTDM_H 1

#include "TString.h"


enum {
  TTDM_00_Has2Leptons,
  TTDM_01_Routin,
  TTDM_02_Preselection,
  TTDM_03_AN16105,
  ncut  // This line should be always last
};

const TString scut[ncut] = {
  "TTDM/00_Has2Leptons",
  "TTDM/01_Routin",
  "TTDM/02_Preselection",
  "TTDM/03_AN16105"
};

#endif
