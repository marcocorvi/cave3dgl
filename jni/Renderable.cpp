#include "Renderable.h"

#include "Geometry.h"

Vector3 
Renderable::GetBoundMin() const
{
  return geometry->GetMin();
}

Vector3
Renderable::GetBoundMax() const 
{
  return geometry->GetMax();
}

