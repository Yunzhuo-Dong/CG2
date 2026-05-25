#include <cmath>
#include <algorithm>
#include <cgv/math/fvec.h>
#include "implicit_primitive.h"

template <typename T>
struct box : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	vec_type center;
	vec_type size; 

	box() : center(0, 0, 0), size(1.0, 1.0, 1.0)
	{
	
		implicit_base<T>::gui_color = 0x88FF88;
	}

	std::string get_type_name() const { return "box"; }

	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		return
			rh.reflect_member("center_x", center(0)) &&
			rh.reflect_member("center_y", center(1)) &&
			rh.reflect_member("center_z", center(2)) &&
			rh.reflect_member("size_x", size(0)) &&
			rh.reflect_member("size_y", size(1)) &&
			rh.reflect_member("size_z", size(2)) && 
			implicit_primitive<T>::self_reflect(rh);
	}

	void create_gui() {
		provider::add_member_control(this, "center_x", center(0), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "center_y", center(1), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "center_z", center(2), "value_slider", "min=-5;max=5;ticks=true");
		provider::add_member_control(this, "size_x", size(0), "value_slider", "min=0.1;max=5;log=true;ticks=true");
		provider::add_member_control(this, "size_y", size(1), "value_slider", "min=0.1;max=5;log=true;ticks=true");
		provider::add_member_control(this, "size_z", size(2), "value_slider", "min=0.1;max=5;log=true;ticks=true");
		implicit_primitive<T>::create_gui();
	}

	T evaluate(const pnt_type& p) const
	{
		vec_type d = p - center;

		vec_type abs_d(
			std::abs(d(0)) - size(0),
			std::abs(d(1)) - size(1),
			std::abs(d(2)) - size(2)
		);

		vec_type max_abs_d(
			std::max<T>(abs_d(0), 0.0),
			std::max<T>(abs_d(1), 0.0),
			std::max<T>(abs_d(2), 0.0)
		);

		double f_p = max_abs_d.length() + std::min<T>(std::max<T>(abs_d(0), std::max<T>(abs_d(1), abs_d(2))), 0.0);

		return f_p;
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type d = p - center;

		vec_type s(d(0) > 0 ? 1.0 : -1.0,
			d(1) > 0 ? 1.0 : -1.0,
			d(2) > 0 ? 1.0 : -1.0);

		vec_type abs_d(std::abs(d(0)) - size(0),
			std::abs(d(1)) - size(1),
			std::abs(d(2)) - size(2));

		if (abs_d(0) <= 0.0 && abs_d(1) <= 0.0 && abs_d(2) <= 0.0) {
			if (abs_d(0) > abs_d(1) && abs_d(0) > abs_d(2)) return vec_type(s(0), 0.0, 0.0);
			if (abs_d(1) > abs_d(0) && abs_d(1) > abs_d(2)) return vec_type(0.0, s(1), 0.0);
			return vec_type(0.0, 0.0, s(2));
		}

		vec_type max_abs_d(std::max<T>(abs_d(0), 0.0),
			std::max<T>(abs_d(1), 0.0),
			std::max<T>(abs_d(2), 0.0));

		T len = max_abs_d.length();

		if (len > T(1e-8)) {
			return vec_type((max_abs_d(0) / len) * s(0),
				(max_abs_d(1) / len) * s(1),
				(max_abs_d(2) / len) * s(2));
		}
		else {
			return vec_type(0.0, 0.0, 0.0);
		}
	}
};

scene_factory_registration<box<double> > sfr_box("box;B");
