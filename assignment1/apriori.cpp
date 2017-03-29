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
#include <iomanip>

#include <cstdlib>

using namespace std;

vector< set<int> > getTransactions(string &input_path);
map< set<int>, int > generateFrequentSets(double &min_support, vector< set<int> > &Transactions);
void putAssociationRules(map< set<int>, int > &freq_sets, int num_of_transactions, string &output_path);

int main(int argc, char *argv[]) {
    if(argc != 4) {
        cerr << "Input argument type mismatch. Please check again!" << endl;
        return -1;
    }
    
    double min_support = atof(argv[1]);
    string input_path = argv[2];
    string output_path = argv[3];
    
    /* Read transaction data from input file. */
    vector< set<int> > Transactions = getTransactions(input_path);
    
    /* Generate frequent sets. */
    map< set<int>, int > freq_sets = generateFrequentSets(min_support, Transactions);
    
    /* Generate association rules and Write into output file. */
    putAssociationRules(freq_sets, (int)Transactions.size(), output_path);
    
    return 0;
}

/* Read transaction data from input file. */
vector< set<int> > getTransactions(string &input_path) {
    /* Open input file. */
    ifstream ifs(input_path, ifstream::in);
    
    /* Transaction data will be stored here. */
    vector< set<int> > Transactions;
    
    /* Read each line from input file until we meet EOF. */
    while(ifs.eof() == false) {
        set<int> each_transaction;
        
        string S;
        getline(ifs, S);
        
        /* Parsing transaction data. */
        int pos = 0;
        string delimiter = "\t";
        while((pos = (int)S.find(delimiter)) != string::npos) {
            string token = S.substr(0, pos);
            each_transaction.insert(stoi(token));
            S.erase(0, pos + delimiter.length());
        }
        each_transaction.insert(stoi(S));
        
        /* Insert into transaction list. */
        Transactions.push_back(each_transaction);
    }
    
    /* Close input file. */
    ifs.close();
    
    return Transactions;
}

/* Generate frequent sets. */
map< set<int>, int > generateFrequentSets(double &min_support, vector< set<int> > &Transactions) {
    vector< map< set<int>, int> > L;
    L.resize(2);
    
    /* find frequent 1-itemsets */
    for(auto &trans: Transactions) {
        for(auto &t_elem: trans) {
            set<int> temp_S;
            temp_S.insert(t_elem);
            
            /* Check if already counted before. */
            if(L[1].find(temp_S) != L[1].end()) {
                continue;
            }
            
            /* Count for each element. */
            int count_v = 0;
            
            /* Search all transaction data and increase count value if target element exists. */
            for(auto &transaction: Transactions) {
                if(transaction.find(t_elem) != transaction.end()) {
                    count_v++;
                }
            }
            
            /* If support value of current element is not less than min_support, add to L[1]. */
            if((double)count_v / (double)Transactions.size() * 100.0 >= min_support) {
                L[1].insert(pair< set<int>, int >(temp_S, count_v));
            }
        }
    }
    
    /* Function for check if target set has an infrequent set. */
    auto hasInfreqSubset = [](const set<int> &new_set, const map< set<int>, int > &prev_L) -> bool {
        for(auto &elem: new_set) {
            /* Create temporary set for checking for infrequent set. Iteratively erase one element. */
            set<int> temp_set(new_set);
            temp_set.erase(elem);
            
            /* If temporary set is frequent, it must be in L. Otherwise, it is an infrequent set. */
            if(prev_L.find(temp_set) == prev_L.end()) {
                return true;
            }
        }
        
        return false;
    };
    
    /* Generate candidate set for next L. */
    auto aprioriGenCandidate = [&hasInfreqSubset](const map< set<int>, int > &prev_L) -> set< set<int> > {
        set< set<int> > candidate_set;
        
        /* Generate candidate. */
        for(auto &l1: prev_L) {
            for(auto &l2: prev_L) {
                /* We don't have to doing cartesian product for same set. */
                if(l1 == l2) {
                    continue;
                }
                
                /* Doing cartesian product. */
                set<int> new_set(l1.first);
                new_set.insert(l2.first.begin(), l2.first.end());
                
                /* Check the size of new set and it does not contain infrequent subset. */
                if(new_set.size() == l1.first.size() + 1) {
                    if(hasInfreqSubset(new_set, prev_L) == false) {
                        candidate_set.insert(new_set);
                    }
                }
            }
        }
        
        return candidate_set;
    };
    
    /* Check if 'sub_set' is subset of 'super_set'. */
    auto checkSubset = [](const set<int> &super_set, const set<int> &sub_set) -> bool {
        if(super_set.size() < sub_set.size()) {
            /* Size of subset can't be bigger than superset! */
            return false;
        }
        else if(super_set == sub_set) {
            /* Same set! */
            return true;
        }
        else {
            for(auto &elem: sub_set) {
                /*
                 * If element in 'sub_set' is not int 'super_set',
                 * 'sub_set' is not subset of 'super_set'!
                 */
                if(super_set.find(elem) == super_set.end()) {
                    return false;
                }
            }
            
            return true;
        }
    };
    
    for(int k = 2; ; k++) {
        /* Get candidate set. */
        set< set<int> > candidate_set = aprioriGenCandidate(L[k - 1]);
        
        /* Build L */
        map< set<int>, int > new_L;
        for(auto &trans: Transactions) {
            for(auto &candidate: candidate_set) {
                if(checkSubset(trans, candidate) == true) {
                    if(new_L.find(candidate) == new_L.end()) {
                        new_L.insert(pair< set<int>, int >(candidate, 1));
                    }
                    else {
                        new_L[candidate]++;
                    }
                }
                
            }
        }
        
        /* Add new L into list of L (L[k]) -> This is not completed L! */
        L.push_back(new_L);
        
        /* If there is infrequent set in L[k], remove it. */
        for(auto &l_elem: new_L) {
            if((double)l_elem.second / (double)Transactions.size() * 100.0 < min_support) {
                L[k].erase(l_elem.first);
            }
        }
        
        /* If frequent set is noy generated(that is, L[k] is empty), escape loop. */
        if(L[k].empty()) {
            L.pop_back();
            break;
        }
    }
    
    /* Store all frequent sets into single map data structure. */
    map< set<int>, int > freq_sets;
    for(auto &l_elem: L) {
        for(auto &s_elem: l_elem) {
            freq_sets.insert(s_elem);
        }
    }
    
    return freq_sets;
}

