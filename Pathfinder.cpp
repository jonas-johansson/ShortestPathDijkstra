#include "stdafx.h"

using namespace std;

class Graph
{
private:
	struct Node;

	typedef shared_ptr<Node> NodePtr;

	struct Connection
	{
		NodePtr node;
		float traversalCost;

		Connection(NodePtr& _node, float _traversalCost)
			: node(_node), traversalCost(_traversalCost)
		{
		}
	};

	struct Node
	{
		vector<Connection> connections;
		bool visited;
		float tentativeCost;
		NodePtr prev;
	};

	typedef vector<NodePtr> Nodes;
	Nodes m_allNodes;

	NodePtr GetUnvisitedNodeWithLowestTentativeCost()
	{
		auto cheapestNode = NodePtr();
		auto cheapestCost = numeric_limits<float>::infinity();
		
		for (auto& node : m_allNodes)
		{
			if (!node->visited && node->tentativeCost < cheapestCost)
			{
				cheapestNode = node;
				cheapestCost = node->tentativeCost;
			}
		}

		return cheapestNode;
	}
	
public:
	NodePtr CreateNode()
	{
		auto node = make_shared<Node>();
		m_allNodes.push_back(node);
		return node;
	}

	void Connect(NodePtr node, NodePtr neighbor, float traversalCost)
	{
		node->connections.push_back(Connection(neighbor, traversalCost));
	}

	// Find a path using Dijkstra's algorithm
	Nodes FindPath(NodePtr startNode, NodePtr endNode)
	{
		// Prepare
		for (auto& node : m_allNodes)
		{
			node->visited = false;
			node->tentativeCost = (node == startNode) ? 0.0f : numeric_limits<float>::infinity();
		}

		// Explore the graph
		auto currentNode = startNode;
		while (currentNode)
		{
			currentNode->visited = true;

			// Go through connected unvisited nodes, and see if we can
			// provide the cheapest path to them. If we do, record it.
			for (auto& connection : currentNode->connections)
			{
				auto& connectedNode = connection.node;
				if (!connectedNode->visited)
				{
					// Calculate the cost of going from the start, through the current node,
					// to the connected node. If the cost of this path is lower than that of
					// the current one, we redirect.
					auto cost = currentNode->tentativeCost + connection.traversalCost;
					if (cost < connectedNode->tentativeCost)
					{
						connectedNode->tentativeCost = cost;
						connectedNode->prev = currentNode;
					}
				}
			}

			bool reachedEnd = (currentNode == endNode);
			if (reachedEnd)
			{
				// Generate the path by traversing the chain
				// from end to start through the links
				Nodes path;
				for (auto node = endNode; node != startNode; node = node->prev)
				{
					path.push_back(node);
				}
				path.push_back(startNode);
				reverse(path.begin(), path.end());
				return path;
			}

			currentNode = GetUnvisitedNodeWithLowestTentativeCost();
		}

		return Nodes();
	}
};

void RunPathfinderTests()
{
	// No path found when there are no connections
	{
		Graph graph;

		auto nodeA = graph.CreateNode();
		auto nodeB = graph.CreateNode();

		auto path = graph.FindPath(nodeA, nodeB);

		assert(path.empty());
	}

	// Two-step path found
	{
		Graph graph;
		
		auto nodeA = graph.CreateNode();
		auto nodeB = graph.CreateNode();
		
		graph.Connect(nodeA, nodeB, 1.0f);
		
		auto path = graph.FindPath(nodeA, nodeB);
		
		assert(path.size() == 2);
		assert(path[0] == nodeA);
		assert(path[1] == nodeB);
	}

	// Three-step path found
	{
		Graph graph;

		auto nodeA = graph.CreateNode();
		auto nodeB = graph.CreateNode();
		auto nodeC = graph.CreateNode();

		graph.Connect(nodeA, nodeB, 1.0f);
		graph.Connect(nodeB, nodeC, 1.0f);

		auto path = graph.FindPath(nodeA, nodeC);

		assert(path.size() == 3);
		assert(path[0] == nodeA);
		assert(path[1] == nodeB);
		assert(path[2] == nodeC);
	}

	// Cheapest path taken #1
	{
		Graph graph;

		auto nodeA = graph.CreateNode();
		auto nodeB = graph.CreateNode();
		auto nodeC = graph.CreateNode();

		// Cheap path
		graph.Connect(nodeA, nodeB, 1.0f);
		graph.Connect(nodeB, nodeC, 1.0f);

		// Expensive path
		graph.Connect(nodeA, nodeC, 8.0f);

		auto path = graph.FindPath(nodeA, nodeC);

		assert(path.size() == 3);
		assert(path[0] == nodeA);
		assert(path[1] == nodeB);
		assert(path[2] == nodeC);
	}

	// Cheapest path taken #2
	{
		Graph graph;

		auto nodeA = graph.CreateNode();
		auto nodeB = graph.CreateNode();
		auto nodeC = graph.CreateNode();

		// Expensive path
		graph.Connect(nodeA, nodeB, 5.0f);
		graph.Connect(nodeB, nodeC, 5.0f);

		// Cheap path
		graph.Connect(nodeA, nodeC, 2.0f);

		auto path = graph.FindPath(nodeA, nodeC);

		assert(path.size() == 2);
		assert(path[0] == nodeA);
		assert(path[1] == nodeC);
	}
}

int main(int argc, char** argv)
{
	RunPathfinderTests();
	return 0;
}

