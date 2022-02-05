#ifndef IRENDERPASS_H
#define IRENDERPASS_H

#include <vector>
#include <memory>

class RenderTask;
class RenderContext;

class IRenderPass {
public:
    virtual ~IRenderPass() = 0;

    virtual void DrawObjects(std::vector<std::unique_ptr<RenderTask>> tasks, RenderContext &renderContext) = 0;
};

#endif
