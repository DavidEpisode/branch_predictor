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
uint32_t* branch_history_buffer;
uint32_t branch_history_register;

// custom

int chooser_size;
int g_buffer_size;
int l_buffer_size;
int pc_buffer_size;

uint32_t *chooser_buffer;
uint32_t *g_buffer;
uint32_t *l_buffer;
uint32_t *pc_buffer;



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

void one_bit_buffer_update(uint32_t pc, uint8_t outcome)
{
    int buffer_counter = pc % buffer_size;
    switch (branch_history_buffer[buffer_counter])
    {
        case NOTTAKEN:
            branch_history_buffer[buffer_counter] = outcome ? TAKEN : NOTTAKEN;
            break;
        case TAKEN:
            branch_history_buffer[buffer_counter] = outcome ? TAKEN : NOTTAKEN;
            break;
        default:
            break;
    }
}

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
// gshare
void gshare_init(){
    buffer_size = (int)pow(2, ghistoryBits);
    branch_history_buffer = (uint32_t *) malloc(buffer_size*sizeof(uint32_t));
    branch_history_register &= (0 << ghistoryBits);
    for(int i = 0; i < buffer_size; ++i)
    {
        branch_history_buffer[i] = WN;
    }
}

uint8_t gshare_predict(uint32_t pc)
{
    uint32_t buffer_counter = pc ^ branch_history_register;
    uint32_t prediction = branch_history_buffer[buffer_counter % buffer_size];
    if(prediction < 2)
        return NOTTAKEN;
    else
        return TAKEN;
}

void gshare_train(uint32_t pc, uint8_t outcome)
{
    uint32_t buffer_counter = pc ^ branch_history_register;
    two_bit_buffer_update(buffer_counter, outcome, branch_history_buffer, buffer_size);

    branch_history_register = branch_history_register<<1 | outcome;
}
// tournament
void tournament_init(){

}

void tournament_train(uint32_t pc, uint8_t outcome)
{

}
// custom
void custom_init(){
    // combination of gshare and simple BHT
    ghistoryBits = 10;
    lhistoryBits = 10;
    pcIndexBits = 10;
    branch_history_register = 0;

    chooser_size = (int)pow(2, ghistoryBits);
    g_buffer_size = (int)pow(2, ghistoryBits);
    l_buffer_size = (int)pow(2, lhistoryBits);
    pc_buffer_size = (int)pow(2, pcIndexBits);

    chooser_buffer = (uint32_t*) malloc(chooser_size*sizeof(uint32_t));
    g_buffer = (uint32_t*) malloc(g_buffer_size*sizeof(uint32_t));
    l_buffer = (uint32_t*) malloc(l_buffer_size*sizeof(uint32_t));
    pc_buffer = (uint32_t*) malloc(pc_buffer_size*sizeof(uint32_t));

    for (int i = 0; i < chooser_size; ++i)
        chooser_buffer[i] = 2;

    for (int i = 0; i < g_buffer_size; ++i)
        g_buffer[i] = WN;

    for (int i = 0; i < l_buffer_size; ++i)
        l_buffer[i] = WN;

    for (int i = 0; i < pc_buffer_size; ++i)
        pc_buffer[i] = 0;
}

uint32_t custom_predict(uint32_t pc){
    uint32_t g_buffer_counter = pc ^ branch_history_register;
    uint32_t choice = chooser_buffer[g_buffer_counter % chooser_size];
    uint32_t prediction;
    // choose global
    if(choice > 1) {
        prediction = g_buffer[g_buffer_counter % g_buffer_size];
    }
    else{
        int idx = pc_buffer[pc % pc_buffer_size];
        prediction = l_buffer[idx % l_buffer_size];
    }
    if(prediction > 1)
        return TAKEN;
    else
        return NOTTAKEN;
}

void custom_train(uint32_t pc, uint8_t outcome)
{

    uint32_t g_buffer_counter = pc ^ branch_history_register;
    uint32_t p1 = g_buffer[g_buffer_counter % g_buffer_size];

    uint32_t l_buffer_counter = pc_buffer[pc % pc_buffer_size];
    uint32_t p2 = l_buffer[l_buffer_counter % l_buffer_size];

    int p1c = ((p1/2) == outcome);
    int p2c = ((p2/2) == outcome);
    int action = p1c - p2c;

    uint32_t choice = chooser_buffer[g_buffer_counter % chooser_size];
    switch(action){
        case -1:
            if(choice > 0)
                chooser_buffer[g_buffer_counter % chooser_size]--;
            break;
        case 1:
            if(choice < 3)
                chooser_buffer[g_buffer_counter % chooser_size]++;
            break;
        case 0:
            break;
        default:
            break;
    }
    two_bit_buffer_update(g_buffer_counter, outcome, g_buffer, g_buffer_size);
    two_bit_buffer_update(l_buffer_counter, outcome, l_buffer, l_buffer_size);

    branch_history_register = (branch_history_register<<1 | outcome);
    pc_buffer[pc % pcIndexBits] = (l_buffer_counter<< 1 | outcome);

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
            return custom_predict(pc);
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
}