/* Generate association rules and Write into output file. */
void putAssociationRules(map< set<int>, int > &freq_sets, int num_of_transactions, string &output_path) {
    
    /* Generate all subset except null set. */
    auto getAllSubset = [](const set<int> &S) -> set< set<int> > {
        set< set<int> > subsets;
        
        for(auto &item: S) {
            for(auto &inner_set: subsets) {
                set<int> temp_s(inner_set);
                temp_s.insert(item);
                
                subsets.insert(temp_s);
            }
            set<int> temp_s2;
            temp_s2.insert(item);
            subsets.insert(temp_s2);
        }
        
        return subsets;
    };
    
    /* Open output file. */
    ofstream ofs(output_path, ofstream::out);
    ofs << setprecision(2) << fixed;
    
    for(auto &each_set: freq_sets) {
        if(each_set.first.size() < 2) {
            continue;
        }
        
        /* 
         * Association Rule
         * {each_set} => {associative_set}
         */
        
        set< set<int> > subsets = getAllSubset(each_set.first);
        subsets.erase(each_set.first);
        
        for(auto &item_set: subsets) {
            set<int> associative_set(each_set.first);
            for(auto &elem: item_set) {
                associative_set.erase(elem);
            }
            
            string item_set_format = "{";
            for(auto &elem: item_set) {
                item_set_format += to_string(elem);
                item_set_format += ",";
            }
            item_set_format.pop_back();
            item_set_format.push_back('}');
            
            string associative_set_format = "{";
            for(auto &elem: associative_set) {
                associative_set_format += to_string(elem);
                associative_set_format += ",";
            }
            associative_set_format.pop_back();
            associative_set_format.push_back('}');
            
            double support_v = (double)each_set.second / (double)num_of_transactions * 100.0;
            double confidence_v = (double)each_set.second / (double)freq_sets[item_set] * 100.0;
            
            /* To avoid Banker's Rounding, I use custom rounding macro for this project. */
            auto customRoundToThree = [](double X) -> double {
                return ((double)((int)((X) * 100.0 + 0.5))) / 100.0;
            };
            
            ofs << item_set_format << '\t' << associative_set_format << '\t';
            ofs << customRoundToThree(support_v) << '\t' << customRoundToThree(confidence_v) << endl;
        }
    }
    
    ofs.close();
}
