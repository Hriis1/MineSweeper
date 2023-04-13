#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "SpriteCodex.h"

#include <assert.h>
#include <random>
#include <vector>

class MineField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden, Flagged, Revealed
		};
	public:
		void draw(const Vei2& screenPos,bool gameLost, Graphics& gfx) const;

		void spawnMine();
		void reveal(MineField& field);
		void toggleFlag();

		bool hasMine() const { return _hasMine; }
		bool isRevealed() const { return _state == State::Revealed; }
		bool isFlagged() const { return _state == State::Flagged; }

		//setters
		void setNeighborMineCount(int mineCount) { assert(_nNeighborMines == -1); _nNeighborMines = mineCount; }
		void setFieldPos(int fieldPos) { assert(_fieldPos == -1); _fieldPos = fieldPos; }
	private:
		State _state = State::Hidden;
		bool _hasMine = false;
		int _nNeighborMines = -1;
		int _fieldPos = -1;
	};
public:
	MineField(int nMines);

	void draw(Graphics& gfx) const;
	void onRevealClick(const Vei2& screenPos);
	void onFlagClick(const Vei2& screenPos);

	RectI getRect() const { return RectI(0, _width * SpriteCodex::tileSize, 0, _height * SpriteCodex::tileSize); }

private:
	Tile& tileAt(const Vei2& gridPos) { return _field[gridPos.y * _width + gridPos.x]; }
	const Tile& tileAt(const Vei2& gridPos) const { return _field[gridPos.y * _width + gridPos.x]; }
	Vei2 screenToGrid(const Vei2& screenPos) const { return screenPos / SpriteCodex::tileSize; }
	int countNeighborMines(const Vei2& gridPos);
	std::vector<Tile*> getNeighborTiles(int fieldPos);
private:
	static constexpr int _width = 20;
	static constexpr int _height = 16;

	bool _gameLost = false;
	Tile _field[_width * _height];
};

