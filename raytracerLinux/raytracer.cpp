/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/
//The main body of the raytracer, including the scene graph.  Simple 
// addition and traversal code to the graph are provided to you. 

#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

//----------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------

// Initialize the root with no light source
Raytracer::Raytracer() : _lightSource(NULL) {
	// The root is initialize with a new node where everything in this  node is NULL 
	_root = new SceneDagNode();
}

// Delete the root only 
Raytracer::~Raytracer() {
	delete _root;
}

//----------------------------------------------------------------------------------------------------------
// Adding Scene Objects to the Directed Acyclic Graph 
//----------------------------------------------------------------------------------------------------------
// This is a method from RayTracer buy returns a node instead
SceneDagNode* Raytracer::addObject( SceneDagNode* parent, SceneObject* obj, Material* mat ) 
{
	// Once Raytracer is created, root points to a created object, thus, parent points to that object 

	// Create a new node with SceneDagNode for the Directed Acyclic Graph  with parent object and 
	// Note: Created with parent pointing to NULL 
	SceneDagNode* node = new SceneDagNode( obj, mat ); // makes a new sceneDagNode with sceneObject obj, and material mat.
							   // Every node in the graph therefore links the object with the material 
							   // as well as the matrix transformation associated with it. 

	// Make this node point to the parent 
	node->parent = parent;  // Note: Originally _root points to NULL, so first object added has parent point to NULL 
	// Initialize the next to NULL (basically sibling), transformation applied to this node not applied to child 
	node->next = NULL;
	// Initialize the child to NULL, transformation applied to this node will be applied to child
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) 
	{
		parent->child = node;
	}
	// go to the child of your parent and add it to the siblings of that child 
	else {
		parent = parent->child;
		while (parent->next != NULL) 
		{
			parent = parent->next;
		}
		parent->next = node;
	}
	return node;
}

//----------------------------------------------------------------------------------------------------------
// Adding Light objects to the linked list  
//----------------------------------------------------------------------------------------------------------
LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;	// temp points to head of the linkedlist which may be null 
	// Add the light source to the node and return it 
	_lightSource = new LightListNode( light, tmp ); // create a new light with next pointing to head of linked list or null if it is initially empty 
							// make the original _lightSource in ray tracer point to the new head of the node 
	return _lightSource;	 // return the head of the linked list 
}

//----------------------------------------------------------------------------------------------------------
// Transformation on each individual scene object given the SceneDagNode that contains that scene object
//----------------------------------------------------------------------------------------------------------
void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	// Create a matrix for transoformation 
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	// QUESTION: Don't get the i 0 to 2 part 
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		//Question What is this? 
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	// A temporary matrix to store the information for translation and it's inverse
	// Node => The SceneDagNode that stores the SceneObject and material properties
		// as well as all the transformation it has to be multiplied into 
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	// Update the node's transformation to multiply with the translation  (world to object) 
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];	// put a negative, which basically means it is the inverse translation 
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	// Update the inverse transformation to multiply with translation (object to world)
	node->invtrans = translation*node->invtrans; 
	// transformation = transformation*A
	// inverseTransformation = Ainverse*inverseTransformation 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	// Create a matrix called scale to hold A 
	Matrix4x4 scale;
	
	// Create A 
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	// transformation = transformation*A
	node->trans = node->trans*scale; 	
		
	// Create Ainverse
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
	// inverseTransformation = Ainverse*inverseTransformation 
}

//----------------------------------------------------------------------------------------------------------
// ShadeRay() Helper Functions, note: ShadeRay() itself is a helper function for Render()
//----------------------------------------------------------------------------------------------------------

// Helper function called by shadeRay() 
void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) 
{
	SceneDagNode *childPtr;
	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel; 
	if (node->obj)
	{
		// Perform intersection.
		if (node->obj->intersect(ray, _worldToModel, _modelToWorld)) 
		{
			ray.intersection.mat = node->mat;
		}
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray);
		childPtr = childPtr->next;
	}

	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

