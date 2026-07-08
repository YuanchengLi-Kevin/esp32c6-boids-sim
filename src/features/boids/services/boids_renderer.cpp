/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/boids/services/boids_renderer.hpp"

#include "Material.hpp"
#include "Object.hpp"
#include "core/rendering/rendering.hpp"

#include <algorithm>
#include <array>

namespace boids::renderer
{
	namespace
	{

		std::array<Renderer::Object, kMaxRenderedBoids> boid_objects;
		Renderer::Material boid_material(constants::colors::kBoid);
		bool initialized = false;

		void buildBoidMesh(Renderer::Object &object)
		{
			using Renderer::CullingMode;

			object.vertices.reserve(4);
			object.triangles.reserve(4);

			const Vector2 uv = {0, 0};
			const Vector3 nose = {
				constants::boid_mesh::kNoseX,
				constants::boid_mesh::kNoseY,
				constants::boid_mesh::kNoseZ,
			};
			const Vector3 left = {
				constants::boid_mesh::kLeftX,
				constants::boid_mesh::kLeftY,
				constants::boid_mesh::kLeftZ,
			};
			const Vector3 right = {
				constants::boid_mesh::kRightX,
				constants::boid_mesh::kRightY,
				constants::boid_mesh::kRightZ,
			};
			const Vector3 top = {
				constants::boid_mesh::kTopX,
				constants::boid_mesh::kTopY,
				constants::boid_mesh::kTopZ,
			};

			object.addVertex({nose, uv, {0, 0, FIXED_POINT_SCALE}});
			object.addVertex({left, uv, {0, 0, FIXED_POINT_SCALE}});
			object.addVertex({right, uv, {0, 0, FIXED_POINT_SCALE}});
			object.addVertex({top, uv, {0, 0, FIXED_POINT_SCALE}});

			object.addTriangle(0, 1, 3, &boid_material);
			object.addTriangle(0, 3, 2, &boid_material);
			object.addTriangle(0, 2, 1, &boid_material);
			object.addTriangle(1, 2, 3, &boid_material);
			object.computeFlatNormals();
			object.calculateBoundingBox();
			object.cullingMode = CullingMode::NO_CULLING;
		}

	} // namespace

	void init(Renderer::Scene &scene)
	{
		if (initialized)
		{
			return;
		}

		boid_material.emissive = true;
		boid_material.shadingMode = Renderer::ShadingMode::UNLIT;

		for (Renderer::Object &object : boid_objects)
		{
			buildBoidMesh(object);
			scene.addObject(&object);
		}

		initialized = true;
	}

	void update(const RenderSnapshot &snapshot)
	{
		const std::size_t count = std::min(snapshot.count, kMaxRenderedBoids);
		for (std::size_t i = 0; i < kMaxRenderedBoids; ++i)
		{
			boid_objects[i].enabled = i < count;
			if (i >= count)
			{
				continue;
			}

			const RenderBoid &boid = snapshot.boids[i];

			const Vector3 position = core::rendering::toWorld(boid.position.x, boid.position.y, boid.position.z);

			const Vec3 targetPos = boid.position + boid.velocity;
			const Vector3 target = core::rendering::toWorld(targetPos.x, targetPos.y, targetPos.z);

			boid_objects[i].setPosition(position);
			boid_objects[i].lookAt(target);
		}
	}

} // namespace boids::renderer
