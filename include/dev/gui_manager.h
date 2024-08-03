#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <algorithm>
#include "logger.h"
#include "material.h"
#include "renderobject.h"
#include "profiler.h"
#include "chunk.h"


struct ObjectSelectionOptions
{
    ObjectSelectionOptions() 
    {
        clear();
    };

    void clear()
    {
        m_RotationAxis = glm::vec3(0.0f); 
        m_RotationMagnitude = 0.1f; 
        m_ScaleMagnitude = 1.0f;
    }

    glm::vec3 m_RotationAxis;
    float m_RotationMagnitude;
    float m_ScaleMagnitude;
};


class GUI
{
public:
    GUI();
    ~GUI();
    void RenderGUI();
    void Begin();
    void End();
    void HandleObjectSelection(RenderObject* obj);
    void RegisterLogTarget(Logger* logger);
    


    bool m_bShowProfilerStatistics;
    bool m_bShowChunkManagementOptions;
private:
    /* logging functionality */
    static std::thread m_LogThread;
    static LogTarget m_LogTarget;
    static void LoggingThread();
    static bool m_bRunLogThread;
    void DisplayLogs();

    void DisplayProfilerStatistics();
    void DisplayChunkManagementOptions();
    

    /* object selection functionality */
    ObjectSelectionOptions m_ObjectOptions;
    RenderObject* m_CurrentObject;
    void DisplayObjectOptions();
    void HandleObjectPositionOptions();
};




#endif