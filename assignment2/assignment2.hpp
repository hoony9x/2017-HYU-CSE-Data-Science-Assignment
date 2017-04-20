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
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <tuple>
#include <chrono>
#include <random>

#include <cmath>

using namespace std;

/* This is data structure for decision tree. */
class Node {
public:
    string name; /* This is attribute name if 'next_list' is empty. Otherwise, this is class label. */
    map<string, Node* > next_list; /* Branch for each attribute value. (If this node is not a leaf node.)  */
};

vector< vector< pair<string, string> > > getTrainingData(string &);
vector< vector< pair<string, string> > > getTestData(string &);
Node* processLearning(vector< vector< pair<string, string> > > &);
void createTestResult(Node *decision_tree, vector< vector< pair<string, string> > > &, string, string);

#endif /* assignment2_hpp */
