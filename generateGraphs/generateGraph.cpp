// C++ code to implement the approach

#include <bits/stdc++.h>
#include "generateGraph.hpp"

namespace generate {

using namespace std;


// Function to generate random graph
vector<Edge> Generator::randomGraph(int edgeCount, int vertexCount)
{
	std::vector<Edge> vec;

	int i, j, edge[edgeCount][2], count;
	i = 0;

	while (i < edgeCount) {
		edge[i][0] = rand() % vertexCount;
		edge[i][1] = rand() % vertexCount;

		if (edge[i][0] == edge[i][1])
			continue;
		else {
			for (j = 0; j < i; j++) {
				if ((edge[i][0] == edge[j][0]
					&& edge[i][1] == edge[j][1])
					|| (edge[i][0] == edge[j][1]
						&& edge[i][1] == edge[j][0]))
					i--;
			}
		}
		i++;
	}
	for (i = 0; i < vertexCount; i++) {

		count = 0;
		cout << "\t" << i << "-> { ";
		for (j = 0; j < edgeCount; j++) {
			if (edge[j][0] == i) {
				Edge e;
				e.source = i;
				e.target = edge[j][1];
				vec.push_back(e);

				cout << edge[j][1] << " ";
				count++;
			}
			else if (edge[j][1] == i) {
				Edge e;
				e.source = i;
				e.target = edge[j][0];
			//	vec.push_back(e);
				cout << edge[j][0] << " ";
				count++;
			}
			else if (j == edgeCount - 1 && count == 0)

				// Print “Isolated vertex”
				// for the vertex having
				// no degree.
				cout << "Isolated Vertex!";
		}

		cout << " }" << endl;
	}

	return vec;
}

}