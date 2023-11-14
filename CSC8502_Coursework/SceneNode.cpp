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

Shader* SceneNode::getShader() const{
    return _shader;
}

void SceneNode::setShader(Shader* shader){
    _shader = shader;
    for (SceneNode* childNode : _children)
    {
        childNode->setShader(shader);
    }
}

std::string& SceneNode::getNodeName(){
    return _nodeName;
}

void SceneNode::setNodeName(const std::string& nodeName){
    _nodeName = nodeName;
}

bool SceneNode::getIsFrustrumCheckable() const{
    return _isFrustrumCheckable;
}

void SceneNode::setIsFrustrumCheckable(bool isFrustrumCheckable){
    _isFrustrumCheckable = isFrustrumCheckable;
}

GLuint SceneNode::getTexture() const{
    return _texture;
}

void SceneNode::setTexture(GLuint texture){
    _texture = texture;
}

bool SceneNode::compareByCameraDistance(SceneNode* firstNode, SceneNode* secondNode){
    if (!firstNode->getIsFrustrumCheckable())
        return true;
    else if (!secondNode->getIsFrustrumCheckable())
        return false;
    return firstNode->_distanceFromCamera < secondNode->_distanceFromCamera ? true : false;
}

void SceneNode::addChild(SceneNode* child) {
    _children.push_back(child);
    child->_parent = this;
}

void SceneNode::removeChild(SceneNode* child){
    for (int i = 0; i < _children.size(); i++){
        if (_children[i] == child)
            _children.erase(_children.begin() + i);
    }
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

void SceneNode::draw(OGLRenderer& renderer, bool isDrawingForShadows) {
    if (isDrawingForShadows) {
        _mesh->Draw();
        return;
    }
    if (_shader) {
        setUpShader(renderer);
    }

    if (_mesh)
        _mesh->Draw();

    postDraw();
}

std::vector<SceneNode*>::const_iterator SceneNode::getChildIteratorStart()
{
    return _children.begin();
}

std::vector<SceneNode*>::const_iterator SceneNode::getChildIteratorEnd()
{
    return _children.end();
}

void SceneNode::setUpShader(OGLRenderer& renderer){
    renderer.BindShader(_shader);
    renderer.UpdateShaderMatrices();
    Matrix4 model = getWorldTransform() * Matrix4::Scale(getModelScale());
    glUniformMatrix4fv(glGetUniformLocation(_shader->GetProgram(), "modelMatrix"), 1, false, model.values);
    glUniform4fv(glGetUniformLocation(_shader->GetProgram(), "nodeColour"), 1, (float*)&getColour());

    _texture = getTexture();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glUniform1i(glGetUniformLocation(_shader->GetProgram(), "useTexture"), _texture);

    glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);

}

void SceneNode::postDraw()
{
}
