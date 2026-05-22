#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_group.h"

// ======================================================================================
//  Task 1.1b: GENERAL HINTS
//
//  The common super class of all CSG nodes is implicit_group. This class defines a
//  method ::get_implicit_child(unsigned int), which retrieves a pointer - already
//  casted to implicit_base<T>* - to the indicated child node, on which you can then call
//  ::evaluate() and ::evaluate_gradient().
//  Use the method ::get_nr_children() of the super super class cgv::base::group to query
//  the number of children registered with your operator.
//
// ======================================================================================

template <typename T>
class union_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	union_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "union_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{

		// Task 1.1b: You can outsource logic here that evaluates the operator function
		//            and reports the index of the relevant child in selected_i

		//because we are looking fot the mininum value, so we create a infinity vaule

		T value= std::numeric_limits<double>::infinity();

		//then we need to loop through all the children of the union node and evaluate the implicit function at p for each child
		unsigned int nr_children = implicit_group<T>::get_nr_children();
		//now we traverse all the nodes of the children
		for (unsigned int i = 0; i < nr_children; ++i) {
			//get the child node
			//get the i pointer to the child node
			const	implicit_base<T>* child = implicit_group<T>::get_implicit_child(i);
			//compute the vaule of the implicit function at p for the child node
			double child_value = child->evaluate(p);
			//choose the minimum vaule
			if (child_value < value) {
				value = child_value;
				selected_i = i; // update selected_i to the index of the child that gives the minimum value
			}
		}




		return value;
	}

	T evaluate(const pnt_type& p) const
	{
		double f_p = std::numeric_limits<double>::infinity();

		// Task 1.1b: Evaluate the union operator at p.
		unsigned int union_index;

		f_p = eval_and_get_index(p, union_index);

		return f_p;
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad_f_p(0, 0, 0);

		// Task 1.1b: Return the gradient of the union operator at p
		unsigned int target_index;

		//compute the value of the union operator at p and get the index of the child that gives the minimum value

		eval_and_get_index(p, target_index);

		const implicit_base<T>* target_child = implicit_group<T>::get_implicit_child(target_index);

		

		//we need to return the child's gradient at p
		return target_child->evaluate_gradient(p);
	}
};

template <typename T>
class intersection_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	intersection_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "intersection_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{

		// Task 1.1b: You can outsource logic here that evaluates the operator function
		//            and reports the index of the relevant child in selected_i
		
		//we need to find the maximum value among the children of the intersection node, so we initialize the value to negative infinity
		T value = -std::numeric_limits<double>::infinity();

		//then we need to loop through all the children of the intersection node and evaluate the implicit function at p for each child	

		unsigned int nr_children = implicit_group<T>::get_nr_children();
		//now we traverse all the nodes of the children
		for (unsigned int i = 0; i < nr_children; ++i) {
			//get the child node
			//get the i pointer to the child node
			const	implicit_base<T>* child = implicit_group<T>::get_implicit_child(i);
			//compute the vaule of the implicit function at p for the child node
			double child_value = child->evaluate(p);
			//choose the maximum vaule
			if (child_value > value) {
				value = child_value;
				selected_i = i; // update selected_i to the index of the child that gives the maximum value
			}
		}
		return value;
	}

	T evaluate(const pnt_type& p) const
	{
		double f_p = std::numeric_limits<double>::infinity();

		// Task 1.1b: Evaluate the intersection operator at p.

		unsigned int intersect_index;

		f_p = eval_and_get_index(p, intersect_index);

		return f_p;
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad_f_p(0, 0, 0);

		// Task 1.1b: Return the gradient of the intersection operator at p

		unsigned int target_index;

		//compute the value of the intersection operator at p and get the index of the child that gives the maximum value
		eval_and_get_index(p, target_index);
		const implicit_base<T>* target_child = implicit_group<T>::get_implicit_child(target_index);

		//we need to return the child's gradient at p

		return target_child->evaluate_gradient(p);
	}
};

template <typename T>
class difference_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	difference_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "difference_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{

		// Task 1.1b: You can outsource logic here that evaluates the operator function
		//            and reports the index of the relevant child in selected_i

		//first vaule is possitive, the others are negative, so we initialize the value to negative infinity
		T value = -std::numeric_limits<double>::infinity();

		unsigned int nr_children = implicit_group<T>::get_nr_children();

		//we need to loop through all the children of the difference node and evaluate the implicit function at p for each child

		for (unsigned int i = 0;i < nr_children;i++) {
			//get the child node
			const	implicit_base<T>* child = implicit_group<T>::get_implicit_child(i);

			//compute the vaule of the implicit function at p for the child node
			double child_value = child->evaluate(p);

			//first is the positive, the others are negetive, because we want to extract the otheres
			if (i == 0) {
				value = child_value;
				selected_i = i; // update selected_i to the index of the child that gives the value
			}
			else {
				if (-child_value > value) {
					value = -child_value;
					selected_i = i; // update selected_i to the index of the child that gives the maximum value
				}
			
			}
		}
		return value;
	}

	T evaluate(const pnt_type& p) const
	{
		double f_p = std::numeric_limits<double>::infinity();

		// Task 1.1b: Evaluate the difference operator at p.

		unsigned int difference_index;
		f_p = eval_and_get_index(p, difference_index);

		return f_p;
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad_f_p(0, 0, 0);

		// Task 1.1b: Return the gradient of the difference operator at p

		unsigned int target_index;
		eval_and_get_index(p, target_index);
		
		const implicit_base<T>* target_child = implicit_group<T>::get_implicit_child(target_index);

		vec_type child_gradient = target_child->evaluate_gradient(p);

		if (target_index>0 ) {
			
			return -child_gradient;
		}
			
		return child_gradient;
	}
};

scene_factory_registration<union_node<double> > sfr_union("union;+");
scene_factory_registration<intersection_node<double> > sfr_intersect("intersection;*");
scene_factory_registration<difference_node<double> > sfr_difference("difference;-");
