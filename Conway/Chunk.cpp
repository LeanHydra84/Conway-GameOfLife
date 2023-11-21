#include <memory>
#include "Chunk.hpp"
#include "ChunkRenderer.hpp"

Chunk::Chunk(int x, int y, ChunkRenderer* parent) : Chunk(V2i(x, y), parent) { }
Chunk::Chunk(const V2i& pos, ChunkRenderer* parent) : chunk_position(pos), update0(), update1(), owner(parent)
{
	memset(cells0, 0, CHUNK_SIZE * sizeof(Cell));
	memset(cells1, 0, CHUNK_SIZE * sizeof(Cell));
	BACKBUFFER = false;
}


void Chunk::execute_update(Chunk* neighbors[8])
{
	/// yada yada
	//std::cout << "Chunk " << position() << ": update0{" << update0.size() << "}, update1{" << update1.size() << "}" << std::endl;

	Cell* fromBuffer = (BACKBUFFER) ? cells0 : cells1;
	Cell* toBuffer = (BACKBUFFER) ? cells1 : cells0;
	memcpy(toBuffer, fromBuffer, CHUNK_SIZE * sizeof(Cell));

	std::vector<V2i>& toExec = (BACKBUFFER) ? update0 : update1;
	std::vector<V2i>& toUpdate = (BACKBUFFER) ? update1 : update0;
	toUpdate.clear();

	for (const V2i& vec : toExec)
	{
		//std::cout << "Updating cell: " << vec << " in chunk: " << position() << std::endl;
		int index = v2itoindex(vec);

		Cell cell = fromBuffer[index];
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
			toBuffer[index] = newValue;
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
	if (std::find(buffer.begin(), buffer.end(), pos) == buffer.end())
	{
		buffer.push_back(pos);
	}
}

void Chunk::add_to_update_list(const V2i& pos)
{
	std::vector<V2i>& updates = (BACKBUFFER) ? update1 : update0;
	add_to_update_buffer(pos, updates);
	//std::cout << "added update " << pos << " to buffer, size=" << updates.size() << std::endl;
	owner->queue_update(this);
}

// Neighbors: column-wise, top to bottom
// TL, L, BL, T, B, TR, R, BR
//  0  1   2  3  4   5  6   7
static enum NeighborValue
{
	TL, L, BL, T, B, TR, R, BR, SELF
};

static NeighborValue get_chunk_neighbor_value(const V2i& vec)
{
	if (vec.x < CHUNK_DIMENSION && vec.x >= 0 &&
		vec.y < CHUNK_DIMENSION && vec.y >= 0)
		return SELF;

	if (vec.x < 0)
	{
		if (vec.y < 0) // TOP LEFT
			return TL;
		else if (vec.y >= CHUNK_DIMENSION) // BOTTOM LEFT
			return BL;
		else // LEFT
			return L;
	}
	else if (vec.x >= CHUNK_DIMENSION)
	{
		if (vec.y < 0) // TOP RIGHT
			return TR;
		else if (vec.y >= CHUNK_DIMENSION) // BOTTOM RIGHT
			return BR;
		else // RIGHT
			return R;
	}
	else if (vec.y < 0) // TOP
		return T;
	else // BOTTOM
		return B;
}



static V2i get_neighbor_offset(NeighborValue neighbor_index)
{
	switch (neighbor_index)
	{
	case TL: // TOP LEFT
		return V2i(CHUNK_DIMENSION, CHUNK_DIMENSION);
	case L: // LEFT
		return V2i(CHUNK_DIMENSION, 0);
	case BL: // BOTTOM LEFT
		return V2i(CHUNK_DIMENSION, -CHUNK_DIMENSION);
	case T: // TOP
		return V2i(0, CHUNK_DIMENSION);
	case B: // BOTTOM
		return V2i(0, -CHUNK_DIMENSION);
	case TR: // TOP RIGHT
		return V2i(-CHUNK_DIMENSION, CHUNK_DIMENSION);
	case R: // RIGHT
		return V2i(-CHUNK_DIMENSION, 0);
	case BR: // BOTTOM RIGHT
		return V2i(-CHUNK_DIMENSION, -CHUNK_DIMENSION);
	case -1:
	default:
		return V2i();
	}
}

// Neighbors: column-wise, top to bottom
// TL, L, BL, T, B, TR, R, BR
int Chunk::get_neighbor_count(const V2i& pos, Chunk* neighbors[8])
{
	int ncount = 0;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (j == 0 && i == 0) continue;
			V2i np = pos + V2i(i, j);
			NeighborValue nv = get_chunk_neighbor_value(np);
			Cell value;

			if (nv == SELF)
			{
				value = get_pixel_c(np);
			}
			else
			{
				if (neighbors[nv] == nullptr)
					continue;

				V2i offset_pos = np + get_neighbor_offset(nv);
				Chunk* ch = neighbors[nv];

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

	NeighborValue neighbor = get_chunk_neighbor_value(vec);
	V2i offset = get_neighbor_offset(neighbor);

	Chunk* ch = neighbors[neighbor];
	if (ch == nullptr) return;

	ch->add_to_update_list(vec + offset);
}

void Chunk::update_adjacent(const V2i& vec, Chunk* neighbors[8])
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			// update all 9 tiles (adjacent + self)
			update_correct_chunk(vec + V2i(i, j), neighbors);
		}
	}
}

void Chunk::toggle_pixel(const V2i& pos)
{
	Cell& cell = get_pixel(pos);
	if (cell == CELL_ALIVE) 
		cell = CELL_DEAD;
	else cell = CELL_ALIVE;
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