#include "MineField.h"
#include "Vei2.h"

#include <assert.h>


void MineField::Tile::draw(const Vei2& screenPos, Graphics& gfx) const
{
	switch (_state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Revealed:
		if (!hasMine())
		{
			SpriteCodex::DrawTile0(screenPos, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		break;
	}
}

void MineField::Tile::spawnMine()
{
	assert(!_hasMine);

	_hasMine = true;
}

void MineField::Tile::reveal()
{
	assert(_state == State::Hidden);

	_state = State::Revealed;
}

MineField::MineField(int nMines)
{
	assert(nMines > 0 && nMines < _width* _height);
	
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, _width - 1);
	std::uniform_int_distribution<int> yDist(0, _height - 1);

	for (size_t nSpawned = 0; nSpawned < nMines; nSpawned++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = Vei2(xDist(rng), yDist(rng));
		} while (tileAt(spawnPos).hasMine());

		tileAt(spawnPos).spawnMine();
	}

	////reveal test
	//for (size_t i = 0; i < 120; i++)
	//{
	//	const Vei2 gridPos = Vei2(xDist(rng), yDist(rng));

	//	if(!tileAt(gridPos).isRevealed())
	//		tileAt(gridPos).reveal();
	//}
}

void MineField::draw(Graphics& gfx) const
{
	gfx.DrawRect(getRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos(0, 0); gridPos.y < _height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < _width; gridPos.x++)
		{
			tileAt(gridPos).draw(gridPos * SpriteCodex::tileSize, gfx);
		}
		
	}
}

void MineField::onRevealClick(const Vei2& screenPos)
{
	Vei2 gridPos = screenToGrid(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < _width&& gridPos.y >= 0 && gridPos.y < _height);

	if(!tileAt(gridPos).isRevealed())
		tileAt(gridPos).reveal();
}
