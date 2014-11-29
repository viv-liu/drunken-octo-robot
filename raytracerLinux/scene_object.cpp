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

	Vector3D rayVec = Vector3D(ray.dir[0], ray.dir[1], ray.dir[2]);
	Point3D rayOrigin = Point3D(ray.origin[0], ray.origin[1], ray.origin[2]);
	
	rayVec = worldToModel * rayVec;
	rayOrigin = worldToModel * rayOrigin;
	
	Ray3D rayObjSpace = Ray3D(rayOrigin, rayVec);

	// Compute t_value from z intersection at z = 0
	double t_value = (-1*rayObjSpace.origin[2])/rayObjSpace.dir[2];
	
	// If the ray already has an intersection at an earlier t_value, return but don't update
	// ray.intersection fields
	if(ray.intersection.none == false) {
		if(t_value > ray.intersection.t_value) {
			return false;
		}
	}

	// Compute coordinates
	Vector3D intersection(rayObjSpace.origin[0] + rayObjSpace.dir[0] * t_value,
					     	   rayObjSpace.origin[1] + rayObjSpace.dir[1] * t_value,
							   0);
	//double x_at_t = rayObjSpace.origin[0] + rayObjSpace.dir[0] * t_value;
	//double y_at_t = rayObjSpace.origin[1] + rayObjSpace.dir[1] * t_value;

	// Test x
	if(intersection[0] >= -0.5 && intersection[0] <= 0.5) {
		// Text y
		if(intersection[1] >= -0.5 && intersection[1] <= 0.5) {
			// We have an intersection
			ray.intersection.point = Point3D(intersection[0], intersection[1], intersection[2]);
			ray.intersection.normal = Vector3D(0, 0, 1);
			ray.intersection.normal = transNorm(modelToWorld, ray.intersection.normal);
			ray.intersection.none = false;
			ray.intersection.t_value = t_value;

			// Convert back to world space
			ray.intersection.point = modelToWorld * ray.intersection.point;
			ray.intersection.normal = modelToWorld * ray.intersection.normal;
		
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
	
	Vector3D rayVec = Vector3D(ray.dir[0], ray.dir[1], ray.dir[2]);
	Point3D rayOrigin = Point3D(ray.origin[0], ray.origin[1], ray.origin[2]);
	
	rayVec = worldToModel * rayVec;
	rayOrigin = worldToModel * rayOrigin;
	
	Ray3D rayObjSpace = Ray3D(rayOrigin, rayVec);

	double a = pow(rayObjSpace.dir[0], 2) + pow(rayObjSpace.dir[1], 2) + pow(rayObjSpace.dir[2], 2);
	double b = 2 * (rayObjSpace.origin[0] * rayObjSpace.dir[0] +
					rayObjSpace.origin[1] * rayObjSpace.dir[1] +
					rayObjSpace.origin[2] * rayObjSpace.dir[2]);
	double c = pow(rayObjSpace.origin[0], 2) + 
			   pow(rayObjSpace.origin[1], 2) + 
			   pow(rayObjSpace.origin[2], 2) -1;

	double discriminant = pow(b, 2) - 4 * a * c;

	if(discriminant < 0) { // No intersection
		return false;

	} else if (discriminant == 0) { // Single intersection aka. tangent to sphere
		double t_value = -1 * b / (2 * a);
		// If the ray already has an intersection at an earlier t_value, return but don't update
		// ray.intersection fields
		if(ray.intersection.none == false) {
			if(t_value > ray.intersection.t_value) {
				return true;
			}
		}
		double x_at_t = rayObjSpace.origin[0] + rayObjSpace.dir[0] * t_value;
		double y_at_t = rayObjSpace.origin[1] + rayObjSpace.dir[1] * t_value;
		double z_at_t = rayObjSpace.origin[2] + rayObjSpace.dir[2] * t_value;
		double mag = sqrt(pow(x_at_t, 2) + pow(y_at_t, 2) + pow(z_at_t, 2));

		ray.intersection.point = Point3D(x_at_t, y_at_t, z_at_t);
		ray.intersection.normal = Vector3D(x_at_t/mag, y_at_t/mag, z_at_t/mag);
		ray.intersection.none = false;
		ray.intersection.t_value = t_value;

		// Convert back to world space
		ray.intersection.point = modelToWorld * ray.intersection.point;
		ray.intersection.normal = transNorm(worldToModel, ray.intersection.normal);
		return true;

	} else { // LINE has two points of intersection, however, RAY only permits positive values of t
		double t_1 = (-1 * b + sqrt(discriminant)) / (2 * a);
		double t_2 = (-1 * b - sqrt(discriminant)) / (2 * a);
		double t_value = 0;

		if(t_1 < 0 && t_2 < 0) { // No ray intersection; all negative values of t so intersection is past the tail of the ray
			return false; 
		} else { // At least one 0 or positive value of t
			if(t_1 >= 0) t_value = t_1;
			if(t_2 >= 0) t_value = t_2;
			if(t_1 >= 0 && t_2 >= 0) t_value = t_1 > t_2 ? t_2 : t_1; // If both are positive, keep smaller t_value aka. first intersection
		}
		// If the ray already has an intersection at an earlier t_value, return but don't update
		// ray.intersection fields
		if(ray.intersection.none = false) {
			if(t_value > ray.intersection.t_value) {
				return false;
			}
		}
		// Compute coordinates
		Vector3D intersectionPoint(rayObjSpace.origin[0] + rayObjSpace.dir[0] * t_value,
		rayObjSpace.origin[1] + rayObjSpace.dir[1] * t_value,
		rayObjSpace.origin[2] + rayObjSpace.dir[2] * t_value);

		ray.intersection.point = Point3D(intersectionPoint[0], intersectionPoint[1], intersectionPoint[2]);
		intersectionPoint.normalize();
		ray.intersection.normal = transNorm(modelToWorld, intersectionPoint);
		ray.intersection.none = false;
		ray.intersection.t_value = t_value;

		// Convert back to world space
		//ray.intersection.point = modelToWorld * ray.intersection.point;
		std::cout << "Point ";
		std::cout << ray.intersection.point[0];
		std::cout << ray.intersection.point[1];
		std::cout << ray.intersection.point[2];
		std::cout <<"\n";
		return true;
	}
}

