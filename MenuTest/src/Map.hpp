#pragma once

#ifndef Map_H
#define Map_H

#include <SDL.h>

#include <string>

#include "../App_Window.hpp"
#include "../src/Game.hpp"
#include "../src/Texture.hpp"

class MapTile
{
protected:

	Uint16 m_id;

public:

	MapTile();
	~MapTile();

	void SetId(Uint16 num);
	Uint16 GetId();

	bool render(SDL_Renderer* renderer, SDL_Point leftTopPoint);

};

class Map 
{

protected:

	MapTile** m_map;

	Uint16 m_mapWidth;
	Uint16 m_mapHeight;

	Uint16 m_tileWidth;
	Uint16 m_tileHeight;

	Uint16 m_mapSizeWidth;
	Uint16 m_mapSizeHeight;

	bool m_isInitialized;

	Texture* m_mapTexture;
	SDL_Rect* m_MapTextureRenderQuad;

	bool first;
	int firstX;
	int firstY;
	int firstTextureX;
	int firstTextureY;

	std::string m_lastError;

public:

	Map(Uint16 mapWidth, Uint16 mapHeight);
	~Map();

	bool init();
	bool render(SDL_Renderer* renderer);

	void renderMapTexture(SDL_Renderer* renderer);

	void handleEvent(SDL_Event* e);

	bool isInitialized();

	std::string GetLastError();

};

#endif

MapTile::MapTile()
{
	m_id = 0;
}

MapTile::~MapTile()
{

}

void MapTile::SetId(Uint16 num)
{
	m_id = num;
}

Uint16 MapTile::GetId()
{
	return m_id;
}

bool MapTile::render(SDL_Renderer* renderer, SDL_Point leftTopPoint)
{
	Uint16 xMultiplier = 30;
	Uint16 yMultiplier = xMultiplier / 2;
	SDL_Point points[5];
	points[0] = SDL_Point{ leftTopPoint.x, leftTopPoint.y };
	points[1] = SDL_Point{ leftTopPoint.x + 1 * xMultiplier, leftTopPoint.y - 1 * yMultiplier };
	points[2] = SDL_Point{ leftTopPoint.x + 2 * xMultiplier, leftTopPoint.y };
	points[3] = SDL_Point{ leftTopPoint.x + 1 * xMultiplier, leftTopPoint.y + 1 * yMultiplier };
	points[4] = SDL_Point{ leftTopPoint.x, leftTopPoint.y };

	return SDL_RenderDrawLines(renderer, points, 5);
}

Map::Map(Uint16 mapWidth, Uint16 mapHeight)
{
	m_map = nullptr;

	m_mapWidth = mapWidth;
	m_mapHeight = mapHeight;

	m_tileWidth = 30;
	m_tileHeight = m_tileWidth / 2;

	 m_mapSizeWidth= (m_mapWidth + m_mapHeight) * m_tileWidth;
	 m_mapSizeHeight= (m_mapWidth + m_mapHeight) * m_tileHeight;

	m_isInitialized = false;
}

Map::~Map()
{

}

bool Map::init()
{
	m_map = new MapTile * [m_mapHeight];

	for (Uint16 i = 0; i < m_mapHeight; i++)
	{
		m_map[i] = new MapTile[m_mapWidth];
	}

	for (Uint16 i = 0; i < m_mapHeight; i++)
	{
		for (Uint16 j = 0; j < m_mapWidth; j++)
		{
			m_map[i][j].SetId(0);
		}
	}

	//for (Uint16 i = 0; i < m_mapHeight; i++)
	//{
	//	for (Uint16 j = 0; j < m_mapWidth; j++)
	//	{
	//		printf("i = %d, j = %d, id = %d\n", i, j, m_map[i][j].GetId());
	//	}
	//}

	App_Window* window = App_Window::GetInstance();
	Uint16 wWidth = window->GetWidth();
	Uint16 wHeight = window->GetHeight();
	m_MapTextureRenderQuad = new SDL_Rect{ -m_mapSizeWidth/2+ wWidth/2,-m_mapSizeHeight/2+ wHeight/2, m_mapSizeWidth , m_mapSizeHeight };

	m_isInitialized = true;

	return true;
}

