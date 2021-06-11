/*
color a graph
expects a graph in the form vector<vector<int>> of vertices' neighbors
usage: 
vector<vector<int>> graph; //your graph here
vector<int> colors; //to be overwritten with coloring
int numColors = 4;
bool success = graphColor::graphColor(colors, graph, numColors);
*/

#pragma once
#include <vector>
#include <string>
#include <algorithm>
namespace graphColor {
	struct node;
	struct columnHeader;
	struct intNode;
	struct intColumnHeader;
	struct vertex {
		std::vector<int> neighbors;
		int id;

	};

	struct node;
	struct columnHeader;
	struct intNode;
	struct intColumnHeader;
	//nodes link to their neighbors and to their column header
	struct node {
		node* L;
		node* R;
		node* U;
		node* D;
		columnHeader* C;
		std::string name;

	};
	//column headers link to their neighbors and also contain their own size and name
	struct columnHeader {
		columnHeader* L;
		columnHeader* R;
		node* U;
		node* D;
		std::string name;
		int size;
	};

	//variables for dancing links
	std::vector<node*> nodes;
	std::vector<columnHeader*> columnHeaders;
	std::vector<std::string> solution;




	//create a circular double linked list of column headers
	void setupColumns(std::vector<std::string> columnNames) {
		columnHeaders.clear();
		nodes.clear();
		solution.clear();
		columnHeader* root = new columnHeader;
		std::string blah = "-1";
		root->name = blah;
		root->size = 0;
		root->L = root;
		root->R = root;
		columnHeaders.push_back(root);
		columnHeader* lastPointer = root;
		for (std::string name : columnNames) {
			columnHeader* newColumnPointer = new columnHeader;
			newColumnPointer->name = name;
			newColumnPointer->size = 0;
			newColumnPointer->L = lastPointer;
			newColumnPointer->R = root;
			lastPointer->R = newColumnPointer;
			lastPointer = newColumnPointer;
			columnHeaders.push_back(newColumnPointer);
		}
	}
	//add a row to the column matrix
	void addRow(std::vector<std::string> newRow, std::string name) {
		std::vector<node*> newNodes;
		for (int i = 0; i < newRow.size(); ++i) {
			node* newNode = new node;
			newNode->name = name;

			newNodes.push_back(newNode);

		}
		newNodes[0]->L = newNodes[newNodes.size() - 1];
		newNodes[newNodes.size() - 1]->R = newNodes[0];
		for (int i = 0; i < newRow.size(); ++i) {
			if (i > 0) newNodes[i]->L = newNodes[i - 1];
			if (i < newRow.size() - 1) newNodes[i]->R = newNodes[i + 1];
		}
		for (int i = 0; i < newRow.size(); ++i) {
			node* nodePointer = newNodes[i];
			std::string column = newRow[i];
			for (int j = 0; j < columnHeaders.size(); ++j) {
				columnHeader* columnPointer = columnHeaders[j];
				if (columnPointer->name == column) {
					nodePointer->C = columnPointer;
					if (columnPointer->size == 0) {
						columnPointer->U = nodePointer;
						columnPointer->D = nodePointer;
						nodePointer->U = nodePointer;
						nodePointer->D = nodePointer;

					}
					else {

						nodePointer->U = columnPointer->U;
						nodePointer->D = columnPointer->D;
						columnPointer->U->D = nodePointer;
						columnPointer->U = nodePointer;
						columnPointer->D->U = nodePointer;

					}
					newNodes[i] = nodePointer;
					columnPointer->size += 1;
					break;
				}
			}
		}
		for (node* n : newNodes) {
			nodes.push_back(n);
		}
	}


	//dancing links method
	void cover(columnHeader* columnPointer) {
		//cout << "covering column " << columnPointer->name << endl;
		columnPointer->R->L = columnPointer->L;
		columnPointer->L->R = columnPointer->R;
		node* columnNode = columnPointer->D;
		for (int i = 0; i < columnPointer->size; ++i) {
			node* rowNode = columnNode->R;
			//cout << "covering row " << rowNode->name << ": ";
			while (rowNode->C != columnPointer) {
				//cout << rowNode->C->name << ";";
				rowNode->U->D = rowNode->D;
				rowNode->D->U = rowNode->U;
				if (rowNode->C->D == rowNode) rowNode->C->D = rowNode->D;
				if (rowNode->C->U == rowNode) rowNode->C->U = rowNode->U;
				rowNode->C->size -= 1;
				rowNode = rowNode->R;
			}
			//cout << endl;
			columnNode = columnNode->D;
		}
	}
	//dancing links method
	void uncover(columnHeader* columnPointer) {

		columnPointer->R->L = columnPointer;
		columnPointer->L->R = columnPointer;
		node* columnNode = columnPointer->U;
		for (int i = 0; i < columnPointer->size; ++i) {
			node* rowNode = columnNode->L;
			while (rowNode->C != columnPointer) {
				rowNode->C->size += 1;
				if (rowNode->C->U == rowNode->U) rowNode->C->U = rowNode;
				if (rowNode->C->D == rowNode->D) rowNode->C->D = rowNode;
				rowNode->U->D = rowNode;
				rowNode->D->U = rowNode;
				rowNode = rowNode->L;
			}
			columnNode = columnNode->U;
		}
	}
	//solves exact cover problem
	bool dancingLinks(int solutionSize) {
		columnHeader* h = columnHeaders[0];


		if ((h->R == h) || solution.size() == solutionSize) { //we should never need the second condition. there is a bug.

			return true;

		}
		int minSize = 10000000;
		int actualMin = 100000000;
		columnHeader* best = h->R;
		columnHeader* c = best;
		while (c != h) {
			int cSize = c->size;
			if (cSize < actualMin) {
				actualMin = cSize;
			}
			if (c->name[0] == 'n') {
				if (cSize < minSize) {
					best = c;
					minSize = best->size;
				}
			}
			c = c->R;
		}
		c = best;
		if (actualMin == 0) {
			return false;
		}
		cover(c);
		node* rowToChoose = c->D;
		for (int i = 0; i < c->size; ++i) {
			solution.push_back(rowToChoose->name);
			node* toCover = rowToChoose->R;
			while (rowToChoose != toCover) {
				cover(toCover->C);
				toCover = toCover->R;
			}
			if (dancingLinks(solutionSize)) return true;

			solution.pop_back();
			node* toUncover = rowToChoose->L;
			while (rowToChoose != toUncover) {
				uncover(toUncover->C);
				toUncover = toUncover->L;
			}
			rowToChoose = rowToChoose->D;
		}
		uncover(c);
		return false;

	}