// Helper function called by shadeRay() 
void Raytracer::computeShading( Ray3D& ray ) 
{
	LightListNode* curLight = _lightSource;
	for (;;) {
		if (curLight == NULL) break;
		// Each lightSource provides its own shading function.

		// SHADOWS
		bool isInShadow = false;
		
		Point3D lightPos = curLight->light->get_position();
		if(ray.intersection.none == false) {
			// Construct a ray from intersection point on ray to light source
			Point3D intersectPos = ray.intersection.point;
			double dx = lightPos[0] - intersectPos[0];
			double dy = lightPos[1] - intersectPos[1];
			double dz = lightPos[2] - intersectPos[2];
			double d = 0.01;

			// Set slight offset from intersection
			intersectPos[0] = dx > 0 ? intersectPos[0] + d : intersectPos[0] - d;
			intersectPos[1] = dy > 0 ? intersectPos[1] + d : intersectPos[1] - d;
			intersectPos[2] = dz > 0 ? intersectPos[2] + d : intersectPos[2] - d;

			Vector3D vec = Vector3D(dx, dy, dz);
			double epsilon = 0.001; 
			Ray3D shadowRay = Ray3D(lightPos, intersectPos-lightPos);
			traverseScene(_root, shadowRay);
		
			// Check if light intersects at intersectPos on object, if not then it's in shadow 
			if(shadowRay.intersection.none == false) 
			{
				//double t_light = (lightPos[0] - shadowRay.origin[0]) / shadowRay.dir[0];
				Point3D shadowRayIntersect = shadowRay.intersection.point;
				if( shadowRay.intersection.t_value >= epsilon &&   shadowRay.intersection.t_value<= 1-epsilon)
				{
					isInShadow = true; 
				}
			}	
			//std::cout<< isInShadow;
		}
		curLight->light->shade(ray, isInShadow);
		curLight = curLight->next;
	}
}

//----------------------------------------------------------------------------------------------------------
// Render() Helper Functions 
//----------------------------------------------------------------------------------------------------------

// This initializes the view matrix based on the position, direction and orientation of the eye/camera . 
Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, Vector3D up) 
{
	// Create a 4by4 matrix 
	Matrix4x4 mat; 

	// Create a 3D Vector, w 
	Vector3D w;
	
	// Normalize the view/direction Vector given 
	view.normalize();
	
	// Question: What does this do? 
	// Update the orientation vector to be 
	// orientation = orientation - (orientation(dot)view)*view
	up = up - up.dot(view)*view; 
			// (make the view vector the length of the dot product)
			// make the up vector deduct the view vector 
	
	// normalize the orientation vector 
	up.normalize();
	
	// Set up the 3D vector created to be the normal of the view vector and the up vector 
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];
// mat => resulting matrix
// 	wx	upx	-viewx	 eyex
//	wy	upy	-viewy	 eyey
// 	wz	upz	-viewz	 eyez
	return mat; 
}

// Helper function called by render()
// This function initializes the pixel buffer assuming _scrWidth & _scrheight has been given the right values 
void Raytracer::initPixelBuffer() 
{
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) 
	{
		for (int j = 0; j < _scrWidth; j++) 
		{
			_rbuffer[i*_scrWidth+j] = 0;
			_gbuffer[i*_scrWidth+j] = 0;
			_bbuffer[i*_scrWidth+j] = 0;
		}
	}
}

