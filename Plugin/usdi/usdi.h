#pragma once

#define usdiCLinkage extern "C"
#ifdef _WIN32
    #ifndef usdiStaticLink
        #ifdef usdiImpl
            #define usdiExport __declspec(dllexport)
        #else
            #define usdiExport __declspec(dllimport)
        #endif
    #else
        #define usdiExport
    #endif
#else
    #define usdiExport
#endif

namespace usdi {

#ifdef usdiImpl
    class Context;
    class Attribute;
    class Schema;
    class Xform;
    class Camera;
    class Mesh;
    class Points;
#else
    // force make convertible
    class Context {};
    class Attribute {};
    class Schema {};
    class Xform : public Schema  {};
    class Camera : public Xform {};
    class Mesh : public Xform {};
    class Points : public Xform {};
#endif


typedef unsigned char byte;
typedef unsigned int uint;
struct float2 { float x, y; };
struct float3 { float x, y, z; };
struct float4 { float x, y, z, w; };
struct quaternion { float x, y, z, w; };
struct float3x3 { float3 v[3]; };
struct float4x4 { float4 v[4]; };

enum class InterpolationType
{
    None,
    Linear,
};

enum class AttributeType
{
    Unknown,
    Byte,
    Int,
    UInt,
    Float,
    Float2,
    Float3,
    Float4,
    Quaternion,
    Token,
    String,
    UnknownArray = 0x100,
    ByteArray,
    IntArray,
    UIntArray,
    FloatArray,
    Float2Array,
    Float3Array,
    Float4Array,
    QuaternionArray,
    TokenArray,
    StringArray,
};

enum class TopologyVariance
{
    Constant, // both vertices and topologies are constant
    Homogenous, // vertices are not constant (= animated). topologies are constant.
    Heterogenous, // both vertices and topologies are not constant
};

typedef double Time;

struct ImportConfig
{
    InterpolationType interpolation = InterpolationType::Linear;
    float scale = 1.0f;
    bool triangulate = true;
    bool swap_handedness = true;
    bool swap_faces = true;
};

struct ExportConfig
{
    float scale = 1.0f;
    bool swap_handedness = true;
    bool swap_faces = true;
};

struct XformData
{
    float3 position = { 0.0f, 0.0f, 0.0f};
    quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    float3 scale = { 1.0f, 1.0f, 1.0f };
};

struct CameraData
{
    float near_clipping_plane = 0.3f;
    float far_clipping_plane = 1000.0f;
    float field_of_view = 60.0f;        // in degree. vertical one
    float aspect_ratio = 16.0f / 9.0f;

    float focus_distance = 5.0f;        // in cm
    float focal_length = 0.0f;          // in mm
    float aperture = 35.0f;             // in mm. vertical one
};


struct MeshSummary
{
    uint                peak_num_points = 0;
    uint                peak_num_counts = 0;
    uint                peak_num_indices = 0;
    uint                peak_num_indices_triangulated = 0;
    TopologyVariance    topology_variance = TopologyVariance::Constant;
    bool                has_normals = false;
    bool                has_uvs = false;
    bool                has_velocities = false;
};

struct MeshData
{
    // these pointers can be null (in this case, just be ignored).
    // otherwise, if you pass to usdiMeshSampleReadData(), pointers must point valid memory block to store data.
    float3  *points = nullptr;
    float3  *velocities = nullptr;
    float3  *normals = nullptr;
    float2  *uvs = nullptr;
    int     *counts = nullptr;
    int     *indices = nullptr;
    int     *indices_triangulated = nullptr;

    uint    num_points = 0;
    uint    num_counts = 0;
    uint    num_indices = 0;
    uint    num_indices_triangulated = 0;
};


struct PointsSummary
{
    uint                peak_num_points = 0;
    bool                has_velocities = false;
};

struct PointsData
{
    // these pointers can be null (in this case, just be ignored).
    // otherwise, if you pass to usdiMeshSampleReadData(), pointers must point valid memory block to store data.
    float3  *points = nullptr;
    float3  *velocities = nullptr;

    uint    num_points = 0;
};

} // namespace usdi

