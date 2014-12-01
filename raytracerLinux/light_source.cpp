/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/
// Defines the basic light class.  You could define different types of 
// lights, which shades the ray differently.  Point lights are sufficient 
// for most scenes.  

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray, bool isInShadow ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

	if(!ray.intersection.none) {
		// Intersection point to point light source position
		Vector3D s = Vector3D(this->_pos - ray.intersection.point);
		s.normalize();

		// Reflection of s across n
		Vector3D r = Vector3D(-1 * s + 2 * ray.intersection.normal.dot(s) * ray.intersection.normal);

		r.normalize();
		// Intersection point to camera position
		Vector3D b = ray.origin - ray.intersection.point;
		b.normalize();

		// Material properties, easier access
		Colour mat_a =  Colour(ray.intersection.mat->ambient);
		Colour mat_d =  Colour(ray.intersection.mat->diffuse);
		Colour mat_s =  Colour(ray.intersection.mat->specular);
		
		// Light source properties
		Colour I_a =   this->_col_ambient;
		Colour I_d =  this->_col_diffuse;
		Colour I_s =  this->_col_specular;

		double alpha = ray.intersection.mat->specular_exp;

		// Compute ray colours using Phong model
		ray.col[0] += mat_a[0] * I_a[0]; 
		
		ray.col[1] += mat_a[1] * I_a[1];

		ray.col[2] += mat_a[2] * I_a[2];
					 
		if(isInShadow == false) {
			ray.col[0] += mat_d[0] * I_d[0] * fmax(0, s.dot(ray.intersection.normal)) +
					 mat_s[0] * I_s[0] * pow(fmax(0, r.dot(b)), alpha);
			ray.col[1] += mat_d[1] * I_d[1] * fmax(0, s.dot(ray.intersection.normal)) +
					 mat_s[1] * I_s[1] * pow(fmax(0, r.dot(b)), alpha);
			ray.col[2] += mat_d[2] * I_d[2] * fmax(0, s.dot(ray.intersection.normal)) +
					 mat_s[2] * I_s[2] * pow(fmax(0, r.dot(b)), alpha);
		}

		// Clamp values down to 1.0
		ray.col.clamp();
	}
}

