//
//  main.cpp
//  DS_Final
//
//  Created by KIHOON HAN on 08/06/2017.
//  Copyright © 2017 khhan1993. All rights reserved.
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

#include <cmath>

using namespace std;

map<int, map<int, int> > readData(string &data_path);
vector< tuple<int, int, int> > processPrediction(map<int, map<int, int> > &training, map<int, map<int, int> > &test);
void printResult(vector< tuple<int, int, int> > &results, string &output_path);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Input argument type mismatch. Please check again!" << endl;
        return -1;
    }
    
    string traning_data_path = argv[1];
    string test_data_path = argv[2];
    string output_path = traning_data_path + "_prediction.txt";
    
    auto training_data = readData(traning_data_path);
    auto test_data = readData(test_data_path);
    auto results = processPrediction(training_data, test_data);
    
    printResult(results, output_path);
    
    return 0;
}

map<int, map<int, int> > readData(string &data_path) {
    ifstream ifs(data_path, ifstream::in);
    
    map<int, map<int, int> > data;
    
    while(true) {
        /* We read timestamp, but not use it. */
        int user_id, item_id, rating, timestamp;
        ifs >> user_id >> item_id >> rating >> timestamp;
        
        if(ifs.eof()) {
            break;
        }
        
        data[user_id][item_id] = rating;
    }
    
    ifs.close();
    
    return data;
}

