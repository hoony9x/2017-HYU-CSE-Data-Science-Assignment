//
//  main.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 13..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

int main(int argc, char * argv[]) {
    if(argc != 4) {
        cerr << "Input argument type mismatch. Please check again!" << endl;
        return -1;
    }
    
    string train_path = argv[1]; /* train data input file path */
    string test_path = argv[2]; /* test data input file path */
    string result_path = argv[3]; /* result data output file path */
    
    auto tranining_data = getTrainingData(train_path); /* Get training data. */
    auto test_data = getTestData(test_path); /* Get test data. */
    
    Node *decision_tree = processLearning(tranining_data); /* Generate decision tree using GainRatio. */
    string class_name = tranining_data.back().back().first; /* Get class name from training data. (last column of each data tuple) */
    createTestResult(decision_tree, test_data, result_path, class_name); /* Generate test result and write into result file. */
    
    return 0;
}
