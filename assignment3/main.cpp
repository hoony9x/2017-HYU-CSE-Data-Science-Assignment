//
//  main.cpp
//  DC_Assignment3
//
//  Created by KIHOON HAN on 2017. 5. 14..
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

#include <cmath>

using namespace std;

map<int, pair<bool, pair<double, double> > > readDataFromInput(string &input_path);
vector< set<int> > DBSCAN(map<int, pair<bool, pair<double, double> > > &objects, int num, double Eps, int MinPts);
void printResult(string &input_path, vector< set<int> > &cluster_list);

int main(int argc, char *argv[]) {
    if(argc != 5) {
        cerr << "Input argument type mismatch. Please check again!" << endl;
        return -1;
    }
    
    string input_path = argv[1];
    int num = atoi(argv[2]);
    double Eps = atof(argv[3]);
    int MinPts = atoi(argv[4]);
    
    auto objects = readDataFromInput(input_path);
    vector< set<int> > cluster_list = DBSCAN(objects, num, Eps, MinPts);
    printResult(input_path, cluster_list);
    
    return 0;
}

map<int, pair<bool, pair<double, double> > > readDataFromInput(string &input_path) {
    ifstream ifs(input_path, ifstream::in);
    
    map<int, pair<bool, pair<double, double> > > objects;
    
    while(true) {
        int obj_id;
        double x, y;
        
        ifs >> obj_id >> x >> y;
        
        if(ifs.eof()) {
            break;
        }
        
        objects.insert(make_pair(obj_id, make_pair(false, make_pair(x, y))));
    }
    
    ifs.close();
    
    return objects;
}

vector< set<int> > DBSCAN(map<int, pair<bool, pair<double, double> > > &objects, int num, double Eps, int MinPts) {
    auto getDist = [](pair<double, double> &A, pair<double, double> &B) {
        auto dist_x_square = (A.first - B.first) * (A.first - B.first);
        auto dist_y_square = (A.second - B.second) * (A.second - B.second);
        
        return sqrt(dist_x_square + dist_y_square);
    };
    
    auto regionQuery = [&getDist, &objects, &Eps](int obj_id) {
        pair<double, double> target_point = objects[obj_id].second;
        
        set<int> N;
        for(auto &P: objects) {
            pair<double, double> other_point = P.second.second;
            if(getDist(target_point, other_point) <= Eps) {
                N.insert(P.first);
            }
        }
        
        return N;
    };
    
    map<int, int> obj_cluster_map;
    auto expandCluster = [&regionQuery, &MinPts, &objects, &obj_cluster_map](int cur_obj_id, set<int> &N, int cluster_id) {
        obj_cluster_map.insert(make_pair(cur_obj_id, cluster_id));
        
        while(true) {
            bool is_finish = true;
            
            for(auto &obj_id: N) {
                auto P = objects[obj_id];
                if(!P.first) {
                    objects[obj_id].first = true;
                    set<int> _N = regionQuery(obj_id);
                    
                    if(_N.size() >= MinPts) {
                        is_finish = false;
                        N.insert(_N.begin(), _N.end());
                    }
                }
                
                if(obj_cluster_map.find(obj_id) == obj_cluster_map.end()) {
                    obj_cluster_map.insert(make_pair(obj_id, cluster_id));
                }
                
                if(!is_finish) {
                    break;
                }
            }
            
            if(is_finish) {
                break;
            }
        }
    };
    
    int cluster_id = 0;
    for(auto &P: objects) {
        P.second.first = true;
        set<int> N = regionQuery(P.first);
        
        if(N.size() < MinPts) {
            // Noise
        }
        else {
            expandCluster(P.first, N, cluster_id);
            cluster_id++;
        }
    }
    
    vector< set<int> > cluster_list_candidate(cluster_id);
    for(auto &item: obj_cluster_map) {
        int obj_id = item.first;
        int mapped_cluster_id = item.second;
        
        cluster_list_candidate[mapped_cluster_id].insert(obj_id);
    }
    
    vector< set<int> > cluster_list;
    for(auto &data_set: cluster_list_candidate) {
        if(data_set.size() >= MinPts) {
            cluster_list.push_back(data_set);
        }
    }
    
    sort(cluster_list.rbegin(), cluster_list.rend(), [](set<int> &A, set<int> &B) {
        return A.size() < B.size();
    });
    
    while(cluster_list.size() > num) {
        cluster_list.pop_back();
    }
    
    return cluster_list;
}

void printResult(string &input_path, vector< set<int> > &cluster_list) {
    string output_path = input_path;
    output_path.erase(output_path.rfind(".txt"), 4);
    
    for(auto i = 0; i < cluster_list.size(); i++) {
        string output_file_path = output_path + "_cluster_" + to_string(i) + ".txt";
        ofstream ofs(output_file_path, ofstream::out);
        
        for(auto &obj_id: cluster_list[i]) {
            ofs << obj_id << endl;
        }
        
        ofs.close();
    }
}
