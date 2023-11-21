#include <iostream>

#include "RenderData.hpp"
#include "ChunkRenderer.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#define TICKRATE 5

int main()
{
	al_init();
	al_init_primitives_addon();

	RenderData data;
	data.Height = 800;
	data.Width = 800;
	data.ActiveColor = al_map_rgb(255, 255, 255);
	data.InactiveColor = al_map_rgb(128, 64, 64);

	ChunkRenderer chunks = ChunkRenderer(&data);

	al_install_mouse();
	al_install_keyboard();
	
	ALLEGRO_DISPLAY* display = al_create_display(data.Width, data.Height);
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0f / TICKRATE);

	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());

	ALLEGRO_EVENT e;
	bool render = true;

	//al_start_timer(timer);

	while (true)
	{
		al_wait_for_event(queue, &e);
		if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		else if (e.type == ALLEGRO_EVENT_TIMER)
		{
			chunks.execute_updates();
			chunks.flip_backbuffer();
			render = true;
		}
		else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			V2i mpos = V2i(e.mouse.x, e.mouse.y);
			V2i clicked = chunks.screen_position_to_normalized_cell(mpos);

			Cell c = chunks.get_cell_global(mpos);
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
			else if (e.keyboard.keycode == ALLEGRO_KEY_SPACE)
			{
				if (al_get_timer_started(timer))
					al_stop_timer(timer);
				else
					al_start_timer(timer);
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