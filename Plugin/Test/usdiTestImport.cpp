
#include <cstdio>
#include <vector>
#include "../usdi/usdi.h"

typedef unsigned char byte;
typedef unsigned int uint;
using usdi::float2;
using usdi::float3;
using usdi::float4;
using usdi::quaternion;

template<class T> void P(T v);
template<> void P<byte>(byte v) { printf("0x%02X", v); }
template<> void P<int>(int v) { printf("%d", v); }
template<> void P<uint>(uint v) { printf("%u", v); }
template<> void P<float>(float v) { printf("%.3f", v); }
template<> void P<float2>(float2 v) { printf("(%.3f, %.3f)", v.x, v.y); }
template<> void P<float3>(float3 v) { printf("(%.3f, %.3f, %.3f)", v.x, v.y, v.z); }
template<> void P<float4>(float4 v) { printf("(%.3f, %.3f, %.3f, %.3f)", v.x, v.y, v.z, v.w); }
template<> void P<quaternion>(quaternion v) { printf("(%.3f, %.3f, %.3f, %.3f)", v.x, v.y, v.z, v.w); }
template<> void P<const char*>(const char* v) { printf("\"%s\"", v); }


template<class T>
void P(const std::vector<T>& data)
{
    printf("[");
    auto n = data.size();
    for (size_t i = 0; i < n; ++i) {
        if (i > 0) { printf(", "); }
        P<T>(data[i]);
    }
    printf("]");
}

void InspectAttribute(usdi::Attribute *attr)
{

    usdi::Time t = 0.0;

#define ImplScalar(Enum, Type)\
    case usdi::AttributeType::Enum:\
    {\
        Type buf;\
        usdiAttrReadSample(attr, &buf, t);\
        P(buf);\
        break;\
    }

#define ImplVector(Enum, Type)\
    case usdi::AttributeType::Enum##Array:\
    {\
        std::vector<Type> buf;\
        buf.resize(usdiAttrGetArraySize(attr, t));\
        usdiAttrReadArraySample(attr, &buf[0], buf.size(), t);\
        P(buf);\
        break;\
    }


    printf("    %s (%s): ", usdiAttrGetName(attr), usdiAttrGetTypeName(attr));
    switch (usdiAttrGetType(attr)) {
        ImplScalar(Byte, byte);
        ImplScalar(Int, int);
        ImplScalar(UInt, uint);
        ImplScalar(Float, float);
        ImplScalar(Float2, float2);
        ImplScalar(Float3, float3);
        ImplScalar(Float4, float4);
        ImplScalar(Quaternion, quaternion);
        ImplScalar(Token, const char*);
        ImplScalar(String, const char*);

        ImplVector(Byte, byte);
        ImplVector(Int, int);
        ImplVector(UInt, uint);
        ImplVector(Float, float);
        ImplVector(Float2, float2);
        ImplVector(Float3, float3);
        ImplVector(Float4, float4);
        ImplVector(Quaternion, quaternion);
        ImplVector(Token, const char*);
        ImplVector(String, const char*);
    }

#undef ImplScalar
#undef ImplVector

    printf("\n");
}

void InspectRecursive(usdi::Schema *schema)
{
    if (!schema) { return; }

    printf("  %s (%s)\n", usdiGetPath(schema), usdiGetTypeName(schema));
    {
        int nattr = usdiGetNumAttributes(schema);
        for (int i = 0; i < nattr; ++i) {
            InspectAttribute(usdiGetAttribute(schema, i));
        }
    }

    int num_children = usdiGetNumChildren(schema);
    for (int i = 0; i < num_children; ++i) {
        auto child = usdiGetChild(schema, i);
        InspectRecursive(child);
    }
}

bool TestImport(const char *path)
{
    if (!path) { return false; }

    auto *ctx = usdiCreateContext();
    if (!usdiOpen(ctx, path)) {
        printf("failed to load %s\n", path);
    }
    else {
        InspectRecursive(usdiGetRoot(ctx));
    }
    usdiDestroyContext(ctx);

    return true;
}


int main(int argc, char *argv[])
{
    const char *path = nullptr;
    int int_value;
    for (int i = 1; i < argc; ++i) {
        auto *arg = argv[i];
        if (sscanf(arg, "debug_level=%d", &int_value)) {
            usdiSetDebugLevel(int_value);
        }
        else {
            path = arg;
        }
    }

    TestImport(path);
}
