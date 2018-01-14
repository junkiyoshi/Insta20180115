#include "ofApp.h"

//--------------------------------------------------------------
ofApp::~ofApp() {
	for (ofxBulletSphere* tmp : this->spheres) {
		delete tmp;
	}
	this->spheres.clear();
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofEnableDepthTest();
	ofBackground(0);
	ofSetWindowTitle("Insta");

	this->cam.setPosition(ofVec3f(0, 3.f, -30.f));
	this->cam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));

	this->world.setup();
	this->world.enableGrabbing();
	this->world.setCamera(&this->cam);
	this->world.setGravity(ofVec3f(0.0, 0.0, 0.0));

	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 130, true, false, true);
	vector<string> words;
	words.push_back("JUN");
	words.push_back("KIYOSHI");
	
	for (int word_index = 0; word_index < words.size(); word_index++) {
		ofVec2f word_size = ofVec2f(this->font.stringWidth(words[word_index]), -this->font.stringHeight(words[word_index]));
		vector<ofTTFCharacter> char_paths = this->font.getStringAsPoints(words[word_index]);
		for (int path_index = 0; path_index < char_paths.size(); path_index++) {
			vector<ofPolyline> outline = char_paths[path_index].getOutline();
			for (int outline_index = 0; outline_index < outline.size(); outline_index++) {
				outline[outline_index] = outline[outline_index].getResampledBySpacing(5);
				vector<ofPoint> vertices = outline[outline_index].getVertices();
				for (int vertices_index = 0; vertices_index < vertices.size(); vertices_index++) {
					ofVec3f location = vertices[vertices_index] - word_size / 2;
					location.y = location.y + 150 * (word_index - 1);

					ofxBulletSphere* sphere = new ofxBulletSphere();
					ofVec3f box_location = ofVec3f(location.x, location.y, location.z);
					float size = 2.5;
					sphere->create(this->world.world, box_location, 1.0, size);
					sphere->setRestitution(1.0);
					sphere->add();

					this->spheres.push_back(sphere);

					ofColor color;
					color.setHsb(ofRandom(255), 255, 255);
					this->spheres_color.push_back(color);
					this->spheres_location.push_back(box_location);
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofVec3f diff;
	for (int i = 0; i < this->spheres.size(); i++) {
		diff = this->spheres_location[i] - this->spheres[i]->getPosition();
		diff *= 10;
		this->spheres[i]->applyCentralForce(diff);
	}

	Leap::Frame frame = this->leap.frame();
	for (Leap::Hand hand : frame.hands()) {
		if(hand.isRight()){
			for (Leap::Finger finger : hand.fingers()) {
				if (finger.type() == Leap::Finger::Type::TYPE_INDEX) {
					float x = ofMap(finger.tipPosition().x, -200, 200, -ofGetWidth() / 2, ofGetWidth() / 2);
					float y = ofMap(finger.tipPosition().y, 300, 0, -ofGetHeight() / 2, ofGetHeight() / 2);
					float power = finger.tipPosition().z * -1;
					this->finger_position = ofVec3f(x, y, 0);

					ofVec3f diff;
					for (int i = 0; i < this->spheres.size(); i++) {
						diff = this->finger_position - this->spheres[i]->getPosition();
						if (diff.length() < power) {
							diff *= 5;
							this->spheres[i]->applyCentralForce(-diff);
						}
					}
				}
			}
		}
	}

	this->world.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	
	ofDrawBox(this->finger_position, 5);

	float len = 50;
	for (int i = 0; i < this->spheres.size(); i++) {
		ofSetColor(this->spheres_color[i]);
		this->spheres[i]->draw();
		ofDrawLine(this->spheres[i]->getPosition(), this->spheres_location[i]);
	}
}

//========================================================================
int main() {
	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}