#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/ObjLoader.h"
#include "cinder/Arcball.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ModelLoadingExampleApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void draw();
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );

	void frameCurrentObject();

	Arcball myArcball;
	MayaCamUI myMayaCam;
	TriMesh myMesh;
	gl::VboMesh myVBO;
	gl::GlslProg myShader;
	gl::Texture mTexture;
};

// Set up the model, shader, texture, and camera
void ModelLoadingExampleApp::setup()
{
	// Helper class for loading obj files
	ObjLoader loader( (DataSourceRef)loadAsset( "cube.obj" ) );
	// Loads the mesh from the obj file
	loader.load( &myMesh );
	myVBO = gl::VboMesh( myMesh );
	
	// Loads the texture cube_texture from the assets folder
	mTexture = gl::Texture( loadImage( loadAsset( "cube_texture.jpg" ) ) );
	// Loads the shader
	myShader = gl::GlslProg( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );

	// Sets the intial camera, which is pretty far away from the cube
	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	myMayaCam.setCurrentCam( initialCam );

	
	mTexture.bind();
	myShader.bind();

	// Set the camera around the cube
	frameCurrentObject();
}

void ModelLoadingExampleApp::mouseDown( MouseEvent event )
{
	myMayaCam.mouseDown( event.getPos() );
}

// Check for 'o' keydown, to open a new texture
void ModelLoadingExampleApp::keyDown( KeyEvent event )
{
	// Open another texture for the cube
	if( event.getChar() == 'o')
	{
		fs::path path = getOpenFilePath();
		if( !path.empty())
		{
			mTexture = gl::Texture( loadImage( loadFile( path )));
		}
	}
}

// Drag the camera as the mouse drags on the screen
void ModelLoadingExampleApp::mouseDrag( MouseEvent event )
{
	myMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

// Frames the .obj file to fit in the camera completely
void ModelLoadingExampleApp::frameCurrentObject()
{
	Sphere boundingSphere = Sphere::calculateBoundingSphere( myMesh.getVertices() );

	myMayaCam.setCurrentCam( myMayaCam.getCamera().getFrameSphere( boundingSphere, 100));	
}


void ModelLoadingExampleApp::draw()
{
	gl::enableDepthWrite();
	gl::enableDepthRead();
	glDisable( GL_CULL_FACE );
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices( myMayaCam.getCamera() );

	myShader.bind();
	gl::pushMatrices();
	gl::rotate( myArcball.getQuat() );
	gl::draw(myVBO);
	gl::popMatrices();
	
}

CINDER_APP_NATIVE( ModelLoadingExampleApp, RendererGl )
