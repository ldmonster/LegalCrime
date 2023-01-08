#pragma once

#ifndef Gameplay_H
#define Gameplay_H

#include <SDL.h>

#include <string>

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

class Gameplay: public GameSection
{

protected:

	SDL_Renderer* m_renderer;

	MapTile** m_map;

	Uint16 m_mapWidth;
	Uint16 m_mapHeight;

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

	Gameplay(SDL_Renderer* m_renderer, Uint16 mapWidth, Uint16 mapHeight);
	~Gameplay();

	bool init();
	bool render();

	void renderMapTexture();

	void handleEvent(SDL_Event* e);

	bool isInitialized();
	bool isShutdown();

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
	Uint16 yMultiplier = xMultiplier/2;
	SDL_Point points[5];
	points[0] = SDL_Point{ leftTopPoint.x, leftTopPoint.y };
	points[1] = SDL_Point{ leftTopPoint.x + 1 * xMultiplier, leftTopPoint.y - 1 * yMultiplier };
	points[2] = SDL_Point{ leftTopPoint.x + 2* xMultiplier, leftTopPoint.y};
	points[3] = SDL_Point{ leftTopPoint.x + 1 * xMultiplier, leftTopPoint.y + 1 * yMultiplier };
	points[4] = SDL_Point{ leftTopPoint.x, leftTopPoint.y };

	return SDL_RenderDrawLines(renderer, points, 5);
}



Gameplay::Gameplay(SDL_Renderer* renderer, Uint16 mapWidth, Uint16 mapHeight)
{
	m_renderer = renderer;

	m_map = nullptr;

	m_mapWidth = mapWidth;
	m_mapHeight = mapHeight;

	m_isInitialized = false;
}

Gameplay::~Gameplay()
{

}

bool Gameplay::init()
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

	Uint16 xMultiplier = 30;
	Uint16 yMultiplier = xMultiplier / 2;
	m_MapTextureRenderQuad = new SDL_Rect{ 100,100, m_mapWidth * xMultiplier + m_mapHeight * xMultiplier, m_mapWidth * yMultiplier + m_mapHeight * yMultiplier };

	m_isInitialized = true;

	return true;
}

bool Gameplay::render()
{
	if (m_mapTexture == NULL)
	{
		printf("drawing");
		renderMapTexture();
	}

	Uint16 xMultiplier = 30;
	Uint16 yMultiplier = xMultiplier / 2;

	SDL_Rect* clip = new SDL_Rect{ 0,0, m_mapWidth * xMultiplier + m_mapHeight * xMultiplier, m_mapWidth * yMultiplier + m_mapHeight * yMultiplier };
	m_mapTexture->render(m_renderer, m_MapTextureRenderQuad, clip);

	return true;
}


void Gameplay::renderMapTexture()
{
	Uint16 xMultiplier = 30;
	Uint16 yMultiplier = xMultiplier / 2;

	Uint16 firstX = 0;
	Uint16 firstY = m_mapWidth * yMultiplier;

	SDL_Texture* target_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_mapWidth * xMultiplier + m_mapHeight * xMultiplier, m_mapWidth * yMultiplier + m_mapHeight * yMultiplier);
	SDL_SetRenderTarget(m_renderer, target_texture);
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	SDL_RenderClear(m_renderer);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	for (Uint16 i = 0; i < m_mapHeight; i++)
	{
		for (Uint16 j = 0; j < m_mapWidth; j++)
		{
			SDL_Point point{ firstX + j * xMultiplier + i * xMultiplier, firstY - j * yMultiplier + i * yMultiplier };

			m_map[i][j].render(m_renderer, point);
		}
	}

	SDL_Point points[5];
	points[0] = SDL_Point{ 0, 0 };
	points[1] = SDL_Point{ m_mapWidth * xMultiplier + m_mapHeight * xMultiplier-1, 0 };
	points[2] = SDL_Point{ m_mapWidth * xMultiplier + m_mapHeight * xMultiplier-1, m_mapWidth * yMultiplier + m_mapHeight * yMultiplier-1 };
	points[3] = SDL_Point{ 0, m_mapWidth * yMultiplier + m_mapHeight * yMultiplier-1 };
	points[4] = SDL_Point{ 0, 0 };

	SDL_RenderDrawLines(m_renderer, points, 5);

	m_mapTexture = new Texture(target_texture);

	SDL_SetRenderTarget(m_renderer, NULL);
	SDL_RenderPresent(m_renderer);
}

void Gameplay::handleEvent(SDL_Event* e)
{
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		int x, y;
		Uint32 mouseState = SDL_GetMouseState(&x, &y);

		int tx, ty;
		tx= x - m_MapTextureRenderQuad->x;
		ty = y - m_MapTextureRenderQuad->y;

		printf("coords by texture x: %d, y: %d\n", tx,ty);

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
				int newX,newY;
				newX = firstTextureX - (firstX - x);
				newY = firstTextureY - (firstY - y);

				if (newX + m_MapTextureRenderQuad->w > 1000)
				{
					newX = 1000 - m_MapTextureRenderQuad->w;
				}
				if (newY + m_MapTextureRenderQuad->h > 800)
				{
					newY = 800 - m_MapTextureRenderQuad->h;
				}
				if (newX < -200)
				{
					newX = -200;
				}
				if (newY < -200)
				{
					newY = -200;
				}

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


bool Gameplay::isInitialized()
{
	return m_isInitialized;
}

bool Gameplay::isShutdown()
{
	return false;
}

std::string Gameplay::GetLastError()
{
	return m_lastError;
}
