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
    
    string input_path = argv[1]; /* Input file path. */
    int num = atoi(argv[2]); /* Number of clusters. */
    double Eps = atof(argv[3]); /* Eps */
    int MinPts = atoi(argv[4]); /* Minimum points. */
    
    auto objects = readDataFromInput(input_path); /* Read object position data from input file. */
    vector< set<int> > cluster_list = DBSCAN(objects, num, Eps, MinPts); /* Generate clusters. */
    printResult(input_path, cluster_list); /* Write result into output fiie. */
    
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
        
        /* Just read data from input file, and push into 'map' object. */
        objects.insert(make_pair(obj_id, make_pair(false, make_pair(x, y))));
    }
    
    ifs.close();
    
    return objects;
}

vector< set<int> > DBSCAN(map<int, pair<bool, pair<double, double> > > &objects, int num, double Eps, int MinPts) {
    /* Get distance between two objects. */
    auto getDist = [](pair<double, double> &A, pair<double, double> &B) {
        auto dist_x_square = (A.first - B.first) * (A.first - B.first);
        auto dist_y_square = (A.second - B.second) * (A.second - B.second);
        
        return sqrt(dist_x_square + dist_y_square);
    };
    
    /* Get list of neighbors. */
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
    
    /* Clustered data list. (key: object_id, data: cluster_id) */
    map<int, int> obj_cluster_map;
    auto expandCluster = [&regionQuery, &MinPts, &objects, &obj_cluster_map](int cur_obj_id, set<int> &N, int cluster_id) {
        /* Add selected object into cluster. */
        obj_cluster_map.insert(make_pair(cur_obj_id, cluster_id));
        
        while(true) {
            bool is_finish = true;
            
            for(auto &obj_id: N) { /* Each object in current neighbor group */
                auto P = objects[obj_id]; /* Get one object. */
                if(!P.first) { /* Check if already visited. */
                    objects[obj_id].first = true; /* Set this object as visited. */
                    set<int> _N = regionQuery(obj_id); /* Get more neighbors */
                    
                    if(_N.size() >= MinPts) {
                        is_finish = false;
                        N.insert(_N.begin(), _N.end()); /* Union current neighbors group and new neighbors group. */
                    }
                }
                
                /* If object is not yet member of any cluster, add it into cluster. */
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
    
    int cluster_id = 0; /* Set initial cluster_id as 0. Increment in each iteration. */
    for(auto &P: objects) {
        /* Check if already visited. */
        if(P.second.first) {
            continue;
        }
        
        P.second.first = true; /* Set it as visited. */
        set<int> N = regionQuery(P.first); /* Get neighbors of this object. */
        
        if(N.size() < MinPts) {
            // Noise
        }
        else {
            /* If this object is not noise, expand cluster. */
            expandCluster(P.first, N, cluster_id);
            cluster_id++; /* Increase cluster_id */
        }
    }
    
    /* Convert data store object from 'map' to list of 'set' to make easy output process. */
    vector< set<int> > cluster_list_candidate(cluster_id);
    for(auto &item: obj_cluster_map) {
        int obj_id = item.first;
        int mapped_cluster_id = item.second;
        
        cluster_list_candidate[mapped_cluster_id].insert(obj_id);
    }
    
    /* Select cluster if number of elements it contains is equal or bigger than MinPts. */
    vector< set<int> > cluster_list;
    for(auto &data_set: cluster_list_candidate) {
        if(data_set.size() >= MinPts) {
            cluster_list.push_back(data_set);
        }
    }
    
    /* Sort cluster_list using cluster size in reverse order. */
    sort(cluster_list.rbegin(), cluster_list.rend(), [](set<int> &A, set<int> &B) {
        return A.size() < B.size();
    });
    
    /* Delete some clusters. */
    while(cluster_list.size() > num) {
        cluster_list.pop_back();
    }
    
    return cluster_list;
}

/* Just write result into output file. */
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
