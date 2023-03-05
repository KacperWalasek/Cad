#include "Elipse.h"
#include "imgui/imgui.h"
std::tuple<bool,float, float> Elipse::calculateZ(float x, float y) const
{
    float pa = tD[2][2];
    float pb = x * (tD[2][0] + tD[0][2]) + y * (tD[2][1] + tD[1][2]) + tD[2][3] + tD[3][2];
    float pc =  x * (tD[0][0] * x + tD[0][1] * y + tD[0][3]) +
                y * (tD[1][0] * x + tD[1][1] * y + tD[1][3]) +
                     tD[3][0] * x + tD[3][1] * y + tD[3][3];

    float delta = pb * pb - 4 * pa * pc;
    if (delta < 0)
        return { false,0,0 };
    
    float sqrtDelta = sqrt(delta);
    return { true, (-pb - sqrtDelta) / (2 * pa),
                   (-pb + sqrtDelta) / (2 * pa) };
}

CadMath::Vector4 Elipse::calculateNormal(float x, float y, float z) const
{
    CadMath::Vector4 position = { {x,y,z,1.0f} };
    CadMath::Vector4 grad = 2 * (tD * position);
    
    return grad/grad.getLength();
}

Elipse::Elipse()
    : m(2)
{
    temporaryTransformation.location = { {0,0,40} };
    applyTemporaryTransformation();
}

Elipse::~Elipse()
{
    delete image;
}

void Elipse::setRadious(float x, float y, float z)
{
    rX = x;
    rY = y; 
    rZ = z;
	a = 1/(x*x);
	b = 1/(y*y);
	c = 1/(z*z);
    D = { {{a,0,0,0},{0,b,0,0},{0,0,c,0},{0,0,0,-1}} };
    updateTransforamtions();
}

RGB* Elipse::createTexture(int sizeX, int sizeY)
{
    RGB* tmpImage = new RGB[sizeX * sizeY];

    for (int i = 0; i < sizeY; i++) {
        for (int j = 0; j < sizeX; j++) {
            float x = (j  - 0.5*(float)sizeX) * TexSizeX/sizeX;
            float y = (i  - 0.5* (float)sizeY)* TexSizeX / sizeX;
            const auto [onElipse, z1, z2] = calculateZ(x, y);

            if (onElipse)
            {
                float z = z2;
                
                CadMath::Vector4 n = calculateNormal(x, y, z);
                CadMath::Vector4 v = CadMath::vectorTo({ {x,y,z} }, { {0,0,0,1} });
                v = v / v.getLength();
                CadMath::Vector4 color = n.getColor();
                float intensivity = pow(n * v,m);
                if(intensivity>0)
                    tmpImage[i * sizeX + j] = {
                        (unsigned char)(intensivity*255.0f),
                        (unsigned char)0,
                        (unsigned char)0 };
                else
                    tmpImage[i * sizeX + j] = {
                        (unsigned char)0,
                        (unsigned char)0,
                        (unsigned char)0 };
            }
            else
                tmpImage[i* sizeX + j] = {100, 100, 100};

        }
    }
    currentSizeX = sizeX;
    currentSizeY = sizeY;
	return tmpImage;
}

void Elipse::updateTransforamtions()
{
    tD = (temporaryTransformation.GetMatrix()*M).transpose() * D * temporaryTransformation.GetMatrix() * M;
}

void Elipse::applyTemporaryTransformation()
{
    M = temporaryTransformation.GetMatrix()*M;
    temporaryTransformation = Transform();
    updateTransforamtions();
}

void Elipse::renderUI()
{
    ImGui::Begin("Elipse");

    float changedRX = rX, changedRY = rY, changedRZ = rZ;
    if (ImGui::InputFloat("RadiusX", &changedRX))
        setRadious(changedRX, rY, rZ);
    if (ImGui::InputFloat("RadiusY", &changedRY))
        setRadious(rX, changedRY, rZ);
    if (ImGui::InputFloat("RadiusZ", &changedRZ))
        setRadious(rX, rY, changedRZ);

    float changedM = m;
    if (ImGui::InputFloat("Specular m", &changedM))
        m = changedM;

    ImGui::End();
}
