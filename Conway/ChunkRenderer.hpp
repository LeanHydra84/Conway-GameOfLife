#ifndef _CHUNKRENDERER_HPP_
#define _CHUNKRENDERER_HPP_

#include <unordered_map>
#include <vector>

#include "Chunk.hpp"
#include "vec2.hpp"
#include "V2i.hpp"
#include "RenderData.hpp"

using ch_hash_t = uint64_t;


class ChunkRenderer
{
public:
	ChunkRenderer(const RenderData* data);
	~ChunkRenderer();
private:
	std::unordered_map<ch_hash_t, Chunk*> chunks;
	std::unordered_map<ch_hash_t, Chunk*> update_queue;

	const RenderData* data;

	Vector2 _center;
	float _scale;
	bool _backbuffer;

private:
	ch_hash_t v2i_hashkey(const V2i& v2i) const;
	bool cull_chunk(const Vector2& corner) const;
	void render_chunk(const Vector2& corner, Chunk* chunk) const;
	void get_neighbors(const V2i& center, Chunk* buffer[8]) const;
	void delete_chunk_by_hash(ch_hash_t hash);
public:
	bool backbuffer() const;

	void execute_updates();
	void queue_update(Chunk* chunk);

	bool does_chunk_exist(const V2i& v2i) const;
	Chunk* add_chunk(const V2i& v2i);
	void delete_chunk(const V2i& v2i);
	void free_unused();

	Cell get_cell_global(const V2i& v2i) const;
	void set_cell_global(const V2i& v2i, Cell value);
	void queue_cell_update_global(const V2i& globalpos);
	void render() const;
	void flip_backbuffer();

	V2i screen_position_to_normalized_cell(V2i mpos) const;

	// Getters
	Chunk* get_chunk(const V2i& v2i) const;
	Vector2 center() const;
	float scale() const;

	// Setters
	void set_center(const Vector2& vec);
	void set_scale(float scale);

};

#endif