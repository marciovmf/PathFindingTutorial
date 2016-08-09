#ifndef NAVIGATION_GRID_H
#define NAVIGATION_GRID_H
class NavigationGrid
{
	uint16 width;
	uint16 height;

public:
	NavigationNode* nodes;

	NavigationGrid(uint16 width, uint16 height)
	{
		this->width = width;
		this->height = height;
		// Reserve memory for all nodes
		this->nodes = new NavigationNode[width * height * sizeof(NavigationNode)];
		// initialize the nodes
		for (uint16 y = 0; y < height; y++)
		{
			//void* row = nodes + (sizeof(NavigationNode) * width * y);
			NavigationNode* row = nodes + (width * y);
			for (uint16 x = 0; x < width; x++)
			{
				NavigationNode* nodePtr = row + x;
				nodePtr->x = x;
				nodePtr->y = y;
				nodePtr->blocked = false;
				nodePtr->neighborCount = 0;

				NavigationNode* neighbor = nullptr;
				NavigationNode *left = nullptr, *right = nullptr, *top = nullptr, *bottom = nullptr;

				// Calculate this node's neighbors

				// RIGHT
				if (x < width - 1){
					right = nodePtr + 1; nodePtr->addNeighbor(right);
				}

				// BOTTOM
				if (y < height - 1){
					bottom = nodePtr + width; nodePtr->addNeighbor(bottom);
				}


				// LEFT
				if (x > 0) {
					left = nodePtr - 1; nodePtr->addNeighbor(left);
				}

				// TOP
				if (y > 0) {
					top = nodePtr - width; nodePtr->addNeighbor(top);
				}



				// TOP RIGHT
				if (top && right){ nodePtr->addNeighbor(top + 1); }

				// BOTTOM RIGHT
				if (bottom && right){ nodePtr->addNeighbor(bottom + 1); }

				// BOTTOM LEFT
				if (bottom && left){ nodePtr->addNeighbor(bottom - 1); }

				// TOP LEFT
				if (top && left){ nodePtr->addNeighbor(top - 1); }


			}
		}
	}

	~NavigationGrid()
	{
		delete[] nodes;
	}

	inline NavigationNode* getNodeAt(uint16 x, uint16 y)
	{
		if (x > width || y > height || x < 0 || y < 0)
			return nullptr;
		return nodes + (y * width) + x;
	}
};
#endif