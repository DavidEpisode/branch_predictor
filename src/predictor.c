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
#include <string.h>

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

//perceptron
int perceptron_table_size;
int weight_size;
int *perceptron_table;
int theta;


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
void two_bit_buffer_update0(uint32_t index, uint8_t outcome, uint32_t* branch_history_buffer) {
    switch (branch_history_buffer[index])
    {
        case SN:
            branch_history_buffer[index] = outcome ? WN : SN;
            break;
        case WN:
            branch_history_buffer[index] = outcome ? WT : SN;
            break;
        case WT:
            branch_history_buffer[index] = outcome ? ST : WN;
            break;
        case ST:
            branch_history_buffer[index] = outcome ? ST : WT;
            break;
        default:
            break;
    }
}

void two_bit_buffer_update(uint32_t pc, uint8_t outcome, uint32_t* branch_history_buffer, int buffer_size)
{
    int buffer_counter = pc % buffer_size;
    two_bit_buffer_update0(buffer_counter, outcome, branch_history_buffer);
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
// info: global hisotory, local history or PC
int entryIndex(int info, int mask) {
    return info & mask;
}

int lHistoryTableIndex(int pc) {
    int mask  = (1 << pcIndexBits) - 1;
    return entryIndex(pc, mask);
}

int lPredictionTableIndex(int lHistory) {
    int mask = (1 << lhistoryBits) - 1;
    return entryIndex(lHistory, mask);
}

int gPredictionTableIndex(int gHistory) {
    int mask = (1 << ghistoryBits) - 1;
    return entryIndex(gHistory, mask);
}

int choicePredictionTableIndex(int gHistory) {
    int mask = (1 << ghistoryBits) - 1;
    return entryIndex(gHistory, mask);
}


// Translate a two bit saturated counter to a prediction.
uint8_t action(uint32_t twoBitCounter) {
    if (twoBitCounter < 2) {
        return NOTTAKEN;
    } else {
        return TAKEN;
    }
}

void tournament_init() {
    gHistoryRegister = 0;

    // Two level local predictions
    // local level 1
    lHistoryTableSize = pow(2, pcIndexBits);
    lHistoryTable = (uint32_t*) malloc(lHistoryTableSize * sizeof(uint32_t));

    // local level 2
    lPredictionTableSize = pow(2, lhistoryBits);
    lPredictionTable = (uint32_t*) malloc(lPredictionTableSize * sizeof(uint32_t));
    for (int i = 0; i < lPredictionTableSize; i++) {
        lPredictionTable[i] = WN;
    }

    // Global predictor
    gPredictionTableSize = pow(2, ghistoryBits);
    gPredictionTable = (uint32_t*) malloc(gPredictionTableSize * sizeof(uint32_t));
    for (int i = 0; i < gPredictionTableSize; i++) {
        gPredictionTable[i] = WN;
    }

    // Choice predictor
    choicePredictionTableSize = pow(2, ghistoryBits);
    choicePredictionTable = (uint32_t*) malloc(choicePredictionTableSize * sizeof(uint32_t));
    for (int j = 0; j < choicePredictionTableSize; ++j) {
        choicePredictionTable[j] = 2;
    }
}


// Obtain the prediction(T/NT) by the local predictor.
int tournament_local_predict(uint32_t pc) {
    uint32_t lHistory = lHistoryTable[lHistoryTableIndex(pc)];
    uint32_t lPredictCounter = lPredictionTable[lPredictionTableIndex(lHistory)];
    return lPredictCounter & 0b11; // 2-bit counter
}

// Obtain the prediction(T/NT) by the global predictor.
int tournament_global_predict() {
    int index = gPredictionTableIndex(gHistoryRegister);
    return gPredictionTable[index] & 0b11;
}


// Obtain the final prediction(T/NT) by the choice predictor.
uint8_t tournament_predict(uint32_t pc) {
    int index = choicePredictionTableIndex(gHistoryRegister);
    int choice = choicePredictionTable[index];
    int prediction;
    if (choice < 2) { // local
        prediction = tournament_local_predict(pc); // my
    } else { // global
        prediction = tournament_global_predict(); // my
    }
    return action(prediction);
}

void tournament_train(uint32_t pc, uint8_t outcome) {
    int gPrediction = action(tournament_global_predict());
    int lPrediction = action(tournament_local_predict(pc));
    // update choice prediction only when local & global makes different predictions
    if (gPrediction != lPrediction) {
        int index = choicePredictionTableIndex(gHistoryRegister);
        int choice = choicePredictionTable[index];
        if (gPrediction == outcome && choice < 3) { // global is correct
            choicePredictionTable[index] += 1;
        } else if (lPrediction == outcome && choice > 0){ // local is correct
            choicePredictionTable[index] -= 1;
        }
    }

    int l1_index = lHistoryTableIndex(pc);
    int lHistory = lHistoryTable[l1_index];
    int l2_index = lPredictionTableIndex(lHistory);

    // Update local predictor
    two_bit_buffer_update(l2_index, outcome, lPredictionTable, lPredictionTableSize);
//    two_bit_buffer_update(l2_index, outcome, lHistoryTable, lHistoryTableSize); // buggy

    // Update global predictor
    two_bit_buffer_update(gHistoryRegister, outcome, gPredictionTable, gPredictionTableSize);

    // Update local history
    lHistoryTable[l1_index] <<= 1;
    lHistoryTable[l1_index] |= outcome;

    // Update global history
    gHistoryRegister <<= 1;
    gHistoryRegister |= outcome; // update global history
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

uint8_t custom_predict(uint32_t pc){
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
    pc_buffer[pc % pc_buffer_size] = (l_buffer_counter<< 1 | outcome);

}
// perceptron predictor
void perceptron_init(){

    ghistoryBits = 9;
    lhistoryBits = 10;
    pcIndexBits = 10;
    theta = (int)(1.94*ghistoryBits+14);

    pc_buffer_size = (int)pow(2, lhistoryBits);
    l_buffer_size = (int)pow(2, lhistoryBits);
    g_buffer_size = (int)pow(2, ghistoryBits);

    perceptron_table_size = 22;
    weight_size = 8;

    branch_history_register = 0;
    g_buffer = (uint32_t*)malloc(g_buffer_size * sizeof(uint32_t));
    l_buffer = (uint32_t*) malloc(l_buffer_size*sizeof(uint32_t));
    pc_buffer = (uint32_t*) malloc(pc_buffer_size*sizeof(uint32_t));
//    chooser_buffer = (uint32_t*)malloc(chooser_size * sizeof(uint32_t));

    for (int i = 0; i < pc_buffer_size; ++i) {
        pc_buffer[i] = 0;
    }

    for (int i = 0; i < l_buffer_size; ++i) {
        pc_buffer[i] = WN;
    }

    for (int k = 0; k < g_buffer_size; ++k) {
        g_buffer[k] = WN;
    }

//    for (int k = 0; k < chooser_size; ++k) {
//        chooser_buffer[k] = 2;
//    }
    perceptron_table = (int*) malloc( perceptron_table_size * ghistoryBits * sizeof(int));
    // weight initialization
    for(int i = 0; i < perceptron_table_size; ++i)
        for(int j = 0; j < ghistoryBits; ++j)
            perceptron_table[ i * ghistoryBits + j ] = 0;
}

uint8_t perceptron_predict(uint32_t pc){
    uint32_t g_buffer_counter = pc ^ branch_history_register;
    uint32_t p1 = g_buffer[g_buffer_counter % g_buffer_size];

    int l_buffer_counter = pc_buffer[pc % pc_buffer_size];
    uint32_t p2 = l_buffer[l_buffer_counter % l_buffer_size];

    int p3;
    int perceptron = pc % perceptron_table_size;
    // prediction
    int y = 1;
    int mask = 1;
    int bhr = branch_history_register;
    for (int i = 0; i < ghistoryBits; ++i) {
        int xi = mask & (bhr >> i);
        xi = (xi==1) ? 1 : -1;
        y += xi * perceptron_table[perceptron * ghistoryBits + i];
    }
    if(y > 0)
        p3 = TAKEN;
    else
        p3 = NOTTAKEN;

    int vote = 0;
    if(p1 > 1)
        vote++;
    if(p2 > 1)
        vote++;
    if(p3==1)
        vote++;
    if(vote > 1)
        return TAKEN;
    else
        return NOTTAKEN;

//    int choice = chooser_buffer[g_buffer_counter % chooser_size];
//    int prediction;
//    if(choice > 1){
//        prediction = g_buffer[g_buffer_counter % g_buffer_size];
//        if(prediction > 1)
//            return TAKEN;
//        else
//            return NOTTAKEN;
//    }
//    else{
//        int perceptron = pc % perceptron_table_size;
//        // prediction
//        int y = 1;
//        int mask = 1;
//        int bhr = branch_history_register;
//        for (int i = 0; i < ghistoryBits; ++i) {
//            int xi = mask & (bhr >> i);
//            xi = (xi==1) ? 1 : -1;
//            y += xi * perceptron_table[perceptron * ghistoryBits + i];
//        }
//        if(y > 0)
//            return TAKEN;
//        else
//            return NOTTAKEN;
//    }

}

void perceptron_train(uint32_t pc, uint8_t outcome){

    //chooser update
//    uint32_t g_buffer_counter = pc ^ branch_history_register;
//    uint32_t p1 = g_buffer[g_buffer_counter % g_buffer_size];
//
//    int perceptron = pc % perceptron_table_size;
//    // prediction
//    int y = 1;
//    int mask = 1;
//    int bhr = branch_history_register;
//    for (int i = 0; i < ghistoryBits; ++i) {
//        int xi = mask & (bhr >> i);
//        y += xi * perceptron_table[perceptron * ghistoryBits + i];
//    }
//    int p2;
//    if(y > 0)
//        p2 = 1;
//    else
//        p2 = 0;
//
//    int p1c = ((p1/2) == outcome);
//    int p2c = (p2 == outcome);
//
//    int action = p1c - p2c;
//
//    uint32_t choice = chooser_buffer[g_buffer_counter % chooser_size];
//    switch(action){
//        case -1:
//            if(choice > 0)
//                chooser_buffer[g_buffer_counter % chooser_size]--;
//            break;
//        case 1:
//            if(choice < 3)
//                chooser_buffer[g_buffer_counter % chooser_size]++;
//            break;
//        case 0:
//            break;
//        default:
//            break;
//    }

    int perceptron = pc % perceptron_table_size;
    // prediction
    int y = 1;
    int mask = 1;
    int bhr = branch_history_register;
    for (int i = 0; i < ghistoryBits; ++i) {
        int xi = mask & (bhr >> i);
        y += xi * perceptron_table[perceptron * ghistoryBits + i];
    }

    // perceptron update
    int t = (outcome == 1) ? 1 : -1;

    int limit = (int)pow(2, weight_size);
    int sign = (y > 0) ? 1 : -1;
    if( (sign!=t) || (abs(y) <= theta) ){
        for (int i = 0; i < ghistoryBits; ++i) {
            int xi = mask & (bhr >> i);
            xi = (xi==1) ? 1 : -1;
            int w = perceptron_table[perceptron * ghistoryBits + i] + t * xi;
            if(abs(w) < limit)
                perceptron_table[perceptron * ghistoryBits + i] = w;
        }
    }

    uint32_t g_buffer_counter = pc ^ branch_history_register;
    uint32_t l_buffer_counter = pc_buffer[pc % pc_buffer_size];

    // gshare update
    two_bit_buffer_update(g_buffer_counter, outcome, g_buffer, g_buffer_size);
    two_bit_buffer_update(l_buffer_counter, outcome, l_buffer, l_buffer_size);

    branch_history_register = (branch_history_register<<1 | outcome);
    pc_buffer[pc % pc_buffer_size] = (l_buffer_counter<< 1 | outcome);
}


// bimodal
// Use 11-bit in PC
uint32_t * directionTableTaken;
uint32_t * directionTableNTaken;
int choiceMask;
int directionMask;

void bimode_init() {
    ghistoryBits = 11;
    pcIndexBits = 12;

    choiceMask = (1 << pcIndexBits) - 1;
    directionMask = (1 << ghistoryBits) - 1;

    choicePredictionTableSize = (1 << pcIndexBits);
    choicePredictionTable = (uint32_t *) malloc(choicePredictionTableSize * sizeof(uint32_t));

    int directionTableSize = (1 << ghistoryBits);
    directionTableTaken = (uint32_t *) malloc(directionTableSize * sizeof(uint32_t));
    directionTableNTaken= (uint32_t *) malloc(directionTableSize * sizeof(uint32_t));
    int i;
    for (i = 0; i < directionTableSize; i++) {
        directionTableTaken[i] = WT;
        directionTableNTaken[i] = WN;
    }
}

int bimode_predict(uint32_t pc) {
    // Refer to choice predictor to select one direction.
    int choice = choicePredictionTable[pc & choiceMask] & 0b11;
    int directionIndex = (pc ^ gHistoryRegister) & directionMask;
    int prediction;
    if (choice == ST || choice == WT) { // taken
        prediction = directionTableTaken[directionIndex];
    } else { // not taken
        prediction = directionTableNTaken[directionIndex];
    }
    return action(prediction);
}

void bimode_train(uint32_t pc, uint8_t outcome) {
    int choiceIndex = pc & choiceMask;
    int choice = choicePredictionTable[choiceIndex] & 0b11;
    // Choice predictor is always updated with the branch outcome
    two_bit_buffer_update0(choiceIndex, outcome, choicePredictionTable);

    // Only the selected counter in direction predictor is updated
    int directionIndex = (pc ^ gHistoryRegister) & directionMask;
    if (choice == ST || choice == WT) { // taken
        two_bit_buffer_update0(directionIndex, outcome, directionTableTaken);
    } else { // not taken
        two_bit_buffer_update0(directionIndex, outcome, directionTableNTaken);
    }
    // Update global history
    gHistoryRegister <<= 1;
    gHistoryRegister |= outcome;
}




// Initialize the predictor
//
void init_predictor()
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
            //perceptron_init();
            bimode_init();
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
            // perceptron_predict(pc);
            return bimode_predict(pc);
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
            // perceptron_train(pc, outcome);
            bimode_train(pc, outcome);
            return;
        default:
            break;
    }
}

void cleanup_predictor() {
    switch (bpType) {
        case GSHARE:
            free(branch_history_buffer);
            break;
        case CUSTOM:
            free(chooser_buffer);
            free(g_buffer);
            free(l_buffer);
            free(pc_buffer);
            break;
        default:
            break;
    }
}