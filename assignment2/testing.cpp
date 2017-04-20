//
//  testing.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

/* Create test result and write into result file. */
void createTestResult(Node *decision_tree, vector< vector< pair<string, string> > > &test_data_list, string result_path, string class_name) {
    /* Convert test data structure into map data structure. */
    vector< map<string, string> > test_data;
    for(auto &list: test_data_list) {
        map<string, string> each_map;
        for(auto &elem: list) {
            each_map.insert(elem);
        }
        test_data.push_back(each_map);
    }

    /* Get attribute list from test data. (Will be used in data writing process) */
    vector<string> attr_list;
    for(auto &elem: test_data_list.back()) {
        attr_list.push_back(elem.first);
    }
    attr_list.push_back(class_name); /* attach class name. */
    
    ofstream ofs(result_path, ofstream::out);
    for(auto i = 0; i < attr_list.size() - 1; i++) {
        ofs << attr_list[i] << '\t';
    }
    ofs << attr_list.back() << '\n';
    
    /* Testing function. */
    function<string (Node *, map<string, string> &)> testFunction = [&testFunction, &class_name](Node *N, map<string, string> &test_data) -> string {
        if((N->next_list).empty()) {
            /* if 'next_list' is empty, it means this node is leaf node. */
            return N->name;
        }
        else {
            string attr_name = N->name; /* Get attribute name from current node. */
            string value = test_data[attr_name]; /* Get attribute value of current test data tuple. */
            
            if((N->next_list).find(value) == (N->next_list).end()) {
                /* This case occurs when current attribute value is not in training data. */
                
                map<string, int> chk;
                for(auto &elem: N->next_list) {
                    Node *next = (N->next_list)[elem.first]; /* Select each branch of current node. */
                    string val = testFunction(next, test_data); /* Get result value from each branch. */
                    if(chk.find(val) == chk.end()) {
                        chk.insert(make_pair(val, 1));
                    }
                    else {
                        chk[val]++;
                    }
                }
                
                /* Select class label which has the biggest count value. */
                int max_v = 0;
                string selected;
                for(auto &elem: chk){
                    if(max_v < elem.second) {
                        max_v = elem.second;
                        selected = elem.first;
                    }
                }
                
                return selected;
            }
            else {
                /* Select branch and move into next node. */
                Node *next = (N->next_list)[value];
                return testFunction(next, test_data);
            }
        }
    };
    
    for(auto i = 0; i < test_data_list.size(); i++) {
        for(auto &elem: test_data_list[i]) {
            ofs << elem.second << '\t';
        }
        
        /* Write test result. */
        ofs << testFunction(decision_tree, test_data[i]) << '\n';
    }
    
    ofs.close();
}
