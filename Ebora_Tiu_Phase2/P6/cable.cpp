#include "cable.h"

ParticleContact* Cable::GetContact()
{

	float currLen = CurrentLength();

	if (currLen < maxLength)
		return nullptr;

	if (currLen == 0.f)
		return nullptr;

	ParticleContact* ret = new ParticleContact();

	ret->particles[0] = particles[0];
	ret->particles[1] = nullptr;

	glm::vec3 dir = anchorPoint - particles[0]->Position;
	dir = glm::normalize(dir);

	ret->contactNormal = dir;

	ret->depth = currLen - maxLength;

	ret->restitiution = restitution;

	return ret;
}

float Cable::CurrentLength()
{
	glm::vec3 relative = particles[0]->Position - anchorPoint;

	return glm::length(relative);
}
