/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	Ray3D ObjectRay = Ray3D(worldToModel*ray.origin, worldToModel * ray.dir);

	// Compute t_value from z intersection at z = 0
	double t_value = (-1*ObjectRay.origin[2])/ObjectRay.dir[2];
	
	// If the ray already has an intersection at an earlier t_value, return but don't update
	// ray.intersection fields
	if(ray.intersection.none == false) {
		if(t_value > ray.intersection.t_value) {
			return false;
		}
	}

	// Compute coordinates
	Vector3D intersection(ObjectRay.origin[0] + ObjectRay.dir[0] * t_value,
					     	   ObjectRay.origin[1] + ObjectRay.dir[1] * t_value,
							   0);
	double x = ObjectRay.origin[0] + ObjectRay.dir[0] * t_value;
	double y = ObjectRay.origin[1] + ObjectRay.dir[1] * t_value;

	// Test x
	if(x >= -0.5 && x <= 0.5) {
		// Text y
		if(y >= -0.5 && y <= 0.5) {
			// We have an intersection
			ray.intersection.t_value = t_value;
			ray.intersection.point = modelToWorld * Point3D(x, y, 0);
			ray.intersection.normal = Vector3D(0, 0, 1);
			
			ray.intersection.normal = transNorm(worldToModel, ray.intersection.normal);
			ray.intersection.normal.normalize();
			ray.intersection.none = false;

			return true;
		}
	}

	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	Ray3D ObjectRay = Ray3D(worldToModel * ray.origin, worldToModel * ray.dir);

	double a = pow(ObjectRay.dir[0], 2) + pow(ObjectRay.dir[1], 2) + pow(ObjectRay.dir[2], 2);
	double b = 2 * (ObjectRay.origin[0] * ObjectRay.dir[0] +
					ObjectRay.origin[1] * ObjectRay.dir[1] +
					ObjectRay.origin[2] * ObjectRay.dir[2]);
	double c = pow(ObjectRay.origin[0], 2) + 
			   pow(ObjectRay.origin[1], 2) + 
			   pow(ObjectRay.origin[2], 2) -1;

	double discriminant = pow(b, 2) - 4 * a * c;

	if(discriminant < 0) { // No intersection
		return false;

	} else {
		double t_1 = (-1 * b + sqrt(discriminant)) / (2 * a);
		double t_2 = (-1 * b - sqrt(discriminant)) / (2 * a);
		double t_value = 0;

		if ( t_2 < 0 && t_1 < 0)
			return false;
		else if ( t_1 < 0 )
			t_value = t_2;
		else if ( t_2 < 0 )
			t_value = t_1;
		else
			t_value = fmin(t_1, t_2);

		// If the ray already has an intersection at an earlier t_value, return but don't update
		// ray.intersection fields
		if(ray.intersection.none == false) {
			if(t_value > ray.intersection.t_value) {
				return true;
			}
		}

		double x = ObjectRay.origin[0] + ObjectRay.dir[0]*t_value;
		double y = ObjectRay.origin[1] + ObjectRay.dir[1]*t_value;
		double z = ObjectRay.origin[2] + ObjectRay.dir[2]*t_value;
		
		Vector3D intersection = Vector3D(x, y, z);
		ray.intersection.t_value = t_value;		
		ray.intersection.point = modelToWorld * Point3D(x, y, z);
		ray.intersection.normal = transNorm(worldToModel, intersection);
		ray.intersection.normal.normalize();		
		ray.intersection.none = false;

		/*std::cout << "Normal ";
			std::cout << ray.intersection.normal[0];
			std::cout << ray.intersection.normal[1];
			std::cout << ray.intersection.normal[2];
			std::cout <<"\n";*/
		std::cout << "Point ";
			std::cout << intersection[0];
			std::cout << intersection[1];
			std::cout << intersection[2];
			std::cout <<"\n";
		return true;
	}
}

