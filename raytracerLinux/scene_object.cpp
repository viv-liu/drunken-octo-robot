/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"


// Note: To vivian, your intersection for t value was missing these, these fixes the back facing shadow, which i changed back to original 
bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel, const Matrix4x4& modelToWorld )
{
	// Implement intersection code for UnitSquare, which is
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
	Point3D origin(0,0,0);
		Vector3D rayDir = worldToModel * ray.dir;
		Point3D rayOrigin = worldToModel * ray.origin;
		Vector3D squareNormal(0,0,1);
		double t_value;
		Point3D intersectionPoint;

		if (rayDir[2] != 0)
		{
			t_value  = -rayOrigin[2]/rayDir[2];
			if ((t_value*t_value )< 0.00001){
				return false;
			}
		}

		//the ray and the square are on the same plane
		else if (rayDir[2] == 0 && rayOrigin[2] == 0)
		{
			double temp1 = (-1.5 - rayOrigin[0])/rayDir[0];
			double temp2 = (1.5 - rayOrigin[0])/rayDir[0];
			double temp3 = (-1.5 - rayOrigin[1])/rayDir[1];
			double temp4 = (1.5 - rayOrigin[1])/rayDir[1];
			t_value  = temp1;
			if (temp2 < t_value )
			{
				t_value  = temp2;
			}
			if (temp3 < t_value )
			{
				t_value  =temp3;
			}
			if (temp4< t_value )
			{
				t_value  = temp4;
			}
		}
		else
		{

			return false;
		}
	
	Ray3D ObjectRay = Ray3D(worldToModel*ray.origin, worldToModel * ray.dir);

	// Compute t_value from z intersection at z = 0
	//double t_value = (-1*ObjectRay.origin[2])/ObjectRay.dir[2]; // NOTE: THIS ITSELF IS WRONG!! NEED ALL CHECKING ABOVE 
	
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

			// For Image Mapping 			
			ray.intersection.CenterPoint = modelToWorld * Point3D(0,0,0); // every model thinks it is in the center 
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

		// FOR TEXTURE MAPPING!
		ray.intersection.CenterPoint = modelToWorld * Point3D(0,0,0); // every model thinks it is in the center 

		ray.intersection.none = false;

	//	std::cout << "Normal ";
	//		std::cout << ray.intersection.normal[0];
	//		std::cout << ray.intersection.normal[1];

	//		std::cout << ray.intersection.normal[2];
	//		std::cout <<"\n";
	//	std::cout << "Point ";
	//		std::cout << intersection[0];
	//		std::cout << intersection[1];

	//		std::cout << intersection[2];
	//		std::cout <<"\n";
		return true;
	}
}
// */


/*
// EXAMPLE 1
bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
        const Matrix4x4& modelToWorld ) {
    Point3D origin = worldToModel * ray.origin;
    Vector3D direction = worldToModel * ray.dir;

    Vector3D originVector = origin - Point3D(0, 0, 0);

    double a = direction.dot(direction),
           b = originVector.dot(direction),
           c = originVector.dot(originVector) - 1,
           d = b * b - a * c;

    if (d <= 0) {
        return false;
    }

    double t_1 = (- b + sqrt(d)) / a,
           t_2 = (- b - sqrt(d)) / a;

    double t = t_1 < t_2 ? t_1 : t_2;
    if (!ray.intersection.none && t >= ray.intersection.t_value) {
        return false;
    }

    Point3D point = origin + t * direction;

    Vector3D normal = 2 * (point - Point3D());
    normal.normalize();

    ray.intersection.t_value = t;
    ray.intersection.point = modelToWorld * point;
    ray.intersection.normal = transNorm(worldToModel, normal);
    ray.intersection.none = false;
    return true;
}
// */
/*
// Other Example 
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
	
	Point3D rayOrigin = worldToModel * ray.origin;

		Vector3D rayDir = worldToModel * ray.dir;
		Point3D sphereOrigin(0,0,0);
		double lambdaStar;
		//std::cout << "ray origin is " << rayOrigin << "\n";
		//std::cout << "ray direction is " << rayDir << "\n";

		double A = rayDir.dot(rayDir);
		double B = (rayOrigin-sphereOrigin).dot(rayDir);
		double C = (rayOrigin-sphereOrigin).dot(rayOrigin-sphereOrigin) - 1;
		double D = B*B-A*C;
/*
// EXAMPLE 1
bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
        const Matrix4x4& modelToWorld ) {
    Point3D origin = worldToModel * ray.origin;
    Vector3D direction = worldToModel * ray.dir;

    Vector3D originVector = origin - Point3D(0, 0, 0);

    double a = direction.dot(direction),
           b = originVector.dot(direction),
           c = originVector.dot(originVector) - 1,
           d = b * b - a * c;

    if (d <= 0) {
        return false;
    }

    double t_1 = (- b + sqrt(d)) / a,
           t_2 = (- b - sqrt(d)) / a;

    double t = t_1 < t_2 ? t_1 : t_2;
    if (!ray.intersection.none && t >= ray.intersection.t_value) {
        return false;
    }

    Point3D point = origin + t * direction;

    Vector3D normal = 2 * (point - Point3D());
    normal.normalize();

    ray.intersection.t_value = t;
    ray.intersection.point = modelToWorld * point;
    ray.intersection.normal = transNorm(worldToModel, normal);
    ray.intersection.none = false;
    return true;
}
// */
/*
// Other Example 
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
	
	Point3D rayOrigin = worldToModel * ray.origin;

		Vector3D rayDir = worldToModel * ray.dir;
		Point3D sphereOrigin(0,0,0);

		//std::cout << "A, B, C, D are "<< A << "," << B << "," << C << "," << D << "\n";
		if (D<0)
		{
			return false;
		}else if(D*D < 0.00001){
			double lambda = -B/A;
			if (((lambda-0)*(lambda-0)) < 0.00001){
				return false;
			}else{
				lambdaStar = lambda;
			}
		}
		else
		{
			double lambda_1 = -B/A + sqrt(D) / A;
			double lambda_2 = -B/A - sqrt(D) / A;
			//std::cout << "lambda1  "<< lambda_1 << " lambda2 " << lambda_2 << "\n";

			if (lambda_1 < 0 && lambda_2 < 0)
			{
				return false;
			}
			else if (lambda_1 > 0 && lambda_2 < 0)
			{
				lambdaStar = lambda_1;
			}

			else
			{
				lambdaStar = lambda_2;
			}

		}
		if (lambdaStar < 0.01){
			return false;
		}
		Point3D intersectionPoint = rayOrigin + lambdaStar * rayDir;
		Vector3D normal = intersectionPoint - sphereOrigin;

		if (!ray.intersection.none && lambdaStar > ray.intersection.t_value){

			return false;
		}
		ray.intersection.point = modelToWorld * intersectionPoint;
		//std::cout << "ray origin " << ray.origin << "\n";
		//std::cout << "intersection point " << ray.intersection.point << "\n";
	//	if ((ray.origin - ray.intersection.point).length() < 0.0001){
	//		return false;
	//	}

		ray.intersection.normal = modelToWorld * normal;
		ray.intersection.normal.normalize();
		ray.intersection.t_value = lambdaStar;
		ray.intersection.none = false;
		return true;


	//return false;
}
// */





