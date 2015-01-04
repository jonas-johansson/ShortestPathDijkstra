#include "stdafx.h"

using namespace std;

class Graph
{
private:
	struct Node;

	struct Connection
	{
		shared_ptr<Node> node;
		float traversalCost;

		Connection(shared_ptr<Node>& _node, float _cost)
			: node(_node), traversalCost(_cost)
		{
		}
	};

	struct Node
	{
		vector<Connection> connections;
		bool visited;
		float tentativeCost;
		shared_ptr<Node> link;
	};

	typedef vector<shared_ptr<Node>> Nodes;
	Nodes m_nodes;
	
public:
	shared_ptr<Node> CreateNode()
	{
		auto node = make_shared<Node>();
		m_nodes.push_back(node);
		return node;
	}

	void Connect(shared_ptr<Node> node, shared_ptr<Node> neighbor, float cost)
	{
		node->connections.push_back(Connection(neighbor, cost));
	}

	Nodes FindPath(shared_ptr<Node> start, shared_ptr<Node> end)
	{
		const float infinity = numeric_limits<float>::infinity();

		// Set initial tentative cost
		for (auto& node : m_nodes)
		{
			node->visited = false;

			if (node == start)
				node->tentativeCost = 0.0f;
			else
				node->tentativeCost = infinity;
		}

		auto currentNode = start;

		while (true)
		{
			currentNode->visited = true;

			// Go though unvisited neighbor nodes
			for (auto& connection : currentNode->connections)
			{
				if (!connection.node->visited)
				{
					// If we're a cheaper link to the neighbor node ...
					auto cost = currentNode->tentativeCost + connection.traversalCost;
					if (cost < connection.node->tentativeCost)
					{
						// ... record it
						connection.node->tentativeCost = cost;
						connection.node->link = currentNode;
					}
				}
			}

			// If we just visited the end node, we found a path
			if (currentNode == end)
			{
				// We're done, traverse the chain backwards
				// from end to start through the links
				Nodes path;
				for (auto node = end; node != start; node = node->link)
				{
					path.push_back(node);
				}
				path.push_back(start);
				reverse(path.begin(), path.end());
				return path;
			}

			// Pick the node with the cheapest tentative cost
			auto cheapestNode = shared_ptr<Node>();
			auto cheapestCost = infinity;
			for (auto& node : m_nodes)
			{
				if (!node->visited && node->tentativeCost < cheapestCost)
				{
					cheapestNode = node;
					cheapestCost = node->tentativeCost;
				}
			}

			if (cheapestNode)
				currentNode = cheapestNode;
			else
				break; // Destination can't be reached
		}

		return Nodes();
	}
};

void RunPathfinderTests()
{
	// No path found when there's no connection
	{
		Graph g;
		auto a = g.CreateNode();
		auto b = g.CreateNode();
		auto path = g.FindPath(a, b);
		assert(path.empty());
	}
	// Path found when there's a simple connection
	{
		Graph g;
		auto a = g.CreateNode();
		auto b = g.CreateNode();
		g.Connect(a, b, 1.0f);
		auto path = g.FindPath(a, b);
		assert(path.size() == 2);
		assert(path[0] == a);
		assert(path[1] == b);
	}
	// Path found when there's a complex connection
	{
		Graph g;
		auto a = g.CreateNode();
		auto b = g.CreateNode();
		auto c = g.CreateNode();
		g.Connect(a, b, 1.0f);
		g.Connect(b, c, 1.0f);
		auto path = g.FindPath(a, c);
		assert(path.size() == 3);
		assert(path[0] == a);
		assert(path[1] == b);
		assert(path[2] == c);
	}
	// Cheapest path taken #1
	{
		Graph g;
		auto a = g.CreateNode();
		auto b = g.CreateNode();
		auto c = g.CreateNode();

		// Route 1
		g.Connect(a, b, 1.0f);
		g.Connect(b, c, 1.0f);

		// Route 2
		g.Connect(a, c, 8.0f);

		auto path = g.FindPath(a, c);

		assert(path.size() == 3);
		assert(path[0] == a);
		assert(path[1] == b);
		assert(path[2] == c);
	}
	// Cheapest path taken #2
	{
		Graph g;
		auto a = g.CreateNode();
		auto b = g.CreateNode();
		auto c = g.CreateNode();

		// Route 1
		g.Connect(a, b, 5.0f);
		g.Connect(b, c, 5.0f);

		// Route 2
		g.Connect(a, c, 2.0f);

		auto path = g.FindPath(a, c);

		assert(path.size() == 2);
		assert(path[0] == a);
		assert(path[1] == c);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	RunPathfinderTests();
	return 0;
}

