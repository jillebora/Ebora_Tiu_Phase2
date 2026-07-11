#pragma once

#include "particleLink.h"
#include "particle.h"

#include <glm/glm.hpp>

class Cable : public ParticleLink
{
	public:
		// The fixed point in space the particle hangs from
		glm::vec3 anchorPoint = glm::vec3(0.f);

		// max dist from the anchor
		float maxLength = 1.f;

		// bounciness
		float restitution = 0.9f;

		ParticleContact* GetContact() override;


	protected:
		// distance from the anchor point
		float CurrentLength();
};