// Helper Function called by render() 
// This flushes the pixel buffer by writing it onto the bmp file, and then deleting all the buffers for thsi ray tracer 
void Raytracer::flushPixelBuffer( char *file_name ) 
{
	// Note: Change this function if you want to write to an image with a different format or something else (VGA Screen, etc.) 
	bmp_write( file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}

// This function computes a color for a particular pixel, given a ray towards that pixel. 
Colour Raytracer::shadeRay( Ray3D& ray ) 
{
	// Initialize the color to black 
	Colour col(0.0, 0.0, 0.0); 
	// Traverse from the root 
	traverseScene(_root, ray); 
	
	// Only shade if ray hit something
	if (!ray.intersection.none) 
	{
		computeShading(ray); 
		col = ray.col;  
	}

	// TODO: 
	// You'll want to call shadeRay recursively (with a different ray, 
	// of course) here to implement reflection/refraction effects.  
	return col; 
}	

//----------------------------------------------------------------------------------------------------------
// Render()
//----------------------------------------------------------------------------------------------------------

// This function renders the entire scene once it is done being set up. 
void Raytracer::render( int width, int height, Point3D eye, Vector3D view, Vector3D up, double fov, char* fileName ) 
{
	// Create a matrix called view to world 
	Matrix4x4 viewToWorld;
	// Set the screen width and the screen height of the raytracer to width and height given 
	_scrWidth = width;
	_scrHeight = height;
	
	// Calculate the factor 
	// QUESTION: WHAT IS THIS????? 
	double factor = (double(height)/2)/tan(fov*M_PI/360.0); // used to determine where the rays get projected 

	// Initialize the pixel buffer after setting _scrWidth and _scrHeight 
	initPixelBuffer();

	// Initialize the viewToWorld Matrix given the camera properties 
	viewToWorld = initInvViewMatrix(eye, view, up);

	
	// Construct a ray for each pixel.
	for (int i = 0; i < _scrHeight; i++) 
	{
		for (int j = 0; j < _scrWidth; j++) 
		{
			// Sets up ray origin and direction in view space, image plane is at z = -1.
			Point3D origin(0, 0, 0); // Create a point called origin 
			Point3D imagePlane;	// the image plane where it is some distance away from the camera 
						// It initializes the ray direction 
			imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor; // x position of image plane (factor influences the final width of the image plane) 
			imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor; // y position of image plane 
// Anti-aliasing
//imagePlane[0] = (-double(width)/2 + ((double)rand()/(double)RAND_MAX) + j)/factor;
//imagePlane[1] = (-double(height)/2 + ((double)rand()/(double)RAND_MAX) + i)/factor;
			imagePlane[2] = -1; // z position of image plane 
			
//std::cout << ((double)rand()/(double)RAND_MAX);
// TODO: Convert ray to world space and call 

			// Construct the ray direction based on the points on the image plane computed 
			Vector3D rayDir = Vector3D(imagePlane[0], imagePlane[1], imagePlane[2]);

			// update the ray Direction to change from the view matrix to the world matrix 
			rayDir = viewToWorld * rayDir;
			// normalize the ray direction 
			rayDir.normalize();
			// Now create the actual ray given the origin and ray direction (from the point origin given above) 
			Ray3D ray = Ray3D(origin, rayDir);

			// Get the color for this pixel by calling the shade ray function 
			// shadeRay(ray) to generate pixel colour. 	
			Colour col = shadeRay(ray); 
			// Finally, color the pixel based on the color that was given 
			_rbuffer[i*width+j] = int(col[0]*255);
			_gbuffer[i*width+j] = int(col[1]*255);
			_bbuffer[i*width+j] = int(col[2]*255);
		}
	}
	// Finally, flush the entire pixelbuffer to the file name given. 
	flushPixelBuffer(fileName);
}

//----------------------------------------------------------------------------------------------------------
// Main Function () 
//----------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{	
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	//-----------------------------------------------------------------------------------------------------------
	// Step1 : Initialize the main raytracer, screen wdith and height and camera 
	//-----------------------------------------------------------------------------------------------------------
	Raytracer raytracer;
		//-------------------------------------------------
		// contains: int 		_scrWidth
		// 	     int 		_scrHeight
		// 	     LightListNode* 	_lightSource 	// a linked list of light sources
		//	     SceneDagNode*	_root		// The root of the linked list graph data structure 
		// 	     unsigned char*	_rbuffer;	
		// 	     unsigned char*	_gbuffer;	
		// 	     unsigned char*	_bbuffer;	
		//	     Matrix4x4		_modelToWorld
		//	     Matrix4x4		_worldToModel
		//-------------------------------------------------

	// Set default width and height of the pixel screen 
	int width = 320; 
	int height = 240; 

	// Update width and height if given in commandline 
	if (argc == 3) 
	{
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);

	// Angle for field of view is 60 degrees
	double fov = 60;
	//-----------------------------------------------------------------------------------------------------------
	// Step2 : Add Light Sources and Scene Objects into the scene 
	//-----------------------------------------------------------------------------------------------------------

	// Create materials with AmbientColor, DiffuseColor, SpecularcColor, and SpecularExponent 
	// Note: Material is used for shading 
	// Material 1: Gold
	Material gold( Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648), Colour(0.628281, 0.555802, 0.366065), 51.2);
