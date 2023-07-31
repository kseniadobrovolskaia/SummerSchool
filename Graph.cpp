#include "Generate.hpp"


int main(int argc, char* argv[])
{
	bool DomTree = false, DF = false,
		 IDF     = false, DJ = false;
	
	int CountNodes = 10;

    for (int arg = 1; arg < argc; arg++)
    {
    	if (!strcmp(argv[arg], "DomTree"))
	    	DomTree = true;
    	else if (!strcmp(argv[arg], "DF"))
	    	DF = true;
    	else if (!strcmp(argv[arg], "IDF"))
	    	IDF = true;
    	else if (!strcmp(argv[arg], "DJ"))
	    	DJ = true;
	    else
	        CountNodes = std::stoi(argv[arg]);  	
    }
 
 	
 	// Graph from 1st lecture
	#if 0
	Gr.addNode(0, {1});
	Gr.addNode(1, {2, 5});
	Gr.addNode(2, {3});
	Gr.addNode(3, {4, 1});
	Gr.addNode(4, {});
	Gr.addNode(5, {6, 8});
	Gr.addNode(6, {7});
	Gr.addNode(7, {3});
	Gr.addNode(8, {7});
	#endif

	Graph Gr = generate(CountNodes, CountNodes - 1);
	Gr.print("Graph.png");

	Graph DomTreeGr = Gr.getDomTree();
	if (DomTree)
		DomTreeGr.print("DomTree.png");

	Graph DFGr = Gr.getDF();
	if (DF)
		DFGr.print("DF.png");

	Graph DJGr = Gr.getDJ();
	if (DJ)
		DJGr.print("DJ.png");

	Graph IDFGr = Gr.getIDF();
	if (IDF)	
		IDFGr.print("IDF.png");

	return 0;
}
