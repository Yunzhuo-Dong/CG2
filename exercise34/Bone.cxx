// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
#include "Bone.h"

#include <cgv/math/transformations.h>
#include <cgv/math/inv.h>

#include "math_helper.h"

Bone::Bone()
	: parent(nullptr), length(0.0f), direction_in_world_space(0.0, 0.0, 0.0), translationTransforms(0)
{}

Bone::~Bone()
{
	for (auto bone : children)
		delete bone;
	children.clear();

	dofs.clear();

	for (auto dof : orientation)
		delete dof;
	orientation.clear();
}

void Bone::calculate_matrices()
{
    orientationSystemTransformLocalToGlobal.identity();

    std::for_each(orientation.begin(), orientation.end(), [&](AtomicTransform* t) {
        orientationSystemTransformLocalToGlobal =
            orientationSystemTransformLocalToGlobal * t->calculate_matrix();
        });

    // This is actually orientationSystemTransformGlobalToLocal,
    // because orientationModelTransformLocalToGlobal is a reference to it.
    orientationModelTransformLocalToGlobal =
        cgv::math::inverse(orientationSystemTransformLocalToGlobal);

    // ------------------------------------------------------------
    // Task 3.1: orientation from previous bone to current bone
    // ------------------------------------------------------------
    //
    
    // current local -> global -> parent local
    //
    // current local -> global:
    //     orientationSystemTransformLocalToGlobal
    //
    // global -> parent local:
    //     parent->orientationSystemTransformGlobalToLocal
    //
    if (parent != nullptr)
    {
        orientationModelTransformPrevJointToCur =
            parent->orientationSystemTransformGlobalToLocal
            *
            orientationSystemTransformLocalToGlobal;
    }
    else
    {
        // Root has no parent.
        // Treat the previous coordinate system as the global coordinate system.
        //
        // root local -> global
        orientationModelTransformPrevJointToCur =
            orientationSystemTransformLocalToGlobal;
    }

    // ------------------------------------------------------------
    // Task 3.1: translation from current joint to next joint
    // ------------------------------------------------------------
    //
    // ASF direction is stored in world/global coordinates.
    // But this translation must be expressed in the current bone's local coordinates.
    //
    // direction: global -> current local
    Vec4 directionWorld(
        direction_in_world_space.x(),
        direction_in_world_space.y(),
        direction_in_world_space.z(),
        0.0f
    );

    Vec4 directionLocal =
        orientationSystemTransformGlobalToLocal
        *
        directionWorld;

    translationModelTransformCurJointToNext =
        translate(
            directionLocal.x() * length,
            directionLocal.y() * length,
            directionLocal.z() * length
        );

    // Task 4.5 later.
}

Mat4 Bone::calculate_transform_prev_to_current_with_dofs()
{
	////
	// Task 3.1: Implement matrix calculation

	Mat4 t;

	t.identity();

	if (parent != nullptr)

	{
		// First move from the parent joint to the end of the parent bone.
		// The end of the parent bone is the start joint of the current bone.
		t = t * parent->translationModelTransformCurJointToNext;
	}

	// Rotate from the parent's orientation into the current bone's orientation.
	t = t * orientationModelTransformPrevJointToCur;

	for (auto& dof : dofs)
	{
		t = t * dof->calculate_matrix();
	}
	return t;
}

Mat4 Bone::calculate_transform_prev_to_current_without_dofs()
{
	////
	// Task 3.1: Implement matrix calculation

	Mat4 t;

	t.identity();

	if (parent != nullptr)

	{
		t = t * parent->translationModelTransformCurJointToNext;
	}

	t = t * orientationModelTransformPrevJointToCur;


	return t;
}

void Bone::add_dof(AtomicTransform* dof)
{
	dof->set_index_in_amc((int)dofs.size());	
	if (dynamic_cast<AtomicTranslationTransform*>(dof))
	{
		dofs.push_front(std::shared_ptr<AtomicTransform>(dof));
		++translationTransforms;
	}
	else
		dofs.insert(dofs.begin() + translationTransforms, std::shared_ptr<AtomicTransform>(dof));
}

void Bone::set_name(const std::string& name) { this->name = name; }
const std::string& Bone::get_name() const { return name; }

void Bone::set_direction_in_world_space(const Vec3& direction) { this->direction_in_world_space = direction; }
const Vec3& Bone::get_direction_in_world_space() const { return direction_in_world_space; }

void Bone::set_length(float l) { this->length = l; }
float Bone::get_length() const { return length; }

void Bone::add_axis_rotation(AtomicRotationTransform* transform) { orientation.push_front(transform); }
void Bone::add_child(Bone* child)
{
	child->set_parent(this);
	children.push_back(child);
}
Bone* Bone::child_at(int i) const { return children[i]; }
int Bone::childCount() const { return (int)children.size(); }

void Bone::set_parent(Bone* parent)
{
	this->parent = parent;
}
Bone* Bone::get_parent() const { return parent; }

int Bone::dof_count() const { return (int)dofs.size(); }
std::shared_ptr<AtomicTransform> Bone::get_dof(int dofIndex) const { return dofs[dofIndex]; }

const Mat4& Bone::get_binding_pose_matrix() const
{
	return systemTransformGlobalToLocal;
}

const Mat4& Bone::get_translation_transform_current_joint_to_next() const { return translationModelTransformCurJointToNext; }
const Mat4& Bone::get_orientation_transform_prev_joint_to_current() const { return orientationModelTransformPrevJointToCur; }

Vec4 Bone::get_bone_local_root_position() const { return Vec4(0, 0, 0, 1); }
Vec4 Bone::get_bone_local_tip_position() const { return translationModelTransformCurJointToNext * Vec4(0, 0, 0, 1); }
