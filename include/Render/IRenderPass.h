#ifndef IRENDERPASS_H
#define IRENDERPASS_H

#include <vector>
#include <memory>

class IRenderTask;
class RenderContext;

class IRenderPass {
public:
    virtual ~IRenderPass() = 0;

    virtual void DrawObjects(std::vector<std::unique_ptr<IRenderTask>> tasks, RenderContext &renderContext) = 0;
};

#endif
