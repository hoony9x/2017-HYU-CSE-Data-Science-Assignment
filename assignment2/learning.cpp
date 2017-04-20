//
//  learn.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

Node* learn(string, vector< map<string, string> >, set<string>, map<string, set<string> > &);

string selectAttributeByGiniIdx(string, vector< map<string, string> > &, set<string> &);
string selectAttributeByGainRatio(string, vector< map<string, string> > &, set<string> &);

pair<bool, string> checkIfPure(string, vector< map<string, string> > &);
pair<string, bool> getMajorityAttrVal(string, vector< map<string, string> > &);

double getInfo(string, vector< map<string, string> > &);
double getSpecificInfo(string, vector< map<string, string> > &, string);
double getGain(string, vector< map<string, string> > &, string);
double splitInfo(vector< map<string, string> > &, string);
double getGainRatio(string, vector< map<string, string> > &, string);

/* Initiate decision tree building process. */
Node* processLearning(vector< vector< pair<string, string> > > &training_data_list) {
    string class_name = training_data_list.back().back().first; /* Get class name (target name) */
    
    vector< map<string, string> > training_data; /* Convert data structure into map. */
    set<string> attr_list; /* Get attribute name list. */
    map<string, set<string> > orig_attr_list; /* Get values of each attribute name. */
    
    /* Convert training data structure into map data structure, and get attribute name list and its values. */
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
    
    /* Start building decision tree and return it. */
    return learn(class_name, training_data, attr_list, orig_attr_list);
}

/* Decision tree building process. This function generates each node. */
Node* learn(string class_name, vector< map<string, string> > dataset, set<string> attr_list, map<string, set<string> > &orig_attr_list) {
    Node *N = new Node;
    
    /* Check if current dataset is pure. */
    auto chk_if_pure = checkIfPure(class_name, dataset);
    if(chk_if_pure.first == true) {
        N->name = chk_if_pure.second;
        return N;
    }
    
    /* Check if there is no more attribute name. */
    if(attr_list.empty()) {
        N->name = getMajorityAttrVal(class_name, dataset).first;
        return N;
    }
    
    /* Select attribute name using Gain Ratio. */
    string selected_attr = selectAttributeByGainRatio(class_name, dataset, attr_list);
    
    /* Assign selected attribute name into node and delete it from attribute list. */
    N->name = selected_attr;
    attr_list.erase(selected_attr);
    
    /* Generate each branch belongs to attribute values */
    for(auto &elem: orig_attr_list[selected_attr]) {
        string value = elem; /* attribute value. */
        
        /* Get training data which attribute value match with it. */
        vector< map<string, string> > domain;
        for(auto &each_dataset: dataset) {
            if(each_dataset[selected_attr] == value) {
                domain.push_back(each_dataset);
            }
        }
        
        /* If no data has been matched with attribute value, select class label using majority vote. */
        if(domain.empty()) {
            Node *next = new Node;
            next->name = getMajorityAttrVal(class_name, dataset).first;
            
            (N->next_list).insert(make_pair(value, next));
        }
        else {
            /* Generate subtree. */
            Node *next = learn(class_name, domain, attr_list, orig_attr_list);
            (N->next_list).insert(make_pair(value, next));
        }
    }
    
    /* Return each node with its subtree. */
    return N;
}

/* Check of given dataset is pure. */
pair<bool, string> checkIfPure(string class_name, vector< map<string, string> > &training_data) {
    string value = training_data[0][class_name];
    for(auto i = 1; i < training_data.size(); i++) {
        if(training_data[i][class_name] != value) {
            return make_pair(false, value);
        }
    }
    
    /* return with class label. */
    return make_pair(true, value);
}

/* Get class label using majority vote. */
pair<string, bool> getMajorityAttrVal(string class_name, vector< map<string, string> > &training_data) {
    /* Count each attribute value. */
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
    bool is_ambiguous = false;
    
    /* Select class label which has the biggest count value. */
    for(auto &elem: chk) {
        if(max_v < elem.second) {
            max_v = elem.second;
            is_ambiguous = false;
            result = elem.first;
        }
        else if(max_v == elem.second) {
            is_ambiguous = true;
        }
    }
    
    return make_pair(result, is_ambiguous);
}

/* Select attribute name using Gain Ratio. */
string selectAttributeByGainRatio(string class_name, vector< map<string, string> > &training_data, set<string> &attr_list) {
    string result;
    double max_gain_ratio = 0.0;
    for(auto &str: attr_list) {
        double gain_ratio = getGainRatio(class_name, training_data, str);
        
        /* Select attribute which has the biggest gain ratio. */
        if(gain_ratio > max_gain_ratio) {
            max_gain_ratio = gain_ratio;
            result = str;
        }
    }
    
    return result;
}

/* Get entropy. */
double getInfo(string class_name, vector< map<string, string> > &training_data) {
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
}

/* Get entropy for specific attribute name (target) */
double getSpecificInfo(string class_name, vector< map<string, string> > &training_data, string target) {
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
        
        p *= getInfo(class_name, partial_data);
        result += p;
    }
    
    return result;
}

/* Get information gain. */
double getGain(string class_name, vector< map<string, string> > &training_data, string target) {
    return getInfo(class_name, training_data) - getSpecificInfo(class_name, training_data, target);
}

/* Get split info. */
double splitInfo(vector< map<string, string> > &training_data, string target) {
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
}

/* Get gain ratio. */
double getGainRatio(string class_name, vector< map<string, string> > &training_data, string target) {
    return getGain(class_name, training_data, target) / splitInfo(training_data, target);
}
