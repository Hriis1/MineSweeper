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
		void reveal();
		void toggleFlag();

		bool hasMine() const { return _hasMine; }
		bool isRevealed() const { return _state == State::Revealed; }
		bool isFlagged() const { return _state == State::Flagged; }
	private:
		State _state = State::Hidden;
		bool _hasMine = false;
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
private:
	static constexpr int _width = 20;
	static constexpr int _height = 16;

	Tile _field[_width * _height];
};

