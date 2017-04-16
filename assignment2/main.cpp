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
    
    string train_path = argv[1];
    string test_path = argv[2];
    string result_path = argv[3];
    
    auto tranining_data = getTrainingData(train_path);
    auto test_data = getTestData(test_path);
    
    Node *decision_tree = processLearning(tranining_data);
    createTestResult(decision_tree, test_data, result_path, tranining_data.back().back().first);
    
    return 0;
}
