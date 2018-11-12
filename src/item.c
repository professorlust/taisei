/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "taisei.h"

#include "item.h"
#include "global.h"
#include "list.h"
#include "stageobjects.h"
#include "objectpool_util.h"

static Sprite* item_sprite(ItemType type) {
	static const char *const map[] = {
		[Power]     = "item/power",
		[MiniPower] = "item/minipower",
		[Surge]     = "item/surge",
		[Point]     = "item/point",
		[Life]      = "item/life",
		[Bomb]      = "item/bomb",
		[LifeFrag]  = "item/lifefrag",
		[BombFrag]  = "item/bombfrag",
		[BPoint]    = "item/bullet_point",
	};

	// int cast to silence a WTF warning
	assert((int)type < sizeof(map)/sizeof(char*));
	return get_sprite(map[type]);
}

static void ent_draw_item(EntityInterface *ent) {
	Item *i = ENT_CAST(ent, Item);

	Color *c = RGBA_MUL_ALPHA(1, 1, 1,
		i->type == BPoint && !i->auto_collect
			? clamp(2.0 - (global.frames - i->birthtime) / 60.0, 0.1, 1.0)
			: 1.0
	);

	r_draw_sprite(&(SpriteParams) {
		.sprite_ptr = item_sprite(i->type),
		.pos = { creal(i->pos), cimag(i->pos) },
		.color = c,
	});
}

Item* create_item(complex pos, complex v, ItemType type) {
	if((creal(pos) < 0 || creal(pos) > VIEWPORT_W)) {
		// we need this because we clamp the item position to the viewport boundary during motion
		// e.g. enemies that die offscreen shouldn't spawn any items inside the viewport
		return NULL;
	}

	// type = 1 + floor(Life * frand());

	if(type == MiniPower && player_is_powersurge_active(&global.plr)) {
		type = Surge;
	}

	Item *i = (Item*)objpool_acquire(stage_object_pools.items);
	alist_append(&global.items, i);

	i->pos = pos;
	i->pos0 = pos;
	i->v = v;
	i->birthtime = global.frames;
	i->auto_collect = 0;
	i->type = type;

	i->ent.draw_layer = LAYER_ITEM | i->type;
	i->ent.draw_func = ent_draw_item;
	ent_register(&i->ent, ENT_ITEM);

	return i;
}

void delete_item(Item *item) {
	ent_unregister(&item->ent);
	objpool_release(stage_object_pools.items, &alist_unlink(&global.items, item)->object_interface);
}

Item* create_bpoint(complex pos) {
	Item *i = create_item(pos, 0, BPoint);

	if(i) {
		PARTICLE(
			.sprite = "flare",
			.pos = pos,
			.timeout = 30,
			.draw_rule = Fade,
			.layer = LAYER_BULLET+1
		);
		collect_item(i, 1, 10);
	}

	return i;
}

void delete_items(void) {
	for(Item *i = global.items.first, *next; i; i = next) {
		next = i->next;
		delete_item(i);
	}
}

static complex move_item(Item *i) {
	int t = global.frames - i->birthtime;
	complex lim = 0 + 2.0*I;

	complex oldpos = i->pos;

	if(i->auto_collect) {
		i->pos -= (7+i->auto_collect)*cexp(I*carg(i->pos - global.plr.pos));
	} else {
		complex oldpos = i->pos;
		i->pos = i->pos0 + log(t/5.0 + 1)*5*(i->v + lim) + lim*t;

		complex v = i->pos - oldpos;
		double half = item_sprite(i->type)->w/2.0;
		bool over = false;

		if((over = creal(i->pos) > VIEWPORT_W-half) || creal(i->pos) < half) {
			complex normal = over ? -1 : 1;
			v -= 2 * normal * (creal(normal)*creal(v));
			v = 1.5*creal(v) - I*fabs(cimag(v));

			i->pos = clamp(creal(i->pos), half, VIEWPORT_W-half) + I*cimag(i->pos);
			i->v = v;
			i->pos0 = i->pos;
			i->birthtime = global.frames;
		}
	}

	return i->pos - oldpos;
}

static bool item_out_of_bounds(Item *item) {
	double margin = max(item_sprite(item->type)->w, item_sprite(item->type)->h);

	return (
		creal(item->pos) < -margin ||
		creal(item->pos) > VIEWPORT_W + margin ||
		cimag(item->pos) > VIEWPORT_H + margin
	);
}

