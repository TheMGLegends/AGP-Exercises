#pragma once

#include <list>

#include "Particle.h"

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void Clean();

	float RandomZeroToOne();
	float RandomNegOneToPosOne();

	inline float GetPreviousTime() const { return m_timePrevious; }
	inline void SetPreviousTime(float time) { m_timePrevious = time; }

	inline float GetUntilParticle() const { return m_untilParticle; }
	inline void SetUntilParticle(float time) { m_untilParticle = time; }

	std::list<Particle*> m_free;
	std::list<Particle*> m_active;

	float m_timePrevious;
	float m_untilParticle;
};

