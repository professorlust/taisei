/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "taisei.h"

#include "global.h"
#include "plrmodes.h"
#include "marisa.h"
#include "stagedraw.h"

PlayerCharacter character_marisa = {
	.id = PLR_CHAR_MARISA,
	.lower_name = "marisa",
	.proper_name = "Marisa",
	.full_name = "Kirisame Marisa",
	.title = "Ordinary Black Magician",
	.dialog_sprite_name = "dialog/marisa",
	.player_sprite_name = "player/marisa",
	.ending = {
		.good = good_ending_marisa,
		.bad = bad_ending_marisa,
	},
};

double marisa_common_property(Player *plr, PlrProperty prop) {
	switch(prop) {
		case PLR_PROP_BOMB_TIME:
			return 300;

		case PLR_PROP_COLLECT_RADIUS:
			return (plr->inputflags & INFLAG_FOCUS) ? 60 : 30;

		case PLR_PROP_SPEED:
			// NOTE: For equivalents in Touhou units, divide these by 1.25.
			return (plr->inputflags & INFLAG_FOCUS) ? 2.75 : 6.25;

		case PLR_PROP_POC:
			return VIEWPORT_H / 3.5;

		case PLR_PROP_DEATHBOMB_WINDOW:
			return 12;
	}

	UNREACHABLE;
}

void marisa_common_shot(Player *plr, float dmg) {
	play_loop("generic_shot");

	if(!(global.frames % 6)) {
		Color *c = RGB(1, 1, 1);

		for(int i = -1; i < 2; i += 2) {
			PROJECTILE(
				.proto = pp_marisa,
				.pos = plr->pos + 10 * i - 15.0*I,
				.color = c,
				.rule = linear,
				.args = { -20.0*I },
				.type = PlrProj,
				.damage = dmg,
				.shader = "sprite_default",
			);
		}
	}
}

void marisa_common_slave_visual(Enemy *e, int t, bool render) {
	if(!render) {
		return;
	}

	r_draw_sprite(&(SpriteParams) {
		.sprite = "hakkero",
		.shader = "sprite_hakkero",
		.pos = { creal(e->pos), cimag(e->pos) },
		.rotation.angle = t * 0.05,
		.color = RGB(0.2, 0.4, 0.5),
	});
}

static void draw_masterspark_ring(int t, float width) {
	float sy = sqrt(t / 500.0) * 6;
	float sx = sy * width / 800;

	if(sx == 0 || sy == 0) {
		return;
	}

	r_draw_sprite(&(SpriteParams) {
		.sprite = "masterspark_ring",
		.shader = "sprite_default",
		.pos = { 0, -t*t*0.4 + 2 },
		.color = RGBA(0.5, 0.5, 0.5, 0.0),
		.scale = { .x = sx, .y = sy * sy * 1.5 },
	});
}

static void draw_masterspark_beam(complex origin, complex size, float angle, int t, float alpha) {
	r_mat_push();
	r_mat_translate(creal(origin), cimag(origin), 0);
	r_mat_rotate(angle, 0, 0, 1);

	r_shader("masterspark");
	r_uniform_float("t", t);

	r_mat_push();
	r_mat_translate(0, cimag(size) * -0.5, 0);
	r_mat_scale(alpha * creal(size), cimag(size), 1);
	r_draw_quad();
	r_mat_pop();

	for(int i = 0; i < 4; i++) {
		draw_masterspark_ring(t % 20 + 10 * i, alpha * creal(size));
	}

	r_mat_pop();
}

void marisa_common_masterspark_draw(complex origin, complex size, float angle, int t, float alpha) {
	r_state_push();

	float blur = 1.0 - alpha;
	int pp_quality = config_get_int(CONFIG_POSTPROCESS);

	if(pp_quality < 1 || (pp_quality < 2 && blur == 0)) {
		Framebuffer *main_fb = r_framebuffer_current();
		FBPair *aux = stage_get_fbpair(FBPAIR_FG_AUX);

		r_framebuffer(aux->back);
		r_clear(CLEAR_COLOR, RGBA(0, 0, 0, 0), 1);
		draw_masterspark_beam(origin, size, angle, t, alpha);
		fbpair_swap(aux);
		r_framebuffer(main_fb);
		r_shader_standard();
		r_color4(1, 1, 1, 1);
		draw_framebuffer_tex(aux->front, VIEWPORT_W, VIEWPORT_H);
	} else if(blur == 0) {
		draw_masterspark_beam(origin, size, angle, t, alpha);
	} else {
		Framebuffer *main_fb = r_framebuffer_current();
		FBPair *aux = stage_get_fbpair(FBPAIR_FG_AUX);

		r_framebuffer(aux->back);
		r_clear(CLEAR_COLOR, RGBA(0, 0, 0, 0), 1);

		draw_masterspark_beam(origin, size, angle, t, alpha);

		if(pp_quality > 1) {
			r_shader("blur25");
		} else {
			r_shader("blur5");
		}

		r_uniform_vec2("blur_resolution", VIEWPORT_W, VIEWPORT_H);
		r_uniform_vec2("blur_direction", blur, 0);

		fbpair_swap(aux);
		r_framebuffer(aux->back);
		r_clear(CLEAR_COLOR, RGBA(0, 0, 0, 0), 1);
		draw_framebuffer_tex(aux->front, VIEWPORT_W, VIEWPORT_H);

		r_uniform_vec2("blur_direction", 0, blur);

		fbpair_swap(aux);
		r_framebuffer(main_fb);
		draw_framebuffer_tex(aux->front, VIEWPORT_W, VIEWPORT_H);
	}

	r_state_pop();
}
