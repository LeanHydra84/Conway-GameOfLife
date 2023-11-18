#include <memory>
#include "Chunk.hpp"

Chunk::Chunk(int x, int y, ChunkQueue queue) : Chunk(V2i(x, y), queue) { }
Chunk::Chunk(const V2i& pos, ChunkQueue queue) : chunk_position(pos), update0(), update1(), global_chunk_update_queue(queue)
{
	memset(cells0, 0, CHUNK_SIZE * sizeof(Cell));
	memset(cells1, 0, CHUNK_SIZE * sizeof(Cell));
	BACKBUFFER = false;
}


void Chunk::execute_update(Chunk* neighbors[8])
{
	/// yada yada
	std::vector<V2i>& buffer = (BACKBUFFER) ? update0 : update1;
	std::vector<V2i>& back = (BACKBUFFER) ? update1 : update0;
	back.clear();
	for (const V2i& vec : buffer)
	{
		Cell& cell = get_pixel(vec);
		Cell newValue = cell;

		int ncount = get_neighbor_count(vec, neighbors);
		if (ncount < 2 || ncount >= 4)
		{
			newValue = CELL_DEAD;
		}
		if (ncount == 3)
		{
			newValue = CELL_ALIVE;
		}

		if (cell != newValue)
		{
			cell = newValue;
			update_adjacent(vec, neighbors);
		}
	}
}

void Chunk::clear_update_lists()
{
	update0.clear();
	update1.clear();
}

void Chunk::add_to_update_buffer(const V2i& pos, std::vector<V2i>& buffer)
{
	if (std::find(buffer.begin(), buffer.end(), pos) != buffer.end())
	{
		buffer.push_back(pos);
	}
}

void Chunk::add_to_update_list(const V2i& pos)
{
	std::vector<V2i>& updates = (BACKBUFFER) ? update0 : update1;
	add_to_update_buffer(pos, updates);

}

static int get_chunk_neighbor_value(const V2i& vec)
{
	if (vec.x < CHUNK_DIMENSION && vec.x >= 0 &&
		vec.y < CHUNK_DIMENSION && vec.y >= 0)
		return -1;

	if (vec.x < 0)
	{
		if (vec.y < 0) // TOP LEFT
			return 0;
		else if (vec.y >= CHUNK_DIMENSION) // BOTTOM LEFT
			return 6;
		else // LEFT
			return 7;
	}
	else if (vec.x >= CHUNK_DIMENSION)
	{
		if (vec.y < 0) // TOP RIGHT
			return 2;
		else if (vec.y >= CHUNK_DIMENSION) // BOTTOM RIGHT
			return 4;
		else // RIGHT
			return 3;
	}
	else if (vec.y < 0) // TOP
		return 1;
	else // BOTTOM
		return 5;
}

static V2i get_neighbor_offset(int neighbor_index)
{
	switch (neighbor_index)
	{
	case 0: // TOP LEFT
		return V2i(CHUNK_DIMENSION, CHUNK_DIMENSION);
	case 1: // TOP
		return V2i(0, CHUNK_DIMENSION);
	case 2: // TOP RIGHT
		return V2i(-CHUNK_DIMENSION, CHUNK_DIMENSION);
	case 3: // RIGHT
		return V2i(-CHUNK_DIMENSION, 0);
	case 4: // BOTTOM RIGHT
		return V2i(-CHUNK_DIMENSION, -CHUNK_DIMENSION);
	case 5: // BOTTOM
		return V2i(0, -CHUNK_DIMENSION);
	case 6: // BOTTOM LEFT
		return V2i(CHUNK_DIMENSION, -CHUNK_DIMENSION);
	case 7: // LEFT
		return V2i(CHUNK_DIMENSION, 0);
	case -1:
	default:
		return V2i();
	}
}

// Neighbors: starting at top left, clockwise.
// Ergo: Top left, up, top right, right, bottom right, down, bottom left, left
int Chunk::get_neighbor_count(const V2i& pos, Chunk* neighbors[8])
{
	int ncount = 0;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (j == 0 && i == 0) continue;
			V2i np = pos + V2i(i, j);
			int chunk = get_chunk_neighbor_value(np);
			Cell value;

			if (chunk == -1)
			{
				value = get_pixel_c(np);
			}
			else
			{
				if (neighbors[chunk] == nullptr)
					continue;

				V2i offset_pos = np + get_neighbor_offset(chunk);
				Chunk* ch = neighbors[chunk];

				value = ch->get_pixel_c(offset_pos);
			}

			if (value == CELL_ALIVE)
				ncount++;
		}
	}

	return ncount;
}

void Chunk::update_correct_chunk(const V2i& vec, Chunk* neighbors[8])
{
	// BASE CASE: update is within current chunk
	if (vec.x < CHUNK_DIMENSION && vec.x >= 0 &&
		vec.y < CHUNK_DIMENSION && vec.y >= 0)
	{
		add_to_update_list(vec);
		return;
	}

	int chunk = get_chunk_neighbor_value(vec);
	V2i offset = get_neighbor_offset(chunk);

	Chunk* ch = neighbors[chunk];
	if (ch == nullptr) return;

	ch->add_to_update_list(vec + offset);
}

void Chunk::update_adjacent(const V2i& vec, Chunk* neighbors[8])
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (j == 0 && i == 0) continue;
			update_correct_chunk(vec + V2i(i, j), neighbors);
		}
	}
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
	Cell* cells = (BACKBUFFER) ? cells0 : cells1;
	return cells[v2itoindex(v2i)];
}

Cell Chunk::get_pixel_c(const V2i& v2i) const
{
	const Cell* cells = (BACKBUFFER) ? cells0 : cells1;
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