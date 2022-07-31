// renderer.h
#pragma once

class IRenderer{
    public:
    virtual ~IRenderer(){}
    virtual bool LoadScene(const char* filename) = 0;
    virtual void SetViewportSize(int w, int h) = 0;
    virtual void SetCameraPosition(double x, double y, double z) = 0;
    virtual void SetLookAt(double x, double y, double z) = 0;
    virtual void Render() = 0;
};