#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <algorithm>
#include "types.h"
#include "NavigationNode.h"

struct Path
{
	NavigationNode* start;
	NavigationNode* end;
	enum {UNPROCCESSED, FOUND, IMPOSSIBLE} status;
};

#ifdef _DEBUG
#define assert(expression,msg) if (!(expression)) { std::cerr << "ASSERTION FAILED: " << msg << std::endl; *(int*)0=0;  }
#else
#define assert
#endif

class PathFinder
{
	std::vector<NavigationNode*> openList;
	std::vector<NavigationNode*> closedList;
	inline uint16 calculateHCost(NavigationNode *current, NavigationNode *goal) const
	{
		uint16 absX = ABS(current->x - goal->x);
		uint16 absY = ABS(current->y - goal->y);
		uint32 h;

		if (absX > absY)
			h = 14 * absY + 10 * (absX - absY);
		else
			h = 14 * absX + 10 * (absY - absX);

		return h;
	}

public:

	PathFinder(uint32 size = 100) : openList(size), closedList(size){ }

	Path FindPath(NavigationNode* start, NavigationNode* end)
	{
		Path path;
		path.start = start;
		path.end = end;
		path.status = Path::UNPROCCESSED;

		openList.clear();
		closedList.clear();

		// initialize the start node
		start->GCost = 0;
		start->HCost = calculateHCost(start, end);
		end->parent = start->parent = nullptr;

		openList.push_back(start);

		while (path.status == Path::UNPROCCESSED)
		{
			// Se a lista ABERTA estiver vazia, não pudemos encontrar um caminho.
			if (openList.size() == 0){ path.status = Path::IMPOSSIBLE; break; }

			// Ordena a lista aberta pelo menor FCost
			std::sort(openList.begin(), openList.end(),
				[](NavigationNode *a, NavigationNode *b)
			{
					return a->FCost < b->FCost; 
			});

			// Pega o nó com menor FCost da lista ABERTA
			NavigationNode* current = openList[0];

			// Remove o no atual da lista ABERTA
			openList.erase(openList.begin());

			// Adiciona na lista FECHADA
			closedList.push_back(current);

			// Se este for o nó destino, terminamos
			if (current == end){ path.status = Path::FOUND; break; }

			NavigationNode* blockedNode[8] = {};
			uint16 blockedNodeCount = 0;

			// Processa os nós filhos do nó atual
			for (uint16 neighborIndex = 0; neighborIndex < current->neighborCount; neighborIndex++)
			{

				NavigationNode* neighbor = current->neighbor[neighborIndex];

				// Ignora se este vizinho for um bloqueio
				if (neighbor->blocked)
				{
					blockedNode[blockedNodeCount++] = neighbor;
					continue;
				}

				// Ignora se este vizinho estiver na lista fechada
				if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end())
					continue;

				// Custo de movimento Ortogonal: 10; Diagonal: 14
				uint16 movementCost = 10;
				if (neighbor->x != current->x && neighbor->y != current->y)
				{
					movementCost = 14;
				}

				uint8 ignoreNode = false;
				for (uint16 i = 0; i < blockedNodeCount; i++)
				{
					NavigationNode* blocked = blockedNode[i];
					// Current na mesma linha do bloqueio
					if (current->x == blocked->x)
					{
						if (neighbor->x != blocked->x && neighbor->y == blocked->y)
						{
							ignoreNode = true;
							break;
						}
					}
					// Current na mesma coluna do bloqueio
					else if (current->y == blocked->y)
					{
						if (neighbor->y != blocked->y && neighbor->x == blocked->x)
						{
							ignoreNode = true;
							break;
						}
					}
				}

				if (ignoreNode)
					continue;

				if (std::find(openList.begin(), openList.end(), neighbor) != openList.end())
				{
					// Este no ja esta na lista aberta, podemos melhorar o caminho ate ele ?
					uint16 betterGCost = current->GCost + movementCost;
					if (betterGCost < neighbor->GCost)
					{
						neighbor->parent = current;
						neighbor->GCost = betterGCost;
						neighbor->FCost = neighbor->GCost + neighbor->HCost;
					}
				}
				else
				{
					neighbor->parent = current;
					neighbor->GCost = current->GCost + movementCost;
					neighbor->HCost = calculateHCost(neighbor, end);
					neighbor->FCost = neighbor->GCost + neighbor->HCost;
					openList.push_back(neighbor);
				}
			}
		}
		return path;
	}
};
#endif