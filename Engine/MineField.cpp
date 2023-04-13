#include "MineField.h"
#include "Vei2.h"


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
			SpriteCodex::DrawTileNumber(screenPos,_nNeighborMines, gfx);
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

void MineField::Tile::toggleFlag()
{
	assert(!isRevealed());

	if (_state == State::Hidden)
		_state = State::Flagged;
	else if (_state == State::Flagged)
		_state = State::Hidden;
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

	for (Vei2 gridPos(0, 0); gridPos.y < _height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < _width; gridPos.x++)
		{
			tileAt(gridPos).setNeighborMineCount(countNeighborMines(gridPos));
		}
	}
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

	if(!tileAt(gridPos).isRevealed() && !tileAt(gridPos).isFlagged())
		tileAt(gridPos).reveal();
}

void MineField::onFlagClick(const Vei2& screenPos)
{
	Vei2 gridPos = screenToGrid(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < _width&& gridPos.y >= 0 && gridPos.y < _height);

	Tile tile = tileAt(gridPos);
	if (!tileAt(gridPos).isRevealed())
		tileAt(gridPos).toggleFlag();
}

int MineField::countNeighborMines(const Vei2& gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int xEnd = std::min(_width - 1, gridPos.x + 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int yEnd = std::min(_height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos(xStart, yStart); gridPos.y <= yEnd; gridPos.y++)
	{
		for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			if (tileAt(gridPos).hasMine())
			{
				count++;
			}
		}
	}

	return count;
}
