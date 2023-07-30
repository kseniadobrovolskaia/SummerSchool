#ifndef GENERATE_H
#define GENERATE_H

#include "Graph.hpp"

Graph generate(unsigned CountNodes, unsigned Depth, unsigned MaxChilds = 3)
{
	assert(Depth < CountNodes && "CountNodes must be greater then Depth");
	Graph Result;
	std::vector<std::vector<unsigned>> Gr;

	for (unsigned NumStr = 0; NumStr < Depth; NumStr++)
	{
		std::vector<unsigned> Str(Depth, 0);
		if (CountNodes <= 0)
		{
			Str[NumStr + 1 + rand() % (Depth - NumStr)] = 1;
			Gr.push_back(Str);
			continue;
		}
		unsigned Cnt = 1 + rand() % MaxChilds;
		CountNodes -= Cnt;
		if (CountNodes < 0)
			Cnt = CountNodes + Cnt;
		for (unsigned Node = 0; Node < Cnt; Node++)
			Str[NumStr + 1 + rand() % (Depth - NumStr)] = 1;
		Gr.push_back(Str);	
	}

	Result.addConnection(0, 1);
	for (unsigned NumStr = 0; NumStr < Depth; NumStr++)
	{
		for (unsigned NumElem = NumStr + 1; NumElem < Depth; NumElem++)
			if ((Gr[NumStr])[NumElem])
				Result.addConnection(NumStr, NumElem);
	}
	for (auto Elem : Result.getGraphMap())
	{
		if (Elem.second->getPredecessors().empty())
		{
			if (Elem.first == 0)
				continue;
			Result.addConnection(Elem.first - 1, Elem.first);
		}
	}
	return Result;
}


#endif
