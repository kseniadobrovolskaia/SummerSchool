#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <exception>
#include <assert.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <set>
#include <cstring>
#include <string>
#include <limits>


class GraphNode
{
    bool IsVisit = false;
    unsigned Idx;
    std::shared_ptr<GraphNode> IDom;
    std::set<std::shared_ptr<GraphNode>> Dominators;
    std::set<std::shared_ptr<GraphNode>> Predecessors;
    std::set<std::shared_ptr<GraphNode>> Children;
    std::set<std::shared_ptr<GraphNode>> DomFrontiers;

public:
	GraphNode() {};
	GraphNode(unsigned Id, std::set<std::shared_ptr<GraphNode>> Ch) 
	: Idx(Id), Children(Ch) {
	};

	std::set<std::shared_ptr<GraphNode>> &getChildren() { return Children; }
	std::set<std::shared_ptr<GraphNode>> &getPredecessors() { return Predecessors; }
	std::set<std::shared_ptr<GraphNode>> &getDominators() { return Dominators; }
	std::set<std::shared_ptr<GraphNode>> &getDomFrontiers() { return DomFrontiers; }
	std::shared_ptr<GraphNode> getIDom() { return IDom; }
	unsigned getIdx() { return Idx; }
	bool getIsVisit() { return IsVisit; }
	void setIsVisit() { IsVisit = true; }

	void setIDom(std::shared_ptr<GraphNode> ID) { IDom = ID; }

	void addPredecessor(std::shared_ptr<GraphNode> Pred) { Predecessors.insert(Pred); }
	std::set<std::shared_ptr<GraphNode>> intersectDominatorsOfPredecessors()
	{
		std::set<std::shared_ptr<GraphNode>> Intersection;
		if (Predecessors.empty())
			return Intersection;

		Intersection = (*Predecessors.begin())->getDominators();

		for (auto Pred : Predecessors)
		{
			std::erase_if(Intersection, [&](std::shared_ptr<GraphNode> Elem) {
											return !(Pred->getDominators()).contains(Elem);});
		}

		return Intersection;
	}

	void getReversePostOrder(std::vector<unsigned> &RPO)
	{
		if (IsVisit)
			return;
		if (Children.empty())
		{
			IsVisit = true;
			RPO.push_back(Idx);
			return;
		}
		IsVisit = true;
		for (auto Child : Children)
		{
			Child->getReversePostOrder(RPO);
		}
		RPO.push_back(Idx);
	}

	void resetIsVisit()
	{
		if (!IsVisit)
			return;
		if (Children.empty())
		{
			IsVisit = false;
			return;
		}
		IsVisit = false;
		for (auto Child : Children)
		{
			Child->resetIsVisit();
		}
	}
		

	unsigned containsChildren(unsigned Elem, unsigned *Level)
	{
		if (Children.empty())
			return 0;
		
		for (auto Child : Children)
		{
			if (Child->getIdx() < Idx)
				continue;
			if (Child->getIdx() == Elem)
			{
				(*Level)++;
				return 1;
			}
			(*Level)++;
			if (Child->containsChildren(Elem, Level))
				return 1;
		}

		(*Level)--;
		return 0;
	}

	unsigned getNearestDom()
	{
		unsigned Nearest;
		unsigned MinDist = std::numeric_limits<unsigned>::max(), Dist;
		for (auto Dom : Dominators)
		{
			if (Dom->getIdx() == Idx)
				continue;
			Dist = 0;
			Dom->containsChildren(Idx, &Dist);
			if (Dist < MinDist)
			{
				MinDist = Dist;
				Nearest = Dom->getIdx();
			}
		}
		return Nearest;
	}

	void print()
	{
		std::cout << "\n\nNode " << Idx << " :";
		std::cout << "\nPredecessors: \n";
		for (auto Elem : Predecessors)
			std::cout << Elem->getIdx() << " ";
		std::cout << "\nDominators: \n";
		for (auto Elem : Dominators)
			std::cout << Elem << " ";
		std::cout << "\nChildren: \n";
		for (auto Elem : Children)
			std::cout << Elem->getIdx() << " ";
		std::cout << "\nDomFrontiers: \n";
		for (auto Elem : DomFrontiers)
			std::cout << Elem->getIdx() << " ";
		std::cout << "\nIDom : " << (IDom ? IDom->getIdx() : 0) << "\n";	
	}
};


