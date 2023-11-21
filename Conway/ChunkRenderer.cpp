#include "ChunkRenderer.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#define SQUARE_PIXEL_SCALE 16

ChunkRenderer::ChunkRenderer(const RenderData* _data) : chunks(), _center(),
_scale(1), data(_data), backbuffer(false)
{
	flip_backbuffer();
}

ChunkRenderer::~ChunkRenderer()
{
	for (auto& x : chunks)
	{
		Chunk* ch = x.second;
		if (ch == nullptr) continue;
		delete ch;
	}
}

ch_hash_t ChunkRenderer::v2i_hashkey(const V2i& v2i) const
{
	static_assert(sizeof(v2i) == sizeof(ch_hash_t), "V2i is incorrect size for this datatype");

	// ew
	ch_hash_t hashkey = 0;
	hashkey |= *reinterpret_cast<const unsigned int*>(&v2i.x);
	ch_hash_t higher_order = *reinterpret_cast<const unsigned int*>(&v2i.y);
	
	hashkey |= (higher_order << (sizeof(int) * 8));
	return hashkey;
}

bool ChunkRenderer::does_chunk_exist(const V2i& v2i) const
{
	ch_hash_t hash = v2i_hashkey(v2i);
	return chunks.find(hash) != chunks.end();
}

// Getters
Vector2 ChunkRenderer::center() const
{
	return _center;
}

float ChunkRenderer::scale() const
{
	return _scale;
}

// Setters
void ChunkRenderer::set_center(const Vector2& vec)
{
	_center = vec;
}

void ChunkRenderer::set_scale(float scale)
{
	_scale = scale;
}

//

Chunk* ChunkRenderer::add_chunk(const V2i& v2i)
{
	Chunk* newchunk = new Chunk(v2i, this);
	newchunk->BACKBUFFER = backbuffer;

	ch_hash_t hashkey = v2i_hashkey(v2i);
	chunks[hashkey] = newchunk;

	return newchunk;
}

Cell ChunkRenderer::get_cell_global(const V2i& v2i) const
{
	V2i chunkcoord = v2i / CHUNK_DIMENSION;
	V2i interior = V2i(v2i.x % CHUNK_DIMENSION, v2i.y % CHUNK_DIMENSION);

	Chunk* ch = get_chunk(chunkcoord);
	if (ch == nullptr)
		return CELL_DEAD;

	return ch->get_pixel_c(interior);
}

void ChunkRenderer::set_cell_global(const V2i& v2i, Cell value)
{
	V2i chunkcoord = v2i / CHUNK_DIMENSION;
	V2i interior = V2i(v2i.x % CHUNK_DIMENSION, v2i.y % CHUNK_DIMENSION);
	
	Chunk* ch = get_chunk(chunkcoord);
	if (ch == nullptr)
	{
		ch = add_chunk(chunkcoord);
	}

	ch->set_pixel(interior, value);
}

void ChunkRenderer::queue_cell_update_global(const V2i& v2i)
{
	V2i chunkcoord = v2i / CHUNK_DIMENSION;
	V2i interior = V2i(v2i.x % CHUNK_DIMENSION, v2i.y % CHUNK_DIMENSION);

	Chunk* ch = get_chunk(chunkcoord);
	if (ch == nullptr)
	{
		ch = add_chunk(chunkcoord);
	}

	Chunk* neighbors[8];
	get_neighbors(ch->position(), neighbors);
	
	// janky hack
	ch->BACKBUFFER = !ch->BACKBUFFER;
	ch->update_adjacent(interior, neighbors);
	ch->BACKBUFFER = !ch->BACKBUFFER;
}

void ChunkRenderer::render() const
{
	for (auto& ch : chunks)
	{
		Chunk* chunk = ch.second;
		float chunk_edge = (CHUNK_DIMENSION * SQUARE_PIXEL_SCALE) * _scale;
		Vector2 corner = (chunk->position() * chunk_edge) + _center;

		// cull chunks that are out of screenspace
		if (cull_chunk(corner)) continue;
		
		render_chunk(corner, chunk);
	}
}

void ChunkRenderer::render_chunk(const Vector2& corner, Chunk* chunk) const
{
	float chunk_edge = (CHUNK_DIMENSION * SQUARE_PIXEL_SCALE) * _scale;
	al_draw_filled_rectangle(corner.x, corner.y, corner.x + chunk_edge, corner.y + chunk_edge, data->InactiveColor);

	float square_size = SQUARE_PIXEL_SCALE * _scale;
	for (int i = 0; i < CHUNK_DIMENSION; i++)
	{
		for (int j = 0; j < CHUNK_DIMENSION; j++)
		{
			V2i chindex = V2i(i, j);
			if (chunk->get_pixel_c(chindex) == CELL_DEAD)
				continue;
			Vector2 sqrbase = (chindex * SQUARE_PIXEL_SCALE) + corner;
			al_draw_filled_rectangle(sqrbase.x, sqrbase.y, sqrbase.x + square_size, sqrbase.y + square_size, data->ActiveColor);
		}
	}
}

// returns true if chunk is not visible and should be culled
bool ChunkRenderer::cull_chunk(const Vector2& corner) const
{
	float chunk_edge = (CHUNK_DIMENSION * SQUARE_PIXEL_SCALE) * _scale;
	if (corner.x > data->Width || (corner.x + chunk_edge) < 0) return true;
	if (corner.y > data->Height || (corner.y + chunk_edge) < 0) return true;
	return false;
}

// Getters
Chunk* ChunkRenderer::get_chunk(const V2i& v2i) const
{
	ch_hash_t hashkey = v2i_hashkey(v2i);
	if (chunks.find(hashkey) == chunks.end()) return nullptr;
	return chunks.at(hashkey);
}


void ChunkRenderer::execute_updates()
{
	std::vector<Chunk*> to_update;
	to_update.reserve(update_queue.size());
	for (auto& kv : update_queue)
		to_update.push_back(kv.second);
	update_queue.clear();

	Chunk* nbuffer[8];

	for (Chunk* chunk : to_update)
	{
		get_neighbors(chunk->position(), nbuffer);
		chunk->execute_update(nbuffer);
	}

}

void ChunkRenderer::queue_update(Chunk* chunk)
{
	ch_hash_t hashkey = v2i_hashkey(chunk->position());
	update_queue[hashkey] = chunk;
	//std::cout << "Queueing Update for chunk: " << hashkey << std::endl;
}

void ChunkRenderer::get_neighbors(const V2i& center, Chunk* buffer[8]) const
{
	// ORDER:
	// TL, L, BL, T, B, TR, R, BR
	int counter = 0;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0) continue;
			V2i index = V2i(i, j) + center;
			buffer[counter++] = get_chunk(index);
		}
	}
}

void ChunkRenderer::flip_backbuffer()
{
	backbuffer = !backbuffer;
	for (auto& kv : chunks)
	{
		kv.second->BACKBUFFER = backbuffer;
	}
}

V2i ChunkRenderer::screen_position_to_normalized_cell(V2i mpos) const
{
	Vector2 centered = (mpos.to_v2f() - center()) / (scale() * SQUARE_PIXEL_SCALE);
	V2i normalized = V2i(roundf(centered.x), roundf(centered.y));
	return normalized;
}