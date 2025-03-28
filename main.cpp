#include "glslc.h"
#include "nvn.h"
#include "nvnTool_GlslcInterface.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

#include "cpp-httplib/httplib.h"

static std::string readFileString(const char* filename)
{
    if (!std::filesystem::exists(filename)) {
        fprintf(stderr, "%s does not exist\n", filename);
        exit(1);
    }

    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

static std::mutex sMutex;

static void compile(const httplib::Request& req, httplib::Response& res, NVNshaderStage stage, bool isControl)
{
    std::scoped_lock lock(sMutex);

    GLSLCcompileObject initInfo;
    initInfo.options = glslcGetDefaultOptions();

    int rc = glslcInitialize(&initInfo);
    if (!rc) {
        res.set_content("glslcInitialize failed", "text/plain");
        res.status = 500;
        return;
    }

    const char* shaders[] { req.body.c_str() };
    NVNshaderStage stages[] { stage };

    initInfo.input.sources = shaders;
    initInfo.input.stages = stages;
    initInfo.input.count = 1;

    uint8_t result = glslcCompile(&initInfo);

    if (result == 0) {
        if (initInfo.lastCompiledResults->compilationStatus->infoLog)
            res.set_content(initInfo.lastCompiledResults->compilationStatus->infoLog, "text/plain");
        printf("Fail: %s\n", initInfo.lastCompiledResults->compilationStatus->infoLog);
        res.status = 400;
        return;
    }

    auto* output = initInfo.lastCompiledResults->glslcOutput;
    uintptr_t binary = uintptr_t(output);
    if (output->numSections != 1) {
        res.set_content("Wrong amount of sections output from glslc", "text/plain");
        res.status = 500;
        return;
    }

    auto& header = output->headers[0];

    void* data = (void*)(binary + (header.gpuCodeHeader.common.dataOffset + (isControl ? header.gpuCodeHeader.controlOffset : header.gpuCodeHeader.dataOffset)));

    res.status = 200;
    res.set_content((const char*)data, isControl ? header.gpuCodeHeader.controlSize : header.gpuCodeHeader.dataSize, "application/octet-stream");
}

constexpr static const char cHelpString[] =
    R"(
POST with source to the following endpoints:
/compileVert/control
/compileVert/code
/compileFrag/control
/compileFrag/code
/compileGeom/control
/compileGeom/code
/compileTessControl/control
/compileTessControl/code
/compileTessEval/control
/compileTessEval/code
/compileCompute/control
/compileCompute/code
)";

int main()
{
    glslcSetAllocator(
        [](size_t size, size_t alignment, void*) -> void* { return aligned_alloc(alignment, size); },
        [](void* ptr, void*) -> void { free(ptr); },
        [](void* ptr, size_t size, void*) -> void* { return realloc(ptr, size); },
        nullptr);

    httplib::Server server;

    server.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(cHelpString, "text/plain");
    });

    server.Post("/compileVert/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_VERTEX, true);
    });
    server.Post("/compileVert/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_VERTEX, false);
    });
    server.Post("/compileFrag/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_FRAGMENT, true);
    });
    server.Post("/compileFrag/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_FRAGMENT, false);
    });
    server.Post("/compileGeom/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_GEOMETRY, true);
    });
    server.Post("/compileGeom/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_GEOMETRY, false);
    });
    server.Post("/compileTessControl/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_TESS_CONTROL, true);
    });
    server.Post("/compileTessControl/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_TESS_CONTROL, false);
    });
    server.Post("/compileTessEval/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_TESS_EVALUATION, true);
    });
    server.Post("/compileTessEval/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_TESS_EVALUATION, false);
    });
    server.Post("/compileCompute/control", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_COMPUTE, true);
    });
    server.Post("/compileCompute/code", [](const httplib::Request& req, httplib::Response& res) {
        compile(req, res, NVN_SHADER_STAGE_COMPUTE, false);
    });

    server.listen("0.0.0.0", 1337);

    return 0;
}
