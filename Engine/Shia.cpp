#include "Shia.h"
#include "SpriteEffect.h"
#include "World.h"
#include "BvShiaSlowRoll.h"
#include "SpriteElement.h"

Shia::Shia( const Vec2& pos )
	:
	Entity( pos,75.0f,90.0f,60.0f ),
	pBehavior( new SlowRoll( *this,{ 368.0f,300.0f } ) )
{}

void Shia::ProcessLogic( const World& world )
{}

void Shia::Update( World& world,float dt )
{
	pos += vel * dt;

	if( isDoingBoundaryAdjustment )
	{
		world.GetBoundsConst().Adjust( *this );
	}

	if( effectActive )
	{
		effectTime += dt;
		if( effectTime >= effectDuration )
		{
			effectActive = false;
		}
	}

	// handle brain state transition / update
	// delete old state if new one is returned
	// call activate on new state for 2nd part of init
	// and call update on the new one (repeat)
	while( auto pNewState = pBehavior->Update( *this,world,dt ) )
	{
		delete pBehavior;
		pBehavior = pNewState;
		pBehavior->Activate( *this,world );
	}

	sprite.Update( dt );
}

void Shia::ApplyDamage( float damage )
{
	effectActive = true;
	effectTime = 0.0f;
}

void Shia::Draw( Graphics& gfx ) const
{
	if( effectActive ) // draw damage flash
	{
		sprite.Draw( pos,gfx.GetScreenRect(),gfx,
			SpriteEffect::AlphaBakedSubstitution{ Colors::White },facingRight );
	}
	else // draw normal
	{
		sprite.Draw( pos,gfx.GetScreenRect(),gfx,
			SpriteEffect::AlphaBlendBaked{},facingRight );
	}
}

void Shia::DisplaceBy( const Vec2& d )
{
	// bounce off walls
	// (right now, displace is only used when we hit a wall)
	// if d.x != 0, then we know that we hit a vertical wall etc.
	if( d.x != 0.0f )
	{
		vel.x = -vel.x;
	}
	if( d.y != 0.0f )
	{
		vel.y = -vel.y;
	}
	// do the actual position displacement
	Entity::DisplaceBy( d );
}

Shia::Sprite::Sprite()
	:
	SpriteControl( 
[]( SpriteMode mode ) -> SpriteElement*
{
	switch( mode )
	{
	case SpriteMode::Standing:
		return new CompositeSpriteElement( {
			new SurfaceSpriteElement( Codex<Surface>::Retrieve( L"Images\\pm_roomba_left.png" ),
				{ -47.0f,-33.0f },{ -47.0f,-33.0f }
			),
			new SurfaceSpriteElement( Codex<Surface>::Retrieve( L"Images\\pm_shia_left.png" ),
				{ -26.0f,-161.0f },{ -26.0f,-161.0f }
			)
		} );
	case SpriteMode::Pooping:
		return new CompositeSpriteElement( {
			new SurfaceSpriteElement( Codex<Surface>::Retrieve( L"Images\\pm_roomba_left.png" ),
				{ -47.0f,-33.0f },{ -47.0f,-33.0f }
			),
			new AnimationSpriteElement(
				0,0,99,154,6,
				Codex<Surface>::Retrieve( L"Images\\pm_shia_poopin.png" ),
				{ 0.11f,0.11f,0.11f,0.11f,0.18f,std::numeric_limits<float>::max() },
				{ -66.0f,-160.0f },
				{ -35.0f,-160.0f }
			)
		} );
	case SpriteMode::Beam:
		return new CompositeSpriteElement( {
			new SurfaceSpriteElement( Codex<Surface>::Retrieve( L"Images\\pm_roomba_left.png" ),
				{ -47.0f,-33.0f },{ -47.0f,-33.0f }
			),
			new AnimationSpriteElement(
				0,0,120,160,11,
				Codex<Surface>::Retrieve( L"Images\\pm_shia_beam.png" ),
				{ 0.25f,0.25f,0.25f,0.25f,0.25f,0.25f,0.25f,0.25f,0.25f,0.25f,
				  std::numeric_limits<float>::max() },
				{ -66.0f,-160.0f },
				{ -53.0f,-160.0f }
			)
		} );
	default:
		assert( "Bad Mode in Sprite Element factor functor!" && false );
		return nullptr;
	}
},SpriteMode::Standing )
{}