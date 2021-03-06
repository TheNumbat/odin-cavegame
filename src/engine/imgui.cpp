
#include "imgui.h"
#include "util/reflect.h"
#include "log.h"
#include "dbg.h"

const GLchar* imgui_vertex_shader =
	"#version 330\n"
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = UV;\n"
    "	Frag_Color = Color;\n"
    "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

const GLchar* imgui_fragment_shader =
	"#version 330\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";

namespace ImGui {

	bool TreeNodeNoNull(string label) {
	    return TreeNodeL(label.c_str, label.c_str + label.len);
	}

	bool InputText(string label, string buf, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data) {
		return InputText(label.c_str, buf.c_str, buf.cap, flags, callback, user_data);
	}

	void Text(string text) {
		return TextUnformatted(text.c_str, text.c_str + text.len - 1);
	}

	void EnumCombo_T(string label, void* val, _type_info* info, ImGuiComboFlags flags) { 

		if(!info) return;

		_type_info* base = TYPEINFO_H(info->_enum.base_type);
		i64 ival = int_as_i64(val, base);

		if(BeginCombo(label, enum_to_string(ival, info), flags)) {
			for(u32 i = 0; i < info->_enum.member_count; i++) {
				
				bool selected = ival == info->_enum.member_values[i];
				if(Selectable(info->_enum.member_names[i], selected)) {
					int_from_i64(info->_enum.member_values[i], val, base);
				}
				if(selected) {
					SetItemDefaultFocus();
				}
			}
			EndCombo();
		}
	}

	constexpr u32 const_hash(const char* str) {

	    return *str ? (u32)(*str) + 33 * const_hash(str + 1) : 5381;
	}

