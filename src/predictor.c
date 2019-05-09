//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
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
int buffer_size;
uint8_t* branch_history_buffer;
uint32_t branch_history_register;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// two bit saturation counter update
void two_bit_buffer_update(uint32_t pc, uint8_t outcome)
{
  int buffer_counter = pc % buffer_size;
  switch (branch_history_buffer[buffer_counter])
  {
  case SN:
    branch_history_buffer[buffer_counter] = outcome ? WN : SN;
    break;
  case WN:
    branch_history_buffer[buffer_counter] = outcome ? WT : SN;
  case WT:
    branch_history_buffer[buffer_counter] = outcome ? ST : WN;
  case ST:
    branch_history_buffer[buffer_counter] = outcome ? ST : WT;
  default:
    break;
  }
}
// gshare 
void 
gshare_init(){
  buffer_size = pow(2, ghistoryBits);
  branch_history_buffer = (uint8_t*) malloc(buffer_size*sizeof(uint8_t));
  branch_history_register &= 0 << ghistoryBits;
  for(int i = 0; i < buffer_size; ++i)
  {
    branch_history_buffer[i] = SN;
  }
}

uint8_t gshare_predict(uint32_t pc)
{
  uint32_t buffer_counter = pc ^ branch_history_register;
  return branch_history_buffer[buffer_counter % buffer_size]/2;
}

void gshare_train(uint32_t pc, uint8_t outcome)
{
  uint32_t buffer_counter = pc ^ branch_history_register;
  two_bit_buffer_update(buffer_counter, outcome);
  branch_history_register <<= 1;
  branch_history_register |= outcome;
}
// tournament
void
tournament_init(){
  return;
}

void
tournament_train(uint32_t pc, uint8_t outcome)
{

}
// custom
void
custom_init(){
  int bit_number = 2;
  buffer_size = 4;
  branch_history_buffer = (uint8_t*)malloc(buffer_size*sizeof(uint8_t));
  printf("%d", buffer_size);
  for(int i = 0; i < buffer_size; ++i)
  {
    branch_history_buffer[i] = SN;
  }
}

void
custom_train(uint32_t pc, uint8_t outcome)
{
  int buffer_counter = pc % buffer_size;
  switch (branch_history_buffer[buffer_counter])
  {
  case SN:
    branch_history_buffer[buffer_counter] = outcome ? WN : SN;
    break;
  case WN:
    branch_history_buffer[buffer_counter] = outcome ? WT : SN;
  case WT:
    branch_history_buffer[buffer_counter] = outcome ? ST : WN;
  case ST:
    branch_history_buffer[buffer_counter] = outcome ? ST : WT;
  default:
    break;
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
      return gshare_predict(pc);
    case TOURNAMENT:
      break;
    case CUSTOM:
      return branch_history_buffer[pc % buffer_size]/2;
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
  switch (bpType){
    case STATIC:
      return;
    case GSHARE:
      gshare_train(pc, outcome);
      return;
    case TOURNAMENT:
      tournament_train(pc, outcome);
      return;
    case CUSTOM:
      custom_train(pc, outcome);
      return;
    default:
      break;
  }
  return;
}
