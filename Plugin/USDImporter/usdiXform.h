#pragma once

namespace usdi {

class XformSample
{
public:
};


class Xform : public Schema
{
typedef Schema super;
public:
    Xform(Schema *parent, const UsdGeomXformable& xf);
    ~Xform() override;

    UsdGeomXformable&   getUSDType() override;

    void                readSample(XformData& dst, Time t);
    void                writeSample(const XformData& src, Time t);

private:
    typedef std::vector<UsdGeomXformOp> UsdGeomXformOps;

    UsdGeomXformable    m_xf;
    UsdGeomXformOps     m_read_ops;
    UsdGeomXformOps     m_write_ops;
};

} // namespace usdi
