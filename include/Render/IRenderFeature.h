#ifndef IRENDERFEATURE_H
#define IRENDERFEATURE_H

class CommandList;
class RenderContext;

class IRenderFeature {
public:
    virtual ~IRenderFeature() = 0;

    virtual void OnExecute(CommandList &cmd, RenderContext &renderContext) = 0;
};

#endif
