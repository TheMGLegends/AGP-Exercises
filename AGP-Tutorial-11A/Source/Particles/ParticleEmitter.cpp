#include "ParticleEmitter.h"

#include <Windows.h>
#include <random>

ParticleEmitter::ParticleEmitter()
{
	// INFO: Initialize the free list with 100 particles
	for (int i = 0; i < 200; i++)
	{
		m_free.push_back(new Particle());
	}

	// INFO: Set the time stamp of timePrevious
	m_timePrevious = float(timeGetTime());

	m_untilParticle = 5.0f;
}

ParticleEmitter::~ParticleEmitter()
{
	Clean();
}

void ParticleEmitter::Clean()
{
	// INFO: Clean up the free list
	for (Particle* p : m_free)
	{
		delete p;
	}
	m_free.clear();

	// INFO: Clean up the active list
	for (Particle* p : m_active)
	{
		delete p;
	}
	m_active.clear();
}

float ParticleEmitter::RandomZeroToOne()
{
	float range = 1 - 0;
	return 0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / range));
}

float ParticleEmitter::RandomNegOneToPosOne()
{
	float range = 1 - -1;
	return -1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / range));
}

float ParticleEmitter::RandomRange(float min, float max)
{
	//INFO: Generate a random range between min and max(inclusive)
	float range = max - min;
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / range));
}
