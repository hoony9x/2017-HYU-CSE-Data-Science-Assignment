//
//  assignment2.h
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#ifndef assignment2_hpp
#define assignment2_hpp

#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <map>
#include <set>
#include <functional>
#include <fstream>
#include <iomanip>
#include <tuple>

#include <cmath>

using namespace std;

class Node {
public:
    string name;
    map<string, Node*> next_list;
};

vector< vector< pair<string, string> > > getTrainingData(string &train_path);
vector< vector< pair<string, string> > > getTestData(string &test_path);

Node* processLearning(vector< vector< pair<string, string> > > &training_data_list);

void createTestResult(Node *decision_tree, vector< vector< pair<string, string> > > &test_data_list, string result_path, string class_name);

#endif /* assignment2_hpp */
