#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_primitive.h"

template <typename T>
struct cylinder : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	vec_type center;
	double radius;
	vec_type direction;

	cylinder() : center(0, 0, 0), radius(1.0), direction(0, 0, 1)
	{
		implicit_base<T>::gui_color = 0xFF8888;
	}

	std::string get_type_name() const { return "cylinder"; }

	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		return
			rh.reflect_member("center_x", center(0)) &&
			rh.reflect_member("center_y", center(1)) &&
			rh.reflect_member("center_z", center(2)) &&
			rh.reflect_member("radius", radius) &&
			rh.reflect_member("direction_x", direction(0)) &&
			rh.reflect_member("direction_y", direction(1)) &&
			rh.reflect_member("direction_z", direction(2)) &&
			implicit_primitive<T>::self_reflect(rh);
	}
	
	
void create_gui() {
		provider::add_member_control(this, "center_x", center(0), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "center_y", center(1), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "center_z", center(2), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "radius", radius, "value_slider", "min=0.1;max=5;log=true;ticks=true");
		provider::add_member_control(this, "direction_x", direction(0), "value_slider", "min=-1;max=1;ticks=true");
		provider::add_member_control(this, "direction_y", direction(1), "value_slider", "min=-1;max=1;ticks=true");
		provider::add_member_control(this, "direction_z", direction(2), "value_slider", "min=-1;max=1;ticks=true");
		implicit_primitive<T>::create_gui();
	}

	/// Evaluate the implicit cylinder function at p
	T evaluate(const pnt_type& p) const
	{
		vec_type dir = direction;
		T d_len = dir.length();

		
		dir = d_len > 1e-8 ? dir / d_len : vec_type(0, 0, 1);

		vec_type pa = p - center;

		vec_type v_perp = pa - dot(pa, dir) * dir;
		
		return v_perp.length() - radius;
	}

	/// Evaluate the gradient of the implicit cylinder function at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type dir = direction;
		T d_len = dir.length();

	
		dir = d_len > 1e-8 ? dir / d_len : vec_type(0, 0, 1);

		vec_type pa = p - center;
		vec_type v_perp = pa - dot(pa, dir) * dir;

		T len = v_perp.length();
		if (len < 1e-8) {
			return vec_type(0, 0, 0);
		}

		return v_perp / len;
	}
};



scene_factory_registration<cylinder<double> > sfr_cylinder("cylinder;C");
