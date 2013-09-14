
#ifndef _CL_SPACETIME_H_
#define _CL_SPACETIME_H_

#define GRID_DIM  (32)
#define GRID_SIZE (GRID_DIM*GRID_DIM*GRID_DIM)

#if defined(ARCH_64)
//#pragma OPENCL EXTENSION cl_khr_fp64: enable
#endif

#if defined(GPGPU_NO_DOUBLE_SUPPORT)
#if defined(HOST)
#define precision   cl_float
#define precision4  cl_float4
#else
#define precision   float
#define precision4  float4
#endif
#else
#if defined(HOST)
#define precision   cl_double
#define precision   cl_double4
#else
#define precision   double
#define precision4  double4
#endif
#endif

typedef struct _cl_spacetime_grid_t {
    precision  m_mass[GRID_SIZE];
    precision  m_pull[GRID_SIZE];
    precision4 m_position[GRID_SIZE];
    precision4 m_budge[GRID_SIZE];
} cl_spacetime_grid_t;

typedef struct _cl_body_t {
    precision4 m_position;
    precision4 m_velocity;
    precision4 m_acceleration;
    precision  m_mass;
    precision  m_radius;
} cl_body_t;

#endif