	void View_T(string label, void* val, _type_info* info, bool open) { 

		if(!info) return;

		if(info->type_type != Type::_array && info->type_type != Type::_struct) {
			Text("%s", label.c_str); SameLine();
		}

		switch(info->type_type) {
		case Type::_int: {
			if(info->_int.is_signed) {
				if(info->size == 1) {
					Text("%hh", *(i8*)val);
				} else if(info->size == 2) {
					Text("%h", *(i16*)val);
				} else if(info->size == 4) {
					Text("%d", *(i32*)val);
				} else if(info->size == 8) {
					Text("%ll", *(i64*)val);
				}
			} else {
				if(info->size == 1) {
					Text("%hhu", *(u8*)val);
				} else if(info->size == 2) {
					Text("%hu", *(u16*)val);
				} else if(info->size == 4) {
					Text("%u", *(u32*)val);
				} else if(info->size == 8) {
					Text("%lu", *(u64*)val);
				}
			}
		} break;

		case Type::_float: {
			Text("%f", float_as_f64(val, info));
		} break;

		case Type::_bool: {
			*(bool*)val ? Text("true") : Text("false");
		} break;

		case Type::_ptr: {
			Text("%p", *(void**)val);
		} break;

		case Type::_array: {
			if(TreeNodeEx(label, open ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
				
				_type_info* of = TYPEINFO_H(info->_array.of);

				for(u32 i = 0; i < info->_array.length; i++) {

					u8* place = (u8*)val + i * of->size;
					PushID(i);
					View_T(""_, place, of);
					PopID();
				}
				TreePop();
			}
		} break;

		// TODO(max): specializations for data structures
		case Type::_struct: {
			u32 name = const_hash(info->name.c_str);

			switch(name) {
			case const_hash("v2"): {
				v2* v = (v2*)val;
				Text("%s {%f,%f}", label.c_str, v->x, v->y);
			} break;
			case const_hash("v3"): {
				v3* v = (v3*)val;
				Text("%s {%f,%f,%f}", label.c_str, v->x, v->y, v->z);
			} break;
			case const_hash("v4"): {
				v4* v = (v4*)val;
				Text("%s {%f,%f,%f,%f}", label.c_str, v->x, v->y, v->z, v->w);
			} break;
			default: {
				if(TreeNodeEx(label, open ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {

					for(u32 i = 0; i < info->_struct.member_count; i++) {

						string member_name = info->_struct.member_names[i];
						_type_info* member = TYPEINFO_H(info->_struct.member_types[i]);
						u8* place = (u8*)val + info->_struct.member_offsets[i];

						View_T(member_name, place, member);
					}
					TreePop();
				}
			} break;
			}
		} break;

		case Type::_enum: {
			Text(enum_to_string(int_as_i64(val, TYPEINFO_H(info->_enum.base_type)), info));
		} break;

		case Type::_string: {
			Text("\"%s\"", (*(string*)val).c_str);
		} break;

		default: break;
		}
	}

	void Edit_T(string label, void* val, _type_info* info, bool open) { 

		if(!info) return;

		switch(info->type_type) {
		case Type::_int: {
			if(info->_int.is_signed) {
				if(info->size == 4) {
					InputScalar(label, ImGuiDataType_S32, val);
				} else if(info->size == 8) {
					InputScalar(label, ImGuiDataType_S64, val);
				} else {
					LOG_ERR("Edit int not 32/64 bit!"_);
				}
			} else {
				if(info->size == 4) {
					InputScalar(label, ImGuiDataType_U32, val);
				} else if(info->size == 8) {
					InputScalar(label, ImGuiDataType_U64, val);
				} else {
					LOG_ERR("Edit int not 32/64 bit!"_);
				}
			}
		} break;

		case Type::_float: {
			if(info->size == 4) {
				InputScalar(label, ImGuiDataType_Float, val);
			} else {
				InputScalar(label, ImGuiDataType_Double, val);
			}
		} break;

		case Type::_bool: {
			Checkbox(label, (bool*)val);
		} break;

		case Type::_ptr: {
			Text("%p", *(void**)val);
		} break;

		case Type::_array: {
			_type_info* of = TYPEINFO_H(info->_array.of);
			if(of->hash == (type_id)typeid(char).hash_code()) {
				InputText(label, (char*)val, info->_array.length);
			} else if(TreeNodeEx(label, open ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {

				for(u32 i = 0; i < info->_array.length; i++) {

					u8* place = (u8*)val + i * of->size;
					PushID(i);
					Edit_T(""_, place, of);
					PopID();
				}
				TreePop();
			}
		} break;

		// TODO(max): specializations for data structures
		// TODO(max): reflect member range labels for sliders
		case Type::_struct: {
			u32 name = const_hash(info->name.c_str);

			switch(name) {
			case const_hash("v2"): {
				InputFloat2(label, ((v2*)val)->a);
			} break;
			case const_hash("v3"): {
				InputFloat3(label, ((v3*)val)->a);
			} break;
			case const_hash("v4"): {
				InputFloat4(label, ((v4*)val)->a);
			} break;
			default: {
				if(TreeNodeEx(label, open ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {

					for(u32 i = 0; i < info->_struct.member_count; i++) {

						string member_name = info->_struct.member_names[i];
						_type_info* member = TYPEINFO_H(info->_struct.member_types[i]);
						u8* place = (u8*)val + info->_struct.member_offsets[i];

						Edit_T(member_name, place, member);
					}
					TreePop();
				}
			} break;
			}
		} break;

		case Type::_enum: {
			EnumCombo_T(label, val, info, 0);
		} break;

		case Type::_string: {
			// NOTE(max): DANGER! If string is statically (as in text segment) allocated, editing it will crash the program
			string s = *(string*)val;
			InputText(label, s.c_str, s.cap);
		} break;

		default: break;
		}
	}
}

void* imgui_alloc(u64 size, void* data) { 

	allocator* a = (allocator*)data;
	return a->allocate_(size, 0, a, CONTEXT);
}

void imgui_free(void* mem, void* data) { 

	if (mem) {
		allocator* a = (allocator*)data;
		a->free_(mem, 0, a, CONTEXT);
	}
}

const char* imgui_get_clipboard(void* data) { 

	return global_api->get_clipboard().c_str;
}

void imgui_set_clipboard(void* data, const char* text_utf8) { 

	global_api->set_clipboard(string::literal(text_utf8));
}

imgui_manager imgui_manager::make(platform_window* window, allocator* a) { 

	imgui_manager ret;

	a->track_sizes = false;
	ret.alloc = a;
	ret.perf_freq = global_api->get_perfcount_freq();

	ImGui::SetAllocatorFunctions(imgui_alloc, imgui_free, a);
	
	ret.context = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 0.0f;

	io.IniFilename = null;

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.KeyMap[ImGuiKey_Tab]        = (i32)platform_keycode::tab;
	io.KeyMap[ImGuiKey_LeftArrow]  = (i32)platform_keycode::left;
	io.KeyMap[ImGuiKey_RightArrow] = (i32)platform_keycode::right;
	io.KeyMap[ImGuiKey_UpArrow]    = (i32)platform_keycode::up;
	io.KeyMap[ImGuiKey_DownArrow]  = (i32)platform_keycode::down;
	io.KeyMap[ImGuiKey_PageUp]     = (i32)platform_keycode::pgup;
	io.KeyMap[ImGuiKey_PageDown]   = (i32)platform_keycode::pgdown;
	io.KeyMap[ImGuiKey_Home]       = (i32)platform_keycode::home;
	io.KeyMap[ImGuiKey_End]        = (i32)platform_keycode::end;
	io.KeyMap[ImGuiKey_Insert]     = (i32)platform_keycode::insert;
	io.KeyMap[ImGuiKey_Delete]     = (i32)platform_keycode::del;
	io.KeyMap[ImGuiKey_Backspace]  = (i32)platform_keycode::backspace;
	io.KeyMap[ImGuiKey_Space]      = (i32)platform_keycode::space;
	io.KeyMap[ImGuiKey_Enter]      = (i32)platform_keycode::enter;
	io.KeyMap[ImGuiKey_Escape]     = (i32)platform_keycode::escape;
	io.KeyMap[ImGuiKey_A]          = (i32)platform_keycode::a;
	io.KeyMap[ImGuiKey_C]          = (i32)platform_keycode::c;
	io.KeyMap[ImGuiKey_V]          = (i32)platform_keycode::v;
	io.KeyMap[ImGuiKey_X]          = (i32)platform_keycode::x;
	io.KeyMap[ImGuiKey_Y]          = (i32)platform_keycode::y;
	io.KeyMap[ImGuiKey_Z]          = (i32)platform_keycode::z;

	ret.cursor_values[(i32)ImGuiMouseCursor_Arrow] = platform_cursor::pointer;
	ret.cursor_values[(i32)ImGuiMouseCursor_TextInput] = platform_cursor::I;
	ret.cursor_values[(i32)ImGuiMouseCursor_ResizeNS] = platform_cursor::hand;
	ret.cursor_values[(i32)ImGuiMouseCursor_ResizeEW] = platform_cursor::hand;
	ret.cursor_values[(i32)ImGuiMouseCursor_ResizeNESW] = platform_cursor::hand;
	ret.cursor_values[(i32)ImGuiMouseCursor_ResizeNWSE] = platform_cursor::hand;

    io.SetClipboardTextFn = imgui_set_clipboard;
    io.GetClipboardTextFn = imgui_get_clipboard;
    io.ClipboardUserData = null;

    ret.gl_load();

	return ret;
}

void imgui_manager::gl_destroy() {

	glDeleteBuffers(1, &gl_info.vbo);
	glDeleteBuffers(1, &gl_info.ebo);

	glUseProgram(0);
	glDeleteProgram(gl_info.program);
	glDeleteShader(gl_info.vertex);
	glDeleteShader(gl_info.fragment);

	glDeleteVertexArrays(1, &gl_info.vao);
	
	glDeleteTextures(1, &gl_info.font_texture);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;

	gl_info = {};
}

void imgui_manager::gl_load() {

	gl_info.program = glCreateProgram();
	gl_info.vertex = glCreateShader(gl_shader_type::vertex);
	gl_info.fragment = glCreateShader(gl_shader_type::fragment);
	glShaderSource(gl_info.vertex, 1, &imgui_vertex_shader, null);
	glShaderSource(gl_info.fragment, 1, &imgui_fragment_shader, null);
	glCompileShader(gl_info.vertex);
	glCompileShader(gl_info.fragment);
	glAttachShader(gl_info.program, gl_info.vertex);
	glAttachShader(gl_info.program, gl_info.fragment);
	glLinkProgram(gl_info.program);

	gl_info.tex_loc   = glGetUniformLocation(gl_info.program, "Texture");
	gl_info.mat_loc   = glGetUniformLocation(gl_info.program, "ProjMtx");
	gl_info.pos_loc   = glGetAttribLocation(gl_info.program, "Position");
	gl_info.uv_loc    = glGetAttribLocation(gl_info.program, "UV");
	gl_info.color_loc = glGetAttribLocation(gl_info.program, "Color");

	glGenVertexArrays(1, &gl_info.vao);
	glBindVertexArray(gl_info.vao);

	glGenBuffers(1, &gl_info.vbo);
	glGenBuffers(1, &gl_info.ebo);
	
	glBindBuffer(gl_buf_target::array, gl_info.vbo);
	glBindBuffer(gl_buf_target::element_array, gl_info.ebo);

	glEnableVertexAttribArray(gl_info.pos_loc);
	glEnableVertexAttribArray(gl_info.uv_loc);
	glEnableVertexAttribArray(gl_info.color_loc);

	glVertexAttribPointer(gl_info.pos_loc, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
	glVertexAttribPointer(gl_info.uv_loc, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
	glVertexAttribPointer(gl_info.color_loc, 4, gl_vert_attrib_type::unsigned_byte, gl_bool::_true, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));

	glBindVertexArray(0);

	load_font();
}

void imgui_manager::set_font(string name, f32 size, asset_store* store) { 

	font_asset_name = name;
	font_size = size;
	load_font(store);
}

void imgui_manager::load_font(asset_store* store) { 

	if(!store) store = last_store;
	else last_store = store;

	ImGuiIO io = ImGui::GetIO();
	if(gl_info.font_texture) {
		glDeleteTextures(1, &gl_info.font_texture);
		
		gl_info.font_texture = 0;
		io.Fonts->TexID = 0;
	}

	io.Fonts->ClearInputData();
	if(font_asset_name && store) {
		asset* font = store->get(font_asset_name);

		ImFontConfig cfg;
		cfg.FontData = font->mem;
		cfg.FontDataSize = (i32)font->ttf_font.file_size;
		cfg.FontDataOwnedByAtlas = false;
		cfg.SizePixels = font_size;

		io.Fonts->AddFont(&cfg);
	} else {
		io.Fonts->AddFontDefault();
	}

	u8* bitmap;
	i32 w, h;
	io.Fonts->GetTexDataAsRGBA32(&bitmap, &w, &h);
	
	glGenTextures(1, &gl_info.font_texture);
	glBindTexture(gl_tex_target::_2D, gl_info.font_texture);
	glTexParameteri(gl_tex_target::_2D, gl_tex_param::min_filter, (GLint)gl_tex_filter::nearest);
	glTexParameteri(gl_tex_target::_2D, gl_tex_param::mag_filter, (GLint)gl_tex_filter::nearest);
	glPixelStorei(gl_pix_store::unpack_row_length, 0);
	glTexImage2D(gl_tex_target::_2D, 0, gl_tex_format::rgba, w, h, 0, gl_pixel_data_format::rgba, gl_pixel_data_type::unsigned_byte, bitmap);
	glBindTexture(gl_tex_target::_2D, 0);

	io.Fonts->TexID = (void*)(u64)gl_info.font_texture;
}

void imgui_manager::destroy() { 

	gl_destroy();

	ImGui::DestroyContext(context);
	context = null;
}

void imgui_manager::reload() { 

	ImGui::SetAllocatorFunctions(imgui_alloc, imgui_free, alloc);
	ImGui::SetCurrentContext(context);
}

void imgui_manager::process_event(platform_event evt) { 

	ImGuiIO& io = ImGui::GetIO();

	if(evt.type == platform_event_type::key) {

		io.KeysDown[(i32)evt.key.code] = (evt.key.flags & (u16)platform_keyflag::press) != 0 || 
										 (evt.key.flags & (u16)platform_keyflag::repeat) != 0;
		io.KeyShift = (evt.key.flags & (u16)platform_keyflag::shift) != 0;
		io.KeyCtrl 	= (evt.key.flags & (u16)platform_keyflag::ctrl) != 0;
		io.KeyAlt 	= (evt.key.flags & (u16)platform_keyflag::alt) != 0;
	}

	else if(evt.type == platform_event_type::mouse) {
	
		if(evt.mouse.flags & (u16)platform_mouseflag::press) {

			mouse[0] = (evt.mouse.flags & (u16)platform_mouseflag::lclick) != 0;
			mouse[1] = (evt.mouse.flags & (u16)platform_mouseflag::rclick) != 0;
			mouse[2] = (evt.mouse.flags & (u16)platform_mouseflag::mclick) != 0;
		}

		else if(evt.mouse.flags & (u16)platform_mouseflag::wheel) {

			io.MouseWheel += evt.mouse.w;
		}
	}

	else if(evt.type == platform_event_type::rune) {

		io.AddInputCharactersUTF8(evt.rune.rune_utf8);
	}
}

void imgui_manager::begin_frame(platform_window* window) { PROF_FUNC

	ImGuiIO& io = ImGui::GetIO();

    int w, h;
    int display_w, display_h;
    global_api->get_window_size(window, &w, &h);
    global_api->get_window_drawable(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((f32)w, (f32)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((f32)display_w / w) : 0, h > 0 ? ((f32)display_h / h) : 0);

	u64 perf = global_api->get_perfcount();
	
	io.DeltaTime = (f32)((f64)(perf - last_perf) / perf_freq);
	last_perf = perf;

	if(io.WantSetMousePos)
		CHECKED(set_cursor_pos, window, (i32)io.MousePos.x, (i32)io.MousePos.y);

	io.MouseDown[0] = mouse[0] || global_api->mousedown(platform_mouseflag::lclick);
	io.MouseDown[1] = mouse[1] || global_api->mousedown(platform_mouseflag::rclick);
	io.MouseDown[2] = mouse[2] || global_api->mousedown(platform_mouseflag::mclick);
	mouse[0] = mouse[1] = mouse[2] = false;

	if(global_api->window_focused(window) && global_api->cursor_shown()) {
		i32 mx, my;
		CHECKED(get_cursor_pos, window, &mx, &my);
		io.MousePos = ImVec2((f32)mx, (f32)my);
	} else {
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	global_api->set_cursor(window, cursor_values[ImGui::GetMouseCursor()]);

	ImGui::NewFrame();
}

void imgui_manager::end_frame() { PROF_FUNC

	ImGui::Render();

	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGuiIO& io = ImGui::GetIO();

    i32 fb_width = (i32)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    i32 fb_height = (i32)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

	glEnable(gl_capability::blend);
	glBlendEquation(gl_blend_mode::add);
	glBlendFunc(gl_blend_factor::src_alpha, gl_blend_factor::one_minus_src_alpha);
	glDisable(gl_capability::cull_face);
	glDisable(gl_capability::depth_test);
	glEnable(gl_capability::scissor_test);
	glPolygonMode(gl_face::front_and_back, gl_poly_mode::fill);

	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	m4 mat = ortho(0, (f32)fb_width, (f32)fb_height, 0, -1, 1);

	glBindFramebuffer(gl_framebuffer::val, 0);
	glDisable(gl_capability::framebuffer_srgb);
	glUseProgram(gl_info.program);
	glUniform1i(gl_info.tex_loc, 0);
	glUniformMatrix4fv(gl_info.mat_loc, 1, gl_bool::_false, &mat[0][0]);
	glBindVertexArray(gl_info.vao);

	for(i32 n = 0; n < draw_data->CmdListsCount; n++) {

		ImDrawList* cmd_list = draw_data->CmdLists[n];
		ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(gl_buf_target::array, gl_info.vbo);
		glBufferData(gl_buf_target::array, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, gl_buf_usage::stream_draw);
		glBindBuffer(gl_buf_target::element_array, gl_info.ebo);
		glBufferData(gl_buf_target::element_array, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, gl_buf_usage::stream_draw);

		for(i32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {

			ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			if(pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			} else {
				glBindTexture(gl_tex_target::_2D, (GLuint)(u64)pcmd->TextureId);
				glScissor((i32)pcmd->ClipRect.x, (i32)(fb_height - pcmd->ClipRect.w), (i32)(pcmd->ClipRect.z - pcmd->ClipRect.x), (i32)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(gl_draw_mode::triangles, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? gl_index_type::unsigned_short : gl_index_type::unsigned_int, idx_buffer_offset);
			}

			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	glUseProgram(0);
	glBindTexture(gl_tex_target::_2D, 0);
	glBindVertexArray(0);
	glDisable(gl_capability::scissor_test);
}