bool collect_item(Item *item, float value, float speed) {
	if(item->auto_collect || !player_is_alive(&global.plr)) {
		return false;
	}

	item->auto_collect = speed;
	item->pickup_value = clamp(value, ITEM_MIN_VALUE, ITEM_MAX_VALUE);

	return true;
}

void collect_all_items(float value, float speed) {
	for(Item *i = global.items.first; i; i = i->next) {
		collect_item(i, value, speed);
	}
}

void process_items(void) {
	Item *item = global.items.first, *del = NULL;
	float r = player_property(&global.plr, PLR_PROP_COLLECT_RADIUS);
	bool plr_alive = player_is_alive(&global.plr);

	while(item != NULL) {
		bool may_collect = true;

		if(item->type == Surge && !player_is_powersurge_active(&global.plr)) {
			item->type = BPoint;
		}

		if(global.stage->type == STAGE_SPELL && (item->type == Life || item->type == Bomb)) {
			// just in case we ever have some weird spell that spawns those...
			item->type = Point;
		}

		if(global.frames - item->birthtime < 20) {
			may_collect = false;
		}

		if(may_collect) {
			if(plr_alive) {
				if(cimag(global.plr.pos) < player_property(&global.plr, PLR_PROP_POC)) {
					collect_item(item, 1, 1);
				} else if(cabs(global.plr.pos - item->pos) < r) {
					collect_item(item, 1 - cimag(global.plr.pos) / VIEWPORT_H, 1);
				}
			} else if(item->auto_collect) {
				item->auto_collect = 0;
				item->pos0 = item->pos;
				item->birthtime = global.frames;
				item->v = -10*I + 5*nfrand();
			}
		}

		complex deltapos = move_item(item);
		int v = may_collect ? collision_item(item) : 0;

		if(v == 1) {
			switch(item->type) {
			case Power:
				player_add_power(&global.plr, POWER_VALUE);
				player_add_points(&global.plr, 25);
				player_extend_powersurge(&global.plr, 24);
				play_sound("item_generic");
				break;
			case MiniPower:
				player_add_power(&global.plr, POWER_VALUE_MINI);
				player_add_points(&global.plr, 5);
				play_sound("item_generic");
				break;
			case Surge:
				player_extend_powersurge(&global.plr, 8);
				player_add_points(&global.plr, 25);
				play_sound("item_generic");
				break;
			case Point:
				player_add_points(&global.plr, round(global.plr.point_item_value * item->pickup_value));
				play_sound("item_generic");
				break;
			case BPoint:
				player_add_piv(&global.plr, 1);
				play_sound("item_generic");
				break;
			case Life:
				player_add_lives(&global.plr, 1);
				break;
			case Bomb:
				player_add_bombs(&global.plr, 1);
				break;
			case LifeFrag:
				player_add_life_fragments(&global.plr, 1);
				break;
			case BombFrag:
				player_add_bomb_fragments(&global.plr, 1);
				break;
			}
		}

		if(v == 1 || (cimag(deltapos) > 0 && item_out_of_bounds(item))) {
			del = item;
			item = item->next;
			delete_item(del);
		} else {
			item = item->next;
		}
	}
}

int collision_item(Item *i) {
	if(cabs(global.plr.pos - i->pos) < 10)
		return 1;

	return 0;
}

void spawn_item(complex pos, ItemType type) {
	tsrand_fill(2);
	create_item(pos, (12 + 6 * afrand(0)) * (cexp(I*(3*M_PI/2 + anfrand(1)*M_PI/11))) - 3*I, type);
}

void spawn_items(complex pos, ...) {
	va_list args;
	va_start(args, pos);

	ItemType type;

	while((type = va_arg(args, ItemType))) {
		int num = va_arg(args, int);
		for(int i = 0; i < num; ++i) {
			spawn_item(pos, type);
		}
	}

	va_end(args);
}

void items_preload(void) {
	preload_resources(RES_SPRITE, RESF_PERMANENT,
		"item/power",
		"item/minipower",
		"item/surge",
		"item/point",
		"item/life",
		"item/bomb",
		"item/lifefrag",
		"item/bombfrag",
		"item/bullet_point",
	NULL);

	preload_resources(RES_SFX, RESF_OPTIONAL,
		"item_generic",
	NULL);
}
