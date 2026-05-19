#include <cgv/math/fvec.h>
#include "implicit_primitive.h"


template <typename T>
struct box : public implicit_primitive<T>
{
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	box() {}
	std::string get_type_name() const { return "box"; }
	void on_set(void* member_ptr) { implicit_base<T>::update_scene(); }

	/*********************************************************************************/
	/* Task 1.1a: If you need any auxiliary functions for this task, put them here.  */

	// < your code >

	



	/* [END] Task 1.1a
	/*********************************************************************************/

	/// Evaluate the implicit box function at p
	T evaluate(const pnt_type& p) const
	{
		double f_p = std::numeric_limits<double>::infinity();

		// Task 1.1a: Implement a function of p that evaluates to 0 on the unit cube.
		//            You may use any suitable distance metric.

	 //to evaluate the implicit box function at p
	 //we use $$f(x, y, z) = \max(|x|, |y|, |z|) - 1$$
	//first I need to extract the x, y, z coordinates from the point p

		double abs_x = std::abs(p[0]);

		double abs_y = std::abs(p[1]);

		double abs_z = std::abs(p[2]);

		//second I need to find the maximum of the absolute values of x, y, z

		double max_abs = std::max(abs_x, std::max(abs_y, abs_z));

		//finally I need to subtract 1 from the maximum absolute value to get the implicit function value
		f_p = max_abs - 1.0;

		return f_p;
	}

	/// Evaluate the gradient of the implicit box function at p
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad_f_p(0, 0, 0);

		// Task 1.1a: Return the gradient of the function at p.
		
		//first i need to extract the x, y, z coordinates from the point p

	

		double abs_x = std::abs(p[0]);

		double abs_y = std::abs(p[1]);

		double abs_z = std::abs(p[2]);

		//second I need to know which the coordinate has the maximum absolute value

		if (abs_x >= abs_y && abs_x >= abs_z) {
			grad_f_p[0] = (p[0] >= 0) ? 1.0 : -1.0; // x has the maximum absolute value
		}
		else if (abs_y >= abs_x && abs_y >= abs_z) {
			grad_f_p[1] = (p[1] >= 0) ? 1.0 : -1.0; //  y has the maximum absolute value
		}
		else {
			grad_f_p[2] = (p[2] >= 0) ? 1.0 : -1.0; // z has the maximum absolute value
		}

		return grad_f_p;
	}

	void create_gui()
	{
		implicit_primitive<T>::create_gui();
	}
};

scene_factory_registration<box<double> > sfr_box("box;B");
