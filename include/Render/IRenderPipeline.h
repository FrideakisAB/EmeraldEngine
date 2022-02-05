#ifndef IRENDERPIPELINE_H
#define IRENDERPIPELINE_H

class FrameData;
class RenderContext;

class IRenderPipeline {
public:
    virtual ~IRenderPipeline() = 0;

    virtual void Draw(FrameData frameData, RenderContext &renderContext) = 0;
};

#endif
