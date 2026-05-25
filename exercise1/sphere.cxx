#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_primitive.h"


template <typename T>
struct sphere : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;
	//bouns part 1.3a


	vec_type center;
	double radius;


	sphere() : center(0,0,0),radius(1.0){ implicit_base<T>::gui_color = 0xFF8888; }
	
	
	std::string get_type_name() const { return "sphere"; }
	//we need to reflect the memeory

	bool self_reflect(cgv::reflect::reflection_handler& rh) {

		return 
			rh.reflect_member("cx", center(0))&&

			rh.reflect_member("cy", center(1)) &&
			
			rh.reflect_member("cz", center(2)) &&

			rh.reflect_member("r", radius);

		implicit_primitive<T>::self_reflect(rh);

	}

	//create gui
	void create_gui() {
		implicit_primitive<T>::create_gui();
		provider::add_member_control(this, "center_x", center(0), "value_slider", "min=-5;max=5;ticks=true");

		provider::add_member_control(this, "center_y", center(1), "value_slider", "min=-5;max=5;ticks=true");

		provider::add_member_control(this, "center_z", center(2), "value_slider", "min=-5;max=5;ticks=true");

		provider::add_member_control(this, "radius", radius, "value_slider", "min=0.1;max=5;log=true;ticks=true");
	}




	/// Evaluate the sphere quadric at p
	T evaluate(const pnt_type& p) const
	{
		//$ || P - Center || -Radius = 0$。
		return (p - center).length() - radius;
	}

	/// Evaluate the gradient of the sphere quadric at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type d = p - center;

		T len = d.length();
		if (len < 1e-7) {
		
			return vec_type(0, 0, 0);
		
		}

		//mathematics formular will look like $\frac{\partial}{\partial x}\sqrt{x^2+y^2+z^2} = \frac{x}{\sqrt{x^2+y^2+z^2}} = \frac{x}{len}$


		vec_type grad_f_p = d / len;

		return grad_f_p;
	}

	
};

scene_factory_registration<sphere<double> > sfr_sphere("sphere;S");
