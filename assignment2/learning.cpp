//
//  learn.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

Node* learn(string class_name, vector< map<string, string> > training_data, set<string> attr_list, map<string, set<string> > &orig_attr_list);

pair<bool, string> checkIfPure(string class_name, vector< map<string, string> > &training_data);
string getMajorityAttrVal(string class_name, vector< map<string, string> > &training_data);
string selectAttribute(string class_name, vector< map<string, string> > &training_data, set<string> &attr_list);

Node* processLearning(vector< vector< pair<string, string> > > &training_data_list) {
    string class_name = training_data_list.back().back().first;
    
    vector< map<string, string> > training_data;
    set<string> attr_list;
    map<string, set<string> > orig_attr_list;
    for(auto &list: training_data_list) {
        map<string, string> each_map;
        for(auto &elem: list) {
            each_map.insert(elem);
            attr_list.insert(elem.first);
            
            if(orig_attr_list.find(elem.first) == orig_attr_list.end()) {
                set<string> tmp;
                tmp.insert(elem.second);
                orig_attr_list.insert(make_pair(elem.first, tmp));
            }
            else {
                orig_attr_list[elem.first].insert(elem.second);
            }
        }
        training_data.push_back(each_map);
    }
    attr_list.erase(class_name);
    
    return learn(class_name, training_data, attr_list, orig_attr_list);
}

string selectAttribute(string class_name, vector< map<string, string> > &training_data, set<string> &attr_list) {
    
    auto getInfo = [&class_name](vector< map<string, string> > &training_data) -> double {
        map<string, int> cnt;
        for(auto &elem: training_data) {
            string val = elem[class_name];
            if(cnt.find(val) == cnt.end()) {
                cnt.insert(make_pair(val, 1));
            }
            else {
                cnt[val]++;
            }
        }
        
        double result = 0.0;
        for(auto &elem: cnt) {
            double p = (double)elem.second / (double)training_data.size();
            result += (-p * log2(p));
        }
        
        return result;
    };
    
    auto getSpecificInfo = [&getInfo](vector< map<string, string> > &training_data, string target) -> double {
        map<string, int> cnt;
        for(auto &elem: training_data) {
            string val = elem[target];
            if(cnt.find(val) == cnt.end()) {
                cnt.insert(make_pair(val, 1));
            }
            else {
                cnt[val]++;
            }
        }
        
        double result = 0.0;
        for(auto &elem: cnt) {
            double p = (double)elem.second / (double)training_data.size();
            
            vector< map<string, string> > partial_data;
            for(auto &data: training_data) {
                if(elem.first == data[target]) {
                    partial_data.push_back(data);
                }
            }
            
            p *= getInfo(partial_data);
            result += p;
        }
        
        return result;
    };
    
    auto splitInfo = [](vector< map<string, string> > &training_data, string target) -> double {
        map<string, int> cnt;
        for(auto &elem: training_data) {
            string val = elem[target];
            if(cnt.find(val) == cnt.end()) {
                cnt.insert(make_pair(val, 1));
            }
            else {
                cnt[val]++;
            }
        }
        
        double result = 0.0;
        for(auto &elem: cnt) {
            double p = (double)elem.second / (double)training_data.size();
            result -= (p * log2(p));
        }
        
        return result;
    };
    
    auto getGainRatio = [&getInfo, &getSpecificInfo, &splitInfo](vector< map<string, string> > &training_data, string target) -> double {
        double gain = getInfo(training_data) - getSpecificInfo(training_data, target);
        return gain / splitInfo(training_data, target);
    };
    
    string result;
    double max_gain_ratio = 0.0;
    for(auto &str: attr_list) {
        double gain_ratio = getGainRatio(training_data, str);
        if(gain_ratio > max_gain_ratio) {
            max_gain_ratio = gain_ratio;
            result = str;
        }
    }
    
    return result;
}

pair<bool, string> checkIfPure(string class_name, vector< map<string, string> > &training_data) {
    string value = training_data[0][class_name];
    for(auto i = 1; i < training_data.size(); i++) {
        if(training_data[i][class_name] != value) {
            return make_pair(false, value);
        }
    }
    
    return make_pair(true, value);
}

string getMajorityAttrVal(string class_name, vector< map<string, string> > &training_data) {
    map<string, int> chk;
    for(auto &elem: training_data) {
        string val = elem[class_name];
        if(chk.find(val) == chk.end()) {
            chk.insert(make_pair(val, 1));
        }
        else {
            chk[val]++;
        }
    }
    
    int max_v = 0;
    string result;
    for(auto &elem: chk) {
        if(max_v < elem.second) {
            max_v = elem.second;
            result = elem.first;
        }
    }
    
    return result;
}

Node* learn(string class_name, vector< map<string, string> > training_data, set<string> attr_list, map<string, set<string> > &orig_attr_list) {
    Node *N = new Node;
    
    auto check_if_pure = checkIfPure(class_name, training_data);
    if(check_if_pure.first == true) {
        N->name = check_if_pure.second;
        return N;
    }
    
    if(attr_list.empty()) {
        N->name = getMajorityAttrVal(class_name, training_data);
        return N;
    }
    
    string selected_attr = selectAttribute(class_name, training_data, attr_list);
    attr_list.erase(selected_attr);
    
    N->name = selected_attr;
    for(auto &elem: orig_attr_list[selected_attr]) {
        string value = elem;
        
        vector< map<string, string> > domain;
        for(auto &each_dataset: training_data) {
            if(each_dataset[selected_attr] == value) {
                domain.push_back(each_dataset);
            }
        }
        
        if(domain.empty()) {
            Node *next = new Node;
            next->name = getMajorityAttrVal(class_name, training_data);
            
            (N->next_list).insert(make_pair(value, next));
        }
        else {
            Node *next = learn(class_name, domain, attr_list, orig_attr_list);
            (N->next_list).insert(make_pair(value, next));
        }
    }
    
    return N;
}