//Material gold( Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648), Colour(0.628281, 0.555802, 0.366065), 1); // Test: Material with lower exponent 
	// Material 2: Jade
	Material jade( Colour(0, 0, 0), Colour(0.54, 0.89, 0.63), Colour(0.316228, 0.316228, 0.316228), 12.8 );
	
	// Defines a point light source with location in 3D as well as the color of the light.  (closed to white) 
	// Add the point light source to the ray tracer's list of lightSource 
	raytracer.addLightSource( new PointLight(Point3D(-5, 0, 0), Colour(0.9, 0.9, 0.9)));
	raytracer.addLightSource( new PointLight(Point3D(5, 0, 0), Colour(0.9, 0.9, 0.9)));

	// TODO: Make sure code works with multiple light sources 
// raytracer.addLightSource( new PointLight(Point3D(5, 0, 0), Colour(0.4, 0.4, 0.4))); // Test: Darker light source 

	// Need associate created material with objects 
	// Adding actual objects to world  (known as SceneObject) 

	// Add a unit square into the scene with material mat
	// Note: It is pointer that points to objects created and returned by raytracer 
						// (SceneObject, Material) 
	SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &gold );
	SceneDagNode* plane = raytracer.addObject( new UnitSquare(), &jade );
	// now root has 2 childs, both the unitSphere and the unitSquare 
	// but really it is
	// root->child->Sphere
	// root->Sphere->next->plane
	
	//-----------------------------------------------------------------------------------------------------------
	// Step3 : Apply transformation to the objects to put them in place 
	//-----------------------------------------------------------------------------------------------------------
	// Apply some transformations to the unit square.

	// Create a factor for scaling (ScaleX, ScaleY, ScaleZ) 
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };

	// Do transformations on sphere 
	// Add the translation matrix to the sphere's list of transformation and inverse transformation 
	raytracer.translate(sphere, Vector3D(0, 0, -5));	
	// Similarly, add rotations and scaling 
	raytracer.rotate(sphere, 'x', -45); 
	raytracer.rotate(sphere, 'z', 45); 
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

	// Similarly, do transformations on the plane 
	raytracer.translate(plane, Vector3D(0, 0, -7));	
	raytracer.rotate(plane, 'z', 45); 
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	//---------------------------------------------------------------------------------------------------------------
	// Step 4: Render the scene given the width, height, camera location, orientation, frame of view, and direction
	//---------------------------------------------------------------------------------------------------------------
	// Render the scene, feel free to make the image smaller for testing purposes.	
	// Note: Render creates and flushes the pixel buffer in 2 different functions (it creates the pixel buffer in render() and flushes it in flushPixelBuffer() that is called by render() 
	raytracer.render(width, height, eye, view, up, fov, "view1.bmp");	// output the image with name view1.bmp
	
	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	//Point3D eye2(2, 0, 1);
	//Vector3D view2(0, 0, -1);
	raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");	// output the image with name view2.bmp 
	
	return 0;
}

