//
//  file_io_process.cpp
//  DC_Assignment2
//
//  Created by KIHOON HAN on 2017. 4. 16..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

#include "assignment2.hpp"

string deleteCarrageReturn(string S);

vector< vector< pair<string, string> > > getTrainingData(string &train_path) {
    ifstream ifs(train_path, ifstream::in);
    
    string str;
    
    getline(ifs, str);
    str = deleteCarrageReturn(str);
    
    string token;
    string delimiter = "\t";
    auto pos = string::npos;
    vector<string> attribute_names;
    
    while((pos = (int)str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        attribute_names.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    attribute_names.push_back(str);
    
    vector< vector<string> > attribute_values;
    while(true) {
        vector<string> tmp_list;
        
        getline(ifs, str);
        str = deleteCarrageReturn(str);
        
        if(ifs.eof()) {
            break;
        }
        
        while((pos = (int)str.find(delimiter)) != string::npos) {
            token = str.substr(0, pos);
            tmp_list.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tmp_list.push_back(str);
        
        attribute_values.push_back(tmp_list);
    }
    
    ifs.close();
    
    vector< vector< pair<string, string> > > training_data;
    for(auto &elem: attribute_values) {
        vector< pair<string, string> > each_data;
        for(auto i = 0; i < elem.size(); i++) {
            each_data.push_back(make_pair(attribute_names[i], elem[i]));
        }
        training_data.push_back(each_data);
    }
    
    return training_data;
}

vector< vector< pair<string, string> > > getTestData(string &test_path) {
    ifstream ifs(test_path, ifstream::in);
    
    string str;
    
    getline(ifs, str);
    str = deleteCarrageReturn(str);
    
    string token;
    string delimiter = "\t";
    auto pos = string::npos;
    vector<string> attribute_names;
    
    while((pos = (int)str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        attribute_names.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    attribute_names.push_back(str);
    
    vector< vector<string> > attribute_values;
    while(true) {
        vector<string> tmp_list;
        
        getline(ifs, str);
        str = deleteCarrageReturn(str);
        
        if(ifs.eof()) {
            break;
        }
        
        while((pos = (int)str.find(delimiter)) != string::npos) {
            token = str.substr(0, pos);
            tmp_list.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tmp_list.push_back(str);
        
        attribute_values.push_back(tmp_list);
    }
    
    ifs.close();
    
    vector< vector< pair<string, string> > > test_data;
    for(auto &elem: attribute_values) {
        vector< pair<string, string> > each_data;
        for(auto i = 0; i < elem.size(); i++) {
            each_data.push_back(make_pair(attribute_names[i], elem[i]));
        }
        test_data.push_back(each_data);
    }
    
    return test_data;
}

string deleteCarrageReturn(string S) {
    string carrage_return = "\r";
    auto pos = string::npos;
    
    while((pos = (int)S.find(carrage_return)) != string::npos) {
        S.erase(pos, pos + carrage_return.length());
    }
    
    return S;
}
