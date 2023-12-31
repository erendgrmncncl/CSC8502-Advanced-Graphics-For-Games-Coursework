#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
    _mesh = mesh;
    _colour = colour;
    _parent = nullptr;
    _modelScale = Vector3(1, 1, 1);
    _texture = 0;
}

SceneNode::~SceneNode() {
    for (int i = 0; i < _children.size(); i++) {
        delete _children[i];
    }
}

void SceneNode::setTransform(const Matrix4& matrix) {
    _transform = matrix;
}

const Matrix4& SceneNode::getTransform() const {
    return _transform;
}

Matrix4 SceneNode::getWorldTransform() const {
    return _worldTransform;
}

Vector4 SceneNode::getColour() const {
    return _colour;
}

void SceneNode::setColour(Vector4 colour) {
    _colour = colour;
}

Vector3 SceneNode::getModelScale() const {
    return _modelScale;
}

void SceneNode::setModelScale(Vector3 modelScale) {
    _modelScale = modelScale;
}

void SceneNode::setScale(float scale) {
    Vector3 newScale = Vector3(_modelScale.x * scale, _modelScale.y * scale, _modelScale.z * scale);
    setModelScale(newScale);
    for (SceneNode* child : _children) {
        child->setScale(scale);
    }
}

Mesh* SceneNode::getMesh() const {
    return _mesh;
}

void SceneNode::setMesh(Mesh* mesh) {
    _mesh = mesh;
}

float SceneNode::getBoundingRadius() const{
    return _boundingRadius;
}

void SceneNode::setBoundingRadius(float boundingRadius){
    _boundingRadius = boundingRadius;
}

float SceneNode::getCameraDistance() const{
    return _distanceFromCamera;
}

void SceneNode::setCameraDistance(float distanceFromCamera){
    _distanceFromCamera = distanceFromCamera;
}

GLuint SceneNode::getTexture() const{
    return _texture;
}

void SceneNode::setTexture(GLuint texture){
    _texture = texture;
}

bool SceneNode::compareByCameraDistance(SceneNode* firstNode, SceneNode* secondNode){
    return firstNode->_distanceFromCamera < secondNode->_distanceFromCamera ? true : false;
}

void SceneNode::addChild(SceneNode* child) {
    _children.push_back(child);
    child->_parent = this;
}

void SceneNode::update(float dt) {
    if (_parent)
        _worldTransform = _parent->_worldTransform * _transform;
    else
        _worldTransform = _transform;
    for (vector<SceneNode*>::iterator i = _children.begin(); i != _children.end(); ++i) {
        (*i)->update(dt);
    }
}

void SceneNode::draw(const OGLRenderer& renderer) {
    if (_mesh)
        _mesh->Draw();
}

std::vector<SceneNode*>::const_iterator SceneNode::getChildIteratorStart()
{
    return _children.begin();
}

std::vector<SceneNode*>::const_iterator SceneNode::getChildIteratorEnd()
{

    return _children.end();
}
