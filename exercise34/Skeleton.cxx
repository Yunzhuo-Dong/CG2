// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
#include "Skeleton.h"

#include <fstream>
#include <map>
#include <string>
#include <sstream>

#include <algorithm>
#include <functional>
#include <cmath>

#include "math_helper.h"

Skeleton::Skeleton()
{
	origin.identity();
}

Skeleton::~Skeleton()
{
	if (root)
		delete root;
}

Bone* Skeleton::get_root() const { return root; }
const Mat4& Skeleton::get_origin() const { return origin; }
Vec3 Skeleton::get_origin_vec() const { return Vec3(origin(0, 3), origin(1, 3), origin(2,3)); }
void Skeleton::set_origin(const Vec3& v) { origin = translate(v); }
void Skeleton::set_origin(const Mat4& m) { origin = m; }

enum ParseState
{
	Ignore,
	Root,
	BoneData,
	Hierarchy,
};

bool Skeleton::fromASFFile(const std::string& filename)
{
	origin.identity();

	std::ifstream fin;
	reset_bounding_box();
	ParseState state = Ignore;

	Bone* current_node = nullptr;

	bones.clear();

	int n_dofs;

	try
	{
#ifdef _WIN32
		std::wstring wfilename = cgv::utils::str2wstr(filename);
		fin.open(wfilename);
#else
		fin.open(filename);
#endif
		if (!fin.good())
			return false;
		while (!fin.eof())
		{
			char buf[CHARS_PER_LINE];
			fin.getline(buf, CHARS_PER_LINE);
			std::string str(buf);
			str = trim(str); //remove whitespaces
			if (str.find('#') == 0)
				continue; //don't handle comments

			if (str.find(':') == 0)
			{
				if (str.find(":version") == 0)
					version = str.substr(9);
				else if (str.find(":name") == 0)
					name = str.substr(6);
				else if (str.find(":root") == 0)
				{
					state = Root;
					current_node = new Bone();
					current_node->set_name("root");
					bones["root"] = current_node;
				}
				else if (str.find(":bonedata") == 0)
					state = BoneData;
				else if (str.find(":hierarchy") == 0)
					state = Hierarchy;
				else state = Ignore;
				continue;
			}
			switch (state)
			{
			case Ignore:
				//ignore this line
				break;
			case Root:
				if (str.find("order") == 0)
				{
					std::string dofs = str.substr(6);
					std::string dofstr;
					std::stringstream ss(dofs);
					n_dofs = 0;
					while (!ss.eof())
					{
						ss >> dofstr;
						AtomicTransform* dof;
						if (dofstr.find("RX") == 0)
							dof = new AtomicXRotationTransform();
						else if (dofstr.find("RY") == 0)
							dof = new AtomicYRotationTransform();
						else if (dofstr.find("RZ") == 0)
							dof = new AtomicZRotationTransform();
						else if (dofstr.find("TX") == 0)
							dof = new AtomicXTranslationTransform();
						else if (dofstr.find("TY") == 0)
							dof = new AtomicYTranslationTransform();
						else if (dofstr.find("TZ") == 0)
							dof = new AtomicZTranslationTransform();
						dof->set_value(0);
						current_node->add_dof(dof);
						++n_dofs;
					}
				}				
				break;
			case BoneData:
				if (str.find("begin") == 0)
					current_node = new Bone();
				else if (str.find("name") == 0)
				{
					std::string name = str.substr(5);
					current_node->set_name(name);
					bones[name] = current_node;
				}
				else if (str.find("direction") == 0)
				{
					std::string direction = str.substr(10);
					float dx, dy, dz;
					std::stringstream ss(direction);
					ss >> dx >> dy >> dz;
					current_node->set_direction_in_world_space(Vec3(dx, dy, dz));
				}
				else if (str.find("length") == 0)
				{
					std::string length = str.substr(7);
					float l;
					std::stringstream ss(length);
					ss >> l;
					current_node->set_length(l);
				}
				else if (str.find("axis") == 0)
				{
					std::string axis = str.substr(5);
					float a[3];
					std::string order;
					std::stringstream ss(axis);
					ss >> a[0] >> a[1] >> a[2] >> order;
					for (int i = 0; i < 3; ++i)
					{
						AtomicRotationTransform* t;
						if (order.at(i) == 'X')
							t = new AtomicXRotationTransform();
						else if (order.at(i) == 'Y')
							t = new AtomicYRotationTransform();
						else if (order.at(i) == 'Z')
							t = new AtomicZRotationTransform();
						t->set_value(a[i]);
						current_node->add_axis_rotation(t);
					}
				}
				else if (str.find("dof") == 0)
				{
					std::string dofs = str.substr(4);
					std::string dofstr;
					std::stringstream ss(dofs);
					n_dofs = 0;
					while (!ss.eof())
					{
						ss >> dofstr;
						AtomicTransform* dof;
						if (dofstr.find("rx") == 0)
							dof = new AtomicXRotationTransform();
						else if (dofstr.find("ry") == 0)
							dof = new AtomicYRotationTransform();
						else if (dofstr.find("rz") == 0)
							dof = new AtomicZRotationTransform();
						current_node->add_dof(dof);
						++n_dofs;
					}
				}
				else if (str.find("limits") == 0)
				{
					std::string limits = str.substr(7);
					for (int i = 0; i < n_dofs; ++i)
					{
						if (i > 0)
						{
							fin.getline(buf, CHARS_PER_LINE);
							limits = std::string(buf);
							limits = trim(limits);
						}

						limits = limits.substr(limits.find('(') + 1);
						std::stringstream ss(limits);
						float lower, upper;
						ss >> lower >> upper;
						current_node->get_dof(n_dofs - i - 1)->set_limits(lower, upper);
					}

				}
				break;
			case Hierarchy:
				if (str.find("begin") == std::string::npos && str.find("end") == std::string::npos)
				{
					std::stringstream ss(str);
					std::string parentstr;
					ss >> parentstr;
					Bone* parent = bones[parentstr];
					std::string childstr;
					while (!ss.eof())
					{
						ss >> childstr;
						Bone* child = bones[childstr];
						parent->add_child(child);
					}
				}
			}
		}
		fin.close();
		root = bones["root"];

		//root->revert_dofs();

		postprocess(root, Vec3(0, 0, 0));
		return true;
	}
	catch (...)
	{
		fin.close();
		return false;
	}
}

