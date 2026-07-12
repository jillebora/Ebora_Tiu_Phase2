#include "cable.h"


/*
	Generates contact to make sure particle only moves until maxlength
*/
ParticleContact* Cable::GetContact()
{

	float currLen = CurrentLength();

	// if cable is loose/slack, no constraint needed
	if (currLen < maxLength)
		return nullptr;

	// undefined so skip
	if (currLen == 0.f)
		return nullptr;

	// cable is fully extended so build contact
	ParticleContact* ret = new ParticleContact();

	ret->particles[0] = particles[0];
	ret->particles[1] = nullptr;

	glm::vec3 dir = anchorPoint - particles[0]->Position;
	dir = glm::normalize(dir);

	ret->contactNormal = dir;

	// how far past max length
	// resolver uses this to pull back to maxLength
	ret->depth = currLen - maxLength;

	// bounciness at full length
	ret->restitiution = restitution;

	return ret;
}

float Cable::CurrentLength()
{
	// vector from anchor to particle
	glm::vec3 relative = particles[0]->Position - anchorPoint;

	// current cable length
	return glm::length(relative);
}