vector< tuple<int, int, int> > processPrediction(map<int, map<int, int> > &training_data, map<int, map<int, int> > &test_data) {
    
    set<int> item_list;
    int max_user_id = 0;
    int max_item_id = 0;
    
    /* Get max user_id and item_id. (1) */
    set<int> user_list_train;
    for(auto &user: training_data) {
        for(auto &item: user.second) {
            item_list.insert(item.first);
            max_item_id = max(max_item_id, item.first);
        }
        
        user_list_train.insert(user.first);
        max_user_id = max(max_user_id, user.first);
    }
    
    /* Get max user_id and item_id. (2) */
    set<int> user_list_test;
    for(auto &user: test_data) {
        for(auto &item: user.second) {
            item_list.insert(item.first);
            max_item_id = max(max_item_id, item.first);
        }
        
        user_list_test.insert(user.first);
        max_user_id = max(max_user_id, user.first);
    }
    
    /* Above process is to determine maximum user_id and maximum item_id. */
    
    /* Construct rating table and its empty check table. */
    vector< vector<double> > rating_table(max_user_id + 1);
    vector< vector<bool> > is_rating_table_empty(max_user_id + 1);
    
    for(auto i = 0; i <= max_user_id; i++) {
        rating_table[i].resize(max_item_id + 1);
        is_rating_table_empty[i].resize(max_item_id + 1);
        
        fill(rating_table[i].begin(), rating_table[i].end(), 0.0);
        fill(is_rating_table_empty[i].begin(), is_rating_table_empty[i].end(), true);
    }
    
    /* Generate average ratings for each user. */
    vector<double> user_avg_ratings(max_user_id + 1);
    fill(user_avg_ratings.begin(), user_avg_ratings.end(), 0.0);
    
    /* Assign rating value corresponds to user_id and item_id, and calculate each user's average rating.  */
    for(auto &user: training_data) {
        double sum = 0.0;
        for(auto &item: user.second) {
            sum += (double)item.second;
            
            /* Assign rating into 2D array and set this place as 'not empty'. */
            rating_table[user.first][item.first] = (double)item.second;
            is_rating_table_empty[user.first][item.first] = false;
        }
        
        /* Get each user's average value. */
        double avg = sum / (double)user.second.size();
        user_avg_ratings[user.first] = avg;
    }
    
    /* Similarity calculation function. */
    auto getSimilarity = [&rating_table, &is_rating_table_empty, &item_list](int user_x, int user_y) -> double {
        double numerator = 0.0;
        double denominator_left = 0.0, denominator_right = 0.0;
        double user_x_avg_rate = 0.0, user_y_avg_rate = 0.0;
        
        int count_v = 0;
        for(auto &item_id: item_list) {
            if(!is_rating_table_empty[user_x][item_id] && !is_rating_table_empty[user_y][item_id]) {
                /* First, get average value with rating of each item - if rating exists in both users. */
                user_x_avg_rate += rating_table[user_x][item_id];
                user_y_avg_rate += rating_table[user_y][item_id];
                count_v++;
            }
        }
        
        user_x_avg_rate = user_x_avg_rate / (double)count_v;
        user_y_avg_rate = user_y_avg_rate / (double)count_v;
        
        for(auto &item_id: item_list) {
            if(!is_rating_table_empty[user_x][item_id] && !is_rating_table_empty[user_y][item_id]) {
                /* Calculate similarity. */
                numerator += ((rating_table[user_x][item_id] - user_x_avg_rate) * (rating_table[user_y][item_id] - user_y_avg_rate));
                denominator_left += pow(rating_table[user_x][item_id] - user_x_avg_rate, 2);
                denominator_right += pow(rating_table[user_y][item_id] - user_y_avg_rate, 2);
            }
        }
        
        return numerator / (sqrt(denominator_left) * sqrt(denominator_right));
    };
    
    /* Prepare for prediction. */
    map<int, vector< pair<double, int> > > similarity_list;
    for(auto i = 0; i <= max_user_id; i++) {
        for(auto j = i + 1; j <= max_user_id; j++) {
            /* Get similarity. */
            double similarity = getSimilarity(i, j);
            
            /* If similarity is a number, push into each user's similarity list. */
            if(!isnan(similarity)) {
                similarity_list[i].push_back(make_pair(similarity, j));
                similarity_list[j].push_back(make_pair(similarity, i));
            }
        }
        
        /* Sort */
        sort(similarity_list[i].rbegin(), similarity_list[i].rend());
    }
    
    /* This function determines K value which is last index number of similarity_list that has greater than 0. */
    auto determineNumK = [&similarity_list](int user_id) -> int {
        int iteration_count = 0;
        for(auto i = 0; i < (int)similarity_list[user_id].size(); i++) {
            if(similarity_list[user_id][i].first < 0.0) {
                iteration_count = i;
                break;
            }
        }

        return iteration_count;
    };
    
    /* Rate prediction function. */
    auto predictRate = [&similarity_list, &rating_table, &is_rating_table_empty, &user_avg_ratings](int user_id, int item_id, int k) -> double {
        double r_a = user_avg_ratings[user_id];
        double predicted = r_a;
        double numerator = 0.0, denominator = 0.0;
        
        if(k > (int)similarity_list[user_id].size()) {
            k = (int)similarity_list[user_id].size();
        }
        
        for(auto i = 0; i < k; i++) {
            double similarity = similarity_list[user_id][i].first;
            int other_user_id = similarity_list[user_id][i].second;
            if(is_rating_table_empty[other_user_id][item_id]) {
                continue;
            }
            
            double r_b = user_avg_ratings[other_user_id];
            
            denominator += (similarity * (rating_table[other_user_id][item_id] - r_b));
            numerator += similarity;
        }
        
        if(!isnan(denominator / numerator)) {
            predicted += (denominator / numerator);
        }
        
        return predicted;
    };
    
    /* Store results in here. */
    vector< tuple<int, int, int> > results;
    for(auto &user: test_data) {
        int user_id = user.first;
        
        for(auto &item: user.second) {
            int item_id = item.first;
            int k = determineNumK(user_id);
            int predicted_rate = (int)round(predictRate(user_id, item_id, k));
            
            if(predicted_rate < 1) {
                results.push_back(make_tuple(user_id, item_id, 1));
            }
            else if(predicted_rate > 5) {
                results.push_back(make_tuple(user_id, item_id, 5));
            }
            else {
                results.push_back(make_tuple(user_id, item_id, predicted_rate));
            }
        }
    }
    
    return results;
}

void printResult(vector< tuple<int, int, int> > &results, string &output_path) {
    ofstream ofs(output_path, ofstream::out);
    
    for(auto &elem: results) {
        ofs << get<0>(elem) << '\t' << get<1>(elem) << '\t' << get<2>(elem) << '\n';
    }
    
    ofs.close();
}

