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


	/// Evaluate the sphere quadric at p
	T evaluate(const pnt_type& p) const
	{
		return ((p[0] - center[0]) * (p[0] - center[0]) + (p[1] - center[1]) * (p[1] - center[1]) + (p[2] - center[2]) * (p[2] - center[2])) - radius * radius;
	}

	/// Evaluate the gradient of the sphere quadric at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad_f_p(0, 0, 0);

		// Task 1.1a: Return the gradient of the function at p.
		double x = p[0];

		double y = p[1];

		double z = p[2];

		grad_f_p[0] = 2.0 * x;

		grad_f_p[1] = 2.0 * y;

		grad_f_p[2] = 2.0 * z;

		return grad_f_p;
	}

	void create_gui()
	{
		implicit_primitive<T>::create_gui();
	}
};

scene_factory_registration<sphere<double> > sfr_sphere("sphere;S");
