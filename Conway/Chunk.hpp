#pragma once

#include <vector>
#include "V2i.h"

using Cell = bool;
inline Cell invert_cell(Cell a) { return !a; }

#define CHUNK_DIMENSION 16
#define CHUNK_SIZE (CHUNK_DIMENSION * CHUNK_DIMENSION)

class Chunk
{
private:
	Cell cells[CHUNK_SIZE];
	V2i chunk_position;

	std::vector<V2i> update0;
	std::vector<V2i> update1;

public:
	Chunk(int x, int y);
	Chunk(const V2i& pos);

private:
	void add_to_update_buffer(const V2i& pos, std::vector<V2i>& buffer);
	
public:
	int get_neighbor_count(const V2i& pos, Chunk* neighbors[8]);
	void execute_update(Chunk* neighbors[8], bool backbuffer);
	void clear_update_lists();
	void add_to_update_list(const V2i& pos, bool backbuffer);

	void toggle_pixel(const V2i& pos);
	void set_pixel(const V2i& pos, Cell c);


	Cell& get_pixel(const V2i& v2i); // returns nonconst reference type
	Cell get_pixel_c(const V2i& v2i) const; // returns const value type
	V2i pixel_to_world_pos(const V2i& pixel) const;

	const V2i& position() const noexcept;

	static int v2itoindex(const V2i& v2i);
};