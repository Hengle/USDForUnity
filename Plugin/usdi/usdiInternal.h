#pragma once

#define usdiImpl

#define usdiLogError(...) usdi::LogImpl(1, __VA_ARGS__)
#define usdiLogWarning(...) usdi::LogImpl(2, __VA_ARGS__)
#define usdiLogInfo(...) usdi::LogImpl(3, __VA_ARGS__)
#ifdef usdiDebug
    #define usdiLogTrace(...) usdi::LogImpl(4, __VA_ARGS__)
    #define usdiTraceFunc(...) usdi::TraceFuncImpl _trace_(__FUNCTION__)
#else
    #define usdiLogTrace(...)
    #define usdiTraceFunc(...)
#endif

#include "usdi.h"

namespace usdi {

void LogImpl(int level, const char *format, ...);
struct TraceFuncImpl
{
    const char *m_func;
    TraceFuncImpl(const char *func);
    ~TraceFuncImpl();
};

extern const float Rad2Deg;
extern const float Deg2Rad;

float2 operator*(const float2& l, float r);
float3 operator*(const float3& l, float r);
float4 operator*(const float4& l, float r);
quaternion operator*(const quaternion& l, float r);
quaternion operator*(const quaternion& l, const quaternion& r);
float2& operator*=(float2& l, float r);
float3& operator*=(float3& l, float r);
float4& operator*=(float4& l, float r);
quaternion& operator*=(quaternion& l, float r);

} // namespace usdi
