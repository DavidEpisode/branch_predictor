//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Xinming Zhang";
const char *studentID   = "A53283531";
const char *email       = "xiz015@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
const int bhb_size = 1024;
int bhb[bhb_size];

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// gshare 
void 
gshare_init(){

}
// tournament
void
tournament_init(){

}
// custom
void
custom_init(){
  for(int i = 0; i < bhb_size; ++i)
  {
    bhb[i] = SN;
  }
}

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType){
    case STATIC:
      return;
    case GSHARE:
      gshare_init();
      return;
    case TOURNAMENT:
      tournament_init();
      return;
    case CUSTOM: 
      custom_init();
      return;
    default:
      break;
  };

  return;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
      return bhb[pc % bhb_size]/2;
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  int index = pc % bhb_size;
  switch (bhb[index])
  {
  case SN:
    bhb[index] = outcome ? WN : SN;
    break;
  case WN:
    bhb[index] = outcome ? WT : SN;
  case WT:
    bhb[index] = outcome ? ST : WN;
  case ST:
    bhb[index] = outcome ? ST : WT;
  default:
    break;
  }
  return;
}
