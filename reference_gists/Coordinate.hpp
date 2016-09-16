/*
 Reference Implementation:
 Widget Positioning Types
 By Patrick Hebron
 
 Notes:
 - Is there any value in templating Coord? 
	- integer usecase doesn't seem to0 valuable. image sizes?
	- double precision doesn't seem important here
 - Is there any reason to support inheritance?
	- not really. can always add later.
 - Do we want to lock into Cinder with ci::Anim?
	- Though unfortunate to split from ci::Anim, highly preferable to avoid Cinder dep at this level in codebase.
	- We could either copy Cinder's timeline impl into our codebase, but may want to take alt approach. Consider:
		- ci::app::timeline() uses a singleton. Need to double check whether there are any thread safety issues that might impact Cairo.
 - Do we want to lock into Cinder with ci::EaseFn?
	- If we're sticking with ci::Anim, this is a non-issue.
	- Otherwise, this would not be a terrible loss since source material is available / easy to implement.
 - If we decide to remove Cinder dep from this level of codebase, we'll also need to remove use of ci::Rectf in CoordRect.
 */

#pragma once

#include <iostream>
#include "cinder/Timeline.h"

namespace reference {
	
	struct Coord
	{
	private:
		
		ci::Anim<float> mAbs, mRel;
		
	public:
		
		Coord(float relative = 0.0, float absolute = 0.0)
		: mRel( relative ), mAbs( absolute ) { /* no-op */ }
		
		Coord& relative(float val, float duration = 0.0, ci::EaseFn easingFn = ci::EaseNone())
		{
			setRelative( val, duration, easingFn );
			return *this;
		}
		
		Coord& absolute(float val, float duration = 0.0, ci::EaseFn easingFn = ci::EaseNone())
		{
			setAbsolute( val, duration, easingFn );
			return *this;
		}
		
		void setRelative(float val, float duration = 0.0, ci::EaseFn easingFn = ci::EaseNone())
		{
			if( duration == 0.0 )
				mRel = val;
			else
				ci::app::timeline().apply( &mRel, val, duration, easingFn );
		}
		
		void setAbsolute(float val, float duration = 0.0, ci::EaseFn easingFn = ci::EaseNone())
		{
			if( duration == 0.0 )
				mAbs = val;
			else
				ci::app::timeline().apply( &mAbs, val, duration, easingFn );
		}
		
		float getRelative() const
		{
			return mRel.value();
		}
		
		float getAbsolute() const
		{
			return mAbs.value();
		}
		
		float rasterize(float parentMin, float parentMax) const
		{
			return ( parentMax - parentMin ) * mRel.value() + mAbs.value() + parentMin;
		}
	};
	
	struct CoordRect
	{
	private:
		
		Coord mX1, mY1, mX2, mY2;
		
	public:
		
		CoordRect(const Coord& x1 = Coord(), const Coord& y1 = Coord(), const Coord& x2 = Coord(), const Coord& y2 = Coord())
		: mX1( x1 ), mY1( y1 ), mX2( x2 ), mY2( y2 ) {}
		
		CoordRect& x1(const Coord& v) { mX1 = v; return *this; }
		CoordRect& y1(const Coord& v) { mY1 = v; return *this; }
		CoordRect& x2(const Coord& v) { mX2 = v; return *this; }
		CoordRect& y2(const Coord& v) { mY2 = v; return *this; }
		
		void setX1(const Coord& v) { mX1 = v; }
		void setY1(const Coord& v) { mY1 = v; }
		void setX2(const Coord& v) { mX2 = v; }
		void setY2(const Coord& v) { mY2 = v; }
		
		const Coord& getX1() const { return mX1; }
		const Coord& getY1() const { return mY1; }
		const Coord& getX2() const { return mX2; }
		const Coord& getY2() const { return mY2; }
		
		ci::Rectf rasterize(const ci::Rectf& parentRect) const
		{
			return ci::Rectf(  mX1.rasterize( parentRect.x1, parentRect.x2 )
							 , mY1.rasterize( parentRect.y1, parentRect.y2 )
							 , mX2.rasterize( parentRect.x1, parentRect.x2 )
							 , mY2.rasterize( parentRect.y1, parentRect.y2 ) );
		}
	};
	
} // namespace reference
