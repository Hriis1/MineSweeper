#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "SpriteCodex.h"

#include <random>

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
		void draw(const Vei2& screenPos, Graphics& gfx) const;

		void spawnMine();
		bool hasMine() const;
	private:
		State _state = State::Hidden;
		bool _hasMine = false;
	};
public:
	MineField(int nMines);

	void draw(Graphics& gfx) const;

	RectI getRect() const { return RectI(0, _width * SpriteCodex::tileSize, 0, _height * SpriteCodex::tileSize); }

private:
	Tile& tileAt(const Vei2& gridPos) { return _field[gridPos.y * _width + gridPos.x]; }
	const Tile& tileAt(const Vei2& gridPos) const { return _field[gridPos.y * _width + gridPos.x]; }
private:
	static constexpr int _width = 20;
	static constexpr int _height = 16;

	Tile _field[_width * _height];
};

