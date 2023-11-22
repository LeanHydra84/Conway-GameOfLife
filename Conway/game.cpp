#include <iostream>

#include "RenderData.hpp"
#include "ChunkRenderer.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#define TICKRATE 5
#define MOVEMENT_TICKRATE 60
#define SENSITIVITY 200

enum MovementKeys
{
	LEFT, RIGHT, UP, DOWN, Z_IN, Z_OUT
};

Vector2 process_input_movement(const bool navkeys[6])
{
	const constexpr float delta = SENSITIVITY * (1.0f / MOVEMENT_TICKRATE);
	Vector2 np = Vector2();
	if (navkeys[LEFT]) np.x += delta;
	if (navkeys[RIGHT]) np.x -= delta;
	if (navkeys[UP]) np.y += delta;
	if (navkeys[DOWN]) np.y -= delta;
	return np;
}

float clamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float lerp(float from, float to, float percent)
{
	return (to - from) * percent + from;
}

#define SCALE_IN_SENSITIVITY 1
#define SCALE_OUT_SENSITIVITY 0.1
#define SCALE_MIN 0.1
#define SCALE_MAX 15

float process_input_scale(const bool navkeys[6], float scale)
{
	if (navkeys[Z_IN] == navkeys[Z_OUT]) return scale;
	if (navkeys[Z_IN])
	{
		static const float delta = 1.0f - expf(-SCALE_IN_SENSITIVITY * (1.0f / MOVEMENT_TICKRATE));
		return lerp(scale, SCALE_MIN, delta);
	}
	else // Z_OUT == true
	{
		static const float delta = 1.0f - expf(-SCALE_OUT_SENSITIVITY * (1.0f / MOVEMENT_TICKRATE));
		return lerp(scale, SCALE_MAX, delta);
	}
}

int main()
{
	al_init();
	al_init_primitives_addon();

	RenderData data;
	data.Height = 800;
	data.Width = 800;

	data.ActiveColor = al_map_rgb(255, 255, 255);
	data.InactiveColor = al_map_rgb(128, 64, 64);
	data.LineColor = al_map_rgb(128, 128, 128);

	ChunkRenderer chunks = ChunkRenderer(&data);

	al_install_mouse();
	al_install_keyboard();
	
	ALLEGRO_DISPLAY* display = al_create_display(data.Width, data.Height);
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	ALLEGRO_TIMER* tick_timer = al_create_timer(1.0f / TICKRATE);
	ALLEGRO_TIMER* input_timer = al_create_timer(1.0f / MOVEMENT_TICKRATE);

	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_timer_event_source(tick_timer));
	al_register_event_source(queue, al_get_timer_event_source(input_timer));

	ALLEGRO_EVENT e;
	bool render = true;

	bool navkeys[6] = { 0 };
	al_start_timer(input_timer);

	while (true)
	{
		al_wait_for_event(queue, &e);
		if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		else if (e.type == ALLEGRO_EVENT_TIMER)
		{
			if (e.timer.source == input_timer)
			{
				Vector2 newcenter = process_input_movement(navkeys) + chunks.center();
				float scale = process_input_scale(navkeys, chunks.scale());
				float scale_ratio = chunks.scale() / scale;
				chunks.set_scale(scale);
				chunks.set_center(newcenter * scale_ratio);
			}
			else if (e.timer.source == tick_timer)
			{
				chunks.execute_updates();
			}

			render = true;
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			V2i mpos = V2i(e.mouse.x, e.mouse.y);
			V2i clicked = chunks.screen_position_to_normalized_cell(mpos);

			Cell c = chunks.get_cell_global(clicked);
			if (c == CELL_ALIVE) c = CELL_DEAD;
			else c = CELL_ALIVE;

			chunks.set_cell_global(clicked, c);
			chunks.queue_cell_update_global(clicked);
			render = true;
		}
		else if (e.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (e.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				break;
			if (e.keyboard.keycode == ALLEGRO_KEY_G)
			{
				chunks.execute_updates();
				render = true;
			}
			else if (e.keyboard.keycode == ALLEGRO_KEY_SPACE)
			{
				if (al_get_timer_started(tick_timer))
					al_stop_timer(tick_timer);
				else
					al_start_timer(tick_timer);
			}
			else
			{
				switch (e.keyboard.keycode)
				{
				case ALLEGRO_KEY_LEFT:
					navkeys[LEFT] = true; break;
				case ALLEGRO_KEY_RIGHT:
					navkeys[RIGHT] = true; break;
				case ALLEGRO_KEY_DOWN:
					navkeys[DOWN] = true; break;
				case ALLEGRO_KEY_UP:
					navkeys[UP] = true; break;
				case ALLEGRO_KEY_Q:
					navkeys[Z_IN] = true; break;
				case ALLEGRO_KEY_E:
					navkeys[Z_OUT] = true; break;
				}
			}
		}
		else if (e.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (e.keyboard.keycode)
			{
			case ALLEGRO_KEY_LEFT:
				navkeys[LEFT] = false; break;
			case ALLEGRO_KEY_RIGHT:
				navkeys[RIGHT] = false; break;
			case ALLEGRO_KEY_DOWN:
				navkeys[DOWN] = false; break;
			case ALLEGRO_KEY_UP:
				navkeys[UP] = false; break;
			case ALLEGRO_KEY_Q:
				navkeys[Z_IN] = false; break;
			case ALLEGRO_KEY_E:
				navkeys[Z_OUT] = false; break;
			}
		}

		if (render && al_is_event_queue_empty(queue))
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));

			chunks.render();

			al_flip_display();
			render = false;
		}
	}
}