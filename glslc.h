#pragma once

#include "nvnTool_GlslcInterface.h"
extern "C" {
GLSLCoptions glslcGetDefaultOptions();
void glslcSetAllocator(GLSLCallocateFunction, GLSLCfreeFunction, GLSLCreallocateFunction, void*);
uint8_t glslcInitialize(GLSLCcompileObject*);
void glslcFinalize(GLSLCcompileObject*);
uint8_t glslcCompile(GLSLCcompileObject*);
const GLSLCoutput* const* glslcCompileSpecialized(GLSLCcompileObject*, const GLSLCspecializationBatch*);

void* glslc_Alloc(size_t size, size_t alignment, void* user_data = nullptr);
void glslc_Free(void* ptr, void* user_data);
void* glslc_Realloc(void* ptr, size_t size, void* user_data);
}