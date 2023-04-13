#include "MineField.h"
#include "Vei2.h"


void MineField::Tile::draw(const Vei2& screenPos, bool gameLost, Graphics& gfx) const
{
	if (!gameLost)
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
				SpriteCodex::DrawTileNumber(screenPos, _nNeighborMines, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else //game is lost
	{
		switch (_state)
		{
		case State::Hidden:
			if(hasMine())
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			else
				SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Flagged:
			if (hasMine())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (!hasMine())
			{
				SpriteCodex::DrawTileNumber(screenPos, _nNeighborMines, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		}
	}
}

void MineField::Tile::spawnMine()
{
	assert(!_hasMine);

	_hasMine = true;
}

void MineField::Tile::reveal(MineField& field)
{
	assert(_state == State::Hidden);

	_state = State::Revealed;

	if (_nNeighborMines == 0)
	{
		std::vector<Tile*> neighborTiles = field.getNeighborTiles(_fieldPos);
		for (size_t i = 0; i < neighborTiles.size(); i++)
		{
			if (neighborTiles[i]->_state == State::Hidden)
				neighborTiles[i]->reveal(field);
		}
	}

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

	for (size_t i = 0; i < _width * _height; i++)
	{
		_field[i].setFieldPos(i);
	}
}

void MineField::draw(Graphics& gfx) const
{
	gfx.DrawRect(getRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos(0, 0); gridPos.y < _height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < _width; gridPos.x++)
		{
			tileAt(gridPos).draw(gridPos * SpriteCodex::tileSize, _gameLost,gfx);
		}
	}
}

void MineField::onRevealClick(const Vei2& screenPos)
{
	if (!_gameLost)
	{
		Vei2 gridPos = screenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < _width&& gridPos.y >= 0 && gridPos.y < _height);

		if (!tileAt(gridPos).isRevealed() && !tileAt(gridPos).isFlagged())
			tileAt(gridPos).reveal(*this);

		if (tileAt(gridPos).hasMine())
			_gameLost = true;
	}
}

void MineField::onFlagClick(const Vei2& screenPos)
{
	if (!_gameLost)
	{
		Vei2 gridPos = screenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < _width&& gridPos.y >= 0 && gridPos.y < _height);

		Tile tile = tileAt(gridPos);
		if (!tileAt(gridPos).isRevealed())
			tileAt(gridPos).toggleFlag();
	}
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

std::vector<MineField::Tile*> MineField::getNeighborTiles(int fieldPos)
{
	Vei2 gridPos;
	gridPos.x = fieldPos % _width;
	gridPos.y = fieldPos / _width;
	std::vector<MineField::Tile*> tiles;
	const int xStart = std::max(0, gridPos.x - 1);
	const int xEnd = std::min(_width - 1, gridPos.x + 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int yEnd = std::min(_height - 1, gridPos.y + 1);

	for (Vei2 gridPos(xStart, yStart); gridPos.y <= yEnd; gridPos.y++)
	{
		for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			tiles.push_back(&tileAt(gridPos));
		}
	}

	return tiles;
}
