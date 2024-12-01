#pragma once
#include "Vector2.hpp"

/* Interpolation function */
inline double lerp(double a, double b, double c) { return a+((b-a)*c); }


/* Limit function */
inline double limit(double value, std::optional<double> min = std::nullopt, std::optional<double> max = std::nullopt) { return ((min.has_value())?((max.has_value())?(value>max?max:(value<min?min:value)):(value<min?min:value)):((max.has_value())?(value>max?max:value):value)).value(); }


/* Within limit function */
inline bool inlimit(double value, std::optional<double> min = std::nullopt, std::optional<double> max = std::nullopt) { return (min.has_value())?((max.has_value())?(value<max&&value>=min):(value>=min)):((max.has_value())?(value<max):true); }


/* Contained function */
inline bool contained(Vector2 point, SDL_FRect container) { return ((container.w>0)?point.x>=container.x:point.x<=container.x) && ((container.h>0)?point.y>=container.y:point.y<=container.y) && ((container.w>0)?point.x<=container.x+container.w:point.x>=container.x+container.w) && ((container.h>0)?point.y<=container.y+container.h:point.y>=container.y+container.h); }
