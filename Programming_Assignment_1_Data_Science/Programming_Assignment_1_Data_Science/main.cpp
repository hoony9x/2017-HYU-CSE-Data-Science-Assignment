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
    
    vector< set< set<int> > > L;
    L.resize(2);
    
    // Initialize candidates for frequent 1-itemsets
    set<int> C;
    for(auto &transaction: Transactions) {
        for(auto &elem: transaction) {
            C.insert(elem);
        }
    }
    
    // Build frequent 1-itemsets
    for(auto &elem: C) {
        int count_v = 0;
        
        for(auto &transaction: Transactions) {
            if(transaction.find(elem) != transaction.end()) {
                count_v++;
            }
        }
        
        if((double)count_v / (double)Transactions.size() * 100.0 >= min_support) {
            set<int> temp_set;
            
            temp_set.insert(elem);
            L[1].insert(temp_set);
        }
    }
    
    auto has_infrequent_subset = [](set<int> &C, set< set<int> > &previous_L) {
        for(auto &elem: C) {
            set<int> temp_set(C.begin(), C.end());
            temp_set.erase(elem);
            
            if(previous_L.find(temp_set) == previous_L.end()) {
                return true;
            }
        }
        
        return false;
    };
    
    auto apriori_gen = [&has_infrequent_subset](set< set<int> > &previous_L) {
        set< set<int> > Candidate_Set;
        for(auto &l1: previous_L) {
            for(auto &l2: previous_L) {
                if(l1 == l2) {
                    continue;
                }
                
                set<int> new_set(l1);
                new_set.insert(l2.begin(), l2.end());
                
                if(new_set.size() == l1.size() + 1) {
                    if(has_infrequent_subset(new_set, previous_L) == false) {
                        Candidate_Set.insert(new_set);
                    }
                }
            }
        }
        
        return Candidate_Set;
    };
    
    for(auto k = 2; ; k++) {
        set< set<int> > Candidate_k = apriori_gen(L[k - 1]);
        for(auto &transaction: Transactions) {
            
        }
    }
}