void Skeleton::postprocess(Bone* node, const Vec3& global_position)
{
	//For display adaptation
	auto bone_offset_in_global_system = node->get_direction_in_world_space() * node->get_length();
	auto bone_tip_in_global_system = global_position + bone_offset_in_global_system;
	add_point(bone_tip_in_global_system);

	node->calculate_matrices();
	int n = node->childCount();
	for (int i = 0; i < n; ++i)
	{
		postprocess(node->child_at(i), bone_tip_in_global_system);
	}
}

void Skeleton::write_pinocchio_file(const std::string& filename)
{
	std::ofstream o;
#ifdef _WIN32
	std::wstring wfilename = cgv::utils::str2wstr(filename);
	o.open(wfilename, std::ios::out);
#else
	o.open(filename, std::ios::out);
#endif	

	if (o)
	{
		/*Task 4.1: Write Pinocchio file into o */

		//if no bones, write nothing

		if (root == nullptr)
		{
			o.close();
			return;
		}


		// 1. Get the bounding box calculated by postprocess()


		Vec3 min_corner = getMin();
		Vec3 max_corner = getMax();

		Vec3 box_size = max_corner - min_corner;


		float largest_size =
			std::max(
				box_size.x(),
				std::max(box_size.y(), box_size.z())
			);



			Vec3 center =
			(min_corner + max_corner) * 0.5f;

		// The next available Pinocchio node ID.
		int next_id = 0;


		// 2. Define a recursive depth-first traversal
		std::function<void(Bone*, const Vec3&, int)> write_bone;

		write_bone =
			[&](Bone* bone, //lamda function, can be visited as local vairables 
				const Vec3& bone_root_position,
				int parent_id)
			{
				// Find the tip of this bone in world coordinates.
				Vec3 bone_tip = bone_root_position + bone->get_direction_in_world_space() * bone->get_length();

				int current_id = next_id;
				++next_id;

				// Scale and translate the point into the unit cube.
				Vec3 normalized_position =
					(bone_tip - center) / largest_size
					+ Vec3(0.5f, 0.5f, 0.5f);

				// Pinocchio format:
			   // ID x y z parent_ID
				o << current_id << " "
					<< normalized_position.x() << " "
					<< normalized_position.y() << " "
					<< normalized_position.z() << " "
					<< parent_id << "\n";
				// Visit children.
				for (int i = 0; i < bone->childCount(); ++i)
				{
					write_bone(
						bone->child_at(i),
						bone_tip,
						current_id
					);
				}
			};
		// -------------------------------------------------
	   // 3. Start DFS at the root
	   // -------------------------------------------------

		write_bone(
			root,
			Vec3(0.0f, 0.0f, 0.0f),
			-1
		);
	}

	o.close();

}



