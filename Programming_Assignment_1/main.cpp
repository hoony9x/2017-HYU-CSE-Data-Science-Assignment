//
//  main.cpp
//  Programming_Assignment_1_Data_Science
//
//  Created by KIHOON HAN on 2017. 3. 15..
//  Copyright © 2017년 khhan1993. All rights reserved.
//

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

using namespace std;

vector< set<int> > getTransactions(string &input_path);
void processAprioriAlgorithm(double &min_support, vector< set<int> > &Transactions, string &output_path);

int main(int argc, char *argv[]) {
    if(argc != 4) {
        cerr << "Input argument type mismatch. Please check again!" << endl;
        return -1;
    }
    
    double min_support = atof(argv[1]);
    string input_path = argv[2];
    string output_path = argv[3];
    
    vector< set<int> > Transactions = getTransactions(input_path);
    processAprioriAlgorithm(min_support, Transactions, output_path);
    
    return 0;
}

vector< set<int> > getTransactions(string &input_path) {
    ifstream ifs(input_path, ifstream::in);
    
    vector< set<int> > Transactions;
    while(ifs.eof() == false) {
        set<int> each_transaction;
        
        string S;
        getline(ifs, S);
        
        int pos = 0;
        string delimiter = "\t";
        while((pos = (int)S.find(delimiter)) != string::npos) {
            string token = S.substr(0, pos);
            each_transaction.insert(stoi(token));
            S.erase(0, pos + delimiter.length());
        }
        each_transaction.insert(stoi(S));
        Transactions.push_back(each_transaction);
    }
    
    ifs.close();
    
    return Transactions;
}

void processAprioriAlgorithm(double &min_support, vector< set<int> > &Transactions, string &output_path) {
    vector< map< set<int>, int> > L;
    L.resize(2);
    
    // find frequent 1-itemsets
    for(auto &trans: Transactions) {
        for(auto &t_elem: trans) {
            set<int> temp_S;
            temp_S.insert(t_elem);
            
            for(auto &s_elem: temp_S) {
                int count_v = 0;
                
                for(auto &transaction: Transactions) {
                    if(transaction.find(s_elem) != transaction.end()) {
                        count_v++;
                    }
                }
                
                if((double)count_v / (double)Transactions.size() * 100.0 >= min_support) {
                    if(L[1].find(temp_S) == L[1].end()) {
                        L[1].insert(pair< set<int>, int >(temp_S, 1));
                    }
                    else {
                        L[1][temp_S]++;
                    }
                }
            }
        }
    }
    
    auto has_infrequent_subset = [](const set<int> &C, const map< set<int>, int > &prev_L) {
        for(auto &elem: C) {
            set<int> temp_set(C);
            temp_set.erase(elem);
            
            if(prev_L.find(temp_set) == prev_L.end()) {
                return true;
            }
        }
        
        return false;
    };
    
    auto apriori_gen = [&has_infrequent_subset](const map< set<int>, int > &prev_L) {
        set< set<int> > Candidate_Set;
        
        for(auto &l1: prev_L) {
            for(auto &l2: prev_L) {
                if(l1 == l2) {
                    continue;
                }
                
                set<int> new_set(l1.first);
                new_set.insert(l2.first.begin(), l2.first.end());
                
                if(new_set.size() == l1.first.size() + 1) {
                    if(has_infrequent_subset(new_set, prev_L) == false) {
                        Candidate_Set.insert(new_set);
                    }
                }
            }
        }
        
        return Candidate_Set;
    };
    
    auto chk_if_subset = [](const set<int> &super_set, const set<int> &sub_set) {
        if(super_set.size() < sub_set.size()) {
            return false;
        }
        else if(super_set == sub_set) {
            return true;
        }
        else {
            for(auto &elem: sub_set) {
                if(super_set.find(elem) == super_set.end()) {
                    return false;
                }
            }
            
            return true;
        }
    };
    
    for(int k = 2; ; k++) {
        set< set<int> > Candidate_Set = apriori_gen(L[k - 1]);
        
        map< set<int>, int > new_L;
        for(auto &trans: Transactions) {
            for(auto &candidate: Candidate_Set) {
                if(chk_if_subset(trans, candidate) == true) {
                    if(new_L.find(candidate) == new_L.end()) {
                        new_L.insert(pair< set<int>, int >(candidate, 1));
                    }
                    else {
                        new_L[candidate]++;
                    }
                }
                
            }
        }
        
        L.push_back(new_L);
        
        for(auto &l_elem: new_L) {
            if((double)l_elem.second / (double)Transactions.size() * 100.0 < min_support) {
                L[k].erase(l_elem.first);
            }
        }
        
        if(L[k].empty()) {
            L.pop_back();
            break;
        }
    }
    
    int stop = 0;
}
