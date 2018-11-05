#include "BvShiaOrbit.h"
#include "World.h"
#include "Shia.h"
#include "Poo.h"
#include <algorithm>
#include <iterator>


Shia::Orbit::Orbit( std::mt19937& rng )
	:
	rng( rng )
{}

Shia::Behavior* Shia::Orbit::Update( Shia& shia,World& world,float dt )
{
	s_time += dt;
	if( !activated || s_time >= duration )
	{
		if( HasSuccessors() )
		{
			return PassTorch();
		}
	}

	shia.SetSpeed( 0.0f );

	return nullptr;
}

void Shia::Orbit::Activate( Shia& shia,const World& world )
{
	auto& poos = world.GetPoosConst();
	if( poos.size() > 0 )
	{
		// generate list of refs to candidate poos
		std::vector<std::reference_wrapper<const Poo>> candidates;
		std::copy_if( poos.begin(),poos.end(),std::back_inserter( candidates ),
			[]( const Poo& poo ) { return poo.GetVel().GetLengthSq() == 0.0f; }
		);
		if( candidates.size() > 0 )
		{
			// select a random # of poos
			std::normal_distribution<float> npoodist( mean_n_poos,std_dev );
			const int n = std::min( std::max( 1,(int)(npoodist( rng ) + 0.5f) ),(int)candidates.size() );
			std::shuffle( candidates.begin(),candidates.end(),rng );
			const float angle = 2.0f * PI / (float)n;
			for( int c = 0; c < n; c++ )
			{
				// hack:chili (should World be const here?)
				const_cast<Poo&>(candidates.back().get()).MakeOrbit( (float)c * angle );
				candidates.pop_back();
			}
			Codex<Sound>::Retrieve( L"sounds\\poo_arise.wav" )->Play( 1.0f,1.5f );
			activated = true;
		}
	}
}