void Skeleton::read_pinocchio_file(std::string filename)
{
	std::ifstream o;
#ifdef _WIN32
	std::wstring wfilename = cgv::utils::str2wstr(filename);
	o.open(wfilename, std::ios::in);
#else
	o.open(filename, std::ios::in);
#endif

	bool success = false;

	if (o)
	{
		/*Task 4.3: Read Pinocchio file */

		//1: Rebuild the fitted rest pose from Pinocchio joint positions.
		//2: Recover each bone from its fitted root and tip., and align the coordinate
		//3: use parent_ tip=child_ root to iterate the function
		//4:Recompute the skeleton matrices after adaptation.

		if (root == nullptr)
		{
			o.close();
			return;
		}

		reset_bounding_box();

		const float epsilon = 0.000001f;

		std::function<bool(Bone*, const Vec3&)> read_and_adjust;

		read_and_adjust = [&](Bone* bone, const Vec3& parent_position)
			{
				// One Pinocchio line:
				// node_id x y z parent_id
				int node_id;
				int file_parent_id;

				float x;
				float y;
				float z;

				if (!(o >> node_id >> x >> y >> z >> file_parent_id))
				{
					return false;
				}

				Vec3 current_position(x, y, z);

				//add joint position or tip 
				add_point(current_position);

				if (bone == root)
				{
					//The root node has no parent.
					set_origin(current_position);
				}
				else
				{
					//we need to find the rotation of the old
					Vec3 old_direction = bone->get_direction_in_world_space();

					float old_direction_length = old_direction.length();

					if (old_direction_length > epsilon)
					{
						old_direction = old_direction / old_direction_length;
					}

					//parent_position   = current bone root
					// current_position  = current bone tip

					Vec3 new_offset = current_position - parent_position;

					float new_length = new_offset.length();

					if (new_length <= epsilon)
					{
						return false;
					}

					Vec3 new_direction = new_offset / new_length;

					//Adjust the bone coordinate system,We need a rotation R for: R * old_direction = new_direction

					if (old_direction_length > epsilon)
					{
						Vec3 rotation_axis = cgv::math::cross(old_direction, new_direction);

						float axis_length = rotation_axis.length();

						float cosine = cgv::math::dot(old_direction, new_direction);

						// Floating-point calculations can produce

						cosine = std::max(-1.0f, std::min(1.0f, cosine));

						if (axis_length > epsilon)
						{
							//the two directions are not parallel.
							rotation_axis = rotation_axis / axis_length;

							float angle_degrees = std::acos(cosine) * 180.0f / PI;

							AtomicRotationTransform* correction = new AtomicRotationTransform(rotation_axis);

							correction->set_value(angle_degrees);

							bone->add_axis_rotation(correction);
						}
						else if (cosine < 0.0f)
						{
							//old_direction and new_direction point in exactly opposite directions.

							//Their cross product is zero

							Vec3 helper_axis;

							if (std::abs(old_direction.x()) < 0.9f)
							{
								helper_axis = Vec3(1.0f, 0.0f, 0.0f);
							}
							else
							{
								helper_axis = Vec3(0.0f, 1.0f, 0.0f);
							}

							rotation_axis = cgv::math::cross(old_direction, helper_axis);

							rotation_axis.normalize();

							AtomicRotationTransform* correction = new AtomicRotationTransform(rotation_axis);

							correction->set_value(180.0f);

							bone->add_axis_rotation(correction);
						}
					}

					// Store the new fitted rest-pose geometry

					bone->set_length(new_length);
					bone->set_direction_in_world_space(new_direction);
				}

				for (int i = 0; i < bone->childCount(); ++i)
				{
					bool child_success = read_and_adjust(bone->child_at(i), current_position);

					if (!child_success)
					{
						return false;
					}
				}

				return true;
			};

		success = read_and_adjust(root, Vec3(0.0f, 0.0f, 0.0f));
	}

	o.close();

	if (success)
	{
		postprocess(root, get_origin_vec());
	}
}


void Skeleton::get_skinning_matrices(std::vector<Mat4>& matrices)
{
	/*Task 4.5: Calculate skinning matrices */
}



Bone* Skeleton::find_bone(const std::string& name) const
{ 
	auto it = bones.find(name); 
	if (it == bones.end())
		return nullptr;
	else
		return it->second;
}