class Graph
{
	std::unordered_map<unsigned, std::shared_ptr<GraphNode>> GraphMap;

	void createNodes(std::ofstream &File)
	{
		if (!(File.is_open()))
		{
			throw std::logic_error("File \"graph.txt\" did not open");
		}

		std::vector<std::string> Colors = { "lightcyan2", "azure", "powderblue", "pink", "violet"};
		unsigned CountColors = Colors.size();
		
		for (auto Elem : GraphMap)
		{
			unsigned NumNode = Elem.first;
			File << "\n node_" << NumNode 
			<< "[label = " << NumNode << ", style=\"filled\", shape=\"record\", fillcolor = \"" 
			<< Colors[NumNode % CountColors] << "\"];";

			for (auto Child : GraphMap[NumNode]->getChildren())
			{
				File << "\n node_" << NumNode << "  -> node_" << Child->getIdx() << ";\n";
			}
		}
	}
public:
	Graph() {};

	std::unordered_map<unsigned, std::shared_ptr<GraphNode>> &getGraphMap() { return GraphMap; }
	
	void checkNode(unsigned Elem)
	{
		if (GraphMap.contains(Elem))
			return;
		GraphMap[Elem] = std::make_shared<GraphNode>(Elem, std::set<std::shared_ptr<GraphNode>>());
	}

	void addNode(unsigned NumNode, std::vector<unsigned> Childrens)
	{
		std::set<std::shared_ptr<GraphNode>> Ch;
		std::shared_ptr<GraphNode> CurrNode;

		checkNode(NumNode);
		for (auto Elem : Childrens)
		{
			checkNode(Elem);
			Ch.insert(GraphMap[Elem]);
		}

		GraphMap[NumNode]->getChildren().merge(Ch);
		
		for (auto Elem : GraphMap[NumNode]->getChildren())
			Elem->getPredecessors().insert(GraphMap[NumNode]);
	}

	void print(std::string NamePng) 
	{
		std::ofstream File;

		File.open("graph.txt");

		if (!(File.is_open()))
		{
			throw std::logic_error("File \"graph.txt\" did not open");
		}

		File << "digraph G{\n";
		createNodes(File);

		File << "}";
		File.close();

		std::stringstream Command;
		Command << "dot -Tpng graph.txt -o " << NamePng;
		system((Command.str()).c_str());

		std::stringstream Command2;
		Command2 << "xdg-open "<< NamePng;
		system((Command2.str()).c_str());
	}

	void printAllGraph(std::string Name)
	{
		std::cout << "\n" << Name << ": \n\n";
		for (auto Node : GraphMap)
			Node.second->print();
	}

	std::vector<unsigned> getReversePostOrder()
	{
		std::vector<unsigned> PO;
		PO.reserve(GraphMap.size());
		GraphMap[0]->getReversePostOrder(PO);
		std::vector<unsigned> RPO(PO.size());

		for (unsigned Num = 0; Num < PO.size(); Num++)
		{
			RPO[PO[Num]] = Num;
		}
		unsigned CountNodes = PO.size() - 1;
		std::for_each(RPO.begin(), RPO.end(), [CountNodes](unsigned &Elem) { Elem = CountNodes - Elem; });
		GraphMap[0]->resetIsVisit();

		return RPO;
	}

