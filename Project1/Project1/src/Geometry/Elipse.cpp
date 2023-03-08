#include "Elipse.h"
#include "imgui/imgui.h"
#include <math.h>

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
    temporaryTransformation.location = { {0,0,-1000} };
    applyTemporaryTransformation();
    setRadious(100, 100, 200);
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

RGB Elipse::CalculatePixelColor(float x, float y) const
{
    const auto [onElipse, z1, z2] = calculateZ(x, y);

    float z = fmax(z1, z2);
    if (onElipse && z>0)
    {

        CadMath::Vector4 n = calculateNormal(x, y, z);
        CadMath::Vector4 v = CadMath::vectorTo({ {x,y,z,1} }, { {0,0,0,1} });
        v = v / v.getLength();
        CadMath::Vector4 color = v.getColor();
        float intensivity = pow(n * v, m);
        if (intensivity > 0)
            //return { (unsigned char)color.X(),(unsigned char)color.Y(),(unsigned char)color.Z() };
            return {
                (unsigned char)(intensivity * 255.0f),
                (unsigned char)0,
                (unsigned char)0 };
        else
            return {
                (unsigned char)0,
                (unsigned char)0,
                (unsigned char)0 };
    }
    else
        return { 100, 100, 100 };
}

void Elipse::updateTransforamtions()
{
    tD = (temporaryTransformation+transform).GetMatrix().transpose() * D * (temporaryTransformation + transform).GetMatrix();
}

void Elipse::applyTemporaryTransformation()
{
    transform = transform + temporaryTransformation;
    //M = temporaryTransformation.GetMatrix()*M;
    temporaryTransformation = Transform();
    updateTransforamtions();
}

void Elipse::Update(Window& window)
{
    if (window.curentMouseVectorX != 0 || window.curentMouseVectorY != 0)
    {
        CadMath::Vector4 mouseVector = { {(float)window.curentMouseVectorX,(float)window.curentMouseVectorY ,0,0} };
        if (window.shouldApply)
        {
            applyTemporaryTransformation();
            window.shouldApply = false;
        }
        if (window.isCrtlPressed)
        {
            float len = sqrt(window.curentMouseVectorX * window.curentMouseVectorX + window.curentMouseVectorY * window.curentMouseVectorY);
            temporaryTransformation.scale = { {100 / (1 + len),100 / (1 + len),100 / (1 + len),0} };
        }
        else if (window.isShiftPressed)
            temporaryTransformation.location = CadMath::Vector4({ (float)-window.curentMouseVectorX, (float)window.curentMouseVectorY, 0, 0 });
        else
            temporaryTransformation.rotation = CadMath::Vector4( {-(float)window.curentMouseVectorY / 1000, (float)window.curentMouseVectorX / 1000, 0, 0});

        updateTransforamtions();
    }
}
