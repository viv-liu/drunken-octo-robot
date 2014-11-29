/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

	if(!ray.intersection.none) {
		// Intersection point to point light source position
		Vector3D s = this->_pos - ray.intersection.point;
		s.normalize();
		// Projection of s onto intersection normal
		Vector3D t = ray.intersection.normal.dot(s) * ray.intersection.normal;
		//t.normalize();
		// Reflection of s across n
		Vector3D r = -1 * s + 2 * t;
		r.normalize();
		// Intersection point to camera position
		Vector3D b = ray.origin - ray.intersection.point;
		b.normalize();
		// Material properties, easier access
		Colour mat_ambient = Colour(ray.intersection.mat->ambient);
		Colour mat_diffuse = Colour(ray.intersection.mat->diffuse);
		Colour mat_specular = Colour(ray.intersection.mat->specular);

		// Compute ray colours using Phong model
		double ray_col_r = mat_ambient[0] * this->_col_ambient[0] +
						   mat_diffuse[0] * this->_col_diffuse[0] * fmax(0, ray.intersection.normal.dot(s)) +
						   mat_specular[0] * this->_col_specular[0] * pow(fmax(0, r.dot(b)), ray.intersection.mat->specular_exp);
		
		double ray_col_g = mat_ambient[1] * this->_col_ambient[1] +
						   mat_diffuse[1] * this->_col_diffuse[1] * fmax(0, ray.intersection.normal.dot(s)) +
						   mat_specular[1] * this->_col_specular[1] * pow(fmax(0, r.dot(b)), ray.intersection.mat->specular_exp);				

		double ray_col_b = mat_ambient[2] * this->_col_ambient[2] +
						   mat_diffuse[2] * this->_col_diffuse[2] * fmax(0, ray.intersection.normal.dot(s)) +
						   mat_specular[2] * this->_col_specular[2] * pow(fmax(0, r.dot(b)), ray.intersection.mat->specular_exp);   
		double total_col = ray_col_r + ray_col_g + ray_col_b;
		ray.col = Colour(ray_col_r/total_col, ray_col_g/total_col, ray_col_b/total_col);

		// Clamp values down to 1.0
		ray.col.clamp();
	}
}

