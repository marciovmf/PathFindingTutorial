#include <SDL.h>
#include "types.h"
#include "NavigationNode.h"
#include "NavigationGrid.h"
#include "Pathfinder.h"
#include <cstdio>

// SDL.h inclui SDLMain.h que #define main. Por isso tenho que #undef isso antes de declarar o main
#ifdef main
#undef main
#endif

int main(int argc, char** argv)
{
	const uint16 gridHNodes = 10;
	const uint16 gridVNodes = 10;
	const uint16 tileSize = 64;
	const uint16 windowWidth = gridHNodes * tileSize;
	const uint16 windowHeight = gridVNodes * tileSize;

	// Start and end nodes for pathfinding
	NavigationNode* startNode = nullptr;
	NavigationNode* endNode = nullptr;

	// SDL initializations
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error initializing SDL", SDL_GetError(), 0);
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Pathfinding tutorial", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (!window)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating window", SDL_GetError(), 0);
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating renderer", SDL_GetError(), 0);
		return 1;
	}

	// Load the atlas bitmap
	const char *assetFile = "tile.bmp";
	SDL_Surface* bitmap = SDL_LoadBMP(assetFile);
	if (!bitmap)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error loading asset", "Could not load tile.bmp", 0);
		return 1;
	}

	// Upload the bitmap to the GPU
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bitmap);
	SDL_FreeSurface(bitmap);
	if (!texture)
	{
		SDL_ShowSimpleMessageBox(0, "Fatal Error creating texture", SDL_GetError(), 0);
		return 5;
	}

	// Sprites
	SDL_Rect grassSprite;
	grassSprite.x = 128;
	grassSprite.y = 0;
	grassSprite.w = grassSprite.h = 128;

	SDL_Rect waterSprite;
	waterSprite.x = 0;
	waterSprite.y = 0;
	waterSprite.w = waterSprite.h = 128;

	SDL_Rect flagSprite;
	flagSprite.x = 0;
	flagSprite.y = 128;
	flagSprite.w = flagSprite.h = 128;

	SDL_Rect cursorSprite;
	cursorSprite.x = 128;
	cursorSprite.y = 128;
	cursorSprite.w = cursorSprite.h = 128;

	// Initialize the navigation grid
	NavigationGrid *grid = new NavigationGrid(gridHNodes, gridVNodes);

	// tracks the cursor position
	SDL_Rect cursor = {};
	cursor.w = cursor.h = tileSize;

	uint32 numGridNodes = gridHNodes * gridVNodes;

	PathFinder pathFinder(numGridNodes);
	Path path = {};

	// Render loop
	bool running = true;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	while (running)
	{
		SDL_Event event;

		// Proccess input
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:
				case SDL_SCANCODE_W:
					cursor.y = MAX(0, cursor.y - tileSize);
					break;

				case SDL_SCANCODE_LEFT:
				case SDL_SCANCODE_A:
					cursor.x = MAX(0, cursor.x - tileSize);
					break;

				case SDL_SCANCODE_DOWN:
				case SDL_SCANCODE_S:
					cursor.y = MIN(windowHeight - tileSize, cursor.y + tileSize);
					break;

				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
					cursor.x = MIN(windowWidth - tileSize, cursor.x + tileSize);
					break;

				case SDL_SCANCODE_ESCAPE:
					running = false;
					break;

					// Toggle Walkable/Unwalkable
				case SDL_SCANCODE_1:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					node->blocked = !node->blocked;

					// if the start or end nodes became unwalkable, reset it
					if (node->blocked)
					{
						if (node == startNode)
							startNode = nullptr;
						if (node == endNode)
							endNode = nullptr;
					}
					path.status = Path::UNPROCCESSED;
				}break;

				// Set START node
				case SDL_SCANCODE_2:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					if (node->blocked) node->blocked = false;
					startNode = node;
					if (endNode == startNode)
						endNode = nullptr;

					path.status = Path::UNPROCCESSED;
				}break;

				// Set END node
				case SDL_SCANCODE_3:
				{
					NavigationNode* node = grid->getNodeAt(cursor.x / tileSize, cursor.y / tileSize);
					if (node->blocked) node->blocked = false;
					endNode = node;
					if (startNode == endNode)
						startNode = nullptr;
					path.status = Path::UNPROCCESSED;

				}break;
				}
			}
		}

		// Render the navigation grid
		SDL_Rect dstRect = {};
		dstRect.w = dstRect.h = tileSize;
		NavigationNode* node = grid->nodes;

		for (uint32 i = 0; i < numGridNodes; i++)
		{
			SDL_Rect *r = (node->blocked ? &waterSprite : &grassSprite);
			SDL_RenderCopy(renderer,
				texture,
				r,
				&dstRect);

			// Render Start and End nodes
			if (node == startNode)
				SDL_RenderCopy(renderer, texture, &flagSprite, &dstRect);
			else if (node == endNode)
				SDL_RenderCopyEx(renderer, texture, &flagSprite, &dstRect, 0, 0, SDL_FLIP_HORIZONTAL);

			// Adjust line and column coords for the node
			dstRect.x += tileSize;
			if (dstRect.x >= windowWidth)
			{
				dstRect.x = 0;
				dstRect.y += tileSize;
			}
			++node;
		}

		// Render the cursor
		SDL_RenderCopy(renderer, texture, &cursorSprite, &cursor);

		// Render the path
		if (path.status == Path::FOUND)
		{
			NavigationNode* current = path.end;
			while (current != nullptr)
			{
				SDL_Rect r;
				r.w = r.h = tileSize;
				r.x = (current->x * tileSize);
				r.y = (current->y * tileSize);
				SDL_RenderDrawRect(renderer, &r);
				current = current->parent;
			}
		}

		// Pathfind
		if (path.status == Path::UNPROCCESSED && startNode != 0 && endNode != 0)
		{
			path = pathFinder.FindPath(startNode, endNode);
		}

		// Flush
		SDL_RenderPresent(renderer);
	}

	SDL_Quit();
	return 0;
#undef SDLFATALERROR
}

#ifdef WIN32
#include <Windows.h>
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	char* argv = "game";
	return main(1, &argv);
}
#endif