extern "C" {

usdiExport void             usdiSetDebugLevel(int l);

// Context interface
usdiExport usdi::Context*   usdiCreateContext();
usdiExport void             usdiDestroyContext(usdi::Context *ctx);
usdiExport bool             usdiOpen(usdi::Context *ctx, const char *path);
usdiExport void             usdiCreateStage(usdi::Context *ctx, const char *identifier);
usdiExport bool             usdiWrite(usdi::Context *ctx, const char *path);
usdiExport void             usdiSetImportConfig(usdi::Context *ctx, const usdi::ImportConfig *conf);
usdiExport void             usdiGetImportConfig(usdi::Context *ctx, usdi::ImportConfig *conf);
usdiExport void             usdiSetExportConfig(usdi::Context *ctx, const usdi::ExportConfig *conf);
usdiExport void             usdiGetExportConfig(usdi::Context *ctx, usdi::ExportConfig *conf);
usdiExport usdi::Schema*    usdiGetRoot(usdi::Context *ctx);

// Schema interface
usdiExport int              usdiGetID(usdi::Schema *schema);
usdiExport const char*      usdiGetPath(usdi::Schema *schema);
usdiExport const char*      usdiGetName(usdi::Schema *schema);
usdiExport const char*      usdiGetTypeName(usdi::Schema *schema);
usdiExport usdi::Schema*    usdiGetParent(usdi::Schema *schema);
usdiExport int              usdiGetNumChildren(usdi::Schema *schema);
usdiExport usdi::Schema*    usdiGetChild(usdi::Schema *schema, int i);
usdiExport int              usdiGetNumAttributes(usdi::Schema *schema);
usdiExport usdi::Attribute* usdiGetAttribute(usdi::Schema *schema, int i);
usdiExport usdi::Attribute* usdiFindAttribute(usdi::Schema *schema, const char *name);
usdiExport usdi::Attribute* usdiCreateAttribute(usdi::Schema *schema, const char *name, usdi::AttributeType type);

// Xform interface
usdiExport usdi::Xform*     usdiAsXform(usdi::Schema *schema); // dynamic cast to Xform
usdiExport usdi::Xform*     usdiCreateXform(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiExport bool             usdiXformReadSample(usdi::Xform *xf, usdi::XformData *dst, usdi::Time t);
usdiExport bool             usdiXformWriteSample(usdi::Xform *xf, const usdi::XformData *src, usdi::Time t);

// Camera interface
usdiExport usdi::Camera*    usdiAsCamera(usdi::Schema *schema); // dynamic cast to Camera
usdiExport usdi::Camera*    usdiCreateCamera(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiExport bool             usdiCameraReadSample(usdi::Camera *cam, usdi::CameraData *dst, usdi::Time t);
usdiExport bool             usdiCameraWriteSample(usdi::Camera *cam, const usdi::CameraData *src, usdi::Time t);

// Mesh interface
usdiExport usdi::Mesh*      usdiAsMesh(usdi::Schema *schema); // dynamic cast to Mesh
usdiExport usdi::Mesh*      usdiCreateMesh(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiExport void             usdiMeshGetSummary(usdi::Mesh *mesh, usdi::MeshSummary *dst);
usdiExport bool             usdiMeshReadSample(usdi::Mesh *mesh, usdi::MeshData *dst, usdi::Time t);
usdiExport bool             usdiMeshWriteSample(usdi::Mesh *mesh, const usdi::MeshData *src, usdi::Time t);

// Points interface
usdiExport usdi::Points*    usdiAsPoints(usdi::Schema *schema); // dynamic cast to Points
usdiExport usdi::Points*    usdiCreatePoints(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiExport void             usdiPointsGetSummary(usdi::Points *points, usdi::PointsSummary *dst);
usdiExport bool             usdiPointsReadSample(usdi::Points *points, usdi::PointsData *dst, usdi::Time t);
usdiExport bool             usdiPointsWriteSample(usdi::Points *points, const usdi::PointsData *src, usdi::Time t);

// Attribute interface
usdiExport const char*          usdiAttrGetName(usdi::Attribute *attr);
usdiExport const char*          usdiAttrGetTypeName(usdi::Attribute *attr);
usdiExport usdi::AttributeType  usdiAttrGetType(usdi::Attribute *attr);
usdiExport int                  usdiAttrGetArraySize(usdi::Attribute *attr, usdi::Time t); // always 1 if attr is scalar
usdiExport int                  usdiAttrGetNumSamples(usdi::Attribute *attr);
usdiExport bool                 usdiAttrReadSample(usdi::Attribute *attr, void *dst, usdi::Time t);
usdiExport bool                 usdiAttrReadArraySample(usdi::Attribute *attr, void *dst, int size, usdi::Time t);
usdiExport bool                 usdiAttrWriteSample(usdi::Attribute *attr, const void *src, usdi::Time t);
usdiExport bool                 usdiAttrWriteArraySample(usdi::Attribute *attr, const void *src, int size, usdi::Time t);

} // extern "C"

