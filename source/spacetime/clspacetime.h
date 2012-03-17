
#ifndef _CL_SPACETIME_H_
#define _CL_SPACETIME_H_

#define GRID_DIM  (32)
#define GRID_SIZE (GRID_DIM*GRID_DIM*GRID_DIM)

#if defined(HOST)
#define CL_TYPE(type) cl_##type
#else
#define CL_TYPE(type) type
#endif

typedef struct _cl_spacetime_grid_t {
	CL_TYPE(float)  m_mass[GRID_SIZE];
	CL_TYPE(float)  m_pull[GRID_SIZE];
	CL_TYPE(float4) m_position[GRID_SIZE];
	CL_TYPE(float4) m_budge[GRID_SIZE];
} cl_spacetime_grid_t;

typedef struct _cl_body_t {
	CL_TYPE(float4) m_position;
	CL_TYPE(float4) m_velocity;
	CL_TYPE(float4) m_acceleration;
	CL_TYPE(float)  m_mass;
	CL_TYPE(float)  m_radius;
} cl_body_t;

#endif
