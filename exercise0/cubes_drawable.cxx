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
	cgv::rgba cube_color; 
	
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


public:
	
	cubes_fractal_render() {

		//give parameter to depteh
		re_depth = 1;

		//give parameter to rgb
		cube_color_r = 0.1f;

		cube_color_g = 0.2f;

		cube_color_r = 0.3f;

	}
	
	std::string get_type_name(void) const {

		return "cubes_fractal";
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh) {
	
		return rh.reflect_member();
	}
	void create_gui(void) {
	
	}

	void draw(cgv::render::context& ctx) {
	
	}
};

cgv::base::object_registration<cubes_fractal_render> cubes_fractal_render_registration(
	"cubes_fractal_render"
);

//
// Task 0.2b: Utilize the cubes_fractal class to render a fractal of hierarchically
//            transformed cubes. Expose its recursion depth and color properties to GUI
//            manipulation and reflection. Set reasonable values via the config
//            file.
//
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
