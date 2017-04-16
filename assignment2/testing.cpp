//
//  testing.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

void createTestResult(Node *decision_tree, vector< vector< pair<string, string> > > &test_data_list, string result_path, string class_name) {
    vector< map<string, string> > test_data;
    for(auto &list: test_data_list) {
        map<string, string> each_map;
        for(auto &elem: list) {
            each_map.insert(elem);
        }
        test_data.push_back(each_map);
    }

    vector<string> attr_list;
    for(auto &elem: test_data_list.back()) {
        attr_list.push_back(elem.first);
    }
    attr_list.push_back(class_name);
    
    ofstream ofs(result_path, ofstream::out);
    for(auto i = 0; i < attr_list.size() - 1; i++) {
        ofs << attr_list[i] << '\t';
    }
    ofs << attr_list.back() << '\n';
    
    function<string (Node *, map<string, string> &)> testFunction = [&testFunction](Node *N, map<string, string> &test_data) -> string {
        if((N->next_list).empty()) {
            return N->name;
        }
        else {
            string attr_name = N->name;
            string value = test_data[attr_name];
            
            Node *next = (N->next_list)[value];
            return testFunction(next, test_data);
        }
    };
    
    for(auto i = 0; i < test_data_list.size(); i++) {
        for(auto &elem: test_data_list[i]) {
            ofs << elem.second << '\t';
        }
        
        ofs << testFunction(decision_tree, test_data[i]) << '\n';
    }
    
    ofs.close();
}