bool Map::render(SDL_Renderer* renderer)
{
	if (m_mapTexture == NULL)
	{
		printf("drawing");
		renderMapTexture(renderer);
	}

	SDL_Rect* clip = new SDL_Rect{ 0,0,  m_mapSizeWidth , m_mapSizeHeight };
	m_mapTexture->render(renderer, m_MapTextureRenderQuad, clip);

	return true;
}


void Map::renderMapTexture(SDL_Renderer* renderer)
{
	Uint16 firstX = 0;
	Uint16 firstY = m_mapSizeHeight / 2;

	SDL_Texture* target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_mapSizeWidth, m_mapSizeHeight);
	SDL_SetRenderTarget(renderer, target_texture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	for (Uint16 i = 0; i < m_mapHeight; i++)
	{
		for (Uint16 j = 0; j < m_mapWidth; j++)
		{
			SDL_Point point{ firstX + (j + i) * m_tileWidth, firstY - (j - i) * m_tileHeight };

			m_map[i][j].render(renderer, point);
		}
	}

	SDL_Point points[5];
	points[0] = SDL_Point{ 0, 0 };
	points[1] = SDL_Point{ m_mapSizeWidth - 1, 0 };
	points[2] = SDL_Point{ m_mapSizeWidth - 1, m_mapSizeHeight - 1 };
	points[3] = SDL_Point{ 0, m_mapSizeHeight - 1 };
	points[4] = SDL_Point{ 0, 0 };

	SDL_RenderDrawLines(renderer, points, 5);

	m_mapTexture = new Texture(target_texture);

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderPresent(renderer);
}

void Map::handleEvent(SDL_Event* e)
{
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		int x, y;
		Uint32 mouseState = SDL_GetMouseState(&x, &y);

		int tx, ty;
		tx = x - m_MapTextureRenderQuad->x;
		ty = y - m_MapTextureRenderQuad->y;

		printf("coords by texture x: %d, y: %d\n", tx, ty);

		if (mouseState == 1)
		{
			if (!first)
			{
				firstX = x;
				firstY = y;
				firstTextureX = m_MapTextureRenderQuad->x;
				firstTextureY = m_MapTextureRenderQuad->y;
				first = true;
			}
			if (e->type == SDL_MOUSEMOTION)
			{
				int newX, newY;
				newX = firstTextureX - (firstX - x);
				newY = firstTextureY - (firstY - y);

				//if (newY > -m_mapSizeHeight / 2 + 300)
				//{
				//	if (newY > m_mapSizeWidth / 2 + newX)
				//	{
				//		printf("popchik: x: %d, y: %d\n", m_mapSizeWidth / 2 + newX, newY);
				//	}
				//	//if ( newY > )
				//	/*if (newY < 800 - m_mapSizeWidth)
				//	{
				//		newX = 800 - m_mapSizeWidth;
				//	}*/
				//}
				//else
				//{
				//}
				
				

				//// screen width minus map size in px
				//if (newX < 400 - m_mapSizeWidth)
				//{
				//	newX = 400 - m_mapSizeWidth;
				//}
				//// screen height minus map size in px
				//if (newY < 600 - m_mapSizeHeight)
				//{
				//	newY = 600 - m_mapSizeHeight;
				//}
				//if (newX > 0)
				//{
				//	newX = 0;
				//}
				//if (newY > 0)
				//{
				//	newY = 0;
				//}

				m_MapTextureRenderQuad->x = newX;
				m_MapTextureRenderQuad->y = newY;

			}
			//backgroundRenderQuad->x = backgroundRenderQuad->x - (firstX  - x);
			//backgroundRenderQuad->y = backgroundRenderQuad->y - (firstY  - y);
		}
		else
		{
			first = false;
		}
	}

}


bool Map::isInitialized()
{
	return m_isInitialized;
}

std::string Map::GetLastError()
{
	return m_lastError;
}
