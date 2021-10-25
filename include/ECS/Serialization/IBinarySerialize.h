#ifndef IBINARYSERIALIZE_H
#define IBINARYSERIALIZE_H

#include <fstream>

class IBinarySerialize {
private:
    bool isNeeded;

protected:
    explicit IBinarySerialize(bool binaryNeeded=false) noexcept : isNeeded(binaryNeeded) {}
    IBinarySerialize(IBinarySerialize &&asset) noexcept : isNeeded(asset.isNeeded) {}

public:
    [[nodiscard]] bool IsBinaryNeeded() const noexcept { return isNeeded; }

    virtual void SerializeBin(std::ofstream &file) const {}
    virtual void UnSerializeBin(std::ifstream &file) {}
};

#endif
