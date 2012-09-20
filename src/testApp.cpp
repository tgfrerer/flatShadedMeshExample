#include "testApp.h"

/*
    _____    ___
   /    /   /  /     flatShadedMeshExample
  /  __/ * /  /__    (c) ponies & light, 2012.
 /__/     /_____/    poniesandlight.co.uk/notes/flat_shading_on_osx/
 
 Created by Tim Gfrerer 2012.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 
 */

//--------------------------------------------------------------
void testApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	isShaderDirty = true;

	mLigDirectional.setup();
	mLigDirectional.setDirectional();

	mCamMainCam.setupPerspective(false);
	mLigDirectional.setAmbientColor(ofColor::fromHsb(0, 0, 200));
	mLigDirectional.setDiffuseColor(ofColor::fromHsb(120, 120, 128));
	mLigDirectional.setSpecularColor(ofColor(255,255,255));
	
	
	mMatMainMaterial.setDiffuseColor(ofColor(0,0,0));
	mMatMainMaterial.setSpecularColor(ofColor(200,200,200));
	mMatMainMaterial.setShininess(25.0f);
	
	mCamMainCam.setDistance(200);
	
	
	shouldDrawBuiltinBox = false;
	shouldRenderNormals = false;
	shouldUseFlatShading = true;
	
	// create cube mesh
	
	ofVec3f vertices[] = {
	
		ofVec3f(-1, -1,  1),		// front square vertices
		ofVec3f( 1, -1,  1),
		ofVec3f( 1,  1,  1),
		ofVec3f(-1,  1,  1),
		
		ofVec3f(-1, -1, -1),		// back square vertices
		ofVec3f( 1, -1, -1),
		ofVec3f( 1,  1, -1),
		ofVec3f(-1,  1, -1),
		
	};
	
	ofIndexType indices[] = {
		// -- winding is counter-clockwise (facing camera)
		0,1,2,		// pos z
		0,2,3,
		1,5,6,		// pos x
		1,6,2,
		2,6,7,		// pos y
		2,7,3,

		// -- winding is clockwise (facing away from camera)
		3,4,0,		// neg x
		3,7,4,
		4,5,1,		// neg y
		4,1,0,
		5,7,6,		// neg z
		5,4,7,
	};
	
	ofVec3f normals[] = {
		ofVec3f( 0,  0,  1),
		ofVec3f( 1,  0,  0),
		ofVec3f( 0,  1,  0),
		ofVec3f(-1,  0,  0),
		ofVec3f( 0, -1,  0),
		ofVec3f( 0,  0, -1),
		ofVec3f(1,0,0), // can be anything, will not be used
		ofVec3f(1,0,0), //  -- " --
	};
	
	mMshCube.addVertices(vertices, 8);
	mMshCube.addNormals(normals,8);
	mMshCube.addIndices(indices, 3*2*6);
}

//--------------------------------------------------------------
void testApp::update(){

	if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (mShdPhong != NULL ) delete mShdPhong;
		mShdPhong = new ofShader();
		mShdPhong->load("shaders/phong");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;

		
		isShaderDirty = false;
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	
	glShadeModel(GL_SMOOTH);
	glProvokingVertex(GL_LAST_VERTEX_CONVENTION);

	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));
	
	mCamMainCam.begin();
	
	ofEnableLighting();

	mLigDirectional.setGlobalPosition(1000, 1000, 1000);
	mLigDirectional.lookAt(ofVec3f(0,0,0));

	ofEnableSeparateSpecularLight();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	mLigDirectional.enable();
	ofSetColor(mLigDirectional.getDiffuseColor());
	mMatMainMaterial.begin();

	mShdPhong->begin();

	if (shouldRenderNormals){
		mShdPhong->setUniform1f("shouldRenderNormals", 1.0);
	} else {
		mShdPhong->setUniform1f("shouldRenderNormals", 0.0);
	}

	glPushAttrib(GL_SHADE_MODEL);
	if (shouldUseFlatShading){
		mShdPhong->setUniform1f("shouldUseFlatShading", 1.0);
		glShadeModel(GL_FLAT);
		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);		// OpenGL default is GL_LAST_VERTEX_CONVENTION
	} else {
		mShdPhong->setUniform1f("shouldUseFlatShading", 0.0);
		glShadeModel(GL_SMOOTH);
		glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
	}
	
	if (shouldDrawBuiltinBox) {
		ofBox(0, 0, 0, 100);
	} else {
		ofPushMatrix();
		ofScale(50, 50,50);
		mMshCube.draw();
		ofPopMatrix();
	}
	
	// restores shade model
	glPopAttrib();
	// restores vertex convention defaults.
	glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
	
	mShdPhong->end();
	
	// we revert to default values, to not end up 

	
	mMatMainMaterial.end();
	mLigDirectional.disable();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	ofDisableLighting();

	ofSetColor(255);
	for (int i=0; i<8; i++){
		// we do a manual scale by scaling the mesh vertex coord by 50.f to render the point at the vertex position
		ofDrawBitmapString(ofToString(i), mMshCube.getVertex(i) * 50.0f);
	}

	mCamMainCam.end();

	// draw HUD items outside of camera.
	
	string builtInIndicator = ((shouldDrawBuiltinBox) ? "built-in" : "mesh-based");
	ofDrawBitmapStringHighlight( "Rendering " + builtInIndicator + " box. Press 'b' to toggle."  , ofVec3f(10,20), ofColor(255), ofColor(0));
	
	string normalsIndicator = ((shouldRenderNormals) ? "normals" : "faces");
	ofDrawBitmapStringHighlight( "Showing " + normalsIndicator + ". Press 'n' to toggle."  , ofVec3f(10,40), ofColor(255), ofColor(0));

	string flatShadingIndicator = ((shouldUseFlatShading) ? "on" : "off");
	ofDrawBitmapStringHighlight( "Flat Shading is " + flatShadingIndicator + ". Press 'f' to toggle."  , ofVec3f(10,60), ofColor(255), ofColor(0));

	ofDrawBitmapStringHighlight( "Press SPACEBAR to reload shader."  , ofVec3f(10,80), ofColor(0), ofColor(255));

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key) {
		case ' ':
			isShaderDirty = true;
			break;
		case 'b':
			shouldDrawBuiltinBox ^= true;
			break;
		case 'n':
			shouldRenderNormals ^= true;
			break;
		case 'f':
			shouldUseFlatShading ^= true;
			break;
		default:
			break;
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}