#include <memory>
#include "Chunk.hpp"

Chunk::Chunk(int x, int y) : Chunk(V2i(x, y)) { }
Chunk::Chunk(const V2i& pos) : chunk_position(pos), update0(), update1()
{
	memset(cells, 0, CHUNK_SIZE * sizeof(Cell));
}

// Neighbors: starting at top left, clockwise.
// Ergo: Top left, up, top right, right, bottom right, down, bottom left, left

int get_neighbor_count(const V2i& pos, Chunk* neighbors[8])
{

}

void Chunk::execute_update(Chunk* neighbors[8], bool backbuffer)
{
	/// yada yada
	std::vector<V2i>& buffer = (backbuffer) ? update0 : update1;
	std::vector<V2i>& back = (backbuffer) ? update1 : update0;
	back.clear();
	for (const V2i& vec : buffer)
	{
		int ncount = get_neighbor_count(vec, neighbors);
		if(ncount < 2 || ncount >= 4)
	}
}

void Chunk::clear_update_lists()
{
	update0.clear();
	update1.clear();
}

void add_to_update_buffer(const V2i& pos, std::vector<V2i>& buffer)
{
	if (std::find(buffer.begin(), buffer.end(), pos) != buffer.end())
	{
		buffer.push_back(pos);
	}
}

void Chunk::add_to_update_list(const V2i& pos, bool backbuffer)
{
	std::vector<V2i>& updates = (backbuffer) ? update0 : update1;
	add_to_update_buffer(pos, updates);
}

void Chunk::toggle_pixel(const V2i& pos)
{
	Cell& cell = get_pixel(pos);
	cell = invert_cell(cell);
}

void Chunk::set_pixel(const V2i& pos, Cell c)
{
	get_pixel(pos) = c;
}

V2i Chunk::pixel_to_world_pos(const V2i& pixel) const
{
	return (chunk_position * CHUNK_DIMENSION) + pixel;
}

Cell& Chunk::get_pixel(const V2i& v2i)
{
	return cells[v2itoindex(v2i)];
}

Cell Chunk::get_pixel_c(const V2i& v2i) const
{
	return cells[v2itoindex(v2i)];
}

int Chunk::v2itoindex(const V2i& v2i)
{
	return (v2i.y * CHUNK_DIMENSION) + v2i.x;
}

const V2i& Chunk::position() const noexcept
{
	return chunk_position;
}