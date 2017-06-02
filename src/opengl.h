
#pragma once

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31

#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B

#define GL_MIRRORED_REPEAT                0x8370
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_CLAMP_TO_BORDER                0x812D

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

typedef void (*glDebugProc_t)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
typedef void (*glDebugMessageCallback_t)(glDebugProc_t callback, const void *userParam);
typedef void (*glDebugMessageInsert_t)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message);
typedef void (*glDebugMessageControl_t)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);

typedef void (*glAttachShader_t)(GLuint program, GLuint shader);
typedef void (*glCompileShader_t)(GLuint shader);
typedef GLuint (*glCreateProgram_t)(void);
typedef GLuint (*glCreateShader_t)(GLenum type);
typedef void (*glDeleteProgram_t)(GLuint program);
typedef void (*glDeleteShader_t)(GLuint shader);
typedef void (*glLinkProgram_t)(GLuint program);
typedef void (*glShaderSource_t)(GLuint shader, GLsizei count, const GLchar* const* str, const GLint* length);
typedef void (*glUseProgram_t)(GLuint program);

typedef void (*glGenerateMipmap_t)(GLenum target);

typedef void (*glBindVertexArray_t)(GLuint array);
typedef void (*glDeleteVertexArrays_t)(GLsizei n, const GLuint *arrays);
typedef void (*glGenVertexArrays_t)(GLsizei n, GLuint *arrays);

typedef void (*glBindBuffer_t)(GLenum target, GLuint buffer);
typedef void (*glDeleteBuffers_t)(GLsizei n, const GLuint *buffers);
typedef void (*glGenBuffers_t)(GLsizei n, GLuint *buffers);
typedef void (*glBufferData_t)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

typedef void (*glVertexAttribPointer_t)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (*glEnableVertexAttribArray_t)(GLuint index);

glDebugMessageCallback_t 	glDebugMessageCallback;
glDebugMessageInsert_t		glDebugMessageInsert;
glDebugMessageControl_t		glDebugMessageControl;

glAttachShader_t  			glAttachShader;
glCompileShader_t 			glCompileShader;
glCreateProgram_t 			glCreateProgram;
glCreateShader_t  			glCreateShader;
glDeleteProgram_t 			glDeleteProgram;
glDeleteShader_t  			glDeleteShader;
glLinkProgram_t   			glLinkProgram;
glShaderSource_t  			glShaderSource;
glUseProgram_t    			glUseProgram;

glGenerateMipmap_t			glGenerateMipmap;

glBindVertexArray_t    		glBindVertexArray; 		
glDeleteVertexArrays_t 		glDeleteVertexArrays;
glGenVertexArrays_t    		glGenVertexArrays;

glBindBuffer_t				glBindBuffer;
glDeleteBuffers_t			glDeleteBuffers;
glGenBuffers_t				glGenBuffers;
glBufferData_t				glBufferData;

glVertexAttribPointer_t		glVertexAttribPointer;
glEnableVertexAttribArray_t glEnableVertexAttribArray;

struct shader_source {
	string path;
	platform_file_attributes last_attrib;
	string source;
	allocator* alloc = NULL;
};

typedef u32 shader_program_id;

struct shader_program {
	shader_program_id id = 0;
	GLuint handle = 0;
	shader_source vertex;
	shader_source fragment;
	// tessellation control, evaluation, geometry
};

enum texture_wrap {
	wrap_repeat,
	wrap_mirror,
	wrap_clamp,
	wrap_clamp_border,
};

struct texture {
	GLuint handle 		= 0;
	texture_wrap wrap 	= wrap_repeat;
	bool pixelated 		= false;;
};

struct opengl {
	map<shader_program_id, shader_program> programs;
	shader_program_id dbg_shader = 0;
	shader_program_id next_id = 1;
	allocator* alloc = NULL;
	string version, renderer, vendor;
};

void ogl_load_global_funcs();

opengl make_opengl(allocator* a);
void destroy_opengl(opengl* ogl);
shader_program_id ogl_add_program(opengl* ogl, string v_path, string f_path);
void ogl_select_program(opengl* ogl, shader_program_id id);

void ogl_dbg_render_texture_fullscreen(opengl* ogl, texture* tex);

shader_source make_source(string path, allocator* a);
void load_source(shader_source* source);
void destroy_source(shader_source* source);
bool refresh_source(shader_source* source);

shader_program make_program(string vert, string frag, allocator* a);
void compile_program(shader_program* prog);
void refresh_program(shader_program* prog);
void destroy_program(shader_program* prog);

texture make_texture(texture_wrap wrap = wrap_repeat, bool pixelated = false);
void texture_load_bitmap(texture* tex, asset_store* as, string name);
void destroy_texture(texture* tex);

void debug_proc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userPointer);