	Graph getDomTree()
	{
		std::vector<std::set<std::shared_ptr<GraphNode>>> PrevDoms(GraphMap.size()), CurrDoms(GraphMap.size());
		std::vector<unsigned> RPO = getReversePostOrder();
		unsigned CountNodes = GraphMap.size();

		PrevDoms[0].insert(GraphMap[0]);
		GraphMap[0]->getDominators().insert(GraphMap[0]);
		for (unsigned NumNode = 0; NumNode < CountNodes; NumNode++)
			PrevDoms[1].insert(GraphMap[NumNode]);
		GraphMap[1]->getDominators() = PrevDoms[1];
		
		for (unsigned NumNode = 2; NumNode < CountNodes; NumNode++)
		{
			PrevDoms[NumNode]= PrevDoms[1];
			GraphMap[NumNode]->getDominators() = PrevDoms[1];
		}
		do
		{
			for (auto NodeIt = RPO.begin(); NodeIt < RPO.end(); NodeIt++)
			{
				unsigned NumNode = *NodeIt;
				PrevDoms = CurrDoms;

				std::set<std::shared_ptr<GraphNode>> Dominators;
				std::shared_ptr<GraphNode> Node = GraphMap[NumNode];

				Dominators = Node->intersectDominatorsOfPredecessors();
				Dominators.insert(Node);
				Node->getDominators() = Dominators;
				CurrDoms[Node->getIdx()] = Dominators;
			}
			
		} while (PrevDoms != CurrDoms);

		Graph DomTree;
		for (unsigned NumNode = 1; NumNode < CountNodes; NumNode++)
		{
			unsigned Nearest = GraphMap[NumNode]->getNearestDom();
			GraphMap[NumNode]->setIDom(GraphMap[Nearest]);
			DomTree.addConnection(Nearest, NumNode);
		}
		return DomTree;
	}

	Graph getDJ()
	{
		Graph DJ = getDomTree();

		for (auto NodePair : GraphMap)
		{
			unsigned NumNode = NodePair.first;
			std::shared_ptr<GraphNode> Node = NodePair.second;

			if (Node->getPredecessors().size() > 1)
			{
				for (auto Pred : Node->getPredecessors())
					DJ.addConnection(Pred->getIdx(), NumNode);
			}
		}
		return DJ;
	}

	Graph getDF()
	{
		unsigned CountNodes = GraphMap.size();
		for (unsigned NumNode = 0; NumNode < CountNodes; NumNode++)
		{
			for (auto Pred : GraphMap[NumNode]->getPredecessors())
			{
				unsigned R = Pred->getIdx();
				while (R != GraphMap[NumNode]->getIDom()->getIdx())
				{
					GraphMap[R]->getDomFrontiers().insert(GraphMap[NumNode]);
					if (R == 0)
						break;
					R = GraphMap[R]->getIDom()->getIdx();
				}
			}
		}
		Graph DomFrontier;
		for (unsigned NumNode = 0; NumNode < CountNodes; NumNode++)
		{
			for (auto DF : GraphMap[NumNode]->getDomFrontiers())
				DomFrontier.addConnection(GraphMap[NumNode]->getIdx(), DF->getIdx());
		}
		return DomFrontier;
	}

	Graph getIDF()
	{
		
		std::vector<unsigned> RPO = getReversePostOrder();
		Graph IDF = getDF();
		std::vector<std::vector<unsigned>> Remembered(GraphMap.size());
		
		for (auto NodeIt = RPO.begin(); NodeIt < RPO.end(); NodeIt++)
		{
			unsigned NumNode = *NodeIt;
			if (!(IDF.GraphMap).contains(NumNode))
				continue;
			std::shared_ptr<GraphNode> Node = IDF.GraphMap[NumNode];

			for (auto Child : Node->getChildren())
			{
				Remembered[Child->getIdx()].push_back(NumNode);
				for (auto Rem : Remembered[NumNode])
					IDF.addConnection(Rem, Child->getIdx());
			}
		}
		return IDF;
	}

	void addConnection(unsigned First, unsigned Second)
	{
		checkNode(First);
		checkNode(Second);
		
		GraphMap[First]->getChildren().insert(GraphMap[Second]);
		GraphMap[Second]->getPredecessors().insert(GraphMap[First]);
	}

	void deleteConnection(std::shared_ptr<GraphNode> First, std::shared_ptr<GraphNode> Last)
	{
		std::set<std::shared_ptr<GraphNode>> &Pr = First->getChildren();
		auto it = std::find_if(Pr.begin(), Pr.end(), [Last](std::shared_ptr<GraphNode> Node){ return Node == Last;});
		Pr.erase(it);
	}
};


#endif
