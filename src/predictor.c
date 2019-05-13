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
// Gshare
uint32_t gHistoryRegister;
uint8_t* BHT;

// Tournament
int gHistoryMask;
int lHistoryMask;
int pcIndexMask;

uint32_t* gPredictionTable;
uint32_t* lHistoryTable;
uint32_t* lPredictionTable;
uint32_t* choicePredictionTable;

uint32_t gPredictionTableSize;
uint32_t lHistoryTableSize;
uint32_t lPredictionTableSize;
uint32_t choicePredictionTableSize;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//
// two bit saturation counter update
void two_bit_buffer_update(uint32_t pc, uint8_t outcome, uint32_t* branch_history_buffer, int buffer_size)
{
    int buffer_counter = pc % buffer_size;
    switch (branch_history_buffer[buffer_counter])
    {
        case SN:
            branch_history_buffer[buffer_counter] = outcome ? WN : SN;
            break;
        case WN:
            branch_history_buffer[buffer_counter] = outcome ? WT : SN;
            break;
        case WT:
            branch_history_buffer[buffer_counter] = outcome ? ST : WN;
            break;
        case ST:
            branch_history_buffer[buffer_counter] = outcome ? ST : WT;
            break;
        default:
            break;
    }
}

void choice_predictor_increment() {
    int index = gHistoryRegister % choicePredictionTableSize;
    if (choicePredictionTable[index] < 3) {
        choicePredictionTable[index] += 1;
    }
}

void choice_predictor_decrement() {
    int index = gHistoryRegister % choicePredictionTableSize;
    if (choicePredictionTable[index] > 0) {
        choicePredictionTable[index] -= 1;
    }
}

// tournament
int lowerBitMask(int bits) {
    return (1 << bits) - 1;
}

void tournament_init() {
    gHistoryRegister = 0;

    ghistoryBits = 9;
    lhistoryBits = 10;
    pcIndexBits  = 10;

    gHistoryMask = lowerBitMask(ghistoryBits);
    lHistoryMask = lowerBitMask(lhistoryBits);
    pcIndexMask  = lowerBitMask(pcIndexBits);

    // Global predictor
    gPredictionTableSize = pow(2, ghistoryBits);
    gPredictionTable = (uint32_t*) malloc(gPredictionTableSize * sizeof(uint32_t));
    for (int i = 0; i < gPredictionTableSize; i++) {
        gPredictionTable[i] = WN;
    }

    // Local
    lHistoryTableSize = pow(2, pcIndexBits);
    lHistoryTable = (uint32_t*) malloc(lHistoryTableSize * sizeof(uint32_t)); // level 1

    lPredictionTableSize = pow(2, lhistoryBits);
    lPredictionTable = (uint32_t*) malloc(lPredictionTableSize * sizeof(uint32_t)); // level 2
    for (int i = 0; i < gPredictionTableSize; i++) {
        lPredictionTable[i] = WN;
    }

    // Choice predictor
    choicePredictionTableSize = pow(2, ghistoryBits);
    choicePredictionTable = (uint32_t*) malloc(choicePredictionTableSize * sizeof(uint32_t));

    //printf("%d | %d | %d | %d\n", gPredictionTableSize, lHistoryTableSize, lPredictionTableSize, choicePredictionTableSize);
}

// Translate a two bit saturated counter to a prediction.
uint8_t predict(uint8_t twoBitCounter) {
    if (twoBitCounter < 2) {
        return NOTTAKEN;
    } else {
        return TAKEN;
    }
}

// Obtain the prediction(T/NT) by the global predictor.
uint8_t tournament_global_predict() {
    // int gIndex = gHistoryRegister & gHistoryMask;
    int gIndex = gHistoryRegister % gPredictionTableSize;
    return predict(gPredictionTable[gIndex]);
}

// Obtain the prediction(T/NT) by the local predictor.
uint8_t tournament_local_predict(uint32_t pc) {
    //uint32_t lv1_index = pc & pcIndexMask;
    //uint32_t lv2_index = lHistoryTable[lv1_index] & lHistoryMask;
    uint32_t lv1_index = pc % lHistoryTableSize;
    uint32_t lv2_index = lHistoryTable[lv1_index] % lPredictionTableSize;
    return predict(lPredictionTable[lv2_index]);
}
// Obtain the final prediction(T/NT) by the choice predictor.
uint8_t tournament_predict(uint32_t pc) {
    // int choiceIndex = gHistoryRegister & gHistoryMask;
    int choiceIndex = gHistoryRegister % choicePredictionTableSize;
    uint8_t choice = choicePredictionTable[choiceIndex];
    // global
    if (choice < 2) {
        return tournament_global_predict();
    } else { // local
        return tournament_local_predict(pc);
    }
}

void tournament_train(uint32_t pc, uint8_t outcome) {
    uint8_t gPrediction = tournament_global_predict(); // global prediction
    uint8_t lPrediction = tournament_local_predict(pc); // local prediction

    // Update choice predictor only if global & local predicts differently.
    if (gPrediction != lPrediction) {
        if (gPrediction == outcome) {
            // global(0) is correct, local(1) is incorrect
            choice_predictor_decrement();
        } else {
            // local is correct, global is incorrect
            choice_predictor_increment();
        }
    }

    // Update global predictor
    two_bit_buffer_update(gHistoryRegister, outcome, gPredictionTable, gPredictionTableSize);

    // Update local predictor
    // uint32_t lv1_index = pc & pcIndexMask;
    uint32_t lv1_index = pc % lHistoryTableSize;
    uint32_t lv2_index = lHistoryTable[lv1_index] % lPredictionTableSize;
    two_bit_buffer_update(lv2_index, outcome, lHistoryTable, lHistoryTableSize);

    // Update local history
    //uint32_t lhistory_old = lHistoryTable[lv1_index] & 7;
    lHistoryTable[lv1_index] = ((lHistoryTable[lv1_index] << 1) | outcome);
    //uint32_t lhistory_updated = lHistoryTable[lv1_index] & 7;

    // update global history
    //uint32_t ghistory_old = gHistoryRegister & 7;
    gHistoryRegister = ((gHistoryRegister << 1) | outcome);
    //uint32_t ghistory_updated = gHistoryRegister & 7;

    printf("");
}


// gshare


void gshare_init() {

}

uint8_t gshare_predict(uint32_t pc) {

}

void gshare_train(uint32_t pc, uint8_t outcome) {

}



// custom
void custom_init(){

}

uint32_t custom_predict(uint32_t pc){

}

void custom_train(uint32_t pc, uint8_t outcome)
{


}
// perceptron predictor
void perceptron_init(){

}

uint8_t perceptron_predict(uint32_t pc){


}

void perceptron_train(uint32_t pc, uint8_t outcome){

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
            perceptron_init();
            return;
        default:
            break;
    };
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
            return tournament_predict(pc);
        case CUSTOM:
            return perceptron_predict(pc);
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
            perceptron_train(pc, outcome);
            return;
        default:
            break;
    }
}

void cleanup_predictor() {
    switch (bpType) {
        case GSHARE:
            free(BHT);
            break;
        case CUSTOM:
            break;
        default:
            break;
    }
}