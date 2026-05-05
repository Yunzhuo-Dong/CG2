// This source code is property of the Computer Graphics and Visualization chair of the
// TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
// The main file of the plugin. It defines a class that demonstrates how to register with
// the scene graph, drawing primitives, creating a GUI, using a config file and various
// other parts of the framework.

// Framework core
#include <cgv/base/register.h>
#include <cgv/gui/provider.h>
#include <cgv/gui/trigger.h>
#include <cgv/render/drawable.h>
#include <cgv/render/shader_program.h>
#include <cgv/render/vertex_buffer.h>
#include <cgv/render/attribute_array_binding.h>
#include <cgv/math/ftransform.h>

// Framework standard plugins
#include <cgv_gl/gl/gl.h>

// Local includes
#include "cubes_fractal.h"


// ************************************************************************************/
// Task 0.2a: Create a drawable that provides a (for now, empty) GUI and supports
//            reflection, so that its properties can be set via config file.
class cubes_fractal_render : //like the demo create a herientance structure which has the function in reflection and providing gui and can be rendered
	public cgv::base::base,      // This class supports reflection
	public cgv::gui::provider,   // Instances of this class provde a GUI
	public cgv::render::drawable // Instances of this class can be rendered
{
protected:
	//
	unsigned int re_depth; //recursion depth
	float cube_color_r, cube_color_g, cube_color_b;

	bool vertex_buffer;

	bool inte_structure;

	// Offscreen framebuffer
	cgv::rgb cube_color;
	
	// Geometry buffers

	//one for  interleaved and one for non- interleaved 
	
	//for interleaved we like the demo
	struct vertex {
		cgv::vec3 pos;
		cgv::vec2 tcoord;
		cgv::vec3 normal;
	};

	//for non-interleaved we separate

	std::vector<cgv::vec3> positions;
	std::vector<cgv::vec2> texcoords;
	std::vector<cgv::vec3> normals;

	std::vector<vertex> vertices;
	cgv::render::vertex_buffer vb;
	cgv::render::attribute_array_binding vertex_array;
	cgv::render::vertex_buffer vb_nml;



	cubes_fractal fractal_engine;

	//
	// Task 0.2b: Utilize the cubes_fractal class to render a fractal of hierarchically
	//            transformed cubes. Expose its recursion depth and color properties to GUI
	//            manipulation and reflection. Set reasonable values via the config
	//            file.
	//
public:
	
	cubes_fractal_render()
      :re_depth(1), cube_color_r(0.1f), cube_color_g(0.2f), cube_color_b(0.2f),
      vertex_buffer(false), inte_structure(false)
  {
		cube_color = cgv::rgb(cube_color_r, cube_color_g, cube_color_b);
	}
	
	std::string get_type_name(void) const {

		return "cubes_fractal";
	}

	bool self_reflect(cgv::reflect::reflection_handler& rh) {
	
		return
			rh.reflect_member("re_depth", re_depth) &&
			rh.reflect_member("cube_color_r", cube_color_r) &&
			rh.reflect_member("cube_folor_g", cube_color_g) &&
			rh.reflect_member("cube_color_b", cube_color_b) &&
			rh.reflect_member("vertex_buffer", vertex_buffer);
	}

	// Part of the cgv::base::base interface, should be implemented to respond to write
	// access to reflected data members of this class, e.g. from config file processing
	// or gui interaction.
	void on_set(void* member_ptr)
	{
      if (member_ptr == &cube_color_r || member_ptr == &cube_color_g || member_ptr == &cube_color_b) {
          cube_color = cgv::rgb(cube_color_r, cube_color_g, cube_color_b);
    }
	  if (member_ptr == &inte_structure) {

		  if (auto ctx_ptr = get_context()) {
			  init(*ctx_ptr);
		  }
	  
	  }
		// tell the fractal engine to update the depth and color
		update_member(member_ptr);

		// draw
		post_redraw();
	}

	// Creates the custom geometry for the cubic
	void init_unit_cube_geometry(void)
	{
		// create 8 fundemental vertices of the cube, which are shared between all faces. 
		// We will use them to create the 36 vertices for the 12 triangles that make up the cube. 
		cgv::vec3 V[8] = {
			{-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}, 
			{-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1}  
		};

		// create the 36 vertices for the 12 triangles that make up the cube.
		int indices[36] = {
			0,1,2, 0,2,3, // Front face
			1,5,6, 1,6,2, // Right face
			5,4,7, 5,7,6, // Back face
			4,0,3, 4,3,7, // Left face
			3,2,6, 3,6,7, // Top face
			4,5,1, 4,1,0  // Bottom face
		};
		//normals for each face, which are shared between the two triangles that make up the face. 
		cgv::vec3 N[6] = {
			{ 0,  0,  1}, 
			{ 1,  0,  0}, 
			{ 0,  0, -1},
			{-1,  0,  0}, 
			{ 0,  1,  0}, 
			{ 0, -1,  0}  
		};

		//give restore space to each function
		vertices.resize(36);
		positions.resize(36);
		texcoords.resize(36);
		normals.resize(36);

		// put the data into the vertex array
		for (int i = 0; i < 36; ++i) {
			// for interleaved, we put the data into the vertex array
			vertices[i].pos = V[indices[i]];
			//we have to compute a texcoord for each vertex, which is the normalized position of the vertex in the cube.
			vertices[i].tcoord = (V[indices[i]] + cgv::vec3(1.0f)) * 0.5f;
			// the normal is the same for all vertices of a face, so we can compute it based on the index of the vertex.
			vertices[i].normal = cgv::vec3(N[i / 6]); // each face has 6 vertices, so we can compute the normal index by dividing the vertex index by 6.

			// for non-interleaved, we put the data into the separate arrays
			positions[i] = V[indices[i]];
			texcoords[i] = (V[indices[i]] + cgv::vec3(1.0f)) * 0.5f;
			normals[i] = cgv::vec3(N[i / 6]);
		}
	
	}

	// Part of the cgv::render::drawable interface, can be overwritten if there is some
	// intialization work to be done that needs a set-up and ready graphics context,
	// which usually you don't have at object construction time. Should return true if
	// the initialization was successful, false otherwise.
	bool init(cgv::render::context& ctx)
	{
		// Keep track of success - do it this way (instead of e.g. returning false
		// immediatly) to perform every init step even if some go wrong.
		bool success = true;

		
		cgv::render::shader_program& default_shader =
			ctx.ref_surface_shader_program();
		// - generate actual geometry
		init_unit_cube_geometry();
	


		// Task 0.2c: Implement an option (configurable via GUI and config file) to use a vertex
	
		cgv::render::type_descriptor
			
			vec3type =
			cgv::render::element_descriptor_traits<cgv::vec3>
			::get_type_descriptor(vertices[0].pos);

		//clean up the vertex buffer and array
		if (vertex_array.is_created()) {
			vertex_array.destruct(ctx);
			vb.destruct(ctx);
			vb_nml.destruct(ctx);
		}

		// - create buffer objects,first in the interleaved way, then in the non-interleaved way, depending on the value of the flag.
		if (inte_structure) {
			success = vb.create(ctx, &(vertices[0]), vertices.size()) && success;//vbo
			success = vertex_array.create(ctx) && success;//vao
			success = vertex_array.set_attribute_array(
				ctx, default_shader.get_position_index(), vec3type, vb,
				0, // position is at start of the struct <-> offset = 0
				vertices.size(), // number of position elements in the array
				sizeof(vertex) // stride from one element to next
			) && success;
		

			success = vertex_array.set_attribute_array(
				ctx, default_shader.get_normal_index(), cgv::render::element_descriptor_traits<cgv::vec3>::get_type_descriptor(vertices[0].normal), vb,
				sizeof(cgv::vec3) + sizeof(cgv::vec2), // normals follow after position and texcoord
				vertices.size(), // number of normal elements in the array
				sizeof(vertex) // stride from one element to next
			) && success;

		}
		//non-interleaved
		else {
			success = vb.create(ctx, &(positions[0]), vertices.size()) && success;//vbo
			success = vertex_array.create(ctx) && success;//vao
			success = vertex_array.set_attribute_array(
				ctx, default_shader.get_position_index(), vec3type, vb,
				0, // position is at start of the struct <-> offset = 0
				positions.size(), // number of position elements in the array
				sizeof(cgv::vec3) // stride from one element to next
			) && success;

			success = vb_nml.create(ctx, &(normals[0]), normals.size()) && success;//vbo for normals
			success = vertex_array.set_attribute_array(
				ctx, default_shader.get_normal_index(), cgv::render::element_descriptor_traits<cgv::vec3>::get_type_descriptor(normals[0]), vb_nml,
				0, 
				normals.size(), // number of normal elements in the array
				sizeof(cgv::vec3) // stride from one element to next
			) && success;
		
		}

		return success;
	
	}



	void create_gui(void) {

		// Simple controls.Notifies us of GUI input via the on_set() method.
			// - section header
			add_decorator("Cubes fractal", "heading", "level=1");
		// - the depth
		add_member_control(this, "Recursion depth", re_depth, "value_slider","min=0;max=5;ticks=true");
		// - the Color
		add_member_control(this, "Cube color", cube_color);

		// - the vertex buffer toggle
		add_member_control(this, "Use Vertex Array", vertex_buffer, "check");
		add_member_control(this, "Interleaved Struct", inte_structure, "check");
	
	}

	void draw(cgv::render::context& ctx) {
	//using gui_color and its depth, and using cubes_fractal.cxx to compute, we just need to send two parameter, depth and color
				
		cgv::render::shader_program& default_shader =
        ctx.ref_surface_shader_program();

		// Enable shader program we want to use for drawing
		default_shader.enable(ctx);

		// Set the "color" vertex attribute for all geometry drawn hereafter, except if
			// it explicitely specifies its own color data by means of an attribute array.
			// Note that this only works for shaders that define a vec4 attribute named
			// "color" in their layout specification.
			// We want white to retain the original color information in the texture.

		ctx.set_color(cube_color);
		
		
		if (vertex_buffer) {
			fractal_engine.use_vertex_array(&vertex_array, vertices.size(), GL_TRIANGLES);

		}
		else {
			fractal_engine.use_vertex_array(nullptr, 0, GL_TRIANGLES);
		}


		fractal_engine.draw_recursive(ctx, cube_color, re_depth, 0);
		
		// Disable shader program 
		default_shader.disable(ctx);
	}
};



// Task 0.2c: Implement an option (configurable via GUI and config file) to use a vertex
//            array object for rendering the cubes. The vertex array functionality 
//            should support (again, configurable via GUI and config file) both
//            interleaved (as in cgv_demo.cpp) and non-interleaved attributes.

// < your code here >

// [END] Tasks 0.2a, 0.2b and 0.2c
// ************************************************************************************/


// ************************************************************************************/
// Task 0.2a: register an instance of your drawable.

cgv::base::object_registration<cubes_fractal_render> cubes_fractal_render_registration(
	"cubes_fractal"
);
