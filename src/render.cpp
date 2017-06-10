
mesh_2d make_mesh_2d(allocator* alloc, u32 verts) {

	if(alloc == NULL) {
		alloc = CURRENT_ALLOC();
	}

	mesh_2d ret;

	ret.alloc = alloc;

	ret.verticies = make_vector<v2>(verts, alloc);
	ret.texCoords = make_vector<v3>(verts, alloc);
	ret.colors 	  =	make_vector<v4>(verts, alloc);
	ret.elements  = make_vector<uv3>(verts * 3, alloc); 

	return ret;
}

void destroy_mesh_2d(mesh_2d* m) {

	destroy_vector(&m->verticies);
	destroy_vector(&m->texCoords);
	destroy_vector(&m->colors);
	destroy_vector(&m->elements);
	m->alloc = NULL;
}

void mesh_push_cutrect(mesh_2d* m, r2 r, f32 round, color c) {

	u32 idx = m->verticies.size;

	vector_push(&m->verticies, V2(r.x, r.y + round));
	vector_push(&m->verticies, V2(r.x, r.y + r.h - round));
	vector_push(&m->verticies, V2(r.x + round, r.y + r.h));
	vector_push(&m->verticies, V2(r.x + r.w - round, r.y + r.h));
	vector_push(&m->verticies, V2(r.x + r.w, r.y + r.h - round));
	vector_push(&m->verticies, V2(r.x + r.w, r.y + round));
	vector_push(&m->verticies, V2(r.x + r.w - round, r.y));
	vector_push(&m->verticies, V2(r.x + round, r.y));

	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));

	colorf cf = color_to_f(c);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);

	vector_push(&m->elements, V3u(idx, idx + 1, idx + 2));
	vector_push(&m->elements, V3u(idx, idx + 2, idx + 7));
	vector_push(&m->elements, V3u(idx + 7, idx + 2, idx + 3));
	vector_push(&m->elements, V3u(idx + 7, idx + 6, idx + 3));
	vector_push(&m->elements, V3u(idx + 3, idx + 4, idx + 5));
	vector_push(&m->elements, V3u(idx + 3, idx + 5, idx + 6));
}

void mesh_push_rect(mesh_2d* m, r2 r, color c) {

	u32 idx = m->verticies.size;

	vector_push(&m->verticies, V2(r.x, r.y + r.h)); // BLC
	vector_push(&m->verticies, r.xy);				// TLC
	vector_push(&m->verticies, add(r.xy, r.wh));	// BRC
	vector_push(&m->verticies, V2(r.x + r.w, r.y));	// TRC

	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));
	vector_push(&m->texCoords, V3(0.0f, 0.0f, 0.0f));

	colorf cf = color_to_f(c);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);
	vector_push(&m->colors, cf);

	vector_push(&m->elements, V3u(idx, idx + 1, idx + 2));
	vector_push(&m->elements, V3u(idx + 1, idx + 2, idx + 3));
}

f32 mesh_push_text_line(mesh_2d* m, asset* font, string text_utf8, v2 pos, f32 point, color c) {

	colorf cf = color_to_f(c);
	f32 x = pos.x;
	f32 y = pos.y;
	f32 scale = point / font->font.point;
	if(point == 0.0f) {
		scale = 1.0f;
	}
	y += scale * font->font.linedist;

	u32 index = 0;
	while(u32 codepoint = get_next_codepoint(text_utf8, &index)) {

		u32 idx = m->verticies.size;
		glyph_data glyph = get_glyph_data(font, codepoint);

		f32 w = (f32)font->font.width;
		f32 h = (f32)font->font.height;
		v3 tlc = V3(glyph.x1/w, 1.0f - glyph.y1/h, 1.0f);
		v3 brc = V3(glyph.x2/w, 1.0f - glyph.y2/h, 1.0f);
		v3 trc = V3(glyph.x2/w, 1.0f - glyph.y1/h, 1.0f);
		v3 blc = V3(glyph.x1/w, 1.0f - glyph.y2/h, 1.0f);

		vector_push(&m->verticies, V2(x + scale*glyph.xoff1, y + scale*glyph.yoff2)); 	// BLC
 		vector_push(&m->verticies, V2(x + scale*glyph.xoff1, y + scale*glyph.yoff1));	// TLC
 		vector_push(&m->verticies, V2(x + scale*glyph.xoff2, y + scale*glyph.yoff2));	// BRC
 		vector_push(&m->verticies, V2(x + scale*glyph.xoff2, y + scale*glyph.yoff1));	// TRC

		vector_push(&m->texCoords, blc);
		vector_push(&m->texCoords, tlc);
		vector_push(&m->texCoords, brc);
		vector_push(&m->texCoords, trc);

		vector_push(&m->colors, cf);
		vector_push(&m->colors, cf);
		vector_push(&m->colors, cf);
		vector_push(&m->colors, cf);

		vector_push(&m->elements, V3u(idx, idx + 1, idx + 2));
		vector_push(&m->elements, V3u(idx + 1, idx + 2, idx + 3));

		x += scale * glyph.advance;
	}

	return scale * font->font.linedist;
}

mesh_3d make_mesh_3d(allocator* alloc, u32 verts, u32 inds) {

	if(alloc == NULL) {
		alloc = CURRENT_ALLOC();
	}

	mesh_3d ret;

	ret.alloc = alloc;

	ret.verticies = make_vector<v3>(verts, alloc);
	ret.texCoords = make_vector<v2>(verts, alloc);

	return ret;
}

void destroy_mesh_3d(mesh_3d* m) {

	destroy_vector(&m->verticies);
	destroy_vector(&m->texCoords);

	m->alloc = NULL;	
}

render_command make_render_command(render_command_type type, void* data) {

	render_command ret;

	ret.cmd = type;
	ret.data = data;

	return ret;
}

render_command_list make_command_list(allocator* alloc, u32 cmds) {

	if(alloc == NULL) {
		alloc = CURRENT_ALLOC();
	}

	render_command_list ret;

	ret.alloc = alloc;

	ret.commands = make_vector<render_command>(cmds, alloc);

	return ret;
}

void destroy_command_list(render_command_list* rcl) {

	destroy_vector(&rcl->commands);
	rcl->alloc = NULL;
}

void render_add_command(render_command_list* rcl, render_command rc) {

	vector_push(&rcl->commands, rc);
}