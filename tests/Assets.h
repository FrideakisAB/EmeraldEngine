#include "Assets/IAsset.h"

TEST_CASE( "Assets system test", "[Assets]")
{
    SECTION("IAsset test")
    {
        class TestAsset final : public IAsset {
        public:
            [[nodiscard]] size_t GetTypeId() const noexcept final { return 0; }
        };

        IAsset *asset = new TestAsset();

        REQUIRE( asset->GetTypeId() == 0 );
        REQUIRE( asset->IsBinaryNeeded() == false );
        REQUIRE( asset->GetName().empty() == true );
        REQUIRE( asset->IsDynamic() == true );
        REQUIRE( asset->Clone() == nullptr );

        delete asset;
    }
}
