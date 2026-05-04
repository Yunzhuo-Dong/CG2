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
	unsigned int re_depth;//recursion depth

	// Background color of the offscreen framebuffer. We use individual floats because
	// the cgv::media::color datatype does not currently support reflection, and fvecs
	// can not currently be set easily in a config file. We still want to use
	// cgv::media::color because of its nice GUI representation, so we have to copy the
	// values around.
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
	};

	//for non-interleaved we separate

	std::vector<cgv::vec3> postions;
	std::vector<cgv::vec2> texcoords;

	std::vector<vertex> vertices;
	cgv::render::vertex_buffer vb;
	cgv::render::attribute_array_binding vertex_array;

	cubes_fractal fractal_engine;

	//
	// Task 0.2b: Utilize the cubes_fractal class to render a fractal of hierarchically
	//            transformed cubes. Expose its recursion depth and color properties to GUI
	//            manipulation and reflection. Set reasonable values via the config
	//            file.
	//
public:
	
	cubes_fractal_render() {

		//give parameter to depteh
		re_depth = 1;

		//give parameter to rgb
		cube_color_r = 0.1f;

		cube_color_g = 0.2f;

		cube_color_b = 0.3f;


		//set these parameter to gui
		cube_color = cgv::rgb(cube_color_r, cube_color_g, cube_color_b);

		vertex_buffer = false;

		inte_structure = false;

	}
	
	std::string get_type_name(void) const {

		return "cubes_fractal";
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh) {
	
		return

			rh.reflect_member("recursion_depth", re_depth) &&

			rh.reflect_member("color_red", cube_color_r) &&

			rh.reflect_member("color_blue", cube_color_b) &&

			rh.reflect_member("color_green", cube_color_g) &&

			rh.reflect_member("vertex_buffer", vertex_buffer) &&

			rh.reflect_member("interleaved_structure", inte_structure);

	}
	// Part of the cgv::base::base interface, should be implemented to respond to write
	// access to reflected data members of this class, e.g. from config file processing
	// or gui interaction.
	void on_set(void* member_ptr)
	{
		//tell the fractal engine to update the depth and color
		update_member(member_ptr);

		//draw
		post_redraw();
	}




	// Creates the custom geometry for the cubic
	void init_unit_cube_geometry(void)
	{
		// Prepare array
		vertices.resize(36);
		// create 8 
		cgv::vec3 V[8] = {
			{-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}, 
			{-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1}  
		};


	
	}

	void create_gui(void) {

		// Simple controls.Notifies us of GUI input via the on_set() method.
			// - section header
			add_decorator("Cubes fractal", "heading", "level=1");
		// - the depth
		add_member_control(this, "Recursion depth", re_depth, "value_slider","min=0;max=5;ticks=true");
		// - the Color
		add_member_control(this, "Cube color", cube_color);
	
	}

	void draw(cgv::render::context& ctx) {
	//using gui_color and its depth, and using cubes_fractal.cxx to compute, we just need to send two parameter, depth and color
		
		
		cgv::render::shader_program& default_shader =
			ctx.ref_default_shader_program(false /* false for we only need one color*/);

		// Enable shader program we want to use for drawing
		default_shader.enable(ctx);

		// Set the "color" vertex attribute for all geometry drawn hereafter, except if
			// it explicitely specifies its own color data by means of an attribute array.
			// Note that this only works for shaders that define a vec4 attribute named
			// "color" in their layout specification.
			// We want white to retain the original color information in the texture.
		ctx.set_color(cube_color);



		fractal_engine.draw_recursive(ctx, cube_color, re_depth, 0);

		// Disable shader program 
		default_shader.disable(ctx);
	}
};

cgv::base::object_registration<cubes_fractal_render> cubes_fractal_render_registration(
	"cubes_fractal_render"
);


// Task 0.2c: Implement an option (configurable via GUI and config file) to use a vertex
//            array object for rendering the cubes. The vertex array functionality 
//            should support (again, configurable via GUI and config file) both
//            interleaved (as in cgv_demo.cpp) and non-interleaved attributes.

// < your code here >

// [END] Tasks 0.2a, 0.2b and 0.2c
// ************************************************************************************/


// ************************************************************************************/
// Task 0.2a: register an instance of your drawable.

// < your code here >
