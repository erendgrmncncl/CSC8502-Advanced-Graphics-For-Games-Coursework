#include "HeightMap.h"

HeightMap::HeightMap(const std::string& name) {
    int iWidth, iHeight, iChans;
    unsigned char* data = SOIL_load_image(name.c_str(),
        &iWidth, &iHeight, &iChans, 1);
    if (!data) {
        std::cout << "Heightmap can't load file !" << std::endl;
        return;
    }

    numVertices = iWidth * iHeight;
    numIndices = (iWidth - 1)* (iHeight - 1) * 6;
    vertices = new Vector3[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector3 vertexScale = Vector3(16.f, .3f, 16.f);
    Vector2 textureScale = Vector2(1 / 16.f, 1 / 16.f);

    for (int i = 0; i < iHeight; ++i){
        for (int j = 0; j < iWidth; ++j){
            int offset = (i * iWidth) + j;
            vertices[offset] = Vector3(j, data[offset], i) * vertexScale;
            textureCoords[offset] = Vector2(i, j) * textureScale;
        }
    }
    SOIL_free_image_data(data);

    int i = 0;
    for (int z = 0; z < iHeight - 1; z++){
        for (int x = 0; x < iWidth - 1; x++) {
            int a = (z * (iWidth)) + x;
            int b = (z * (iWidth)) + (x + 1);
            int c = ((z + 1) * (iWidth)) + (x + 1);
            int d = ((z + 1) * (iWidth)) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;
            
            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }

    GenerateNormals();
    GenerateTangents();
    BufferData();

    _heightMapSize.x = vertexScale.x * (iWidth - 1);
    _heightMapSize.y = vertexScale.y * 255.f;
    _heightMapSize.z = vertexScale.z * (iHeight - 1);
}

HeightMap::~HeightMap(){

}

Vector3 HeightMap::getHeightmapSize() const
{
    return _heightMapSize;
}