	//solve exact cover maximizing color variety
	std::vector<int> colorCount;
	bool varietyDancingLinks(int solutionSize) {
		columnHeader* h = columnHeaders[0];


		if ((h->R == h) || solution.size() == solutionSize) { //we should never need the second condition. there is a bug.

			return true;

		}
		int minSize = 10000000;
		int actualMin = 10000000;
		columnHeader* best = h->R;
		columnHeader* c = best;

		while (c != h) {
			int cSize = c->size;
			if (cSize < actualMin) {
				actualMin = cSize;
			}
			if (c->name[0] == 'n') {
				if (cSize < minSize) {
					best = c;
					minSize = best->size;
				}
			}
			c = c->R;
		}
		c = best;
		if (actualMin == 0) {
			return false;
		}
		cover(c);
		node* rowToChoose = c->D;
		std::vector<std::pair<int,node*>> zip;
		for (int i = 0; i < c->size; ++i) {
			std::string rowName = rowToChoose->name;
			int position = rowName.find('n');
			int comma = rowName.find(',');
			int color = std::stoi(rowName.substr(5, comma - 5));
			zip.push_back(std::make_pair(colorCount[color], rowToChoose));
			rowToChoose = rowToChoose->D;
		}
		std::sort(std::begin(zip), std::end(zip),
			[&](const auto& a, const auto& b)
			{
				return a.first < b.first;
			});

		for (auto rPair : zip) {
			node* rowToChoose = rPair.second;
			std::string rowName = rowToChoose->name;
			int position = rowName.find('n');
			int comma = rowName.find(',');
			int color = std::stoi(rowName.substr(5, comma - 5));
			colorCount[color] += 1;
			solution.push_back(rowToChoose->name);
			node* toCover = rowToChoose->R;
			while (rowToChoose != toCover) {
				cover(toCover->C);
				toCover = toCover->R;
			}
			if (varietyDancingLinks(solutionSize)) return true;

			solution.pop_back();
			colorCount[color] -= 1;
			node* toUncover = rowToChoose->L;
			while (rowToChoose != toUncover) {
				uncover(toUncover->C);
				toUncover = toUncover->L;
			}
			rowToChoose = rowToChoose->D;
		}
		uncover(c);
		return false;

	}


	std::vector<int> extractColors(std::vector<std::string> solution) {
		std::vector<int> colors(solution.size(), 0);
		for (std::string rowName : solution) {
			int position = rowName.find('n');
			if (position != std::string::npos) {
				int comma = rowName.find(',');
				int color = std::stoi(rowName.substr(5, comma - 5));
				int node = stoi(rowName.substr(comma + 1, std::string::npos));
				colors[node] = color;
			}
		}
		return colors;
	}


	bool graphColor(std::vector<int>& colors, std::vector<std::vector<int>> graph, int numColors = 4,bool maxVariety = false) {
		std::vector<int> coloring;
		std::vector<std::string> columns;
		std::vector<std::string> rowNames;
		std::vector<std::vector<std::string>> rows;
		for (int color = 0; color < numColors; ++color) {
			for (int i = 0; i < graph.size(); ++i) {
				std::vector<int> neighbors = graph[i];
				for (int neighbor : neighbors) {
					std::string columnName = "edgecolor:" + std::to_string(color) + "," + std::to_string(std::min(i, neighbor)) + "," + std::to_string(std::max(i, neighbor));
					if (find(columns.begin(), columns.end(), columnName) == columns.end()) {
						rowNames.push_back(columnName);
						std::vector<std::string> newRow = { columnName };
						rows.push_back(newRow);
						columns.push_back(columnName);
					}
				}

			}
		}
		for (int i = 0; i < graph.size(); ++i) {
			//singleton nodes
			std::string columnName = "node:" + std::to_string(i);
			columns.push_back(columnName);
		}



		for (int color = 0; color < numColors; ++color) {
			for (int i = 0; i < graph.size(); ++i) {

				std::vector<std::string> newRow;
				std::vector<int> neighbors = graph[i];
				newRow.push_back("node:" + std::to_string(i));
				for (auto neighbor : neighbors) {
					newRow.push_back("edgecolor:" + std::to_string(color) + "," + std::to_string(std::min(i, neighbor)) + "," + std::to_string(std::max(i, neighbor)));
				}

				rowNames.push_back("node:" + std::to_string(color) + "," + std::to_string(i));
				rows.push_back(newRow);
			}
		}
		setupColumns(columns);
		for (int i = 0; i < rowNames.size(); ++i) {
			addRow(rows[i], rowNames[i]);
		}
		bool tried;
		if (!maxVariety) {
			tried = dancingLinks(graph.size());
		}
		else {
			for (int i = 0; i < numColors; i++) {
				colorCount.push_back(0);
			}
			tried = varietyDancingLinks(graph.size());

		}
		colors = extractColors(solution);
		return tried;
	}
}