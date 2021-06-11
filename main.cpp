#include "graphColor.h";
#include <vector>;
#include <iostream>;
#include <chrono>;
#include <thread>;
using namespace std;
void main() {
	vector<vector<int>> graph;
	vector<int> v0 = {1,2,3};
	vector<int> v1 = {2,3,4,0 };
	vector<int> v2 = { 3,4,0,1};
	vector<int> v3 = { 0,1,2,4 };
	vector<int> v4 = { 0,1,2 };
	
	graph.push_back(v0);
	graph.push_back(v1);
	graph.push_back(v2);
	graph.push_back(v3);
	graph.push_back(v4);
	vector<int> colors;
	bool success = graphColor::graphColor(colors, graph, 5,true);
	cout << success;
	for (auto c : colors) {
		cout << c;
	}
	std::this_thread::sleep_for(chrono::milliseconds(100000));
}